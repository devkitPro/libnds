/*---------------------------------------------------------------------------------

system.c -- System code

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

$Log: 

---------------------------------------------------------------------------------*/

#include <nds/memory.h>

//////////////////////////////////////////////////////////////////////

// Changes all bus ownerships
void sysSetBusOwners(bool arm9rom, bool arm9sram, bool arm9card) {
  uint16 pattern = WAIT_CR & ~(ARM9_OWNS_SRAM|ARM9_OWNS_CARD|ARM9_OWNS_ROM);
  pattern = pattern | (arm9sram ? 0 : ARM9_OWNS_SRAM) | 
                      (arm9card ? 0 : ARM9_OWNS_CARD) |
                      (arm9rom ? 0 : ARM9_OWNS_ROM);
  WAIT_CR = pattern;
}

//////////////////////////////////////////////////////////////////////

// Changes only the gba rom cartridge ownership
void sysSetCartOwner(bool arm9) {
  WAIT_CR = (WAIT_CR & ~0x0080) | (arm9 ? 0 : 0x80);
}

//////////////////////////////////////////////////////////////////////
