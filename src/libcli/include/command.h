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

/* forward declarations*/
typedef struct command *command;
typedef struct flag *flag;

/* flag handler callback function: flag_fn */
typedef bool (*flag_fn) (flag);

typedef enum flag_arg {
	NONE,
	REQUIRED,
	OPTIONAL,
} flag_arg;

typedef struct flag {
	string name;
	string long_option;
	char short_option;
	flag_arg has_arg;

	/* the flag's argument value, if it takes one, after parsing */
	string arg;

	/* optional callback function */
	flag_fn fn;

	/* command this flag belongs to */
	command command;
} *flag;

/* command handler callback function: command_fn */
typedef bool (*command_fn) (command);

typedef struct command {
	string name;
	string help;
	command_fn fn;

	/* the original args passed to the command */
	int argc;
	char **argv;

	/* flags after parsing argv */
	map *flags;

	/* args after parsing argv (for command or args[0] subcommand) */
	vector *args;

	/* subcommands */
	map *commands;
	struct command *parent;

	/* settings: map[char *] -> string */
	map *settings;

	/* errors during command execution */
	stack *errors;
} *command;

command command_new (const char *name, const char *help, command_fn fn);
void command_free (command cmd);

void command_set (command cmd, const char *key, const char *value);
string command_get (command cmd, const char *name);

bool command_run (command cmd, int argc, char **argv);

void command_print_errors (command cmd);

/**
 * Add a flag to command.
 * At least one of long_option (not null) or short_option (not 0) is required.
 */
flag command_add_flag (command cmd, const char *name, const char *long_option,
		       char short_option, flag_arg has_arg);

/**
 * Set an optional callback function for a flag. This is mostly convenient for
 * flags that short circuit normal processing, like printing help and exiting,
 * but might be useful for other use cases. Flag callbacks run before the
 * callback for the command they belong to.
 */
void flag_add_callback (flag f, flag_fn fn);

/**
 * Add a subcommand to command.
 */
void command_add_command (command cmd, const char *name, const char *help,
			  command_fn fn);

#endif /* COMMAND_H */
