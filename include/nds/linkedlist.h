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
/*! \file
	\brief A simple doubly linked, unsorted list implementation.
*/

#include <malloc.h>

#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

//! A node for the linked list.
typedef struct LinkedList{
	struct LinkedList *next;	//!< A pointer to the next node.
	struct LinkedList *prev;	//!< A pointer to the previous node.
	void *data;					//!< A pointer to some data.
}LinkedList;


/*!
	\brief Adds data to a linked list.

	This will only store the pointer to the data, so you have to make sure that the pointer stays valid.

	\param front A pointer to a pointer to the front of the linked list (or a pointer to NULL if you don't have a linked list yet).
	\param data A pointer to the data you want to store.

	\return A pointer to the new node, which is also the new front.
*/
static inline LinkedList* linkedlistAdd(LinkedList **front, void* data)
{
	LinkedList *node = (LinkedList*)malloc(sizeof(LinkedList));

	//TODO: add check to see if malloc has succeeded/node is NULL.

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

	//shouldn't the data of the node be set here too?

	return node;
}

/*!
	\brief Removes a node from a linked list.

	The data pointer of the node will be lost after this, so make sure you don't need it anymore.

	\param node The node you want to remove.
*/
static inline void linkedlistRemove(LinkedList *node)
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
