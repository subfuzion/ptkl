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

/**
 * Dynamic string with reference counting.
 * - Call `dstring_addref` when making a copy of the reference to a dstring.
 * - Call `dstring_release` when finished with a reference to a dstring.
 * Example:
 *     dstring a = dstring_new("foo");  // .count = 1
 *     dstring_addref (a);              // increment .count to 2 to make a copy
 *     dstring b = a;                   // make another copy of the reference
 *     dstring_release (b);             // decrement .count to when done with b
 *     b = nullptr;                     // finished with b
 *     dstring_release (a);             // decrement .count to 0 =>
 *                                      //  - internal string is freed
 *                                      //  - dstring is freed
 *     a = nullptr;                     // finished with a
 *
 * Example:
 *     dstring a = dstring_new("foo");  // .count = 1
 *     dstring b = dstring_new("bar");  // .count = 1
 *     store_put(store, a, b);          // store calls addref (.count = 2 for a,
 * b) dstring_release(a);              // decrement .count to 1
 *     dstring_release(b);              // decrement .count to 1
 *     return;                          // store calls release later (which
 * frees a, b)
 */
typedef struct dstring {
	int count;
	sds str;
} *dstring;

/**
 * Create a new dstring (with reference count = 1)
 */
dstring dstring_new (const char *str);

/**
 * Always call either free or release when leaving the dstring's scope:
 * - call free if you created the dstring, no longer need it, and there are no
 * other refs
 * - call release if the dstring was handed to you (e.g., returned from a
 * function)
 */
void dstring_free (dstring s);

/**
 * Always call either release or free when leaving the dstring's scope:
 * - call release if the dstring was handed to you (e.g., returned from a
 * function)
 * - call free if you created the dstring, no longer need it, and there are no
 * other refs
 */
void dstring_release (dstring s);

/**
 * Call this when assigning another reference to the dstring,
 * such as storing a copy of the pointer)
 */
void dstring_addref (dstring s);

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
