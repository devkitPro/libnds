#include <nds/arm9/sprite.h>

#include <nds/arm9/input.h>
#include <nds/arm9/console.h>
#include <nds/arm9/cache.h>
#include <nds/dma.h>
#include <nds/bios.h>

#include <stdlib.h>

// A terse macro to convert an allocation buffer index
// into an AllocHeader. Assumes 'oam' is in the current scope.

#define AH(id) getAllocHeader(oam, id)


//buffer grows depending on usage
static void resizeBuffer(OamState *oam)
{
	oam->allocBufferSize *= 2;

	oam->allocBuffer = (AllocHeader*)realloc(oam->allocBuffer, sizeof(AllocHeader) * oam->allocBufferSize);
}

static AllocHeader* getAllocHeader(OamState *oam, int index)
{
	// Note that this function may resize the allocBuffer any time you refer to
	// a new 'index' for the first time. This will invalidate any pointers that
	// getAllocHeader() has previously returned, since the buffer may have moved
	// to a different location in memory. The pointers returned by this function
	// must be discarded any time a new allocHeader may have been allocated.

	//resize buffer if needed
	if(index >= oam->allocBufferSize)
		resizeBuffer(oam);

	return &oam->allocBuffer[index];
}

void oamAllocReset(OamState *oam)
{
	//allocate the buffer if null
	if(oam->allocBuffer == NULL)
	{
		oam->allocBuffer = (AllocHeader*)malloc(sizeof(AllocHeader) * oam->allocBufferSize);
	}

	AH(0)->nextFree = 1024;
	AH(0)->size = 1024;
}

static int simpleAlloc(OamState *oam, int size)
{
	if(oam->allocBuffer == NULL)
	{
		oamAllocReset(oam);
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

	int next = oam->firstFree;
	int last = next;

	//find a big enough block
	while(AH(next)->size - misalignment < size)
	{
		curOffset = AH(next)->nextFree;

		misalignment = curOffset & (size - 1);

		if(misalignment)
			misalignment = size - misalignment;

		if(curOffset >= 1024)
		{ 
			return -1;
		}

		last = next;
		next = curOffset;
	}

	//next should now point to a large enough block and last should point to the block prior

	////align to block size
	if(misalignment)
	{
		int tempSize = AH(next)->size;
		int tempNextFree = AH(next)->nextFree;

		curOffset += misalignment;

		AH(next)->size = misalignment;
		AH(next)->nextFree = curOffset;

		last = next;
		next = curOffset;

		AH(next)->size = tempSize - misalignment;
		AH(next)->nextFree = tempNextFree; 
	}

	//is the block the first free block
	if(curOffset == oam->firstFree)
	{
		if(AH(next)->size == size)
		{
			oam->firstFree = AH(next)->nextFree;
		}
		else
		{
			oam->firstFree = curOffset + size;
			AH(oam->firstFree)->nextFree = AH(next)->nextFree;
			AH(oam->firstFree)->size = AH(next)->size - size;
		}
	}
	else
	{
		if(AH(next)->size == size)
		{
			AH(last)->nextFree = AH(next)->nextFree;
		}
		else
		{
			AH(last)->nextFree = curOffset + size;

			AH(curOffset + size)->nextFree = AH(next)->nextFree;
			AH(curOffset + size)->size = AH(next)->size - size;
		}
	}

	AH(next)->size = size;

	return curOffset;
}

static void simpleFree(OamState *oam, int index)
{
	u16 curOffset = oam->firstFree;

	int next = oam->firstFree;
	int current = index;

	//if we were out of memory its trivial
	if(oam->firstFree == -1 || oam->firstFree >= 1024)
	{
		oam->firstFree = index;
		AH(current)->nextFree = 1024;
		return;
	}

	//if this index is before the first free block its also trivial
	if(index < oam->firstFree)
	{
		//check for abutment and combine if necessary
		if(index + AH(current)->size == oam->firstFree)
		{
			AH(current)->size += AH(next)->size;
			AH(current)->nextFree = AH(next)->nextFree;
		}
		else
		{
			AH(current)->nextFree = oam->firstFree;
		}

		oam->firstFree = index;

		return;
	}

	//otherwise locate the free block prior to index
	while(index > AH(next)->nextFree)
	{
		curOffset = AH(next)->nextFree;

		next = AH(next)->nextFree;
	}


	//check if the next free block and current can be appended
	// [curOffset]         [index]          [next->nextFree] 
	//    next      | ~ |  current    | ~ |     nextFree

	//check if current abuts nextFree
	if(AH(next)->nextFree == index + AH(current)->size && AH(next)->nextFree < 1024)
	{
		AH(current)->size += AH(AH(next)->nextFree)->size;
		AH(current)->nextFree = AH(AH(next)->nextFree)->nextFree;
	}
	else
	{
		AH(current)->nextFree = AH(next)->nextFree;
	}

	//check if current abuts previous free block
	if (curOffset + AH(next)->size == index)
	{
		AH(next)->size += AH(current)->size;
		AH(next)->nextFree = AH(current)->nextFree;   
	}
	else
	{
		AH(next)->nextFree = index;
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
	simpleFree(oam, oamGfxPtrToOffset(oam, gfxOffset));
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
