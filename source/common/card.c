/*---------------------------------------------------------------------------------

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
#include "nds/card.h"
#include "nds/dma.h"
#include "nds/memory.h"


//---------------------------------------------------------------------------------
void cardWriteCommand(const uint8 *command) {
//---------------------------------------------------------------------------------
	int index;

	REG_AUXSPICNTH = CARD_CR1_ENABLE | CARD_CR1_IRQ;

	for (index = 0; index < 8; index++) {
		CARD_COMMAND[7-index] = command[index];
	}
}


//---------------------------------------------------------------------------------
void cardPolledTransfer(uint32 flags, uint32 *destination, uint32 length, const uint8 *command) {
//---------------------------------------------------------------------------------
	u32 data;
	cardWriteCommand(command);
	REG_ROMCTRL = flags;
	uint32 * target = destination + length;
	do {
		// Read data if available
		if (REG_ROMCTRL & CARD_DATA_READY) {
			data=CARD_DATA_RD;
			if (destination < target)
				*destination = data;
			destination++;
		}
	} while (REG_ROMCTRL & CARD_BUSY);
}


//---------------------------------------------------------------------------------
void cardStartTransfer(const uint8 *command, uint32 *destination, int channel, uint32 flags) {
//---------------------------------------------------------------------------------
	cardWriteCommand(command);

	// Set up a DMA channel to transfer a word every time the card makes one
	DMA_SRC(channel) = (uint32)&CARD_DATA_RD;
	DMA_DEST(channel) = (uint32)destination;
	DMA_CR(channel) = DMA_ENABLE | DMA_START_CARD | DMA_32_BIT | DMA_REPEAT | DMA_SRC_FIX | 0x0001;

	REG_ROMCTRL = flags;
}


//---------------------------------------------------------------------------------
uint32 cardWriteAndRead(const uint8 * command, uint32 flags) {
//---------------------------------------------------------------------------------
	cardWriteCommand(command);
	REG_ROMCTRL = flags | CARD_ACTIVATE | CARD_nRESET | CARD_BLK_SIZE(7);
	while (!(REG_ROMCTRL & CARD_DATA_READY)) ;
	return CARD_DATA_RD;
}

//---------------------------------------------------------------------------------
void cardParamCommand (uint8 command, uint32 parameter, uint32 flags, uint32 *destination, uint32 length) {
//---------------------------------------------------------------------------------
	u8 cmdData[8];
	
	cmdData[7] = (u8) command;
	cmdData[6] = (u8) (parameter >> 24);
	cmdData[5] = (u8) (parameter >> 16);
	cmdData[4] = (u8) (parameter >>  8);
	cmdData[3] = (u8) (parameter >>  0);
	cmdData[2] = 0;
	cmdData[1] = 0;
	cmdData[0] = 0;

	cardPolledTransfer(flags, destination, length, cmdData);
}

//---------------------------------------------------------------------------------
void cardReadHeader(uint8 *header) {
//---------------------------------------------------------------------------------
	cardParamCommand(CARD_CMD_HEADER_READ, 0,
		CARD_ACTIVATE | CARD_nRESET | CARD_CLK_SLOW | CARD_BLK_SIZE(1) | CARD_DELAY1(0x1FFF) | CARD_DELAY2(0x3F),
		(uint32 *)header, 512/4);
}


//---------------------------------------------------------------------------------
uint32 cardReadID(uint32 flags) {
//---------------------------------------------------------------------------------
	const uint8 command[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, CARD_CMD_HEADER_CHIPID};
	return cardWriteAndRead(command, flags);
}




