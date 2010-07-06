/*---------------------------------------------------------------------------------

	Sound Functions

	Copyright (C) 2005
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
#ifndef _sound_h_
#define _sound_h_

/*! \file sound.h
	\brief A simple sound playback library for the DS.  Provides functionality
	for starting and stopping sound effects from the ARM9 side as well as access
	to PSG and noise hardware.  Maxmod should be used in most music and sound effect
	situations.
*/


#ifdef __cplusplus
extern "C" {
#endif

#include <nds/ndstypes.h>

typedef void (* MicCallback)(void* completedBuffer, int length);

/*! \brief Sound formats used by the DS */
typedef enum {
	SoundFormat_16Bit = 1, /*!<  16-bit PCM */
	SoundFormat_8Bit = 0,  /*!<  8-bit PCM */
	SoundFormat_PSG = 3,   /*!<  PSG (programmable sound generator?) */
	SoundFormat_ADPCM = 2  /*!<  IMA ADPCM compressed audio  */
}SoundFormat;

/*! \brief Microphone recording formats DS */
typedef enum {
	MicFormat_8Bit = 1, /*!<  8-bit PCM */
	MicFormat_12Bit = 0 /*!<  12-bit PCM */
}MicFormat;

/*! \brief PSG Duty cycles used by the PSG hardware */
typedef enum {
	DutyCycle_0  = 7, /*!<  0.0% duty cycle */
	DutyCycle_12 = 0, /*!<  12.5% duty cycle */
	DutyCycle_25 = 1, /*!<  25.0% duty cycle */
	DutyCycle_37 = 2, /*!<  37.5% duty cycle */
	DutyCycle_50 = 3, /*!<  50.0% duty cycle */
	DutyCycle_62 = 4, /*!<  62.5% duty cycle */
	DutyCycle_75 = 5, /*!<  75.0% duty cycle */
	DutyCycle_87 = 6  /*!<  87.5% duty cycle */
}DutyCycle;

/*! \fn void soundEnable(void)
	\brief Enables Sound on the DS.  Should be called prior to
	attempting sound playback
*/
void soundEnable(void);

/*! \fn void soundDisable(void)
	\brief Disables Sound on the DS.
*/
void soundDisable(void);

/*! \fn int soundPlaySample(const void* data, SoundFormat format, u32 dataSize, u16 freq, u8 volume, u8 pan, bool loop, u16 loopPoint);
	\brief Plays a sound in the specified format at the specified frequency.

	\param data A pointer to the sound data
	\param format The format of the data (only 16-bit and 8-bit pcm and ADPCM formats are supported by this function)
	\param dataSize The size in bytes of the sound data
	\param freq The frequency in Hz of the sample
	\param volume The channel volume.  0 to 127 (min to max)
	\param pan The channel pan 0 to 127 (left to right with 64 being centered)
	\param loop If true, the sample will loop playing once then repeating starting at the offset stored in loopPoint
	\param loopPoint The offset for the sample loop to restart when repeating
	\return An integer id coresponding to the channel of playback.  This value can be used to pause, resume, or kill the sound
	as well as adjust volume, pan, and frequency
*/
int soundPlaySample(const void* data, SoundFormat format, u32 dataSize, u16 freq, u8 volume, u8 pan, bool loop, u16 loopPoint);

/*! \fn int soundPlayPSG(DutyCycle cycle, u16 freq, u8 volume, u8 pan);
	\brief Pause a tone with the specified properties
	\param cycle The DutyCycle of the sound wave
	\param freq The frequency in Hz of the sample
	\param volume The channel volume.  0 to 127 (min to max)
	\param pan The channel pan 0 to 127 (left to right with 64 being centered)
	\return An integer id coresponding to the channel of playback.  This value can be used to pause, resume, or kill the sound
	as well as adjust volume, pan, and frequency
*/
int soundPlayPSG(DutyCycle cycle, u16 freq, u8 volume, u8 pan);

/*! \fn int soundPlayNoise(u16 freq, u8 volume, u8 pan);
	\brief Plays white noise with the specified parameters
	\param freq The frequency in Hz of the sample
	\param volume The channel volume.  0 to 127 (min to max)
	\param pan The channel pan 0 to 127 (left to right with 64 being centered)
	\return An integer id coresponding to the channel of playback.  This value can be used to pause, resume, or kill the sound
	as well as adjust volume, pan, and frequency
*/
int soundPlayNoise(u16 freq, u8 volume, u8 pan);

/*! \fn void soundPause(int soundId)
	\brief Pause the sound specified by soundId
	\param soundId The sound ID returned by play sound
*/
void soundPause(int soundId);

/*! \fn void soundSetWaveDuty(int soundId, DutyCycle cycle)
	\brief Sets the Wave Duty of a PSG sound
	\param soundId The sound ID returned by play sound
	\param cycle The DutyCycle of the sound wave
*/
void soundSetWaveDuty(int soundId, DutyCycle cycle);

/*! \fn void soundKill(int soundId)
	\brief Stops the sound specified by soundId and frees any resources allocated
	\param soundId The sound ID returned by play sound
*/
void soundKill(int soundId);

/*! \fn void soundResume(int soundId)
	\brief Resumes a paused sound
	\param soundId The sound ID returned by play sound
*/
void soundResume(int soundId);

/*! \fn void soundSetVolume(int soundId, u8 volume)
	\brief Sets the sound volume
	\param soundId The sound ID returned by play sound
	\param volume The new volume (0 to 127 min to max)
*/
void soundSetVolume(int soundId, u8 volume);

/*! \fn void soundSetPan(int soundId, u8 pan)
	\brief Sets the sound pan
	\param soundId The sound ID returned by play sound
	\param pan The new pan value (0 to 127 left to right (64 = center))
*/
void soundSetPan(int soundId, u8 pan);

/*! \fn void soundSetFreq(int soundId, u16 freq)
	\brief Sets the sound frequency
	\param soundId The sound ID returned by play sound
	\param freq The frequency in Hz
*/
void soundSetFreq(int soundId, u16 freq);

/*! \fn int soundMicRecord(void *buffer, u32 bufferLength, MicFormat format, int freq, MicCallback callback);

	\brief Starts a microphone recording to a double buffer specified by buffer
	\param buffer A pointer to the start of the double buffer
	\param bufferLength The length of the buffer in bytes (both halfs of the double buffer)
	\param format Microphone can record in 8 or 12 bit format.  12 bit is shifted up to 16 bit pcm
	\param freq The sample frequency
	\param callback This will be called every time the buffer is full or half full

	\return Returns non zero for success.
*/
int soundMicRecord(void *buffer, u32 bufferLength, MicFormat format, int freq, MicCallback callback);

/*! \fn void soundMicOff(void)
	\brief Stops the microphone from recording
*/
void soundMicOff(void);

#ifdef __cplusplus
}
#endif

#endif // _sound_h_
