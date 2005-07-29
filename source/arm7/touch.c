/*---------------------------------------------------------------------------------
	$Id: touch.c,v 1.4 2005-07-29 00:57:40 wntrmute Exp $

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
	Revision 1.3  2005/07/12 17:32:20  wntrmute
	updated file header
	
	Revision 1.2  2005/07/11 23:12:15  wntrmute
	*** empty log message ***

---------------------------------------------------------------------------------*/

#include "nds/arm7/touch.h"


// these are pixel positions of the two points you click when calibrating
#define TOUCH_CNTRL_X1   (*(vu8*)0x027FFCDC)
#define TOUCH_CNTRL_Y1   (*(vu8*)0x027FFCDD)
#define TOUCH_CNTRL_X2   (*(vu8*)0x027FFCE2)
#define TOUCH_CNTRL_Y2   (*(vu8*)0x027FFCE3)

// these are the corresponding touchscreen values:
#define TOUCH_CAL_X1   (*(vu16*)0x027FFCD8)
#define TOUCH_CAL_Y1   (*(vu16*)0x027FFCDA)
#define TOUCH_CAL_X2   (*(vu16*)0x027FFCDE)
#define TOUCH_CAL_Y2   (*(vu16*)0x027FFCE0)


//---------------------------------------------------------------------------------
uint16 touchRead(uint32 command) {
//---------------------------------------------------------------------------------
  uint16 result;
  SerialWaitBusy();

  // Write the command and wait for it to complete
  SERIAL_CR = SERIAL_ENABLE | 0xA01;
  SERIAL_DATA = command;
  SerialWaitBusy();

  // Write the second command and clock in part of the data
  SERIAL_DATA = 0;
  SerialWaitBusy();
  result = SERIAL_DATA;

  // Clock in the rest of the data (last transfer)
  SERIAL_CR = SERIAL_ENABLE | 0x201;
  SERIAL_DATA = 0;
  SerialWaitBusy();

  // Return the result
  return ((result & 0x7F) << 5) | (SERIAL_DATA >> 3);
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

// reading pixel position:
//---------------------------------------------------------------------------------
touchPosition touchReadXY() {
//---------------------------------------------------------------------------------

	
	touchPosition touchPos;

	static int TOUCH_WIDTH;
	static int TOUCH_HEIGHT;
	static int CNTRL_WIDTH;
	static int CNTRL_HEIGHT;
	
	if ( !touchInit ) {

		TOUCH_WIDTH  = TOUCH_CAL_X2 - TOUCH_CAL_X1;
		TOUCH_HEIGHT = TOUCH_CAL_Y2 - TOUCH_CAL_Y1;
		CNTRL_WIDTH  = TOUCH_CNTRL_X2 - TOUCH_CNTRL_X1;
		CNTRL_HEIGHT = TOUCH_CNTRL_Y2 - TOUCH_CNTRL_Y1;

		
		touchInit = true;

	}
	
	touchPos.x = ( touchRead(TSC_MEASURE_X) - (int16) TOUCH_CAL_X1) * CNTRL_WIDTH  / TOUCH_WIDTH + (int16) TOUCH_CNTRL_X1;
	touchPos.y = ( touchRead(TSC_MEASURE_Y) - (int16) TOUCH_CAL_Y1) * CNTRL_HEIGHT / TOUCH_HEIGHT + (int16) TOUCH_CNTRL_Y1;

	return touchPos;

}

