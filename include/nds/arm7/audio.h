/*---------------------------------------------------------------------------------

	ARM7 audio control

	Copyright (C) 2005
		Michael Noland (joat)
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

#ifndef AUDIO_ARM7_INCLUDE
#define AUDIO_ARM7_INCLUDE

//---------------------------------------------------------------------------------
// Sound (ARM7 only)
//---------------------------------------------------------------------------------
#ifndef ARM7
#error Audio is only available on the ARM7
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <nds/arm7/serial.h>
#include <nds/system.h>

#define SOUND_VOL(n)	(n)
#define SOUND_FREQ(n)	((-0x1000000 / (n)))
#define SOUND_ENABLE	BIT(15)

#define SOUND_REPEAT    BIT(27)
#define SOUND_ONE_SHOT  BIT(28)

#define SOUND_FORMAT_16BIT (1<<29)
#define SOUND_FORMAT_8BIT	(0<<29)
#define SOUND_FORMAT_PSG    (3<<29)
#define SOUND_FORMAT_ADPCM  (2<<29)

#define SOUND_PAN(n)	((n) << 16)

#define SCHANNEL_ENABLE BIT(31)

//---------------------------------------------------------------------------------
// registers
//---------------------------------------------------------------------------------

#define REG_MASTER_VOLUME	(*(vu8*)0x4000500)
#define REG_SOUNDCNT		(*(vu16*)0x4000500)
#define REG_SOUNDBIAS		(*(vu32*)0x4000504)


#define SCHANNEL_CR(n)				(*(vu32*)(0x04000400 + ((n)<<4)))
#define SCHANNEL_VOL(n)				(*(vu8*)(0x04000400 + ((n)<<4)))
#define SCHANNEL_PAN(n)				(*(vu8*)(0x04000402 + ((n)<<4)))
#define SCHANNEL_SOURCE(n)			(*(vu32*)(0x04000404 + ((n)<<4)))
#define SCHANNEL_TIMER(n)			(*(vu16*)(0x04000408 + ((n)<<4)))
#define SCHANNEL_REPEAT_POINT(n)	(*(vu16*)(0x0400040A + ((n)<<4)))
#define SCHANNEL_LENGTH(n)			(*(vu32*)(0x0400040C + ((n)<<4)))


//---------------------------------------------------------------------------------
// Sound Capture Registers
//---------------------------------------------------------------------------------
#define REG_SNDCAP0CNT	(*(vu8*)0x04000508)
#define REG_SNDCAP1CNT	(*(vu8*)0x04000509)

#define REG_SNDCAP0DAD	(*(vu32*)0x04000510)
#define REG_SNDCAP0LEN  (*(vu16*)0x04000514)
#define REG_SNDCAP1DAD	(*(vu32*)0x04000518)
#define REG_SNDCAP1LEN	(*(vu16*)0x0400051C)

typedef void (*MIC_BUF_SWAP_CB)(u8* completedBuffer, int length);




/*---------------------------------------------------------------------------------
	microphone code based on neimod's microphone example.
	See: http://neimod.com/dstek/ 
	Chris Double (chris.double@double.co.nz)
	http://www.double.co.nz/nintendo_ds
---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------
	Read a byte from the microphone
---------------------------------------------------------------------------------*/
u8 micReadData8();
u16 micReadData12();

/*---------------------------------------------------------------------------------
	Fill the buffer with data from the microphone. The buffer will be
	signed sound data at 16kHz. Once the length of the buffer is
	reached, no more data will be stored. Uses ARM7 timer 0.  
---------------------------------------------------------------------------------*/
void micStartRecording(u8* buffer, int length, int freq, int timer, bool eightBitSample, MIC_BUF_SWAP_CB bufferSwapCallback);

/*---------------------------------------------------------------------------------
	Stop recording data, and return the length of data recorded.
---------------------------------------------------------------------------------*/
int micStopRecording(void);

/* This must be called during IRQ_TIMER0 */
void micTimerHandler(void);

void micSetAmp(u8 control, u8 gain);

//---------------------------------------------------------------------------------
// Turn the microphone on 
//---------------------------------------------------------------------------------
static inline void micOn(void) {
//---------------------------------------------------------------------------------
  micSetAmp(PM_AMP_ON, PM_GAIN_160);
}


//---------------------------------------------------------------------------------
// Turn the microphone off 
//---------------------------------------------------------------------------------
static inline void micOff(void) {
//---------------------------------------------------------------------------------
  micSetAmp(PM_AMP_OFF, 0);
}

void installSoundFIFO(void);

#ifdef __cplusplus
}
#endif

#endif

