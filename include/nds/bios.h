/*---------------------------------------------------------------------------------
	$Id: bios.h,v 1.5 2005-08-23 17:06:10 wntrmute Exp $

	BIOS functions

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

	$Log: not supported by cvs2svn $
	Revision 1.4  2005/08/03 05:26:42  wntrmute
	use BIT macro
	corrected header include
	
	Revision 1.3  2005/08/01 23:18:22  wntrmute
	adjusted headers for logging
	

---------------------------------------------------------------------------------*/

#ifndef _BIOS_INCLUDE_
#define _BIOS_INCLUDE_

#ifdef __cplusplus
extern "C" {
#endif

#include <nds/jtypes.h>


typedef struct DecompressionStream {
  int (*getSize)(uint8 * source, uint16 * dest, uint32 r2);
  int (*getResult)(uint8 * source); // can be NULL
  uint8 (*readByte)(uint8 * source);
} PACKED TDecompressionStream;


typedef struct UnpackStruct {
  uint16 sourceSize; // in bytes
  uint8 sourceWidth;
  uint8 destWidth;
  uint32 dataOffset;  
} PACKED TUnpackStruct, * PUnpackStruct;

// SoftReset (swi 0x00)
extern void swiSoftReset(void);

// DelayLoop (swi 0x03)
extern void swiDelay(uint32 duration);


/*---------------------------------------------------------------------------------
	IntrWait (swi 0x04)

	waitForSet -	0: Return if the interrupt has already occured
					1: Wait until the interrupt has been set since the call
	flags - interrupt sensitivity bitmask to wait for
---------------------------------------------------------------------------------*/
void swiIntrWait(int waitForSet, uint32 flags);

/*---------------------------------------------------------------------------------
	WaitForVBlank (swi 0x05)
	Identical to calling IntrWait(1, 1)
---------------------------------------------------------------------------------*/
void swiWaitForVBlank(void);

#ifdef ARM9
/*---------------------------------------------------------------------------------
	WaitForIRQ (swi 0x06)
		mov r0, #0
		MCR p15, 0, r0, c7, c0, 4
---------------------------------------------------------------------------------*/
void swiWaitForIRQ(void);

#endif // ARM9

#ifdef ARM7
/*---------------------------------------------------------------------------------
	Halt (swi 0x06)
	Identical to calling SetHaltCR(?)
---------------------------------------------------------------------------------*/
void swiHalt(void);
/*---------------------------------------------------------------------------------
	Sleep (swi 0x07)
	Identical to calling SetHaltCR(?)
---------------------------------------------------------------------------------*/
void swiSleep(void);
/*---------------------------------------------------------------------------------
	SetHaltCR(uint32 data) (swi 0x1F)
	Writes data to 0x04000300:32
---------------------------------------------------------------------------------*/
void swiSetHaltCr(uint32 data);

#endif //ARM7

//////////////////////////////////////////////////////////////////////
//
// ChangeSound(int enabled, int delay)
//   enabled
//     0: decrement SOUND_BIAS once per delay until it reaches 0x000
//     1: increment SOUND_BIAS once per delay until it reaches 0x200
//   delay is in the same units of time as swiDelayLoop
//
//////////////////////////////////////////////////////////////////////

void swiChangeSoundBias(int enabled, int delay);

//////////////////////////////////////////////////////////////////////
//
// swi 0x09: Divide (returns result in r0, remainder in r1)
//
// int Divide(int numerator, int divisor);
//   returns numerator / divisor
//
// int Remainder(int numerator, int divisor);
//   returns numerator % divisor
//
// void DivMod(int numerator, int divisor, int * result, int * remainder);
//   sets *result = numerator / divisor and remainder = numerator % divisor
//
//////////////////////////////////////////////////////////////////////

extern int swiDivide(int numerator, int divisor);
extern int swiRemainder(int numerator, int divisor);
extern void swiDivMod(int numerator, int divisor, int * result, int * remainder);

//////////////////////////////////////////////////////////////////////
//
// Copy (swi 0x0B)
//  source = pointer to transfer source
//  dest = pointer to transfer destination
//  flags(26) = transfer width (0: halfwords, 1: words)
//  flags(24) = transfer mode (0: copy, 1: fill)
//  flags(20..0) = tranfer count (always in words)
//
// FastCopy (swi 0x0C)
//  source = pointer to transfer source
//  dest = pointer to transfer destination
//  flags(24) = transfer mode (0: copy, 1: fill)
//  flags(20..0) = tranfer count (in words)
//
// Transfers more quickly, but has a higher interrupt latency
//
//////////////////////////////////////////////////////////////////////

#define COPY_MODE_HWORD	(0)
#define COPY_MODE_WORD	(1<<26)
#define COPY_MODE_COPY	(0)
#define COPY_MODE_FILL	(1<<24)

extern void swiCopy(void * source, void * dest, int flags);
extern void swiFastCopy(void * source, void * dest, int flags);

//////////////////////////////////////////////////////////////////////
// Sqrt (0x0D)
//////////////////////////////////////////////////////////////////////

extern int swiSqrt(int value);

//////////////////////////////////////////////////////////////////////
//
// CRC16(uint16 crc, void * data, uint32 size) (swi 0x0E)
//   crc - starting CRC-16
//   data - pointer to data (processed nibble by nibble)
//   size - size in bytes
//
// Returns the CRC-16 after the data has been processed
//
//////////////////////////////////////////////////////////////////////

extern uint16 swiCRC16(uint16 crc, void * data, uint32 size);

//////////////////////////////////////////////////////////////////////
//
// int IsDebugger;
//   Returns 0 if running on a debugger (8 MB of ram instead of 4 MB)
//
//////////////////////////////////////////////////////////////////////

extern int swiIsDebugger(void);

//////////////////////////////////////////////////////////////////////
//
// UnpackBits (swi 0x10)
//  r0 - Source address
//  r1 - destination address (word aligned)
//  r2 - Unpack structure
//       uint16 sourceSize (in bytes)
//       uint8 sourceWidth (1,2,4,8)
//       uint8 destWidth (1,2,4,8,16,32)
//       uint32 dataOffset (bits 0..30 are added to all non-zero
//                          destination writes, unless b31 is set,
//                          which does it for zeros too)
//
// Unpack data stored multiple elements to a byte into a larger space
// i.e. 8 elements per byte (i.e. b/w font), into 1 element per byte
//
//////////////////////////////////////////////////////////////////////

extern void swiUnpackBits(uint8 * source, uint32 * destination, PUnpackStruct params);

//////////////////////////////////////////////////////////////////////
//
// DecompressLZSSWram (swi 0x11)
//   r0 - pointer to a header word, followed by compressed data
//        word(31..8) = size of uncompressed data (in bytes)
//        word(7..0) = ignored
//   r1 - destination address
// Writes data a byte at a time
//
// DecompressLZSSVram (swi 0x12)
// Writes data a halfword at a time
// See DecompressRLEVram for parameter details
//
//////////////////////////////////////////////////////////////////////

extern void swiDecompressLZSSWram(void * source, void * destination);
extern int swiDecompressLZSSVram(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream);

//////////////////////////////////////////////////////////////////////
//
// swiDecompressHuffman (swi 0x13)
// See DecompressRLE for parameter details
//
//////////////////////////////////////////////////////////////////////

extern int swiDecompressHuffman(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream);

//////////////////////////////////////////////////////////////////////
//
// DecompressRLEWram (swi 0x14)
//   r0 - pointer to a header word, followed by compressed data
//        word(31..8) = size of uncompressed data (in bytes)
//        word(7..0) = ignored
//   r1 - destination address
// Writes data a byte at a time
//
// DecompressRLEVram (swi 0x15)
//   r0 - pointer to compressed source data, maybe! (always goes through
//        the function pointers, so could just be an offset)
//   r1 - pointer to destination (halfword)
//   r2 - no idea, passed to getSize function
//   r3 - pointer to a structure
// Writes data a halfword at a time
//
// Result of getSize is a word, with the size of decompressed data in
// bits 31..8, and bits 7..0 are ignored.  This value is also returned
// by the swi, unless getResult is non-NULL and returns a neg. value
//
// getResult is used to provide a result for the swi, given the source
// pointer after all data has been read (or if getSize < 0).  It can
// be NULL, unlike getSize and readByte.  Its value is only returned
// if negative, otherwise the typical result is used, so it is likely
// some sort of error-checking procedure
//
// readByte is used in lieu of directly accessing memory
//
// compressed data format:
//   bit(7): 0: uncompressed, 1: compressed
//   bit(6..0) when uncompressed: run length - 1, followed by run_length bytes of true data
//   bit(6..0) when compressed: run length - 3, followed by one byte of true data, to be repeated
//
//////////////////////////////////////////////////////////////////////

extern void swiDecompressRLEWram(void * source, void * destination);
extern int swiDecompressRLEVram(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream);

//////////////////////////////////////////////////////////////////////
//
// DecodeDelta8 (swi 0x16)
//   r0 - pointer to a header word, followed by encoded data
//        word(31..8) = size of data (in bytes)
//        word(7..0) = ignored
//   r1 - destination address
// Writes data a byte at a time
//
// DecodeDelta16 (swi 0x18)
//   r0 - pointer to a header word, followed by encoded data
//        word(31..8) = size of data (in bytes)
//        word(7..0) = ignored
//   r1 - destination address
// Writes data a halfword at a time
//
//////////////////////////////////////////////////////////////////////

extern void swiDecodeDelta8(void * source, void * destination);
extern void swiDecodeDelta16(void * source, void * destination);

//////////////////////////////////////////////////////////////////////
//
// swiGetSineTable(int index) (swi 0x1A)
//  Returns an entry in the sine table (index = 0..63)
//
//////////////////////////////////////////////////////////////////////

uint16 swiGetSineTable(int index);

//////////////////////////////////////////////////////////////////////
//
// GetPitchTable(int index) (swi 0x1B)
//  Returns an entry in the pitch table (index = 0..767)
//
//////////////////////////////////////////////////////////////////////

extern uint16 swiGetPitchTable(int index);

//////////////////////////////////////////////////////////////////////
//
// GetVolumeTable(int index)
//  Returns an entry in the volume table (index = 0..723)
//
//////////////////////////////////////////////////////////////////////

extern uint8 swiGetVolumeTable(int index);


//////////////////////////////////////////////////////////////////////
//
// Unknown (swi 0x1D)
//
//////////////////////////////////////////////////////////////////////

extern void swiUnknown(void);

/*---------------------------------------------------------------------------------
	SetHaltCR(uint32 data) (swi 0x1F)
	Writes data to 0x04000300:32
---------------------------------------------------------------------------------*/
extern void swiSetHaltCR(uint32 data);


#ifdef __cplusplus
}
#endif

#endif

