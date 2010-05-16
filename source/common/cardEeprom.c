/*---------------------------------------------------------------------------------

	Copyright (C) 2005 - 2010
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
#include "nds/card.h"
#include "nds/dma.h"
#include "nds/memory.h"

//---------------------------------------------------------------------------------
static inline void EepromWaitBusy() {
//---------------------------------------------------------------------------------
	while (REG_AUXSPICNT & /*BUSY*/0x80);
}

//---------------------------------------------------------------------------------
u8 cardEepromCommand(u8 command) {
//---------------------------------------------------------------------------------
	u8 retval;

	REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;

	REG_AUXSPIDATA = command;

	EepromWaitBusy();

	REG_AUXSPIDATA = 0;
	EepromWaitBusy();
	retval = REG_AUXSPIDATA;
	REG_AUXSPICNT = /*MODE*/0x40;
	return retval;
}


//---------------------------------------------------------------------------------
u32 cardEepromReadID() {
//---------------------------------------------------------------------------------
	int i;

	REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;

	REG_AUXSPIDATA = EEPROM_RDID;

	EepromWaitBusy();
	u32 id = 0;
	for ( i=0; i<3; i++) {
		REG_AUXSPIDATA = 0;
		EepromWaitBusy();
		id = (id << 8) | REG_AUXSPIDATA;
	}

	REG_AUXSPICNT = /*MODE*/0x40;

	return id;
}


//---------------------------------------------------------------------------------
int cardEepromGetType(void) {
//---------------------------------------------------------------------------------
	int sr = cardEepromCommand(EEPROM_RDSR);
	int id = cardEepromReadID();
	
	if ( sr == 0xff && id == 0xffffff ) return -1;
	if ( sr == 0xf0 && id == 0xffffff ) return 1;
	if ( sr == 0x00 && id == 0xffffff ) return 2;
	if ( id != 0xffffff) return 3;
	
	return 0;
}

//---------------------------------------------------------------------------------
uint32 cardEepromGetSize() {
//---------------------------------------------------------------------------------

	int type = cardEepromGetType();

	if(type == -1)
		return 0;
	if(type == 0)
		return 8192;
	if(type == 1)
		return 512;
	if(type == 2) {
		static const uint32 offset0 = (8*1024-1);		 //		 8KB
		static const uint32 offset1 = (2*8*1024-1);		 //		 16KB
		u8 buf1;	 //		 +0k data		 read -> write
		u8 buf2;	 //		 +8k data		 read -> read
		u8 buf3;	 //		 +0k ~data			write
		u8 buf4;	 //		 +8k data new	 comp buf2
		cardReadEeprom(offset0,&buf1,1,type);
		cardReadEeprom(offset1,&buf2,1,type);
		buf3=~buf1;
		cardWriteEeprom(offset0,&buf3,1,type);
		cardReadEeprom (offset1,&buf4,1,type);
		cardWriteEeprom(offset0,&buf1,1,type);
		if(buf4!=buf2)		//		+8k
			return 8*1024;	//		 8KB(64kbit)
		else
			return 64*1024; //		64KB(512kbit)
	}

	int device;

	if(type == 3) {
		int id = cardEepromReadID();

		device = id & 0xffff;
		
		if ( ((id >> 16) & 0xff) == 0x20 ) {
			
		
			if( device == 0x4014)
				return 1024*1024;		//	8Mbit(1 meg)

			if( device == 0x4013)
				return 512*1024;		//	4Mbit(512KByte)

			if( device == 0x2017)
				return 8*1024*1024;		//	64Mbit(8 meg)
		}

		if ( ((id >> 16) & 0xff) == 0x62 ) {
			
			if (device == 0x1100)
				return 512*1024;		//	4Mbit(512KByte)

		}
		

		return 256*1024;		//	2Mbit(256KByte)
	}

	return 0;
}


//---------------------------------------------------------------------------------
void cardReadEeprom(uint32 address, uint8 *data, uint32 length, uint32 addrtype) {
//---------------------------------------------------------------------------------

	REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
	REG_AUXSPIDATA = 0x03 | ((addrtype == 1) ? address>>8<<3 : 0);
	EepromWaitBusy();

	if (addrtype == 3) {
		REG_AUXSPIDATA = (address >> 16) & 0xFF;
		EepromWaitBusy();
	} 
	
	if (addrtype >= 2) {
		REG_AUXSPIDATA = (address >> 8) & 0xFF;
		EepromWaitBusy();
	}


	REG_AUXSPIDATA = (address) & 0xFF;
	EepromWaitBusy();

	while (length > 0) {
		REG_AUXSPIDATA = 0;
		EepromWaitBusy();
		*data++ = REG_AUXSPIDATA;
		length--;
	}

	EepromWaitBusy();
	REG_AUXSPICNT = /*MODE*/0x40;
}


//---------------------------------------------------------------------------------
void cardWriteEeprom(uint32 address, uint8 *data, uint32 length, uint32 addrtype) {
//---------------------------------------------------------------------------------

	uint32 address_end = address + length;
	int i;
	int maxblocks = 32;
	if(addrtype == 1) maxblocks = 16;
	if(addrtype == 2) maxblocks = 32;
	if(addrtype == 3) maxblocks = 256;

	while (address < address_end) {
		// set WEL (Write Enable Latch)
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x06; EepromWaitBusy();
		REG_AUXSPICNT = /*MODE*/0x40;

		// program maximum of 32 bytes
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;

		if(addrtype == 1) {
		//	WRITE COMMAND 0x02 + A8 << 3
			REG_AUXSPIDATA = 0x02 | (address & BIT(8)) >> (8-3) ;
			EepromWaitBusy();
			REG_AUXSPIDATA = address & 0xFF;
			EepromWaitBusy();
		}
		else if(addrtype == 2) {
			REG_AUXSPIDATA = 0x02;
			EepromWaitBusy();
			REG_AUXSPIDATA = address >> 8;
			EepromWaitBusy();
			REG_AUXSPIDATA = address & 0xFF;
			EepromWaitBusy();
		}
		else if(addrtype == 3) {
			REG_AUXSPIDATA = 0x02;
			EepromWaitBusy();
			REG_AUXSPIDATA = (address >> 16) & 0xFF;
			EepromWaitBusy();
			REG_AUXSPIDATA = (address >> 8) & 0xFF;
			EepromWaitBusy();
			REG_AUXSPIDATA = address & 0xFF;
			EepromWaitBusy();
		}

		for (i=0; address<address_end && i<maxblocks; i++, address++) { 
			REG_AUXSPIDATA = *data++; 
			EepromWaitBusy(); 
		}
		REG_AUXSPICNT = /*MODE*/0x40;

		// wait programming to finish
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x05; EepromWaitBusy();
		do { REG_AUXSPIDATA = 0; EepromWaitBusy(); } while (REG_AUXSPIDATA & 0x01); // WIP (Write In Progress) ?
		EepromWaitBusy();
		REG_AUXSPICNT = /*MODE*/0x40;
	}
}

//---------------------------------------------------------------------------------
//	Chip Erase : clear FLASH MEMORY (TYPE 3 ONLY)
//---------------------------------------------------------------------------------
void cardEepromChipErase(void) {
//---------------------------------------------------------------------------------
	int sz, sector;
	sz=cardEepromGetSize();

	for ( sector = 0; sector < sz; sector+=0x10000) {
		cardEepromSectorErase(sector);
	}
}

//---------------------------------------------------------------------------------
//	COMMAND Sec.erase 0xD8
//---------------------------------------------------------------------------------
void cardEepromSectorErase(uint32 address) {
//---------------------------------------------------------------------------------
		// set WEL (Write Enable Latch)
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x06;
		EepromWaitBusy();

		REG_AUXSPICNT = /*MODE*/0x40;

		// SectorErase 0xD8
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0xD8;
		EepromWaitBusy();
		REG_AUXSPIDATA = (address >> 16) & 0xFF;
		EepromWaitBusy();
		REG_AUXSPIDATA = (address >> 8) & 0xFF;
		EepromWaitBusy();
		REG_AUXSPIDATA = address & 0xFF;
		EepromWaitBusy();

		REG_AUXSPICNT = /*MODE*/0x40;

		// wait erase to finish
		REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		REG_AUXSPIDATA = 0x05;
		EepromWaitBusy();

		do {
			REG_AUXSPIDATA = 0;
			EepromWaitBusy();
		} while (REG_AUXSPIDATA & 0x01);  // WIP (Write In Progress) ?
		REG_AUXSPICNT = /*MODE*/0x40;
}
