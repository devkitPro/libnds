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

#ifndef __DYNAMICARRAY_H__
#define __DYNAMICARRAY_H__

#include <stdlib.h>
#include <string.h>

typedef struct
{
	void** data;
	int cur_size;
	
}DynamicArray;


static inline void* DynamicArrayInit(DynamicArray* v, int initialSize)
{
	v->cur_size = initialSize;
	v->data = (void**)malloc(sizeof(void*) * initialSize);
	
	return v->data;
}

static inline void* DynamicArrayGet(DynamicArray* v, int index)
{
	if(index >= v->cur_size)
	{
		return NULL;
	}
	
	return v->data[index];
}

static inline void DynamicArraySet(DynamicArray *v, int index, void* item)
{
	if(index >= v->cur_size)
	{
		v->cur_size *= 2;
		v->data = (void**)realloc(v->data, sizeof(void*) * v->cur_size);
	}
	
	v->data[index] = item;
}

#endif
