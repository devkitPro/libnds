/*---------------------------------------------------------------------------------

  Copyright (C) 2005 - 2016
    Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

---------------------------------------------------------------------------------*/
#include <nds/asminc.h>

	.arch	armv5te
	.cpu	arm946e-s

	.text
	.arm

//---------------------------------------------------------------------------------
BEGIN_ASM_FUNC swiIntrWait
//---------------------------------------------------------------------------------
	@ savedIME = REG_IME, REG_IME = 1
	mov r2, #1
	mov r12, #0x4000000
	ldrb r3, [r12, #0x208]
	strb r2, [r12, #0x208]
	
	@ Wait for IRQ
	mov r2, #0
	mcr 15, 0, r2, c7, c0, 4
	
	@ REG_IME = savedIME
	strb r3, [r12, #0x208]
	
	@ return
	bx lr
