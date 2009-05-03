/*---------------------------------------------------------------------------------

	Timer API

  Copyright (C) 2008 - 2009
			Jason Rogers (dovoto)
			Mukunda Johnson (eKid)

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
void timerStart(int channel, ClockDivider divider, u16 ticks, fp callback){
//---------------------------------------------------------------------------------
	
	TIMER_DATA(channel) = ticks;
	
	if(callback)
	{
		irqSet(IRQ_TIMER(channel), callback);
		irqEnable(IRQ_TIMER(channel));
		TIMER_CR(channel) = TIMER_IRQ_REQ | divider | TIMER_ENABLE;
	}
	else
	{
		TIMER_CR(channel) = divider | TIMER_ENABLE;
	}
	
	
	
}

u16 elapsed[4] = {0};
//---------------------------------------------------------------------------------
u16 timerElapsed(int channel){
//---------------------------------------------------------------------------------
	u16 time = TIMER_DATA(channel);

	s32 result = (s32)elapsed[channel] - (s32)time;

	//overflow...this will only be accurate if it has overflowed 
	//no more than once
	if(result < 0)
	{
		result = time + (0x10000 - elapsed[channel]);
	}

	elapsed[channel] = time;

	return (u16) result;
}


/*
  CPU Usage - http://forums.devkitpro.org/viewtopic.php?f=6&t=415

  original Source by eKid
  adapted by Ryouarashi and Weirdfox
*/

static int localTimer = 0;

//---------------------------------------------------------------------------------
void cpuStartTiming(u32 timer) {
//---------------------------------------------------------------------------------
	sassert(timer < 3, "timer must be in range 0 - 2");
	localTimer = timer;

    TIMER_CR(timer) = 0;
    TIMER_CR(timer+1) = 0;

    TIMER_DATA(timer) = 0;
    TIMER_DATA(timer+1) = 0;

    TIMER_CR(timer+1) = TIMER_CASCADE | TIMER_ENABLE;
    TIMER_CR(timer) = TIMER_ENABLE;
}

//---------------------------------------------------------------------------------
u32 cpuEndTiming() {
//---------------------------------------------------------------------------------
    TIMER_CR(localTimer) = 0;
    TIMER_CR(localTimer+1) = 0;

    return ( (TIMER_DATA(localTimer) | (TIMER_DATA(localTimer+1)<<16) ));
}

