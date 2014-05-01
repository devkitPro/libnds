/*---------------------------------------------------------------------------------

	Firmware Functions

	Copyright (C) 20014
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
#include <nds/fifocommon.h>
#include <nds/fifomessages.h>
#include <nds/arm9/cache.h>

//---------------------------------------------------------------------------------
void readFirmware(u32 address, void *buffer, u32 length) {
//---------------------------------------------------------------------------------

	FifoMessage msg;

	msg.type = FW_READ;
	msg.fwParams.address = address;
	msg.fwParams.buffer = buffer;
	msg.fwParams.length = length;

	fifoSendDatamsg(FIFO_FIRMWARE, sizeof(msg), (u8*)&msg);

	while(!fifoCheckValue32(FIFO_FIRMWARE));
	fifoGetValue32(FIFO_FIRMWARE);
	DC_InvalidateRange(buffer,length);

}	

//---------------------------------------------------------------------------------
int writeFirmware(u32 address, void *buffer, u32 length) {
//---------------------------------------------------------------------------------

	if ( ((address & 0xff) != 0) || ((length  & 0xff) != 0)) return -1;
	DC_FlushRange(buffer,length);

	FifoMessage msg;

	msg.type = FW_WRITE;
	msg.fwParams.address = address;
	msg.fwParams.buffer = buffer;
	msg.fwParams.length = length;
	
	fifoSendDatamsg(FIFO_FIRMWARE, sizeof(msg), (u8*)&msg);

	while(!fifoCheckValue32(FIFO_FIRMWARE));

	return (int)fifoGetValue32(FIFO_FIRMWARE);

}	
