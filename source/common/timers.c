/*---------------------------------------------------------------------------------

	Timer API

  Copyright (C) 2008 - 2011
			Jason Rogers (dovoto)
			Mukunda Johnson (eKid)
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


---------------------------------------------------------------------------------*/
#include <nds/timers.h>
#include <nds/interrupts.h>

#include <nds/arm9/sassert.h>
//---------------------------------------------------------------------------------
void timerStart(unsigned timer, ClockDivider divider, u16 ticks, VoidFn callback){
//---------------------------------------------------------------------------------
	sassert(timer < 2, "timer must be in range 0 - 1");
	TIMER_CR(timer) = 0;
	TIMER_DATA(timer) = ticks;

	if(callback) {
		irqSet(IRQ_TIMER(timer), callback);
		irqEnable(IRQ_TIMER(timer));
		TIMER_CR(timer) = TIMER_IRQ_REQ | divider | TIMER_ENABLE;
	} else {
		TIMER_CR(timer) = divider | TIMER_ENABLE;
	}
}


static u16 elapsed[2];

//---------------------------------------------------------------------------------
u16 timerElapsed(unsigned timer) {
//---------------------------------------------------------------------------------
	sassert(timer < 2, "timer must be in range 0 - 1");
	u16 time = TIMER_DATA(timer);

	s32 result = (s32)time - (s32)elapsed[timer];

	//overflow...this will only be accurate if it has overflowed no more than once.
	if(result < 0) {
		result = time + (0x10000 - elapsed[timer]);
	}

	elapsed[timer] = time;

	return (u16) result;
}


//---------------------------------------------------------------------------------
u16 timerPause(unsigned timer) {
//---------------------------------------------------------------------------------
	sassert(timer < 2, "timer must be in range 0 - 1");
	TIMER_CR(timer) &= ~TIMER_ENABLE;
	u16 temp = timerElapsed(timer);
	elapsed[timer] = 0;
	return temp;
}

//---------------------------------------------------------------------------------
u16 timerStop(unsigned timer) {
//---------------------------------------------------------------------------------
	sassert(timer < 2, "timer must be in range 0 - 1");
	TIMER_CR(timer) = 0;
	u16 temp = timerElapsed(timer);
	elapsed[timer] = 0;
	return temp;
}


static u32 tickRef;

//---------------------------------------------------------------------------------
void cpuStartTiming(int unused) {
//---------------------------------------------------------------------------------
	tickInit();
	tickRef = tickGetCount();
}

//---------------------------------------------------------------------------------
u32 cpuGetTiming(void) {
//---------------------------------------------------------------------------------
	return (tickGetCount() - tickRef) * 64;
}
