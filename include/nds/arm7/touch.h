/*---------------------------------------------------------------------------------
	$Id: touch.h,v 1.10 2008-02-21 23:39:23 dovoto Exp $

	Microphone control for the ARM7

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

---------------------------------------------------------------------------------*/
#ifndef ARM7_TOUCH_INCLUDE
#define ARM7_TOUCH_INCLUDE
//---------------------------------------------------------------------------------


#ifndef ARM7
#error Touch screen is only available on the ARM7
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <nds/arm7/serial.h>


#define SCREEN_WIDTH	256
#define SCREEN_HEIGHT	192


#define TSC_MEASURE_TEMP1    0x84
#define TSC_MEASURE_Y        0x90
#define TSC_MEASURE_BATTERY  0xA4
#define TSC_MEASURE_Z1       0xB4
#define TSC_MEASURE_Z2       0xC4
#define TSC_MEASURE_X        0xD0
#define TSC_MEASURE_AUX      0xE4
#define TSC_MEASURE_TEMP2    0xF4



typedef struct touchPosition {
	int16	x;    //!< Raw x value from the A2D
	int16	y;    //!< Raw x value from the A2D
	int16	px;   //!< Processes pixel X value
	int16	py;   //!< Processes pixel Y value
	int16	z1;   //!< Raw cross panel resistance
	int16	z2;   //!< Raw cross panel resistance
} touchPosition;

touchPosition touchReadXY(void);
uint16 touchRead(uint32 command);
uint32 touchReadTemperature(int * t1, int * t2);

#ifdef __cplusplus
}
#endif


//---------------------------------------------------------------------------------
#endif	// ARM7_TOUCH_INCLUDE
//---------------------------------------------------------------------------------

