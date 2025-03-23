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

#ifndef STRINGS_H
#define STRINGS_H

#include "sds.h"

/**
 * string is a wrapper for the underlying sds dynamic string library
 * (https://github.com/antirez/sds) created by Salvatore Sanfilippo
 * and used by redis. The purpose for this wrapper is to:
 *  - provide a (somewhat) simplified facade for just the bits needed
 *  - facilitate replacing with another implementation, if desired
 *
 * A string is not immutable and many of the functions below invalidate
 * existing references to the string due to reallocating memory.
 * Therefore, for any of the functions below that return a string, you
 * must use the returned string and not ignore it. Any other references
 * (copies of the pointer) are invalidated.
 *
 * If you want to treat existing references as immutable, then always
 * duplicate the string first before mutating it. It's ideal to do that
 * inline:
 *
 *     string a = string_new ("foo");
 *     string b = a; // a and b both point to "foo"
 *     string a = string_cat (a, "bar"); // "foobar"
 *     // WARNING! b is no longer points to valid memory!
 *
 *     string a = string_new ("foo");
 *     string b = a;
 *     string a = string_cat (string_dup (a), "bar"); // "foobar"
 *     // b remains VALID ("foo")
 *
 */
typedef sds string;

string string_new (const char *str);

void string_free (string s);

/* Immutable */

size_t string_length (string s);

string string_from (string s);

int string_compare (string s, string t);

string string_copy (string s, const char *t);

string string_set (string s, const char *t);

string string_format (const char *fmt, ...);

string string_cat_fmt (string s, const char *fmt, ...);

string string_cat (string s, const char *t);

/**
 * string_cat works, but string_cat_string optimized for
 * two string (sds) objects
 */
string string_cat_string (string s, string t);

string string_trim (string s, const char *chars);

/**
 * Extracts the text from the string. The string is updated in place.
 * @param start The index of the first character to include in the returned
 * substring.
 * @param end The index of the first character to exclude from the returned
 * substring.
 */
void string_slice (string s, ssize_t start, ssize_t end);

string *string_split (string s, const char *sep, int *count);

string *string_split_args (const char *line, int *argc);

void string_free_tokens (string *tokens, int count);

string string_join (char **argv, int argc, const char *sep);

string string_join_strings (string *argv, int argc, const char *sep);

void string_tolower (string s);

void string_toupper (string s);

/* Mutable */

void string_clear (string s);

#endif /* STRINGS_H */
