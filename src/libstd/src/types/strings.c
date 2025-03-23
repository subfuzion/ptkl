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
#include <string.h>

string string_new (const char *str)
{
	return sdsnew (str);
}


void string_free (string s)
{
	sdsfree (s);
}


size_t string_length (const string s)
{
	return sdslen (s);
}


string string_from (const string s)
{
	/* HACK: possible bug with sdsdup */
	return string_new (s);
}


int string_compare (const string s, const string t)
{
	return sdscmp (s, t);
}


string string_format (const char *fmt, ...)
{
	va_list (args);
	va_start (args, fmt);
	string s = sdscatvprintf (sdsempty (), fmt, args);
	va_end (args);
	return s;
}


string string_copy (string s, const char *t)
{
	return sdscpy (s, t);
}


string string_set (string s, const char *t)
{
	return sdscpy (s, t);
}


string string_set_fmt (string s, const char *fmt, ...)
{
	s = string_copy (s, "");

	va_list args;
	va_start (args, fmt);
	s = sdscatvprintf (s, fmt, args);
	va_end (args);

	return s;
}

string string_cat_fmt (string s, const char *fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	s = sdscatvprintf (s, fmt, args);
	va_end (args);
	return s;
}


string string_cat (string s, const char *t)
{
	return sdscat (s, t);
}


string string_cat_string (string s, string t)
{
	return sdscatlen (s, t, sdslen (t));
}


string string_trim (string s, const char *chars)
{
	return sdstrim (s, chars);
}


void string_slice (string s, ssize_t start, ssize_t end)
{
	/* fix end to behave like string split in JavaScript */
	end = end > 0 ? end - 1 : end;
	sdsrange (s, start, end);
}


string *string_split (string s, const char *sep, int *count)
{
	/* casts are *not* redundant, even if your linter lies to you */
	return sdssplitlen (s, (ssize_t)sdslen (s), sep, (int)strlen (sep),
			    count);
}


string *string_split_args (const char *line, int *argc)
{
	/* TODO: implement string_split_args */
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
{
	/* TODO: implement string_free_tokens */
}


void string_tolower (string s)
{
	sdstolower (s);
}


void string_toupper (string s)
{
	sdstoupper (s);
}


void string_clear (string s)
{
	sdsclear (s);
}
