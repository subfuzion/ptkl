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

#include "command.h"
#include "test.h"

#if 0

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
	CLI = cli_new ("ptkl", "Partikle CLI", nullptr);
	cli_set (CLI, "version", "0.1.0");
}


static void teardown ()
{
	if (CLI == nullptr) return;
	cli_free (CLI);
	CLI = nullptr;
}


static void test_cli_new ()
{
	setup ();
	expect_eq_str ("ptkl", CLI->name);
	expect_eq_str ("Partikle CLI", CLI->help);
	expect_not_null (CLI->commands);
	// expect_not_null (CLI->options);
	// expect_not_null (CLI->args);

	string version = cli_get (CLI, "version");
	expect_eq_str ("0.1.0", version);
	teardown ();
}


static void test_cli_add_command ()
{
	setup ();

	// command cmd = cli_add_command (CLI, "foo", "bar");
	// expect_not_null (cmd);
	//
	// command found = map_get (CLI->commands, "foo");
	// expect_eq_str ("foo", found->name->str);

	teardown ();
}


static void test_cli_add_option ()
{}


static void test_cli_parse ()
{
	setup ();

	// /* command line */
	// char *args[] = {
	// 	"ptkl",
	// 	"foo",
	// 	"bar",
	// };
	//
	// int argc = sizeof (args) / sizeof (args[0]);
	// char **argv = &args[0];
	//
	// bool res = cli_parse (CLI, argc, argv);
	// expect_true (res);
	//
	// /* parse doesn't add argv[0] (the cmd name) to args */
	// expect_eq_int (argc - 1, (int)vector_size (CLI->args));
	// expect_eq_str ("foo", (char *)vector_get (CLI->args, 0));
	// expect_eq_str ("bar", (char *)vector_get (CLI->args, 1));

	teardown ();
}

#endif /* 0 */

void cli_test ()
{
	// test (test_cli_new);
	// test (test_cli_add_command);
	// test (test_cli_add_option);
	// test (test_cli_parse);
}
