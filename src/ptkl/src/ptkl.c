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
#include "qjs.h"
#include "qjsc.h"

#include "commands.h"

/*
 * To add a new command:
 *   1. Create a file under ./commands and add a callback function
 *      of type command_fn (defined in libcli/include/command.h)
 *   2. Add it to ./CMakeLists.txt
 *   3. Add the extern declaration below (sorted)
 *   4. Add the command in main()
 */

/* Main command configuration */
extern command main_command_new (const char *name, const char *group);

/* Subcommand configuration functions */
extern command compile_new (command parent, const char *group);
extern command console_new (command parent, const char *group);
extern command data_new (command parent, const char *group);
extern command help_new (command parent, const char *group);
extern command logs_new (command parent, const char *group);
extern command repl_new (command parent, const char *group);
extern command run_new (command parent, const char *group);
extern command serve_new (command parent, const char *group);
extern command service_new (command parent, const char *group);
extern command storage_new (command parent, const char *group);
extern command version_new (command parent, const char *group);

int main (const int argc, char **argv)
{
	/* Command group names */
	const char *GROUP_DEVELOPMENT = "Development";
	const char *GROUP_SERVICES = "Service Management";
	const char *GROUP_INTERACTIVE = "Interactive Tools";

	ptkl_init ();

	auto cmd = main_command_new (argv[0], NULL);

	run_new (cmd, GROUP_DEVELOPMENT);
	serve_new (cmd, GROUP_DEVELOPMENT);
	compile_new (cmd, GROUP_DEVELOPMENT);

	service_new (cmd, GROUP_SERVICES);
	storage_new (cmd, GROUP_SERVICES);
	data_new (cmd, GROUP_SERVICES);
	logs_new (cmd, GROUP_SERVICES);

	console_new (cmd, GROUP_INTERACTIVE);
	repl_new (cmd, GROUP_INTERACTIVE);

	bool ok = command_run (cmd, argc, argv);
	if (!ok) command_print_errors (cmd);

	/* TODO: handle atexit for cleanup if normal flow is short-circuited */
	command_free (cmd);
	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
