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
typedef void (*flag_fn) (flag);

typedef enum flag_arg {
	NO_ARGUMENT,
	REQUIRED_ARGUMENT,
	OPTIONAL_ARGUMENT,
} flag_arg;

/* use "flag" internally to avoid confusion with getopt struct option */
typedef struct flag {
	char short_flag;
	string long_flag;
	flag_arg has_arg;
	string help;

	/* the original string obtained by getopt */
	char *text;

	/* the flag's argument value, if it takes one, after parsing */
	string arg;

	/* optional callback function */
	flag_fn fn;

	/* should exit after running callback function (ex: version, help) */
	bool should_exit;

	/* command this flag belongs to */
	command command;
} *flag;

/* command handler callback function: command_fn */
typedef void (*command_fn) (command);

typedef enum command_args {
	COMMAND_ARGS_ANY = -1,
	COMMAND_ARGS_NONE = 0,
} command_args;

typedef struct command {
	string name;
	string help;
	string group; /* category/group this command belongs to */
	command_fn fn;

	/* the original args passed to the command */
	int argc;
	char **argv;

	/* flags after parsing argv */
	vector *flags;

	/* how many args to expect (-1 for any number, 0 for none, etc.) */
	int expect_args;

	/* args after parsing argv (for command or args[0] subcommand) */
	vector *args;

	/* subcommands */
	map *commands; /* for lookup by name */
	vector *ordered_commands; /* for iteration in order */
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

/**
 * Set the group/category that a command belongs to.
 * @param cmd The command to set the group for
 * @param group The name of the group
 */
void command_set_group (command cmd, const char *group);

/**
 * The number of arguments the command expects
 * @param count Set a specific count (>= 0) or any amount (-1)
 */
void command_expect_args (command cmd, command_args count);

bool command_run (command cmd, int argc, char **argv);


void command_push_error (command cmd, const char *error);
void command_push_error_string (command cmd, string error);
void command_push_errorf (command cmd, const char *fmt, ...);
void command_print_errors (command cmd);

/**
 * Add a flag to command.
 * At least one of long_option (not null) or short_option (not 0) is required.
 */
flag command_flag (command cmd, char short_option, const char *long_option,
		   flag_arg has_arg, const char *help);

/**
 * Set an optional callback function for a flag. This is mostly convenient for
 * flags that short circuit normal processing, like printing help and exiting,
 * but might be useful for other use cases. Flag callbacks run before the
 * callback for the command they belong to.
 */
void flag_add_callback (flag f, flag_fn fn, bool should_exit);

/**
 * Add a subcommand to command.
 */
command command_add (command cmd, const char *name, const char *help,
		     command_fn fn);


#endif /* COMMAND_H */
