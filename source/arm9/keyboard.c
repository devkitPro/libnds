/*---------------------------------------------------------------------------------

	keyboard.h -- stdin integration for a simple keyboard

	Copyright (C) 2007
		Jason Rogers (Dovoto) 

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
	must not claim that you wrote the original software. If you use
	this software in a product, an acknowledgment in the product
	documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
	must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
	distribution.

---------------------------------------------------------------------------------*/

#include <sys/iosupport.h>
#include "keyboardGfx.h"
#include <nds/ndstypes.h>
#include <nds/arm9/keyboard.h>
#include <nds/arm9/input.h>
#include <nds/arm9/background.h>
#include <string.h>
#include <stdio.h>


#define KEY_BUFFER_SIZE 256


//keybuffer state
int keyBufferOffset = 0;
int keyBufferLength = 0; 
int lastKey = -1;

char keyBuffer[KEY_BUFFER_SIZE];

// negative values are keys with no sensible ascii representation.  
// numbers are chosen to mimic ascii control sequences.
enum { 
   NOKEY         = -1,     DVK_FOLD      = -23, 
   DVK_TAB       =  9,     DVK_BACKSPACE =  8, 
   DVK_CAPS      = -15,    DVK_SHIFT     = -14,
   DVK_SPACE     =  32,    DVK_MENU      = -5, 
   DVK_ENTER     =  10,    DVK_CTRL      = -16,
   DVK_UP        = -17,    DVK_RIGHT     = -18,
   DVK_DOWN      = -19,    DVK_LEFT      = -20,
   DVK_ALT       = -26
};

//default keyboard map
int SimpleKbdLower[] = {

   DVK_FOLD, DVK_FOLD, NOKEY, '1', '1', '2', '2', '3', '3', '4', '4', '5', '5', '6', '6', '7', 
   '7', '8', '8', '9', '9', '0', '0', '-', '-', '+', '+', DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE ,

   DVK_TAB, DVK_TAB, DVK_TAB, DVK_TAB, 'q', 'q', 'w', 'w', 'e', 'e', 'r', 'r', 't', 't', 'y', 'y', 
   'u', 'u', 'i', 'i', 'o', 'o', 'p', 'p', '[', '[', ']', ']', '\\', '\\', '`', '`' ,

   DVK_CAPS, DVK_CAPS, DVK_CAPS, DVK_CAPS, DVK_CAPS, 'a', 'a', 's', 's', 'd', 'd', 'f', 'f', 'g', 'g', 'h', 
   'h', 'j', 'j', 'k', 'k', 'l', 'l', ';', ';', '\'', '\'', DVK_ENTER, DVK_ENTER, DVK_ENTER, DVK_ENTER, DVK_ENTER ,

   DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, 'z', 'z', 'x', 'x', 'c', 'c', 'v', 'v', 'b', 'b', 
   'n', 'n', 'm', 'm', ',', ',', '.', '.', '/', '/', NOKEY, NOKEY, DVK_UP, DVK_UP, NOKEY, NOKEY ,

   DVK_CTRL,	DVK_CTRL,  DVK_CTRL,  DVK_CTRL,  DVK_CTRL,  DVK_ALT,   DVK_ALT,   DVK_ALT, 
   DVK_ALT,	DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, 
   DVK_SPACE,	DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_MENU,  DVK_MENU,  DVK_MENU, 
   DVK_MENU,	DVK_MENU,  DVK_LEFT,  DVK_LEFT,  DVK_DOWN,  DVK_DOWN,  DVK_RIGHT, DVK_RIGHT 

};

int SimpleKbdUpper[] = {

   DVK_FOLD, DVK_FOLD, NOKEY, '!', '!', '@', '@', '#', '#', '$', '$', '%', '%', '^', '^', '&', 
   '&', '*', '*', '(', '(', ')', ')', '_', '_', '+', '+', DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE ,

   DVK_TAB, DVK_TAB, DVK_TAB, DVK_TAB, 'Q', 'Q', 'W', 'W', 'E', 'E', 'R', 'R', 'T', 'T', 'Y', 'Y', 
   'U', 'U', 'I', 'I', 'O', 'O', 'P', 'P', '{', '{', '}', '}', '|', '|', '~', '~' ,

   DVK_CAPS, DVK_CAPS, DVK_CAPS, DVK_CAPS, DVK_CAPS, 'A', 'A', 'S', 'S', 'D', 'D', 'F', 'F', 'G', 'G', 'H', 
   'H', 'J', 'J', 'K', 'K', 'L', 'L', ':', ':', '"', '"', DVK_ENTER, DVK_ENTER, DVK_ENTER, DVK_ENTER, DVK_ENTER ,

   DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, 'Z', 'Z', 'X', 'X', 'C', 'C', 'V', 'V', 'B', 'B', 
   'N', 'N', 'M', 'M', '<', '<', '>', '>', '?', '?', NOKEY, NOKEY, DVK_UP, DVK_UP, NOKEY, NOKEY ,

   DVK_CTRL,	DVK_CTRL,  DVK_CTRL,  DVK_CTRL,  DVK_CTRL,  DVK_ALT,   DVK_ALT,   DVK_ALT, 
   DVK_ALT,	DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, 
   DVK_SPACE,	DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_MENU,  DVK_MENU,  DVK_MENU, 
   DVK_MENU,	DVK_MENU,  DVK_LEFT,  DVK_LEFT,  DVK_DOWN,  DVK_DOWN,  DVK_RIGHT, DVK_RIGHT 

};

KeyMap capsLock = 
{
   keyboardGfxMap,
   keyboardGfxMap + 32 * 20,
   SimpleKbdUpper,
   32,
   5
};
KeyMap lowerCase = 
{
   keyboardGfxMap + 32 * 10,
   keyboardGfxMap + 32 * 30,
   SimpleKbdLower,
   32,
   5
};


Keyboard defaultKeyboard = 
{
   3, //background
   1, //display on sub screen 
   0, //offset x
   0, //offset y
   8, //grid width 
   16, //grid height
   Lower, //start with lower case
   0, //shifted
   0, //visible
   &lowerCase, //keymap for lowercase 
   &capsLock, //keymap for caps lock
   0, //keymap for numeric entry
   0, //keymap for reduced footprint
   (const u16*)keyboardGfxTiles,  //graphics tiles  
   keyboardGfxTilesLen, //graphics tiles length
   keyboardGfxPal, //palette
   keyboardGfxPalLen, //size of palette
   30, //map base
   1, //tile base
   0, //tile offset
   0, //keypress callback
   0, //key release callback
};

Keyboard *curKeyboard = &defaultKeyboard;


int keyboardGetKey(int x, int y)
{
   //todo: check lower bounds as well
   if(x - curKeyboard->offset_x < 0 || y - curKeyboard->offset_y < 0)
      return NOKEY;

   if(curKeyboard->state == Lower)
   {
      lastKey = curKeyboard->lower->keymap[((x - curKeyboard->offset_x) / curKeyboard->grid_width) + ((y + curKeyboard->offset_y) / curKeyboard->grid_height) * 32];
   }
   else if(curKeyboard->state == Upper)
   {
      lastKey =  curKeyboard->upper->keymap[((x - curKeyboard->offset_x) / curKeyboard->grid_width) + ((y + curKeyboard->offset_y) / curKeyboard->grid_height) * 32];
   }

   return lastKey;
}

void keyboardShiftState()
{
   if(curKeyboard->state == Upper)
   {
      curKeyboard->state = Lower;
      dmaCopy(curKeyboard->lower->mapDataPressed, bgGetMapPtr(curKeyboard->background), 
         curKeyboard->lower->width * curKeyboard->lower->height * curKeyboard->grid_height * curKeyboard->grid_width / 64 * 2);
   }
   else
   {
      curKeyboard->state = Upper;
      dmaCopy(curKeyboard->upper->mapDataPressed, bgGetMapPtr(curKeyboard->background), 
         curKeyboard->upper->width * curKeyboard->upper->height * curKeyboard->grid_height * curKeyboard->grid_width / 64 * 2);
   }
}
void keyboardUpdate(void)
{
   static int pressed = 0;
   touchPosition touch;


   scanKeys();
   u32 keys = keysDown();

   if(pressed)
   {
      if(!(keys & KEY_TOUCH))
      {
         pressed = 0;

         if(curKeyboard->OnKeyReleased)
            curKeyboard->OnKeyReleased(lastKey);
      }
   }
   else
   {
      if(keys & KEY_TOUCH)
      {	
         touchRead(&touch);

         int key = keyboardGetKey(touch.px, touch.py); 

         if(key == NOKEY)
         {
            return;
         }

         pressed = 1;
 
         if(key == DVK_CAPS)
         {
            keyboardShiftState();
            return;
         }
         else if(key == DVK_SHIFT)
         {
            keyboardShiftState();
            curKeyboard->shifted = curKeyboard->shifted ? 0 : 1;
            return;
         }

         if(curKeyboard->shifted)
         {
            keyboardShiftState();
            curKeyboard->shifted = 0;
         }

         if(key >= 0)
         {
            keyBuffer[keyBufferOffset++] = (char)key;

            if(keyBufferLength < KEY_BUFFER_SIZE)
            {
               keyBufferLength++;
            }

            if(keyBufferOffset >= KEY_BUFFER_SIZE)
            {
               keyBufferOffset = 0;
            }
         }
         if(curKeyboard->OnKeyPressed)
            curKeyboard->OnKeyPressed(lastKey);
      }
   }
}

int keyboardRead(struct _reent *r, int unused, char *ptr, int len)
{	
   int wasHidden = 0;
   int tempLen;
   int c = NOKEY;

   if(!curKeyboard->visible)
   {
      wasHidden = 1;
      keyboardShow();
   }

   do
   {
      keyboardUpdate();

   }while(keyBufferLength <= 0 || (keyBufferLength < KEY_BUFFER_SIZE && lastKey != DVK_ENTER));

   tempLen = keyBufferLength;

   while(len > 0 && keyBufferLength > 0)
   {
      c = keyBuffer[(keyBufferOffset - keyBufferLength) % 256];

      *ptr++ = c;

      keyBufferLength--;
      len--;
   }

   if(wasHidden)
   {
      keyboardHide();
   }

   return tempLen;
}

const devoptab_t std_in = 
{
   "stdin",
   0,
   NULL,
   NULL,
   NULL,
   keyboardRead,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL
};


Keyboard* keyboardGetDefault(void)
{
   return &defaultKeyboard;
}




void keyboardInit(Keyboard* keyboard)
{     
   if(keyboard->keyboardOnSub)
      keyboard->background = bgInitSub(keyboard->background, BgType_Text4bpp, BgSize_T_256x256, keyboard->mapBase, keyboard->tileBase);
   else
      keyboard->background = bgInit(keyboard->background, BgType_Text4bpp, BgSize_T_256x256, keyboard->mapBase, keyboard->tileBase);

   if(keyboard->visible)
      bgShow(keyboard->background);   
   else
      bgHide(keyboard->background);

   memset(bgGetMapPtr(keyboard->background), 0, 64);

  
   dmaCopy(keyboard->tiles, bgGetGfxPtr(keyboard->background) + keyboard->tileOffset / 2, keyboard->tileLen);
   dmaCopy(keyboard->palette, BG_PALETTE_SUB, keyboard->paletteLen);
   dmaCopy(keyboard->lower->mapDataPressed, bgGetMapPtr(keyboard->background), 
     keyboard->lower->width * keyboard->lower->height * keyboard->grid_height * keyboard->grid_width / 64 * 2);

   BG_PALETTE_SUB[255] = RGB15(31,31,31);

   keyboard->offset_x = 0;
   keyboard->offset_y = -192 + keyboard->lower->height * keyboard->grid_height;

   devoptab_list[STD_IN] = &std_in;
}



void keyboardShow(void)
{
   int i;

   curKeyboard->visible = 1;

   bgSetScroll(curKeyboard->background, 0, -192);
 
   bgShow(curKeyboard->background);

   for(i = -192; i < curKeyboard->offset_y; i += 3)
   {
      bgSetScroll(curKeyboard->background, 0, i);
      swiWaitForVBlank();
   }

   bgSetScroll(curKeyboard->background, 0, curKeyboard->offset_y);
}

void keyboardHide(void)
{
   int i;

   curKeyboard->visible = 0;

   for(i = curKeyboard->offset_y; i < -192; i--)
   {
      bgSetScroll(curKeyboard->background, 0, i);

      swiWaitForVBlank();
   }

   bgHide(curKeyboard->background);
}

int keyboardGetChar(void)
{
   int pressed;


   while(1)
   {
      scanKeys();
      pressed = keysDown();	

      if(pressed & KEY_TOUCH)
      {
         touchPosition touch;
         touchRead(&touch);

         int key = keyboardGetKey(touch.px, touch.py);

         if(key != NOKEY)
            return key;
      }
   }

   return 0;
}

void keyboardGetString(char * buffer, int maxLen)
{
   char *end = buffer + maxLen;
   char c;

   while(buffer < end)
   {	
      c = (char)keyboardGetChar();

      if(c == DVK_ENTER)
         break;

      *buffer++ = c;
   }
   *buffer = 0;
}




