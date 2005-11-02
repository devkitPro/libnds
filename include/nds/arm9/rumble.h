/*---------------------------------------------------------------------------------
	$Id: rumble.h,v 1.2 2005-11-02 07:06:20 bigredpimp Exp $
 
	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)
		Mike Parks (BigRedPimp)
 
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
 
---------------------------------------------------------------------------------*/

/*! \file rumble.h
    \brief nds rumble option pak support.
*/

#ifndef RUMBLE_H
#define RUMBLE_H

#define P __attribute__ ((__packed__))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct  
sGBAHeader {
  P uint32 entryPoint;
  P uint8 logo[156];
  P char title[0xC];
  P char gamecode[0x4];
  P uint16 makercode;
  P uint8 is96h;
  P uint8 unitcode;
  P uint8 devicecode;
  P uint8 unused[7];
  P uint8 version;
  P uint8 complement;
  P uint16 checksum;
} tGBAHeader;

#undef P

#define GBA_HEADER (*(tGBAHeader *)0x08000000)
#define GBA_BUS    ((vuint16 *)(0x08000000))
#define RUMBLE_PAK (*(vuint16 *)0x08000000)

/*! \fn bool isRumbleInserted(void);
	\brief Check for rumble option pak.
	
	Returns true if the cart in the GBA slot is a Rumble option pak.
 
*/
bool isRumbleInserted(void);
/*! \fn void setRumble(bool position);
	\param position Alternates position of the actuator in the pak
	\brief Fires the rumble actuator.

*/
void setRumble(bool position);

#ifdef __cplusplus
}
#endif

#endif
