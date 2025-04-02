/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 *
 * Copyright (c) 2025 Tony Pujals
 */

#include "command.h"
#include "ptkl.h"

extern command help_new (command parent, const char *group);
extern command version_new (command parent, const char *group);
extern command help_flag_new (command parent);
extern command version_flag_new (command parent);
extern void help (command cmd);

static void default_command (command cmd)
{
	help (cmd);
}

command main_command_new (const char *name)
{
	auto description = "Partikle is a lightweight runtime for the web";
	command cmd = command_new (name, description, default_command);
	command_set (cmd, "version", CONFIG_VERSION);

	/* Configure flags */
	version_flag_new (cmd);
	help_flag_new (cmd);

	/* Add built-in commands */
	help_new (cmd, NULL);
	version_new (cmd, NULL);

	return cmd;
}
