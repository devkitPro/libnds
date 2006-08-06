/*---------------------------------------------------------------------------------
	$Id: card.c,v 1.9 2006-08-06 07:51:48 chishm Exp $

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

	$Log: not supported by cvs2svn $
	Revision 1.8  2006/01/17 00:10:11  dovoto
	Loopy added a fix to cardPolledTransfer
	
	Revision 1.7  2006/01/05 04:05:40  dovoto
	Fixed a reference to CARD_DATA to be refer to CARD_DATA_RD
	
	Revision 1.6  2005/11/07 01:09:53  wntrmute
	corrected card read header address
	corrected line endings
	
	Revision 1.5  2005/10/21 22:43:06  wntrmute
	Removed bogus ASSERT
	
	Revision 1.4  2005/09/14 06:21:58  wntrmute
	and address bytes in cardRead00
	
	Revision 1.3  2005/09/12 06:55:21  wntrmute
	replaced EepromWaitBusy macro with static inline

	Revision 1.2  2005/08/30 17:52:24  wntrmute
	corrected CARD_CR1
	fixed cardReadEeprom


---------------------------------------------------------------------------------*/
#include "nds/card.h"
#include "nds/dma.h"


//---------------------------------------------------------------------------------
void cardWriteCommand(const uint8 * command) {
//---------------------------------------------------------------------------------
	int index;

	CARD_CR1H = CARD_CR1_ENABLE | CARD_CR1_IRQ;

	for (index = 0; index < 8; index++) {
		CARD_COMMAND[7-index] = command[index];
	}
}


//---------------------------------------------------------------------------------
void cardPolledTransfer(uint32 flags, uint32 * destination, uint32 length, const uint8 * command) {
//---------------------------------------------------------------------------------
	u32 data;;
	cardWriteCommand(command);
	CARD_CR2 = flags;
	uint32 * target = destination + length;
	do {
		// Read data if available
		if (CARD_CR2 & CARD_DATA_READY) {
			data=CARD_DATA_RD;
			if (destination < target)
				*destination = data;
			destination++;
		}
	} while (CARD_CR2 & CARD_BUSY);
}


//---------------------------------------------------------------------------------
void cardStartTransfer(const uint8 * command, uint32 * destination, int channel, uint32 flags) {
//---------------------------------------------------------------------------------
	cardWriteCommand(command);

	// Set up a DMA channel to transfer a word every time the card makes one
	DMA_SRC(channel) = (uint32)&CARD_DATA_RD;
	DMA_DEST(channel) = (uint32)destination;
	DMA_CR(channel) = DMA_ENABLE | DMA_START_CARD | DMA_32_BIT | DMA_REPEAT | DMA_SRC_FIX | 0x0001;;

	CARD_CR2 = flags;
}


//---------------------------------------------------------------------------------
uint32 cardWriteAndRead(const uint8 * command, uint32 flags) {
//---------------------------------------------------------------------------------
	cardWriteCommand(command);
	CARD_CR2 = flags | CARD_ACTIVATE | CARD_nRESET | 0x07000000;
	while (!(CARD_CR2 & CARD_DATA_READY)) ;
	return CARD_DATA_RD;
}


//---------------------------------------------------------------------------------
void cardRead00(uint32 address, uint32 * destination, uint32 length, uint32 flags) {
//---------------------------------------------------------------------------------f
	uint8 command[8];
	command[7] = 0;
	command[6] = (address >> 24) & 0xff;
	command[5] = (address >> 16) & 0xff;
	command[4] = (address >> 8) & 0xff;
	command[3] = address & 0xff;
	command[2] = 0;
	command[1] = 0;
	command[0] = 0;
	cardPolledTransfer(flags, destination, length, command);
}


//---------------------------------------------------------------------------------
void cardReadHeader(uint8 * header) {
//---------------------------------------------------------------------------------
	cardRead00(0, (uint32 *)header, 512, 0xA93F1FFF);
}


//---------------------------------------------------------------------------------
int cardReadID(uint32 flags) {
//---------------------------------------------------------------------------------
	uint8 command[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90};
	return cardWriteAndRead(command, flags);
}


//---------------------------------------------------------------------------------
static inline void EepromWaitBusy()	{
//---------------------------------------------------------------------------------
	while (CARD_CR1 & /*BUSY*/0x80);
}

//---------------------------------------------------------------------------------
void cardReadEeprom(uint32 address, uint8 *data, uint32 length, uint32 addrtype) {
//---------------------------------------------------------------------------------

    CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
    CARD_EEPDATA = 0x03 | ((addrtype == 1) ? address>>8<<3 : 0);
    EepromWaitBusy();

    if (addrtype == 3) {
        CARD_EEPDATA = (address >> 16) & 0xFF;
    } else if (addrtype >= 2) {
        CARD_EEPDATA = (address >> 8) & 0xFF;
    }

	EepromWaitBusy();

	CARD_EEPDATA = (address) & 0xFF;
    EepromWaitBusy();

    while (length > 0) {
        CARD_EEPDATA = 0;
        EepromWaitBusy();
        *data++ = CARD_EEPDATA;
        length--;
    }
    CARD_CR1 = /*MODE*/0x40;
}


// NOTE: does not work for small EEPROMs
//---------------------------------------------------------------------------------
void cardWriteEeprom(uint32 address, uint8 *data, uint32 length, uint32 addrtype) {
//---------------------------------------------------------------------------------

	uint32 address_end = address + length;
	int i;
	while (address < address_end) {
		// set WEL (Write Enable Latch)
		CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARD_EEPDATA = 0x06; EepromWaitBusy();
		CARD_CR1 = /*MODE*/0x40;

		// program maximum of 32 bytes
		CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARD_EEPDATA = 0x02 | ((addrtype == 1) ? address>>8<<3 : 0); EepromWaitBusy();
		if (addrtype > 1) { CARD_EEPDATA = address >> 8; EepromWaitBusy(); }
		CARD_EEPDATA = address & 0xFF; EepromWaitBusy();
		for (i=0; address<address_end && i<32; i++, address++) { CARD_EEPDATA = *data++; EepromWaitBusy(); }
		CARD_CR1 = /*MODE*/0x40;

		// wait programming to finish
		CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
		CARD_EEPDATA = 0x05; EepromWaitBusy();
		do { CARD_EEPDATA = 0; EepromWaitBusy(); } while (CARD_EEPDATA & 0x01);	// WIP (Write In Progress) ?
		CARD_CR1 = /*MODE*/0x40;
	}
}

