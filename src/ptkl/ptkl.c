/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 *
 * Copyright (c) 2025 Tony Pujals
 * Copyright (c) 2017-2024 Charlie Gordon
 * Copyright (c) 2017-2024 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ptkl.h"

#include <errors.h>

#include "dstring.h"
#include "map.h"
#include "vector.h"

#include <string.h>

typedef void (*command_fn) ();

typedef struct command {
	char *name;
	command_fn fn;
	vector *args;
} *command;


command command_new (const char *name, command_fn fn)
{
	command cmd = malloc (sizeof (struct command));
	if (cmd == nullptr) panic ("out of memory");
	memset (cmd, 0, sizeof(struct command));
	cmd->name = strdup (name);
	cmd->fn = fn;
	return cmd;
}


void command_free (command cmd)
{
	if (cmd == nullptr) return;
	if (cmd->name != nullptr) free (cmd->name);
	if (cmd->args != nullptr) vector_free (cmd->args);
	free (cmd);
}


typedef struct cli {
	map *commands;
} *cli;


cli cli_new ()
{
	cli cli = malloc (sizeof (struct cli));
	if (cli == nullptr) panic ("out of memory");
	memset (cli, 0, sizeof(struct cli));
	cli->commands = malloc (sizeof (map));
	if (cli->commands == nullptr) panic ("out of memory");
	map_init (cli->commands);
	return cli;
}


void cli_free (cli cli)
{
	if (!cli) return;
	void **values = malloc (sizeof (char *) * map_size (cli->commands));
	map_values (cli->commands, values);
	for (int i = 0; i < map_size (cli->commands); i++) {
		struct command *cmd = values[i];
		free (cmd);
	}
	map_free (cli->commands);
	free (cli);
}


void cli_add_command (cli cli, const char *name, command_fn fn)
{
	command cmd = command_new (name, fn);
	map_put (cli->commands, name, cmd);
}


void help ()
{
	printf ("Partikle Runtime (version " CONFIG_VERSION ")\n"
		"usage: " PTKL " [options] [file [args]]\n"
		"-v  --version              print version\n"
		"-h  --help                 show this help\n");
}


void version ()
{
	printf ("%s %s\n", PTKL, CONFIG_VERSION);
}


struct parse_result {
	bool ok;

	union {
		char error[100];
		struct command *cmd;
	};
};


static void parse_args (const int argc, char **argv, const cli cli, struct parse_result *result)
{
	opterr = 0;
	int c;
	struct command *cmd;

	while (1) {
		cmd = nullptr;
		int option_index = 0;
		static struct option long_options[] = {
			{"version", no_argument, 0, 'v'},
			{"help", no_argument, 0, 'h'},
			{0, 0, 0, 0}
		};

		c = getopt_long (argc, argv, ":hv",
				 long_options, &option_index);
		if (c == -1) break;

		switch (c) {
		/* handle long options that aren't handled by a short option */
		case 0: printf ("TODO: handle long option %s", long_options[option_index].name);
			if (optarg) printf (" with arg %s", optarg);
			printf ("\n");
			break;

		case 'v': cmd = map_get (cli->commands, "version");
			result->cmd = cmd;
			break;

		case 'h': cmd = map_get (cli->commands, "help");
			result->cmd = cmd;
			break;

		/* unknown option */
		case '?': result->ok = false;
			sprintf (result->error, "unknown option: %s", argv[optind-1]);
			return;

		/* missing option arg */
		case ':': result->ok = false;
			sprintf (result->error, "missing option argument for: %s", argv[optind-1]);
			return;

		/* unhandled option */
		default: result->ok = false;
			sprintf (result->error, "missing option handler for: %s", argv[optind-1]);
			return;
		}
	}

	if (optind < argc) {
		printf ("non-option ARGV-elements: ");
		if (cmd) {
			while (optind < argc) {
				vector_add (cmd->args, argv[optind++]);
			}
		}
		while (optind < argc) printf ("%s ", argv[optind++]);
		printf ("\n");
	}

	result->ok = true;
}


int main (const int argc, char **argv)
{
	int exit_code;
	register_signal_panic_handlers();

	cli cli = cli_new();
	cli_add_command (cli, "version", version);
	cli_add_command (cli, "help", help);

	struct parse_result result = {};
	parse_args (argc, argv, cli, &result);

	if (!result.ok) {
		fprintf (stderr, "error: %s\n", result.error);
		exit_code = EXIT_FAILURE;
		goto done;
	}

	struct command *cmd = result.cmd;
	if (cmd->fn) cmd->fn();
	exit_code = EXIT_SUCCESS;

done: cli_free (cli);
	return exit_code;
}


