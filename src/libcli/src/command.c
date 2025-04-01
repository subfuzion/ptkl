/*
 * ptkl command api
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

#include "command.h"
#include "log.h"
#include "strings.h"


command command_new (const char *name, const char *help, command_fn fn)
{
	command cmd = malloc (sizeof (struct command));
	if (cmd == nullptr) panic ("out of memory");
	memset (cmd, 0, sizeof (struct command));

	cmd->name = string_new (name);
	cmd->help = string_new (help);
	cmd->fn = fn;

	/* settings */
	cmd->settings = malloc (sizeof (map));
	if (cmd->settings == nullptr) panic ("out of memory");
	map_init (cmd->settings);

	/* error stack */
	cmd->errors = malloc (sizeof (stack));
	if (cmd->errors == nullptr) panic ("out of memory");
	stack_init (cmd->errors);

	/* options vector */
	cmd->flags = malloc (sizeof (vector));
	if (cmd->flags == nullptr) panic ("out of memory");
	vector_init (cmd->flags);

	/* command map and vector */
	cmd->commands = malloc (sizeof (map));
	if (cmd->commands == nullptr) panic ("out of memory");
	map_init (cmd->commands);

	cmd->ordered_commands = malloc (sizeof (vector));
	if (cmd->ordered_commands == nullptr) panic ("out of memory");
	vector_init (cmd->ordered_commands);

	/* args vector */
	cmd->args = malloc (sizeof (vector));
	if (cmd->args == nullptr) panic ("out of memory");
	vector_init (cmd->args);

	return cmd;
}


void command_free (command cmd)
{
	if (cmd == nullptr) return;

	string_free (cmd->name);
	string_free (cmd->help);

	map *m;
	void **values;

	/* free settings */
	m = cmd->settings;
	values = malloc (sizeof (string) * map_size (m));
	map_values (m, values);
	for (int i = 0; i < map_size (m); i++) {
		string_free (values[i]);
	}
	map_free (m);

	/* shouldn't be any outstanding errors, but in case, free by printing */
	command_print_errors (cmd);
	stack_free (cmd->errors);


	/* free flags */
	for (int i = 0; i < vector_size (cmd->flags); i++) {
		flag f = vector_get (cmd->flags, i);
		string_free (f->long_flag);
		string_free (f->help);
		free (f);
	}
	vector_free (cmd->flags);

	/* free commands */
	m = cmd->commands;
	values = malloc (sizeof (char *) * map_size (m));
	map_values (m, values);
	for (int i = 0; i < map_size (m); i++) {
		/* this vector always stories a copy of the value */
		free (values[i]);
	}
	map_free (cmd->commands);
	free (values);

	/* free ordered commands vector - no need to free values since they're
	 * the same command objects stored in the map above and already freed */
	vector_free (cmd->ordered_commands);

	free (cmd);
}


void command_set (command cmd, const char *key, const char *value)
{
	/* always duplicate value, this vector frees */
	map_put (cmd->settings, key, string_new (value));
}


string command_get (command cmd, const char *name)
{
	while (cmd != nullptr) {
		string value = map_get (cmd->settings, name);
		if (value != nullptr) return value;
		cmd = cmd->parent;
	}
	return nullptr;
}


void command_expect_args (command cmd, int count)
{
	cmd->expect_args = count;
}


void command_print_errors (command cmd)
{
	stack *errors = cmd->errors;
	size_t size = stack_size (errors);

	for (int i = 0; i < size; i++) {
		string error = stack_pop (errors);
		log_error (error);
		sdsfree (error);
	}
}


static flag find_flag (command cmd, char short_flag, const char *long_flag)
{
	for (int i = 0; i < vector_size (cmd->flags); i++) {
		flag f = vector_get (cmd->flags, i);
		if (f->short_flag == short_flag ||
		    (long_flag != nullptr &&
		     strcmp (long_flag, f->long_flag) == 0)) {
			return f;
		}
	}
	return nullptr;
}

flag command_flag (command cmd, char short_option, const char *long_option,
		   flag_arg has_arg, const char *help)
{
	flag f = malloc (sizeof (struct flag));
	if (f == nullptr) panic ("out of memory");
	memset (f, 0, sizeof (struct flag));

	f->command = cmd;
	f->long_flag = string_new (long_option);
	f->short_flag = short_option;
	f->has_arg = has_arg;
	f->help = string_new (help);

	vector_add (cmd->flags, f);
	return f;
}


void flag_add_callback (flag f, flag_fn fn, bool should_exit)
{
	f->fn = fn;
	f->should_exit = should_exit;
}


command command_add (command cmd, const char *name, const char *help,
		     command_fn fn)
{
	command subcmd = command_new (name, help, fn);
	subcmd->parent = cmd;
	map_put (cmd->commands, name, subcmd);
	vector_add (cmd->ordered_commands, subcmd);
	return subcmd;
}


void command_push_error (command cmd, const char *error)
{
	stack_push (cmd->errors, string_new (error));
}


void command_push_error_string (command cmd, string error)
{
	stack_push (cmd->errors, error);
}


void command_push_errorf (command cmd, const char *fmt, ...)
{
	va_list (args);
	va_start (args, fmt);
	command_push_error (cmd, sdscatvprintf (sdsempty (), fmt, args));
	va_end (args);
}


/**
 * Transforms command flags into a format suitable for calling getopt_long().
 *
 * For example:
 *
 *     struct option long_options[] = {
 *             { "version",  no_argument,        nullptr,  'v' },
 *             { "help",     optional_argument,  nullptr,  'h' },
 *             { "foo",      required_argument,  nullptr,  'f' },
 *             {},
 *     };
 *
 *     char *short_options = ":vhf:";
 *
 *    int optindex;
 *    int c = getopt_long (argc, argv, short_options, long_options, &optindex);
 *
 *    Reference:
 *    https://man7.org/linux/man-pages/man3/getopt.3.html
 */

struct getopt_options {
	struct option *long_options;
	string short_options;
};

struct getopt_options *new_getopt_options (command cmd)
{
	struct getopt_options *options =
		malloc (sizeof (struct getopt_options));
	if (options == nullptr) panic ("out of memory");
	memset (options, 0, sizeof (struct getopt_options));

	size_t flag_count = vector_size (cmd->flags);

	/* prepare long_options to store results of flags enumeration */
	/* add room for terminating empty option */
	size_t total_count = flag_count + 1;
	struct option *long_options =
		malloc (sizeof (struct option) * total_count);
	if (long_options == nullptr) panic ("out of memory");
	memset (long_options, 0, sizeof (struct option) * total_count);

	string short_options = string_new (":");

	/* transform flags to long_options */
	int long_option_count = 0;
	for (int i = 0; i < flag_count; i++) {
		flag f = vector_get (cmd->flags, i);
		char *long_option = f->long_flag;
		char short_option = f->short_flag;
		flag_arg has_arg = f->has_arg;

		/*
		if (long_option != nullptr) {
			printf ("flag: long: %s, short: %c, has_arg: %d\n",
				long_option, short_option, has_arg);
		}
		*/


		long_options[i] = (struct option){long_option, short_option,
						  nullptr, has_arg};
		long_option_count++;

		if (short_option != 0) {
			short_options = string_cat_fmt (short_options, "%c",
							short_option);
			if (has_arg == REQUIRED_ARGUMENT) {
				short_options = string_cat (short_options, ":");
			} else if (has_arg == OPTIONAL_ARGUMENT) {
				short_options =
					string_cat (short_options, "::");
			}
		}


		TRACE ("short_options: \"%s\"\n", short_options);
	}

	/* terminate long_options for getopt */
	long_options[long_option_count] = (struct option){};

	/* store results */
	options->long_options = long_options;
	options->short_options = short_options;

	return options;
}

void free_getopt_options (struct getopt_options *opts)
{
	if (opts == nullptr) return;
	if (opts->long_options != nullptr) free (opts->long_options);
	if (opts->short_options != nullptr) string_free (opts->short_options);
	free (opts);
}


bool command_run (command cmd, int argc, char **argv)
{
	bool ok = false;
	opterr = 0;
	int c = 0;
	flag f = nullptr;

	/* TODO: getopt mutates argv, so save a copy for handing off if we fork
	 */
	cmd->argc = argc;
	cmd->argv = argv;

	/* collect any unhandled flags in case they apply to a subcommand */
	vector unhandled_flags;
	vector_init (&unhandled_flags);

	/* flag handlers to run after parsing all the args */
	vector pending_flag_handlers;
	vector_init (&pending_flag_handlers);

	/* transform command flags to getopt options */
	struct getopt_options *options = new_getopt_options (cmd);

	TRACE ("start parse loop");
	while (1) {
		int long_options_i;
		struct option *long_options = options->long_options;
		string short_options = options->short_options;

		c = getopt_long (argc, argv, short_options, long_options,
				 &long_options_i);

		/* No more options */
		if (c == -1) break;

		/* If the short option has a handler, invoke it */
		f = find_flag (cmd, c, nullptr);
		TRACE ("look up short option: '%c' (%s)", c, c == 0 ? "0" : "");
		if (f != nullptr && f->fn != nullptr) {
			TRACE ("add pending flag handler for: %c", c);
			vector_add (&pending_flag_handlers, f);
			continue;
		}

		switch (c) {
		case 0: {
			TRACE ("case 0");
			/*
			 * Check the long option. If it has a corresponding
			 * short option, then the handler was already invoked
			 * above.
			 *
			 * TODO: add support for repeating flags
			 *
			 * When we reach this point, the following name and
			 * flag pointers are guaranteed to not be null (but
			 * still need to confirm the flag handler pointer).
			 */
			const char *name = long_options[long_options_i].name;
			TRACE ("long option: %s", name);
			f = find_flag (cmd, 0, name);
			// if (f->short_flag == 0 && f->fn != nullptr) {
			TRACE ("add pending flag handler for: %s", name);
			vector_add (&pending_flag_handlers, f);
			//}
			TRACE ("case 0 break");
			break;
		}

		case '?': {
			TRACE ("case ?");
			/* unhandled option */
			vector_add (&unhandled_flags, argv[optind - 1]);
			TRACE ("case ? break");
			break;
		}

		case ':': {
			trace ("case :");
			/* TODO: optarg holds an option argument */
			command_push_errorf (
				cmd, "missing expected argument for option: %s",
				argv[optind - 1]);

			trace ("case : goto done");
			goto done;
		}

		default: {
			trace ("case default");
			command_push_errorf (cmd, "unexpected: %s",
					     argv[optind - 1]);
			trace ("case default goto done");
			goto done;
		}
		}
	}

	TRACE ("done with parse loop");

	/* Collect remaining non-option args to pass them to this command or to
	 * a subcommand (along with any unhandled flags) */
	vector args;
	vector_init (&args);

	while (optind < argc) {
		char *arg = argv[optind++];
		vector_add (&args, arg);
		TRACE ("collecting args: %s", arg);
	}

	/*
	 * The command's argv has been parsed, so if there are remaining args or
	 * unhandled flags:
	 *
	 * - If there are any unhandled flags, there must be a subcommand that
	 *   can handle them, otherwise it's an error.
	 *
	 * - If the first of the remaining args matches a subcommand name, then
	 *   set up the subcommand's argc, argv, and call parse_args
	 *
	 * - If there is no match for a subcommand, then as long as there are
	 *   no unhandled flags, call this command's handler with the remaining
	 *   args. If the command doesn't expect any args, or if the args are
	 *   otherwise not valid, the command is responsible for reporting the
	 *   error.
	 *
	 * - Otherwise, report an error for the unhandled flags.
	 */

	bool has_subcommands = map_size (cmd->commands) > 0;
	bool has_unhandled_flags = vector_size (&unhandled_flags) > 0;
	bool has_args = vector_size (&args) > 0;

	TRACE ("has_subcommands: %s", has_subcommands ? "true" : "false");
	TRACE ("has_unhandled_flags: %s",
	       has_unhandled_flags ? "true" : "false");
	TRACE ("has_args: %s", has_args ? "true" : "false");

	/* Run the command */
	if (!has_unhandled_flags && !has_args) {
		TRACE ("no unhandled flags and no args, goto run");
		goto run;
	}

	/* If nothing to process unhandled flags, error */
	if (has_unhandled_flags && !has_subcommands) {
		TRACE ("unhandled flags and no subcommands, push error and "
		       "goto done");
		command_push_errorf (cmd, "unknown option: %s",
				     vector_get (&unhandled_flags, 0));
		goto done;
	}

	/* If unhandled flags and no args to match against a subcommand, error
	 */
	if (has_unhandled_flags && !has_args) {
		TRACE ("unexpected option: %s",
		       vector_get (&unhandled_flags, 0));
		command_push_errorf (cmd, "unexpected option: %s",
				     vector_get (&unhandled_flags, 0));
		goto done;
	}

	/* check if first arg matches a subcommand, if so, run that */
	TRACE ("has_args: %s", has_args ? "true" : "false");
	TRACE ("map_get");
	TRACE ("size of args: %lu", vector_size (&args));
	command subcmd = map_get (cmd->commands, vector_get (&args, 0));
	TRACE ("DONE map_get");
	TRACE ("there are args, first check to see if first arg matches a "
	       "subcommand");
	if (subcmd != nullptr) {
		TRACE ("subcommand match: %s", subcmd->name);

		/* get total count of flags and args to pass to subcommand */
		int sub_argc = (int)vector_size (&args);
		sub_argc += (int)vector_size (&unhandled_flags);

		/* prepare to run subcommand */
		char **sub_argv = (char **)malloc (sub_argc * sizeof (char *));
		if (sub_argv == nullptr) panic ("out of memory");
		memset (sub_argv, 0, sub_argc * sizeof (char *));

		/* increment index across loops */
		int index = 0;

		/* add args */
		for (int i = 0; i < vector_size (&args); i++) {
			char *arg = vector_get (&args, i);
			sub_argv[index++] = arg;
		}

		/* add options (continue index from previous loop) */
		for (int i = 0; i < vector_size (&unhandled_flags); i++) {
			char *opt = vector_get (&unhandled_flags, i);
			sub_argv[index++] = opt;
		}

		/* run the subcommand */
		TRACE ("running subcommand: %s", subcmd->name);
		optind = 0;
		ok = command_run (subcmd, sub_argc, sub_argv);
		if (!ok) {
			TRACE ("subcommand %s failed, printing errors, goto "
			       "done",
			       subcmd->name);
			command_print_errors (subcmd);
		}
		goto done;
	}

	/* Since no subcommand handled the args, add the args to this command */
	TRACE ("no matching subcommand, check if this command expects any "
	       "args");
	if (cmd->expect_args == 0) {
		TRACE ("the command doesn't expect any args, so add error and "
		       "goto done");
		command_push_errorf (cmd, "unexpected argument: %s",
				     vector_get (&args, 0));
		goto done;
	}
	if (cmd->expect_args > 0 && vector_size (&args) > cmd->expect_args) {
		TRACE ("too many args, expected up to %d, got %d, add error "
		       "and goto done",
		       cmd->expect_args, vector_size (&args));
		command_push_errorf (cmd,
				     "too many arguments (expected up "
				     "to %d, got %d)",
				     cmd->expect_args, vector_size (&args));
		goto done;
	}
	for (int i = 0; i < vector_size (&args); i++) {
		TRACE ("  adding: %s\n", vector_get (&args, i));
		vector_add (cmd->args, vector_get (&args, i));
	}

	vector_free (&args);

run:
	TRACE ("run: running command: %s", cmd->name);

	/* If we reached this point, we're almost ready to run the command */

	/* 1. Run any pending flag callbacks */
	TRACE ("  1. run any pending callbacks for: %s", cmd->name);
	for (int i = 0; i < vector_size (&pending_flag_handlers); i++) {
		f = vector_get (&pending_flag_handlers, i);
		/* TODO: still need to ensure optarg gets added to f->arg */
		if (f->fn != nullptr) {
			TRACE ("    running: -%c, --%s", f->short_flag,
			       f->long_flag);
			f->fn (f);
			if (f->should_exit) {
				ok = true;
				goto done;
			}
		}
	}

	/* 2. Check that there are no reported errors */
	TRACE ("  2. check for reported errors for: %s", cmd->name);
	if (stack_size (cmd->errors) > 0) {
		TRACE ("    there are errors (%d), goto done",
		       stack_size (cmd->errors));
		goto done;
	}

	/* 3. Run the command. */
	TRACE ("  3. run the command: %s (valid fn: %s)", cmd->name,
	       cmd->fn != nullptr ? "true" : "false");
	if (cmd->fn != nullptr) {
		cmd->fn (cmd);
	}

	/* 4. Check again that there are no reported errors */
	TRACE ("  4. check again for reported errors for: %s", cmd->name);
	if (stack_size (cmd->errors) > 0) {
		TRACE ("    there are errors (%d), goto done",
		       stack_size (cmd->errors));
		goto done;
	}

	/* Success! */
	TRACE ("success!");
	ok = true;

done:
	TRACE ("done: clean up, and return if ok: %s", ok ? "true" : "false");
	vector_free (&unhandled_flags);
	vector_free (&pending_flag_handlers);
	free_getopt_options (options);

	return ok;
}
