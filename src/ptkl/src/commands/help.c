/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 * Copyright (c) 2025 Tony Pujals
 */

#include <stdio.h>
#include <string.h>
#include "command.h"
#include "map.h"
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

void help (command cmd)
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

	/* First check if there are any ungrouped commands */
	bool has_ungrouped = false;
	for (size_t i = 0; i < vector_size (cmd->ordered_commands); i++) {
		command subcmd = vector_get (cmd->ordered_commands, i);
		if (subcmd->group == nullptr) {
			has_ungrouped = true;
			break;
		}
	}

	/* Then show ungrouped commands if any */
	if (has_ungrouped) {
		printf ("\nCommands:\n");
		for (size_t i = 0; i < vector_size (cmd->ordered_commands);
		     i++) {
			command subcmd = vector_get (cmd->ordered_commands, i);
			if (subcmd->group == nullptr) {
				print_aligned (subcmd->name, subcmd->help,
					       width);
			}
		}
	}

	/* Track which groups we've shown */
	map *shown_groups = malloc (sizeof (map));
	map_init (shown_groups);

	/* Then show commands by group */
	for (size_t i = 0; i < vector_size (cmd->ordered_commands); i++) {
		command subcmd = vector_get (cmd->ordered_commands, i);
		if (subcmd->group && !map_get (shown_groups, subcmd->group)) {
			printf ("\n%s:\n", subcmd->group);
			map_put (shown_groups, subcmd->group, (void *)1);

			/* Show all commands in this group */
			for (size_t j = 0;
			     j < vector_size (cmd->ordered_commands); j++) {
				command cmd2 =
					vector_get (cmd->ordered_commands, j);
				if (cmd2->group &&
				    strcmp (cmd2->group, subcmd->group) == 0) {
					print_aligned (cmd2->name, cmd2->help,
						       width);
				}
			}
		}
	}

	map_free (shown_groups);
	free (shown_groups);
}

static void help_flag (flag f)
{
	help (f->command);
}

command help_new (command parent, const char *group)
{
	auto help_cmd = command_add (parent, "help", "print help", help);
	command_expect_args (help_cmd, COMMAND_ARGS_ANY);
	if (group != nullptr) command_set_group (help_cmd, group);
	return help_cmd;
}

command help_flag_new (command parent)
{
	flag hf = command_flag (parent, 'h', "help", NO_ARGUMENT, "print help");
	flag_add_callback (hf, help_flag, true);
	return parent;
}
