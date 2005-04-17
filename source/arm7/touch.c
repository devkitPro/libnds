//////////////////////////////////////////////////////////////////////
//
// touch.cpp -- Touch screen control for the ARM7
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

#include "nds/jtypes.h"
#include "nds/bios.h"
#include "nds/arm7/touch.h"

//////////////////////////////////////////////////////////////////////

uint16 touchRead(uint32 command) {
  uint16 result;
  while (SERIAL_CR & SERIAL_BUSY) swiDelay(1);

  // Write the command and wait for it to complete
  SERIAL_CR = SERIAL_ENABLE | 0xA01;
  SERIAL_DATA = command;
  while (SERIAL_CR & SERIAL_BUSY) swiDelay(1);

  // Write the second command and clock in part of the data
  SERIAL_DATA = 0;
  while (SERIAL_CR & SERIAL_BUSY) swiDelay(1);
  result = SERIAL_DATA;

  // Clock in the rest of the data (last transfer)
  SERIAL_CR = SERIAL_ENABLE | 0x201;
  SERIAL_DATA = 0;
  while (SERIAL_CR & SERIAL_BUSY) swiDelay(1);

  // Return the result
  return ((result & 0x7F) << 5) | (SERIAL_DATA >> 3);
}

//////////////////////////////////////////////////////////////////////

uint32 touchReadTemperature(int * t1, int * t2) {
  *t1 = touchRead(TSC_MEASURE_TEMP1);
  *t2 = touchRead(TSC_MEASURE_TEMP2);
  return 8490 * (*t2 - *t1) - 273*4096;
}

//////////////////////////////////////////////////////////////////////
