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

#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "command.h"
#include "log.h"
#include "strings.h"


static void command_push_error (command cmd, string error)
{
	stack_push (cmd->errors, error);
}


static void command_push_errorf (command cmd, const char *fmt, ...)
{
	va_list (args);
	va_start (args, fmt);
	command_push_error (cmd, sdscatvprintf (sdsempty (), fmt, args));
	va_end (args);
}


command command_new (const char *name, const char *help, command_fn fn)
{
	command cmd = malloc (sizeof (struct command));
	if (cmd == nullptr) panic ("out of memory");
	memset (cmd, 0, sizeof (struct command));

	cmd->name = string_new (name);
	cmd->help = string_new (help);
	cmd->fn = fn;

	/* settings */
	cmd->settings = malloc (sizeof (map));
	if (cmd->settings == nullptr) panic ("out of memory");
	map_init (cmd->settings);

	/* error stack */
	cmd->errors = malloc (sizeof (stack));
	if (cmd->errors == nullptr) panic ("out of memory");
	stack_init (cmd->errors);

	/* options map */
	cmd->flags = malloc (sizeof (map));
	if (cmd->flags == nullptr) panic ("out of memory");
	map_init (cmd->flags);

	/* command map */
	cmd->commands = malloc (sizeof (map));
	if (cmd->commands == nullptr) panic ("out of memory");
	map_init (cmd->commands);

	return cmd;
}


void command_free (command cmd)
{
	if (cmd == nullptr) return;

	string_free (cmd->name);
	string_free (cmd->help);

	/* TODO: add back after testing */
	map *m;
	void **values;

	/* free settings */
	m = cmd->settings;
	values = malloc (sizeof (string) * map_size (m));
	map_values (m, values);
	for (int i = 0; i < map_size (m); i++) {
		string_free (values[i]);
	}
	map_free (m);

	/* shouldn't be any outstanding errors, but in case, free by printing */
	command_print_errors (cmd);
	stack_free (cmd->errors);


	/* free options */
	m = cmd->flags;
	values = malloc (sizeof (char *) * map_size (m));
	map_values (m, values);
	for (int i = 0; i < map_size (m); i++) {
		free (values[i]);
	}
	map_free (cmd->flags);

	/* free commands */
	m = cmd->commands;
	values = malloc (sizeof (char *) * map_size (m));
	map_values (m, values);
	for (int i = 0; i < map_size (m); i++) {
		free (values[i]);
	}
	map_free (cmd->commands);

	free (cmd);
}


void command_set (command cmd, const char *key, const char *value)
{
	map_put (cmd->settings, key, string_new (value));
}


string command_get (command cmd, const char *name)
{
	return map_get (cmd->settings, name);
}


void command_print_errors (command cmd)
{
	stack *errors = cmd->errors;
	size_t size = stack_size (errors);

	for (int i = 0; i < size; i++) {
		string error = stack_pop (errors);
		log_error (error);
		sdsfree (error);
	}
}


flag command_add_flag (command cmd, const char *name, const char *long_option,
		       char short_option, flag_arg has_arg)
{
	flag f = malloc (sizeof (struct flag));
	if (f == nullptr) panic ("out of memory");
	memset (f, 0, sizeof (struct flag));

	f->command = cmd;
	f->name = string_new (name);
	f->long_option = string_new (long_option);
	f->short_option = short_option;
	f->has_arg = has_arg;

	map_put (cmd->flags, name, f);
	return f;
}


void command_add_command (command cmd, const char *name, const char *help,
			  command_fn fn)
{

	command subcmd = command_new (name, help, fn);
	subcmd->parent = cmd;
	map_put (cmd->commands, name, subcmd);
}


static command parse_args (command cmd)
{
	int argc = cmd->argc;
	char **argv = cmd->argv;
	opterr = 0;
	int c;

	while (1) {
		int option_index = 0;
		/* TODO: add command and options dynamically */
		static struct option long_options[] = {
			{"version", no_argument, nullptr},
			{"help", required_argument, nullptr, 'h'},
			{"foo", no_argument},
			{}};

		c = getopt_long (argc, argv, ":h:v", long_options,
				 &option_index);

		if (c == -1) break;

		/* note: remember optarg holds an option argument */

		switch (c) {
		/* long options that don't have a corresponding short option */
		case 0: {
			const char *name = long_options[option_index].name;
			cmd = map_get (cmd->commands, name);
			if (cmd != nullptr) break;
			command_push_errorf (
				cmd, "missing long option handler for %s",
				name);
			break;
		}

		case 'v': {
			cmd = map_get (cmd->commands, "version");
			break;
		}

		case 'h': {
			cmd = map_get (cmd->commands, "help");
			break;
		}

		case '?': {
			command_push_errorf (cmd, "unknown option: %s",
					     argv[optind - 1]);
			break;
		}

		case ':': {
			command_push_errorf (cmd,
					     "missing option argument for: %s",
					     argv[optind - 1]);
			break;
		}

		default: {
			command_push_errorf (cmd,
					     "missing option handler for: %s",
					     argv[optind - 1]);
			break;
		}
		}
	}

	if (optind < argc) {
		if (cmd) {
			while (optind < argc) {
				vector_add (cmd->args, argv[optind++]);
			}
		} else {
			/* unhandled command or unexpected arguments */
			string err = string_format ("unknown command:");
			while (optind < argc) {
				err = string_cat_fmt (err, " %s",
						      argv[optind++]);
			}
			command_push_error (cmd, err);
		}
	}

	/* should never happen! */
	if (cmd == nullptr && stack_size (cmd->errors) == 0)
		command_push_error (cmd, "unknown error");

	return cmd;
}


bool command_run (command cmd, int argc, char **argv)
{
	cmd->argc = argc;
	cmd->argv = argv;

	if (!parse_args (cmd)) {
		return false;
	}

	return cmd->fn (cmd);
}
