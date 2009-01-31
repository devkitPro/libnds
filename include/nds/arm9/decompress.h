/*---------------------------------------------------------------------------------

	Copyright (C) 2005
		Jason Rogers (dovoto)

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
/*! \file decompress.h
    \brief wraps the bios decompress functionality into something a bit easier to deal with
*/

	


#ifndef NDS_DECOMPRESS
#define NDS_DECOMPRESS

#include <nds/ndstypes.h>

typedef u8 (*getByteCallback)(u8 *source);
typedef int (*getHeaderCallback)(u8 *source, u16 *dest, u32 arg);

/** \brief the types of decompression available */
typedef enum
{
   LZ77, /** \brief LZ77 decompression  */
   LZ77Vram,/** \brief vram safe LZ77 decompression  */
   HUFF,/** \brief vram safe huff decompression  */
   RLE,/** \brief run length encoded decompression  */
   RLEVram /** \brief vram safe run length encoded decompression  */
}DecompressType;

#ifdef __cplusplus
extern "C" {
#endif

/** \fn decompress(const void* data, void* dst, DecompressType type)
*    \brief decompresses data using the suported type
*    \param dst the destination to decompress to
*    \param data the data to decompress
*    \param type the type of data to decompress
*/
void decompress(const void* data, void* dst, DecompressType type);

/** \fn decompressStream(const void* data, void* dst, DecompressType type,  getByteCallback readCB, getHeaderCallback getHeaderCB)
*    \brief decompresses data using the suported type (only LZ77Vram, HUFF, and RLEVram support streaming)
*    \param dst the destination to decompress to
*    \param data the data to decompress
*    \param type the type of data to decompress
*    \param readCB a callback to read the next byte of data
*    \param getHeaderCB a callback to read the 32 byte header
*/
void decompressStream(const void* data, void* dst, DecompressType type, getByteCallback readCB, getHeaderCallback getHeaderCB);

#ifdef __cplusplus
}
#endif

#endif

