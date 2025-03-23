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

#include "strings.h"
#include "test.h"

void test_string ()
{
	string s = string_new ("foo");
	expect_not_null (s);

	expect_eq_str ("foo", s);

	string_toupper (s);
	expect_eq_str ("FOO", s);

	string_tolower (s);
	expect_eq_str ("foo", s);

	string_clear (s);
	expect_eq_str ("", s);
	expect_eq_int (0, string_length (s));

	s = string_format ("%s%s", "foo", "bar");
	expect_eq_str ("foobar", s);

	s = string_cat_fmt (s, "%s", "baz");
	expect_eq_str ("foobarbaz", s);

	string s2 = string_from (s);
	expect_eq_str (s, s2);
	expect (string_compare (s, s2) == 0);

	s = string_cat (s, "biz");
	expect_eq_str ("foobarbazbiz", s);

	/* string_cat works, but string_cat_string optimized for two string
	 * (sds) objects */
	s = string_cat_string (s, string_new ("!"));
	expect_eq_str ("foobarbazbiz!", s);

	string_free (s);

	s = string_new ("  FOO  !");
	s = string_trim (s, " !");
	expect_eq_str ("FOO", s);

	string_free (s);
	string_free (s2);
}

void test_slice ()
{
	string s = string_new ("foobar");

	string_slice (s, 0, 3);
	expect_eq_str ("foo", s);

	s = string_set (s, "foobar");
	expect_eq_str ("foobar", s);


	s = string_set (s, "foobar");
	string_slice (s, 3, 4);
	expect_eq_str ("b", s);

	s = string_set (s, "foobar");
	string_slice (s, 6, 7);
	expect_eq_str ("", s);

	s = string_set (s, "foobar");
	string_slice (s, -3, -1);
	expect_eq_str ("bar", s);

	string_free (s);
}

void test_split_join ()
{
	string s1 = string_new ("foo:bar:baz");
	int count = 0;
	string *tokens = string_split (s1, ":", &count);
	expect_eq_int (3, count);
	expect_eq_str ("foo", tokens[0]);
	expect_eq_str ("bar", tokens[1]);
	expect_eq_str ("baz", tokens[2]);

	string joined = string_join_strings (tokens, count, ",");
	expect_eq_str ("foo,bar,baz", joined);

	string_free (s1);
	string_free_tokens (tokens, count);
}

void string_test ()
{
	test (test_string);
	test (test_slice);
	test (test_split_join);
}
