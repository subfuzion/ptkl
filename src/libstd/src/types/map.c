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
#include <stdio.h> /* for debugging */
#include <stdlib.h>
#include <string.h>

static bool map_resize (map *m, size_t new_capacity);

/*
 * map implemented using a hashmap with chaining for simplicity and good
 * general performance, although potentially at the cost of less than ideal
 * cache performance.
 */

#define INITIAL_CAPACITY 16
#define MAX_LOAD_FACTOR 0.75

/* hash function based on djb2 algorithm */
unsigned long hash (const char *str)
{
	unsigned long hash = 5381;
	unsigned c;
	while ((c = (unsigned char)*str++)) {
		hash = (hash << 5) + hash + c;
	}
	return hash;
}


void map_init (map *m)
{
	m->capacity = INITIAL_CAPACITY;
	m->size = 0;
	m->buckets = calloc (m->capacity, sizeof (mapnode *));
	if (m->buckets == nullptr) {
		fprintf (stderr,
			 "Error: map_init: failed to allocate memory for "
			 "buckets\n");
		exit (EXIT_FAILURE);
	}
}


bool map_put (map *m, const char *key, void *value)
{
	/* check load factor to determine whether resizing is necessary */
	if ((double)m->size / (double)m->capacity >= MAX_LOAD_FACTOR &&
	    !map_resize (m, m->capacity * 2)) {
		fprintf (stderr, "Error: map_put: unable to resize map\n");
		goto fail;
	}

	const unsigned long index = hash (key) % m->capacity;
	mapnode *new_node = malloc (sizeof (mapnode));
	if (new_node == nullptr) {
		fprintf (stderr,
			 "Error: map_put: unable to allocate new mapnode\n");
		goto fail;
	}

	new_node->key = strdup (key);
	if (new_node->key == nullptr) {
		fprintf (stderr,
			 "Error: map_put: unable to allocate new key\n");
		free (new_node);
		goto fail;
	}

	new_node->value = value;

	/* if key already exists, then update the value */
	mapnode *current = m->buckets[index];
	while (current != nullptr) {
		if (strcmp (current->key, key) == 0) {
			free (new_node->key);
			free (new_node);
			current->value = value;
			return true;
		}
		current = current->next;
	}

	new_node->next = m->buckets[index];
	m->buckets[index] = new_node;
	m->size++;
	return true;

fail:
	return false;
}


void *map_get (const map *m, const char *key)
{
	const unsigned long index = hash (key) % m->capacity;
	const mapnode *current = m->buckets[index];
	while (current != nullptr) {
		if (strcmp (current->key, key) == 0) {
			return current->value;
		}
		current = current->next;
	}
	return nullptr;
}


bool map_delete (map *m, const char *key)
{
	const unsigned long index = hash (key) % m->capacity;
	mapnode *current = m->buckets[index];
	mapnode *previous = nullptr;
	while (current != nullptr) {
		if (strcmp (current->key, key) == 0) {
			if (previous == nullptr) {
				m->buckets[index] = current->next;
			} else {
				previous->next = current->next;
			}
			free (current->key);
			free (current);
			m->size--;
			return true;
		}
		previous = current;
		current = current->next;
	}
	return false;
}


void map_free (map *m)
{
	for (size_t i = 0; i < m->capacity; i++) {
		mapnode *current = m->buckets[i];
		while (current != nullptr) {
			mapnode *next = current->next;
			free (current->key);
			free (current);
			current = next;
		}
	}
	m->size = 0;
	m->capacity = 0;
	free (m->buckets);
}


/* get number of elements in hashmap */
size_t map_size (const map *m)
{
	return m->size;
}


/* resize hashmap when load factor is exceeded */
static bool map_resize (map *m, const size_t new_capacity)
{
	mapnode **new_buckets = calloc (new_capacity, sizeof (mapnode *));
	if (new_buckets == nullptr) {
		return false;
	}

	/* existing elements must be rehashed into new buckets */
	for (size_t i = 0; i < m->capacity; i++) {
		mapnode *current = m->buckets[i];
		while (current != nullptr) {
			mapnode *next = current->next;
			const unsigned long new_index =
				hash (current->key) % new_capacity;
			current->next = new_buckets[new_index];
			new_buckets[new_index] = current;
			current = next;
		}
	}

	free (m->buckets);
	m->buckets = new_buckets;
	m->capacity = new_capacity;
	return true;
}


void map_keys (const map *m, char **keys)
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


void map_values (const map *m, void **values)
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


void map_items (const map *m, char **keys, void **values)
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
