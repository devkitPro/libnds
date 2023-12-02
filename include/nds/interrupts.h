/*---------------------------------------------------------------------------------

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


---------------------------------------------------------------------------------*/

/*! \file interrupts.h
    \brief nds interrupt support.
*/

#ifndef NDS_INTERRUPTS_INCLUDE
#define NDS_INTERRUPTS_INCLUDE

#include <nds/ndstypes.h>
#include <calico/system/irq.h>
#include <calico/system/thread.h>

#define REG_AUXIE          REG_IE2
#define REG_AUXIF          REG_IF2

#define IRQ_NETWORK        IRQ_RTC
#define IRQ_KEYS           IRQ_KEYPAD
#define IRQ_CART           IRQ_SLOT2
#define IRQ_IPC_SYNC       IRQ_PXI_SYNC
#define IRQ_FIFO_EMPTY     IRQ_PXI_SEND
#define IRQ_FIFO_NOT_EMPTY IRQ_PXI_RECV
#define IRQ_CARD           IRQ_SLOT1_TX
#define IRQ_CARD_LINE      IRQ_SLOT1_IREQ
#define IRQ_LID            IRQ_HINGE
#define IRQ_ALL            UINT32_MAX

#define IRQ_I2C            IRQ2_MCU
#define IRQ_SDMMC          IRQ2_TMIO0

#define irqSetAUX          irqSet2
#define irqClearAUX        irqClear2
#define irqEnableAUX       irqEnable2
#define irqDisableAUX      irqDisable2

#define swiIntrWait        threadIrqWait
#define swiWaitForVBlank   threadWaitForVBlank

typedef u32 IRQ_MASKS;
typedef u32 IRQ_MASKSAUX;

//! maximum number of interrupts.
#define MAX_INTERRUPTS  MK_IRQ_NUM_HANDLERS

//! values allowed for REG_IME
enum IME_VALUE {
	IME_DISABLE = 0, 	/*!< Disable all interrupts. */
	IME_ENABLE = 1,		/*!< Enable all interrupts not masked out in REG_IE */
};

#ifdef __cplusplus
extern "C" {
#endif

static inline int enterCriticalSection(void) {
	return irqLock();
}

static inline void leaveCriticalSection(int oldIME) {
	irqUnlock(oldIME);
}

#ifdef __cplusplus
}
#endif

#endif //NDS_INTERRUPTS_INCLUDE
