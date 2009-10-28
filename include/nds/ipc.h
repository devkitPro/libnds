/*---------------------------------------------------------------------------------

	Inter Processor Communication

	Copyright (C) 2005
		Michael Noland (joat)
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

#ifndef NDS_IPC_INCLUDE
#define NDS_IPC_INCLUDE

#include "ndstypes.h"

//---------------------------------------------------------------------------------
// Synchronization register
//---------------------------------------------------------------------------------
#define REG_IPC_SYNC	(*(vuint16*)0x04000180)

enum IPC_SYNC_BITS {
	IPC_SYNC_IRQ_ENABLE		=	BIT(14),
	IPC_SYNC_IRQ_REQUEST	=	BIT(13)
};

//---------------------------------------------------------------------------------
static inline void IPC_SendSync(unsigned int sync) {
//---------------------------------------------------------------------------------
	REG_IPC_SYNC = (REG_IPC_SYNC & 0xf0ff) | (((sync) & 0x0f) << 8) | IPC_SYNC_IRQ_REQUEST;
}

//---------------------------------------------------------------------------------
static inline int IPC_GetSync() {
//---------------------------------------------------------------------------------
	return REG_IPC_SYNC & 0x0f;
}

//---------------------------------------------------------------------------------
// fifo
//---------------------------------------------------------------------------------
#define REG_IPC_FIFO_TX		(*(vu32*)0x4000188)
#define REG_IPC_FIFO_RX		(*(vu32*)0x4100000)
#define REG_IPC_FIFO_CR		(*(vu16*)0x4000184)

enum IPC_CONTROL_BITS {
	IPC_FIFO_SEND_EMPTY	=	(1<<0),
	IPC_FIFO_SEND_FULL	=	(1<<1),
	IPC_FIFO_SEND_IRQ	=	(1<<2),
	IPC_FIFO_SEND_CLEAR	=	(1<<3),
	IPC_FIFO_RECV_EMPTY	=	(1<<8),
	IPC_FIFO_RECV_FULL	=	(1<<9),
	IPC_FIFO_RECV_IRQ	=	(1<<10),
	IPC_FIFO_ERROR		=	(1<<14),
	IPC_FIFO_ENABLE		=	(1<<15)
};

#endif // NDS_IPC_INCLUDE


