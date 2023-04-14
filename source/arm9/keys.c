/*------------------------------------------------------------------------------
	$Id: keys.c,v 1.19 2008-12-11 19:56:44 dovoto Exp $

	key input code -- provides slightly higher level input forming

	Copyright (C) 2005
			Christian Auby (DesktopMan)
			Dave Murphy (WinterMute)

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

------------------------------------------------------------------------------*/

#include <stdlib.h>

#include <nds/ipc.h>
#include <nds/system.h>
#include <nds/arm9/input.h>

//------------------------------------------------------------------------------

static Keypad s_keypad;

static uint16 keysrepeat = 0;

static u8 delay = 30, repeat = 15, count = 30;

//------------------------------------------------------------------------------
void scanKeys(void) {
//------------------------------------------------------------------------------
	s_keypad.old = s_keypad.cur;
	s_keypad.cur = keysCurrent();

	if ( delay != 0 ) {
		if ( s_keypad.cur != s_keypad.old ) {
			count = delay;
			keysrepeat = keypadDown(&s_keypad);
		}
		count--;
		if ( count == 0 ) {
			count = repeat;
			keysrepeat = s_keypad.cur;
		}
	}
}

//------------------------------------------------------------------------------
uint32 keysHeld(void) {
//------------------------------------------------------------------------------
	return keypadHeld(&s_keypad);
}

//------------------------------------------------------------------------------
uint32 keysDown(void) {
//------------------------------------------------------------------------------
	return keypadDown(&s_keypad);
}

//------------------------------------------------------------------------------
uint32 keysDownRepeat(void) {
//------------------------------------------------------------------------------
	uint32 tmp = keysrepeat;

    keysrepeat = 0;

    return tmp;
}

//------------------------------------------------------------------------------
void keysSetRepeat( u8 setDelay, u8 setRepeat ) {
//------------------------------------------------------------------------------
    delay = setDelay ;
    repeat = setRepeat ;
    count = delay ;
    keysrepeat = 0 ;
}

//------------------------------------------------------------------------------
uint32 keysUp(void) {
//------------------------------------------------------------------------------
	return keypadUp(&s_keypad);
}

//------------------------------------------------------------------------------
uint32 keysCurrent(void) {
//------------------------------------------------------------------------------
	touchPosition tp;
	Keypad k = {0};
	keypadRead(&k);
	return keypadHeld(&k) | (touchRead(&tp) ? KEY_TOUCH : 0);
}
