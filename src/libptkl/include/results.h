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

typedef struct error {
	char *message;
} error;

typedef union ptkl_result {
	error *error;
	bool bool_val;
	char char_val;
	char *string_val;
	int int_val;
	long long_val;
	double double_val;
	void *pointer;
} result;

/* result constructors */

result make_error_result (const char *err);
result make_bool_result (bool val);
result make_char_result (char ch);
result make_string_result (const char *string);
result make_int_result (int n);
result make_long_result (long n);
result make_double_result (double val);
result make_pointer_result (void *pointer);

/* result accessors */

error *result_error (const result res);
bool result_bool (const result res);
char result_char (const result res);
char *result_string (const result res);
int result_int (const result res);
long result_long (const result res);
double result_double (const result res);
void *result_pointer (const result res);

/* result helpers */

bool succeeded (const result res);
bool failed (const result res);
void check (const result res);

#endif //RESULTS_H
