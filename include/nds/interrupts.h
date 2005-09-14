/*---------------------------------------------------------------------------------
	$Id: interrupts.h,v 1.7 2005-09-14 06:20:57 wntrmute Exp $

	Interrupt registers and vector pointers

	Copyright (C) 2005
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

	$Log: not supported by cvs2svn $
	Revision 1.6  2005/09/04 16:27:22  wntrmute
	added 1 to MAX_INTERRUPTS

	Revision 1.5  2005/09/03 17:09:35  wntrmute
	added interworking aware interrupt dispatcher

	Revision 1.4  2005/08/03 05:17:26  wntrmute
	use BIT macro for consistency

	Revision 1.3  2005/08/01 23:18:22  wntrmute
	adjusted headers for logging


---------------------------------------------------------------------------------*/

#ifndef NDS_INTERRUPTS_INCLUDE
#define NDS_INTERRUPTS_INCLUDE

#include <nds/jtypes.h>

// Interrupt flags for IE and IF
enum IRQ_MASKS {
	IRQ_VBLANK			=	BIT(0),
	IRQ_HBLANK			=	BIT(1),
	IRQ_YTRIGGER		=	BIT(2),
	IRQ_TIMER0			=	BIT(3),
	IRQ_TIMER1			=	BIT(4),
	IRQ_TIMER2			=	BIT(5),
	IRQ_TIMER3			=	BIT(6),
	IRQ_NETWORK			=	BIT(7),
	IRQ_DMA0			=	BIT(8),
	IRQ_DMA1			=	BIT(9),
	IRQ_DMA2			=	BIT(10),
	IRQ_DMA3			=	BIT(11),
	IRQ_KEYS			=	BIT(12),
	IRQ_CART			=	BIT(13),
	IRQ_IPC_SYNC		=	BIT(16),
	IRQ_FIFO_EMPTY		=	BIT(17),
	IRQ_FIFO_NOT_EMPTY	=	BIT(18),
	IRQ_CARD			=	BIT(19),
	IRQ_CARD_LINE		=	BIT(20),
	IRQ_GEOMETRY_FIFO	=	BIT(21),
	IRQ_LID				=	BIT(22),
	IRQ_SPI				=	BIT(23),
	IRQ_WIFI			=	BIT(24),
	IRQ_ALL				=	(~0)
};

#define MAX_INTERRUPTS  25


/*---------------------------------------------------------------------------------
	IE: Interrupt Enable Register
	This is the activation mask for the internal interrupts.  Unless
	the corresponding bit is set, the IRQ will be masked out.
---------------------------------------------------------------------------------*/
#define IE             (*(vuint32*)0x04000210)

/*---------------------------------------------------------------------------------
	IF: Interrupt Flags Register
	Since there is only one hardware interrupt vector, the IF register
	contains flags to indicate when a particular sort of interrupt
	has occured.
---------------------------------------------------------------------------------*/
#define IF             (*(vuint32*)0x04000214)

/*---------------------------------------------------------------------------------
	IME: Interrupt Master Enable Register
	When bit 0 is clear, all interrupts are masked.  When it is 1,
	interrupts will occur if not masked out.
---------------------------------------------------------------------------------*/
#define IME            (*(vuint16*)0x04000208)

enum IME_VALUES {
	IME_DISABLED = 0,
	IME_ENABLED = 1,
};


#ifdef ARM7
#define VBLANK_INTR_WAIT_FLAGS  (*(vuint32*)(0x04000000-8))
#define IRQ_HANDLER             (*(VoidFunctionPointer *)(0x04000000-4))
#endif

#ifdef ARM9

#define VBLANK_INTR_WAIT_FLAGS  (*(vuint32*)0x00803FF8)
#define IRQ_HANDLER             (*(VoidFunctionPointer *)0x00803FFC)
#endif

#ifdef __cplusplus
extern "C" {
#endif


struct IntTable{IntFn handler; u32 mask;};

void irqInit();
void irqSet(int irq, VoidFunctionPointer handler);
void irqClear(int irq);
void irqInitHandler(VoidFunctionPointer handler);
void irqEnable(int irq);
void irqDisable(int irq);
void IntrMain();

#ifdef __cplusplus
}
#endif

#endif //NDS_INTERRUPTS_INCLUDE

