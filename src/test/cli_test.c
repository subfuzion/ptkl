/*
 * Unit tests for Partikle Runtime
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

#include <limits.h>

/* #include "args.h" */
#include "test.h"

/****************************************************************************/
/* cutils.c                                                        */
/****************************************************************************/

/* error handling */

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void
print_stack_trace ()
{
	void *buffer[1024];
	int count = backtrace (buffer, sizeof(buffer) / sizeof(buffer[0]));
	char **traces = backtrace_symbols (buffer, count);
	if (traces != NULL) {
		fprintf (stderr, "Stack trace:\n");
		for (int i = 3; i < count; i++) {
			fprintf (stderr, "%s\n", traces[i]);
		}
		free (traces);
	} else {
		perror ("backtrace_symbols");
		exit (EXIT_FAILURE);
	}
	exit (EXIT_FAILURE);
}

void
panic_signal_handler (int sig)
{
	fprintf (stderr, "Caught signal: %d\n", sig);
	print_stack_trace();
}

void
set_signal_panic_handlers ()
{
	/* don't intercept SIGABRT, SIGINT, SIGTERM */
	signal (SIGFPE, panic_signal_handler); /* arithmetic (divide by zero, etc) */
	signal (SIGILL, panic_signal_handler); /* illegal instruction */
	signal (SIGSEGV, panic_signal_handler); /* segmentation violation */
	signal (SIGBUS, panic_signal_handler); /* bus error accessing invalid address */
}

void
panic (const char *msg)
{
	fprintf (stderr, "panic: %s\n", msg);
	print_stack_trace();
}

/* 8 bytes (size of the biggest member: double */
typedef union ptkl_result {
	char *error;
	bool bool_val;
	char char_val;
	char *string_val;
	int int_val;
	long long_val;
	double double_val;
	void *pointer;
} result;

result
make_error_result (const char *err)
{
	return (result){.error = strdup (err)};
}

result
make_bool_result (bool val)
{
	return (result){.bool_val = val};
}

result
make_char_result (char ch)
{
	return (result){.char_val = ch};
}

result
make_string_result (const char *string)
{
	return (result){.string_val = strdup (string)};
}

result
make_int_result (int n)
{
	return (result){.int_val = n};
}

result
make_long_result (long n)
{
	return (result){.long_val = n};
}

result
make_double_result (double val)
{
	return (result){.double_val = val};
}

result
make_pointer_result (void *pointer)
{
	return (result){.pointer = pointer};
}



char *
result_error (const result res)
{
	return res.error;
}

bool
result_bool (const result res)
{
	return res.bool_val;
}

char
result_char (const result res)
{
	return res.char_val;
}

char *
result_string (const result res)
{
	return res.string_val;
}

int
result_int (const result res)
{
	return res.int_val;
}

long
result_long (const result res)
{
	return res.long_val;
}

double
result_double (const result res)
{
	return res.double_val;
}

void *
result_pointer (const result res)
{
	return res.pointer;
}

bool
succeeded (const result res)
{
	return !res.error;
}

bool
failed (const result res)
{
	return res.error != nullptr;
}

void
check (const result res)
{
	if (failed (res)) {
		panic (res.error ? res.error : "unknown error");
	}
}

/* memory */

/* strings */

/****************************************************************************/
/* src/libcli/args.h                                                        */
/****************************************************************************/

#include <stddef.h>

#include "map.h"
#include "vector.h"

/* Specifies how to parse a command line token */
enum ptkl_token_type {
	TT_STR,
	TT_BOOL,
	TT_INT,
};

/* Holds the command line token's value after parsing */
union ptkl_parse_value {
	char *string;
	bool boolean;
	int integer;
};

/* A command line option spec */
struct ptkl_option_spec {
	/* name is used as the long option */
	const char *name;
	const char short_opt;
	const char *help;
	bool multi;
	enum ptkl_token_type type;

	/* internal */
	/* struct ptkl_command *command; */
};

/* A parsed command line option */
struct ptkl_option {
	struct ptkl_option_spec spec;
	char *text;

	union {
		union ptkl_parse_value value;
		vector values;
	};

	char error[256];
};

typedef struct ptkl_cli {
	char *name;
	char *version;
	char *description;

	map *options;
} *PartikleCLI;


bool cli_add_option (struct ptkl_cli *cli, struct ptkl_option_spec spec);

bool cli_parse (struct ptkl_cli *cli, int argc, char **argv);
bool parse_option (struct ptkl_option *opt);

#define STRDUP(str) (str) ? strdup (str) : nullptr

#define FREE(var)												      \
	do {                                                                                                           \
		if (var) {                                                                                             \
			free (var);                                                                                    \
			var = nullptr;                                                                                 \
		}                                                                                                      \
	} while (0)

#define FREE_FN(var, free_fn)												      \
	do {                                                                                                           \
		if (var) {                                                                                             \
			free_fn (var);                                                                                    \
			var = nullptr;                                                                                 \
		}                                                                                                      \
	} while (0)

#define PANIC(msg)												      \
	do {                                                                                                           \
		fprintf (stderr, "%s: %s\n", __func__, msg);                                                                         \
		exit (EXIT_FAILURE);                                                                                   \
	} while (0)

/****************************************************************************/
/* src/libcli/args.c                                                        */
/****************************************************************************/

static void
cli_init (struct ptkl_cli *cli, const char *name, const char *version, const char *description)
{
	cli->name = STRDUP (name);
	cli->version = STRDUP (version);
	cli->description = STRDUP (description);

	map *options = malloc (sizeof (map));
	if (!options)
		PANIC ("Out of memory");
	map_init (options);
	cli->options = options;
}

void
cli_free (struct ptkl_cli *cli)
{
	FREE (cli->name);
	FREE (cli->version);
	FREE (cli->description);
	FREE_FN (cli->options, map_free);
}

PartikleCLI
cli_new (const char *name, const char *version, const char *description)
{
	PartikleCLI cli = malloc (sizeof (struct ptkl_cli));
	if (!cli)
		PANIC ("Out of memory");
	cli_init (cli, name, version, description);
	return cli;
}

void
cli_destroy (PartikleCLI cli)
{
	cli_free (cli);
}


bool
cli_parse (struct ptkl_cli *cli, int argc, char **argv)
{
	int optind = 1;
	// while (optind < argc && *argv[optind] == '-') {
	while (optind < argc && *argv[optind]) {
		printf ("%s\n", argv[optind++]);
	}
	return 0;
}

bool
parse_option (struct ptkl_option *opt)
{
	const char *str = opt->text;

	switch (opt->spec.type) {
	case TT_STR:
		// TODO: evaluate strdup pros/cons (don't need fixed buffer / need to free later)
		strcpy (opt->value.string, str);
		return true;
	case TT_BOOL: if (strcmp (opt->text, opt->spec.name) == 0) {
			opt->value.boolean = true;
			return true;
		}
		break;
	case TT_INT: char *end;
		const long num = strtol (str, &end, 10);

		if (*end != '\0') {
			strcpy (opt->error, "%s");
			sprintf (opt->error, "Invalid input or trailing characters: %s", end);
		} else if (num > INT_MAX || num < INT_MIN) {
			strcpy (opt->error, "%s");
			sprintf (opt->error, "Invalid input (number out of range): %s", str);
		} else {
			opt->value.integer = (int)num;
			return true;
		}
		break;

	default:
		// TODO: evaluate if it would be better to treat generically as a string
		strcpy (opt->error, "%s");
		sprintf (opt->error, "unknown option type: %d, can't parse: %s", opt->spec.type, str);
		break;
	}

	return false;
}


/****************************************************************************/
/* TESTS                                                                    */
/****************************************************************************/

void
test_boolean_option ()
{
	const struct ptkl_option_spec spec = {
		.name = "foo",
		.type = TT_BOOL,
	};


	struct ptkl_option opt = {
		.spec = spec,
		.text = "foo",
	};

	const auto expected = true;
	const bool res = parse_option (&opt);
	/* if (opt.error) printf("ERROR: %s (%s)\n", opt.text, opt.error); */
	expect (res);
	expect_eq (expected, opt.value.boolean);
}

void
test_integer_option ()
{
	const struct ptkl_option_spec spec = {.type = TT_INT};

	const auto tok = "777";
	const auto expected = 777;

	struct ptkl_option opt = {
		.spec = spec,
		.text = tok,
	};

	const bool res = parse_option (&opt);
	/* if (opt.error) printf("ERROR: %s (%s)\n", opt.text, opt.error); */
	expect (res);
	expect_eq_int (expected, opt.value.integer);
}

void
test_integer_option_fail ()
{
	const struct ptkl_option_spec spec = {.type = TT_INT};

	const auto tok = "777x";

	struct ptkl_option opt = {
		.spec = spec,
		.text = tok,
	};

	parse_option (&opt);
	expect_error ("Invalid input", opt.error);
}


void
cli_test ()
{
	set_signal_panic_handlers();
	// char *s = NULL;
	// printf ("%s\n", strdup (s));
	// panic ("test");

	result res;

	res = make_string_result ("foo");
	check (res);
	printf ("%s\n", result_string (res));

	res = make_int_result (10);
	check (res);

	if (succeeded (res)) printf ("%d\n", res.int_val);

	res = make_error_result ("whoops");
	check (res);



	// test (test_boolean_option);
	// test (test_integer_option);
	// test (test_integer_option_fail);
	//
	// PartikleCLI cli = cli_new ("ptkl", "0.1.0", "Partikle CLI");
	//
	// cli_destroy (cli);
}
