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

#include "dstring.h"
#include "map.h"
#include "vector.h"

/* Specifies how to parse a command line token */
typedef enum token_type {
	TT_STR,
	TT_BOOL,
	TT_INT,
} token_type;

/* Holds the command line token's value after parsing */
union ptkl_parse_value {
	char *string;
	bool boolean;
	int integer;
};

/* A parsed command line option */
typedef struct cli {
	dstring name;
	dstring version;
	dstring description;

	map *commands;
	map *options;
	vector *args;
} *cli;

typedef struct command {
	dstring name;
	dstring description;
} *command;

// typedef struct option {
// 	/* name is used as the long option */
// 	dstring name;
// 	dstring description;
//
// 	const char short_opt;
// 	bool multi;
// 	token_type type;
//
// 	// struct result {
// 	//
// 	// };
// } *option;

// typedef struct option {
// 	option_spec spec;
//
// 	char *text;
//
// 	union {
// 		union ptkl_parse_value value;
// 		vector values;
// 	};
//
// 	char error[256];
// } *option;

cli cli_new (const char *name, const char *version, const char *description);
void cli_destroy (cli cli);

command cli_add_command (cli cli, const char *name, const char *description);

// option cli_add_option (cli cli, option opt);
void option_set_short_option(char ch);
void option_set_multi(bool allow);


bool cli_parse (cli cli, int argc, char **argv);

// bool parse_option (struct option *opt);

#endif /* ARGS_H */
