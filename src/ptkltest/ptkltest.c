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

#include "test.h"

#include "list.h"
#include "map.h"
#include "stack.h"
#include "vector.h"

void
test_list()
{
	list l;
	list_init(&l);

	// Test list_push and list_pop
	expect_true(list_add(&l, "foo"));
	expect_true(list_add(&l, "bar"));
	expect_true(list_add(&l, "baz"));

	expect_eq_str("foo", (char *)list_get(&l, 0));
	expect_eq_str("bar", (char *)list_get(&l, 1));
	expect_eq_str("baz", (char *)list_get(&l, 2));

	// Test list_size
	expect_eq_int(3, list_size(&l));

	expect_true(list_delete(&l, 2));
	expect_null(list_get(&l, 2));
	expect_eq_int(2, list_size(&l));

	// Test list_free
	list_free(&l);
	expect_eq_int(0, list_size(&l));
}

void
test_stack()
{
	stack l;
	stack_init(&l);

	// Test stack_push, stack_pop, and stack_peek
	expect_true(stack_push(&l, "foo"));
	expect_true(stack_push(&l, "bar"));
	expect_true(stack_push(&l, "baz"));

	expect_eq_str("baz", (char *)stack_pop(&l));
	expect_eq_str("bar", (char *)stack_pop(&l));

	expect_eq_str("foo", (char *)stack_peek(&l));
	expect_eq_int(1, stack_size(&l));
	expect_eq_str("foo", (char *)stack_pop(&l));

	// Test stack_size
	expect_eq_int(0, stack_size(&l));

	expect_null(stack_pop(&l));

	// Test stack_free
	stack_free(&l);
	expect_eq_int(0, stack_size(&l));
}

void
test_vector()
{
	vector v;
	vector_init(&v);

	// Test vector_add and vector_get
	expect_true(vector_add(&v, "foo"));
	expect_true(vector_add(&v, "bar"));
	expect_true(vector_add(&v, "baz"));

	expect_eq_str("foo", (char *)vector_get(&v, 0));
	expect_eq_str("bar", (char *)vector_get(&v, 1));
	expect_eq_str("baz", (char *)vector_get(&v, 2));

	// Test vector_size
	expect_eq_int(2, vector_size(&v));

	// Test vector_delete
	expect_true(vector_delete(&v, 1));
	expect_null(vector_get(&v, 2));
	expect_eq_int(2, vector_size(&v));
	expect_eq_str("foo", (char *)vector_get(&v, 0));
	expect_eq_str("baz", (char *)vector_get(&v, 1));

	// Test vector_free
	vector_free(&v);
	expect_eq_int(0, vector_size(&v));
}

void
test_map()
{
	map m;
	map_init(&m);

	// Test map_put and map_get
	expect_true(map_put(&m, "key1", "value1"));
	expect_true(map_put(&m, "key2", "value2"));
	expect_true(map_put(&m, "key3", "value3"));

	expect_eq_str("value1", (char *)map_get(&m, "key1"));
	expect_eq_str("value2", (char *)map_get(&m, "key2"));
	expect_eq_str("value3", (char *)map_get(&m, "key3"));

	// Test map_size
	expect_eq_int(3, map_size(&m));

	// Test map_delete
	expect_true(map_delete(&m, "key2"));
	expect_null(map_get(&m, "key2"));
	expect_eq_int(2, map_size(&m));

	// Test map_keys and map_values
	char *keys[2];
	void *values[2];
	map_keys(&m, keys);
	map_values(&m, values);

	expect_true((strcmp(keys[0], "key1") == 0 && strcmp(keys[1], "key3") == 0) ||
		(strcmp(keys[0], "key3") == 0 && strcmp(keys[1], "key1") == 0));
	expect_true((strcmp(values[0], "value1") == 0 && strcmp(values[1], "value3") == 0) ||
		(strcmp(values[0], "value3") == 0 && strcmp(values[1], "value1") == 0));

	// Test map_free
	map_free(&m);
	expect_eq_int(0, map_size(&m));
}


int
main()
{
	printf("RUNNING TESTS...\n");

	run_test(test_list);
	run_test(test_stack);
	run_test(test_vector);
	run_test(test_map);

	printf("[100%%] All tests passed.\n");
	return EXIT_SUCCESS;
}
