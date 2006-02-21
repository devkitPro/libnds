/*---------------------------------------------------------------------------------
	$Id: system.h,v 1.10 2006-02-21 00:06:41 wntrmute Exp $

	Power control, keys, and HV clock registers

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

	$Log: not supported by cvs2svn $
	Revision 1.9  2005/11/27 12:30:24  wntrmute
	reverted to correct hardware REGisters
	
	Revision 1.8  2005/11/27 07:55:14  joatski
	Fixed my mistake in the changelogs
	
	Revision 1.7  2005/11/27 07:49:19  joatski
	Renamed REG_KEYINPUT and REG_KEYCNT back to KEYS and KEYS_CR, as the alternatives are defined in registers_alt.h.
	
	Revision 1.6  2005/10/11 03:08:58  dovoto
	added lcdMainOnTop() and llcdMainOnBottom()
	
	Revision 1.5  2005/09/07 18:03:36  wntrmute
	renamed key input registers
	moved key bit definitions to input.h
	
	Revision 1.4  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.3  2005/08/03 05:24:42  wntrmute
	adjusted header for logging
	tidied formatting
	powerON sets not ORs
	powerOFF inverts paramter
	use BIT macro


---------------------------------------------------------------------------------*/

#ifndef NDS_SYSTEM_INCLUDE
#define NDS_SYSTEM_INCLUDE


#define DISP_SR        (*(vuint16*)0x04000004)

#define DISP_IN_VBLANK     BIT(0)
#define DISP_IN_HBLANK     BIT(1)
#define DISP_YTRIGGERED    BIT(2)
#define DISP_VBLANK_IRQ    BIT(3)
#define DISP_HBLANK_IRQ    BIT(4)
#define DISP_YTRIGGER_IRQ  BIT(5)

#define DISP_Y      (*(vuint16*)0x04000006)


// Halt control register
#define HALT_CR       (*(vuint16*)0x04000300)
// Writing 0x40 to HALT_CR activates GBA mode, single bit it seems

// arm7, bit 0 = sound power off/on
#define POWER_CR       (*(vuint16*)0x04000304)

static inline void powerON(int on) { POWER_CR |= on;}
static inline void powerSET(int on) { POWER_CR = on;}
static inline void powerOFF(int off) { POWER_CR &= ~off;}

#ifdef ARM9
// Power control register (arm9)
#define POWER_LCD			BIT(0)
#define POWER_2D_A			BIT(1)
#define POWER_MATRIX		BIT(2)
#define POWER_3D_CORE		BIT(3)
#define POWER_2D_B			BIT(9)
#define POWER_SWAP_LCDS		BIT(15)

#define POWER_ALL_2D     (POWER_LCD |POWER_2D_A |POWER_2D_B)
#define POWER_ALL		 (POWER_ALL_2D | POWER_3D_CORE | POWER_MATRIX)

static inline void lcdSwap(void) { POWER_CR ^= POWER_SWAP_LCDS; }
static inline void lcdMainOnTop(void) { POWER_CR |= POWER_SWAP_LCDS; }
static inline void lcdMainOnBottom(void) { POWER_CR &= ~POWER_SWAP_LCDS; }
#endif

#ifdef ARM7
#define POWER_SOUND       BIT(0)
#define POWER_UNKNOWN     BIT(1)
#endif

typedef struct tPERSONAL_DATA {
  u8  RESERVED0[2];           //0x023FFC80  05 00 ?

  u8  theme;                  //0x027FFC82  favorite color (0-15)
  u8  birthMonth;             //0x027FFC83  birthday month (1-12)
  u8  birthDay;               //0x027FFC84  birthday day (1-31)

  u8  RESERVED1[1];           //0x027FFC85  ???

  s16 name[10];               //0x027FFC86  name, UTF-16?
  u16 nameLen;                //0x027FFC9A  length of name in characters

  s16 message[26];            //0x027FFC9C  message, UTF-16?
  u16 messageLen;             //0x027FFCD0  length of message in characters

  u8  alarmHour;              //0x027FFCD2  alarm hour
  u8  alarmMinute;            //0x027FFCD3  alarm minute

  u8  RESERVED2[4];           //0x027FFCD4  ??

  //calibration information
  u16 calX1;                  //0x027FFCD8
  u16 calY1;                  //0x027FFCDA
  u8  calX1px;                //0x027FFCDC
  u8  calY1px;                //0x027FFCDD

  u16 calX2;                  //0x027FFCDE
  u16 calY2;                  //0x027FFCE0
  u8  calX2px;                //0x027FFCE2
  u8  calY2px;                //0x027FFCE3

  packed_struct {             //0x027FFCE4
    unsigned language    : 3; //            language
    unsigned gbaScreen   : 1; //            GBA mode screen selection. 0=upper, 1=lower
    unsigned RESERVED3   : 2; //            ??
    unsigned autoMode    : 1; //            auto/manual mode. 0=manual, 1=auto
    unsigned RESERVED4   : 1; //            ??
  };
} PACKED PERSONAL_DATA ;


#define	REG_KEYINPUT	(*(vuint16*)0x04000130)
#define	REG_KEYCNT		(*(vuint16*)0x04000132)

#define PersonalData ((PERSONAL_DATA*)0x27FFC80)

#endif
