/*
 * ptkl - Partikle Runtime
 *
 * MIT License
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

#include <stdio.h>
#include "command.h"
#include "map.h"
#include "ptkl.h"
#include "vector.h"

static void help (command cmd)
{
	printf ("Usage: %s [options] [command] [args]\n\n", cmd->name);
	printf ("Options:\n");
	for (int i = 0; i < vector_size (cmd->flags); i++) {
		flag f = vector_get (cmd->flags, i);
		if (f->long_flag) {
			printf ("  -%c, --%s\t%s\n", f->short_flag,
				f->long_flag, f->help);
		} else {
			printf ("  -%c\t%s\n", f->short_flag, f->help);
		}
	}
	printf ("\nCommands:\n");
	size_t num_commands = map_size (cmd->commands);
	char **keys = malloc (num_commands * sizeof (char *));
	void **values = malloc (num_commands * sizeof (void *));
	map_items (cmd->commands, keys, values);
	for (size_t i = 0; i < num_commands; i++) {
		command subcmd = values[i];
		printf ("  %s\t%s\n", subcmd->name, subcmd->help);
	}
	free (keys);
	free (values);
}

static void version (command cmd)
{
	printf ("%s version %s\n", cmd->name, command_get (cmd, "version"));
}

static void default_command (command cmd)
{
	help (cmd);
}

static void help_flag (flag f)
{
	help (f->command);
}

static void version_flag (flag f)
{
	version (f->command);
}

command main_command_new (const char *name)
{
	auto description = "Partikle is a lightweight runtime for the web";
	command cmd = command_new (name, description, default_command);
	command_set (cmd, "version", CONFIG_VERSION);

	/* Configure flags */
	flag vf = command_flag (cmd, 'v', "version", NO_ARGUMENT,
				"print version");
	flag_add_callback (vf, version_flag, true);

	flag hf = command_flag (cmd, 'h', "help", NO_ARGUMENT, "print help");
	flag_add_callback (hf, help_flag, true);

	/* Add built-in commands */
	auto help_cmd = command_add (cmd, "help", "print help", help);
	command_expect_args (help_cmd, COMMAND_ARGS_ANY);

	command_add (cmd, "version", "print version", version);

	return cmd;
}
