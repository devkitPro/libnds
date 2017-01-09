#ifndef _ASMINC_H_
#define _ASMINC_H_

#if !__ASSEMBLER__
	#error This header file is only for use in assembly files!
#endif	// !__ASSEMBLER__


.macro BEGIN_ASM_FUNC name
	.section .text.\name, "ax", %progbits
	.global \name
	.type \name, %function
	.align 2
\name:
.endm

#endif // _ASMINC_H_
