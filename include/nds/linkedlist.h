/*---------------------------------------------------------------------------------

	A simple linked list data structure

  Copyright (C) 2008
			Jason Rogers (dovoto)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.


---------------------------------------------------------------------------------*/
/*! \file linkedlist.h
\brief A simple doubly linked, unsorted list implementation
*/

#include <malloc.h>

#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

typedef struct LinkedList{
	struct LinkedList *next;
	struct LinkedList *prev;
	void *data;
}LinkedList;

static inline

LinkedList* linkedlistAdd(LinkedList **front, void* data)
{
	LinkedList *node = (LinkedList*)malloc(sizeof(LinkedList));

	node->prev = 0;

	if(*front == 0)
	{	
		node->next = 0;

		node->data = data;

		*front = node;

		return node;
	}
    
	node->next = *front;
	
	(*front)->prev = node;

	return node;
}


static inline

void linkedlistRemove(LinkedList *node)
{
	if(node == 0) return;

	if(node->prev)
	{
		node->prev->next = node->next;
	}

	if(node->next)
	{
		node->next->prev = node->prev;
	}

	free(node);
}

#endif