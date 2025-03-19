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
#include <string.h>
#include "vector.h"

void vector_init(Vector *vector) {
    vector->capacity = 4;
    vector->size = 0;
    vector->items = malloc(sizeof(void *) * vector->capacity);
}

void vector_add(Vector *vector, void *item) {
    if (vector->size == vector->capacity) {
        vector->capacity *= 2;
        vector->items = realloc(vector->items, sizeof(void *) * vector->capacity);
    }
    vector->items[vector->size++] = item;
}

void vector_set(Vector *vector, size_t index, void *item) {
    if (index < vector->size) {
        vector->items[index] = item;
    }
}

void *vector_get(Vector *vector, size_t index) {
    if (index < vector->size) {
        return vector->items[index];
    }
    return NULL;
}

void vector_delete(Vector *vector, size_t index) {
    if (index < vector->size) {
        memmove(&vector->items[index], &vector->items[index + 1], sizeof(void *) * (vector->size - index - 1));
        vector->size--;
    }
}

void vector_free(Vector *vector) {
    free(vector->items);
}