/*---------------------------------------------------------------------------------

initSystem.c -- Code for initialising the DS

Copyright (C) 2007 - 2010
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
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/timers.h>
#include <nds/dma.h>
#include <nds/ipc.h>
#include <nds/arm9/video.h>
#include <nds/arm9/sprite.h>
#include <nds/arm9/input.h>
#include <nds/system.h>
#include <nds/interrupts.h>
#include <nds/fifocommon.h>
#include <time.h>
#include <libnds_internal.h>

#include <sys/iosupport.h>
void __libnds_exit(int rc);
extern time_t *punixTime;

//---------------------------------------------------------------------------------
// Reset the DS registers to sensible defaults
//---------------------------------------------------------------------------------
void __attribute__((weak)) initSystem(void) {
//---------------------------------------------------------------------------------
	register int i;
	//stop timers and dma
	for (i=0; i<4; i++) 
	{
		DMA_CR(i) = 0;
		DMA_SRC(i) = 0;
		DMA_DEST(i) = 0;
		TIMER_CR(i) = 0;
		TIMER_DATA(i) = 0;
	}


	//clear video display registers
	dmaFillWords(0, (void*)0x04000000, 0x56);  
	dmaFillWords(0, (void*)0x04001008, 0x56);  
	videoSetModeSub(0);

	vramDefault();

	VRAM_E_CR = 0;
	VRAM_F_CR = 0;
	VRAM_G_CR = 0;
	VRAM_H_CR = 0;
	VRAM_I_CR = 0;

	irqInit();
	fifoInit();

	fifoSetValue32Handler(FIFO_PM, powerValueHandler, 0);
	fifoSetDatamsgHandler(FIFO_SYSTEM, systemMsgHandler, 0);

	if(REG_DSIMODE) {
		fifoSendValue32(FIFO_PM,PM_DSI_HACK);
	}
	__transferRegion()->buttons = 0xffff;

	punixTime = (time_t*)memUncached((void *)&__transferRegion()->unixTime);

	__syscalls.exit = __libnds_exit;
	extern  char *fake_heap_end;
	__transferRegion()->bootcode = (struct __bootstub *)fake_heap_end;
	irqEnable(IRQ_VBLANK);

}
