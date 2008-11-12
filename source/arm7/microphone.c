/*---------------------------------------------------------------------------------
	$Id: microphone.c,v 1.8 2008-11-12 17:47:12 dovoto Exp $

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
#include <nds/interrupts.h>
#include <nds/fifocommon.h>
#include <nds/timers.h>
#include <nds/arm7/audio.h>


//---------------------------------------------------------------------------------
// Turn on the Microphone Amp. Code based on neimod's example.
//---------------------------------------------------------------------------------
void micSetAmp(u8 control, u8 gain) {
//---------------------------------------------------------------------------------
	SerialWaitBusy();
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = PM_AMP_OFFSET;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz;
	REG_SPIDATA = control;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = PM_GAIN_OFFSET;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz;
	REG_SPIDATA = gain;
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

static u8* microphone_front_buffer;
static u8* microphone_back_buffer;
static int microphone_buffer_length = 0;
static int sampleCount = 0;
static bool eightBit = true;
static int micTimer = 0;
static MIC_BUF_SWAP_CB swapCallback;

//---------------------------------------------------------------------------------
void micStartRecording(u8* buffer, int length, int freq, int timer, bool eightBitSample, MIC_BUF_SWAP_CB bufferSwapCallback ) {
//---------------------------------------------------------------------------------
  microphone_front_buffer = buffer + length / 2;
  microphone_back_buffer = buffer;
  microphone_buffer_length = length / 2;
  swapCallback = bufferSwapCallback;
  sampleCount = 0;
  micTimer = timer;
  eightBit = eightBitSample;
  micOn();

  irqSet(BIT(3 + timer), micTimerHandler);
  irqEnable(BIT(3 + timer));

  // Setup a timer
  TIMER_DATA(timer) = TIMER_FREQ(freq); 
  TIMER_CR(timer) = TIMER_ENABLE |TIMER_IRQ_REQ;
  //irqSet(IRQ_TIMER2, micTimerHandler);
  //irqEnable(IRQ_TIMER2);

  //// Setup a timer
  //TIMER_DATA(2) = TIMER_FREQ(freq); 
  //TIMER_CR(2) = TIMER_ENABLE | TIMER_DIV_1 | TIMER_IRQ_REQ;
}

//---------------------------------------------------------------------------------
int micStopRecording(void) {
//---------------------------------------------------------------------------------
  TIMER_CR(micTimer) &= ~TIMER_ENABLE;
  micOff();

  if(swapCallback)
	  swapCallback(microphone_back_buffer, eightBit ? sampleCount : (sampleCount << 1));

  microphone_front_buffer = microphone_back_buffer = 0;

  return sampleCount;
}

//---------------------------------------------------------------------------------
void micTimerHandler(void) {
//---------------------------------------------------------------------------------
	int len = 0;
	// Read data from the microphone. Data from the Mic is unsigned, flipping
    // the highest bit makes it signed.

	

    if(eightBit)
    {
    	*(microphone_back_buffer+sampleCount) = micReadData8() ^ 0x80;
    }
    else
    {
	   *(u16*)(microphone_back_buffer + sampleCount * 2) = (micReadData12() - 2048) << 4; // ^ 0x8000;
	}
  

    sampleCount++;

	len = eightBit ? sampleCount : (sampleCount << 1);
    
	if(len >= microphone_buffer_length)
	{
		sampleCount = 0;
		
		u8* temp = microphone_back_buffer;
		microphone_back_buffer = microphone_front_buffer;
		microphone_front_buffer = temp;

		if(swapCallback)
			swapCallback(microphone_front_buffer, microphone_buffer_length);
	}
	
}
