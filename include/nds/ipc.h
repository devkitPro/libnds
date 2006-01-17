/*---------------------------------------------------------------------------------
	$Id: ipc.h,v 1.13 2006-01-17 09:47:00 wntrmute Exp $

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

	$Log: not supported by cvs2svn $
	Revision 1.12  2005/09/20 04:59:04  wntrmute
	replaced defines with enums
	replaced macros with static inlines
	
	Revision 1.11  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.10  2005/08/03 05:26:42  wntrmute
	use BIT macro
	corrected header include

	Revision 1.9  2005/07/27 02:20:05  wntrmute
	resynchronise with ndslib
	Updated GL with float wrappers for NeHe


---------------------------------------------------------------------------------*/

#ifndef NDS_IPC_INCLUDE
#define NDS_IPC_INCLUDE


#include <nds/jtypes.h>


//---------------------------------------------------------------------------------
typedef struct sTransferSoundData {
//---------------------------------------------------------------------------------
  const void *data;
  u32 len;
  u32 rate;
  u8 vol;
  u8 pan;
  u8 format;
  u8 PADDING;
} TransferSoundData, * pTransferSoundData;


//---------------------------------------------------------------------------------
typedef struct sTransferSound {
//---------------------------------------------------------------------------------
  TransferSoundData data[16];
  u8 count;
  u8 PADDING[3];
} TransferSound, * pTransferSound;


//---------------------------------------------------------------------------------
typedef struct sTransferRegion {
//---------------------------------------------------------------------------------
  uint32 heartbeat;          // counts frames

	int16 touchX,   touchY;   // TSC X, Y
	int16 touchXpx, touchYpx; // TSC X, Y pixel values
	int16 touchZ1,  touchZ2;  // TSC x-panel measurements
	uint16 tdiode1,  tdiode2;  // TSC temperature diodes
	uint32 temperature;        // TSC computed temperature

	uint16 buttons;            // X, Y, /PENIRQ buttons

	union {
		uint8 curtime[8];        // current time response from RTC

    struct {
			u8 rtc_command;
			u8 rtc_year;           //add 2000 to get 4 digit year
			u8 rtc_month;          //1 to 12
			u8 rtc_day;            //1 to (days in month)

			u8 rtc_incr;
			u8 rtc_hours;          //0 to 11 for AM, 52 to 63 for PM
			u8 rtc_minutes;        //0 to 59
			u8 rtc_seconds;        //0 to 59
    };
  };

	uint16 battery;            // battery life ??  hopefully.  :)
	uint16 aux;                // i have no idea...

  // Don't rely on these below, will change or be removed in the future
	pTransferSound soundData;

  vuint32 mailAddr;
  vuint32 mailData;
  vuint8 mailRead;
  vuint8 mailBusy;
  vuint32 mailSize;
} TransferRegion, * pTransferRegion;


#define IPC ((TransferRegion volatile *)(0x027FF000))

#define IPC_PEN_DOWN BIT(6)
#define IPC_X BIT(0)
#define IPC_Y BIT(1)
#define IPC_LID_CLOSED BIT(7)

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
	IPC_FIFO_SEND_IRQ		=	(1<<2),
	IPC_FIFO_SEND_CLEAR	=	(1<<3),
	IPC_FIFO_RECV_EMPTY	=	(1<<8),
	IPC_FIFO_RECV_FULL	=	(1<<9),
	IPC_FIFO_RECV_IRQ		=	(1<<10),
	IPC_FIFO_ERROR			=	(1<<14),
	IPC_FIFO_ENABLE			=	(1<<15)
};

#endif


