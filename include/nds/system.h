/*---------------------------------------------------------------------------------

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

---------------------------------------------------------------------------------*/

//!	NDS hardware definitions.
/*!	\file system.h

	These definitions are usually only touched during
	the initialization of the program.
*/

#ifndef NDS_SYSTEM_INCLUDE
#define NDS_SYSTEM_INCLUDE
#include "ndstypes.h"
//!	LCD status register.
#define	REG_DISPSTAT	(*(vu16*)0x04000004)

//! LCD Status register bitdefines
typedef enum 
{
	DISP_IN_VBLANK   =  BIT(0),//!	The display currently in a vertical blank.
	DISP_IN_HBLANK    = BIT(1),//!	The display currently in a horizontal blank.
	DISP_YTRIGGERED   = BIT(2),//!	Current scanline and %DISP_Y match.
	DISP_VBLANK_IRQ   = BIT(3),//!	Interrupt on vertical blank.
	DISP_HBLANK_IRQ   = BIT(4),//!	Interrupt on horizontal blank.
	DISP_YTRIGGER_IRQ = BIT(5) //!	Interrupt when current scanline and %DISP_Y match.
}DISP_BITS;

//!	Current display scanline.
#define	REG_VCOUNT		(*(vu16*)0x4000006)


//!	Halt control register.
/*!	Writing 0x40 to HALT_CR activates GBA mode.
	%HALT_CR can only be accessed via the BIOS.
*/
#define HALT_CR       (*(vu16*)0x04000300)

//!	Power control register.
/*!	This register controls what hardware should
	be turned on or off.
*/
#define	REG_POWERCNT	*(vu16*)0x4000304


static inline

void SetYtrigger(int Yvalue) {
	REG_DISPSTAT = (REG_DISPSTAT & 0x007F ) | (Yvalue << 8) | (( Yvalue & 0x100 ) >> 1) ;
}

#define PM_ARM9_DIRECT BIT(16)
/*! \enum PM_Bits
\brief Power Management control bits 
*/
typedef enum
{
	PM_SOUND_AMP		= BIT(0) ,   /*!< \brief Power the sound hardware (needed to hear stuff in GBA mode too) */
	PM_SOUND_MUTE		= BIT(1),    /*!< \brief   Mute the main speakers, headphone output will still work. */
	PM_BACKLIGHT_BOTTOM	= BIT(2),    /*!< \brief   Enable the top backlight if set */
	PM_BACKLIGHT_TOP	= BIT(3)  ,  /*!< \brief   Enable the bottom backlight if set */
	PM_SYSTEM_PWR		= BIT(6) ,   /*!< \brief  Turn the power *off* if set */

	POWER_LCD		= PM_ARM9_DIRECT | BIT(0),		//!<	Controls the power for both LCD screens.
	POWER_2D_A		= PM_ARM9_DIRECT | BIT(1),		//!<	Controls the power for the main 2D core.
	POWER_MATRIX	= PM_ARM9_DIRECT | BIT(2),		//!<	Controls the power for the 3D matrix.
	POWER_3D_CORE	= PM_ARM9_DIRECT | BIT(3),		//!<	Controls the power for the main 3D core.
	POWER_2D_B		= PM_ARM9_DIRECT | BIT(9),		//!<	Controls the power for the sub 2D core.
	POWER_SWAP_LCDS	= PM_ARM9_DIRECT | BIT(15),		//!<	Controls which screen should use the main core.
	POWER_ALL_2D	= PM_ARM9_DIRECT | POWER_LCD | POWER_2D_A | POWER_2D_B,	//!< power just 2D hardware
	POWER_ALL		= PM_ARM9_DIRECT | POWER_ALL_2D | POWER_3D_CORE | POWER_MATRIX //!< power everything

}PM_Bits;

void systemSleep(void);



//--------------------------------------------------------------
//    ARM9 section
//--------------------------------------------------------------
#ifdef ARM9

//!	Turns on specified hardware.
/*!	May be called from arm7 or arm9 (arm9 power bits will be ignored by arm7, arm7 power bits
	will be passed to the arm7 from the arm9).

	\param bits What to power on.
*/
void powerOn(int bits);

//!	Turns off specified hardware.
/*!	May be called from arm7 or arm9 (arm9 power bits will be ignored by arm7, arm7 power bits
	will be passed to the arm7 from the arm9).

	\param bits What to power on.
*/
void powerOff(int bits);

//!	Set the LED blink mode
/*!	Arm9 only
	\param bm What to power on.
*/
void ledBlink(int bm);

//internal fifo handlers
void systemMsgHandler(int bytes, void* user_data);
void powerValueHandler(u32 value, void* data);

//!	Switches the screens.
static inline void lcdSwap(void) { REG_POWERCNT ^= POWER_SWAP_LCDS; }

//!	Forces the main core to display on the top.
static inline void lcdMainOnTop(void) { REG_POWERCNT |= POWER_SWAP_LCDS; }

//!	Forces the main core to display on the bottom.
static inline void lcdMainOnBottom(void) { REG_POWERCNT &= ~POWER_SWAP_LCDS; }

//!	Set the arm9 vector base
/*!	Arm9 only
	\param highVector high vector
*/
void setVectorBase(int highVector);

typedef struct sysVectors_t {
	u32	reset;
	u32	undefined;
	u32	swi;
	u32	prefetch_abort;
	u32	data_abort;
	u32	fiq;
} sysVectors;

extern sysVectors SystemVectors;

#endif

//--------------------------------------------------------------
//    ARM7 section
//--------------------------------------------------------------
#ifdef ARM7

//!	Power-controlled hardware devices accessable to the ARM7.
/*!	Note that these should only be used when programming for
	the ARM7.  Trying to boot up these hardware devices via
	the ARM9 would lead to unexpected results.
*/
typedef enum 
{
	POWER_SOUND = BIT(0),	//!<	Controls the power for the sound controller.

	PM_CONTROL_REG   = 0, //!<	Selects the PM control register
	PM_BATTERY_REG   = 1, //!<	Selects the PM nattery register
	PM_AMPLIFIER_REG = 2, //!<	Selects the PM amplifier register
	PM_READ_REGISTER = (1<<7), //!<	Selects the PM read register
	PM_AMP_OFFSET  = 2,		//!<	Selects the PM amp register
	PM_GAIN_OFFSET  = 3, //!<	Selects the PM gain register
	PM_GAIN_20 = 0,	//!<	Sets the mic gain to 20db
	PM_GAIN_40 = 1,//!<	Sets the mic gain to 40db
	PM_GAIN_80 = 2,//!<	Sets the mic gain to 80db
	PM_GAIN_160 = 3,//!<	Sets the mic gain to 160db
	PM_AMP_ON     = 1, //!<	Turns the sound amp on
	PM_AMP_OFF    = 0 //!<	Turns the sound amp off
}ARM7_power;


//!< PM control register bits - LED control
#define PM_LED_CONTROL(m)  ((m)<<4)  // ?
// Warning: These functions use the SPI chain, and are thus 'critical'
// sections, make sure to disable interrupts during the call if you've
// got a VBlank IRQ polling the touch screen, etc...


//install the fifo power handler
void installSystemFIFO(void);

//cause the ds to enter low power mode
void systemSleep(void);
//internal can check if sleep mode is enabled
int sleepEnabled(void);

// Read/write a power management register
int writePowerManagement(int reg, int command);

static inline
int readPowerManagement(int reg) {
	return writePowerManagement((reg)|PM_READ_REGISTER, 0);
}

static inline
void powerOn(PM_Bits bits) {
	REG_POWERCNT |= bits;
}

static inline
void powerOff(PM_Bits bits) {
	REG_POWERCNT &= ~bits;
}

static inline
void systemShutDown() {
	powerOn(PM_SYSTEM_PWR);
}

void readUserSettings();

#endif /* ARM7 */

//!	User's DS settings.
/*!	\struct tPERSONAL_DATA

	Defines the structure the DS firmware uses for transfer
	of the user's settings to the booted program.
*/
typedef struct tPERSONAL_DATA {
  u8 RESERVED0[2];			//!<	??? (0x05 0x00).

  u8 theme;					//!<	The user's theme color (0-15).
  u8 birthMonth;			//!<	The user's birth month (1-12).
  u8 birthDay;				//!<	The user's birth day (1-31).

  u8 RESERVED1[1];			//!<	???

  s16 name[10];				//!<	The user's name in UTF-16 format.
  u16 nameLen;				//!<	The length of the user's name in characters.

  s16 message[26];			//!<	The user's message.
  u16 messageLen;			//!<	The length of the user's message in characters.

  u8 alarmHour;				//!<	What hour the alarm clock is set to (0-23).
  u8 alarmMinute;			//!<	What minute the alarm clock is set to (0-59).
            //0x027FFCD3  alarm minute

  u8 RESERVED2[4];			//!<	???
           //0x027FFCD4  ??

  u16 calX1;				//!<	Touchscreen calibration: first X touch
  u16 calY1;				//!<	Touchscreen calibration: first Y touch
  u8 calX1px;				//!<	Touchscreen calibration: first X touch pixel
  u8 calY1px;				//!<	Touchscreen calibration: first X touch pixel

  u16 calX2;				//!<	Touchscreen calibration: second X touch
  u16 calY2;				//!<	Touchscreen calibration: second Y touch
  u8 calX2px;				//!<	Touchscreen calibration: second X touch pixel
  u8 calY2px;				//!<	Touchscreen calibration: second Y touch pixel

  packed_struct {
    unsigned language			: 3;	//!<	User's language.
    unsigned gbaScreen			: 1;	//!<	GBA screen selection (lower screen if set, otherwise upper screen).
    unsigned defaultBrightness	: 2;	//!<	Brightness level at power on, dslite.
    unsigned autoMode			: 1;	//!<	The DS should boot from the DS cart or GBA cart automatically if one is inserted.
    unsigned RESERVED1			: 2;	//!<	???
	unsigned settingsLost		: 1;	//!<	User Settings Lost (0=Normal, 1=Prompt/Settings Lost)
	unsigned RESERVED2			: 6;	//!<	???
  } _user_data;
  
  u16	RESERVED3;
  u32	rtcOffset;
  u32	RESERVED4;
} PACKED PERSONAL_DATA ;

//!	Key input register.
/*!	On the ARM9, the hinge "button", the touch status, and the
	X and Y buttons cannot be accessed directly.
*/
#define	REG_KEYINPUT	(*(vuint16*)0x04000130)

//!	Key input control register.
#define	REG_KEYCNT		(*(vuint16*)0x04000132)

//!	Default location for the user's personal data (see %PERSONAL_DATA).
#define PersonalData ((PERSONAL_DATA*)0x27FFC80)

// argv struct magic number
#define ARGV_MAGIC 0x5f617267

//!	argv structure
/*!	\struct __argv

	structure used to set up argc/argv on the DS

*/
struct __argv {
	int argvMagic;		//!< argv magic number, set to 0x5f617267 ('_arg') if valid 
	char *commandLine;	//!< base address of command line, set of null terminated strings
	int length;			//!< total length of command line
	int argc;			//!< internal use, number of arguments
	char **argv;		//!< internal use, argv pointer
};

//!	Default location for the libnds argv structure.
#define __system_argv		((struct __argv *)0x027FFF70)


typedef	struct {
	u8 year;	// add 2000 to get 4 digit year
	u8 month;	// 1 to 12
	u8 day;		// 1 to (days in month)

	u8 weekday;	// day of week
	u8 hours;	// 0 to 11 for AM, 52 to 63 for PM
	u8 minutes;	// 0 to 59
	u8 seconds;	// 0 to 59
} RTCtime;

#endif
