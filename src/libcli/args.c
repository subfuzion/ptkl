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

#include "args.h"


void
init_cli (struct ptkl_cli *cli, char *version, char *description)
{

}


bool
cli_parse(struct ptkl_cli *cli, int argc, char **argv)
{
	int optind = 1;
	// while (optind < argc && *argv[optind] == '-') {
	while (optind < argc && *argv[optind]) {
		printf ("%s\n", argv[optind++]);
	}
	return 0;
}

bool
parse_option (struct ptkl_option *opt)
{
	const char *str = opt->text;

	switch (opt->spec.type) {
	case TT_STR:
		// TODO: evaluate strdup pros/cons (don't need fixed buffer / need to free later)
		strcpy (opt->value.string, str);
		return true;
	case TT_BOOL:
		if (strcmp (opt->text, opt->spec.name) == 0) {
			opt->value.boolean = true;
			return true;
		}
		break;
	case TT_INT:
		char *end;
		const long num = strtol (str, &end, 10);

		if (*end != '\0') {
			strcpy (opt->error, "%s");
			sprintf (opt->error, "Invalid input or trailing characters: %s", end);
		} else if (num > INT_MAX || num < INT_MIN) {
			strcpy (opt->error, "%s");
			sprintf (opt->error, "Invalid input (number out of range): %s", str);
		} else {
			opt->value.integer = (int)num;
			return true;
		}
		break;

	default:
		// TODO: evaluate if it would be better to treat generically as a string
		strcpy (opt->error, "%s");
		sprintf (opt->error, "unknown option type: %d, can't parse: %s", opt->spec.type, str);
		break;
	}

	return false;
}
