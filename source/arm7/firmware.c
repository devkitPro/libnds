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
#include <string.h>

static u8 readwriteSPI(u8 data) {
	REG_SPIDATA = data;
	SerialWaitBusy();
	return REG_SPIDATA;
}

//---------------------------------------------------------------------------------
void readFirmware(u32 address, void * destination, u32 size) {
//---------------------------------------------------------------------------------
	int oldIME=enterCriticalSection();
	u8 *buffer = destination;

	// Read command
	REG_SPICNT = SPI_ENABLE | SPI_BYTE_MODE | SPI_CONTINUOUS | SPI_DEVICE_FIRMWARE;
	readwriteSPI(FIRMWARE_READ);

	// Set the address
	readwriteSPI((address>>16) & 0xFF);
	readwriteSPI((address>> 8) & 0xFF);
	readwriteSPI((address) & 0xFF);

	u32 i;

	// Read the data
	for(i=0;i<size;i++) {
		buffer[i] = readwriteSPI(0);
	}

	REG_SPICNT = 0;
	leaveCriticalSection(oldIME);
}

//---------------------------------------------------------------------------------
static int writeFirmwarePage(u32 address,u8 *buffer) {
//---------------------------------------------------------------------------------
	int i;
	u8 pagebuffer[256];
	readFirmware(address, pagebuffer, 256);

	if (memcmp(pagebuffer,buffer,256) == 0) return 0;

	int oldIME=enterCriticalSection();

	//write enable
	REG_SPICNT = SPI_ENABLE|SPI_CONTINUOUS|SPI_DEVICE_NVRAM;
	readwriteSPI(FIRMWARE_WREN);
	REG_SPICNT = 0;

	//Wait for Write Enable Latch to be set
	REG_SPICNT = SPI_ENABLE|SPI_CONTINUOUS|SPI_DEVICE_NVRAM;
	readwriteSPI(FIRMWARE_RDSR);
	while((readwriteSPI(0)&0x02)==0); //Write Enable Latch
	REG_SPICNT = 0;

	//page write
	REG_SPICNT = SPI_ENABLE|SPI_CONTINUOUS|SPI_DEVICE_NVRAM;
	readwriteSPI(FIRMWARE_PW);
	// Set the address
	readwriteSPI((address>>16) & 0xFF);
	readwriteSPI((address>> 8) & 0xFF);
	readwriteSPI((address) & 0xFF);

	for (i=0; i<256; i++) {
		readwriteSPI(buffer[i]);
	}

	REG_SPICNT = 0;

	// wait for programming to finish
	REG_SPICNT = SPI_ENABLE|SPI_CONTINUOUS|SPI_DEVICE_NVRAM;
	readwriteSPI(FIRMWARE_RDSR);
	while(readwriteSPI(0)&0x01);	//Write In Progress
	REG_SPICNT = 0;

	leaveCriticalSection(oldIME);

	// read it back & verify
	readFirmware(address, pagebuffer, 256);
	if (memcmp(pagebuffer,buffer,256) == 0) return 0;
	return -1;
}


//---------------------------------------------------------------------------------
int writeFirmware(u32 address, void * source, u32 size) {
//---------------------------------------------------------------------------------
	if( ((address & 0xff) != 0) || ((size  & 0xff) != 0)) return -1;
	u8 *buffer = source;
	int response = -1;

	while (size >0 ) {
		size -= 256;
		if (writeFirmwarePage(address+size,buffer+size )) break;
	}

	if (size == 0 ) response = 0;

	return response;
}

//---------------------------------------------------------------------------------
void firmwareMsgHandler(int bytes, void *user_data) {
//---------------------------------------------------------------------------------

	FifoMessage msg;

	int response = -1;
	
	fifoGetDatamsg(FIFO_FIRMWARE, bytes, (u8*)&msg);
	
	switch(msg.type) {
		case FW_READ:
			readFirmware(msg.blockParams.address, msg.blockParams.buffer, msg.blockParams.length);
			response = 0;
			break;
		case FW_WRITE:
			response = writeFirmware(msg.blockParams.address, msg.blockParams.buffer, msg.blockParams.length);
			break;
	}
	fifoSendValue32(FIFO_FIRMWARE,response);
}

