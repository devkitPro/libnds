/*---------------------------------------------------------------------------------

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
/*! \file dma.h
\brief Wrapper functions for direct memory access hardware

<div class="fileHeader">
The DS has 4 hardware direct memory access devices per CPU which can be used to transfer or fill chunks of memeory without
CPU intervention.  Utilizing DMA is generaly faster than CPU copies (memcpy, swiCopy, for loops, etc..).

DMA has no access to data caches on the DS and as such will give unexpected results when DMAing data from main memory.  The cache must
be flushed as follows when using DMA to ensure proper opertion on the arm9:

<pre>
DC_FlushRange(source, sizeof(dataToCopy));
dmaCopy(source, destination, sizeof(dataToCopy));
</pre>

</div>
*/

#ifndef NDS_DMA_INCLUDE
#define NDS_DMA_INCLUDE

#include "nds/ndstypes.h"
#include <calico/nds/dma.h>

#define DMA_SRC(n)     REG_DMAxSAD(n)
#define DMA_DEST(n)    REG_DMAxDAD(n)
#define DMA_CR(n)      REG_DMAxCNT(n)
#define DMA_FILL(n)    REG_DMAxFIL(n)

#define DMA0_SRC       DMA_SRC(0)
#define DMA0_DEST      DMA_DEST(0)
#define DMA0_CR        DMA_CR(0)

#define DMA1_SRC       DMA_SRC(1)
#define DMA1_DEST      DMA_DEST(1)
#define DMA1_CR        DMA_CR(1)

#define DMA2_SRC       DMA_SRC(2)
#define DMA2_DEST      DMA_DEST(2)
#define DMA2_CR        DMA_CR(2)

#define DMA3_SRC       DMA_SRC(3)
#define DMA3_DEST      DMA_DEST(3)
#define DMA3_CR        DMA_CR(3)

// DMA control register contents
// The defaults are 16-bit, increment source/dest addresses, no irq
#define DMA_ENABLE      (DMA_START<<16)
#define DMA_BUSY	    (DMA_START<<16)
#define DMA_IRQ_REQ     (DMA_IRQ_ENABLE<<16)

#define DMA_START_NOW   (DMA_TIMING(DmaTiming_Immediate)<<16)
#define DMA_START_CARD  (DMA_TIMING(DmaTiming_Slot1)<<16)
#define DMA_START_VBL   (DMA_TIMING(DmaTiming_VBlank)<<16)

#ifdef ARM9
#define DMA_START_HBL   (DMA_TIMING(DmaTiming_HBlank)<<16)
#define DMA_START_FIFO	(DMA_TIMING(DmaTiming_3dFifo)<<16)
#define DMA_DISP_FIFO	(DMA_TIMING(DmaTiming_MemDisp)<<16)
#endif

#define DMA_16_BIT      (DMA_UNIT_16<<16)
#define DMA_32_BIT      (DMA_UNIT_32<<16)

#define DMA_REPEAT      (DMA_MODE_REPEAT<<16)

#define DMA_SRC_INC     (DMA_MODE_SRC(DmaMode_Increment)<<16)
#define DMA_SRC_DEC     (DMA_MODE_SRC(DmaMode_Decrement)<<16)
#define DMA_SRC_FIX     (DMA_MODE_SRC(DmaMode_Fixed)<<16)

#define DMA_DST_INC     (DMA_MODE_DST(DmaMode_Increment)<<16)
#define DMA_DST_DEC     (DMA_MODE_DST(DmaMode_Decrement)<<16)
#define DMA_DST_FIX     (DMA_MODE_DST(DmaMode_Fixed)<<16)
#define DMA_DST_RESET   (DMA_MODE_DST(DmaMode_IncrReload)<<16)

#define DMA_COPY_WORDS     (DMA_ENABLE | DMA_32_BIT | DMA_START_NOW)
#define DMA_COPY_HALFWORDS (DMA_ENABLE | DMA_16_BIT | DMA_START_NOW)
#define DMA_FIFO	(DMA_ENABLE | DMA_32_BIT  | DMA_DST_FIX | DMA_START_FIFO)

static inline
/*! \fn void dmaCopyWords(u8 channel, const void* src, void* dest, u32 size)
    \brief copies from source to destination on one of the 4 available channels in words
    \param channel the dma channel to use (0 - 3).
    \param src the source to copy from
    \param dest the destination to copy to
    \param size the size in bytes of the data to copy.  Will be truncated to the nearest word (4 bytes)
*/
void dmaCopyWords(u8 channel, const void* src, void* dest, u32 size) {
	dmaStartCopy32(channel, dest, src, size);
	dmaBusyWait(channel);
}

static inline
/*! \fn void dmaCopyHalfWords(u8 channel, const void* src, void* dest, u32 size)
\brief copies from source to destination on one of the 4 available channels in half words
\param channel the dma channel to use (0 - 3).
\param src the source to copy from
\param dest the destination to copy to
\param size the size in bytes of the data to copy.  Will be truncated to the nearest half word (2 bytes)
*/
void dmaCopyHalfWords(u8 channel, const void* src, void* dest, u32 size) {
	dmaStartCopy16(channel, dest, src, size);
	dmaBusyWait(channel);
}

static inline
/*! \fn void dmaCopy(const void * source, void * dest, u32 size)
\brief copies from source to destination using channel 3 of DMA available channels in half words
\param source the source to copy from
\param dest the destination to copy to
\param size the size in bytes of the data to copy.  Will be truncated to the nearest half word (2 bytes)
*/
void dmaCopy(const void * source, void * dest, u32 size) {
	dmaCopyHalfWords(3, source, dest, size);
}

static inline
/*! \fn void dmaCopyWordsAsynch(u8 channel, const void* src, void* dest, u32 size)
\brief copies from source to destination on one of the 4 available channels in half words.
This function returns immediately after starting the transfer.
\param channel the dma channel to use (0 - 3).
\param src the source to copy from
\param dest the destination to copy to
\param size the size in bytes of the data to copy.  Will be truncated to the nearest word (4 bytes)
*/
void dmaCopyWordsAsynch(u8 channel, const void* src, void* dest, u32 size) {
	dmaStartCopy32(channel, dest, src, size);
}

static inline
/*! \fn void dmaCopyHalfWordsAsynch(u8 channel, const void* src, void* dest, u32 size)
\brief copies from source to destination on one of the 4 available channels in half words.
This function returns immediately after starting the transfer.
\param channel the dma channel to use (0 - 3).
\param src the source to copy from
\param dest the destination to copy to
\param size the size in bytes of the data to copy.  Will be truncated to the nearest half word (2 bytes)
*/
void dmaCopyHalfWordsAsynch(u8 channel, const void* src, void* dest, u32 size) {
	dmaStartCopy16(channel, dest, src, size);
}

static inline
/*! \fn void dmaCopyAsynch(const void* src, void* dest, u32 size)
\brief copies from source to destination using channel 3 of DMA available channels in half words.
This function returns immediately after starting the transfer.
\param src the source to copy from
\param dest the destination to copy to
\param size the size in bytes of the data to copy.  Will be truncated to the nearest half word (2 bytes)
*/
void dmaCopyAsynch(const void * source, void * dest, u32 size) {
	dmaCopyHalfWordsAsynch(3, source, dest, size);
}

static inline
/*! \fn void dmaFillWords( u32 value, void* dest, u32 size)
\brief fills the source with the supplied value using DMA channel 3

\param value the 32 byte value to fill memory with
\param dest the destination to copy to
\param size the size in bytes of the area to fill.  Will be truncated to the nearest word (4 bytes)
*/
void dmaFillWords(u32 value, void* dest, u32 size) {
	dmaStartFill32(3, dest, value, size);
	dmaBusyWait(3);
}

static inline
/*! \fn void dmaFillHalfWords( u16 value, void* dest, u32 size)
\brief fills the source with the supplied value using DMA channel 3

\param value the 16 byte value to fill memory with
\param dest the destination to copy to
\param size the size in bytes of the area to fill.  Will be truncated to the nearest half word (2 bytes)
*/
void dmaFillHalfWords(u16 value, void* dest, u32 size) {
	dmaStartFill16(3, dest, value, size);
	dmaBusyWait(3);
}

static inline
/*! \fn dmaBusy(u8 channel)
\brief determines if the specified channel is busy
\param channel the dma channel to check (0 - 3).
\return non zero if busy, 0 if channel is free
*/
int dmaBusy(u8 channel) {
	return dmaIsBusy(channel);
}

#endif
