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

#include "console.h"
#include "command.h"
#include "log.h"

static void handle_command (console c, const char *cmd)
{
	if (strcmp (cmd, "quit") == 0) {
		console_stop (c);
	} else {
		console_error (c, "Unknown command: %s", cmd);
	}
}

static void console_command (command cmd)
{
	console c = console_new ();
	if (!c) {
		LOG_ERROR ("Failed to create console");
		return;
	}

	if (!console_init (c)) {
		LOG_ERROR ("Failed to initialize console");
		console_free (c);
		return;
	}

	/* Set up command handler */
	console_set_command_handler (c, handle_command);
	console_show_command_bar (c, ">");

	/* Print welcome message */
	console_print (c, "Welcome to the ptkl console\n");
	console_print (c, "Type 'quit' to exit\n\n");

	/* Run the console */
	console_run (c);

	/* Clean up */
	console_cleanup (c);
	console_free (c);
}

command console_command_new (command parent, const char *group)
{
	command cmd = command_add (parent, "console", "open the admin console",
				   console_command);
	if (group != nullptr) command_set_group (cmd, group);
	return cmd;
}
