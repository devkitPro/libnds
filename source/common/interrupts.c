/*---------------------------------------------------------------------------------

	Copyright (C) 2005
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

---------------------------------------------------------------------------------*/

#include <nds/interrupts.h>
#include <nds/system.h>
#include <nds/ipc.h>

void IntrMain();	// Prototype for assembly interrupt dispatcher

//---------------------------------------------------------------------------------
void irqDummy(void) {}
//---------------------------------------------------------------------------------


#ifdef ARM9
#define INT_TABLE_SECTION __attribute__((section(".itcm")))
#else
#define INT_TABLE_SECTION
#endif

struct IntTable irqTable[MAX_INTERRUPTS] INT_TABLE_SECTION;

//---------------------------------------------------------------------------------
void irqSet(int mask, IntFn handler) {
//---------------------------------------------------------------------------------
	if (!mask) return;

	int i;

	for	(i=0;i<MAX_INTERRUPTS;i++)
		if	(!irqTable[i].mask || irqTable[i].mask == mask) break;

	if ( i == MAX_INTERRUPTS ) return;

	irqTable[i].handler	= handler;
	irqTable[i].mask	= mask;

	if(mask & IRQ_VBLANK)
		REG_DISPSTAT |= DISP_VBLANK_IRQ ;
	if(mask & IRQ_HBLANK)
		REG_DISPSTAT |= DISP_HBLANK_IRQ ;
	if(mask & IRQ_IPC_SYNC)
		REG_IPC_SYNC |= IPC_SYNC_IRQ_ENABLE;
}

//---------------------------------------------------------------------------------
void irqInit() {
//---------------------------------------------------------------------------------
	int i;

	REG_IE	= 0;			// disable all interrupts

	// Set all interrupts to dummy functions.
	for(i = 0; i < MAX_INTERRUPTS; i ++)
	{
		irqTable[i].handler = irqDummy;
		irqTable[i].mask = 0;
	}

	IRQ_HANDLER = IntrMain;

	REG_IF	= IRQ_ALL;		// clear all pending interrupts
	REG_IME = 1;			// enable global interrupt

}


//---------------------------------------------------------------------------------
void irqInitHandler(IntFn handler) {
//---------------------------------------------------------------------------------
	REG_IME = 0;
	REG_IF = ~0;
	REG_IE = 0;

	IRQ_HANDLER = handler;

	REG_IME = 1;
}

//---------------------------------------------------------------------------------
void irqEnable(uint32 irq) {
//---------------------------------------------------------------------------------
	if (irq & IRQ_VBLANK)
		REG_DISPSTAT |= DISP_VBLANK_IRQ ;
	if (irq & IRQ_HBLANK)
		REG_DISPSTAT |= DISP_HBLANK_IRQ ;
	if (irq & IRQ_VCOUNT)
		REG_DISPSTAT |= DISP_YTRIGGER_IRQ;
	if(irq & IRQ_IPC_SYNC)
		REG_IPC_SYNC |= IPC_SYNC_IRQ_ENABLE;

	REG_IE |= irq;
}

//---------------------------------------------------------------------------------
void irqDisable(uint32 irq) {
//---------------------------------------------------------------------------------
	if (irq & IRQ_VBLANK)
		REG_DISPSTAT &= ~DISP_VBLANK_IRQ ;
	if (irq & IRQ_HBLANK)
		REG_DISPSTAT &= ~DISP_HBLANK_IRQ ;
	if (irq & IRQ_VCOUNT)
		REG_DISPSTAT &= ~DISP_YTRIGGER_IRQ;
	if(irq & IRQ_IPC_SYNC)
		REG_IPC_SYNC &= ~IPC_SYNC_IRQ_ENABLE;

	REG_IE &= ~irq;
}

//---------------------------------------------------------------------------------
void irqClear(int mask) {
//---------------------------------------------------------------------------------
	int i = 0;

	for	(i=0;i<MAX_INTERRUPTS;i++)
		if	(irqTable[i].mask == mask) break;

	if ( i == MAX_INTERRUPTS ) return;

	irqDisable( mask);

	irqTable[i].handler	= irqDummy;
}
