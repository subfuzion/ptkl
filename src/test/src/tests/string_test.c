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
	string s1 = string_new ("foo");
	expect_not_null (s1);

	expect_eq_str ("foo", s1);

	string s2 = string_toupper (s1);
	expect_eq_str ("FOO", s2);

	string s3 = string_tolower (s2);
	expect_eq_str ("foo", s3);

	string_clear (s1);
	expect_eq_str ("", s1);
	expect_eq_int (0, string_len (s1));

	string s4 = string_fmt ("%s%s", "foo", "bar");
	expect_eq_str ("foobar", s4);

	string s5 = string_cat_fmt (s4, "%s", "baz");
	expect_eq_str ("foobarbaz", s5);

	string s6 = string_dup (s5);
	expect_eq_str (s5, s6);
	expect (string_cmp (s5, s6) == 0);

	string s7 = string_cat (s6, "biz");
	expect_eq_str ("foobarbazbiz", s7);

	/* string_cat works, but string_cat_string optimized for two string
	 * (sds) objects */
	string s8 = string_cat_string (s7, string_new ("!"));
	expect_eq_str ("foobarbazbiz!", s8);

	string s9 = string_new ("  FOO  !");
	string s10 = string_trim (s9, " !");
	expect_eq_str ("FOO", s10);

	string_free (s1);
	string_free (s2);
	string_free (s3);
}

void test_slice ()
{
	string s1 = string_new ("foobar");

	string s2 = string_slice (s1, 0, 3);
	expect_eq_str ("foo", s2);

	string s3 = string_slice (s1, 2, 3);
	expect_eq_str ("o", s3);

	string s4 = string_slice (s1, 6, 7);
	expect_eq_str ("", s4);

	string s5 = string_slice (s1, -3, -1);
	expect_eq_str ("bar", s5);

	string_free (s1);
	string_free (s2);
	string_free (s3);
	string_free (s4);
	string_free (s5);
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
