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

#include "stack.h"
#include <stdlib.h>

void
stack_init( stack *s )
{
	s->head = nullptr;
	s->size = 0;
}

bool
stack_push( stack *s, void *data )
{
	stacknode *new_node = malloc(sizeof(stacknode));
	if (new_node == nullptr) {
		return false;
	}
	new_node->data = data;
	new_node->next = s->head;
	s->head = new_node;
	s->size++;
	return true;
}

void *
stack_pop( stack *s )
{
	void *data = nullptr;
	stacknode *node = s->head;
	if (node != nullptr) {
		s->head = node->next;
		data = node->data;
		free(node);
		s->size--;
	}
	return data;
}

void *
stack_peek( const stack *s )
{
	void *data = nullptr;
	stacknode *node = s->head;
	if (node != nullptr) {
		data = node->data;
	}
	return data;
}


void
stack_free( stack *s )
{
	stacknode *current = s->head;
	while (current != nullptr) {
		stacknode *next = current->next;
		free(current);
		current = next;
	}
	s->head = nullptr;
	s->size = 0;
}

size_t
stack_size( stack *s )
{
	return s->size;
}
