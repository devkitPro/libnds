//////////////////////////////////////////////////////////////////////
//
// memory.h -- Declaration of memory regions
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
/*
  $Log: 

*/
//////////////////////////////////////////////////////////////////////

#ifndef NDS_MEMORY_INCLUDE
#define NDS_MEMORY_INCLUDE

//////////////////////////////////////////////////////////////////////

#include "jtypes.h"

//////////////////////////////////////////////////////////////////////

// WAIT_CR: Wait State Control Register
#define WAIT_CR       (*(vuint16*)0x04000204)

#define ARM9_OWNS_SRAM BIT(15)
#define ARM9_OWNS_CARD BIT(11)
#define ARM9_OWNS_ROM  BIT(7)
#define ARM7_OWNS_SRAM 0
#define ARM7_OWNS_CARD 0
#define ARM7_OWNS_ROM  0

//////////////////////////////////////////////////////////////////////

// Protection register (write-once sadly)
#ifdef ARM7
#define PROTECTION    (*(vuint32*)0x04000308)
#endif

//////////////////////////////////////////////////////////////////////

#define ALLRAM        ((uint8*)0x00000000)

#define MAINRAM8      ((uint8*)0x02000000)
#define MAINRAM16     ((uint16*)0x02000000)
#define MAINRAM32     ((uint32*)0x02000000)

// fixme: shared RAM

// GBA_BUS is volatile, while GBAROM is not
#define GBA_BUS       ((vuint16 *)(0x08000000))
#define GBAROM        ((uint16*)0x08000000)

#define SRAM          ((uint8*)0x0A000000)

#ifdef ARM9
#define PALETTE       ((uint16*)0x05000000)
#define PALETTE_SUB   ((uint16*)0x05000400)

#define BG_PALETTE       ((uint16*)0x05000000)
#define BG_PALETTE_SUB   ((uint16*)0x05000400)

#define SPRITE_PALETTE ((uint16*)0x05000200)
#define SPRITE_PALETTE_SUB ((uint16*)0x05000600)

#define BG_GFX			((uint16*)0x6000000)
#define BG_GFX_SUB		((uint16*)0x6200000)
#define SPRITE_GFX			((uint16*)0x6400000)
#define SPRITE_GFX_SUB		((uint16*)0x6600000)

#define VRAM_0        ((uint16*)0x6000000)
#define VRAM          ((uint16*)0x6800000)
#define VRAM_A        ((uint16*)0x6800000)
#define VRAM_B        ((uint16*)0x6820000)
#define VRAM_C        ((uint16*)0x6840000)
#define VRAM_D        ((uint16*)0x6860000)
#define VRAM_E        ((uint16*)0x6880000)
#define VRAM_F        ((uint16*)0x6890000)
#define VRAM_G        ((uint16*)0x6894000)
#define VRAM_H        ((uint16*)0x6898000)
#define VRAM_I        ((uint16*)0x68A0000)

#define OAM           ((uint16*)0x07000000)
#define OAM_SUB       ((uint16*)0x07000400)

#endif

//////////////////////////////////////////////////////////////////////

typedef struct sGBAHeader {
  __attribute__ ((__packed__)) uint32 entryPoint;
  __attribute__ ((__packed__)) uint8 logo[156];
  __attribute__ ((__packed__)) char title[0xC];
  __attribute__ ((__packed__)) char gamecode[0x4];
  __attribute__ ((__packed__)) uint16 makercode;
  __attribute__ ((__packed__)) uint8 is96h;
  __attribute__ ((__packed__)) uint8 unitcode;
  __attribute__ ((__packed__)) uint8 devicecode;
  __attribute__ ((__packed__)) uint8 unused[7];
  __attribute__ ((__packed__)) uint8 version;
  __attribute__ ((__packed__)) uint8 complement;
  __attribute__ ((__packed__)) uint16 checksum;
} tGBAHeader;
#define GBA_HEADER (*(tGBAHeader *)0x08000000)
//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////

#ifdef ARM9

#define BUS_OWNER_ARM9 true
#define BUS_OWNER_ARM7 false

// Changes the owernship for all three busses
void sysSetBusOwners(bool arm9rom, bool arm9sram, bool arm9card);

// Changes only the gba rom cartridge ownership
void sysSetCartOwner(bool arm9);

#endif

//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////
