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
