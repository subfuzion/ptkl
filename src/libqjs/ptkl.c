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

#include "ptklargs.h"
#include "cutils.h"
#include "quickjs-libc.h"

extern const uint8_t qjsc_repl[];
extern const uint32_t qjsc_repl_size;

extern int compile(int argc, char **argv);

// Automatically compiled with ecmascript standard bigint support, so this
// just disables non-standard bigdecimal, bigfloat, and related extensions
static int bignum_ext = 0;

static int eval_buf(JSContext *ctx, const void *buf, const size_t buf_len,
		    const char *filename, const int eval_flags) {
	JSValue val;
	int ret;

	if ((eval_flags & JS_EVAL_TYPE_MASK) == JS_EVAL_TYPE_MODULE) {
		// For modules, compile then run to be able to set import.meta
		val = JS_Eval(ctx, buf, buf_len, filename,
			      eval_flags | JS_EVAL_FLAG_COMPILE_ONLY);
		if (!JS_IsException(val)) {
			js_module_set_import_meta(ctx, val, TRUE, TRUE);
			val = JS_EvalFunction(ctx, val);
		}
		val = js_std_await(ctx, val);
	} else {
		val = JS_Eval(ctx, buf, buf_len, filename, eval_flags);
	}
	if (JS_IsException(val)) {
		js_std_dump_error(ctx);
		ret = -1;
	} else {
		ret = 0;
	}
	JS_FreeValue(ctx, val);
	return ret;
}

static int eval_file(JSContext *ctx, const char *filename, int module) {
	int eval_flags;
	size_t buf_len;

	uint8_t *buf = js_load_file(ctx, &buf_len, filename);
	if (!buf) {
		perror(filename);
		exit(1);
	}

	if (module < 0) {
		module = has_suffix(filename, ".mjs") ||
			 JS_DetectModule((const char *) buf, buf_len);
	}
	if (module)
		eval_flags = JS_EVAL_TYPE_MODULE;
	else
		eval_flags = JS_EVAL_TYPE_GLOBAL;
	const int ret = eval_buf(ctx, buf, buf_len, filename, eval_flags);
	js_free(ctx, buf);
	return ret;
}

// Also used to initialize the worker context
static JSContext *JS_NewCustomContext(JSRuntime *rt) {
	JSContext *ctx = JS_NewContext(rt);
	if (!ctx)
		return nullptr;
	if (bignum_ext) {
		JS_AddIntrinsicBigFloat(ctx);
		JS_AddIntrinsicBigDecimal(ctx);
		JS_AddIntrinsicOperators(ctx);
		JS_EnableBignumExt(ctx, TRUE);
	}
	/* system modules */
	js_init_module_std(ctx, "std");
	js_init_module_os(ctx, "os");
	return ctx;
}

#if defined(__APPLE__)
#define MALLOC_OVERHEAD  0
#else
#define MALLOC_OVERHEAD  8
#endif

struct trace_malloc_data {
	uint8_t *base;
};

static unsigned long long js_trace_malloc_ptr_offset(const uint8_t *ptr,
						     const struct
						     trace_malloc_data *dp) {
	return ptr - dp->base;
}

// Default memory allocation functions with memory limitation
static size_t js_trace_malloc_usable_size(const void *ptr) {
#if defined(__APPLE__)
	return malloc_size(ptr);
#elif defined(EMSCRIPTEN)
    return 0;
#elif defined(__linux__) || defined(__GLIBC__)
    return malloc_usable_size((void *)ptr);
#else
    /* change this to `return 0;` if compilation fails */
    return malloc_usable_size((void *)ptr);
#endif
}

static void
__attribute__((format(printf, 2, 3)))
js_trace_malloc_printf(const JSMallocState *s, const char *fmt, ...) {
	va_list ap;
	int c;

	va_start(ap, fmt);
	while ((c = *fmt++) != '\0') {
		if (c == '%') {
			/* only handle %p and %zd */
			if (*fmt == 'p') {
				const uint8_t *ptr = va_arg(ap, void *);
				if (ptr == nullptr) {
					printf("nullptr");
				} else {
					printf("H%+06lld.%zd",
					       js_trace_malloc_ptr_offset(
						       ptr, s->opaque),
					       js_trace_malloc_usable_size(
						       ptr));
				}
				fmt++;
				continue;
			}
			if (fmt[0] == 'z' && fmt[1] == 'd') {
				const size_t sz = va_arg(ap, size_t);
				printf("%zd", sz);
				fmt += 2;
				continue;
			}
		}
		putc(c, stdout);
	}
	va_end(ap);
}

static void js_trace_malloc_init(struct trace_malloc_data *s) {
	free(s->base = malloc(8));
}

static void *js_trace_malloc(JSMallocState *s, const size_t size) {
	// Do not allocate zero bytes: behavior is platform dependent
	assert(size != 0);

	if (unlikely(s->malloc_size + size > s->malloc_limit))
		return nullptr;
	void *ptr = malloc(size);
	js_trace_malloc_printf(s, "A %zd -> %p\n", size, ptr);
	if (ptr) {
		s->malloc_count++;
		s->malloc_size += js_trace_malloc_usable_size(ptr) +
				MALLOC_OVERHEAD;
	}
	return ptr;
}

static void js_trace_free(JSMallocState *s, void *ptr) {
	if (!ptr)
		return;

	js_trace_malloc_printf(s, "F %p\n", ptr);
	s->malloc_count--;
	s->malloc_size -= js_trace_malloc_usable_size(ptr) + MALLOC_OVERHEAD;
	free(ptr);
}

static void *js_trace_realloc(JSMallocState *s, void *ptr, const size_t size) {
	if (!ptr) {
		if (size == 0)
			return nullptr;
		return js_trace_malloc(s, size);
	}
	const size_t old_size = js_trace_malloc_usable_size(ptr);
	if (size == 0) {
		js_trace_malloc_printf(s, "R %zd %p\n", size, ptr);
		s->malloc_count--;
		s->malloc_size -= old_size + MALLOC_OVERHEAD;
		free(ptr);
		return nullptr;
	}
	if (s->malloc_size + size - old_size > s->malloc_limit)
		return nullptr;

	js_trace_malloc_printf(s, "R %zd %p", size, ptr);

	void *tmp = realloc(ptr, size);
	if (!tmp) {
		fprintf(stderr, "realloc failed, out of memory\n");
		exit(1);
	}
	ptr = tmp;
	js_trace_malloc_printf(s, " -> %p\n", ptr);
	s->malloc_size += js_trace_malloc_usable_size(ptr) - old_size;
	return ptr;
}

static const JSMallocFunctions trace_mf = {
	js_trace_malloc,
	js_trace_free,
	js_trace_realloc,
	js_trace_malloc_usable_size,
};

int main(int argc, char **argv) {
	if (argc > 1 && !strcmp(argv[1], "compile")) {
		for (int i = 1; i < argc; i++) {
			argv[i] = argv[i + 1];
		}
		return compile(--argc, argv);
	}

	JSRuntime *rt;
	struct trace_malloc_data trace_data = {nullptr};

	struct runtime_opts opts = {};
	const int optind = parse_runtime_args(argc, argv, &opts);

	// Initialize the runtime
	if (opts.trace_memory) {
		js_trace_malloc_init(&trace_data);
		rt = JS_NewRuntime2(&trace_mf, &trace_data);
	} else {
		rt = JS_NewRuntime();
	}
	if (!rt) {
		fprintf(stderr, "ptkl: cannot allocate JS runtime\n");
		exit(2);
	}

	// Configure limits
	if (opts.memory_limit != 0)
		JS_SetMemoryLimit(rt, opts.memory_limit);
	if (opts.stack_size != 0)
		JS_SetMaxStackSize(rt, opts.stack_size);

	// Set bignum extension before creating main and worker contexts
	bignum_ext = opts.bignum_ext;

	// Workers
	js_std_set_worker_new_context_func(JS_NewCustomContext);
	js_std_init_handlers(rt);


	// Runtime context
	JSContext *ctx = JS_NewCustomContext(rt);
	if (!ctx) {
		fprintf(stderr, "ptkl: cannot allocate JS context\n");
		exit(2);
	}

	// ES6 module loader
	JS_SetModuleLoaderFunc(rt, nullptr, js_module_loader, nullptr);

	// Handle options

	if (opts.dump_unhandled_promise_rejection) {
		JS_SetHostPromiseRejectionTracker(
			rt, js_std_promise_rejection_tracker,
			nullptr);
	}

	if (!opts.empty_run) {
		js_std_add_helpers(ctx, argc - optind, argv + optind);

		// make 'std' and 'os' visible to non module code
		if (opts.load_std) {
			const char *str = "import * as std from 'std';\n"
					"import * as os from 'os';\n"
					"globalThis.std = std;\n"
					"globalThis.os = os;\n";
			eval_buf(ctx, str, strlen(str), "<input>",
				 JS_EVAL_TYPE_MODULE);
		}

		for (int i = 0; i < opts.include_count; i++) {
			if (eval_file(ctx, opts.include_list[i], opts.module))
				goto fail;
		}

		if (opts.expr) {
			if (eval_buf(ctx, opts.expr, strlen(opts.expr),
				     "<cmdline>", 0))
				goto fail;
		} else if (optind >= argc) {
			/* interactive mode */
			opts.interactive = 1;
		} else {
			const char *filename = argv[optind];
			if (eval_file(ctx, filename, opts.module))
				goto fail;
		}
		if (opts.interactive) {
			js_std_eval_binary(ctx, qjsc_repl, qjsc_repl_size, 0);
		}
		js_std_loop(ctx);
	}

	if (opts.dump_memory) {
		JSMemoryUsage stats;
		JS_ComputeMemoryUsage(rt, &stats);
		JS_DumpMemoryUsage(stdout, &stats, rt);
	}

	js_std_free_handlers(rt);
	JS_FreeContext(ctx);
	JS_FreeRuntime(rt);

	if (opts.empty_run && opts.dump_memory) {
		clock_t t[5];
		double best[5];
		for (int i = 0; i < 100; i++) {
			t[0] = clock();
			rt = JS_NewRuntime();
			t[1] = clock();
			ctx = JS_NewContext(rt);
			t[2] = clock();
			JS_FreeContext(ctx);
			t[3] = clock();
			JS_FreeRuntime(rt);
			t[4] = clock();
			for (int j = 4; j > 0; j--) {
				const unsigned ms =
						1000 * (t[j] - t[j - 1]) /
						CLOCKS_PER_SEC;
				if (i == 0 || best[j] > ms)
					best[j] = ms;
			}
		}
		printf(
			"\nInstantiation times (ms): %.3f = %.3f+%.3f+%.3f+%.3f\n",
			best[1] + best[2] + best[3] + best[4],
			best[1], best[2], best[3], best[4]);
	}

	return 0;
fail:
	js_std_free_handlers(rt);
	JS_FreeContext(ctx);
	JS_FreeRuntime(rt);
	return 1;
}
