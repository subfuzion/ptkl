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

struct ptkl_cli {
	char *version;
	char *description;

	map *options;
};

void init_cli (struct ptkl_cli *cli, char *version, char *description);
bool add_option (struct ptkl_cli *cli, struct ptkl_option_spec spec);
bool cli_parse (struct ptkl_cli *cli, int argc, char **argv);
bool parse_option (struct ptkl_option *opt);


/****************************************************************************/
/* src/libcli/args.c                                                        */
/****************************************************************************/

void
init_cli (struct ptkl_cli *cli, char *version, char *description)
{
}


bool
cli_parse(struct ptkl_cli *cli, int argc, char **argv)
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
	case TT_BOOL:
		if (strcmp (opt->text, opt->spec.name) == 0) {
			opt->value.boolean = true;
			return true;
		}
		break;
	case TT_INT:
		char *end;
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
	test (test_boolean_option);
	test (test_integer_option);
	test (test_integer_option_fail);
}

