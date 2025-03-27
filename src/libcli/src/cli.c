/*
 * ptkl command line args
 *
 * Copyright (c) 2017-2024 Fabrice Bellard
 * Copyright (c) 2017-2024 Charlie Gordon
 * Copyright (c) 2025 Tony Pujals
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
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "errors.h"
#include "strings.h"

cli cli_new (const char *name, const char *version, const char *description)
{
	cli cli = malloc (sizeof (struct cli));
	if (cli == nullptr) panic ("out of memory");
	memset (cli, 0, sizeof (struct cli));

	cli->name = string_new (name);
	cli->version = string_new (version);
	cli->description = string_new (description);

	/* command map */
	cli->commands = malloc (sizeof (map));
	if (cli->commands == nullptr) panic ("out of memory");
	map_init (cli->commands);

	return cli;
}


void cli_free (cli cli)
{
	if (cli == nullptr) return;
	string_free (cli->name);
	string_free (cli->version);
	string_free (cli->description);

	/* free commands, then free command map */
	void **values = malloc (sizeof (char *) * map_size (cli->commands));
	map_values (cli->commands, values);
	for (int i = 0; i < map_size (cli->commands); i++) {
		struct command *cmd = values[i];
		command_free (cmd);
	}
	map_free (cli->commands);

	free (cli);
}


command command_new (const char *name, command_fn fn)
{
	command cmd = malloc (sizeof (struct command));
	if (cmd == nullptr) panic ("out of memory");
	memset (cmd, 0, sizeof (struct command));
	cmd->name = string_new (name);
	cmd->fn = fn;
	return cmd;
}


void command_free (command cmd)
{
	if (cmd == nullptr) return;
	string_free (cmd->name);
	vector_free (cmd->args);
	free (cmd);
}


void cli_add_command (cli cli, const char *name, command_fn fn)
{
	command cmd = command_new (name, fn);
	map_put (cli->commands, name, cmd);
}


void cli_parse_args (int argc, char **argv, cli cli,
		     struct cli_parse_result *result)
{
	opterr = 0;
	int c;
	struct command *cmd;

	while (1) {
		cmd = nullptr;
		int option_index = 0;
		static struct option long_options[] = {
			{"version", no_argument, nullptr, 'v'},
			{"help", no_argument, nullptr, 'h'},
			{}};

		c = getopt_long (argc, argv, ":hv", long_options,
				 &option_index);
		if (c == -1) break;

		switch (c) {
		/* handle long options that aren't handled by a short option */
		case 0:
			printf ("TODO: handle long option %s",
				long_options[option_index].name);
			if (optarg) printf (" with arg %s", optarg);
			printf ("\n");
			break;

		case 'v':
			cmd = map_get (cli->commands, "version");
			result->cmd = cmd;
			break;

		case 'h':
			cmd = map_get (cli->commands, "help");
			result->cmd = cmd;
			break;

		/* unknown option */
		case '?':
			panic (string_fmt ("unknown option: %s",
					   argv[optind - 1]));

		/* missing option arg */
		case ':':
			panic (string_fmt ("missing option argument for: %s",
					   argv[optind - 1]));

		/* unhandled option */
		default:
			panic (string_fmt ("missing option handler for: %s",
					   argv[optind - 1]));
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
