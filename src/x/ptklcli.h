/*
 * ptkl command line args
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
#ifndef ARGS_H
#define ARGS_H

#include "quickjs.h"


struct cliconfig {
	int argc;
	char **argv;

	const char *version;
	const char *usage;

	JSRuntime *js_runtime;
	JSContext *js_context;

	void (*initializer) (struct cliconfig *config);
	void (*finalizer) (struct cliconfig *config);
};

void run (struct cliconfig *config);

struct cli;

typedef void (*cli_fn) (struct cli *cli);

typedef void (*cli_exit_fn) (struct cli *cli);

struct cli {
	struct cliconfig *config;

	int exit_status;

	cli_fn start;
	cli_exit_fn stop;
	cli_exit_fn help;
	cli_exit_fn version;

	cli_fn onsuccess;
	cli_fn onfailure;
};

struct common_opts {};

struct runtime_opts {
	char *expr;
	int interactive;
	int dump_memory;
	int trace_memory;
	int empty_run;
	int module;
	int load_std;
	int dump_unhandled_promise_rejection;
	size_t memory_limit;
	char *include_list[32];
	int include_count;
	size_t stack_size;
	int bignum_ext;
};

struct compiler_opts {};

void cli_init (struct cli *cli, struct cliconfig *config);

int parse_runtime_args (int argc, char **argv, struct runtime_opts *opts);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// forward declarations
enum ptkl_parse_type;
union ptkl_parse_value;
struct ptkl_error;
struct ptkl_context;
struct ptkl_cli;
struct ptkl_arg;
struct ptkl_option;
struct ptkl_command;

// Determines how to parse a command line string
enum ptkl_parse_type {
	OPT_STRING,
	OPT_BOOL,
	OPT_INT,
};

// Stores the value of the parsed command line string
union ptkl_parse_value {
	const char *string;
	bool boolean;
	int integer;

	// Used when `multi` is set for arguments and options
	union ptkl_parse_value *next;
};

// Stores an error message (such as a parse error)
struct ptkl_error {
	const char *msg;

	// internal
	struct ptkl_command_error *next;
};

// Context for command handler functions
struct ptkl_context {
	struct ptkl_cli *cli;
	struct ptkl_command *command;
	int argc;
	const char **argv;
	struct ptkl_error *errors;
};

// The top level CLI
struct ptkl_cli {
	const char *name;
	const char *version;

	void (*run) (struct ptkl_cli *cli, int argc, char **argv);

	// internal
	struct ptkl_command *command;
};

// A CLI command
struct ptkl_command {
	const char *name;
	const char *help;
	const char *category;
	const char *example;

	void (*handler) (struct ptkl_context *ctx);

	// internal
	struct ptkl_command *next;
	struct ptkl_command *parent;
	struct ptkl_command *subcommand;
	struct ptkl_option *options;
	struct ptkl_arg *args;
	char *usage;
};

// A command option
struct ptkl_option {
	const char short_opt;
	const char *long_opt;
	const char *help;
	bool multi;
	enum ptkl_parse_type type;

	// Set after parsing the command line
	union ptkl_parse_value value;

	// internal
	struct ptkl_command *command;
	struct ptkl_option *next;
};

// A command argument
struct ptkl_arg {
	const char *name;
	bool optional;
	bool multi;
	enum ptkl_parse_type type;

	// Set after parsing the command line
	union ptkl_parse_value value;

	// internal
	struct ptkl_arg *next;
};

void ptkl_cli_add_command (struct ptkl_cli *cli, struct ptkl_command *cmd);
void ptkl_command_add_arg (struct ptkl_command *cmd, struct ptkl_arg *arg);
void ptkl_command_add_option (struct ptkl_command *cmd, struct ptkl_option *opt);
void ptkl_command_add_subcommand (struct ptkl_command *cmd, struct ptkl_command *subcommand);

int ptkl_cli_run (struct ptkl_cli *cli, int argc, char **argv);
void ptkl_cli_help (const struct ptkl_cli *cli);
void print_command_help (const struct ptkl_command *cmd);

void ptkl_cli_destroy (struct ptkl_cli *cli);

struct node {
	void *data;
	struct node *next;
};

struct bucket {
	struct node *head;
	struct node *tail;
};

void bucket_add (struct bucket *bucket, void *data);


#endif /* ARGS_H */
