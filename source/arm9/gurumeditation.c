/*---------------------------------------------------------------------------------

  Copyright (C) 2005
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

#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/arm9/exceptions.h>

#include <calico/arm/psr.h>
#include <calico/system/dietprint.h>

//---------------------------------------------------------------------------------
static unsigned long ARMShift(const u32* exceptionRegisters, unsigned long value,unsigned char shift) {
//---------------------------------------------------------------------------------
	// no shift at all
	if (shift == 0x0B) return value ;
	int index ;
	if (shift & 0x01) {
		// shift index is a register
		index = exceptionRegisters[(shift >> 4) & 0x0F];
	} else {
		// constant shift index
		index = ((shift >> 3) & 0x1F) ;
	} ;
	int i ;
	bool isN ;
	switch (shift & 0x06) {
		case 0x00:
			// logical left
			return (value << index) ;
		case 0x02:
			// logical right
			return (value >> index) ;
		case 0x04:
			// arithmetical right
			isN = (value & 0x80000000) ;
			value = value >> index ;
			if (isN) {
				for (i=31;i>31-index;i--) {
					value = value | (1 << i) ;
				} ;
			} ;
			return value ;
		case 0x06:
			// rotate right
			index = index & 0x1F;
			value = (value >> index) | (value << (32-index));
			return value;
	};
	return value;
}

//---------------------------------------------------------------------------------
u32 getExceptionAddress(const ExcptContext* ctx, u32 opcodeAddress) {
//---------------------------------------------------------------------------------

	int Rf, Rb, Rd, Rn, Rm;
	const u32* exceptionRegisters = ctx->r;

	if (ctx->cpsr & ARM_PSR_T) {
		// Thumb

		unsigned short opcode = *(unsigned short *)opcodeAddress ;
		// ldr r,[pc,###]			01001ddd ffffffff
		// ldr r,[r,r]				0101xx0f ffbbbddd
		// ldrsh					0101xx1f ffbbbddd
		// ldr r,[r,imm]			011xxfff ffbbbddd
		// ldrh						1000xfff ffbbbddd
		// ldr r,[sp,###]			1001xddd ffffffff
		// push						1011x10l llllllll
		// ldm						1100xbbb llllllll


		if ((opcode & 0xF800) == 0x4800) {
			// ldr r,[pc,###]
			s8 offset = opcode & 0xff;
			return exceptionRegisters[15] + offset;
		} else if ((opcode & 0xF200) == 0x5000) {
			// ldr r,[r,r]
			Rb = (opcode >> 3) & 0x07 ;
			Rf = (opcode >> 6) & 0x07 ;
			return exceptionRegisters[Rb] + exceptionRegisters[Rf];

		} else if ((opcode & 0xF200) == 0x5200) {
			// ldrsh
			Rb = (opcode >> 3) & 0x07;
			Rf = (opcode >> 6) & 0x03;
			return exceptionRegisters[Rb] + exceptionRegisters[Rf];

		} else if ((opcode & 0xE000) == 0x6000) {
			// ldr r,[r,imm]
			Rb = (opcode >> 3) & 0x07;
			Rf = (opcode >> 6) & 0x1F ;
			return exceptionRegisters[Rb] + (Rf << 2);
		} else if ((opcode & 0xF000) == 0x8000) {
			// ldrh
			Rb = (opcode >> 3) & 0x07 ;
			Rf = (opcode >> 6) & 0x1F ;
			return exceptionRegisters[Rb] + (Rf << 2);
		} else if ((opcode & 0xF000) == 0x9000) {
			// ldr r,[sp,#imm]
			s8 offset = opcode & 0xff;
			return exceptionRegisters[13] + offset;
		} else if ((opcode & 0xF700) == 0xB500) {
			// push/pop
			return exceptionRegisters[13];
		} else if ((opcode & 0xF000) == 0xC000) {
			// ldm/stm
			Rd = (opcode >> 8) & 0x07;
			return exceptionRegisters[Rd];
		}
	} else {
		// arm32
		unsigned long opcode = *(unsigned long *)opcodeAddress ;

		// SWP			xxxx0001 0x00nnnn dddd0000 1001mmmm
		// STR/LDR		xxxx01xx xxxxnnnn ddddffff ffffffff
		// STRH/LDRH	xxxx000x x0xxnnnn dddd0000 1xx1mmmm
		// STRH/LDRH	xxxx000x x1xxnnnn ddddffff 1xx1ffff
		// STM/LDM		xxxx100x xxxxnnnn llllllll llllllll

		if ((opcode & 0x0FB00FF0) == 0x01000090) {
			// SWP
			Rn = (opcode >> 16) & 0x0F;
			return exceptionRegisters[Rn];
		} else if ((opcode & 0x0C000000) == 0x04000000) {
			// STR/LDR
			Rn = (opcode >> 16) & 0x0F;
			if (opcode & 0x02000000) {
				// Register offset
				Rm = opcode & 0x0F;
				if (opcode & 0x01000000) {
					unsigned short shift = (unsigned short)((opcode >> 4) & 0xFF) ;
					// pre indexing
					long Offset = ARMShift(exceptionRegisters,exceptionRegisters[Rm],shift);
					// add or sub the offset depending on the U-Bit
					return exceptionRegisters[Rn] + ((opcode & 0x00800000)?Offset:-Offset);
				} else {
					// post indexing
					return exceptionRegisters[Rn];
				}
			} else {
				// Immediate offset
				unsigned long Offset = (opcode & 0xFFF) ;
				if (opcode & 0x01000000) {
					// pre indexing
					// add or sub the offset depending on the U-Bit
					return exceptionRegisters[Rn] + ((opcode & 0x00800000)?Offset:-Offset);
				} else {
					// post indexing
					return exceptionRegisters[Rn];
				}
			}
		} else if ((opcode & 0x0E400F90) == 0x00000090) {
			// LDRH/STRH with register Rm
			Rn = (opcode >> 16) & 0x0F;
			Rd = (opcode >> 12) & 0x0F;
			Rm = opcode & 0x0F;
			unsigned short shift = (unsigned short)((opcode >> 4) & 0xFF);
			long Offset = ARMShift(exceptionRegisters,exceptionRegisters[Rm],shift);
			// add or sub the offset depending on the U-Bit
			return exceptionRegisters[Rn] + ((opcode & 0x00800000)?Offset:-Offset);
		} else if ((opcode & 0x0E400F90) == 0x00400090) {
			// LDRH/STRH with immediate offset
			Rn = (opcode >> 16) & 0x0F;
			Rd = (opcode >> 12) & 0x0F;
			unsigned long Offset = (opcode & 0xF) | ((opcode & 0xF00)>>8) ;
			// add or sub the offset depending on the U-Bit
			return exceptionRegisters[Rn] + ((opcode & 0x00800000)?Offset:-Offset) ;
		} else if ((opcode & 0x0E000000) == 0x08000000) {
			// LDM/STM
			Rn = (opcode >> 16) & 0x0F;
			return exceptionRegisters[Rn];
		}
	}
	return 0;
}

static const char* const registerNames[] =
	{	"r0","r1","r2","r3","r4","r5","r6","r7",
		"r8 ","r9 ","r10","r11","r12","sp ","lr ","pc " };

//---------------------------------------------------------------------------------
void guruMeditationDump(ExcptContext* ctx, unsigned flags) {
//---------------------------------------------------------------------------------
	unsigned cpuMode = ctx->cpsr & 0x1f;
	unsigned excptMode = ctx->excpt_cpsr & 0x1f;

	u32 codeAddress = ctx->r[15];
	u32 exceptionAddress = 0;

	dietPrint("     Guru Meditation Error!\n");

	if (!(flags & 2) && excptMode == ARM_PSR_MODE_ABT) {
		// The BIOS does not provide a mechanism to distinguish data aborts from
		// prefetch aborts. Assume a prefetch abort if the PC is outside the usual
		// code memory range (ITCM, main RAM).
		u32 dabtCodeAddr = codeAddress-8;
		u32 pabtCodeAddr = codeAddress-4;
		flags = pabtCodeAddr < 0x01000000 || pabtCodeAddr >= 0x03000000;
		codeAddress = flags ? pabtCodeAddr : dabtCodeAddr;
	}

	if (excptMode == ARM_PSR_MODE_ABT) {
		if (flags & 1) {
			dietPrint("        prefetch abort!\n\n");
			exceptionAddress = codeAddress;
		} else {
			dietPrint("          data abort!\n\n");
			exceptionAddress = getExceptionAddress(ctx, codeAddress);
		}
	} else /* if (excptMode == ARM_PSR_MODE_UND) */ {
		dietPrint("     undefined instruction!\n\n");
		codeAddress -= (ctx->cpsr & ARM_PSR_T) ? 2 : 4;
		exceptionAddress = codeAddress;
	}

	dietPrint("  pc: %08lX  addr: %08lX\n\n", codeAddress, exceptionAddress);
	for (unsigned i = 0; i < 7; i ++) {
		dietPrint("  %s: %08lX   %s: %08lX\n",
			registerNames[i],  ctx->r[i],
			registerNames[i+8],ctx->r[i+8]
		);
	}

	dietPrint("  %s: %08lX  cp15: %08lX\n", registerNames[7], ctx->r[7], ctx->cp15cr);
	if (cpuMode == ARM_PSR_MODE_USR || cpuMode == ARM_PSR_MODE_SYS) {
		dietPrint("cpsr: %08lX\n", ctx->cpsr);
	} else {
		dietPrint("cpsr: %08lX  spsr: %08lX\n", ctx->cpsr, ctx->spsr);
	}
}
