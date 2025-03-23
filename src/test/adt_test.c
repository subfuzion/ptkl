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
test_list()
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
test_stack()
{
	stack l;
	stack_init(&l);

	/* Test stack_push, stack_pop, and stack_peek */
	expect(stack_push(&l, "foo"));
	expect(stack_push(&l, "bar"));
	expect(stack_push(&l, "baz"));

	expect_eq_str("baz", (char *)stack_pop(&l));
	expect_eq_str("bar", (char *)stack_pop(&l));

	expect_eq_str("foo", (char *)stack_peek(&l));
	expect_eq_int(1, stack_size(&l));
	expect_eq_str("foo", (char *)stack_pop(&l));

	/* Test stack_size */
	expect_eq_int(0, stack_size(&l));

	expect_null(stack_pop(&l));

	/* Test stack_free */
	stack_free(&l);
	expect_eq_int(0, stack_size(&l));
}

void
test_vector()
{
	vector v;
	vector_init(&v);

	/* Test vector_add and vector_get */
	expect(vector_add(&v, "foo"));
	expect(vector_add(&v, "bar"));
	expect(vector_add(&v, "baz"));

	expect_eq_str("foo", (char *)vector_get(&v, 0));
	expect_eq_str("bar", (char *)vector_get(&v, 1));
	expect_eq_str("baz", (char *)vector_get(&v, 2));

	/* Test vector_size */
	expect_eq_int(3, vector_size(&v));

	/* Test vector_delete */
	expect(vector_delete(&v, 1));
	expect_null(vector_get(&v, 2));
	expect_eq_int(2, vector_size(&v));
	expect_eq_str("foo", (char *)vector_get(&v, 0));
	expect_eq_str("baz", (char *)vector_get(&v, 1));

	/* Test vector_free */
	vector_free(&v);
	expect_eq_int(0, vector_size(&v));
}

void
test_map()
{
	map m;
	map_init(&m);

	/* Test map_put and map_get */
	expect(map_put(&m, "key1", "value1"));
	expect(map_put(&m, "key2", "value2"));
	expect(map_put(&m, "key3", "value3"));

	expect_eq_str("value1", (char *)map_get(&m, "key1"));
	expect_eq_str("value2", (char *)map_get(&m, "key2"));
	expect_eq_str("value3", (char *)map_get(&m, "key3"));

	expect(map_get(&m, "key4") == nullptr);

	/* Test map_size */
	expect_eq_int(3, map_size(&m));

	/* Test map_delete */
	expect(map_delete(&m, "key2"));
	expect_null(map_get(&m, "key2"));
	expect_eq_int(2, map_size(&m));

	/* Test map_keys and map_values */
	bool found1 = false;
	bool found3 = false;

	char *keys[2];
	map_keys(&m, keys);
	for (auto i = 0; i < 2; i++) {
		if (strcmp(keys[i], "key1") == 0) {
			found1 = true;
		}
		if (strcmp(keys[i], "key3") == 0) {
			found3 = true;
		}
	}
	expect(found1);
	expect(found3);

	found1 = false;
	found3 = false;
	void *values[2];
	map_values(&m, values);
	for (auto i = 0; i < 2; i++) {
		if (strcmp(values[i], "value1") == 0) {
			found1 = true;
		}
		if (strcmp(values[i], "value3") == 0) {
			found3 = true;
		}
	}
	expect(found1);
	expect(found3);

	/* Test map_put with existing key (size shouldn't change) */
	expect_eq_int(2, map_size(&m));
	expect(map_put(&m, "key1", "value1"));
	expect_eq_str("value1", (char *)map_get(&m, "key1"));
	expect_eq_int(2, map_size(&m));

	/* Test map_free */
	map_free(&m);
	expect_eq_int(0, map_size(&m));
}


void
adt_test()
{
	run_test(test_list);
	run_test(test_stack);
	run_test(test_vector);
	run_test(test_map);
}
