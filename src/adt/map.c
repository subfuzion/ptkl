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

#include "map.h"
#include <stdlib.h>
#include <string.h>

/*
 * map implemented using a hashmap with chaining for simplicity and good
 * general performance, although potentially at the cost of less than ideal
 * cache performance.
 */

#define INITIAL_CAPACITY 16

/* Set a limit for string duplication without being too restrictive */
#define MAX_KEY_LENGTH (8 * 1024)

unsigned long
hash( const char *str )
{
	/* djb2 algorithm */
	unsigned long hash = 5381;
	unsigned c;
	while ((c = (unsigned char)*str++)) {
		hash = (hash << 5) + hash + c;
	}
	return hash;
}

void
map_init( map *m )
{
	m->capacity = INITIAL_CAPACITY;
	m->size = 0;
	m->buckets = calloc(m->capacity, sizeof(mapnode *));
}

bool
map_put( map *m, const char *key, void *value )
{
	const unsigned long index = hash(key) % m->capacity;
	mapnode *new_node = malloc(sizeof(mapnode));
	if (new_node == nullptr) {
		return false;
	}
	new_node->key = strndup(key, MAX_KEY_LENGTH);
	new_node->value = value;
	new_node->next = m->buckets[index];
	m->buckets[index] = new_node;
	m->size++;
	return true;
}

void *
map_get( const map *m, const char *key )
{
	const unsigned long index = hash(key) % m->capacity;
	const mapnode *current = m->buckets[index];
	while (current != nullptr) {
		if (strncmp(current->key, key, MAX_KEY_LENGTH) == 0) {
			return current->value;
		}
		current = current->next;
	}
	return nullptr;
}

bool
map_delete( map *m, const char *key )
{
	const unsigned long index = hash(key) % m->capacity;
	mapnode *current = m->buckets[index];
	mapnode *previous = nullptr;
	while (current != nullptr) {
		if (strncmp(current->key, key, MAX_KEY_LENGTH) == 0) {
			if (previous == nullptr) {
				m->buckets[index] = current->next;
			} else {
				previous->next = current->next;
			}
			free(current->key);
			free(current);
			m->size--;
			return true;
		}
		previous = current;
		current = current->next;
	}
	return false;
}

void
map_free( map *m )
{
	for (size_t i = 0; i < m->capacity; i++) {
		mapnode *current = m->buckets[i];
		while (current != nullptr) {
			mapnode *next = current->next;
			free(current->key);
			free(current);
			current = next;
		}
	}
	m->size = 0;
	m->capacity = 0;
	free(m->buckets);
}

size_t
map_size( const map *m )
{
	return m->size;
}

void
map_keys( const map *m, char **keys )
{
	size_t index = 0;
	for (size_t i = 0; i < m->capacity; i++) {
		const mapnode *current = m->buckets[i];
		while (current != nullptr) {
			keys[index++] = current->key;
			current = current->next;
		}
	}
}

void
map_values( const map *m, void **values )
{
	size_t index = 0;
	for (size_t i = 0; i < m->capacity; i++) {
		const mapnode *current = m->buckets[i];
		while (current != nullptr) {
			values[index++] = current->value;
			current = current->next;
		}
	}
}

void
map_items( const map *m, char **keys, void **values )
{
	size_t index = 0;
	for (size_t i = 0; i < m->capacity; i++) {
		const mapnode *current = m->buckets[i];
		while (current != nullptr) {
			keys[index] = current->key;
			values[index] = current->value;
			index++;
			current = current->next;
		}
	}
}
