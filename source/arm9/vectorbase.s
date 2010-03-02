/*---------------------------------------------------------------------------------

  Copyright (C) 2009
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
	.section	.vectors,"ax",%progbits

	.global		SystemVectors

	ldr	r15,vec_reset
	ldr	r15,vec_undefined
	ldr	r15,vec_swi
	ldr	r15,vec_prefetch_abort
	ldr	r15,vec_data_abort
dummy:	b	dummy
	ldr	r15,vec_irq
	ldr	r15,vec_fiq

SystemVectors:
vec_reset:
	.word	0xFFFF0000
vec_undefined:
	.word	0xFFFF0004
vec_swi:
	.word	0xFFFF0008
vec_prefetch_abort:
	.word	0xFFFF000C
vec_data_abort:
	.word	0xFFFF0010
vec_irq:
	.word	0xFFFF0018
vec_fiq:
	.word	0xFFFF001C


@---------------------------------------------------------------------------------
@ void setVectorBase(int highVector)
@---------------------------------------------------------------------------------
	.global setVectorBase
@---------------------------------------------------------------------------------
setVectorBase:
@---------------------------------------------------------------------------------
 
	@ load the CP15 Control Register
	mrc p15, 0, r1, c1, c0, 0
 
	@ if (highVector)
	@   // set the bit
	@ r1 |= (1<<13)
	@ else
	@   // clear the bit
	@ r1 &= ~(1<<13)
 
	cmp r0, #0
	biceq r1, r1, #(1<<13)
	orrne r1, r1, #(1<<13)
 
	@ store the control register
	mcr p15, 0, r1, c1, c0, 0
 
	bx lr

