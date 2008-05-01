/*---------------------------------------------------------------------------------
	$Id: microphone.c,v 1.6 2008-05-01 15:10:24 dovoto Exp $

	Microphone control for the ARM7

  Copyright (C) 2005
			Michael Noland (joat)
			Jason Rogers (dovoto)
			Dave Murphy (WinterMute)
			Chris Double (doublec)

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
#include <nds/arm7/audio.h>
#include <nds/timers.h>

//---------------------------------------------------------------------------------
// Turn on the Microphone Amp. Code based on neimod's example.
//---------------------------------------------------------------------------------
void micSetAmp(u8 control) {
//---------------------------------------------------------------------------------
	SerialWaitBusy();
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = PM_AMP_OFFSET;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz;
	REG_SPIDATA = control;
}

//---------------------------------------------------------------------------------
// Read a byte from the microphone. Code based on neimod's example.
//---------------------------------------------------------------------------------
u8 micReadData8() {
//---------------------------------------------------------------------------------
	static u16 result, result2;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_MICROPHONE | SPI_BAUD_2MHz | SPI_CONTINUOUS;
	REG_SPIDATA = 0xEC;  // Touchscreen command format for AUX

	SerialWaitBusy();

	REG_SPIDATA = 0x00;

	SerialWaitBusy();

	result = REG_SPIDATA;
  	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_TOUCH | SPI_BAUD_2MHz;
	REG_SPIDATA = 0x00;

	SerialWaitBusy();

	result2 = REG_SPIDATA;

	return (((result & 0x7F) << 1) | ((result2>>7)&1));
}

//---------------------------------------------------------------------------------
// Read a short from the microphone. Code based on neimod's example.
//---------------------------------------------------------------------------------
u16 micReadData12() {
//---------------------------------------------------------------------------------
	static u16 result, result2;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_MICROPHONE | SPI_BAUD_2MHz | SPI_CONTINUOUS;
	REG_SPIDATA = 0xE4;  // Touchscreen command format for AUX, 12bit

	SerialWaitBusy();

	REG_SPIDATA = 0x00;

	SerialWaitBusy();

	result = REG_SPIDATA;
  	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_TOUCH | SPI_BAUD_2MHz;
	REG_SPIDATA = 0x00;

	SerialWaitBusy();

	result2 = REG_SPIDATA;

	return (((result & 0x7F) << 5) | ((result2>>3)&0x1F));
}

static u8* microphone_buffer = 0;
static int microphone_buffer_length = 0;
static int sampleCount = 0;
static bool eightBit = true;
static int micTimer = 0;

//---------------------------------------------------------------------------------
void micStartRecording(u8* buffer, int length, int freq, int timer, bool eightBitSample) {
//---------------------------------------------------------------------------------
  microphone_buffer = buffer;
  microphone_buffer_length = length;
  sampleCount = 0;
  micTimer = timer;
  eightBit = eightBitSample;
  micOn();

  irqSet(BIT(3 + timer), micTimerHandler);

  // Setup a timer
  TIMER_DATA(timer) = TIMER_FREQ(freq); 
  TIMER_CR(timer) = TIMER_ENABLE | TIMER_DIV_1 | TIMER_IRQ_REQ;
}

//---------------------------------------------------------------------------------
int micStopRecording(void) {
//---------------------------------------------------------------------------------
  TIMER_CR(micTimer) &= ~TIMER_ENABLE;
  micOff();
  microphone_buffer = 0;
  return sampleCount;
}

//---------------------------------------------------------------------------------
void micTimerHandler(void) {
//---------------------------------------------------------------------------------
  if(microphone_buffer && microphone_buffer_length > 0) 
  {
    // Read data from the microphone. Data from the Mic is unsigned, flipping
    // the highest bit makes it signed.
    if(eightBit)
    {
       *microphone_buffer++ = micReadData8() ^ 0x80;
        --microphone_buffer_length;
    }
    else
    {
       *(u16*)microphone_buffer = (micReadData12() - 2048) << 4; // ^ 0x8000;
       microphone_buffer_length -= 2;
    }

    sampleCount++;
  }

}
