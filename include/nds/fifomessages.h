/*---------------------------------------------------------------------------------

	Sound Functions

	Copyright (C) 2008
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
#ifndef FIFOMESSAGE_H
#define FIFOMESSAGE_H

#include "ndstypes.h"
#include <nds/touch.h>

/* internal fifo messages utilized by libnds. */

typedef enum {
	SOUND_PLAY_MESSAGE = 0x1234,
	SOUND_PSG_MESSAGE,
	SOUND_NOISE_MESSAGE,
	MIC_RECORD_MESSAGE,
	MIC_BUFFER_FULL_MESSAGE,
	SYS_INPUT_MESSAGE,
	SDMMC_SD_READ_SECTORS,
	SDMMC_SD_WRITE_SECTORS
} FifoMessageType;

typedef struct FifoMessage {
	u16 type;

	union {

		struct {
			const void* data;
			u32 dataSize;
			u16 loopPoint;
			u16 freq;
			u8 volume;
			u8 pan;
			bool loop;
			u8 format;
			u8 channel;
		} SoundPlay;

		struct{
			u16 freq;
			u8 dutyCycle;
			u8 volume;
			u8 pan;
			u8 channel;
		} SoundPsg;

		struct{
			void* buffer;
			u32 bufferLength;
			u16 freq;
			u8 format;
		} MicRecord;

		struct{
			void* buffer;
			u32 length;
		} MicBufferFull;

		struct{
			touchPosition touch;
			u16 keys;
		} SystemInput;
		
		struct{
			void *buffer;
			u32 startsector;
			u32	numsectors;
		} sdParams;

		struct{
			void *buffer;
			u32 address;
			u32	length;
		} fwParams;
	};

} ALIGN(4) FifoMessage;


#endif
