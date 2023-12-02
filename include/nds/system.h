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

/*!	\file system.h
	\brief NDS hardware definitions.
	These definitions are usually only touched during
	the initialization of the program.
*/

#ifndef NDS_SYSTEM_INCLUDE
#define NDS_SYSTEM_INCLUDE

#include "ndstypes.h"
#include <calico/nds/system.h>
#include <calico/nds/scfg.h>
#include <calico/nds/lcd.h>
#include <calico/nds/pm.h>
#include <calico/nds/mm_env.h>

#define REG_POWERCNT REG_POWCNT

#define isDSiMode       systemIsTwlMode
#define systemSleep     pmEnterSleep
#define getBatteryLevel pmGetBatteryState

//! LCD Status register bitdefines
typedef enum
{
	DISP_IN_VBLANK    = DISPSTAT_IF_VBLANK, //!<	The display currently in a vertical blank.
	DISP_IN_HBLANK    = DISPSTAT_IF_HBLANK, //!<	The display currently in a horizontal blank.
	DISP_YTRIGGERED   = DISPSTAT_IF_VCOUNT, //!<	Current scanline and %DISP_Y match.
	DISP_VBLANK_IRQ   = DISPSTAT_IE_VBLANK, //!<	Interrupt on vertical blank.
	DISP_HBLANK_IRQ   = DISPSTAT_IE_HBLANK, //!<	Interrupt on horizontal blank.
	DISP_YTRIGGER_IRQ = DISPSTAT_IE_VCOUNT  //!<	Interrupt when current scanline and %DISP_Y match.
}DISP_BITS;

//!	Halt control register.
/*!	Writing 0x40 to HALT_CR activates GBA mode.
	%HALT_CR can only be accessed via the BIOS.
*/
#define HALT_CR       (*(vu16*)0x04000300)

static inline
/*!
	\brief sets the Y trigger(?)

	\param Yvalue the value for the Y trigger.
*/
void SetYtrigger(int Yvalue) {
	REG_DISPSTAT = (REG_DISPSTAT & ~DISPSTAT_LYC_MASK ) | DISPSTAT_LYC(Yvalue);
}

#define PM_ARM9_DIRECT BIT(16)
//! Power Management control bits
typedef enum
{
	PM_SOUND_AMP		= BIT(0),		//!< Power the sound hardware (needed to hear stuff in GBA mode too).
	PM_SOUND_MUTE		= BIT(1),		//!< Mute the main speakers, headphone output will still work.
	PM_BACKLIGHT_BOTTOM	= BIT(2),		//!< Enable the bottom backlight if set.
	PM_BACKLIGHT_TOP	= BIT(3),		//!< Enable the top backlight if set.
	PM_SYSTEM_PWR		= BIT(6),		//!< Turn the power *off* if set.

#ifdef ARM9
	POWER_LCD		= PM_ARM9_DIRECT | POWCNT_LCD,			//!<	Controls the power for both LCD screens.
	POWER_2D_A		= PM_ARM9_DIRECT | POWCNT_2D_GFX_A,		//!<	Controls the power for the main 2D core.
	POWER_MATRIX	= PM_ARM9_DIRECT | POWCNT_3D_RENDER,	//!<	Controls the power for the 3D matrix.
	POWER_3D_CORE	= PM_ARM9_DIRECT | POWCNT_3D_GEOMETRY,	//!<	Controls the power for the main 3D core.
	POWER_2D_B		= PM_ARM9_DIRECT | POWCNT_2D_GFX_B,		//!<	Controls the power for the sub 2D core.
	POWER_SWAP_LCDS	= PM_ARM9_DIRECT | POWCNT_LCD_SWAP,		//!<	Controls which screen should use the main core.
	POWER_ALL_2D	= PM_ARM9_DIRECT | POWER_LCD | POWER_2D_A | POWER_2D_B,			//!< power just 2D hardware.
	POWER_ALL		= PM_ARM9_DIRECT | POWER_ALL_2D | POWER_3D_CORE | POWER_MATRIX	//!< power everything.
#endif
}PM_Bits;

static inline
/*!	Set the LED blink mode
	\param bm What to power on.
*/
void ledBlink(int bm) {
	pmSetPowerLed((PmLedMode)(bm&3));
}

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

//!	Switches the screens.
static inline void lcdSwap(void) { pmGfxLcdSwap(); }

//!	Forces the main core to display on the top.
static inline void lcdMainOnTop(void) { pmGfxSetLcdLayout(PmLcdLayout_TopIsA); }

//!	Forces the main core to display on the bottom.
static inline void lcdMainOnBottom(void) { pmGfxSetLcdLayout(PmLcdLayout_TopIsB); }

//! Powers down the DS
static inline
void systemShutDown(void) {
	powerOn(PM_SYSTEM_PWR);
}

static inline
void readFirmware(u32 address, void *buffer, u32 length)
{
	pmReadNvram(buffer, address, length);
}

int writeFirmware(u32 address, const void *buffer, u32 length);

/*! \brief A struct with all the CPU exception vectors.
	Each member contains a pointer to a function that will be executed
	when a corresponding exception occurs.

	See gbatek for more information.
*/
typedef struct sysVectors_t {
	VoidFn	reset;			//!< CPU reset.
	VoidFn	undefined;		//!< undefined instruction.
	VoidFn	swi;			//!< software interrupt.
	VoidFn	prefetch_abort;	//!< prefetch abort.
	VoidFn	data_abort;		//!< data abort.
	VoidFn	irq;			//!< interrupt.
} sysVectors;

#define SystemVectors __excpt_vectors

extern sysVectors __excpt_vectors;

/*!
	\brief Sets the ARM9 clock speed, only possible in DSi mode
	\param speed CPU speed (false = 67.03MHz, true = 134.06MHz)
	\return The old CPU speed value
*/
bool setCpuClock(bool speed);

// Helper functions for heap size
//! returns current start of heap space
u8* getHeapStart();
//! returns current end of heap space
u8* getHeapEnd();
//! returns current heap limit
u8* getHeapLimit();

#endif //ARM9

//--------------------------------------------------------------
//    ARM7 section
//--------------------------------------------------------------
#ifdef ARM7
#include <calico/nds/arm7/pmic.h>

#define REG_CONSOLEID	REG_OTP_CID

//!	Power-controlled hardware devices accessable to the ARM7.
/*!	Note that these should only be used when programming for
	the ARM7.  Trying to boot up these hardware devices via
	the ARM9 would lead to unexpected results.
	ARM7 only.
*/
typedef enum {
	POWER_SOUND = POWCNT_SOUND,			//!<	Controls the power for the sound controller.

	PM_CONTROL_REG		= PmicReg_Control,			//!<	Selects the PM control register
	PM_BATTERY_REG		= PmicReg_BatteryStatus,	//!<	Selects the PM battery register
	PM_AMPLIFIER_REG	= PmicReg_MicAmpControl,	//!<	Selects the PM amplifier register
	PM_AMP_OFFSET		= PmicReg_MicAmpControl,	//!<	Selects the PM amp register
	PM_GAIN_OFFSET		= PmicReg_MicAmpGain,		//!<	Selects the PM gain register
	PM_BACKLIGHT_LEVEL	= PmicReg_BacklightLevel,	//!<	Selects the DS Lite backlight register
	PM_GAIN_20			= PmicMicGain_20,			//!<	Sets the mic gain to 20db
	PM_GAIN_40			= PmicMicGain_40,			//!<	Sets the mic gain to 40db
	PM_GAIN_80			= PmicMicGain_80,			//!<	Sets the mic gain to 80db
	PM_GAIN_160			= PmicMicGain_160,			//!<	Sets the mic gain to 160db
	PM_AMP_ON			= 1,		//!<	Turns the sound amp on
	PM_AMP_OFF			= 0			//!<	Turns the sound amp off
} ARM7_power;

//!< PM control register bits - LED control
#define PM_LED_CONTROL(m)  ((m)<<4)

// Read/write a power management register
static inline
void writePowerManagement(int reg, int command) {
	spiLock();
	pmicWriteRegister((PmicRegister)reg, command);
	spiUnlock();
}

static inline
int readPowerManagement(int reg) {
	spiLock();
	int ret = pmicReadRegister((PmicRegister)reg);
	spiUnlock();
	return ret;
}

static inline
void powerOn(int bits) {
	pmPowerOn(bits);
}

static inline
void powerOff(PM_Bits bits) {
	pmPowerOff(bits);
}

#endif /* ARM7 */


/*!	\brief Backlight level settings.
	Note, these are only available on DS Lite.
*/
typedef enum {
	BACKLIGHT_LOW,	//!< low backlight setting.
	BACKLIGHT_MED,	//!< medium backlight setting.
	BACKLIGHT_HIGH,	//!< high backlight setting.
	BACKLIGHT_MAX	//!< max backlight setting.
} BACKLIGHT_LEVELS;


// Common functions

/*!
	\brief User's DS settings.
	Defines the structure the DS firmware uses for transfer
	of the user's settings to the booted program.

	Theme/Color values:
	- 0 = Gray
	- 1 = Brown
	- 2 = Red
	- 3 = Pink
	- 4 = Orange
	- 5 = Yellow
	- 6 = Yellow/Green-ish
	- 7 = Green
	- 8 = Dark Green
	- 9 = Green/Blue-ish
	- 10 = Light Blue
	- 11 = Blue
	- 12 = Dark Blue
	- 13 = Dark Purple
	- 14 = Purple
	- 15 = Purple/Red-ish

	Language values:
	- 0 = Japanese
	- 1 = English
	- 2 = French
	- 3 = German
	- 4 = Italian
	- 5 = Spanish
	- 6 = Chinese(?)
	- 7 = Unknown/Reserved
*/
typedef struct tPERSONAL_DATA
{
	u8 RESERVED0[2];			//	??? (0x05 0x00). (version according to gbatek)

	u8 theme;					//!<	The user's theme color (0-15).
	u8 birthMonth;				//!<	The user's birth month (1-12).
	u8 birthDay;				//!<	The user's birth day (1-31).

	u8 RESERVED1[1];			//	???

	s16 name[10];				//!<	The user's name in UTF-16 format.
	u16 nameLen;				//!<	The length of the user's name in characters.

	s16 message[26];			//!<	The user's message.
	u16 messageLen;				//!<	The length of the user's message in characters.

	u8 alarmHour;				//!<	What hour the alarm clock is set to (0-23).
	u8 alarmMinute;				//!<	What minute the alarm clock is set to (0-59).
	//0x02FFFCD3  alarm minute

	u8 RESERVED2[4];			//	??? 0x02FFFCD4  ??


	u16 calX1;				//!<	Touchscreen calibration: first X touch
	u16 calY1;				//!<	Touchscreen calibration: first Y touch
	u8 calX1px;				//!<	Touchscreen calibration: first X touch pixel
	u8 calY1px;				//!<	Touchscreen calibration: first X touch pixel

	u16 calX2;				//!<	Touchscreen calibration: second X touch
	u16 calY2;				//!<	Touchscreen calibration: second Y touch
	u8 calX2px;				//!<	Touchscreen calibration: second X touch pixel
	u8 calY2px;				//!<	Touchscreen calibration: second Y touch pixel

	struct {
		unsigned int language			: 3;	//!<	User's language.
		unsigned int gbaScreen			: 1;	//!<	GBA screen selection (lower screen if set, otherwise upper screen).
		unsigned int defaultBrightness	: 2;	//!<	Brightness level at power on, dslite.
		unsigned int autoMode			: 1;	//!<	The DS should boot from the DS cart or GBA cart automatically if one is inserted.
		unsigned int RESERVED5			: 2;	//	???
		unsigned int settingsLost		: 1;	//!<	User Settings Lost (0=Normal, 1=Prompt/Settings Lost)
		unsigned int RESERVED6			: 6;	//	???
	} PACKED;

	u16	RESERVED3;			//	???
	u32	rtcOffset;			//!<	Real Time Clock offset.
	u32	RESERVED4;			//	???
} PACKED PERSONAL_DATA ;

//!	Default location for the user's personal data (see %PERSONAL_DATA).
#define PersonalData ((PERSONAL_DATA*)MM_ENV_USER_SETTINGS)

#endif
