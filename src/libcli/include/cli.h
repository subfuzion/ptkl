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

#include "map.h"
#include "strings.h"
#include "vector.h"


/* CLI commands */

typedef struct cli *cli;

typedef void (*command_fn) (cli);

typedef struct command {
	string name;
	command_fn fn;
	vector *args;
} *command;

command command_new (const char *name, command_fn fn);
void command_free (command cmd);


/* CLI */

typedef struct cli {
	string name;
	string version;
	string description;
	map *commands;
} *cli;

struct cli_parse_result {
	bool ok;
	union {
		string error;
		struct command *cmd;
	};
};

cli cli_new (const char *name, const char *version, const char *description);
void cli_free (cli cli);
void cli_add_command (cli cli, const char *name, command_fn fn);
void cli_parse_args (int argc, char **argv, cli cli,
		     struct cli_parse_result *result);


#endif /* ARGS_H */
