/*
	include licence stuff?

	see dynamicArray for licence...
*/


#include <nds/dynamicArray.h>


void* DynamicArrayInit(DynamicArray* v, unsigned int initialSize)
{
	if(v == NULL)
	{
		return NULL;
	}

	v->cur_size = initialSize;
	v->data = (void**)malloc(sizeof(void*) * initialSize);

	return v->data;
}



void DynamicArrayDelete(DynamicArray* v)
{
	if(v == NULL)
	{
		return;
	}

	if(v->data != NULL)
	{
		free(v->data);
	}
}



void* DynamicArrayGet(DynamicArray* v, unsigned int index)
{
	if(v == NULL)
	{
		return NULL;
	}

	if(index >= v->cur_size)
	{
		return NULL;
	}

	return v->data[index];
}



bool DynamicArraySet(DynamicArray *v, unsigned int index, void* item)
{
	if(v == NULL)
	{
		return false;
	}

	if(index >= v->cur_size)
	{
		//resize the array, making sure it is bigger than index.
		unsigned int newSize = (v->cur_size * 2 > index ? v->cur_size * 2: index + 1);

		void** temp = (void**)realloc(v->data, sizeof(void*) * newSize);
		if(temp == NULL)
		{
			return false;
		}
		memset(v->data + v->cur_size, 0, sizeof(void*) * (newSize - v->cur_size));
		v->cur_size = newSize;
	}

	v->data[index] = item;
	return true;
}
