/*---------------------------------------------------------------------------------
	$Id: keys.c,v 1.3 2005-07-25 02:19:01 desktopman Exp $

	key input code -- provides slightly higher level input forming

  Copyright (C) 2005
			Michael Noland (joat)
			Jason Rogers (dovoto)
			Christian Auby (DesktopMan)
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
	Revision 1.2  2005/07/14 08:00:57  wntrmute
	resynchronise with ndslib
	

---------------------------------------------------------------------------------*/
//////////////////////////////////////////////////////////////////////
//
// keys.h -- provides slightly higher level input forming
//
//  Contributed by DesktopMA
//
// version 0.1, February 14, 2005
//
//  Copyright (C) 2005 Michael Noland (joat) and Jason Rogers (dovoto)
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any
//  damages arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any
//  purpose, including commercial applications, and to alter it and
//  redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you
//     must not claim that you wrote the original software. If you use
//     this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and
//     must not be misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source
//     distribution.
//
// Changelog:
//   0.1: First version
//	
//
//////////////////////////////////////////////////////////////////////


#include <nds.h>
#include <stdlib.h>

#include "nds/arm9/keys.h"

#define KEYS_CUR (( ((~KEYS)&0x3ff) | (((~IPC->buttons)&3)<<10) | (((~IPC->buttons)<<6) & (KEY_TOUCH|KEY_LID) ))^KEY_LID)

u16 keys=0;
u16 keysold=0;

u16 oldx=0;
u16 oldy=0;

void keysInit()
{
	keys=0;
	keysold=0;
}

void scanKeys() {
	keysold=keys;
	keys=KEYS_CUR;

	if(abs(IPC->touchXpx - oldx)>20 || abs(IPC->touchYpx - oldy)>20)
		keys&=~KEY_TOUCH;

	oldx=IPC->touchXpx;
	oldy=IPC->touchYpx;
	
}

u32 keysHeld() {
	return keys;
}

u32 keysDown() {
	return (keys^keysold)&keys;
}

u32 keysUp() {
	return (keys^keysold)&(~keys);
}
