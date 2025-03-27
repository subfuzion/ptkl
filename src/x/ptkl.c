/*
 * ptkl
 *
 * Copyright (c) 2017-2024 Fabrice Bellard
 * Copyright (c) 2017-2024 Charlie Gordon
 * Copyright (c) 2025 Tony Pujals
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#if defined(__APPLE__)
#include <malloc/malloc.h>
#elif defined(__linux__) || defined(__GLIBC__)
#include <malloc.h>
#elif defined(__FreeBSD__)
#include <malloc_np.h>
#endif

#include "cutils.h"
#include "ptklcli.h"
#include "quickjs-libc.h"

////////////////////////////////////////////////////////////////////////////////

extern const uint8_t qjsc_repl[];
extern const uint32_t qjsc_repl_size;

extern int compile (int argc, char **argv);

// Automatically compiled with ecmascript standard bigint support, so this
// just disables non-standard bigdecimal, bigfloat, and related extensions
static int bignum_ext = 0;

////////////////////////////////////////////////////////////////////////////////

static int eval_buf (JSContext *ctx, const void *buf, const size_t buf_len,
		     const char *filename, const int eval_flags)
{
	JSValue val;
	int ret;

	if ((eval_flags & JS_EVAL_TYPE_MASK) == JS_EVAL_TYPE_MODULE) {
		// For modules, compile then run to be able to set import.meta
		val = JS_Eval (ctx, buf, buf_len, filename,
			       eval_flags | JS_EVAL_FLAG_COMPILE_ONLY);
		if (!JS_IsException (val)) {
			js_module_set_import_meta (ctx, val, TRUE, TRUE);
			val = JS_EvalFunction (ctx, val);
		}
		val = js_std_await (ctx, val);
	} else {
		val = JS_Eval (ctx, buf, buf_len, filename, eval_flags);
	}
	if (JS_IsException (val)) {
		js_std_dump_error (ctx);
		ret = -1;
	} else {
		ret = 0;
	}
	JS_FreeValue (ctx, val);
	return ret;
}


static int eval_file (JSContext *ctx, const char *filename, int module)
{
	int eval_flags;
	size_t buf_len;

	uint8_t *buf = js_load_file (ctx, &buf_len, filename);
	if (!buf) {
		perror (filename);
		exit (1);
	}

	if (module < 0) {
		module = has_suffix (filename, ".mjs") ||
			JS_DetectModule ((const char *)buf, buf_len);
	}
	if (module)
		eval_flags = JS_EVAL_TYPE_MODULE;
	else
		eval_flags = JS_EVAL_TYPE_GLOBAL;
	const int ret = eval_buf (ctx, buf, buf_len, filename, eval_flags);
	js_free (ctx, buf);
	return ret;
}


////////////////////////////////////////////////////////////////////////////////

// Also used to initialize the worker context
static JSContext *JS_NewCustomContext (JSRuntime *rt)
{
	JSContext *ctx = JS_NewContext (rt);
	if (!ctx) return nullptr;
	if (bignum_ext) {
		JS_AddIntrinsicBigFloat (ctx);
		JS_AddIntrinsicBigDecimal (ctx);
		JS_AddIntrinsicOperators (ctx);
		JS_EnableBignumExt (ctx, TRUE);
	}
	/* system modules */
	js_init_module_std (ctx, "std");
	js_init_module_os (ctx, "os");
	return ctx;
}


////////////////////////////////////////////////////////////////////////////////

#if defined(__APPLE__)
#define MALLOC_OVERHEAD 0
#else
#define MALLOC_OVERHEAD 8
#endif

struct trace_malloc_data {
	uint8_t *base;
};


static unsigned long long
js_trace_malloc_ptr_offset (const uint8_t *ptr,
			    const struct trace_malloc_data *dp)
{
	return ptr - dp->base;
}


// Default memory allocation functions with memory limitation
static size_t js_trace_malloc_usable_size (const void *ptr)
{
#if defined(__APPLE__)
	return malloc_size (ptr);
#elif defined(EMSCRIPTEN)
	return 0;
#elif defined(__linux__) || defined(__GLIBC__)
	return malloc_usable_size ((void *)ptr);
#else
	/* change this to `return 0;` if compilation fails */
	return malloc_usable_size ((void *)ptr);
#endif
}


static void __attribute__ ((format (printf, 2, 3)))
js_trace_malloc_printf (const JSMallocState *s, const char *fmt, ...)
{
	va_list ap;
	int c;

	va_start (ap, fmt);
	while ((c = *fmt++) != '\0') {
		if (c == '%') {
			/* only handle %p and %zd */
			if (*fmt == 'p') {
				const uint8_t *ptr = va_arg (ap, void *);
				if (ptr == nullptr) {
					printf ("nullptr");
				} else {
					printf ("H%+06lld.%zd",
						js_trace_malloc_ptr_offset (
							ptr, s->opaque),
						js_trace_malloc_usable_size (
							ptr));
				}
				fmt++;
				continue;
			}
			if (fmt[0] == 'z' && fmt[1] == 'd') {
				const size_t sz = va_arg (ap, size_t);
				printf ("%zd", sz);
				fmt += 2;
				continue;
			}
		}
		putc (c, stdout);
	}
	va_end (ap);
}


static void js_trace_malloc_init (struct trace_malloc_data *s)
{
	free (s->base = malloc (8));
}


static void *js_trace_malloc (JSMallocState *s, const size_t size)
{
	// Do not allocate zero bytes: behavior is platform dependent
	assert (size != 0);

	if (unlikely (s->malloc_size + size > s->malloc_limit)) return nullptr;
	void *ptr = malloc (size);
	js_trace_malloc_printf (s, "A %zd -> %p\n", size, ptr);
	if (ptr) {
		s->malloc_count++;
		s->malloc_size +=
			js_trace_malloc_usable_size (ptr) + MALLOC_OVERHEAD;
	}
	return ptr;
}


static void js_trace_free (JSMallocState *s, void *ptr)
{
	if (!ptr) return;

	js_trace_malloc_printf (s, "F %p\n", ptr);
	s->malloc_count--;
	s->malloc_size -= js_trace_malloc_usable_size (ptr) + MALLOC_OVERHEAD;
	free (ptr);
}


static void *js_trace_realloc (JSMallocState *s, void *ptr, const size_t size)
{
	if (!ptr) {
		if (size == 0) return nullptr;
		return js_trace_malloc (s, size);
	}
	const size_t old_size = js_trace_malloc_usable_size (ptr);
	if (size == 0) {
		js_trace_malloc_printf (s, "R %zd %p\n", size, ptr);
		s->malloc_count--;
		s->malloc_size -= old_size + MALLOC_OVERHEAD;
		free (ptr);
		return nullptr;
	}
	if (s->malloc_size + size - old_size > s->malloc_limit) return nullptr;

	js_trace_malloc_printf (s, "R %zd %p", size, ptr);

	void *tmp = realloc (ptr, size);
	if (!tmp) {
		fprintf (stderr, "realloc failed, out of memory\n");
		exit (1);
	}
	ptr = tmp;
	js_trace_malloc_printf (s, " -> %p\n", ptr);
	s->malloc_size += js_trace_malloc_usable_size (ptr) - old_size;
	return ptr;
}


static const JSMallocFunctions trace_mf = {
	js_trace_malloc,
	js_trace_free,
	js_trace_realloc,
	js_trace_malloc_usable_size,
};

////////////////////////////////////////////////////////////////////////////////

int main_old (int argc, char **argv)
{
	if (argc > 1 && !strcmp (argv[1], "compile")) {
		for (int i = 1; i < argc; i++) {
			argv[i] = argv[i + 1];
		}
		return compile (--argc, argv);
	}

	struct cliconfig config = {
		.argc = argc,
		.argv = argv,
		.version = "0.1",
		.usage = "ptkl [OPTS] ARG [...]",
	};

	run (&config);

	JSRuntime *rt;
	struct trace_malloc_data trace_data = {nullptr};

	struct runtime_opts opts = {};
	const int optind = parse_runtime_args (argc, argv, &opts);

	// Initialize the runtime
	if (opts.trace_memory) {
		js_trace_malloc_init (&trace_data);
		rt = JS_NewRuntime2 (&trace_mf, &trace_data);
	} else {
		rt = JS_NewRuntime ();
	}
	if (!rt) {
		fprintf (stderr, "ptkl: cannot allocate JS runtime\n");
		exit (2);
	}

	// Configure limits
	if (opts.memory_limit != 0) JS_SetMemoryLimit (rt, opts.memory_limit);
	if (opts.stack_size != 0) JS_SetMaxStackSize (rt, opts.stack_size);

	// Set bignum extension before creating main and worker contexts
	bignum_ext = opts.bignum_ext;

	// Workers
	js_std_set_worker_new_context_func (JS_NewCustomContext);
	js_std_init_handlers (rt);


	// Runtime context
	JSContext *ctx = JS_NewCustomContext (rt);
	if (!ctx) {
		fprintf (stderr, "ptkl: cannot allocate JS context\n");
		exit (2);
	}

	// ES6 module loader
	JS_SetModuleLoaderFunc (rt, nullptr, js_module_loader, nullptr);

	// Handle options

	if (opts.dump_unhandled_promise_rejection) {
		JS_SetHostPromiseRejectionTracker (
			rt, js_std_promise_rejection_tracker, nullptr);
	}

	if (!opts.empty_run) {
		js_std_add_helpers (ctx, argc - optind, argv + optind);

		// make 'std' and 'os' visible to non module code
		if (opts.load_std) {
			const char *str = "import * as std from 'std';\n"
					  "import * as os from 'os';\n"
					  "globalThis.std = std;\n"
					  "globalThis.os = os;\n";
			eval_buf (ctx, str, strlen (str), "<input>",
				  JS_EVAL_TYPE_MODULE);
		}

		for (int i = 0; i < opts.include_count; i++) {
			if (eval_file (ctx, opts.include_list[i], opts.module))
				goto fail;
		}

		if (opts.expr) {
			if (eval_buf (ctx, opts.expr, strlen (opts.expr),
				      "<cmdline>", 0))
				goto fail;
		} else if (optind >= argc) {
			/* interactive mode */
			opts.interactive = 1;
		} else {
			const char *filename = argv[optind];
			if (eval_file (ctx, filename, opts.module)) goto fail;
		}
		if (opts.interactive) {
			js_std_eval_binary (ctx, qjsc_repl, qjsc_repl_size, 0);
		}
		js_std_loop (ctx);
	}

	if (opts.dump_memory) {
		JSMemoryUsage stats;
		JS_ComputeMemoryUsage (rt, &stats);
		JS_DumpMemoryUsage (stdout, &stats, rt);
	}

	js_std_free_handlers (rt);
	JS_FreeContext (ctx);
	JS_FreeRuntime (rt);

	if (opts.empty_run && opts.dump_memory) {
		clock_t t[5];
		double best[5];
		for (int i = 0; i < 100; i++) {
			t[0] = clock ();
			rt = JS_NewRuntime ();
			t[1] = clock ();
			ctx = JS_NewContext (rt);
			t[2] = clock ();
			JS_FreeContext (ctx);
			t[3] = clock ();
			JS_FreeRuntime (rt);
			t[4] = clock ();
			for (int j = 4; j > 0; j--) {
				const unsigned ms = 1000 * (t[j] - t[j - 1]) /
					CLOCKS_PER_SEC;
				if (i == 0 || best[j] > ms) best[j] = ms;
			}
		}
		printf ("\nInstantiation times (ms): %.3f = "
			"%.3f+%.3f+%.3f+%.3f\n",
			best[1] + best[2] + best[3] + best[4], best[1], best[2],
			best[3], best[4]);
	}

	return 0;
fail:
	js_std_free_handlers (rt);
	JS_FreeContext (ctx);
	JS_FreeRuntime (rt);
	return 1;
}


////////////////////////////////////////////////////////////////////////////////

static void root_command_handler (struct ptkl_context *ctx)
{
	printf ("root: %s\n", ctx->command->name);
}


static void run_command_handler (struct ptkl_context *ctx)
{
	printf ("run: %s\n", ctx->command->name);
}


static void eval_command_handler (struct ptkl_context *ctx)
{
	printf ("eval: %s\n", ctx->command->name);
}


static void repl_command_handler (struct ptkl_context *ctx)
{
	printf ("repl: %s\n", ctx->command->name);
}


static void console_command_handler (struct ptkl_context *ctx)
{
	printf ("console: %s\n", ctx->command->name);
}


///////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv)
{

	// run command
	struct ptkl_command run_cmd = {
		.name = "run",
		.help = "Run a JavaScript program",
		.handler = run_command_handler,
	};
	struct ptkl_arg run_file_arg = {
		.name = "file",
	};
	struct ptkl_arg run_file_extra_args = {
		.name = "args",
		.optional = true,
		.multi = true,
	};
	ptkl_command_add_arg (&run_cmd, &run_file_arg);
	ptkl_command_add_arg (&run_cmd, &run_file_extra_args);

	// eval command
	struct ptkl_command eval_cmd = {
		.name = "eval",
		.help = "Evaluate a JavaScript string",
		.handler = eval_command_handler,
	};
	struct ptkl_arg eval_expr_arg = {
		.name = "expr",
	};

	// repl command
	struct ptkl_command repl_cmd = {
		.name = "repl",
		.help = "Start a JavaScript Read-Eval-Print Loop (REPL)",
		.handler = repl_command_handler,
	};

	// console command
	struct ptkl_command console_cmd = {
		.name = "console",
		.help = "Start interactive console",
		.handler = console_command_handler,
	};

	// root command
	struct ptkl_command root_cmd = {
		.name = "root",
		.handler = root_command_handler,
		.help = "Partikle is a lightweight runtime for modern "
			"JavaScript.\n"
			"Run a subcommand, file, expression, or start the "
			"console.\n\n"
			"  " PTKL " [options] <command> [args...]\n"
			"  " PTKL " [options] <file> [args...]       => " PTKL
			" run\n"
			"  " PTKL " [options] <expr> [args...]       => " PTKL
			" eval\n"
			"  " PTKL " [options]                        => " PTKL
			" console\n",
	};
	struct ptkl_option version_option = {
		.type = OPT_STRING,
		.short_opt = 'v',
		.long_opt = "version",
		.help = "Print version",
	};
	struct ptkl_option help_option = {
		.type = OPT_BOOL,
		.short_opt = 'h',
		.long_opt = "help",
		.help = "Print help",
	};
	// cli
	struct ptkl_cli cli = {
		.name = PTKL,
		.version = CONFIG_VERSION,
		.command = &root_cmd,
	};

	ptkl_command_add_arg (&eval_cmd, &eval_expr_arg);

	ptkl_command_add_option (&root_cmd, &version_option);
	ptkl_command_add_option (&root_cmd, &help_option);

	ptkl_command_add_subcommand (&root_cmd, &run_cmd);
	ptkl_command_add_subcommand (&root_cmd, &eval_cmd);
	ptkl_command_add_subcommand (&root_cmd, &repl_cmd);
	ptkl_command_add_subcommand (&root_cmd, &console_cmd);

	struct ptkl_option bs_option = {
		.type = OPT_STRING,
		.short_opt = 's',
		.long_opt = "bs",
		.help = "bs option",
	};
	ptkl_command_add_option (&repl_cmd, &bs_option);

	struct ptkl_command foo_cmd = {
		.name = "foo",
	};
	struct ptkl_option foo_option = {
		.type = OPT_STRING,
		.short_opt = 'f',
		.long_opt = "foo",
		.help = "foo option",
	};
	ptkl_command_add_option (&foo_cmd, &foo_option);
	ptkl_command_add_subcommand (&repl_cmd, &foo_cmd);

	struct ptkl_command bar_cmd = {
		.name = "bar",
	};
	struct ptkl_option bar_option = {
		.type = OPT_STRING,
		.short_opt = 'b',
		.long_opt = "bar",
		.help = "bar option",
	};
	ptkl_command_add_option (&bar_cmd, &bar_option);
	ptkl_command_add_subcommand (&foo_cmd, &bar_cmd);

	struct ptkl_command baz_cmd = {
		.name = "baz",
	};
	struct ptkl_option baz_option = {
		.type = OPT_STRING,
		.short_opt = 'z',
		.long_opt = "baz",
		.help = "baz option",
	};
	ptkl_command_add_option (&baz_cmd, &baz_option);
	ptkl_command_add_subcommand (&console_cmd, &baz_cmd);

	// ptkl_cli_help(&cli);
	// print_command_help(&run_cmd);

	return ptkl_cli_run (&cli, argc, argv);
}


// This first pass ensures that all options appear to be valid, meaning that
// each one is uniquely associated with a command and matches that command's
// specification for the option. At this point, this doesn't guarantee there
// won't be a failure later if the command that the option matches isn't the
// command that is ultimately being invoked. An unlikely but possible failure
// could arise if an option expects an argument that happens to also match a
// subcommand; to make the command work, the option should be set using an
// explicit assignment (form `--foo=bar` instead of `--foo bar`).
static int process_options (struct ptkl_cli *cli, int argc, char **argv)
{
	int optind = 1;
	// while (optind < argc && *argv[optind] == '-') {
	while (optind < argc && *argv[optind]) {
		printf ("%s\n", argv[optind++]);
	}
	return 0;
}


// static bool find_option()


// Set of options defined by all commands, used to ensure options are unique.
struct option_node {
	struct ptkl_option *option;
	struct option_node *next;
};


static void dbg_print_set (const char *msg, const struct option_node *node)
{
	int i = 0;
	printf ("  %s: set:", msg);
	while (node) {
		printf (" #%d(%s -%c --%s)", ++i, node->option->command->name,
			node->option->short_opt, node->option->long_opt);
		node = node->next;
	}
	printf ("\n");
}


// Insert option into options set; print an error and return false if option is
// already in the set.
static bool set_insert_option (struct option_node **set,
			       struct option_node *new)
{
	if (!*set) {
		*set = new;
		return true;
	}
	struct option_node *e = *set;
	while (e) {
		if (new->option->short_opt == e->option->short_opt) {
			printf ("error: short option -%c for %s command is "
				"already defined for %s command\n",
				new->option->short_opt,
				new->option->command->name,
				e->option->command->name);
			return false;
		}
		if (new->option->long_opt && e->option->long_opt &&
		    !strcmp (new->option->long_opt, e->option->long_opt)) {
			printf ("error: long option -%s for %s command is "
				"already defined for %s command\n",
				new->option->long_opt,
				new->option->command->name,
				e->option->command->name);
			return false;
		}
		if (e->next)
			e = e->next;
		else {
			e->next = new;
			break;
		}
	}
	return true;
}


// Recursively scan all commands and populate options set. Returns false if an
// option is not unique.
static bool scan_command_options (const struct ptkl_command *cmd,
				  struct option_node **set)
{
	while (cmd) {
		struct ptkl_option *opt = cmd->options;
		while (opt) {
			// printf("current command: %s, current option: -%c
			// --%s\n", 	   cmd->name, opt->short_opt,
			// opt->long_opt);
			struct option_node *node =
				calloc (1, sizeof (struct option_node));
			node->option = opt;
			if (!set_insert_option (set, node)) return false;
			opt = opt->next;
		}
		if (cmd->subcommand) {
			if (!scan_command_options (cmd->subcommand, set))
				return false;
		}
		cmd = cmd->next;
	}
	return true;
}


int ptkl_cli_run (struct ptkl_cli *cli, int argc, char **argv)
{
	int error_status = 0;

	// Set of options defined by all commands
	struct option_node *options = nullptr;

	struct ptkl_command *cmd = cli->command;

	if (!scan_command_options (cmd, &options)) goto error;
	dbg_print_set ("final options", options);

	// By default, the expected command is the root command. As options are
	// parsed from left to right, the expected command can become a
	// descendant command if it matches the option. All options must be
	// matched along a direct path from the root to the final descendant
	// command (which will become the expected command). Once a path is
	// established, any remaining unmatched options (even if they
	// potentially match a command on another path) will trigger an error
	// since . struct ptkl_command *expected_cmd = cli->command;
	//
	// //error_status = process_options(cli, argc, argv);
	// for (auto i = 0; i < argc; i++) {
	// 	char *arg = argv[i];
	// 	if (*arg == '-') {
	// 		if (*(arg+1) == '-') {
	// 			// long option
	// 			arg = arg + 2;
	// 			printf("long option: %s\n", arg);
	// 		} else {
	// 			// short option
	// 			arg = arg + 1;
	// 			const unsigned n = strnlen(arg, 10);
	// 			for (auto si = 0; si < n; si++) {
	// 				printf("short option: %c\n", arg[si]);
	// 			}
	// 		}
	// 	} else {
	// 		// command arg
	// 		printf("%s\n", arg);
	// 	}
	// }
	// if (error_status) goto error;

	// while (optind < argc && *argv[optind] == '-') {
	//  while (optind < argc && *argv[optind]) {
	//  }

	return 0;

error:
	return error_status || 1;
}
