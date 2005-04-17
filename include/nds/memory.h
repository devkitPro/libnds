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
// Changelog:
//   0.1: First version
//
//////////////////////////////////////////////////////////////////////

#ifndef NDS_MEMORY_INCLUDE
#define NDS_MEMORY_INCLUDE

//////////////////////////////////////////////////////////////////////

#include "jtypes.h"

//////////////////////////////////////////////////////////////////////

// WAIT_CR: Wait State Control Register
#define WAIT_CR       (*(vuint16*)0x04000204)

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

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////

// Copies floor(size/4) words from source to dest (i.e. size in bytes)
// Utilizes DMA channel 3


//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////
