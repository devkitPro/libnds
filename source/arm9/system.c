/*---------------------------------------------------------------------------------

system.c -- System code

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

#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/interrupts.h>

//todo document
//

static void(*SDcallback)(int)=NULL;

//---------------------------------------------------------------------------------
void setSDcallback(void(*callback)(int)) {
//---------------------------------------------------------------------------------
	SDcallback = callback;
}

//---------------------------------------------------------------------------------
void powerOn(int bits) {
//---------------------------------------------------------------------------------
	if(bits & BIT(16))
		REG_POWERCNT |= bits & 0xFFFF;
	//else
	//	fifoSendValue32(FIFO_PM, PM_REQ_ON | (bits & 0xFFFF));
}

//---------------------------------------------------------------------------------
void powerOff(int bits) {
//---------------------------------------------------------------------------------
	if(bits & BIT(16))
		REG_POWERCNT &= ~(bits & 0xFFFF);
	//else
	//	fifoSendValue32(FIFO_PM, PM_REQ_OFF | (bits & 0xFFFF));
}

//---------------------------------------------------------------------------------
void enableSlot1() {
//---------------------------------------------------------------------------------

	//if(isDSiMode()) fifoSendValue32(FIFO_PM, PM_REQ_SLOT1_ENABLE);

}

//---------------------------------------------------------------------------------
void disableSlot1() {
//---------------------------------------------------------------------------------

	//if(isDSiMode()) fifoSendValue32(FIFO_PM, PM_REQ_SLOT1_DISABLE);

}
