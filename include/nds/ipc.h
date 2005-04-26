//////////////////////////////////////////////////////////////////////
//
// ipc.h -- Inter-processor communication
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

#ifndef NDS_IPC_INCLUDE
#define NDS_IPC_INCLUDE

//////////////////////////////////////////////////////////////////////

#include <NDS/jtypes.h>

//////////////////////////////////////////////////////////////////////

// It tastes like a monkey.  A monkey that's past it's prime!
typedef struct sTransferRegion {
  uint32 heartbeat;         // counts frames

  uint16 touchX, touchY;    // TSC X, Y
  uint16 touchZ1, touchZ2;  // TSC x-panel measurements
  uint16 tdiode1, tdiode2;  // TSC temperature diodes
  uint32 temperature;       // TSC computed temperature

  uint16 buttons;           // X, Y, /PENIRQ buttons
  uint8 curtime[8];         // current time response from RTC

  // Don't rely on these below, will change or be removed in the future
  vuint32 mailAddr;
  vuint32 mailData;
  vuint8 mailRead;
  vuint8 mailBusy;
  vuint8 mailSize;
} TransferRegion, * pTransferRegion;

//////////////////////////////////////////////////////////////////////

#define IPC ((TransferRegion volatile *)(0x037FF000))

#define IPC_PEN_DOWN BIT(6)
#define IPC_X BIT(0)
#define IPC_Y BIT(1)
#define IPC_LID_CLOSED BIT(7)
//////////////////////////////////////////////////////////////////////

// Synchronization register (name *will* change!)
#define MAGIC180      (*(vuint16*)0x04000180)

//////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////

