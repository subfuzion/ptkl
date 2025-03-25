/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 *
 * Copyright (c) 2025 Tony Pujals
 * Copyright (c) 2017-2024 Charlie Gordon
 * Copyright (c) 2017-2024 Fabrice Bellard
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

#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "ptkl.h"

void
help (const int exit_code)
{
	printf ("Partikle Runtime (version " CONFIG_VERSION ")\n"
		"usage: " PTKL " [options] [file [args]]\n"
		"-e  --eval EXPR            evaluate EXPR\n"
		"-v  --version              print version\n"
		"-h  --help                 show this help\n");
	exit (exit_code);
}

void
version ()
{
	printf ("%s %s\n", PTKL, CONFIG_VERSION);
	exit (EXIT_SUCCESS);
}

int
main (const int argc, char **argv)
{
	// struct opts opts = {};
	// if (!parse_args(argc, argv, &opts)) {
	// 	fprintf(stderr, "%s\n", opts.error);
	// 	help(EXIT_FAILURE);
	// }
	//
	// if (opts.cmd.version) version();
	// if (opts.cmd.help) help(EXIT_SUCCESS);

	return EXIT_SUCCESS;
}
