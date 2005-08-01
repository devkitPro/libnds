/*---------------------------------------------------------------------------------
	$Id: image.c,v 1.2 2005-08-01 23:18:22 wntrmute Exp $


  Copyright (C) 2005
			Michael Noland (joat)
			Jason Rogers (dovoto)
			Dave Murphy (WinterMute)

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

	$Log: not supported by cvs2svn $
	Revision 1.1  2005/07/27 02:20:52  wntrmute
	add image functions
	

---------------------------------------------------------------------------------*/
#include <nds/jtypes.h>
#include <nds/arm9/image.h>
#include <malloc.h>

void image8to16(sImage* img)
 {
	 int i;

	 u16* temp = (u16*)malloc(img->height*img->width*2);
	 
	 for(i = 0; i < img->height * img->width; i++)
		 temp[i] = img->palette[img->data8[i]] | (1<<15);

	 free (img->data8);
	 free (img->palette);

	 img->bpp = 16;
	 img->data16 = temp;
 }

void image8to16trans(sImage* img, u8 transperentColor)
{
	 int i;
	 u8 c;
	 
	 u16* temp = (u16*)malloc(img->height*img->width*2);
	 
	 for(i = 0; i < img->height * img->width; i++)
	 {
		 c = img->data8[i];		
		 
		 if(c != transperentColor) 
			 temp[i] = img->palette[c] | (1<<15);
		 else
			temp[i] = img->palette[c]; 
	 }
	 free (img->data8);
	 free (img->palette);

	 img->bpp = 16;
	 img->data16 = temp;
}
void imageDestroy(sImage* img)
{
	if(img->data8) free (img->data8);
	if(img->palette && img->bpp == 8) free (img->palette);
}
