#include <nds/arm9/sprite.h>

#include <nds/arm9/input.h>
#include <nds/arm9/console.h>
#include <nds/arm9/trig_lut.h>
#include <nds/bios.h>

#include <stdlib.h>
#include <string.h>

SpriteEntry OamMemorySub[128];
SpriteEntry OamMemory[128];


OamState oamMain = 
{
   -1, 
   0, 
   NULL, 
   32, 
   {OamMemory}
};

OamState oamSub = 
{
   -1, 
   0, 
   NULL, 
   32, 
   {OamMemorySub}
};

void oamInit(OamState *oam, SpriteMapping mapping, bool extPalette)
{
   int i;
   int extPaletteFlag = extPalette ? 1 : 0;

   oam->gfxOffsetStep = (mapping & 3) + 5;
   
   int zero = 0;

   memset(oam->oamMemory, 0, sizeof(OamMemory));
   
   for(i = 0; i < 128; i++)
   {
      oam->oamMemory[i].isHidden = true;
   }

   for(i = 0; i < 32; i++)
   {
      oam->oamRotationMemory[i].hdx = (1<<8);
      oam->oamRotationMemory[i].vdy = (1<<8);
   }

   swiWaitForVBlank();

   if(oam == &oamMain)
   {
      memcpy(OAM, oam->oamMemory, (sizeof(OamMemory)));
      DISPLAY_CR &= ~DISPLAY_SPRITE_ATTR_MASK;
      DISPLAY_CR |= DISPLAY_SPR_ACTIVE | (mapping & 0xffffff0) | extPaletteFlag;      
   }
   else
   {
      memcpy(OAM_SUB, oam->oamMemory, (sizeof(OamMemory)));
      SUB_DISPLAY_CR &= ~DISPLAY_SPRITE_ATTR_MASK;
      SUB_DISPLAY_CR |= DISPLAY_SPR_ACTIVE | (mapping & 0xffffff0) | extPaletteFlag;      
   }
}

/**! void spritesDisable(void)
*    \brief Disables sprite rendering
*/
void oamDisable(OamState *oam)
{
   if(oam == &oamMain)
   {
      DISPLAY_CR &= ~DISPLAY_SPR_ACTIVE;
   }
   else
   {
      SUB_DISPLAY_CR &= ~DISPLAY_SPR_ACTIVE;
   }
}

void oamEnable(OamState *oam)
{
	if(oam == &oamMain)
   {
      DISPLAY_CR |= DISPLAY_SPR_ACTIVE;
   }
   else
   {
      SUB_DISPLAY_CR |= DISPLAY_SPR_ACTIVE;
   }
}

u16* oamGetGfxPtr(OamState *oam, int gfxOffsetIndex)
{
   if(gfxOffsetIndex < 0) return NULL;

   if(oam == &oamMain)
   {
      return &SPRITE_GFX[(gfxOffsetIndex << oam->gfxOffsetStep) >> 1];
   }
   else
   {
      return &SPRITE_GFX_SUB[(gfxOffsetIndex << oam->gfxOffsetStep) >> 1];
   }
}

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

void oamPrintAllocStatus(OamState *oam)
{
   AllocHeader *next = getAllocHeader(oam, oam->firstFree);

   int curOffset = oam->firstFree;

   while(curOffset < 1024 && curOffset < oam->allocBufferSize)
   {
      fprintf(stderr, "%i:%i:%i ", curOffset, next->nextFree, next->size);
      curOffset = next->nextFree;
      next = getAllocHeader(oam, next->nextFree);
   }   
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

unsigned int oamGfxPtrToOffset(OamState *oam, const void* offset)
{
   unsigned int temp = (unsigned int)offset;

   if(oam == &oamMain)
       temp -= (unsigned int)SPRITE_GFX;
   else
	   temp -= (unsigned int)SPRITE_GFX_SUB;
	   
   temp >>= oam->gfxOffsetStep;

   return temp;
}

u16* oamAllocateGfx(OamState *oam, SpriteSize size, SpriteColorFormat colorFormat)
{
   int bytes = (size & 0xFFF) << 5;

   if(colorFormat == SpriteColorFormat_16Color)
      bytes = bytes >> 1;
   else if(colorFormat == SpriteColorFormat_Bmp)
      bytes = bytes << 1;

   bytes = bytes >>  oam->gfxOffsetStep;

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

void oamSet(OamState* oam, int id,  int x, int y, int palette_alpha, SpriteSize size,SpriteColorFormat format, const void* gfxOffset, bool hide)
{  
   oam->oamMemory[id].isHidden = hide;
   oam->oamMemory[id].shape = (size >> 12) & 0x3;
   oam->oamMemory[id].size = (size >> 14) & 0x3;
   oam->oamMemory[id].gfxIndex = oamGfxPtrToOffset(oam, gfxOffset);
   oam->oamMemory[id].x = x;
   oam->oamMemory[id].y = y;
   oam->oamMemory[id].palette = palette_alpha; 

   if(format != SpriteColorFormat_Bmp)
   {
      oam->oamMemory[id].colorMode = format;
   }
   else
   {  
      oam->oamMemory[id].blendMode = format;
   }
}

void oamUpdate(OamState* oam)
{
   if(oam == &oamMain)
   {
      memcpy(OAM, oam->oamMemory, (sizeof(OamMemory)));
   }
   else
   {
       memcpy(OAM_SUB, oam->oamMemory, (sizeof(OamMemory)));
   }
}

void oamRotateScale(OamState* oam, int rotId, int angle, int sx, int sy)
{
    int ss = sinFixed(angle);
    int cc = cosFixed(angle);

    oam->oamRotationMemory[rotId].hdx = cc*sx>>12; 
    oam->oamRotationMemory[rotId].hdy =-ss*sx>>12;
    oam->oamRotationMemory[rotId].vdx = ss*sy>>12;;
    oam->oamRotationMemory[rotId].vdy = cc*sy>>12;
}

