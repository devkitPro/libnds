/*---------------------------------------------------------------------------------
	$Id: touch.c,v 1.7 2005-09-07 18:05:37 wntrmute Exp $

	Touch screen control for the ARM7

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

	1.	The origin of this software must not be misrepresented; you
			must not claim that you wrote the original software. If you use
			this software in a product, an acknowledgment in the product
			documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
			must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
			distribution.

	$Log: not supported by cvs2svn $
	Revision 1.6  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.5  2005/08/01 23:12:17  wntrmute
	extended touchReadXY to return touchscreen co-ordinates as well as screen co-ordinates

	Revision 1.4  2005/07/29 00:57:40  wntrmute
	updated file headers
	added touchReadXY function
	made header C++ compatible

	Revision 1.3  2005/07/12 17:32:20  wntrmute
	updated file header

	Revision 1.2  2005/07/11 23:12:15  wntrmute
	*** empty log message ***

---------------------------------------------------------------------------------*/

#include <nds/jtypes.h>
#include <nds/system.h>
#include <nds/arm7/touch.h>


//---------------------------------------------------------------------------------
uint16 touchRead(uint32 command) {
//---------------------------------------------------------------------------------
	uint16 result;
	SerialWaitBusy();

	// Write the command and wait for it to complete
	REG_SPICNT = SPI_ENABLE | SPI_BAUD_2MHz | SPI_DEVICE_TOUCH | SPI_CONTINUOUS; //0x0A01;
	REG_SPIDATA = command;
	SerialWaitBusy();

	// Write the second command and clock in part of the data
	REG_SPIDATA = 0;
	SerialWaitBusy();
	result = REG_SPIDATA;

	// Clock in the rest of the data (last transfer)
	REG_SPICNT = SPI_ENABLE | 0x201;
	REG_SPIDATA = 0;
	SerialWaitBusy();

	// Return the result
	return ((result & 0x7F) << 5) | (REG_SPIDATA >> 3);
}


//---------------------------------------------------------------------------------
uint32 touchReadTemperature(int * t1, int * t2) {
//---------------------------------------------------------------------------------
	*t1 = touchRead(TSC_MEASURE_TEMP1);
	*t2 = touchRead(TSC_MEASURE_TEMP2);
	return 8490 * (*t2 - *t1) - 273*4096;
}




// linear mapping can be used to go from touchscreen position to pixel position

static bool touchInit = false;
static u32 xscale, yscale;
static s32 xoffset, yoffset;

// reading pixel position:
//---------------------------------------------------------------------------------
touchPosition touchReadXY() {
//---------------------------------------------------------------------------------


	touchPosition touchPos;


	if ( !touchInit ) {


		xscale = ((PersonalData->calX2px - PersonalData->calX1px) << 16) / (PersonalData->calX2 - PersonalData->calX1);
		yscale = ((PersonalData->calY2px - PersonalData->calY1px) << 16) / (PersonalData->calY2 - PersonalData->calY1);

		xoffset = PersonalData->calX1 * xscale - (PersonalData->calX1px << 16);
		yoffset = PersonalData->calY1 * yscale - (PersonalData->calY1px << 16);

		touchInit = true;

	}

	touchPos.x = touchRead(TSC_MEASURE_X);
	touchPos.y = touchRead(TSC_MEASURE_Y);

	touchPos.px = ( touchPos.x * xscale - xoffset ) >>16;
	touchPos.py = ( touchPos.y * yscale - yoffset ) >>16;

	return touchPos;

}

