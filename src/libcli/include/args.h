/*
 * ptkl command line args
 *
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

#ifndef ARGS_H
#define ARGS_H

#include <stddef.h>

#include "dstring.h"

#include "map.h"
#include "vector.h"

/* Specifies how to parse a command line token */
enum ptkl_token_type {
	TT_STR,
	TT_BOOL,
	TT_INT,
};

/* Holds the command line token's value after parsing */
union ptkl_parse_value {
	char *string;
	bool boolean;
	int integer;
};

/* A command line option spec */
struct ptkl_option_spec {
	/* name is used as the long option */
	const char *name;
	const char short_opt;
	const char *help;
	bool multi;
	enum ptkl_token_type type;

	/* internal */
	/* struct ptkl_command *command; */
};

/* A parsed command line option */
struct ptkl_option {
	struct ptkl_option_spec spec;
	char *text;

	union {
		union ptkl_parse_value value;
		vector values;
	};

	char error[256];
};

struct ptkl_cli {
	char *version;
	char *description;

	map *options;
};

void init_cli (struct ptkl_cli *cli, const char *version, const char *description);
bool add_option (struct ptkl_cli *cli, struct ptkl_option_spec spec);
bool cli_parse (struct ptkl_cli *cli, int argc, char **argv);

bool parse_option (struct ptkl_option *opt);


#endif /* ARGS_H */
