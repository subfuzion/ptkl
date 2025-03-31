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
 * The above copyright notice and this permissio  n notice shall be included in
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

#include "ptkl.h"
#include "command.h"

#include "commands.h"

int main (const int argc, char **argv)
{
	ptkl_init ();

	auto name = argv[0];
	auto description = "Partikle is a lightweight runtime for the web";

	auto cmd = command_new (name, description, default_command);
	command_set (cmd, "version", CONFIG_VERSION);

	flag vf = command_flag (cmd, 'v', "version", NO_ARGUMENT,
				"print version");
	flag_add_callback (vf, version_flag, true);

	flag hf = command_flag (cmd, 'h', "help", NO_ARGUMENT, "print help");
	flag_add_callback (hf, help_flag, true);

	command_add (cmd, "version", "print version", version);
	command_add (cmd, "help", "show this help", help);

	bool ok = command_run (cmd, argc, argv);
	if (!ok) command_print_errors (cmd);

	command_free (cmd);
	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

/* TODO: handle atexit to ensure clean up if normal flow is short-circuited */
