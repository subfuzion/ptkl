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

#define expect_true(expr) do { \
	if (!(expr)) { \
		printf("❌   FAIL (%s:%d): expected to be true: %s\n", __FILE__, __LINE__, #expr); \
		exit(EXIT_FAILURE); \
	} \
} while (0)

#define expect expect_true

#define expect_false(expr) do { \
	if ((expr)) { \
		printf("\r\x1b[K"); \
		printf("❌ %s:%d: expected to be false: %s\n", __FILE__, __LINE__, #expr); \
		exit(EXIT_FAILURE); \
	} \
} while (0)

#define expect_null(expr) do { \
	if ((expr) != nullptr) { \
		printf("\r\x1b[K"); \
		printf("❌ %s: expected to be null: %s (%s:%d)\n", __func__, #expr, __FILE__, __LINE__); \
		exit(EXIT_FAILURE); \
	} \
} while (0)

#define expect_eq(expected, actual) do { \
	if ((expected) != (actual)) { \
		printf("\r\x1b[K"); \
		printf ("❌ %s: expected %d, got %d (%s:%d)\n", __func__, (expected), (actual), __FILE__, __LINE__); \
		exit(EXIT_FAILURE); \
	} \
} while (0)

#define expect_eq_int(expected, actual) do { \
	if ((int)(expected) != (int)(actual)) { \
		printf("\r\x1b[K"); \
		printf ("❌ %s: expected %d, got %d (%s:%d)\n", __func__, (int)(expected), (int)(actual), __FILE__, __LINE__); \
		exit(EXIT_FAILURE); \
	} \
} while (0)

#define expect_eq_str(expected, actual) do { \
	if (strcmp((expected), (actual)) != 0) { \
		printf("\r\x1b[K"); \
		printf ("❌ %s: expected %s, got %s (%s:%d)\n", __func__, (expected), (actual), __FILE__, __LINE__); \
		exit(EXIT_FAILURE); \
	} \
} while (0)

#define run_test(test) do { \
	printf("▷ %s ... ", #test); \
	test(); \
	printf("\r\x1b[K"); \
	printf ("✅%s\n", #test); \
} while (0)

#endif /* TEST_H */
