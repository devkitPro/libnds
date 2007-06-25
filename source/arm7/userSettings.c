/*---------------------------------------------------------------------------------
	$Id: userSettings.c,v 1.1 2007-06-25 20:21:49 wntrmute Exp $

	Copyright (C) 2005
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

---------------------------------------------------------------------------------*/

#include <nds/arm7/serial.h>
#include <nds/system.h>
#include <string.h>

//---------------------------------------------------------------------------------
void readUserSettings() {
//---------------------------------------------------------------------------------

	PERSONAL_DATA slot1;
	PERSONAL_DATA slot2;

	short slot1count, slot2count;
	short slot1CRC, slot2CRC;
	
	readFirmware( 0x3FE00, &slot1, sizeof(PERSONAL_DATA));
	readFirmware( 0x3FF00, &slot2, sizeof(PERSONAL_DATA));
	readFirmware( 0x3FE70, &slot1count, 2);
	readFirmware( 0x3FF70, &slot2count, 2);
	readFirmware( 0x3FE72, &slot1CRC, 2);
	readFirmware( 0x3FF72, &slot2CRC, 2);

	void *currentSettings = &slot1;
	
	short calc1CRC = swiCRC16( 0xffff, &slot1, sizeof(PERSONAL_DATA));
	short calc2CRC = swiCRC16( 0xffff, &slot2, sizeof(PERSONAL_DATA));
	 
	if ( calc1CRC == slot1CRC && calc2CRC == slot2CRC ) { 
		currentSettings = (slot2count == (( slot2count + 1 ) & 0x7f) ? &slot2 : &slot1);
	} else {
		if ( calc2CRC == slot2CRC )
			currentSettings = &slot2;
	}
	memcpy ( PersonalData, currentSettings, sizeof(PERSONAL_DATA));
	
}
