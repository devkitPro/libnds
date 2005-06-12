//////////////////////////////////////////////////////////////////////
//
// timers.h -- Timer defines (both CPUs)
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

#ifndef NDS_TIMERS_INCLUDE
#define NDS_TIMERS_INCLUDE

//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////

#include <NDS/jtypes.h>

//////////////////////////////////////////////////////////////////////
// Timers ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

// The timers are fed with a 33.4 MHz source on the ARM9.  The
// ARM7 timing hasn't been tested yet, but is likely to
// be the same.

#define TIMER_FREQ(n) (-0x2000000/(n))

// Timer values (16 bit timers)
#define TIMER0_DATA    (*(vuint16*)0x04000100)
#define TIMER1_DATA    (*(vuint16*)0x04000104)
#define TIMER2_DATA    (*(vuint16*)0x04000108)
#define TIMER3_DATA    (*(vuint16*)0x0400010C)
#define TIMER_DATA(n)  (*(vuint16*)(0x04000100+(n<<2)))

// Timer control registers
#define TIMER0_CR   (*(vuint16*)0x04000102)
#define TIMER1_CR   (*(vuint16*)0x04000106)
#define TIMER2_CR   (*(vuint16*)0x0400010A)
#define TIMER3_CR   (*(vuint16*)0x0400010E)
#define TIMER_CR(n) (*(vuint16*)(0x04000102+(n<<2)))

// Timer control register contents
#define TIMER_ENABLE    (1<<7)

// Generate an irq when the timer overflows
#define TIMER_IRQ_REQ   (1<<6)

// When one timer down overflows, count up
#define TIMER_CASCADE   (TIMER_ENABLE|(1<<2))

// Count up at the rates below (in clock cycles)
#define TIMER_DIV_1     TIMER_ENABLE
#define TIMER_DIV_64    (TIMER_ENABLE|1)
#define TIMER_DIV_256   (TIMER_ENABLE|2)
#define TIMER_DIV_1024  (TIMER_ENABLE|3)


//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////
