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

#include <log.h>

#include "strings.h"


void help (command cmd)
{
	string name = cmd->name;
	string version = command_get (cmd, "version");

	printf ("Partikle Runtime (version %s)\n"
		"usage: %s [options] [file [args]]\n"
		"-v  --version              print version\n"
		"-h  --help                 show this help\n",
		version, name);
}


void help_flag (flag f)
{
	help (f->command);
}


/* Handle `version` command */
void version (command cmd)
{
	string version = command_get (cmd, "version");
	printf ("%s %s\n", cmd->name, version);
}


/* Handle `-v,--version` flag */
void version_flag (flag f)
{
	version (f->command);
}


void foo (flag f)
{
	printf ("eureka!\n");
}


void hello (command cmd)
{
	string name = vector_get (cmd->args, 0);
	if (name == nullptr) name = "world";
	printf ("Hello, %s!\n", name);
}

void default_command (command cmd)
{
	TRACE ("running default command");
	help (cmd);
	TRACE ("running default command: done");
}

void hello_foo_flag (flag f)
{
	printf ("hello foo!\n");
}

int main (const int argc, char **argv)
{
	ptkl_init ();

	auto name = argv[0];
	auto description = "Partikle is a lightweight runtime for the web";

	/* TODO: change the default function from help to repl when ready */
	auto cmd = command_new (name, description, default_command);
	command_set (cmd, "version", CONFIG_VERSION);

	flag f = command_add_flag (cmd, 'v', "version", NO_ARGUMENT,
				   "print version");
	flag_add_callback (f, version_flag, true);

	f = command_add_flag (cmd, 'h', "help", NO_ARGUMENT, "print help");
	flag_add_callback (f, help_flag, true);

	command_add_command (cmd, "version", "print version", version);
	command_add_command (cmd, "help", "show this help", help);

	/* test flag */
	flag eureka_flag =
		command_add_flag (cmd, 'f', "foo", NO_ARGUMENT, "print eureka");
	flag_add_callback (eureka_flag, foo, true);

	/* test command */
	command hello_cmd =
		command_add_command (cmd, "hello", "print hello", hello);
	command_expect_args (hello_cmd, 1);
	f = command_add_flag (hello_cmd, 'x', "xoo", NO_ARGUMENT, "print foo");
	flag_add_callback (f, hello_foo_flag, false);


	bool ok = command_run (cmd, argc, argv);
	if (!ok) command_print_errors (cmd);

	// command_free (cmd);
	TRACE ("%s", ok ? "OK" : "FAIL");
	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

/* TODO: handle atexit to clean up after handlers that short circuit normal flow
 */
