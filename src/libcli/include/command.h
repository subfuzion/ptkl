/*
 * ptkl command api
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

#ifndef COMMAND_H
#define COMMAND_H

#include "adt.h"

typedef struct command *command;

/* command handler: command_fn */
typedef bool (*command_fn) (command);

typedef struct command {
	string name;
	string help;
	command_fn fn;

	/* settings: map[char *] -> string */
	map *settings;

	/* the original args passed to the command */
	int argc;
	char **argv;

	/* the args remaining after parsing options */
	vector *args;

	/* subcommands */
	map *commands;
	struct command *parent;

	/* errors during command execution */
	stack *errors;
} *command;

command command_new (const char *name, const char *help, command_fn fn);
void command_free (command cmd);

void command_set (command cmd, const char *key, const char *value);
string command_get (command cmd, const char *name);

/* add a subcommand to cmd */
void command_add_command (command cmd, const char *name, const char *help,
			  command_fn fn);

bool command_run (command cmd, int argc, char **argv);

void command_push_error (command cmd, string error);
void command_push_errorf (command cmd, const char *fmt, ...);
void command_print_errors (command cmd);

#endif /* COMMAND_H */
