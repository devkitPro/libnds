/*---------------------------------------------------------------------------------

	Sound Functions

	Copyright (C) 2008
		Dave Murphy (WinterMute)
		Jason Rogers (Dovoto)

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
#include <nds/arm9/sound.h>
#include <string.h>

__attribute__((constructor)) static void _soundEnsureInit(void)
{
	soundInit();
}

static int _soundFindFreeChannel(int chmin, int chmax)
{
	unsigned mask = soundGetActiveChannels();
	for (int i = chmin; i < chmax; i ++) {
		if (!(mask & (1U<<i))) {
			return i;
		}
	}

	return -1;
}

int soundPlayPSG(DutyCycle cycle, u16 freq, u8 volume, u8 pan)
{
	int ch = _soundFindFreeChannel(8, 14);
	if (ch >= 0) {
		soundPreparePsg(ch | SOUND_START, volume<<4, pan, soundTimerFromHz(freq), cycle);
	}

	return ch;
}

int soundPlayNoise(u16 freq, u8 volume, u8 pan)
{
	int ch = _soundFindFreeChannel(14, 16);
	if (ch >= 0) {
		soundPreparePsg(ch | SOUND_START, volume<<4, pan, soundTimerFromHz(freq), SoundDuty_12_5);
	}

	return ch;
}

int soundPlaySample(const void* data, SoundFormat format, u32 dataSize, u16 freq, u8 volume, u8 pan, bool loop, u16 loopPoint)
{
	int ch = _soundFindFreeChannel(0, 16);

	if (ch >= 0) {
		soundPreparePcm(ch | SOUND_START, volume<<4, pan, soundTimerFromHz(freq),
			loop ? SoundMode_Repeat : SoundMode_OneShot, format, data, loopPoint, dataSize/4);
	}

	return ch;
}
