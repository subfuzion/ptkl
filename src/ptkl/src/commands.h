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

#ifndef COMMANDS_H
#define COMMANDS_H

#include "command.h"
#include "log.h"
#include "strings.h"

/* TODO: refactor (command help can now be auto-generated) */
static inline void help (command cmd)
{
	string name = cmd->name;
	string version = command_get (cmd, "version");

	printf ("Partikle Runtime (version %s)\n"
		"\n"
		"Usage: %s [OPTIONS] [COMMANDS]\n"
		"\n"
		"Options:\n"
		"  -v  --version      print version\n"
		"  -h  --help         print help\n"
		"\n"
		"Commands:\n"
		"  help               print help\n"
		"  version            print version\n"
		"\n"
		"  run                run a JavaScript program\n"
		"  compile            compile a JavaScript program\n"
		"  serve              serve the current program\n"
		"\n"
		"  service            manage services (web, job, agent, flow)\n"
		"  storage            manage data (file, kv, sql)\n"
		"\n"
		"Interactive:\n"
		"  console            open the admin console\n"
		"  repl               start a JavaScript shell\n"
		"\n",
		version, name);
}


/* Delegate `-h,--help` flags to `help` command */
static inline void help_flag (flag f)
{
	help (f->command);
}


/* Handle `version` command */
static inline void version (command cmd)
{
	string version = command_get (cmd, "version");
	printf ("%s %s\n", cmd->name, version);
}


/* Delegate `-v,--version` flags to `version` command  */
static inline void version_flag (flag f)
{
	version (f->command);
}


/* Delegate default command to help */
static inline void default_command (command cmd)
{
	/* TODO: start REPL instead of printing help when ready */
	help (cmd);
}

#endif /* COMMANDS_H */
