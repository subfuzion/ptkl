/*
 * ptkl - Partikle Runtime
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

#ifndef RESULTS_H
#define RESULTS_H

#include "strings.h"

/**
 * This is an experimental effort that probably won't pan out.
 * Problems:
 * - to work, the type needs to be included in the result (otherwise, how to
 *   signal an error?
 * - returning a 16 byte value is not optimal, but allocating and freeing
 *   memory for every result is also not optimal
 *
 *   Putting this in the icebox for now, probably abandoning it for good.
 */
typedef enum result_type {
	ERROR,
	BOOL,
	CHAR,
	STRING,
	INT,
	LONG,
	DOUBLE,
	POINTER,
} result_type;

typedef struct error {
	string message;
	void *extra;
} error;

static inline void error_free (error e)
{
	if (e.message != nullptr) string_free (e.message);
}

typedef struct result {
	result_type type;
	union {
		error error;
		bool bool_val;
		char char_val;
		string string_val;
		int int_val;
		long long_val;
		double double_val;
		void *pointer;
	};
} result;

/**
 * result constructors.
 * - If making a string result, caller will be responsible for
 *   calling string_free (result.string_val) when done with it.
 * - If making an error result, caller will be responsible for
 *   calling error_free (result) when done with it.
 */

result make_error_result (const char *err);
result make_bool_result (bool val);
result make_char_result (char ch);
result make_string_result (const char *string);
result make_int_result (int n);
result make_long_result (long n);
result make_double_result (double val);
result make_pointer_result (void *pointer);

/**
 * result accessors.
 * - If the result is a string, caller will be responsible for
 *   calling string_free() when done with it.
 */

error result_error (result res);
bool result_bool (result res);
char result_char (result res);
string result_string (result res);
int result_int (result res);
long result_long (result res);
double result_double (result res);
void *result_pointer (result res);

/* result helpers */

bool succeeded (result res);
bool failed (result res);
void check (result res);

static inline void result_free (result res)
{}

#endif // RESULTS_H
