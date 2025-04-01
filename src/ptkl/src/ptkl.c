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
extern command main_command_new (const char *name);

/* Subcommand configuration functions */
extern command compile_new (command parent);
extern command console_new (command parent);
extern command data_new (command parent);
extern command logs_new (command parent);
extern command repl_new (command parent);
extern command run_new (command parent);
extern command serve_new (command parent);
extern command service_new (command parent);
extern command storage_new (command parent);

int main (const int argc, char **argv)
{
	ptkl_init ();

	auto cmd = main_command_new (argv[0]);

	/* subcommand group */
	/* Command groups */
	run_new (cmd);
	compile_new (cmd);
	serve_new (cmd);

	/* Service management group */
	service_new (cmd);
	storage_new (cmd);
	data_new (cmd);
	logs_new (cmd);

	/* Development tools group */
	console_new (cmd);
	repl_new (cmd);

	bool ok = command_run (cmd, argc, argv);
	if (!ok) command_print_errors (cmd);

	/* TODO: handle atexit for cleanup if normal flow is short-circuited */
	command_free (cmd);
	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
