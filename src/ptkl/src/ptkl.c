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

#include <stdio.h>

#include "command.h"
#include "ptkl.h"
#include "strings.h"


bool help (command cmd)
{
	string name = cmd->name;
	string version = command_get (cmd, "version");

	printf ("Partikle Runtime (version %s)\n"
		"usage: %s [options] [file [args]]\n"
		"-v  --version              print version\n"
		"-h  --help                 show this help\n",
		version, name);

	return true;
}


bool version (command cmd)
{
	string version = command_get (cmd, "version");
	printf ("%s %s\n", cmd->name, version);

	return true;
}


int main (const int argc, char **argv)
{
	ptkl_init ();

	auto name = argv[0];
	auto description = "Partikle is a lightweight runtime for the web";

	auto cmd = command_new (name, description, help);
	command_set (cmd, "version", CONFIG_VERSION);

	command_add_command (cmd, "version", "print version", version);
	command_add_command (cmd, "help", "show this help", help);

	bool ok = command_run (cmd, argc, argv);
	if (!ok) {
		command_print_errors (cmd);
	}

	// command_free (cmd);
	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
