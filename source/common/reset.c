/*---------------------------------------------------------------------------------

	reset API

  Copyright (C) 2010
			Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.


---------------------------------------------------------------------------------*/
#include <nds/ndstypes.h>
#include <nds/ipc.h>

#ifdef ARM7

void resetARM9(u32 address) {
	*((vu32*)0x02FFFE24) = address;

#else

void resetARM7(u32 address) {
	*((vu32*)0x02FFFE34) = address;

#endif

	REG_IPC_FIFO_TX = 0x0c04000c;
	while((REG_IPC_SYNC & 0x0f) != 1);
	REG_IPC_SYNC = 0x100;
	while((REG_IPC_SYNC & 0x0f) != 0);
	REG_IPC_SYNC = 0;	
}

