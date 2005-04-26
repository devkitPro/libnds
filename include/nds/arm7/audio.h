//////////////////////////////////////////////////////////////////////
//
// audio.h -- Audio control for ARM7
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

#ifndef AUDIO_ARM7_INCLUDE
#define AUDIO_ARM7_INCLUDE

#ifndef ARM7
#error Audio is only available on the ARM7
#endif

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////

#include <NDS/NDS.h>

//////////////////////////////////////////////////////////////////////
// Sound (ARM7 only)
//////////////////////////////////////////////////////////////////////

#define SOUND_VOL(n)	(n)
#define SOUND_FREQ(n)	((-0x1000000 / (n)))
#define SOUND_ENABLE	BIT(31)
#define SOUND_REPEAT    BIT(27)
#define SOUND_ONE_SHOT  BIT(28)
#define SOUND_PAN(n)	((n) << 16)

#define SCHANNEL_ENABLE BIT(15)

//registers
#define SCHANNEL_CR(n)				(*(vuint32*)(0x04000400 + ((n)<<4)))
#define SCHANNEL_SOURCE(n)			(*(vuint32*)(0x04000404 + ((n)<<4)))
#define SCHANNEL_TIMER(n)			(*(vint16*)(0x04000408 + ((n)<<4)))
#define SCHANNEL_REPEAT_POINT(n)	(*(vuint16*)(0x0400040A + ((n)<<4)))
#define SCHANNEL_LENGTH(n)			(*(vuint32*)(0x0400040C + ((n)<<4)))

#define SOUND_CR          (*(vuint16*)0x04000500)

//not sure on the following
#define SOUND_BIAS        (*(vuint16*)0x04000504)
#define SOUND508          (*(vuint16*)0x04000508)
#define SOUND510          (*(vuint16*)0x04000510)
#define SOUND514		  (*(vuint16*)0x04000514)
#define SOUND518          (*(vuint16*)0x04000518)
#define SOUND51C          (*(vuint16*)0x0400051C)

//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif

//////////////////////////////////////////////////////////////////////
