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
BEGIN_ASM_FUNC swiWaitForVBlank
//---------------------------------------------------------------------------------
	mov	r0, #1
	mov	r1, #1

//---------------------------------------------------------------------------------
BEGIN_ASM_FUNC swiIntrWait
//---------------------------------------------------------------------------------
	push	{lr}
	mov	r12, #0x4000000
	str	r12, [r12, #0x208]

	mrc	p15, 0, r3, c9, c1, 0
	mov	r3, r3, lsr#12
	mov	r3, r3, lsl#12
	add	r3, r3, #0x4000

	mov 	r12, r0
	bl 	check_flags
	beq	wait

	cmp	r12, #0
	beq	flag_set
wait:
	mov	r12, #0x4000000
wait_flags:
	mov	r2, #1
	str  	r2, [r12,#0x208]

	mov 	r0, #0
	mcr 	p15, 0, r0, c7, c0, 4

	str	r12, [r12, #0x208]
	bl 	check_flags
	beq	wait_flags

flag_set:
	mov	r2, #1
	str  	r2, [r12,#0x208]
	pop	{pc}

//---------------------------------------------------------------------------------
check_flags:
//---------------------------------------------------------------------------------
	ldr	r2, [r3, #-8]
	ands	r0, r1, r2
	eorne	r2, r2, r0
	strne	r2, [r3, #-8]
	bx	lr
