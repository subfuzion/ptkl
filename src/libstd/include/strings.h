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
 *  - provided a (somewhat) simplified facade for just the bits needed
 *  - allow switching out to another string library, if desired
 *  - make strings immutable
 *
 *  Under the hood, sds performs mutations that invalidate existing
 *  pointers; this is more efficient in terms of both time and space;
 *  however, at the expense of this optimization, this wrapper provides
 *  immutability for the assurance that an existing pointer is never
 *  invalidated because copies are always allocated and returned.
 *
 *  Because string is a typedef for sds (at least for now), you can
 *  always use raw sds functions on string to achieve a specific goal.
 *
 *  Caveat: as with every tradeoff, since there is no garbage
 *  collection, you are now generally responsible for freeing two
 *  strings instead of one. For example, the following will cause a
 *  leak:
 *
 *      string s = string_new (" foo ");
 *      s = string_trim (s, " ");  // Don't do this!
 *
 *  The problem is that the original s is "lost." You can't free it.
 *  You need to do this instead:
 *
 *      string s = string_new (" foo ");
 *      string t = string_trim (s, " ");
 *      //   s -> " foo "
 *      //   t -> "foo"
 *
 *      // later ...
 *      string_free(s);
 *      string_free(t);
 *
 *  For this reason, it might be convenient at times to use the
 *  underlying sds functions directly:
 *
 *      string s = string_new (" foo ");
 *      string t = s;
 *      s = string_trim (s, " ");
 *
 *      // s is valid (the updated pointer was assigned back to it)
 *      // t is no longer valid (the pointer needs to be updated).
 *
 *  string is defined as const sds specifically so that you can't
 *  accidentally overwrite a string variable. The first example
 *  shown above was to illustrate the issue, but the compiler
 *  should warn you if you do try this:
 *
 *      // compiler should warn you that you can't assign to a
 *      // variable with a const-qualified type
 *      string s = string_new (" foo ");
 *      s = string_trim (s, " ");
 *
 */
typedef sds string;

string string_new (const char *str);

void string_free (string s);

/* Immutable */

size_t string_len (string s);

string string_dup (string s);

int string_cmp (string s, string t);

string string_fmt (const char *fmt, ...);

string string_cat_fmt (string s, const char *fmt, ...);

string string_cat (string s, const char *t);

/**
 * string_cat works, but string_cat_string optimized for
 * two string (sds) objects
 */
string string_cat_string (string s, string t);

string string_trim (string s, const char *chars);

/**
 * Extracts the text from one string and returns a new string.
 * @param start The index of the first character to include in the returned
 * substring.
 * @param end The index of the first character to exclude from the returned
 * substring.
 * @return A new string containing the extracted section of the string.
 */
string string_slice (string s, ssize_t start, ssize_t end);

string *string_split (string s, const char *sep, int *count);

string *string_split_args (const char *line, int *argc);

void string_free_tokens (string *tokens, int count);

string string_join (char **argv, int argc, const char *sep);

string string_join_strings (string *argv, int argc, const char *sep);

string string_tolower (string s);

string string_toupper (string s);

/* Mutable */

void string_clear (string s);

#endif /* STRINGS_H */
