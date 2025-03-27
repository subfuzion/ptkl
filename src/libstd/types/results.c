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

#include <string.h>

#include "results.h"
#include "errors.h"

static thread_local error g_result_error = {};

/* result constructors */

result make_error_result (const char *err)
{
	g_result_error.message = err ? strdup (err) : "unknown error";
	return (result){.error = &g_result_error};
}


result make_bool_result (bool val)
{
	return (result){.bool_val = val};
}


result make_char_result (char ch)
{
	return (result){.char_val = ch};
}


result make_dstring_result (const char *string)
{
	return (result){.dstring_val = dstring_new (string)};
}


result make_int_result (int n)
{
	return (result){.int_val = n};
}


result make_long_result (long n)
{
	return (result){.long_val = n};
}


result make_double_result (double val)
{
	return (result){.double_val = val};
}


result make_pointer_result (void *pointer)
{
	return (result){.pointer = pointer};
}


/* result accessors */

error *result_error (const result res)
{
	return res.error;
}


bool result_bool (const result res)
{
	return res.bool_val;
}


char result_char (const result res)
{
	return res.char_val;
}


dstring result_dstring (const result res)
{
	return res.dstring_val;
}


int result_int (const result res)
{
	return res.int_val;
}


long result_long (const result res)
{
	return res.long_val;
}


double result_double (const result res)
{
	return res.double_val;
}


void *result_pointer (const result res)
{
	return res.pointer;
}


/* result helpers */

bool succeeded (const result res)
{
	return res.error != &g_result_error;
}


bool failed (const result res)
{
	return res.error == &g_result_error;
}


void check (const result res)
{
	if (failed (res)) panic (res.error->message);
}
