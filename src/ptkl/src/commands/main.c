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
#include <string.h>
#include "command.h"
#include "map.h"
#include "ptkl.h"
#include "vector.h"

static size_t get_max_width (command cmd)
{
	size_t max_width = 0;

	/* Check flag widths */
	for (int i = 0; i < vector_size (cmd->flags); i++) {
		flag f = vector_get (cmd->flags, i);
		size_t width = 4; /* -x + 2 spaces */
		if (f->long_flag) {
			width +=
				2 + strlen (f->long_flag); /* --, --long-flag */
		}
		if (width > max_width) max_width = width;
	}

	/* Check command widths */
	for (size_t i = 0; i < vector_size (cmd->ordered_commands); i++) {
		command subcmd = vector_get (cmd->ordered_commands, i);
		size_t width = strlen (subcmd->name) + 2; /* name + 2 spaces */
		if (width > max_width) max_width = width;
	}

	return max_width;
}

static void print_aligned (const char *name, const char *help, size_t width)
{
	printf ("  %s", name);
	size_t name_len = strlen (name);
	size_t padding = width - name_len + 2; /* Add 2 extra spaces */
	for (size_t i = 0; i < padding; i++) {
		putchar (' ');
	}
	printf ("%s\n", help);
}

static void help (command cmd)
{
	size_t width = get_max_width (cmd);

	printf ("Usage: %s [options] [command] [args]\n\n", cmd->name);
	printf ("Options:\n");
	for (int i = 0; i < vector_size (cmd->flags); i++) {
		flag f = vector_get (cmd->flags, i);
		char flag_str[64];
		if (f->long_flag) {
			snprintf (flag_str, sizeof (flag_str), "-%c, --%s",
				  f->short_flag, f->long_flag);
		} else {
			snprintf (flag_str, sizeof (flag_str), "-%c",
				  f->short_flag);
		}
		print_aligned (flag_str, f->help, width);
	}

	printf ("\nCommands:\n");
	/* First show commands with no group */
	for (size_t i = 0; i < vector_size (cmd->ordered_commands); i++) {
		command subcmd = vector_get (cmd->ordered_commands, i);
		if (subcmd->group == nullptr) {
			print_aligned (subcmd->name, subcmd->help, width);
		}
	}

	/* Track which groups we've shown */
	map *shown_groups = malloc (sizeof (map));
	map_init (shown_groups);

	/* Show commands by group */
	for (size_t i = 0; i < vector_size (cmd->ordered_commands); i++) {
		command subcmd = vector_get (cmd->ordered_commands, i);
		if (subcmd->group != nullptr &&
		    map_get (shown_groups, subcmd->group) == nullptr) {
			/* First time seeing this group, print header and all
			 * commands */
			printf ("\n%s:\n", subcmd->group);
			map_put (shown_groups, subcmd->group, (void *)1);

			/* Print all commands in this group */
			for (size_t j = 0;
			     j < vector_size (cmd->ordered_commands); j++) {
				command cmd2 =
					vector_get (cmd->ordered_commands, j);
				if (cmd2->group != nullptr &&
				    strcmp (cmd2->group, subcmd->group) == 0) {
					print_aligned (cmd2->name, cmd2->help,
						       width);
				}
			}
		}
	}

	map_free (shown_groups);
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
