/*
 * ptkl command line args
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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* #include "args.h" */
#ifndef ARGS_H
#define ARGS_H

#include <stddef.h>

struct cmd_opts {
	bool help;
	bool version;
};

struct runtime_opts {
	char *expr;
	int interactive;
	int dump_memory;
	int trace_memory;
	int empty_run;
	int module;
	int load_std;
	int dump_unhandled_promise_rejection;
	size_t memory_limit;
	char *include_list[32];
	int include_count;
	size_t stack_size;
	int bignum_ext;
};

struct compiler_opts {};

struct opts {
	int optind;
	char error[256];
	struct cmd_opts cmd;
	struct runtime_opts runtime;
	struct compiler_opts compiler;
};

bool parse_args (int argc, char **argv, struct opts *opts);

#endif /* ARGS_H */

#define PTKL "ptkl"

#ifndef count_elements
#define count_elements(x) (sizeof (x) / sizeof ((x)[0]))
#endif

bool
parse_args (const int argc, char **argv, struct opts *opts)
{
	opts->runtime.expr = nullptr;
	opts->runtime.interactive = 0;
	opts->runtime.dump_memory = 0;
	opts->runtime.trace_memory = 0;
	opts->runtime.empty_run = 0;
	// TODO: can't make module the default yet due to tests/test_closure.js::test_with()
	opts->runtime.module = -1;
	opts->runtime.load_std = 1; // 0
	opts->runtime.dump_unhandled_promise_rejection = 0;
	opts->runtime.memory_limit = 0;
	opts->runtime.include_count = 0;
	opts->runtime.stack_size = 0;
	opts->runtime.bignum_ext = 0;

	int optind = 1;
	while (optind < argc && *argv[optind] == '-') {
		char *arg = argv[optind] + 1;
		const char *longopt = "";
		// A single - is not an option, it also stops argument scanning
		if (!*arg) break;
		optind++;
		if (*arg == '-') {
			longopt = arg + 1;
			arg += strlen (arg);
			// -- stops argument scanning
			if (!*longopt) break;
		}
		for (; *arg || *longopt; longopt = "") {
			const char opt = *arg;
			if (opt) arg++;
			if (opt == 'h' || opt == '?' || !strcmp (longopt, "help")) {
				opts->cmd.help = true;
				continue;
			}
			if (opt == 'e' || !strcmp (longopt, "eval")) {
				if (*arg) {
					opts->runtime.expr = arg;
					break;
				}
				if (optind < argc) {
					opts->runtime.expr = argv[optind++];
					break;
				}
				snprintf (opts->error, 256, "%s: missing expression for -e\n", PTKL);
				goto done;
			}
			if (opt == 'I' || !strcmp (longopt, "include")) {
				if (optind >= argc) {
					fprintf (stderr, "expecting filename");
					exit (1);
				}
				if (opts->runtime.include_count >= count_elements (opts->runtime.include_list)) {
					snprintf (opts->error, 256, "too many included files");
					goto done;
				}
				opts->runtime.include_list[opts->runtime.include_count++] = argv[optind++];
				continue;
			}
			if (opt == 'm' || !strcmp (longopt, "module")) {
				opts->runtime.module = 1;
				continue;
			}
			if (!strcmp (longopt, "script")) {
				opts->runtime.module = 0;
				continue;
			}
			if (opt == 'd' || !strcmp (longopt, "dump")) {
				opts->runtime.dump_memory++;
				continue;
			}
			if (opt == 'T' || !strcmp (longopt, "trace")) {
				opts->runtime.trace_memory++;
				continue;
			}
			if (!strcmp (longopt, "std")) {
				opts->runtime.load_std = 1;
				continue;
			}
			if (!strcmp (longopt, "unhandled-rejection")) {
				opts->runtime.dump_unhandled_promise_rejection = 1;
				continue;
			}
			if (!strcmp (longopt, "bignum")) {
				opts->runtime.bignum_ext = 1;
				continue;
			}
			if (opt == 'q' || !strcmp (longopt, "quit")) {
				opts->runtime.empty_run++;
				continue;
			}
			if (!strcmp (longopt, "memory-limit")) {
				if (optind >= argc) {
					snprintf (opts->error, 256, "expecting memory limit");
					goto done;
				}
				opts->runtime.memory_limit = (size_t)strtod (argv[optind++], nullptr);
				continue;
			}
			if (!strcmp (longopt, "stack-size")) {
				if (optind >= argc) {
					snprintf (opts->error, 256, "expecting stack size");
					goto done;
				}
				opts->runtime.stack_size = (size_t)strtod (argv[optind++], nullptr);
				continue;
			}
			if (opt == 'v' || !strcmp (longopt, "version")) {
				opts->cmd.version = true;
				goto done;
			}
			if (opt) {
				snprintf (opts->error, 256, "%s: unknown option '-%c'", PTKL, opt);
				goto done;
			} else {
				snprintf (opts->error, 256, "%s: unknown option '--%s'", PTKL, longopt);
				goto done;
			}
		}
	}

done:
	opts->optind = optind;
	return strnlen (opts->error, 256) == 0;
}
