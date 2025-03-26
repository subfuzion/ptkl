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

#include "errors.h"
#include "results.h"
#include "test.h"

void test_errors()
{
	// char *s = NULL;
	// printf ("%s\n", strdup (s));
	// panic ("test");
}

void test_results() {
	result res;

	res = make_string_result ("foo");
	expect_eq_str ("foo", result_string(res));

	res = make_int_result (10);
	expect_eq_int (10, result_int(res));

	res = make_error_result ("oops");
	expect_not_null(result_error (res));
	expect(failed(res));
	expect_false (succeeded(res));
	expect_eq_str ("oops", result_error(res)->message);
}


void utils_test()
{
	test (test_errors);
	test (test_results);
}
