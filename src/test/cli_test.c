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

#include "adt.h"
#include "test.h"

void
test_args()
{
	list l;
	list_init(&l);

	/* Test list_add and list_get */
	expect(list_add(&l, "foo"));
	expect(list_add(&l, "bar"));
	expect(list_add(&l, "baz"));

	expect_eq_str("foo", (char *)list_get(&l, 0));
	expect_eq_str("bar", (char *)list_get(&l, 1));
	expect_eq_str("baz", (char *)list_get(&l, 2));

	/* Test list_size */
	expect_eq_int(3, list_size(&l));

	/* Test list_delete */
	expect(list_delete(&l, 2));
	expect_null(list_get(&l, 2));
	expect_eq_int(2, list_size(&l));

	/* Test list_free */
	list_free(&l);
	expect_eq_int(0, list_size(&l));
}

void
cli_test()
{
	run_test(test_args);
}
