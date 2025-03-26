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

#include "dstring.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dstring dstring_new (const char *str)
{
	dstring s = malloc (sizeof (struct dstring));
	if (!s) {
		fprintf (stderr, "panic: %s: malloc failed for: %s\n", __func__, str);
		exit (1);
	}
	memset (s, 0, sizeof (struct dstring));
	s->count = 1;
	s->str = sdsnew (str);
	return s;
}


void dstring_free (dstring s)
{
	printf ("%s: free \"%s\"\n", __func__, s->str);
	s->count = 0;
	sdsfree (s->str);
}


void dstring_addref (dstring s)
{
	if (s->str) {
		printf ("%s: from %d to %d\n", __func__, s->count, s->count + 1);
		s->count++;
	}
}


void dstring_release (dstring s)
{
	printf ("%s: from %d to %d\n", __func__, s->count, s->count - 1);
	s->count--;
	if (s->count < 1) {
		dstring_free (s);
		s->str = nullptr;
	}
}


void dstring_clear (dstring s)
{
	sdsclear (s->str);
}

size_t dstring_len (dstring s)
{
	return sdslen (s->str);
}

dstring dstring_dup (dstring s)
{
	return dstring_new (s->str);
}


dstring dstring_strcat (dstring s, const char *t)
{
	s->str = sdscat (s->str, t);
	return s;
}


dstring dstring_cat (dstring s, dstring t)
{
	return dstring_strcat (s, t->str);
}


void dstring_tostring (dstring s, char *buf, size_t len)
{
	snprintf (buf, len, "{count: %d, str: %s}", s->count, s->str);
}
