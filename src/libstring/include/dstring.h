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

#ifndef DSTRING_H
#define DSTRING_H

#include "sds.h"

typedef struct dstring {
	int count;
	sds str;
} *dstring;

/**
 * Create a new dstring (with reference count = 1)
 */
dstring dstring_new (const char *str);

/**
 * Call this when assigning another reference to the dstring,
 * such as storing a copy of the pointer)
 */
void dstring_addref (dstring s);

/**
 * Always call either release or free when leaving the dstring's scope:
 * - call release if the dstring was handed to you (e.g., returned from a function)
 * - call free if you created the dstring, no longer need it, and there are no other refs
 */
void dstring_release (dstring s);

/**
 * Always call either release or free when leaving the dstring's scope:
 * - call release if the dstring was handed to you (e.g., returned from a function)
 * - call free if you created the dstring, no longer need it, and there are no other refs
 */
void dstring_free (dstring s);

/**
 * Clear the string.
 */
void dstring_clear (dstring s);

/**
 * Get the length of the string (s->str).
 */
size_t dstring_len (dstring s);

/**
 * Create a new copy (with reference count = 1)
 */
dstring dstring_dup (dstring s);

/**
 * Warning: this returns a new dstring.
 * References to original `s` will no longer be valid (it will be freed).
 */
dstring dstring_strcat (dstring s, const char *t);

/**
 * Warning: this returns a new dstring.
 * References to original `s` will no longer be valid (it will be freed).
 */
dstring dstring_cat (dstring s, dstring t);

/**
 * Return a formatted struct dstring for debugging.
 */
void dstring_tostring (dstring s, char *buf, size_t len);

#endif /* DSTRING_H */
