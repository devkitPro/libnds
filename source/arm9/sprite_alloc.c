#include <nds/arm9/sprite.h>

#include <nds/arm9/input.h>
#include <nds/arm9/console.h>
#include <nds/arm9/cache.h>
#include <nds/dma.h>
#include <nds/bios.h>

#include <stdlib.h>

//buffer grows depending on usage
void resizeBuffer(OamState *oam)
{
	oam->allocBufferSize *= 2;

	oam->allocBuffer = (AllocHeader*)realloc(oam->allocBuffer, sizeof(AllocHeader) * oam->allocBufferSize);
}

AllocHeader* getAllocHeader(OamState *oam, int index)
{
	//resize buffer if needed
	if(index >= oam->allocBufferSize)
		resizeBuffer(oam);

	return &oam->allocBuffer[index];
}


int simpleAlloc(OamState *oam, int size)
{
	//allocate the buffer if null
	if(oam->allocBuffer == NULL)
	{
		oam->allocBuffer = (AllocHeader*)malloc(sizeof(AllocHeader) * oam->allocBufferSize);

		getAllocHeader(oam, 0)->nextFree = 1024;
		getAllocHeader(oam, 0)->size = 1024;

	}

	u16 curOffset = oam->firstFree;

	//check for out of memory
	if(oam->firstFree >= 1024 || oam->firstFree == -1)
	{
		oam->firstFree = -1;
		return -1;
	}

	int misalignment = curOffset & (size - 1);

	if(misalignment)
		misalignment = size - misalignment;

	AllocHeader *next = getAllocHeader(oam, oam->firstFree);
	AllocHeader *last = next;

	//find a big enough block
	while(next->size - misalignment < size)
	{
		curOffset = next->nextFree;

		misalignment = curOffset & (size - 1);

		if(misalignment)
			misalignment = size - misalignment;

		if(curOffset >= 1024)
		{ 
			return -1;
		}

		last = next;
		next = getAllocHeader(oam, next->nextFree); 
	}

	//next should now point to a large enough block and last should point to the block prior

	////align to block size
	if(misalignment)
	{
		int tempSize = next->size;
		int tempNextFree = next->nextFree;

		curOffset += misalignment;

		next->size = misalignment;
		next->nextFree = curOffset;

		last = next;

		next = getAllocHeader(oam, curOffset);
		next->size = tempSize - misalignment;
		next->nextFree = tempNextFree; 
	}

	//is the block the first free block
	if(curOffset == oam->firstFree)
	{
		if(next->size == size)
		{
			oam->firstFree = next->nextFree;
		}
		else
		{
			oam->firstFree = curOffset + size;
			getAllocHeader(oam, oam->firstFree)->nextFree = next->nextFree;
			getAllocHeader(oam, oam->firstFree)->size = next->size - size;
		}
	}
	else
	{
		if(next->size == size)
		{
			last->nextFree = next->nextFree;
		}
		else
		{
			last->nextFree = curOffset + size;

			getAllocHeader(oam, curOffset + size)->nextFree = next->nextFree;
			getAllocHeader(oam, curOffset + size)->size = next->size - size;
		}
	}

	next->size = size;

	return curOffset;
}

void simpleFree(OamState *oam, int index)
{
	u16 curOffset = oam->firstFree;

	AllocHeader *next = getAllocHeader(oam, oam->firstFree);
	AllocHeader *current = getAllocHeader(oam, index);

	//if we were out of memory its trivial
	if(oam->firstFree == -1 || oam->firstFree >= 1024)
	{
		oam->firstFree = index;
		current->nextFree = 1024;
		return;
	}

	//if this index is before the first free block its also trivial
	if(index < oam->firstFree)
	{
		//check for abutment and combine if necessary
		if(index + current->size == oam->firstFree)
		{
			current->size += next->size;
			current->nextFree = next->nextFree;
		}
		else
		{
			current->nextFree = oam->firstFree;
		}

		oam->firstFree = index;

		return;
	}

	//otherwise locate the free block prior to index
	while(index > next->nextFree)
	{
		curOffset = next->nextFree;

		next = getAllocHeader(oam, next->nextFree);
	}


	//check if the next free block and current can be appended
	// [curOffset]         [index]          [next->nextFree] 
	//    next      | ~ |  current    | ~ |     nextFree

	//check if current abuts nextFree
	if(next->nextFree == index + current->size && next->nextFree < 1024)
	{
		current->size += getAllocHeader(oam, next->nextFree)->size;
		current->nextFree = getAllocHeader(oam, next->nextFree)->nextFree;
	}
	else
	{
		current->nextFree = next->nextFree;
	}

	//check if current abuts previous free block
	if (curOffset + next->size == index)
	{
		next->size += current->size;
		next->nextFree = current->nextFree;   
	}
	else
	{
		next->nextFree = index;
	}
}



u16* oamAllocateGfx(OamState *oam, SpriteSize size, SpriteColorFormat colorFormat)
{
	int bytes = SPRITE_SIZE_PIXELS(size);
    
	if(colorFormat == SpriteColorFormat_16Color)
		bytes = bytes >> 1;
	else if(colorFormat == SpriteColorFormat_Bmp)
		bytes = bytes << 1;

	bytes = bytes >> oam->gfxOffsetStep;

	int offset = simpleAlloc(oam, bytes ? bytes : 1);

	return oamGetGfxPtr(oam, offset);
}

void oamFreeGfx(OamState *oam, const void* gfxOffset)
{
	simpleFree(oam, oamGfxPtrToOffset(gfxOffset));
}

int oamCountFragments(OamState *oam)
{
	int frags = 1;

	int curOffset;

	AllocHeader *next = getAllocHeader(oam, oam->firstFree);

	curOffset = next->nextFree;

	while(curOffset < 1024 && curOffset < oam->allocBufferSize)
	{
		curOffset = next->nextFree;
		next = getAllocHeader(oam, next->nextFree);
		frags++;
	}

	return frags;
}