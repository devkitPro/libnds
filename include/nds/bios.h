/*---------------------------------------------------------------------------------

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

  1.  The origin of this software must not be misrepresented; you
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product
      documentation would be appreciated but is not required.
  2.  Altered source versions must be plainly marked as such, and
      must not be misrepresented as being the original software.
  3.  This notice may not be removed or altered from any source
      distribution.

---------------------------------------------------------------------------------*/

#ifndef BIOS_H_INCLUDE
#define BIOS_H_INCLUDE

/*!	\file bios.h
	\brief Nintendo DS Bios functions

	See gbatek for more information.
*/


#ifdef __cplusplus
extern "C" {
#endif


#include "nds/ndstypes.h"


/*!	\brief Should return the header of a compressed stream of bytes.

	The result is a word, with the size of decompressed data in bits 8-31,
	and bits 0-7 are ignored.  This value is also returned by the bios function, unless getResult is non-NULL and returns a negative value.
	This useally returns the 4 bytes that source points to.

	\param source 	A pointer to the compressed data.
	\param dest		A pointer to the space where the decompressed data should be copied to.
	\param arg		A callback value that gets passed to the bios function.
	\return The header of the compressed data containing the length of the data and the compression type.
*/
typedef int (*getHeaderCallback)(u8 *source, u16 *dest, u32 arg);


/*!	\brief Should verify the result after data got decompressed.

	getResult is used to provide a result for the bios function, given the source pointer after all data has been read
	(or if getSize < 0). Its value is only returned if negative, otherwise the typical result is used, so it is likely
	some sort of error-checking procedure.

	\param source The current source address.
	\return 0 if it went right, or a negative number if something went wrong. value will be returned from bios function if value is negative.
*/
typedef int (*getResultCallback)(u8 * source);


/*!	\brief Should returns a raw byte of the stream.
	\param source A pointer to the byte.
	\return A byte.
*/
typedef u8 (*getByteCallback)(u8 *source);




//! A struct that contains callback function pointers used by the decompression functions.
typedef struct DecompressionStream
{
	getHeaderCallback getSize;		//!< gets called to get the header of the stream.
	getResultCallback getResult;	//!< gets called to verify the result afterwards, can be NULL (no callback).
	getByteCallback readByte;		//!< gets called to get a byte of the compressed data.
	//according to gbatek, there are 2 more callback pointers here.
} PACKED TDecompressionStream;


//! A struct and struct pointer with information about unpacking data.
typedef struct UnpackStruct
{
	uint16 sourceSize; 		//!< in bytes
	uint8 sourceWidth;		//!< 1,2,4 or 8 bits.
	uint8 destWidth;		//!< 1,2,4,8,16 or 32 bits.
	uint32 dataOffset;		//!< bits 0-30 are added to all non-zero destination writes, unless bit 31 is set, which does it for zeros too.
} PACKED TUnpackStruct, * PUnpackStruct;



/*!
	\brief resets the DS.
*/
void swiSoftReset(void);

/*!
	\brief delays the code.

   Delays for for a period X + Y*duration where X is the swi overhead and Y is a cycle of
<CODE><PRE>
     loop:
       sub r0, #1
       bgt loop
</PRE></CODE>
  of thumb fetches in BIOS memory
	\param duration length of delay
	\note Duration should be 1 or more, a duration of 0 is a huge delay.
*/
void swiDelay(uint32 duration);


/*!
	\brief divides 2 numbers.
	\param numerator	signed integer to divide
	\param divisor		signed integer to divide by
	\return numerator / divisor
*/
int swiDivide(int numerator, int divisor);

/*!
	\brief calculate the remainder of an division.
	\param numerator	signed integer to divide
	\param divisor		signed integer to divide by
	\return numerator % divisor
*/
int swiRemainder(int numerator, int divisor);

/*!
	\brief divides 2 numbers and stores both the result and the remainder.

	\param numerator	signed integer to divide
	\param divisor		signed integer to divide by
	\param result		pointer to integer set to numerator / divisor
	\param remainder 	pointer to integer set to numerator % divisor
*/
void swiDivMod(int numerator, int divisor, int * result, int * remainder);


//! copy in chunks of halfword size.
#define COPY_MODE_HWORD		(0)
//! copy in chunks of word size.
#define COPY_MODE_WORD		BIT(26)
//! copy a range of memory to another piece of memory
#define COPY_MODE_COPY		(0)
//! fill a piece of memory with a value.
#define COPY_MODE_FILL		BIT(24)


/*!	\brief copies or fills some memory.
	\param source 	pointer to transfer source or pointer to value to fill the memory with.
	\param dest		pointer to transfer destination.
	\param flags	bits(0-20): size of data to copy/fill in words,
						or'd with the copy mode size (word or halfword) and type (copy or fill).
*/
void swiCopy(const void * source, void * dest, int flags);


/*!	\brief copies or fills some memory.
	can only copy in word chunks.
	\param source	pointer to transfer source or pointer to value to fill the memory with.
	\param dest		pointer to transfer destination.
	\param flags	bits(0-20): size of data to copy/fill in words,
						or'd with the type (copy or fill).

	\note Transfers more quickly than swiCopy, but has higher interrupt latency.
*/
void swiFastCopy(const void * source, void * dest, int flags);


/*!	\brief calculates the square root.
	\param value the value to calculate.
	\return the square root of the value as an integer.
	\note use fixed point math if you want more accuracy.
*/
int swiSqrt(int value);


/*!	\brief calculates a CRC-16 checksum.
	\param crc 		starting CRC-16 value.
	\param data 	pointer to data (processed nibble by nibble)
	\param size 	size in bytes.

	\return the CRC-16 after the data has been processed.
*/
uint16 swiCRC16(uint16 crc, void * data, uint32 size);


/*!	\brief returns 0 if running on a nintendo hardware debugger.
	\return 0 if running on a debugger (8 MB of ram instead of 4 MB), else some other number.
*/
int swiIsDebugger(void);


/*!	\brief Unpack data stored in multiple elements in a byte to a larger space.

	i.e. 8 elements per byte (i.e. b/w font), into 1 element per byte.

	\param source			Source address.
	\param destination		destination address (word aligned).
	\param params			pointer to an UnpackStruct.
*/
void swiUnpackBits(uint8 * source, uint32 * destination, PUnpackStruct params);


/*!	\brief Decompresses LZSS compressed data.
	\param source		pointer to a header word, followed by compressed data.
						bit 0-7 of header is ignored.
						bit 8-31 of header is size of uncompressed data in bytes.
	\param destination	destination address.
	\note Writes data a byte at a time.
	\see decompress.h
*/
void swiDecompressLZSSWram(void * source, void * destination);


/*!	\brief Decompresses LZSS compressed data vram safe.

	\param source			Pointer to source data (always goes through the function pointers, so could just be an offset).
	\param destination		Pointer to destination.
	\param toGetSize		Callback value that is passed to getHeaderCallback function pointer.
	\param stream			Pointer to struct with callback function pointers.

	\return The length of the decompressed data, or a signed errorcode from the Open/Close functions.
	\note Writes data a halfword at a time.
	\see decompress.h
*/
int swiDecompressLZSSVram(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream);

int swiDecompressLZSSVramNTR(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream);
int swiDecompressLZSSVramTWL(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream);


/*!	\brief Decompresses Huffman compressed data.

	\param source			Pointer to source data (always goes through the function pointers, so could just be an offset).
	\param destination		Pointer to destination.
	\param toGetSize		Callback value that is passed to getHeaderCallback function pointer.
	\param stream			Pointer to struct with callback function pointers.

	\return The length of the decompressed data, or a signed errorcode from the Open/Close functions.
	\see decompress.h
*/
int swiDecompressHuffman(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream);


/*!	\brief Decompresses RLE compressed data.

	compressed data format:
	bit(7): 0= uncompressed, 1= compressed.
	bit(0-6) when uncompressed: run length - 1, followed by run_length bytes of true data.
	bit(0-6) when compressed: run length - 3, followed by one byte of true data, to be repeated.

	\param source		pointer to a header word, followed by compressed data.
						bit 0-7 of header is ignored.
						bit 8-31 of header is size of uncompressed data in bytes.
	\param destination	destination address.
	\note Writes data a byte at a time.
	\see decompress.h
*/
void swiDecompressRLEWram(void * source, void * destination);


/*!	\brief Decompresses RLE compressed data vram safe.

	compressed data format:
	bit(7): 0= uncompressed, 1= compressed.
	bit(0-6) when uncompressed: run length - 1, followed by run_length bytes of true data.
	bit(0-6) when compressed: run length - 3, followed by one byte of true data, to be repeated.

	\param source			Pointer to source data (always goes through the function pointers, so could just be an offset).
	\param destination		Pointer to destination.
	\param toGetSize		Callback value that is passed to getHeaderCallback function pointer.
	\param stream			Pointer to struct with callback function pointers.

	\return The length of the decompressed data, or a signed errorcode from the Open/Close functions.
	\note Writes data a halfword at a time.
	\see decompress.h
*/
int swiDecompressRLEVram(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream);



#ifdef ARM9

/*!	\brief wait for any interrupt.

	\note ARM9 exclusive.
*/
void swiWaitForIRQ(void);


/*!	\brief Writes a word of the data to 0x04000300:32

	\param data the word to write.
	\note This is on the ARM9, but works differently then the ARM7 function!
*/
void swiSetHaltCR(uint32 data);


/*!	\brief Decodes a stream of bytes based on the difference of the bytes.
	\param source 		Pointer to a header word, followed by encoded data.
						word(31..8) = size of data (in bytes).
						word(7..0) = ignored.
	\param destination	Destination address.

	\note Writes data a byte at a time.
	\note ARM9 exclusive.
*/
void swiDecodeDelta8(void * source, void * destination);


/*!	\brief Decodes a stream of bytes based on the difference of the bytes.
	\param source		Pointer to a header word, followed by encoded data.
						word(31..8) = size of data (in bytes).
						word(7..0) = ignored.
	\param destination	Destination address.

	\note Writes data a halfword at a time.
	\note ARM9 exclusive.
*/
void swiDecodeDelta16(void * source, void * destination);
#endif


#ifdef ARM7


/*!	\brief Writes a byte of the data to 0x04000301:8
	\param data The byte to write.
	\note ARM7 exclusive.
*/
void swiSetHaltCR(uint8 data);

/*!	\brief Halts the CPU untill an interupt occures.
	\note ARM7 exclusive.
*/
void swiHalt(void);

/*!	\brief Halts the CPU and most of the hardware untill an interupt occures.
	\note ARM7 exclusive.
*/
void swiSleep(void);

/*!	\brief Switches the DS to GBA mode.
	\note ARM7 exclusive.
*/
void swiSwitchToGBAMode(void);


/*!	\brief Returns an entry in the sine table.
	\param index The index of the sine table (0-63).
	\return The entry.
	\note ARM7 exclusive.
*/
uint16 swiGetSineTable(int index);

/*!	\brief Returns an entry in the pitch table.
	\param index The index of the pitch table (0-767).
	\return The entry.
	\note ARM7 exclusive.
*/
uint16 swiGetPitchTable(int index);

/*!	\brief Returns an entry in the volume table.
	\param index The index of the volume table (0-723).
	\return The entry.
	\note ARM7 exclusive.
*/
uint8 swiGetVolumeTable(int index);

/*!	\brief increments or decrements the sound bias once per delay.
	\param enabled 	0 to decrement it until it reaches 0x000, 1 to increment it until it reaches 0x200.
	\param delay 	Is in the same units of time as swiDelay.
	\note ARM7 exclusive.
*/
void swiChangeSoundBias(int enabled, int delay);


#endif //ARM7


#ifdef __cplusplus
}
#endif

#endif
