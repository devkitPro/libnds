/*---------------------------------------------------------------------------------

	A simple vector like dynamic data structure

  Copyright (C) 2005
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
/*! \file dynamicArray.h
\brief A dynamically resizing array for general use
*/

#ifndef __DYNAMICARRAY_H__
#define __DYNAMICARRAY_H__

#include <stdlib.h>
#include <string.h>

/*! \struct DynamicArray
\brief A resizable array
*/
typedef struct
{
	void** data; /*!< \brief pointer to array of void pointers */
	int cur_size; /*!< \brief currently allocated size of the array */
}DynamicArray;


static inline 
/*! \fn void* DynamicArrayInit(DynamicArray* v, int initialSize)
\brief Initializes an array with the supplied initial size
\param v the array to initialize
\param initialSize the initial size to allocate
\return a pointer to the data (0 on error)
*/
void* DynamicArrayInit(DynamicArray* v, int initialSize)
{
	v->cur_size = initialSize;
	v->data = (void**)malloc(sizeof(void*) * initialSize);
	
	return v->data;
}

static inline
/*! \fn void* DynamicArrayDelete(DynamicArray* v)
\brief Frees memory allocated by the dynamic array
\param v the array to delete
*/
void DynamicArrayDelete(DynamicArray* v)
{
	if(v->data) free(v->data);
}

static inline 
/*! \fn void* DynamicArrayGet(DynamicArray* v, int index)
\brief Gets the entry at the supplied index
\param v The array to get from.
\param index The index of the data to get.
\return a The data or NULL if out of range.
*/
void* DynamicArrayGet(DynamicArray* v, int index)
{
	if(index >= v->cur_size)
	{
		return NULL;
	}
	
	return v->data[index];
}

static inline 
/*! \fn void DynamicArraySet(DynamicArray *v, int index, void* item)
\brief Sets the entry to the supplied value
\param v The array to set
\param index The index of the data to set (array will be resized to fit the index).
\param item The data to set.
*/
void DynamicArraySet(DynamicArray *v, int index, void* item)
{
	if(index >= v->cur_size) {
		v->data = (void**)realloc(v->data, sizeof(void*) * v->cur_size * 2);
		memset(v->data + v->cur_size, 0, sizeof(void*) * v->cur_size);
		v->cur_size *= 2;
	}
	
	v->data[index] = item;
}

#endif
