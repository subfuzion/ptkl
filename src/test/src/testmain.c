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

#include <assert.h>

#include "log.h"
#include "test.h"

extern void adt_test ();
extern void cli_test ();
extern void expect_test ();
extern void log_test ();
extern void string_test ();

int main ()
{
	register_signal_panic_handlers ();
	printf ("Running tests\n\n");

	test_suite tests[] = {
		{.name = "libstd: test framework tests", .fn = expect_test},
		{.name = "libstd: adt tests", .fn = adt_test},
		{.name = "libstd: errors tests", .fn = log_test},
		{.name = "libstd: string tests", .fn = string_test},
		{.name = "libcli: CLI tests", .fn = cli_test},
		{},
	};

	run_tests (tests);

	printf ("All tests passed.\n");
	return EXIT_SUCCESS;
}
