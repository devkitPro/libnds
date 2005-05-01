//////////////////////////////////////////////////////////////////////
//
// interrupts.cpp -- Default interrupt handler
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
//
//////////////////////////////////////////////////////////////////////

#include <NDS/NDS.h>


void irqDummy(void)
{
	IF = IF;
}

VoidFunctionPointer irqTable[32] = 
{
	irqDummy, irqDummy, irqDummy, irqDummy,
	irqDummy, irqDummy, irqDummy, irqDummy,
	irqDummy, irqDummy, irqDummy, irqDummy,
	irqDummy, irqDummy, irqDummy, irqDummy,
	
	irqDummy, irqDummy, irqDummy, irqDummy,
	irqDummy, irqDummy, irqDummy, irqDummy,
	irqDummy, irqDummy, irqDummy, irqDummy,
	irqDummy, irqDummy, irqDummy, irqDummy
};

void irqSet(int irq, VoidFunctionPointer handler)
{
	int i = 0;
	
	if(handler)
		for (i = 0; i < 32; i++)
			if(irq & (1 << i) )irqTable[i] = handler;
	
	if(irq & IRQ_VBLANK)
		DISP_SR |= DISP_VBLANK_IRQ ;
	if(irq & IRQ_HBLANK)
		DISP_SR |= DISP_HBLANK_IRQ ;

	IE |= irq;
}
void irqClear(int irq)
{
	int i = 0;

	for (i = 0; i < 32; i++)
		if(irq & (1 << i) )irqTable[i] = irqDummy;
	

	IE &= ~irq;
}

void irqDefaultHandler(void)
{
	int i = 0;

	for (i = 0; i < 32; i++)
	{
		if(IF & (1 << i) )irqTable[i]();
	}
	
	VBLANK_INTR_WAIT_FLAGS = IF | IE;
}

void irqInitHandler(VoidFunctionPointer handler)
{
	IME = 0;
	IE = 0;
	IF = ~0;

	IRQ_HANDLER = handler;
	
	IME = 1;
}

void irqEnable(int irq)
{
	IE |= irq;
	IME = 1;
}

void irqDisable(int irq)
{
	IE &= ~irq;
}

