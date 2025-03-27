/*
 * Partikle Runtime
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

#include "strings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

string string_new (const char *str)
{
	return sdsnew (str);
}


void string_free (string s)
{
	sdsfree (s);
}


size_t string_len (const string s)
{
	return sdslen (s);
}


string string_dup (const string s)
{
	return string_new (s);
}


int string_cmp (const string s, const string t)
{
	return sdscmp (s, t);
}


string string_fmt (const char *fmt, ...)
{
	va_list ap;
	char *t;
	va_start (ap, fmt);
	sds s = sdsempty ();
	t = sdscatvprintf (s, fmt, ap);
	va_end (ap);
	return t;
}


string string_cat_fmt (string s, const char *fmt, ...)
{
	va_list ap;
	char *t;
	va_start (ap, fmt);
	t = sdscatvprintf (sdsdup (s), fmt, ap);
	va_end (ap);
	return t;
}


string string_cat (string s, const char *t)
{
	string copy = sdsdup (s);
	return sdscat (copy, t);
}


string string_cat_string (string s, string t)
{
	string copy = sdsdup (s);
	return sdscatlen (copy, t, sdslen (t));
}


string string_trim (string s, const char *chars)
{
	string copy = sdsdup (s);
	return sdstrim (copy, chars);
}


string string_slice (string s, ssize_t start, ssize_t end)
{
	string copy = string_new (s);
	if (end != 0) end = end > 0 ? end - 1 : end;
	sdsrange (copy, start, end);
	return copy;
}


string *string_split (string s, const char *sep, int *count)
{
	return sdssplitlen (s, (ssize_t)sdslen (s), sep, (int)strlen (sep),
			    count);
}


string *string_split_args (const char *line, int *argc)
{
	return nullptr;
}


string string_join (char **argv, int argc, const char *sep)
{
	return sdsjoin (argv, argc, (char *)sep);
}


string string_join_strings (string *argv, int argc, const char *sep)
{
	return sdsjoinsds ((sds *)argv, argc, sep, strlen (sep));
}


void string_free_tokens (string *tokens, int count)
{}


string string_tolower (string s)
{
	sds copy = sdsdup (s);
	sdstolower (copy);
	return copy;
}


string string_toupper (string s)
{
	sds copy = sdsdup (s);
	sdstoupper (copy);
	return copy;
}


void string_clear (string s)
{
	sdsclear (s);
}
