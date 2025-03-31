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

#include "command.h"
#include "ptkl.h"

#include "commands.h"

/*
 * To add a new command:
 *   1. Create a file under ./commands and add a callback function
 *      of type command_fn (defined in libcli/include/command.h)
 *   2. Add it to ./CMakeLists.txt
 *   3. Add the extern declaration below (sorted)
 *   4. Add the command in main()
 */
extern void compile (command cmd);
extern void console (command cmd);
extern void data (command cmd);
extern void logs (command cmd);
extern void repl (command cmd);
extern void run (command cmd);
extern void serve (command cmd);
extern void service (command cmd);
extern void storage (command cmd);

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

	/* subcommand group */
	command_add (cmd, "help", "print help", help);
	command_add (cmd, "version", "print version", version);

	/* subcommand group */
	command_add (cmd, "run", "run a JavaScript program", run);
	command_add (cmd, "compile", "compile a JavaScript program", compile);
	command_add (cmd, "serve", "serve the current program", serve);

	/* subcommand group */
	command_add (cmd, "service", "manage services (web, job, agent",
		     service);
	command_add (cmd, "storage", "manage storage (file)", service);
	command_add (cmd, "data", "manage data (kv, doc, sql)", data);
	command_add (cmd, "logs", "monitor and query logs", data);

	/* subcommand group */
	command_add (cmd, "console", "open the admin console", console);
	command_add (cmd, "repl", "start a JavaScript shell", repl);

	bool ok = command_run (cmd, argc, argv);
	if (!ok) command_print_errors (cmd);

	/* TODO: handle atexit for cleanup if normal flow is short-circuited */
	command_free (cmd);
	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
