//////////////////////////////////////////////////////////////////////
//
// system.h -- Power control, keys, and HV clock registers
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
//	 0.2
//
//////////////////////////////////////////////////////////////////////

#ifndef NDS_SYSTEM_INCLUDE
#define NDS_SYSTEM_INCLUDE

//////////////////////////////////////////////////////////////////////

#define DISP_SR        (*(vuint16*)0x04000004)

#define DISP_IN_VBLANK     (1 << 0)
#define DISP_IN_HBLANK     (1 << 1)
#define DISP_YTRIGGERED    (1 << 2)
#define DISP_VBLANK_IRQ    (1 << 3)
#define DISP_HBLANK_IRQ    (1 << 4)
#define DISP_YTRIGGER_IRQ  (1 << 5)

#define DISP_Y      (*(vuint16*)0x04000006)

//////////////////////////////////////////////////////////////////////

// Halt control register
#define HALT_CR       (*(vuint16*)0x04000300)
// Writing 0x40 to HALT_CR activates GBA mode, single bit it seems

// arm7, bit 0 = sound power off/on
#define POWER_CR       (*(vuint16*)0x04000304)

static inline void powerON(int on) { POWER_CR |= on;}
static inline void powerOFF(int off) { POWER_CR &= off;}

#ifdef ARM9
// Power control register (arm9)
#define POWER_LCD_TOP    (1<<0)
#define POWER_2D         (1<<1)
#define POWER_MATRIX     (1<<2)
#define POWER_3D_CORE    (1<<3)
#define POWER_LCD_BOTTOM (1<<8)
#define POWER_2D_SUB     (1<<9)
#define POWER_SWAP_LCDS  (1<<15)
#define POWER_ALL_2D     (POWER_LCD_TOP|POWER_LCD_BOTTOM|POWER_2D|POWER_2D_SUB)
#define POWER_ALL		 (POWER_ALL_2D | POWER_3D_CORE | POWER_MATRIX)

static inline void lcdSwap(void) { POWER_CR ^= POWER_SWAP_LCDS; }
#endif

#ifdef ARM7
#define POWER_SOUND       (1<<0)
#define POWER_UNKNOWN     (1<<1)
#endif


//////////////////////////////////////////////////////////////////////
// Keyboard //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#define KEY_A          (1 << 0)
#define KEY_B          (1 << 1)
#define KEY_SELECT     (1 << 2)
#define KEY_START      (1 << 3)
#define KEY_RIGHT      (1 << 4)
#define KEY_LEFT       (1 << 5)
#define KEY_UP         (1 << 6)
#define KEY_DOWN       (1 << 7)
#define KEY_R          (1 << 8)
#define KEY_L          (1 << 9)

#define KEYS           (*(vuint16*)0x04000130)
#define KEYS_CR        (*(vuint16*)0x04000132)

#define READ_KEYS      ((~(KEYS)) & 0x3FF)

//////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////
