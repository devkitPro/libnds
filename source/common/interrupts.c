/*---------------------------------------------------------------------------------
	$Id: interrupts.c,v 1.5 2005-09-20 05:00:24 wntrmute Exp $

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

	$Log: not supported by cvs2svn $
	Revision 1.4  2005/09/04 16:28:04  wntrmute
	place irqTable in ewram

	Revision 1.3  2005/09/03 17:09:35  wntrmute
	added interworking aware interrupt dispatcher


---------------------------------------------------------------------------------*/

#include <nds/interrupts.h>
#include <nds/system.h>

//---------------------------------------------------------------------------------
void irqDummy(void) {}
//---------------------------------------------------------------------------------

// Placing the irqTable in dtcm fails, currently no idea why.

//#ifdef ARM9
//#define INT_TABLE_SECTION __attribute__((section(".sbss")))
//#else
#define INT_TABLE_SECTION
//#endif

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
		DISP_SR |= DISP_VBLANK_IRQ ;
	if(mask & IRQ_HBLANK)
		DISP_SR |= DISP_HBLANK_IRQ ;

	REG_IE |= mask;
}

//---------------------------------------------------------------------------------
void irqInit() {
//---------------------------------------------------------------------------------
	int i;

	// Set all interrupts to dummy functions.
	for(i = 0; i < MAX_INTERRUPTS; i ++)
	{
		irqTable[i].handler = irqDummy;
		irqTable[i].mask = 0;
	}

	IRQ_HANDLER = IntrMain;

}


//---------------------------------------------------------------------------------
void irqClear(int mask) {
//---------------------------------------------------------------------------------
	int i = 0;

	for	(i=0;i<MAX_INTERRUPTS;i++)
		if	(irqTable[i].mask == mask) break;

	if ( i == MAX_INTERRUPTS ) return;

	irqTable[i].handler	= irqDummy;

	if(mask & IRQ_VBLANK)
		DISP_SR &= ~DISP_VBLANK_IRQ ;
	if(mask & IRQ_HBLANK)
		DISP_SR &= ~DISP_HBLANK_IRQ ;

	REG_IE &= ~mask;
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
void irqEnable(int irq) {
//---------------------------------------------------------------------------------
	REG_IE |= irq;
	REG_IME = 1;
}

//---------------------------------------------------------------------------------
void irqDisable(int irq) {
//---------------------------------------------------------------------------------
	REG_IE &= ~irq;
}

