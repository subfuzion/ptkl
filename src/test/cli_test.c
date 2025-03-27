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

#include "args.h"
#include "test.h"

void test_boolean_option ()
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


void test_integer_option ()
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


void test_integer_option_fail ()
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


void test_scratch ()
{
	cli cli = cli_new ("ptkl", "0.1.0", "Partikle CLI");
	expect_eq_str ("ptkl", cli->name->str);
	expect_eq_str ("0.1.0", cli->version->str);
	expect_eq_str ("Partikle CLI", cli->description->str);
}

void cli_test ()
{
	test (test_scratch);

	// test (test_boolean_option);
	// test (test_integer_option);
	// test (test_integer_option_fail);
	//
	// PartikleCLI cli = cli_new ("ptkl", "0.1.0", "Partikle CLI");
	//
	// cli_destroy (cli);
}
