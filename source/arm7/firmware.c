/*---------------------------------------------------------------------------------

  Copyright (C) 2014
	Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1.  The origin of this software must not be misrepresented; you
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product
      documentation would be appreciated but is not required.
  2.  Altered source versions must be plainly marked as such, and
      must not be misrepresented as being the original software.
  3.  This notice may not be removed or altered from any source
      distribution.

---------------------------------------------------------------------------------*/
#include <nds/arm7/serial.h>
#include <nds/interrupts.h>
#include <nds/system.h>
#include <nds/fifocommon.h>
#include <nds/fifomessages.h>

//---------------------------------------------------------------------------------
void readFirmware(u32 address, void * destination, u32 size) {
//---------------------------------------------------------------------------------
	int oldIME=enterCriticalSection();
	u8 *buffer = destination;
	// Read command
	while (REG_SPICNT & SPI_BUSY);
	REG_SPICNT = SPI_ENABLE | SPI_BYTE_MODE | SPI_CONTINUOUS | SPI_DEVICE_FIRMWARE;
	REG_SPIDATA = FIRMWARE_READ;
	SerialWaitBusy();

	// Set the address
	REG_SPIDATA = (address>>16) & 0xFF;
	SerialWaitBusy();
	REG_SPIDATA = (address>>8) & 0xFF;
	SerialWaitBusy();
	REG_SPIDATA = (address) & 0xFF;
	SerialWaitBusy();

	u32 i;
	// Read the data
	for(i=0;i<size;i++) {
		REG_SPIDATA = 0;
		SerialWaitBusy();
		buffer[i] = (REG_SPIDATA & 0xFF);
	}

	REG_SPICNT = 0;
	leaveCriticalSection(oldIME);
}

//---------------------------------------------------------------------------------
int writeFirmware(u32 offset, void * source, u32 size) {
//---------------------------------------------------------------------------------
	u8 *buffer = source;
	buffer[0] = 0;
	return 0;
}

//---------------------------------------------------------------------------------
void firmwareMsgHandler(int bytes, void *user_data) {
//---------------------------------------------------------------------------------

	FifoMessage msg;

	int response = -1;
	
	fifoGetDatamsg(FIFO_FIRMWARE, bytes, (u8*)&msg);
	
	switch(msg.type) {
		case FW_READ:
			readFirmware(msg.fwParams.address, msg.fwParams.buffer, msg.fwParams.length);
			response = 0;
			break;
		case FW_WRITE:
			response = writeFirmware(msg.fwParams.address, msg.fwParams.buffer, msg.fwParams.length);
			break;
	}
	fifoSendValue32(FIFO_FIRMWARE,response);
}

