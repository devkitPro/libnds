//////////////////////////////////////////////////////////////////////
//
// card.h -- DS Memory card registers and accessors
//
// version 0.1, February 14, 2005
//
//  Copyright (C) 2005 Michael Noland (joat) and Jason Rogers (dovoto)
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any
//  damages arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any
//  purpose, including commercial applications, and to alter it and
//  redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you
//     must not claim that you wrote the original software. If you use
//     this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and
//     must not be misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source
//     distribution.
//
// Changelog:
//   0.1: First version
//
//////////////////////////////////////////////////////////////////////

#ifndef NDS_CARD_INCLUDE
#define NDS_CARD_INCLUDE

//////////////////////////////////////////////////////////////////////

#include "jtypes.h"

//////////////////////////////////////////////////////////////////////
// Card bus
//////////////////////////////////////////////////////////////////////

#define CARD_CR1       (*(vuint32*)0x040001A0)
#define CARD_CR1H      (*(vuint8*)0x040001A1)
#define CARD_EEPDATA   (*(vuint8*)0x040001A2)
#define CARD_CR2       (*(vuint32*)0x040001A4)
#define CARD_COMMAND   ((vuint8*)0x040001A8)

#define CARD_DATA      (*(vuint32*)0x04100000)
#define CARD_DATA_RD   (*(vuint32*)0x04100010)

#define CARD_1B0       (*(vuint32*)0x040001B0)
#define CARD_1B4       (*(vuint32*)0x040001B4)
#define CARD_1B8       (*(vuint16*)0x040001B8)
#define CARD_1BA       (*(vuint16*)0x040001BA)

//////////////////////////////////////////////////////////////////////

#define CARD_CR1_ENABLE  0x80  // in byte 1, i.e. 0x8000
#define CARD_CR1_IRQ     0x40  // in byte 1, i.e. 0x4000

//////////////////////////////////////////////////////////////////////

// CARD_CR2 register:

#define CARD_ACTIVATE   (1<<31)  // when writing, get the ball rolling
// 1<<30
#define CARD_nRESET     (1<<29)  // value on the /reset pin (1 = high out, not a reset state, 0 = low out = in reset)
#define CARD_28         (1<<28)  // when writing
#define CARD_27         (1<<27)  // when writing
#define CARD_26         (1<<26)
#define CARD_22         (1<<22)
#define CARD_19         (1<<19)
#define CARD_ENCRYPTED  (1<<14)  // when writing, this command should be encrypted
#define CARD_13         (1<<13)  // when writing
#define CARD_4          (1<<4)   // when writing

// 3 bits in b10..b8 indicate something
// read bits
#define CARD_BUSY       (1<<31)  // when reading, still expecting incomming data?
#define CARD_DATA_READY (1<<23)  // when reading, CARD_DATA_RD or CARD_DATA has another word of data and is good to go

//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////

void cardWriteCommand(uint8 * command);

//fix me DARKFADER!!!!
void cardPolledTransfer(uint32 flags, uint32 * destination, uint32 length, uint8 * command);
void cardStartTransfer(uint8 * command, uint32 * destination, int channel, uint32 flags);
uint32 cardWriteAndRead(uint8 * command, uint32 flags);
void cardRead00(uint32 address, uint32 * destination, uint32 length, uint32 flags);
void cardReadHeader(uint8 * header);
int cardReadID(uint32 flags);
void cardReadEeprom(uint32 address, uint8 *data, uint32 length, uint32 addrtype);
void cardWriteEeprom(uint32 address, uint8 *data, uint32 length, uint32 addrtype);

//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////
