/*---------------------------------------------------------------------------------

	memory.h -- Declaration of memory regions


	Copyright (C) 2005 Michael Noland (joat) and Jason Rogers (dovoto)

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
/*! \file memory.h
\brief Defines for many of the regions of memory on the DS as well as a few control functions for memory bus access
*/
#ifndef NDS_MEMORY_INCLUDE
#define NDS_MEMORY_INCLUDE


#include "ndstypes.h"


#ifdef ARM9
#define REG_EXMEMCNT (*(vu16*)0x04000204)
#else
#define REG_EXMEMSTAT (*(vu16*)0x04000204)
#endif

#define ARM7_MAIN_RAM_PRIORITY BIT(15)
#define ARM7_OWNS_CARD BIT(11)
#define ARM7_OWNS_ROM  BIT(7)


// Protection register (write-once sadly)
#ifdef ARM7
#define PROTECTION    (*(vu32*)0x04000308)
#endif


#define ALLRAM        ((u8*)0x00000000)

/*! \brief 8 bit pointer to main ram */
#define MAINRAM8      ((u8*)0x02000000)
/*! \brief 16 bit pointer to main ram */
#define MAINRAM16     ((u16*)0x02000000)
/*! \brief 32 bit pointer to main ram */
#define MAINRAM32     ((u32*)0x02000000)

// fixme: shared RAM

// GBA_BUS is volatile, while GBAROM is not
/*! \brief 16 bit pointer to the GBA slot bus */
#define GBA_BUS       ((vu16 *)(0x08000000))
#define GBAROM        ((u16*)0x08000000)

/*! \brief 8 bit pointer to GBA slot Save ram */
#define SRAM          ((u8*)0x0A000000)




#ifdef ARM7
#define VRAM          ((u16*)0x06000000)
#endif

/*!
* \brief the GBA file header format
*/
typedef struct sGBAHeader {
	u32 entryPoint;
	u8 logo[156];
	char title[0xC];
	char gamecode[0x4];
	u16 makercode;
	u8 is96h;
	u8 unitcode;
	u8 devicecode;
	u8 unused[7];
	u8 version;
	u8 complement;
	u16 checksum;
} __attribute__ ((__packed__)) tGBAHeader;

#define GBA_HEADER (*(tGBAHeader *)0x08000000)

/*!
* \brief the NDS file header format
*/
typedef struct sNDSHeader {
  char gameTitle[12];
  char gameCode[4];
  char makercode[2];
  u8 unitCode;
  u8 deviceType;           // type of device in the game card
  u8 deviceSize;           // device capacity (1<<n Mbit)
  u8 reserved1[9];
  u8 romversion;
  u8 flags;                // auto-boot flag

  u32 arm9romOffset;
  u32 arm9executeAddress;
  u32 arm9destination;
  u32 arm9binarySize;

  u32 arm7romOffset;
  u32 arm7executeAddress;
  u32 arm7destination;
  u32 arm7binarySize;

  u32 filenameOffset;
  u32 filenameSize;
  u32 fatOffset;
  u32 fatSize;

  u32 arm9overlaySource;
  u32 arm9overlaySize;
  u32 arm7overlaySource;
  u32 arm7overlaySize;

  u32 cardControl13;  // used in modes 1 and 3
  u32 cardControlBF;  // used in mode 2
  u32 bannerOffset;

  u16 secureCRC16;

  u16 readTimeout;

  u32 unknownRAM1;
  u32 unknownRAM2;

  u32 bfPrime1;
  u32 bfPrime2;
  u32 romSize;

  u32 headerSize;
  u32 zeros88[14];
  u8 gbaLogo[156];
  u16 logoCRC16;
  u16 headerCRC16;

  u32 debugRomSource;
  u32 debugRomSize;
  u32 debugRomDestination;
  u32 offset_0x16C;

  u8 zero[0x90];
} __attribute__ ((__packed__)) tNDSHeader;

#define __NDSHeader ((tNDSHeader *)0x027FFE00)

/*!
* \brief the NDS banner format
*/
typedef struct sNDSBanner {
  u16 version;
  u16 crc;
  u8 reserved[28];
  u8 icon[512];
  u16 palette[16];
  u16 titles[6][128];
} __attribute__ ((__packed__)) tNDSBanner;


#ifdef __cplusplus
extern "C" {
#endif

#ifdef ARM9
#define BUS_OWNER_ARM9 true
#define BUS_OWNER_ARM7 false


static inline 
/*! \fn void sysSetCartOwner(bool arm9)
    \brief Sets the owner of the GBA cart.  Both CPUs cannot have access to the gba cart (slot 2).
    \param arm9 if true the arm9 is the owner, otherwise the arm7
*/
void sysSetCartOwner(bool arm9) {
  REG_EXMEMCNT = (REG_EXMEMCNT & ~ARM7_OWNS_ROM) | (arm9 ? 0 :  ARM7_OWNS_ROM);
}

static inline 
/*! \fn void sysSetCardOwner(bool arm9)
    \brief Sets the owner of the DS card bus.  Both CPUs cannot have access to the DS card bus (slot 1).
*   \param arm9 if true the arm9 is the owner, otherwise the arm7
*/
void sysSetCardOwner(bool arm9) {
  REG_EXMEMCNT = (REG_EXMEMCNT & ~ARM7_OWNS_CARD) | (arm9 ? 0 : ARM7_OWNS_CARD);
}

static inline 
/*! \fn void sysSetBusOwners(bool arm9rom, bool arm9card)
    \brief Sets the owner of the DS card bus (slot 1) and gba cart bus (slot 2).  Only one cpu may access the device at a time.
*   \param arm9rom if true the arm9 is the owner of slot 2, otherwise the arm7
*   \param arm9card if true the arm9 is the owner of slot 1, otherwise the arm7
*/
void sysSetBusOwners(bool arm9rom, bool arm9card) {
  u16 pattern = REG_EXMEMCNT & ~(ARM7_OWNS_CARD|ARM7_OWNS_ROM);
  pattern = pattern | (arm9card ?  0: ARM7_OWNS_CARD ) |
                      (arm9rom ?  0: ARM7_OWNS_ROM );
  REG_EXMEMCNT = pattern;
}

#endif


#ifdef __cplusplus
}
#endif


#endif

