//////////////////////////////////////////////////////////////////////
//
// clock.h -- Real-time clock code for the ARM7
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

#ifndef ARM7_CLOCK_INCLUDE
#define ARM7_CLOCK_INCLUDE

//////////////////////////////////////////////////////////////////////

#ifndef ARM7
#error The clock is only available on the ARM7
#endif

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////

#include "nds/jtypes.h"

//////////////////////////////////////////////////////////////////////

// RTC registers
#define WRITE_STATUS_REG1  0x60
#define READ_STATUS_REG1   0x61
#define STATUS_POC       (1<<7)  // read-only, cleared by reading (1 if just powered on)
#define STATUS_BLD       (1<<6)  // read-only, cleared by reading (1 if power dropped below the safety threshold)
#define STATUS_INT2      (1<<5)  // read-only, INT2 has occured
#define STATUS_INT1      (1<<4)  // read-only, INT1 has occured
#define STATUS_SC1       (1<<3)  // R/W scratch bit
#define STATUS_SC0       (1<<2)  // R/W scratch bit
#define STATUS_24HRS     (1<<1)  // 24 hour mode when 1, 12 hour mode when 0
#define STATUS_RESET     (1<<0)  // write-only, reset when 1 written

#define WRITE_STATUS_REG2  0x62
#define READ_STATUS_REG2   0x63
#define STATUS_TEST      (1<<7)  // 
#define STATUS_INT2AE    (1<<6)  // 
#define STATUS_SC3       (1<<5)  // R/W scratch bit
#define STATUS_SC2       (1<<4)  // R/W scratch bit

#define STATUS_32kE      (1<<3)  // Interrupt mode bits
#define STATUS_INT1AE    (1<<2)  // 
#define STATUS_INT1ME    (1<<1)  // 
#define STATUS_INT1FE    (1<<0)  // 

#define WRITE_DATA_REG1    0x64
#define READ_DATA_REG1     0x65
// reads full 7 bytes of current time

#define WRITE_DATA_REG2    0x64
#define READ_DATA_REG2     0x65
// reads last 3 bytes of current time

#define WRITE_INT_REG1     0x68
#define READ_INT_REG1      0x69

#define READ_INT_REG2      0x6A
#define WRITE_INT_REG2     0x6B

#define READ_CLOCK_ADJUST_REG  0x6C
#define WRITE_CLOCK_ADJUST_REG 0x6D
// clock-adjustment register

#define READ_FREE_REG      0x6E
#define WRITE_FREE_REG     0x6F

//////////////////////////////////////////////////////////////////////

void rtcReset(void);
void rtcTransaction(uint8 * command, uint32 commandLength, uint8 * result, uint32 resultLength);
void rtcGetTime(uint8 * time);
void rtcSetTime(uint8 * time);
void rtcGetData(uint8 * data, uint32 size);

void BCDToInteger(uint8 * data, uint32 length);
void integerToBCD(uint8 * data, uint32 length);

//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif

//////////////////////////////////////////////////////////////////////
