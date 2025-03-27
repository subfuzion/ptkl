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

#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *  Tests
 */
typedef void (*test_func) (void);

typedef struct test_suite {
	char *name;
	test_func fn;
	bool skip;
} test_suite;

static inline void run_test (const test_suite *suite)
{
	if (!suite) return;

	printf ("▶︎ %s", suite->name);

	if (suite->skip) {
		printf (" ... skipped");
	}
	printf ("\n");
	suite->fn ();
	printf ("\n");
}

static inline void run_tests (const test_suite *tests)
{
	if (!tests) return;
	test_suite t;
	while ((t = *tests++).name) {
		run_test (&t);
	}
}


/**
 * Test expectations
 */

#define PASS_PREFIX "\r\x1b[K✅ "
#define FAIL_PREFIX "\r\x1b[K❌ "

#define test(t)                                                                \
	do {                                                                   \
		printf ("▷ %s ... ", #t);                                      \
		t ();                                                          \
		printf ("%s %s\n", PASS_PREFIX, #t);                           \
	} while (0)

#define expect_true(expr)                                                      \
	do {                                                                   \
		if (!(expr)) {                                                 \
			printf ("%s %s:%s:%d: expected to be true: %s\n",      \
				FAIL_PREFIX, __func__, __FILE__, __LINE__,     \
				#expr);                                        \
			exit (EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)

#define expect expect_true

#define expect_false(expr)                                                     \
	do {                                                                   \
		if ((expr)) {                                                  \
			printf ("%s %s:%s:%d: expected to be false: %s\n",     \
				FAIL_PREFIX, __func__, __FILE__, __LINE__,     \
				#expr);                                        \
			exit (EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)

#define expect_null(expr)                                                      \
	do {                                                                   \
		if ((void *)(expr) != nullptr) {                               \
			printf ("%s %s:%s:%d: expected to be null: %s\n",      \
				FAIL_PREFIX, __func__, __FILE__, __LINE__,     \
				#expr);                                        \
			exit (EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)

#define expect_not_null(expr)                                                  \
	do {                                                                   \
		if ((expr) == nullptr) {                                       \
			printf ("%s %s:%s:%d: expected not to be null: %s\n",  \
				FAIL_PREFIX, __func__, __FILE__, __LINE__,     \
				#expr);                                        \
			exit (EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)

#define expect_eq(expected, actual)                                            \
	do {                                                                   \
		if ((expected) != (actual)) {                                  \
			printf ("%s %s:%s:%d: expected %d, got %d\n",          \
				FAIL_PREFIX, __func__, __FILE__, __LINE__,     \
				(expected), (actual));                         \
			exit (EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)

#define expect_eq_int(expected, actual)                                        \
	do {                                                                   \
		if ((int)(expected) != (int)(actual)) {                        \
			printf ("%s %s:%s:%d: expected %d, got %d\n",          \
				FAIL_PREFIX, __func__, __FILE__, __LINE__,     \
				(int)(expected), (int)(actual));               \
			exit (EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)

#define expect_eq_long(expected, actual)                                       \
	do {                                                                   \
		if ((int)(expected) != (int)(actual)) {                        \
			printf ("%s %s:%s:%d: expected %ul, got %ul\n",        \
				FAIL_PREFIX, __func__, __FILE__, __LINE__,     \
				(long)(expected), (int)(actual));              \
			exit (EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)

#define expect_eq_str(expected, actual)                                        \
	do {                                                                   \
		if (strcmp ((expected), (actual)) != 0) {                      \
			printf ("%s %s:%s:%d: expected \"%s\", got \"%s\"\n",  \
				FAIL_PREFIX, __func__, __FILE__, __LINE__,     \
				(expected), (actual));                         \
			exit (EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)

/* expect null or zero-length string */
#define expect_empty_str(str)                                                  \
	do {                                                                   \
		if ((str) && strlen ((char *)(str)) != 0) {                    \
			printf ("%s %s:%s:%d: expected empty string, got "     \
				"\"%s\"\n",                                    \
				FAIL_PREFIX, __func__, __FILE__, __LINE__,     \
				(str));                                        \
			exit (EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)

#define expect_error(expected, actual)                                         \
	do {                                                                   \
		if (!(actual) || strlen ((actual)) == 0 ||                     \
		    strlen ((actual)) < strlen ((expected)) ||                 \
		    strncmp ((expected), (actual), strlen ((expected))) !=     \
			    0) {                                               \
			printf ("%s %s:%s:%d: missing error:\n"                \
				"  expected: \"%s\", got: \"%s\"\n",           \
				FAIL_PREFIX, __func__, __FILE__, __LINE__,     \
				expected, actual);                             \
			exit (EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)

#define expect_empty_error(err)                                                \
	do {                                                                   \
		if ((err) != nullptr || strnlen ((str), 1) > 0) {              \
			printf ("%s %s:%s:%d: unexpected error:\n"             \
				"  \"%s\"\n",                                  \
				FAIL_PREFIX, __func__, __FILE__, __LINE__,     \
				err);                                          \
			exit (EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)


#endif /* TEST_H */
