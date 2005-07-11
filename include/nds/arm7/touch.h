//////////////////////////////////////////////////////////////////////
//
// touch.h -- Touch Screen Controller header
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
//////////////////////////////////////////////////////////////////////

#ifndef ARM7_TOUCH_INCLUDE
#define ARM7_TOUCH_INCLUDE

//////////////////////////////////////////////////////////////////////

#ifndef ARM7
#error Touch screen is only available on the ARM7
#endif

//////////////////////////////////////////////////////////////////////

#include <nds/arm7/serial.h>

//////////////////////////////////////////////////////////////////////

#define TSC_MEASURE_TEMP1    0x84
#define TSC_MEASURE_Y        0x94
#define TSC_MEASURE_BATTERY  0xA4
#define TSC_MEASURE_Z1       0xB4
#define TSC_MEASURE_Z2       0xC4
#define TSC_MEASURE_X        0xD4
#define TSC_MEASURE_AUX      0xE4
#define TSC_MEASURE_TEMP2    0xF4

//////////////////////////////////////////////////////////////////////

uint16 touchRead(uint32 command);
uint32 touchReadTemperature(int * t1, int * t2);

//////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////
