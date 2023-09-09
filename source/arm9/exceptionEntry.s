/*---------------------------------------------------------------------------------

  Copyright (C) 2023 fincs

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
#include <calico/asm.inc>
#include <calico/arm/psr.h>
#include <calico/arm/cp15.h>
#include <calico/nds/mm.h>

@---------------------------------------------------------------------------------
FUNC_START32 __libnds_excpt
@---------------------------------------------------------------------------------

	@ Configure 32K ITCM (mirrored throughout 0x0000000..0x1FFFFFF)
	mov   r12, #CP15_TCM_32M
	mcr   p15, 0, r12, c9, c1, 1

	@ Separate extra flags from stack pointer
	bic   r12, sp, #3 @ BIOS exception stack
	and   lr, sp, #3  @ extra flags

	@ Set up new exception stack on ITCM & push context structure
	mov   sp, #MM_MAINRAM @ end of ITCM
	sub   sp, sp, #20*4

	@ Grab non-banked registers directly
	stmia sp, {r0-r7}

	@ Grab saved registers from BIOS exception stack
	ldmia r12, {r0-r3} @ cp15cr cpsr r12 pc
	str   r0, [sp, #18*4] @ cp15cr
	str   r1, [sp, #16*4] @ cpsr
	mov   r12, r2
	str   r3, [sp, #15*4] @ pc

	@ Grab banked registers from correct processor mode
	mrs   r2, cpsr
	ands  r4, r1, #0xf @ Extract and check mode
	moveq r4, #0xf     @ If user: replace with system
	cmpne r4, #0xf     @ Z flag for later = mode is user or system
	orr   r4, r4, #(ARM_PSR_I | ARM_PSR_F | ARM_PSR_MODE_USR)
	add   r5, sp, #8*4
	msr   cpsr_c, r4
	stmia r5, {r8-r14}
	mrsne r6, spsr     @ If not user/system: grab spsr too
	msr   cpsr_c, r2

	@ Store remaining registers
	str   r6, [sp, #17*4] @ spsr
	str   r2, [sp, #19*4] @ excpt_cpsr

	@--------------------------------------------------------------------------
	@ MPU reconfiguration

	@ Configure MPU regions 0-2: background, main RAM, BIOS
	ldr   r4, =CP15_PU_ENABLE | CP15_PU_4G
	mcr   p15, 0, r4, c6, c0, 0
	ldr   r4, =CP15_PU_ENABLE | CP15_PU_16M | MM_MAINRAM
	mcr   p15, 0, r4, c6, c1, 0
	ldr   r4, =CP15_PU_ENABLE | CP15_PU_32K | MM_BIOS
	mcr   p15, 0, r4, c6, c2, 0

	@ Configure MPU region 3: DTCM
	mrc   p15, 0, r4, c9, c1, 0
	bic   r4, r4, #0xff
	orr   r4, r4, #CP15_PU_ENABLE | CP15_PU_16K
	mcr   p15, 0, r4, c6, c3, 0

	@ Clear unused MPU regions 4-7
	mov   r4, #0
	mcr   p15, 0, r4, c6, c4, 0
	mcr   p15, 0, r4, c6, c5, 0
	mcr   p15, 0, r4, c6, c6, 0
	mcr   p15, 0, r4, c6, c7, 0

	@ Set up MPU region attributes
	ldr   r4, =0x3633
	mcr   p15, 0, r4, c5, c0, 2 @ Data permissions
	ldr   r4, =0x0666
	mcr   p15, 0, r4, c5, c0, 3 @ Code permissions
	mov   r4, #0b0110
	mcr   p15, 0, r4, c2, c0, 0 @ Data cacheability
	mcr   p15, 0, r4, c2, c0, 1 @ Code cacheability
	mov   r4, #0b0010
	mcr   p15, 0, r4, c3, c0, 0 @ Write buffer enable

	@ Reenable MPU
	mrc   p15, 0, r4, c1, c0, 0
	orr   r4, r4, #CP15_CR_PU_ENABLE
	mcr   p15, 0, r4, c1, c0, 0

	@--------------------------------------------------------------------------

	@ Grab C function & call it
	ldr   r4, =g_excptHandler
	mov   r0, sp
	ldr   r4, [r4]
	mov   r1, lr
	blx   r4

	@ Infinite loop
1:	b     1b

FUNC_END

@---------------------------------------------------------------------------------
	SECT_BSS g_excptHandler
@---------------------------------------------------------------------------------
	.weak	g_excptHandler
@---------------------------------------------------------------------------------
g_excptHandler:
@---------------------------------------------------------------------------------
	.space	4
