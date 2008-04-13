/*---------------------------------------------------------------------------------

	background.h -- definitions for DS backgrounds

	Copyright (C) 2007
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
/*! \file background.h
    \brief nds background defines and functionality.

<div class="fileHeader">
    The DS contains two separate hardware 2D cores responsible for rendering 2D backgrounds.  The definitions
    below outline the libnds api for utilizing these backgrounds.

    The background engine provides basic initialization and management of the 8 2D backgrounds available on the DS.  
    Other than initialization and hardware limitations background control is identical on both main and sub screens.  

    The following modes of operation are allowed: 
<div class="fixedFont"><pre>
Main 2D engine
______________________________
|Mode | BG0 | BG1 | BG2 |BG3 |           T = Text
|  0  |  T  |  T  |  T  |  T |           R = Rotation
|  1  |  T  |  T  |  T  |  R |           E = Extended Rotation
|  2  |  T  |  T  |  R  |  R |           L = Large Bitmap background
|  3  |  T  |  T  |  T  |  E |
|  4  |  T  |  T  |  R  |  E |
|  5  |  T  |  T  |  E  |  E |
|  6  |     |  L  |     |    |
-----------------------------

Sub 2D engine
______________________________
|Mode | BG0 | BG1 | BG2 |BG3 |
|  0  |  T  |  T  |  T  |  T |
|  1  |  T  |  T  |  T  |  R |
|  2  |  T  |  T  |  R  |  R |
|  3  |  T  |  T  |  T  |  E |
|  4  |  T  |  T  |  R  |  E |
|  5  |  T  |  T  |  E  |  E |
-----------------------------

</pre></div>

On the main engine BG0 can be uses as a 3D rendering surface.

</div> 
    
*/

/*! 
	\example Graphics/2D/BackgroundAllInOne/source/advanced.cpp
	These examples cover rotation and scaling of backgrounds
*/		

/*!
	\example Graphics/2D/BackgroundAllInOne/source/basic.cpp
	These examples cover basic initialization and loading of various backgrounds
*/

/*!
	\example Graphics/2D/BackgroundAllInOne/source/scrolling.cpp
	These examples cover background scrolling
*/

/*!
	\example Graphics/2D/BackgroundAllInOne/source/handmade.cpp
	This example loads a map created in code
*/





#ifndef _libnds_background_h_
#define _libnds_background_h_

#include <nds/ndstypes.h>
#include <nds/arm9/video.h>
#include <nds/arm9/sassert.h>
#include <nds/memory.h>
#include <nds/dma.h>
#include <nds/arm9/trig_lut.h>
#include <string.h>


#define MAP_BASE_SHIFT 8
#define TILE_BASE_SHIFT 2 

#define BG_MAP_RAM(base)		(((base)*0x800) + 0x06000000)
#define BG_MAP_RAM_SUB(base)	(((base)*0x800) + 0x06200000)

#define BG_TILE_RAM(base)		(((base)*0x4000) + 0x06000000)
#define BG_TILE_RAM_SUB(base)	(((base)*0x4000) + 0x06200000)

#define BG_BMP_RAM(base)		(((base)*0x4000) + 0x06000000)
#define BG_BMP_RAM_SUB(base)	(((base)*0x4000) + 0x06200000)


//register overlays
typedef struct {
	u16 x;
	u16 y;
} bg_scroll;

typedef struct {
    s16 xdx; /*!change in x per dx*/
    s16 ydx; /*!change in y per dx*/
    s16 xdy; /*!change in x per dy*/
    s16 ydy; /*!change in y per dy*/
    s32 dx;  /*!map x value which corresponds to the screen origin*/
    s32 dy;  /*!map y value which corresponds to the screen origin*/  
} bg_transform;

typedef struct {
    u16 control[4];
    bg_scroll scroll[4];
    bg_transform bg2_rotation;
    bg_transform bg3_rotation;
} bg_attribute;

#define BACKGROUND           (*((bg_attribute *)0x04000008))
#define BACKGROUND_SUB       (*((bg_attribute *)0x04001008))

//background state	
typedef struct
{
	int angle;
	s32 centerX;
	s32 centerY;
	s32 scaleX;
	s32 scaleY;
	s32 scrollX;
	s32 scrollY;
	int size;
	int type;
	
}BgState;


//id -> register look up tables
extern vuint16* bgControl[8];
extern bg_scroll* bgScrollTable[8];
extern bg_transform* bgTransform[8];


extern BgState bgState[8];

/**
 * \enum BgType
 * 
 *  Allowed background types, used in bgInitMain and bgInitSub
 */
typedef enum
{
	BgType_Text8bpp, /*!< 8bpp Tiled background with 16 bit tile indexes and no allowed rotation or scaling >*/
	BgType_Text4bpp, /*!< 4bpp Tiled background with 16 bit tile indexes and no allowed rotation or scaling >*/
	BgType_Rotation, /*!< Tiled background with 8 bit tile indexes Can be scaled and rotated >*/
	BgType_ExRotation, /*!< Tiled background with 16 bit tile indexes Can be scaled and rotated >*/
	BgType_Bmp8, /*!< Bitmap background with 8 bit color values which index into a 256 color palette >*/
	BgType_Bmp16 /*!< Bitmap background with 16 bit color values of the form aBBBBBGGGGGRRRRR
					(if 'a' is set the pixel will be rendered...if not the pixel will be transparent >*/
	
}BgType;


/**
 * \enum BgSize
 * \brief Allowed background Sizes
 * The lower 16 bits of these defines can be used directly to set the background control register bits
 */
typedef enum
{
	BgSize_R_128x128 =   (0 << 14), /*!< 128 x 128 pixel rotation background */
	BgSize_R_256x256 =   (1 << 14), /*!< 256 x 256 pixel rotation background */
	BgSize_R_512x512 =   (2 << 14), /*!< 512 x 512 pixel rotation background */
	BgSize_R_1024x1024 = (3 << 14), /*!< 1024 x 1024 pixel rotation background */
	
	BgSize_T_256x256 = (0 << 14) | (1 << 16), /*!< 256 x 256 pixel text background */
	BgSize_T_512x256 = (1 << 14) | (1 << 16), /*!< 512 x 256 pixel text background */
	BgSize_T_256x512 = (2 << 14) | (1 << 16), /*!< 256 x 512 pixel text background */
	BgSize_T_512x512 = (3 << 14) | (1 << 16), /*!< 512 x 512 pixel text background */
	
	BgSize_ER_128x128 = (0 << 14) | (2 << 16), /*!< 128 x 128 pixel extended rotation background */
	BgSize_ER_256x256 = (1 << 14) | (2 << 16), /*!< 256 x 256 pixel extended rotation background */
	BgSize_ER_512x512 = (2 << 14) | (2 << 16), /*!< 512 x 512 pixel extended rotation background */
	BgSize_ER_1024x1024 = (3 << 14) | (2 << 16),/*!< 1024 x 1024 extended pixel rotation background */

	BgSize_B8_128x128 =  ((0 << 14) | BIT(7) | (3 << 16)),  /*!< 128 x 128 pixel 8 bit bitmap background */
	BgSize_B8_256x256 =  ((1 << 14) | BIT(7) | (3 << 16)),  /*!< 256 x 256 pixel 8 bit bitmap background */
	BgSize_B8_512x256 =  ((2 << 14) | BIT(7) | (3 << 16)),  /*!< 512 x 256 pixel 8 bit bitmap background */
	BgSize_B8_512x512 =  ((3 << 14) | BIT(7) | (3 << 16)),  /*!< 512 x 512 pixel 8 bit bitmap background */
	BgSize_B8_1024x512 = (1 << 14) | (3 << 16),		    	/*!< 1024 x 512 pixel 8 bit bitmap background */
	BgSize_B8_512x1024 = (0) | (3 << 16),					/*!< 512 x 1024 pixel 8 bit bitmap background */

	BgSize_B16_128x128 = ((0 << 14) | BIT(7) | BIT(2) | (4 << 16)),  /*!< 128 x 128 pixel 16 bit bitmap background */
	BgSize_B16_256x256 = ((1 << 14) | BIT(7) | BIT(2) | (4 << 16)),  /*!< 256 x 256 pixel 16 bit bitmap background */
	BgSize_B16_512x256 = ((2 << 14) | BIT(7) | BIT(2) | (4 << 16)),  /*!< 512 x 512 pixel 16 bit bitmap background */
	BgSize_B16_512x512 = ((3 << 14) | BIT(7) | BIT(2) | (4 << 16)),  /*!< 1024 x 1024 pixel 16 bit bitmap background */

}BgSize;


#ifdef __cplusplus
extern "C" {
#endif

   //internally used for debug assertions
bool bgIsText(int id);
int bgInit_call(int layer, BgType type, BgSize size, int mapBase, int tileBase);
int bgInitSub_call(int layer, BgType type, BgSize size, int mapBase, int tileBase);


void bgUpdate(int id);

static inline
/*! \fn bgSetRotate(int id, int angle)
	\brief Sets the rotation angle of the specified background and updates the transform matrix
	\param id 
		background id returned from bgInit or bgInitSub
	\param angle 
		the angle of counter clockwise rotation (0 to 511)
*/
void bgSetRotate(int id, int angle)
{
   bgState[id].angle = angle & 0x1ff;

   bgUpdate(id);
}



static inline 
/*! \fn bgRotate(int id, int angle)
	\brief Rotates the background counter clockwise by the specified angle. (this rotation is cumulative)
	\param id 
		background id returned from bgInit or bgInitSub
	\param angle 
		the angle of counter clockwise rotation (0 to 511)
*/
void bgRotate(int id, int angle)
{
	sassert(!bgIsText(id), "Cannot Rotate a Text Background");
	
	bgSetRotate(id, angle + bgState[id].angle);
}

static inline 
/*! \fn void bgSet(int id, int angle, s32 sx, s32 sy, s32 scrollX, s32 scrollY, s32 rotCenterX, s32 rotCenterY)
	\brief Sets the rotation and scale of the background and update background control registers
	\param id 
		background id returned from bgInit or bgInitSub
	\param angle 
		the angle of counter clockwise rotation (0 to 511)
	\param sx
		the 24.8 bit fractional fixed point inverse horizontal scaling to apply
	\param sy
		the 24.8 bit fractional fixed point inverse vertical scaling to apply
   \param scrollX
		the 24.8 bit fractional fixed point horizontal scroll to apply
	\param scrollY
		the 24.8 bit fractional fixed point vertical scroll to apply
   \param rotCenterX
   the 24.8 bit fractional fixed point center of rotation x component
	\param rotCenterY
   the 24.8 bit fractional fixed point center of rotation y component

*/
void bgSet(int id, int angle, s32 sx, s32 sy, s32 scrollX, s32 scrollY, s32 rotCenterX, s32 rotCenterY)
{
   bgState[id].scaleX = sx;
	bgState[id].scaleY = sy;

   bgState[id].scrollX = scrollX;
	bgState[id].scrollY = scrollX;
   
  	bgState[id].centerX = rotCenterX;
	bgState[id].centerY = rotCenterY;

   bgSetRotate(id, angle);
}

static inline 
/*! \fn void bgSetRotateScale(int id, int angle, s32 sx, s32 sy)
	\brief Sets the rotation and scale of the background and update background control registers
	\param id 
		background id returned from bgInit or bgInitSub
	\param angle 
		the angle of counter clockwise rotation (0 to 511)
	\param sx
		the 24.8 bit fractional fixed point horizontal scaling to apply
	\param sy
		the 24.8 bit fractional fixed point vertical scaling to apply
*/
void bgSetRotateScale(int id, int angle, s32 sx, s32 sy)
{
   bgState[id].scaleX = sx;
	bgState[id].scaleY = sy;

   bgSetRotate(id, angle);
}
static inline 
/*! \fn bgSetScale(int id, s32 sx, s32 sy)
	\brief Sets the scale of the specified background
	\param id 
		background id returned from bgInit or bgInitSub
	\param sx
		the 24.8 bit fractional fixed point horizontal scaling to apply
	\param sy
		the 24.8 bit fractional fixed point vertical scaling to apply
*/
void bgSetScale(int id, s32 sx, s32 sy)
{
	sassert(!bgIsText(id), "Cannot Scale a Text Background");
	
	bgState[id].scaleX = sx;
	bgState[id].scaleY = sy;
   bgUpdate(id);
	
}

static inline 
/*! \fn bgInit(int layer, BgType type, BgSize size, int mapBase, int tileBase);
	\brief Initializes a background on the main display
		Sets up background control register with specified settings and defaults to 256 color mode
		for tiled backgrounds.  
		Sets the rotation/scale attributes for rot/ex rot backgrounds to 1:1 scale and 0 angle of rotation.
	\param layer 
		background hardware layer to init.  Must be 0 - 3
	\param type 
		the type of background to init 
	\param size
		the size of the background
	\param mapBase
		the 2k offset into vram the tile map will be placed OR
		the 16 offset into vram the bitmap data will be placed for bitmap backgrounds
	\param tileBase
		the 16k offset into vram the tile graphics data will be placed
	\return
		the background id to be used in the supporting functions
	\note 
		tileBase is unused for bitmap backgrounds
*/
int bgInit(int layer, BgType type, BgSize size, int mapBase, int tileBase)
{
    sassert(layer >= 0 && layer <= 3, "Only layers 0 - 3 are supported");
    sassert(tileBase >= 0 && tileBase <= 15, "Background tile base is out of range");
    sassert(mapBase >=0 && mapBase <= 31, "Background Map Base is out of range");
	sassert(layer != 0 || !video3DEnabled(), "Background 0 is currently being used for 3D display");
   sassert(layer > 1 || type == BgType_Text8bpp || type == BgType_Text4bpp, "Incorrect background type for mode");
   //sassert((size != BgSize_B8_512x1024 && size != BgSize_B8_1024x512) || videoGetMode() == 6, "Incorrect background type for mode"); 
	sassert(tileBase == 0 || type < BgType_Bmp8, "Tile base is unused for bitmaps.  Can be offset using mapBase * 16KB");
	sassert((mapBase == 0 || type != BgType_Bmp8) || (size != BgSize_B8_512x1024 && size != BgSize_B8_1024x512), "Large Bitmaps cannot be offset");

	return bgInit_call(layer, type, size, mapBase, tileBase);
}

static inline 
/*! \fn bgInitSub(int layer, BgType type, BgSize size, int mapBase, int tileBase);
	\brief Initializes a background on the sub display
		Sets up background control register with specified settings and defaults to 256 color mode
		for tiled backgrounds.  
		Sets the rotation/scale attributes for rot/ex rot backgrounds to 1:1 scale and 0 angle of rotation.
	\param layer 
		background hardware layer to init.  Must be 0 - 3
	\param type 
		the type of background to init 
	\param size
		the size of the background
	\param mapBase
		the 2k offset into vram the tile map will be placed OR
		the 16 offset into vram the bitmap data will be placed for bitmap backgrounds
	\param tileBase
		the 16k offset into vram the tile graphics data will be placed
	\return
		the background id to be used in the supporting functions
	\note 
		tileBase is unused for bitmap backgrounds
*/
int bgInitSub(int layer, BgType type, BgSize size, int mapBase, int tileBase)
{
    sassert(layer >= 0 && layer <= 3, "Only layers 0 - 3 are supported");
    sassert(tileBase >= 0 && tileBase <= 15, "Background tile base is out of range");
    sassert(mapBase >=0 && mapBase <= 31, "Background Map Base is out of range");
    sassert(layer > 1 || type == BgType_Text8bpp|| type == BgType_Text4bpp , "Incorrect background type for mode");
	sassert(tileBase == 0 || type < BgType_Bmp8, "Tile base is unused for bitmaps.  Can be offset using mapBase * 16KB");
	sassert((size != BgSize_B8_512x1024 && size != BgSize_B8_1024x512), "Sub Display has no large Bitmaps");

	return bgInitSub_call(layer, type, size, mapBase, tileBase);
}

static inline 
/*! \fn bgSetControlBits(int id, u16 bits)
	\brief allows direct access to background control for the chosen layer, returns a pointer
			to the current control bits
	\param id 
		background id returned from bgInit or bgInitSub
	\param bits
		sets the specified bits to set in the backgrounds control register
	\return
		a pointer to the appropriate background control register
*/
vuint16* bgSetControlBits(int id, u16 bits)
{
	*bgControl[id] |= bits; 
	return bgControl[id];
}

static inline 
/*! \fn bgClearControlBits(int id, u16 bits)
	\brief Clears the specified bits from the backgrounds control register
	\param id 
		background id returned from bgInit or bgInitSub
	\param bits
		sets the specified bits to clear in the backgrounds control register
*/
void  bgClearControlBits(int id, u16 bits)
{
	*bgControl[id] &= ~bits;
}

static inline

void bgWrapOn(int id)
{
   bgSetControlBits(id, BIT(13));
}

static inline

void bgWrapOff(int id)
{
   bgClearControlBits(id, BIT(13));
}
static inline 
/*! \fn bgSetPriority(int id, unsigned int priority)
	\brief Sets the background priority
	\param id 
		background id returned from bgInit or bgInitSub
	\param priority
		background priority (0-3), higher level priority will result 
		in background rendering on top of lower level
*/
void bgSetPriority(int id, unsigned int priority)
{
	sassert(priority < 4, "Priority must be less than 4");
	
	*bgControl[id] &= ~ 3;
	*bgControl[id] |= priority;
	
}

static inline 
/*! \fn  bgSetMapBase(int id, unsigned int base)
	\brief Sets the background map base
	\param id 
		background id returned from bgInit or bgInitSub
	\param base
		the 2k offset into vram for the backgrounds tile map or the 
		16k offset for bitmap graphics
*/
void bgSetMapBase(int id, unsigned int base)
{
	sassert(base <= 31, "Map base cannot exceed 31");

	*bgControl[id] &= ~ (31 << MAP_BASE_SHIFT);
	*bgControl[id] |= base << MAP_BASE_SHIFT;
}

static inline 
/*! \fn  bgSetTileBase(int id, unsigned int base)
	\brief Sets the background map base
	\param id 
		background id returned from bgInit or bgInitSub
	\param base
		the 16k offset into vram for the backgrounds tile map 
		ignored for bitmap graphics
*/
void bgSetTileBase(int id, unsigned int base)
{
	sassert(base <= 15, "Tile base cannot exceed 15");

	*bgControl[id] &= ~ (15 << TILE_BASE_SHIFT);
	*bgControl[id] |= base << TILE_BASE_SHIFT;
}


static inline 
/*! \fn  bgSetScrollf(int id, s32 x, s32 y)
	\brief Sets the scroll hardware to the specified location (fixed point)
	\param id 
		background id returned from bgInit or bgInitSub
	\param x
		the 8 bits fractional fixed point horizontal scroll
	\param y
		the 8 bits fractional fixed point vertical scroll
	\note
		while valid for text backgrounds the fractional part will be ignored

*/
void bgSetScrollf(int id, s32 x, s32 y)
{
	
	bgState[id].scrollX = x;
	bgState[id].scrollY = y;
   bgUpdate(id);
}

static inline 
/*! \fn  bgSetScroll(int id, int x, int y)
	\brief Sets the scroll hardware to the specified location 
	\param id 
		background id returned from bgInit or bgInitSub
	\param x
		the horizontal scroll
	\param y
		the vertical scroll
*/
void bgSetScroll(int id, int x, int y)
{
		bgSetScrollf(id, x << 8, y << 8);
}


static inline 
/*! \fn  bgMosaicEnable(int id)
	\brief Enables mosaic on the specified background
	\param id 
		background id returned from bgInit or bgInitSub
*/
void bgMosaicEnable(int id)
{
	*bgControl[id] |= BIT(6);
}

static inline 
/*! \fn bgMosaicDisable(int id)
	\brief Disables mosaic on the specified background
	\param id 
		background id returned from bgInit or bgInitSub
*/
void bgMosaicDisable(int id)
{
	*bgControl[id] &= ~BIT(6);
}

static inline 
/*! \fn bgSetMosaic(unsigned int dx, unsigned int dy)
	\brief Sets the horizontal and vertical mosaic values for all backgrounds
	\param dx
   horizontal mosaic value (between 0 and 15)
	\param dy
   vertical mosaic value (between 0 and 15)
*/
void bgSetMosaic(unsigned int dx, unsigned int dy)
{
   sassert(dx < 16 && dy < 16, "Mosaic range is 0 to 15");
	
	MOSAIC_CR &= ~0xFF;
	MOSAIC_CR |= dx | (dy << 4);
}

static inline
/*! \fn bgSetMosaicSub(unsigned int dx, unsigned int dy)
	\brief Sets the horizontal and vertical mosaic values for all backgrounds (Sub Display)
	\param dx
   horizontal mosaic value (between 0 and 15)
	\param dy
   vertical mosaic value (between 0 and 15)
*/
void bgSetMosaicSub(unsigned int dx, unsigned int dy)
{
   sassert(dx < 16 && dy < 16, "Mosaic range is 0 to 15");
	
	SUB_MOSAIC_CR &= ~0xFF;
	SUB_MOSAIC_CR |= dx | (dy << 4);

}

static inline 
/*! \fn bgGetPriority(int id)
	\brief Gets the background priority
	\param id 
		background id returned from bgInit or bgInitSub
	\return
		background priority 
*/
int bgGetPriority(int id)
{
	return *bgControl[id] & 3;
}

static inline 
/*! \fn bgGetMapBase(int id)
	\brief Gets the current map base for the supplied background
	\param id 
		background id returned from bgInit or bgInitSub
	\return
		background map base
	\note
		this is the integer offset of the base not a pointer to the map
*/
int bgGetMapBase(int id)
{
	return (*bgControl[id] >> MAP_BASE_SHIFT) & 31;
}

static inline 
/*! \fn bgGetTileBase(int id)
	\brief Gets the background tile base
	\param id 
		background id returned from bgInit or bgInitSub
	\return
		background tile base
*/
int bgGetTileBase(int id)
{
	return (*bgControl[id] >> TILE_BASE_SHIFT) & 31;
}

static inline 
/*! \fn bgGetMapPtr(int id) 
	\brief Gets a pointer to the background map
	\param id 
		background id returned from bgInit or bgInitSub
	\return
		a pointer to the map 
*/
void* bgGetMapPtr(int id) 
{
	return (id < 4) ? ((void*)BG_MAP_RAM(bgGetMapBase(id))) : ((void*)BG_MAP_RAM_SUB(bgGetMapBase(id)));
}

static inline 
/*! \fn bgGetGfxPtr(int id) 
	\brief Gets a pointer to the background graphics
	\param id 
		background id returned from bgInit or bgInitSub
	\return
		a pointer to the tile graphics or bitmap graphics
*/
void* bgGetGfxPtr(int id) 
{
	if(bgState[id].type < BgType_Bmp8)
		return (id < 4) ? (void*)(BG_TILE_RAM(bgGetTileBase(id))) : ((void*)BG_TILE_RAM_SUB(bgGetTileBase(id)));
	else
		return (id < 4) ? (void*)(BG_GFX + 0x800 * (bgGetMapBase(id))) : (void*)(BG_GFX_SUB + 0x800 * (bgGetMapBase(id)));
}


static inline 
/*! \fn bgScrollf(int id, s32 dx, s32 dy)
	\brief Scrolls the background by the specified relative values (fixed point)
	\param id 
		background id returned from bgInit or bgInitSub
	\param dx
		the 8 bits fractional fixed point horizontal scroll
	\param dy
		the 8 bits fractional fixed point vertical scroll
	\note
		while valid for text backgrounds the fractional part will be ignored

*/
void bgScrollf(int id, s32 dx, s32 dy)
{	
	bgSetScrollf(id, bgState[id].scrollX + dx, bgState[id].scrollY + dy);
}

static inline 
/*! \fn bgScroll(int id, int dx, int dy)
	\brief Scrolls the background by the specified relative values
	\param id 
		background id returned from bgInit or bgInitSub
	\param dx
		horizontal scroll
	\param dy
		vertical scroll
	\note
		while valid for text backgrounds the fractional part will be ignored

*/
void bgScroll(int id, int dx, int dy)
{	
	bgSetScrollf(id, dx << 8, dy << 8);
}

static inline 
/*! \fn bgShow(int id)
	\brief Shows the current background via the display control register
	\param id 
		background id returned from bgInit or bgInitSub
*/
void bgShow(int id)
{
	if(id < 4)
		videoBgEnable(id);
	else
		videoBgEnableSub(id & 3);
}

static inline 
/*! \fn bgHide(int id)
	\brief Hides the current background via the display control register
	\param id 
		background id returned from bgInit or bgInitSub
*/
void bgHide(int id)
{
	if(id < 4)
		videoBgDisable(id);
	else
		videoBgDisableSub(id & 3);
}

static inline 
/*! \fn bgSetCenterf(int id, s32 x, s32 y)
	\brief Sets the center of rotation for the supplied background (fixed point)
	\param id 
		background id returned from bgInit or bgInitSub
	\param x
		the 8 bits fractional center x
	\param y
		the 8 bits fractional center y
	\note
		while valid for text backgrounds the fractional part will be ignored

*/
void bgSetCenterf(int id, s32 x, s32 y)
{
	sassert(!bgIsText(id), "Text Backgrounds have no Center of Rotation");
		 
 	bgState[id].centerX = x;
	bgState[id].centerY = y;

   bgUpdate(id);
}

static inline 
/*! \fn bgSetCenter(int id, int x, int y)
	\brief Sets the center of rotation for the supplied background 
	\param id 
		background id returned from bgInit or bgInitSub
	\param x
		center x
	\param y
		center y

*/
void bgSetCenter(int id, int x, int y)
{
	bgSetCenterf(id, x << 8, y << 8);
}

#ifdef __cplusplus
}
#endif



#define BG_TILE_BASE(base) ((base) << 2)
#define BG_MAP_BASE(base)  ((base) << 8)
#define BG_BMP_BASE(base)  ((base) << 8)

#define BG_MOSAIC_ENABLE	0x40
#define BG_COLOR_256		0x80
#define BG_COLOR_16			0x00

#define CHAR_BASE_BLOCK(n)			(((n)*0x4000)+ 0x06000000)
#define CHAR_BASE_BLOCK_SUB(n)		(((n)*0x4000)+ 0x06200000)
#define SCREEN_BASE_BLOCK(n)		(((n)*0x800) + 0x06000000)
#define SCREEN_BASE_BLOCK_SUB(n)	(((n)*0x800) + 0x06200000)

#define CHAR_SHIFT        2
#define SCREEN_SHIFT      8
#define TEXTBG_SIZE_256x256    0x0
#define TEXTBG_SIZE_256x512    0x8000
#define TEXTBG_SIZE_512x256    0x4000
#define TEXTBG_SIZE_512x512    0xC000

#define ROTBG_SIZE_128x128    0x0
#define ROTBG_SIZE_256x256    0x4000
#define ROTBG_SIZE_512x512    0x8000
#define ROTBG_SIZE_1024x1024  0xC000

#define	BGCTRL				( (vu16*)0x4000008)
#define	REG_BG0CNT		(*(vu16*)0x4000008)
#define	REG_BG1CNT		(*(vu16*)0x400000A)
#define	REG_BG2CNT		(*(vu16*)0x400000C)
#define	REG_BG3CNT		(*(vu16*)0x400000E)

#define	BGCTRL_SUB				( (vu16*)0x4001008)
#define	REG_BG0CNT_SUB		(*(vu16*)0x4001008)
#define	REG_BG1CNT_SUB		(*(vu16*)0x400100A)
#define	REG_BG2CNT_SUB		(*(vu16*)0x400100C)
#define	REG_BG3CNT_SUB		(*(vu16*)0x400100E)

#define BG_256_COLOR   (BIT(7))
#define BG_16_COLOR    (0)

#define BG_MOSAIC_ON   (BIT(6))
#define BG_MOSAIC_OFF  (0)

#define BG_PRIORITY(n) (n)
#define BG_PRIORITY_0  (0)
#define BG_PRIORITY_1  (1)
#define BG_PRIORITY_2  (2)
#define BG_PRIORITY_3  (3)

#define BG_WRAP_OFF    (0)
#define BG_WRAP_ON     (1 << 13)

#define BG_PALETTE_SLOT0 0
#define BG_PALETTE_SLOT1 0
#define BG_PALETTE_SLOT2 BIT(13)
#define BG_PALETTE_SLOT3 BIT(13)

#define BG_OFFSET ((bg_scroll *)(0x04000010))
#define BG_OFFSET_SUB ((bg_scroll *)(0x04001010))

#define	REG_BGOFFSETS	( (vu16*)0x4000010)
#define	REG_BG0HOFS		(*(vu16*)0x4000010)
#define	REG_BG0VOFS		(*(vu16*)0x4000012)
#define	REG_BG1HOFS		(*(vu16*)0x4000014)
#define	REG_BG1VOFS		(*(vu16*)0x4000016)
#define	REG_BG2HOFS		(*(vu16*)0x4000018)
#define	REG_BG2VOFS		(*(vu16*)0x400001A)
#define	REG_BG3HOFS		(*(vu16*)0x400001C)
#define	REG_BG3VOFS		(*(vu16*)0x400001E)

#define	REG_BG2PA		(*(vu16*)0x4000020)
#define	REG_BG2PB		(*(vu16*)0x4000022)
#define	REG_BG2PC		(*(vu16*)0x4000024)
#define	REG_BG2PD		(*(vu16*)0x4000026)

#define	REG_BG2X		(*(vu32*)0x4000028)
#define	REG_BG2X_L		(*(vu16*)0x4000028)
#define	REG_BG2X_H		(*(vu16*)0x400002A)

#define	REG_BG2Y		(*(vu32*)0x400002C)
#define	REG_BG2Y_L		(*(vu16*)0x400002C)
#define	REG_BG2Y_H		(*(vu16*)0x400002E)

#define	REG_BG3PA		(*(vu16*)0x4000030)
#define	REG_BG3PB		(*(vu16*)0x4000032)
#define	REG_BG3PC		(*(vu16*)0x4000034)
#define	REG_BG3PD		(*(vu16*)0x4000036)

#define	REG_BG3X		(*(vu32*)0x4000038)
#define	REG_BG3X_L		(*(vu16*)0x4000038)
#define	REG_BG3X_H		(*(vu16*)0x400003A)
#define	REG_BG3Y		(*(vu32*)0x400003C)
#define	REG_BG3Y_L		(*(vu16*)0x400003C)
#define	REG_BG3Y_H		(*(vu16*)0x400003E)

#define	REG_BGOFFSETS_SUB	( (vu16*)0x4001010)
#define	REG_BG0HOFS_SUB		(*(vu16*)0x4001010)
#define	REG_BG0VOFS_SUB		(*(vu16*)0x4001012)
#define	REG_BG1HOFS_SUB		(*(vu16*)0x4001014)
#define	REG_BG1VOFS_SUB		(*(vu16*)0x4001016)
#define	REG_BG2HOFS_SUB		(*(vu16*)0x4001018)
#define	REG_BG2VOFS_SUB		(*(vu16*)0x400101A)
#define	REG_BG3HOFS_SUB		(*(vu16*)0x400101C)
#define	REG_BG3VOFS_SUB		(*(vu16*)0x400101E)

#define	REG_BG2PA_SUB		(*(vu16*)0x4001020)
#define	REG_BG2PB_SUB		(*(vu16*)0x4001022)
#define	REG_BG2PC_SUB		(*(vu16*)0x4001024)
#define	REG_BG2PD_SUB		(*(vu16*)0x4001026)

#define	REG_BG2X_SUB		(*(vu32*)0x4001028)
#define	REG_BG2X_SUB_L		(*(vu16*)0x4001028)
#define	REG_BG2X_SUB_H		(*(vu16*)0x400102A)
#define	REG_BG2Y_SUB		(*(vu32*)0x400102C)
#define	REG_BG2Y_SUB_L		(*(vu16*)0x400102C)
#define	REG_BG2Y_SUB_H		(*(vu16*)0x400102E)

#define	REG_BG3PA_SUB		(*(vu16*)0x4001030)
#define	REG_BG3PB_SUB		(*(vu16*)0x4001032)
#define	REG_BG3PC_SUB		(*(vu16*)0x4001034)
#define	REG_BG3PD_SUB		(*(vu16*)0x4001036)

#define	REG_BG3X_SUB		(*(vu32*)0x4001038)
#define	REG_BG3X_L_SUB		(*(vu16*)0x4001038)
#define	REG_BG3X_H_SUB		(*(vu16*)0x400103A)
#define	REG_BG3Y_SUB		(*(vu32*)0x400103C)
#define	REG_BG3Y_L_SUB		(*(vu16*)0x400103C)
#define	REG_BG3Y_H_SUB		(*(vu16*)0x400103E)

#define	BG_32x32       (0 << 14)
#define	BG_64x32       (1 << 14)
#define	BG_32x64       (2 << 14)
#define	BG_64x64      (3 << 14)

#define	BG_RS_16x16     (0 << 14)
#define	BG_RS_32x32     (1 << 14)
#define	BG_RS_64x64     (2 << 14)
#define	BG_RS_128x128   (3 << 14)

#define	BG_BMP8_128x128   ((0 << 14) | BG_256_COLOR)
#define	BG_BMP8_256x256   ((1 << 14) | BG_256_COLOR)
#define	BG_BMP8_512x256   ((2 << 14) | BG_256_COLOR)
#define	BG_BMP8_512x512   ((3 << 14) | BG_256_COLOR)
#define	BG_BMP8_1024x512  BIT(14)
#define	BG_BMP8_512x1024  0

#define	BG_BMP16_128x128  ((0 << 14) | BG_256_COLOR | BIT(2))
#define	BG_BMP16_256x256  ((1 << 14) | BG_256_COLOR | BIT(2))
#define	BG_BMP16_512x256  ((2 << 14) | BG_256_COLOR | BIT(2))
#define	BG_BMP16_512x512  ((3 << 14) | BG_256_COLOR | BIT(2))

#endif
