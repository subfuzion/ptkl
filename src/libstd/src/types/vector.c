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
#include "vector.h"
#include <string.h>

void vector_init (vector *v)
{
	v->capacity = 4;
	v->size = 0;
	v->items = malloc (sizeof (void *) * v->capacity);
}

bool vector_add (vector *v, void *item)
{
	if (v->size == v->capacity) {
		v->capacity *= 2;
		void **buf = realloc (v->items, sizeof (void *) * v->capacity);
		if (buf == nullptr) {
			return false;
		}
		v->items = buf;
	}
	v->items[v->size++] = item;
	return true;
}

void vector_set (const vector *v, const size_t index, void *item)
{
	if (index < v->size) {
		v->items[index] = item;
	}
}

void *vector_get (const vector *v, const size_t index)
{
	if (index < v->size) {
		return v->items[index];
	}
	return nullptr;
}

bool vector_delete (vector *v, const size_t index)
{
	if (index < v->size) {
		memmove (&v->items[index], &v->items[index + 1],
			 sizeof (void *) * (v->size - index - 1));
		v->size--;
		return true;
	}
	return false;
}

void vector_free (vector *v)
{
	if (v == nullptr) return;
	v->size = 0;
	v->capacity = 0;
	free (v->items);
}

size_t vector_size (const vector *v)
{
	return v->size;
}
