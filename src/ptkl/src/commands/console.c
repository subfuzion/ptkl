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
#include "commands.h"
#include "log.h"
#include <string.h>

/* Available commands */
static const char *COMMANDS[] = {"clear", "help", "quit", "service", "storage", "data", "logs", NULL};

/* Get matching commands for completion */
static char **get_matching_commands(const char *prefix, int *count)
{
	if (!prefix) {
		prefix = "";
	}

	/* Count matching commands first */
	*count = 0;
	for (const char **cmd = COMMANDS; *cmd; cmd++) {
		if (strncmp(prefix, *cmd, strlen(prefix)) == 0) {
			(*count)++;
		}
	}

	if (*count == 0) return NULL;

	/* Allocate array for matches */
	char **matches = malloc(*count * sizeof(char*));
	if (!matches) {
		*count = 0;
		return NULL;
	}

	/* Fill matches array */
	int i = 0;
	for (const char **cmd = COMMANDS; *cmd; cmd++) {
		if (strncmp(prefix, *cmd, strlen(prefix)) == 0) {
			matches[i++] = strdup(*cmd);
		}
	}

	return matches;
}

/* Command completion callback */
static char **complete_command(console c, const char *prefix, int *count)
{
	return get_matching_commands(prefix, count);
}

/* Check if input matches a command uniquely */
static const char *match_command(const char *input)
{
	if (!input || !*input) return NULL;

	const char *match = NULL;
	size_t input_len = strlen(input);

	for (const char **cmd = COMMANDS; *cmd; cmd++) {
		if (strncmp(input, *cmd, input_len) == 0) {
			/* If we already found a match, this is ambiguous */
			if (match) return NULL;
			match = *cmd;
		}
	}

	return match;
}

static void handle_command (console c, const char *input)
{
	const char *cmd = match_command(input);
	if (!cmd) {
		console_error(c, "Unknown or ambiguous command: %s", input);
		return;
	}

	if (strcmp(cmd, "quit") == 0) {
		console_stop(c);
	} else if (strcmp(cmd, "clear") == 0) {
		console_clear(c);
	}
}

static void console_command (command cmd)
{
	/* Get version */
	const char *version = command_get(cmd, "version");

	console c = console_new ();
	if (!c) {
		LOG_ERROR ("Failed to create console");
		return;
	}

	/* Set title with version */
	char title[256];
	snprintf(title, sizeof(title), "Partikle Runtime %s", version);
	console_set_title(c, title);

	if (!console_init (c)) {
		LOG_ERROR ("Failed to initialize console");
		console_free (c);
		return;
	}

	/* Set up command handlers */
	console_set_command_handler(c, handle_command);
	console_set_completion_handler(c, complete_command);
	console_show_command_bar(c, ">");

	/* Print welcome message */
	console_print(c, "\n\n");

	/* Print Console Commands */
	console_print(c, "Console Commands:\n");
	console_print(c, "  clear      Clear the screen\n");
	console_print(c, "  help       Show help for commands\n");
	console_print(c, "  quit       Exit the console\n\n");

	/* Print Service Commands */
	console_print(c, "%s:\n", GROUP_SERVICES);
	console_print(c, "  service    Manage services\n");
	console_print(c, "  storage    Manage storage\n");
	console_print(c, "  data       Manage data\n");
	console_print(c, "  logs       View logs\n\n");

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
