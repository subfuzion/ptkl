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
#include <string.h>
#include <unistd.h>

#include "cli.h"
#include "errors.h"
#include "ptkl.h"
#include "strings.h"


void help (cli cli)
{
	printf ("Partikle Runtime (version %s\n"
		"usage: %s [options] [file [args]]\n"
		"-v  --version              print version\n"
		"-h  --help                 show this help\n",
		cli->version, cli->name);
}


void version (cli cli)
{
	printf ("%s %s\n", cli->name, cli->version);
}


int main (const int argc, char **argv)
{
	int exit_code = EXIT_SUCCESS;
	register_signal_panic_handlers ();

	/* init cli */
	auto cmd_name = strdup (argv[0]);
	auto cmd_version = CONFIG_VERSION; /* from makefile */
	auto cmd_description =
		"Partikle is a lightweight runtime for modern JavaScript";
	auto cli = cli_new (cmd_name, cmd_version, cmd_description);

	/* add commands */
	cli_add_command (cli, "version", version);
	cli_add_command (cli, "help", help);

	/* parse command line */
	struct cli_parse_result result = {};
	cli_parse_args (argc, argv, cli, &result);
	if (!result.ok) {
		fprintf (stderr, "error: %s\n", result.error);
		exit_code = EXIT_FAILURE;
		goto done;
	}

	/* run command */
	struct command *cmd = result.cmd;
	if (cmd != nullptr && cmd->fn != nullptr) cmd->fn (cli);

done:
	cli_free (cli);
	return exit_code;
}
