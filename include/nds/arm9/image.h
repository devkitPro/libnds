/*---------------------------------------------------------------------------------

  Copyright (C) 2005
			Jason Rogers (dovoto)
			Dave Murphy (WinterMute)

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
/*! \file image.h
	\brief An image abstraction for working with image data.

<div class="fileHeader">
Image data pointers must be allocated using malloc as the conversion
rutiens will free the pointers and allocate new data.  As such any loader
implemented utilizing this structure must use malloc() to allocate the image
pointer data.
</div>
*/

#ifndef IMAGE_H
#define IMAGE_H

#include <nds/arm9/video.h>

//!	\brief holds a red green blue triplet
typedef struct RGB_24
{
	unsigned char r;	//!< 8 bits for the red value.
	unsigned char g;	//!< 8 bits for the green value.
	unsigned char b;	//!< 8 bits for the blue value.
} __attribute__ ((packed)) RGB_24;



//!	A generic image structure.
typedef struct sImage
{
	short height; 				/*!< \brief The height of the image in pixels */
	short width; 				/*!< \brief The width of the image in pixels */
	int bpp;					/*!< \brief Bits per pixel (should be 4 8 16 or 24) */
	unsigned short* palette;	/*!< \brief A pointer to the palette data */

	//! A union of data pointers to the pixel data.
	union
	{
		u8* data8;		//!< pointer to 8 bit data.
		u16* data16;	//!< pointer to 16 bit data.
		u32* data32;	//!< pointer to 32 bit data.
	} image;

} sImage, *psImage;

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Converts a 24 bit image to 16 bit
	\param img a pointer to image to manipulate
*/
void image24to16(sImage* img);

/*! \brief Converts an 8 bit image to 16 bit setting the alpha bit
	\param img a pointer to image to manipulate
*/
void image8to16(sImage* img);

/*! \brief Converts an 8 bit image to 16 bit with alpha bit cleared for the supplied palette index
	\param img a pointer to image to manipulate
	\param transparentColor Color indexes equal to this value will have the alpha bit clear
*/
void image8to16trans(sImage* img, u8 transparentColor);

/*! \brief frees the image data. Only call if the image data was returned from an image loader
	\param img a pointer to image to manipulate (the image data will be free() )
*/
void imageDestroy(sImage* img);

/*! \brief Tiles 8 bit image data into a sequence of 8x8 tiles
	\param img a pointer to image to manipulate
*/
void imageTileData(sImage* img);

#ifdef __cplusplus
}
#endif

//why is this included here?
#include <nds/arm9/pcx.h>

#endif
