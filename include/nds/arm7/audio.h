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
#include <calico/nds/arm7/sound.h>

#define SOUND_VOL(n)	SOUNDCNT_VOL(n)
#define SOUND_FREQ(n)	(-soundTimerFromHz(n))
#define SOUND_ENABLE	SOUNDCNT_ENABLE

#define SOUND_REPEAT    SOUNDxCNT_MODE(SoundMode_Repeat)
#define SOUND_ONE_SHOT  SOUNDxCNT_MODE(SoundMode_OneShot)

#define SOUND_FORMAT_16BIT SOUNDxCNT_FMT(SoundFmt_Pcm16)
#define SOUND_FORMAT_8BIT  SOUNDxCNT_FMT(SoundFmt_Pcm8)
#define SOUND_FORMAT_PSG   SOUNDxCNT_FMT(SoundFmt_Psg)
#define SOUND_FORMAT_ADPCM SOUNDxCNT_FMT(SoundFmt_ImaAdpcm)

#define SOUND_PAN(n)	SOUNDxCNT_PAN(n)

#define SCHANNEL_ENABLE SOUNDxCNT_ENABLE

//---------------------------------------------------------------------------------
// registers
//---------------------------------------------------------------------------------

#define REG_MASTER_VOLUME	REG_SOUNDCNTVOL


#define SCHANNEL_CR(n)				REG_SOUNDxCNT(n)
#define SCHANNEL_VOL(n)				REG_SOUNDxVOL(n)
#define SCHANNEL_PAN(n)				REG_SOUNDxPAN(n)
#define SCHANNEL_SOURCE(n)			REG_SOUNDxSAD(n)
#define SCHANNEL_TIMER(n)			REG_SOUNDxTMR(n)
#define SCHANNEL_REPEAT_POINT(n)	REG_SOUNDxPNT(n)
#define SCHANNEL_LENGTH(n)			REG_SOUNDxLEN(n)


//---------------------------------------------------------------------------------
// Sound Capture Registers
//---------------------------------------------------------------------------------
#define REG_SNDCAP0CNT	REG_SNDCAPxCNT(0)
#define REG_SNDCAP1CNT	REG_SNDCAPxCNT(1)

#define REG_SNDCAP0DAD	REG_SNDCAPxDAD(0)
#define REG_SNDCAP0LEN  REG_SNDCAPxLEN(0)
#define REG_SNDCAP1DAD	REG_SNDCAPxDAD(1)
#define REG_SNDCAP1LEN	REG_SNDCAPxLEN(1)

typedef void (*MIC_BUF_SWAP_CB)(u8* completedBuffer, int length);

//---------------------------------------------------------------------------------
// DSi Registers
//---------------------------------------------------------------------------------

#define REG_SNDEXTCNT	REG_SNDEXCNT
#define REG_MICCNT		(*(vu16*)0x04004600)
#define REG_MICDATA		(*(vu32*)0x04004604)

#define SNDEXTCNT_RATIO(n)		SNDEXCNT_MIX_RATIO(n)
#define SNDEXTCNT_FREQ_32KHZ	SNDEXCNT_I2S_32728_HZ
#define SNDEXTCNT_FREQ_47KHZ	SNDEXCNT_I2S_4761x_HZ
#define SNDEXTCNT_MUTE			SNDEXCNT_MUTE
#define SNDEXTCNT_ENABLE		SNDEXCNT_ENABLE

#define MICCNT_FORMAT(n)		((n)&3) // unknown, always set to '2'
#define MICCNT_FREQ_DIV(n)		(((n)&3)<<2) // F/(n+1) where F is SNDEXTCNT output freq
#define MICCNT_EMPTY			BIT(8)
#define MICCNT_NOT_EMPTY		BIT(9)
#define MICCNT_MORE_DATA		BIT(10)
#define MICCNT_OVERRUN			BIT(11)
#define MICCNT_CLEAR_FIFO		BIT(12)
#define MICCNT_ENABLE_IRQ		BIT(13)
#define MICCNT_ENABLE_IRQ2		BIT(14)
#define MICCNT_ENABLE			BIT(15)

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

#ifdef __cplusplus
}
#endif

#endif

