/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 * Copyright (c) 2025 Tony Pujals
 */

#include <stdio.h>
#include "command.h"

static void version (command cmd)
{
	printf ("%s version %s\n", cmd->name, command_get (cmd, "version"));
}

static void version_flag (flag f)
{
	version (f->command);
}

command version_new (command parent, const char *group)
{
	auto cmd = command_add (parent, "version", "print version", version);
	if (group) command_set_group (cmd, group);
	return cmd;
}

command version_flag_new (command parent)
{
	flag vf = command_flag (parent, 'v', "version", NO_ARGUMENT,
				"print version");
	flag_add_callback (vf, version_flag, true);
	return parent;
}
