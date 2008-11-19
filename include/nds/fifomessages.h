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

/*! \file fifomessages.h 
\brief standard fifo messages utilized by libnds. 
*/

typedef enum {
	SOUND_PLAY_MESSAGE = 0x1234,
	SOUND_PSG_MESSAGE = 0x1235,
	SOUND_NOISE_MESSAGE = 0x1236,
	MIC_RECORD_MESSAGE = 0x1237,
	MIC_BUFFER_FULL_MESSAGE = 0x1238,
	SYS_INPUT_MESSAGE = 0x1239
}FifoMessageType;

typedef struct{
	u16 type;
	u8 empty[32]; 
}ALIGN(4) FifoMessage;

//sound messages
typedef struct{
	u16 type; 	
	const void* data;
	u16 loopPoint;
	u16 dataSize;
	u16 freq;
	u8 volume;
	u8 pan;
	bool loop;
	u8 format;
}ALIGN(4) SoundPlayMsg;

typedef struct{
	u16 type;
	u16 freq;	
	u8 dutyCycle;
	u8 volume;
	u8 pan;
}ALIGN(4) SoundPsgMsg;

typedef struct{
	u16 type;
	void* buffer;
	u32 bufferLength;
	u16 freq;
	u8 format;
}ALIGN(4) MicRecordMsg;

typedef struct{
	u16 type;
	void* buffer;
	u32 length;
}ALIGN(4) MicBufferFullMsg;

typedef struct{
	u16 type;
	touchPosition touch;
	u16 keys;
}ALIGN(4) SystemInputMsg;

#endif
