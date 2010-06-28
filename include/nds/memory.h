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

//! 8 bit pointer to the start of all the ram.
#define ALLRAM        ((u8*)0x00000000)

//! 8 bit pointer to main ram.
#define MAINRAM8      ((u8*)0x02000000)
//! 16 bit pointer to main ram.
#define MAINRAM16     ((u16*)0x02000000)
//! 32 bit pointer to main ram.
#define MAINRAM32     ((u32*)0x02000000)


// TODO: fixme: shared RAM

// GBA_BUS is volatile, while GBAROM is not
//! 16 bit volatile pointer to the GBA slot bus.
#define GBA_BUS       ((vu16 *)(0x08000000))
//! 16 bit pointer to the GBA slot ROM.
#define GBAROM        ((u16*)0x08000000)

//! 8 bit pointer to GBA slot Save ram.
#define SRAM          ((u8*)0x0A000000)


#ifdef ARM7
#define VRAM          ((u16*)0x06000000)
#endif



/*!
	\brief the GBA file header format.
	See gbatek for more info.
*/
typedef struct sGBAHeader {
	u32 entryPoint;		//!< 32 bits arm opcode to jump to executable code.
	u8 logo[156];		//!< nintendo logo needed for booting the game.
	char title[0xC];	//!< 12 characters for the game title.
	char gamecode[0x4];	//!< 4 characters for the game code. first character is usally A or B, next 2 characters is a short title
						//!< and last character is for destination/language.
	u16 makercode;		//!< identifies the (commercial) developer.
	u8 is96h;			//!< fixed value that is always 96h.
	u8 unitcode;		//!< identifies the required hardware.
	u8 devicecode;		//!< used by nintedo's hardware debuggers. normally 0.
	u8 unused[7];
	u8 version;			//!< the version of the game.
	u8 complement;		//!< complement checksum of the gba header.
	u16 checksum;		//!< a 16 bit checksum? (gbatek says its unused/reserved).
} __attribute__ ((__packed__)) tGBAHeader;

#define GBA_HEADER (*(tGBAHeader *)0x08000000)

/*!
	\brief the NDS file header format
	See gbatek for more info.
*/
typedef struct sNDSHeader {
	char gameTitle[12];			//!< 12 characters for the game title.
	char gameCode[4];			//!< 4 characters for the game code.
	char makercode[2];			//!< identifies the (commercial) developer.
	u8 unitCode;				//!< identifies the required hardware.
	u8 deviceType;				//!< type of device in the game card
	u8 deviceSize;				//!< capacity of the device (1 << n Mbit)
	u8 reserved1[9];
	u8 romversion;				//!< version of the ROM.
	u8 flags;					//!< bit 2: auto-boot flag.

	u32 arm9romOffset;			//!< offset of the arm9 binary in the nds file.
	u32 arm9executeAddress;		//!< adress that should be executed after the binary has been copied.
	u32 arm9destination;		//!< destination address to where the arm9 binary should be copied.
	u32 arm9binarySize;			//!< size of the arm9 binary.

	u32 arm7romOffset;			//!< offset of the arm7 binary in the nds file.
	u32 arm7executeAddress;		//!< adress that should be executed after the binary has been copied.
	u32 arm7destination;		//!< destination address to where the arm7 binary should be copied.
	u32 arm7binarySize;			//!< size of the arm7 binary.

	u32 filenameOffset;			//!< File Name Table (FNT) offset.
	u32 filenameSize;			//!< File Name Table (FNT) size.
	u32 fatOffset;				//!< File Allocation Table (FAT) offset.
	u32 fatSize;				//!< File Allocation Table (FAT) size.

	u32 arm9overlaySource;		//!< File arm9 overlay offset.
	u32 arm9overlaySize;		//!< File arm9 overlay size.
	u32 arm7overlaySource;		//!< File arm7 overlay offset.
	u32 arm7overlaySize;		//!< File arm7 overlay size.

	u32 cardControl13;			//!< Port 40001A4h setting for normal commands (used in modes 1 and 3)
	u32 cardControlBF;			//!< Port 40001A4h setting for KEY1 commands (used in mode 2)
	u32 bannerOffset;			//!< offset to the banner with icon and titles etc.

	u16 secureCRC16;			//!< Secure Area Checksum, CRC-16.

	u16 readTimeout;			//!< Secure Area Loading Timeout.

	u32 unknownRAM1;			//!< ARM9 Auto Load List RAM Address (?)
	u32 unknownRAM2;			//!< ARM7 Auto Load List RAM Address (?)

	u32 bfPrime1;				//!< Secure Area Disable part 1.
	u32 bfPrime2;				//!< Secure Area Disable part 2.
	u32 romSize;				//!< total size of the ROM.

	u32 headerSize;				//!< ROM header size.
	u32 zeros88[14];
	u8 gbaLogo[156];			//!< Nintendo logo needed for booting the game.
	u16 logoCRC16;				//!< Nintendo Logo Checksum, CRC-16.
	u16 headerCRC16;			//!< header checksum, CRC-16.

	u32 debugRomSource;			//!< debug ROM offset.
	u32 debugRomSize;			//!< debug size.
	u32 debugRomDestination;	//!< debug RAM destination.
	u32 offset_0x16C;			//reserved?

	u8 zero[0x90];
} __attribute__ ((__packed__)) tNDSHeader;

#define __NDSHeader ((tNDSHeader *)0x02FFFE00)


/*!
	\brief the NDS banner format.
	See gbatek for more information.
*/
typedef struct sNDSBanner {
  u16 version;			//!< version of the banner.
  u16 crc;				//!< 16 bit crc/checksum of the banner.
  u8 reserved[28];
  u8 icon[512];			//!< 32*32 icon of the game with 4 bit per pixel.
  u16 palette[16];		//!< the pallete of the icon.
  u16 titles[6][128];	//!< title of the game in 6 different languages.
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
  REG_EXMEMCNT = (REG_EXMEMCNT & ~(ARM7_OWNS_CARD|ARM7_OWNS_ROM)) |
								(arm9card ?  0: ARM7_OWNS_CARD) |
                      			(arm9rom ?  0: ARM7_OWNS_ROM );
}
#endif //ARM9

#ifdef __cplusplus
}
#endif


#endif
