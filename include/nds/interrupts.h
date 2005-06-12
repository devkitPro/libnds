














//////////////////////////////////////////////////////////////////////
//
// interrupts.h -- Interrupt registers and vector pointers
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
//	 0.2: Added a simple slow default irq handler and support functions
//
//////////////////////////////////////////////////////////////////////

#ifndef NDS_INTERRUPTS_INCLUDE
#define NDS_INTERRUPTS_INCLUDE

//////////////////////////////////////////////////////////////////////
// Interrupt Control /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

// Interrupt flags for IE and IF
#define IRQ_VBLANK     (1 << 0)
#define IRQ_HBLANK     (1 << 1)
#define IRQ_YTRIGGER   (1 << 2)
#define IRQ_TIMER0     (1 << 3)
#define IRQ_TIMER1     (1 << 4)
#define IRQ_TIMER2     (1 << 5)
#define IRQ_TIMER3     (1 << 6)
#define IRQ_NETWORK    (1 << 7)
#define IRQ_DMA0       (1 << 8)
#define IRQ_DMA1       (1 << 9)
#define IRQ_DMA2       (1 << 10)
#define IRQ_DMA3       (1 << 11)
#define IRQ_KEYS       (1 << 12)
#define IRQ_CART       (1 << 13)
#define IRQ_SYNC		(1 << 16)
#define IRQ_CARD       (1 << 19)
#define IRQ_CARD_LINE  (1 << 20)

#define IRQ_IPC_SYNC    BIT(16)
#define IRQ_FIFO_EMPTY  BIT(17)
#define IRQ_FIFO_FULL	BIT(18)
#define IRQ_ALL			(~0)



//////////////////////////////////////////////////////////////////////

// IE: Interrupt Enable Register
// This is the activation mask for the internal interrupts.  Unless
// the corresponding bit is set, the IRQ will be masked out.
#define IE             (*(vuint32*)0x04000210)

// IF: Interrupt Flags Register
// Since there is only one hardware interrupt vector, the IF register
// contains flags to indicate when a particular sort of interrupt
// has occured.
#define IF             (*(vuint32*)0x04000214)

// IME: Interrupt Master Enable Register
// When bit 0 is clear, all interrupts are masked.  When it is 1,
// interrupts will occur if not masked out.
#define IME            (*(vuint16*)0x04000208)

// Values for IME
#define IME_DISABLED   (0)
#define IME_ENABLED    (1)


//////////////////////////////////////////////////////////////////////

#ifdef ARM7
#define VBLANK_INTR_WAIT_FLAGS  (*(vuint32*)(0x04000000-8))
#define IRQ_HANDLER             (*(VoidFunctionPointer *)(0x04000000-4))
#endif

#ifdef ARM9
//#include <NDS/ARM9/CP15.h>
//#define VBLANK_INTR_WAIT_FLAGS  (*(vuint32*)((CP15_GetDTCM() & ~0xFFF) + 0x3FF8))
//#define IRQ_HANDLER             (*(VoidFunctionPointer *)((CP15_GetDTCM() & ~0xFFF) + 0x3FFC))
#define VBLANK_INTR_WAIT_FLAGS  (*(vuint32*)0x00803FF8)
#define IRQ_HANDLER             (*(VoidFunctionPointer *)0x00803FFC)
#endif

//////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
//////////////////////////////////////////////////////////////////////
void irqSet(int irq, VoidFunctionPointer handler);
void irqClear(int irq);
void irqInitHandler(VoidFunctionPointer handler);
void irqDefaultHandler(void);
void irqEnable(int irq);
void irqDisable(int irq);
//////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////
