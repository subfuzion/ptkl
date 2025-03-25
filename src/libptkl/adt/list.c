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

#include "list.h"
#include <stdlib.h>

void
list_init (list *list)
{
	list->head = nullptr;
	list->size = 0;
}

bool
list_add (list *list, void *data)
{
	listnode *new_node = malloc (sizeof (listnode));
	if (new_node == nullptr) {
		return false;
	}
	new_node->data = data;
	listnode *node = list->head;
	if (node == nullptr) {
		list->head = new_node;
		goto success;
	}
	while (node->next != nullptr) {
		node = node->next;
	}
	node->next = new_node;
success:
	list->size++;
	return true;
}

void *
list_get (const list *list, const size_t index)
{
	if (index >= list->size) {
		return nullptr; // Index out of bounds
	}
	const listnode *current = list->head;
	for (size_t i = 0; i < index; i++) {
		current = current->next;
	}
	return current->data;
}

bool
list_delete (list *list, const size_t index)
{
	if (index >= list->size) {
		return false;
	}
	listnode *current = list->head;
	listnode *previous = nullptr;
	for (size_t i = 0; i < index; i++) {
		previous = current;
		current = current->next;
	}
	if (previous == nullptr) {
		list->head = current->next;
	} else {
		previous->next = current->next;
	}
	free (current);
	list->size--;
	return true;
}

void
list_free (list *list)
{
	listnode *current = list->head;
	while (current != nullptr) {
		listnode *next = current->next;
		free (current);
		current = next;
	}
	list->head = nullptr;
	list->size = 0;
}

size_t
list_size (const list *list)
{
	return list->size;
}
