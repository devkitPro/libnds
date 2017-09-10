/*---------------------------------------------------------------------------------

	DSi microphone control

  Copyright (C) 2017
			fincs

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
#include <nds/interrupts.h>
#include <nds/fifocommon.h>
#include <nds/timers.h>
#include <nds/arm7/audio.h>
#include <nds/arm7/codec.h>

//---------------------------------------------------------------------------------
void micSetAmp_TWL(u8 control, u8 gain) {
//---------------------------------------------------------------------------------

	static const u8 gaintbl[] = { 0x1F, 0x2B, 0x37, 0x43 };

	if (control == PM_AMP_ON) {
		cdcWriteReg(CDC_SOUND, 0x2E, 0x03); // set adc bias
		bool adcOn = cdcReadReg(CDC_CONTROL, 0x51) & 0x80;
		bool dacOn = cdcReadReg(CDC_CONTROL, 0x3F) & 0xC0;
		cdcWriteReg(CDC_CONTROL, 0x51, 0x80); // turn on adc
		if (!adcOn || !dacOn) {
			swiDelay(0x28E91F); // 20ms
		}
		cdcWriteReg(CDC_CONTROL, 0x52, 0x00); // unmute adc
		cdcWriteReg(CDC_SOUND, 0x2F, gaintbl[gain&3]); // set gain
	} else if (control == PM_AMP_OFF) {
		cdcWriteReg(CDC_CONTROL, 0x52, 0x80); // mute adc
		cdcWriteReg(CDC_CONTROL, 0x51, 0x00); // turn off adc
		cdcWriteReg(CDC_SOUND, 0x2E, 0x00); // set adc bias
	}
}

//---------------------------------------------------------------------------------
u16 micReadData16_TWL() {
//---------------------------------------------------------------------------------

	u16 data = 0x8000;
	int timeout = 0;

	REG_MICCNT &= ~MICCNT_ENABLE;
	REG_MICCNT &= ~( (3<<13) | 0xF );
	REG_MICCNT |= MICCNT_CLEAR_FIFO | MICCNT_FORMAT(2);
	REG_MICCNT |= MICCNT_ENABLE;

	while (timeout++ < 200) {
		if (!(REG_MICCNT & MICCNT_EMPTY)) {
			data = REG_MICDATA;
			break;
		}
	}

	return data ^ 0x8000; // convert to unsigned
}
