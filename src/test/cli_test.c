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

#include "args.h"
#include "test.h"


// void test_boolean_option ()
// {
// 	const struct ptkl_option_spec spec = {
// 		.name = "foo",
// 		.type = TT_BOOL,
// 	};
//
//
// 	struct ptkl_option opt = {
// 		.spec = spec,
// 		.text = "foo",
// 	};
//
// 	const auto expected = true;
// 	const bool res = parse_option (&opt);
// 	/* if (opt.error) printf("ERROR: %s (%s)\n", opt.text, opt.error); */
// 	expect (res);
// 	expect_eq (expected, opt.value.boolean);
// }
//
//
// void test_integer_option ()
// {
// 	const struct ptkl_option_spec spec = {.type = TT_INT};
//
// 	const auto tok = "777";
// 	const auto expected = 777;
//
// 	struct ptkl_option opt = {
// 		.spec = spec,
// 		.text = tok,
// 	};
//
// 	const bool res = parse_option (&opt);
// 	/* if (opt.error) printf("ERROR: %s (%s)\n", opt.text, opt.error); */
// 	expect (res);
// 	expect_eq_int (expected, opt.value.integer);
// }
//
//
// void test_integer_option_fail ()
// {
// 	const struct ptkl_option_spec spec = {.type = TT_INT};
//
// 	const auto tok = "777x";
//
// 	struct ptkl_option opt = {
// 		.spec = spec,
// 		.text = tok,
// 	};
//
// 	parse_option (&opt);
// 	expect_error ("Invalid input", opt.error);
// }

/* test instance */
cli CLI = nullptr;


static void setup ()
{
	CLI = cli_new ("ptkl", "0.1.0", "Partikle CLI");
}


static void teardown ()
{
	if (CLI) {
		cli_destroy (CLI);
		CLI = nullptr;
	}
}


static void test_cli_new ()
{
	setup();
	expect_eq_str ("ptkl", CLI->name->str);
	expect_eq_str ("0.1.0", CLI->version->str);
	expect_eq_str ("Partikle CLI", CLI->description->str);
	expect_not_null (CLI->commands);
	expect_not_null (CLI->options);
	expect_not_null (CLI->args);
	teardown();
}


static void test_cli_destroy ()
{
	setup();
	cli_destroy (CLI);
	expect_null (CLI->name);
	expect_null (CLI->version);
	expect_null (CLI->description);
	expect_null (CLI->commands);
	expect_null (CLI->options);
	expect_null (CLI->args);

	/* always set to null after destroy */
	/* in this case, so teardown doesn't try to free twice */
	CLI = nullptr;
	teardown();
}


static void test_cli_add_command ()
{
	setup();

	command cmd = cli_add_command (CLI, "foo", "bar");
	expect_not_null (cmd);

	command found = map_get (CLI->commands, "foo");
	expect_eq_str ("foo", found->name->str);

	teardown();
}


static void test_cli_add_option ()
{}


static void test_cli_parse ()
{
	setup();

	/* command line */
	char *args[] = {
		"ptkl",
		"foo",
		"bar",
	};

	int argc = sizeof (args) / sizeof (args[0]);
	char **argv = &args[0];

	bool res = cli_parse (CLI, argc, argv);
	expect_true (res);

	/* parse doesn't add argv[0] (the cmd name) to args */
	expect_eq_int (argc - 1, (int)vector_size (CLI->args));
	expect_eq_str ("foo", (char *)vector_get (CLI->args, 0));
	expect_eq_str ("bar", (char *)vector_get (CLI->args, 1));

	teardown();
}


#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


static void test_getopt ()
{
	/* command line */
	char *args[] = {
		"ptkl",
		"foo",
		"--bar",
		"4",
		"baz",
	};

	int argc = sizeof (args) / sizeof (args[0]);
	char **argv = &args[0];

	int c;

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"bar", required_argument, 0, 'b'},
			// {"append",  no_argument,       0,  0 },
			// {"delete",  required_argument, 0,  0 },
			// {"verbose", no_argument,       0,  0 },
			// {"create",  required_argument, 0, 'c'},
			// {"file",    required_argument, 0,  0 },
			{0, 0, 0, 0}
		};

		c = getopt_long (argc, argv, "ac:d:b::",
				 long_options, &option_index);
		if (c == -1) break;

		switch (c) {
		case 0: printf ("option %s", long_options[option_index].name);
			if (optarg) printf (" with arg %s", optarg);
			printf ("\n");
			break;

		case 'a': printf ("option a\n");
			break;

		case 'b': printf ("option b, arg: %s\n", optarg);
			break;

		case 'c': printf ("option c with value '%s'\n", optarg);
			break;

		case 'd': printf ("option d with value '%s'\n", optarg);
			break;

		case '?': break;

		default: printf ("?? getopt returned character code 0%o ??\n", c);
		}
	}

	if (optind < argc) {
		printf ("non-option ARGV-elements: ");
		while (optind < argc) printf ("%s ", argv[optind++]);
		printf ("\n");
	}
}


void cli_test ()
{
	// test (test_boolean_option);
	// test (test_integer_option);
	// test (test_integer_option_fail);
	test (test_cli_new);
	test (test_cli_destroy);
	test (test_cli_add_command);
	test (test_cli_add_option);
	test (test_cli_parse);
	test (test_getopt);
}
