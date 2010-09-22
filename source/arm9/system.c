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
#include <nds/fifocommon.h>
#include <nds/interrupts.h>
#include <nds/fifomessages.h>
#include <libnds_internal.h>

//todo document
//


//---------------------------------------------------------------------------------
// Handle system requests from the arm7
//---------------------------------------------------------------------------------
void powerValueHandler(u32 value, void* data){
//---------------------------------------------------------------------------------
	switch(value) {
	case PM_REQ_SLEEP:
		systemSleep();
		break;
	}
}

//---------------------------------------------------------------------------------
void systemMsgHandler(int bytes, void* user_data){
//---------------------------------------------------------------------------------
	FifoMessage msg;

	fifoGetDatamsg(FIFO_SYSTEM, bytes, (u8*)&msg);

	switch (msg.type) {
	case SYS_INPUT_MESSAGE:
		setTransferInputData(&(msg.SystemInput.touch), msg.SystemInput.keys);
		break;
	}
}

//---------------------------------------------------------------------------------
void systemSleep(void) {
//---------------------------------------------------------------------------------
   fifoSendValue32(FIFO_PM, PM_REQ_SLEEP);
  
   //100ms
   swiDelay(419000);
}


//---------------------------------------------------------------------------------
void powerOn(int bits) {
//---------------------------------------------------------------------------------
	if(bits & BIT(16))
		REG_POWERCNT |= bits & 0xFFFF;
	else
		fifoSendValue32(FIFO_PM, PM_REQ_ON | (bits & 0xFFFF));
}

//---------------------------------------------------------------------------------
void powerOff(int bits) {
	if(bits & BIT(16))
		REG_POWERCNT &= ~(bits & 0xFFFF);
	else
		fifoSendValue32(FIFO_PM, PM_REQ_OFF | (bits & 0xFFFF));
}

//---------------------------------------------------------------------------------
void ledBlink(int bm) {
//---------------------------------------------------------------------------------
	fifoSendValue32(FIFO_PM, PM_REQ_LED | bm);
}

//---------------------------------------------------------------------------------
u32 getBatteryLevel() {
//---------------------------------------------------------------------------------
	fifoSendValue32(FIFO_PM, PM_REQ_BATTERY);
	while(!fifoCheckValue32(FIFO_SYSTEM)); //swiIntrWait(1,IRQ_FIFO_NOT_EMPTY);
	return fifoGetValue32(FIFO_SYSTEM);
}
