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

#ifndef MAP_H
#define MAP_H

#include <stdlib.h>

typedef struct mapnode {
	char *key;
	void *value;
	struct mapnode *next;
} mapnode;

typedef struct {
	mapnode **buckets;
	size_t capacity;
	size_t size;
} map;

void map_init (map *m);
bool map_put (map *m, const char *key, void *value);
void *map_get (const map *m, const char *key);
bool map_delete (map *m, const char *key);
void map_free (map *m);
size_t map_size (const map *m);
void map_keys (const map *m, char **keys);
void map_values (const map *m, void **values);
void map_items (const map *m, char **keys, void **values);

#endif /* MAP_H */
