/*---------------------------------------------------------------------------------

	sprite.h -- definitions for DS sprites

	Copyright (C) 2007
		Liran Nuna	(LiraNuna)
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
/*! \file sprite.h
    \brief nds sprite functionality.
*/

#ifndef _libnds_sprite_h_
#define _libnds_sprite_h_

#ifndef ARM9
#error Sprites are only available on the ARM9
#endif

#include "nds/ndstypes.h"
#include "nds/arm9/video.h"
#include "nds/memory.h"
#include "nds/system.h"

// Sprite control defines

// Attribute 0 consists of 8 bits of Y plus the following flags:
#define ATTR0_NORMAL			(0<<8)
#define ATTR0_ROTSCALE			(1<<8)
#define ATTR0_DISABLED			(2<<8)
#define ATTR0_ROTSCALE_DOUBLE	(3<<8)

#define ATTR0_TYPE_NORMAL		(0<<10)
#define ATTR0_TYPE_BLENDED		(1<<10)
#define ATTR0_TYPE_WINDOWED		(2<<10)
#define ATTR0_BMP				(3<<10)

#define ATTR0_MOSAIC			(1<<12)

#define ATTR0_COLOR_16		(0<<13) //16 color in tile mode...16 bit in bitmap mode
#define ATTR0_COLOR_256		(1<<13)

#define ATTR0_SQUARE		(0<<14)
#define ATTR0_WIDE			(1<<14)
#define ATTR0_TALL			(2<<14)

#define OBJ_Y(m)			((m)&0x00ff)

// Atribute 1 consists of 9 bits of X plus the following flags:
#define ATTR1_ROTDATA(n)      ((n)<<9)  // note: overlaps with flip flags
#define ATTR1_FLIP_X          (1<<12)
#define ATTR1_FLIP_Y          (1<<13)
#define ATTR1_SIZE_8          (0<<14)
#define ATTR1_SIZE_16         (1<<14)
#define ATTR1_SIZE_32         (2<<14)
#define ATTR1_SIZE_64         (3<<14)

#define OBJ_X(m)			((m)&0x01ff)

// Atribute 2 consists of the following:
#define ATTR2_PRIORITY(n)     ((n)<<10)
#define ATTR2_PALETTE(n)      ((n)<<12)
#define ATTR2_ALPHA(n)		  ((n)<<12)

/** \brief The blending mode of the sprite */
typedef enum
{
	OBJMODE_NORMAL,		/**< No special mode is on - Normal sprite state. */
	OBJMODE_BLENDED,	/**< Color blending is on - Sprite can use HW blending features. */
	OBJMODE_WINDOWED,	/**< Sprite can be seen only inside the sprite window. */
	OBJMODE_BITMAP,		/**< Sprite is not using tiles - per pixel image data. */

} ObjBlendMode;

/** \brief The shape of the sprite */
typedef enum {
	OBJSHAPE_SQUARE,	/**< Sprite shape is NxN (Height == Width). */
	OBJSHAPE_WIDE,		/**< Sprite shape is NxM with N > M (Height < Width). */
	OBJSHAPE_TALL,		/**< Sprite shape is NxM with N < M (Height > Width). */
	OBJSHAPE_FORBIDDEN,	/**< Sprite shape is undefined. */
} ObjShape;

/** \brief The size of the sprite */
typedef enum {
	OBJSIZE_8,		/**< Major sprite size is 8px. */
	OBJSIZE_16,		/**< Major sprite size is 16px. */
	OBJSIZE_32,		/**< Major sprite size is 32px. */
	OBJSIZE_64,		/**< Major sprite size is 64px. */
} ObjSize;

/** \brief The color mode of the sprite */
typedef enum {
	OBJCOLOR_16,		/**< sprite has 16 colors. */
	OBJCOLOR_256,		/**< sprite has 256 colors. */
} ObjColMode;

/** \brief The priority of the sprite */
typedef enum {
	OBJPRIORITY_0,		/**< sprite priority level 0 - highest. */
	OBJPRIORITY_1,		/**< sprite priority level 1. */
	OBJPRIORITY_2,		/**< sprite priority level 2. */
	OBJPRIORITY_3,		/**< sprite priority level 3 - lowest. */
} ObjPriority;

//! A bitfield of sprite attribute goodness...ugly to look at but not so bad to use.
typedef union SpriteEntry
{
	struct
	{
		struct
		{
			u16 y							:8;	/**< Sprite Y position. */
			union
			{
				struct
				{
					u8 						:1;
					bool isHidden 			:1;	/**< Sprite is hidden (isRotoscale cleared). */
					u8						:6;
				};
				struct
				{
					bool isRotateScale		:1;	/**< Sprite uses affine parameters if set. */
					bool isSizeDouble		:1;	/**< Sprite bounds is doubled (isRotoscale set). */
					ObjBlendMode blendMode	:2;	/**< Sprite object mode. */
					bool isMosaic			:1;	/**< Enables mosaic effect if set. */
					ObjColMode colorMode	:1;	/**< Sprite color mode. */
					ObjShape shape			:2;	/**< Sprite shape. */
				};
			};
		};

		union {
			struct {
				u16 x						:9;	/**< Sprite X position. */
				u8 							:7;
			};
			struct {
				u8							:8;
				union {
					struct {
						u8					:4;
						bool hFlip			:1; /**< Flip sprite horizontally (isRotoscale cleared). */
						bool vFlip			:1; /**< Flip sprite vertically (isRotoscale cleared).*/
						u8					:2;
					};
					struct {
						u8					:1;
						u8 rotationIndex	:5; /**< Affine parameter number to use (isRotoscale set). */
						ObjSize size		:2; /**< Sprite size. */
					};
				};
			};
		};

		struct
		{
			u16 gfxIndex					:10;/**< Upper-left tile index. */
			ObjPriority priority			:2;	/**< Sprite priority. */
			u8 palette						:4;	/**< Sprite palette to use in paletted color modes. */
		};

		u16 attribute3;							/* Unused! Four of those are used as a sprite rotation matrice */
	};

	struct {
		uint16 attribute[3];
		uint16 filler;
	};

} SpriteEntry, * pSpriteEntry;



//! A sprite rotation entry.
typedef struct SpriteRotation
{
	uint16 filler1[3];	/* Unused! Filler for the sprite entry attributes which overlap these */
	int16 hdx;			/**< The change in x per horizontal pixel */

	uint16 filler2[3];	/* Unused! Filler for the sprite entry attributes which overlap these */
	int16 vdx;			/**< The change in y per horizontal pixel */

	uint16 filler3[3];	/* Unused! Filler for the sprite entry attributes which overlap these */
	int16 hdy;			/**< The change in x per vertical pixel */

	uint16 filler4[3];	/* Unused! Filler for the sprite entry attributes which overlap these */
	int16 vdy;			/**< The change in y per vertical pixel */
} SpriteRotation, * pSpriteRotation;


//! maximum number of sprites per engine available.
#define SPRITE_COUNT 128
//! maximum number of affine matrices per engine available.
#define MATRIX_COUNT 32


//is this union still used?
typedef union OAMTable {
	SpriteEntry oamBuffer[SPRITE_COUNT];
	SpriteRotation matrixBuffer[MATRIX_COUNT];
} OAMTable;



//! Enumerates all sizes supported by the 2D engine.
typedef enum {
   SpriteSize_8x8   = (OBJSIZE_8 << 14) | (OBJSHAPE_SQUARE << 12) | (8*8>>5),		//!< 8x8
   SpriteSize_16x16 = (OBJSIZE_16 << 14) | (OBJSHAPE_SQUARE << 12) | (16*16>>5),	//!< 16x16
   SpriteSize_32x32 = (OBJSIZE_32 << 14) | (OBJSHAPE_SQUARE << 12) | (32*32>>5),	//!< 32x32
   SpriteSize_64x64 = (OBJSIZE_64 << 14) | (OBJSHAPE_SQUARE << 12) | (64*64>>5),	//!< 64x64

   SpriteSize_16x8  = (OBJSIZE_8 << 14)  | (OBJSHAPE_WIDE << 12) | (16*8>>5),		//!< 16x8
   SpriteSize_32x8  = (OBJSIZE_16 << 14) | (OBJSHAPE_WIDE << 12) | (32*8>>5),		//!< 32x8
   SpriteSize_32x16 = (OBJSIZE_32 << 14) | (OBJSHAPE_WIDE << 12) | (32*16>>5),		//!< 32x16
   SpriteSize_64x32 = (OBJSIZE_64 << 14) | (OBJSHAPE_WIDE << 12) | (64*32>>5),		//!< 64x32

   SpriteSize_8x16  = (OBJSIZE_8 << 14)  | (OBJSHAPE_TALL << 12) | (8*16>>5),		//!< 8x16
   SpriteSize_8x32  = (OBJSIZE_16 << 14) | (OBJSHAPE_TALL << 12) | (8*32>>5),		//!< 8x32
   SpriteSize_16x32 = (OBJSIZE_32 << 14) | (OBJSHAPE_TALL << 12) | (16*32>>5),		//!< 16x32
   SpriteSize_32x64 = (OBJSIZE_64 << 14) | (OBJSHAPE_TALL << 12) | (32*64>>5)		//!< 32x64

}SpriteSize;

#define SPRITE_SIZE_SHAPE(size) (((size) >> 12) & 0x3)
#define SPRITE_SIZE_SIZE(size)  (((size) >> 14) & 0x3)
#define SPRITE_SIZE_PIXELS(size) (((size) & 0xFFF) << 5)

//! Graphics memory layout options.
typedef enum{
   SpriteMapping_1D_32 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_32 | (0 << 28) | 0,	/**< 1D tile mapping 32 byte boundary between offset */
   SpriteMapping_1D_64 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_64 | (1 << 28) | 1,	/**< 1D tile mapping 64 byte boundary between offset */
   SpriteMapping_1D_128 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | (2 << 28) | 2,	/**< 1D tile mapping 128 byte boundary between offset */
   SpriteMapping_1D_256 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_256 | (3 << 28) | 3,	/**< 1D tile mapping 256 byte boundary between offset */
   SpriteMapping_2D = DISPLAY_SPR_2D | (4 << 28),									/**< 2D tile mapping 32 byte boundary between offset */
   SpriteMapping_Bmp_1D_128 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | DISPLAY_SPR_1D_BMP |DISPLAY_SPR_1D_BMP_SIZE_128 | (5 << 28) | 2,/**< 1D bitmap mapping 128 byte boundary between offset */
   SpriteMapping_Bmp_1D_256 = DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_256 | DISPLAY_SPR_1D_BMP |DISPLAY_SPR_1D_BMP_SIZE_256 | (6 << 28) | 3,/**< 1D bitmap mapping 256 byte boundary between offset */
   SpriteMapping_Bmp_2D_128 = DISPLAY_SPR_2D | DISPLAY_SPR_2D_BMP_128 | (7 << 28) | 2,	/**< 2D bitmap mapping 128 pixels wide bitmap */
   SpriteMapping_Bmp_2D_256 = DISPLAY_SPR_2D | DISPLAY_SPR_2D_BMP_256 | (8 << 28) | 3	/**< 2D bitmap mapping 256 pixels wide bitmap */
}SpriteMapping;

//! Color formats for sprite graphics.
typedef enum{
   SpriteColorFormat_16Color = OBJCOLOR_16,/**< 16 colors per sprite*/
   SpriteColorFormat_256Color = OBJCOLOR_256,/**< 256 colors per sprite*/
   SpriteColorFormat_Bmp = OBJMODE_BITMAP/**< 16-bit sprites*/
}SpriteColorFormat;


typedef struct AllocHeader
{
   u16 nextFree;
   u16 size;
}AllocHeader;


//this struct can be made smaller by rearanging the members.
/**	\brief Holds the state for a 2D sprite engine.
	There are two of these objects, oamMain and oamSub and these must be passed in to all oam functions.
*/
typedef struct OamState
{
	int gfxOffsetStep;			/**< The distance between tiles as 2^gfxOffsetStep */
	s16 firstFree;				/**< pointer to the first free block of tiles */
	AllocHeader *allocBuffer;	/**< array, allocation buffer for graphics allocation */
	s16 allocBufferSize;		/**< current size of the allocation buffer */
	union
	{
		SpriteEntry *oamMemory;				/**< pointer to shadow oam memory */
		SpriteRotation *oamRotationMemory;	/**< pointer to shadow oam memory for rotation */
	};
	SpriteMapping spriteMapping;			//!< the mapping of the oam.
}OamState;

#ifdef __cplusplus
extern "C" {
#endif

//!oamMain an object representing the main 2D engine
extern OamState oamMain;
//!oamSub an object representing the sub 2D engine
extern OamState oamSub;

/**
*    \brief convert a VRAM address to an oam offset
*    \param oam must be: &oamMain or &oamSub
*    \param offset the video memory address of the sprite graphics (not an offset)
*/
unsigned int oamGfxPtrToOffset(OamState *oam, const void* offset);

/**
*    \brief Initializes the 2D sprite engine  In order to mix tiled and bitmap sprites
            use SpriteMapping_Bmp_1D_128 or SpriteMapping_Bmp_1D_256.  This will set mapping for both
            to 1D and give same sized boundaries so the sprite gfx allocation will function.  VBlank IRQ must
            be enabled for this function to work.
*    \param oam must be: &oamMain or &oamSub
*    \param mapping the mapping mode
*    \param extPalette if true the engine sets up extended palettes for 8bpp sprites
*/
void oamInit(OamState* oam, SpriteMapping mapping, bool extPalette);

/**
*    \brief Disables sprite rendering
*    \param oam must be: &oamMain or &oamSub
*/
void oamDisable(OamState* oam );

/**
*    \brief Enables sprite rendering
*    \param oam must be: &oamMain or &oamSub
*/
void oamEnable(OamState* oam );

/**
*    \brief translates an oam offset into a video ram address
*    \param oam must be: &oamMain or &oamSub
*    \param gfxOffsetIndex the index to compute
*    \return the address in vram corresponding to the supplied offset
*/
u16* oamGetGfxPtr(OamState* oam, int gfxOffsetIndex);

/**
*    \brief Allocates graphics memory for the supplied sprite attributes
*    \param oam must be: &oamMain or &oamSub
*    \param size the size of the sprite to allocate
*    \param colorFormat the color format of the sprite
*    \return the address in vram of the allocated sprite
*/
u16* oamAllocateGfx(OamState *oam, SpriteSize size, SpriteColorFormat colorFormat);

/**
*    \brief free vram memory obtained with oamAllocateGfx.
*    \param oam must be: &oamMain or &oamSub
*    \param gfxOffset a vram offset obtained from oamAllocateGfx
*/
void oamFreeGfx(OamState *oam, const void* gfxOffset);

static inline
/**
*    \brief sets engine A global sprite mosaic
*    \param dx (0-15) horizontal mosaic value
*    \param dy (0-15) horizontal mosaic value
*/
void oamSetMosaic(unsigned int dx, unsigned int dy) {
	sassert(dx < 16 && dy < 16, "Mosaic range must be 0 to 15");

	mosaicShadow = ( mosaicShadow & 0x00ff) | (dx << 8)| (dy << 12);
	REG_MOSAIC = mosaicShadow;
}

static inline
/**
*    \brief sets engine B global sprite mosaic
*    \param dx (0-15) horizontal mosaic value
*    \param dy (0-15) horizontal mosaic value
*/
void oamSetMosaicSub(unsigned int dx, unsigned int dy){
	sassert(dx < 16 && dy < 16, "Mosaic range must be 0 to 15");

	mosaicShadowSub = ( mosaicShadowSub & 0x00ff) | (dx << 8)| (dy << 12);
	REG_MOSAIC_SUB = mosaicShadowSub;
}

/**
*    \brief sets an oam entry to the supplied values
*    \param oam must be: &oamMain or &oamSub
*    \param id the oam number to be set [0 - 127]
*    \param x the x location of the sprite in pixels
*    \param y the y location of the sprite in pixels
*	 \param priority The sprite priority (0 to 3)
*    \param palette_alpha the palette number for 4bpp and 8bpp (extended palette mode), or the alpha value for bitmap sprites (bitmap sprites must specify a value > 0 to display) [0-15]
*    \param size the size of the sprite
*    \param format the color format of the sprite
*    \param gfxOffset the video memory address of the sprite graphics (not an offset)
*    \param affineIndex affine index to use (if < 0 or > 31 the sprite will be unrotated)
*    \param sizeDouble if affineIndex >= 0 this will be used to double the sprite size for rotation
*    \param hide if non zero (true) the sprite will be hidden
*    \param vflip flip the sprite vertically
*    \param hflip flip the sprite horizontally
*	 \param mosaic if true mosaic will be applied to the sprite
*/
void oamSet(OamState* oam, int id,  int x, int y, int priority, int palette_alpha, SpriteSize size, SpriteColorFormat format, const void* gfxOffset, int affineIndex, bool sizeDouble, bool hide, bool hflip, bool vflip, bool mosaic);

static inline
/**
*    \brief sets an oam entry to the supplied x,y position
*    \param oam must be: &oamMain or &oamSub
*    \param id the oam number to be set [0-127]
*    \param x the x location of the sprite in pixels
*    \param y the y location of the sprite in pixels
*/
void oamSetXY(OamState* oam, int id, int x, int y)
{
    sassert(oam == &oamMain || oam == &oamSub, "oamSetXY() oam must be &oamMain or &oamSub");
    sassert(id >= 0 && id < SPRITE_COUNT, "oamSetXY() index is out of bounds, must be 0-127");
    oam->oamMemory[id].x = x;
    oam->oamMemory[id].y = y;
}

static inline
/**
*    \brief sets an oam entry to the supplied priority
*    \param oam must be: &oamMain or &oamSub
*    \param id the oam number to be set [0-127]
*    \param priority The sprite priority [0-3]
*/
void oamSetPriority(OamState* oam, int id, int priority)
{
    sassert(oam == &oamMain || oam == &oamSub, "oamSetPriority() oam must be &oamMain or &oamSub");
    sassert(id >= 0 && id < SPRITE_COUNT, "oamSetPriority() index is out of bounds, must be 0-127");
    sassert(priority >= 0 && priority < 4, "oamSetPriority() priority is out of bounds, must be 0-3");
    oam->oamMemory[id].priority = (ObjPriority)priority;
}

static inline
/**
*    \brief sets a paletted oam entry to the supplied palette
*    \param oam must be: &oamMain or &oamSub
*    \param id the oam number to be set [0-127]
*    \param palette the palette number for 4bpp and 8bpp (extended palette mode) sprites [0-15]
*/
void oamSetPalette(OamState* oam, int id, int palette)
{
    sassert(oam == &oamMain || oam == &oamSub, "oamSetPalette() oam must be &oamMain or &oamSub");
    sassert(id >= 0 && id < SPRITE_COUNT, "oamSetPalette() index is out of bounds, must be 0-127");
    sassert(palette >= 0 && palette < 16, "oamSetPalette() palette is out of bounds, must be 0-15");
    sassert(oam->oamMemory[id].blendMode != (ObjBlendMode)SpriteColorFormat_Bmp,
            "oamSetPalette() cannot set palette on a bitmapped sprite");
    oam->oamMemory[id].palette = palette;
}

static inline
/**
*    \brief sets a bitmapped oam entry to the supplied transparency
*    \param oam must be: &oamMain or &oamSub
*    \param id the oam number to be set [0-127]
*    \param alpha the alpha value for bitmap sprites (bitmap sprites must specify a value > 0 to display) [0-15]
*/
void oamSetAlpha(OamState* oam, int id, int alpha)
{
    sassert(oam == &oamMain || oam == &oamSub, "oamSetAlpha() oam must be &oamMain or &oamSub");
    sassert(id >= 0 && id < SPRITE_COUNT, "oamSetAlpha() index is out of bounds, must be 0-127");
    sassert(alpha >= 0 && alpha < 16, "oamSetAlpha() alpha is out of bounds, must be 0-15");
    sassert(oam->oamMemory[id].blendMode == (ObjBlendMode)SpriteColorFormat_Bmp,
            "oamSetAlpha() cannot set alpha on a paletted sprite");
    oam->oamMemory[id].palette = alpha;
}

static inline
/**
*    \brief sets an oam entry to the supplied shape/size/pointer
*    \param oam must be: &oamMain or &oamSub
*    \param id the oam number to be set [0-127]
*    \param size the size of the sprite
*    \param format the color format of the sprite
*    \param gfxOffset the video memory address of the sprite graphics (not an offset)
*/
void oamSetGfx(OamState* oam, int id, SpriteSize size, SpriteColorFormat format, const void* gfxOffset)
{
    sassert(oam == &oamMain || oam == &oamSub, "oamSetGfx() oam must be &oamMain or &oamSub");
    sassert(id >= 0 && id < SPRITE_COUNT, "oamSetGfx() index is out of bounds, must be 0-127");
    oam->oamMemory[id].shape    = (ObjShape)SPRITE_SIZE_SHAPE(size);
    oam->oamMemory[id].size     = (ObjSize)SPRITE_SIZE_SIZE(size);
    oam->oamMemory[id].gfxIndex = oamGfxPtrToOffset(oam, gfxOffset);

    if(format != SpriteColorFormat_Bmp)
        oam->oamMemory[id].colorMode = (ObjColMode)format;
    else
    {
        oam->oamMemory[id].blendMode = (ObjBlendMode)format;
        oam->oamMemory[id].colorMode = (ObjColMode)0;
    }
}

static inline
/**
*    \brief sets an oam entry to the supplied affine index
*    \param oam must be: &oamMain or &oamSub
*    \param id the oam number to be set [0-127]
*    \param affineIndex affine index to use (if < 0 or > 31 the sprite will be unrotated)
*    \param sizeDouble if affineIndex >= 0 and < 32 this will be used to double the sprite size for rotation
*/
void oamSetAffineIndex(OamState* oam, int id, int affineIndex, bool sizeDouble)
{
    sassert(oam == &oamMain || oam == &oamSub, "oamSetAffineIndex() oam must be &oamMain or &oamSub");
    sassert(id >= 0 && id < SPRITE_COUNT, "oamSetAffineIndex() index is out of bounds, must be 0-127");

    if(affineIndex >= 0 && affineIndex < 32)
    {
        oam->oamMemory[id].rotationIndex = affineIndex;
        oam->oamMemory[id].isSizeDouble  = sizeDouble;
        oam->oamMemory[id].isRotateScale = true;
    }
    else
    {
        oam->oamMemory[id].isSizeDouble  = false;
        oam->oamMemory[id].isRotateScale = false;
    }
}

static inline
/**
*    \brief sets an oam entry to the supplied hidden state
*    \param oam must be: &oamMain or &oamSub
*    \param id the oam number to be set [0-127]
*    \param hide if non zero (true) the sprite will be hidden
*/
void oamSetHidden(OamState* oam, int id, bool hide)
{
    sassert(oam == &oamMain || oam == &oamSub, "oamSetHidden() oam must be &oamMain or &oamSub");
    sassert(id >= 0 && id < SPRITE_COUNT, "oamSetHidden() index is out of bounds, must be 0-127");
    sassert(!oam->oamMemory[id].isRotateScale, "oamSetHidden() cannot set hide on a RotateScale sprite");
    oam->oamMemory[id].isHidden = hide ? true : false;
}

static inline
/**
*    \brief sets an oam entry to the supplied flipping
*    \param oam must be: &oamMain or &oamSub
*    \param id the oam number to be set [0-127]
*    \param hflip flip the sprite horizontally
*    \param vflip flip the sprite vertically
*/
void oamSetFlip(OamState* oam, int id, bool hflip, bool vflip)
{
    sassert(oam == &oamMain || oam == &oamSub, "oamSetFlip() oam must be &oamMain or &oamSub");
    sassert(id >= 0 && id < SPRITE_COUNT, "oamSetFlip() index is out of bounds, must be 0-127");
    sassert(!oam->oamMemory[id].isRotateScale, "oamSetFlip() cannot set flip on a RotateScale sprite");
    oam->oamMemory[id].hFlip = hflip ? true : false;
    oam->oamMemory[id].vFlip = vflip ? true : false;
}

static inline
/**
*    \brief sets an oam entry to enable or disable mosaic
*    \param oam must be: &oamMain or &oamSub
*    \param id the oam number to be set [0-127]
*    \param mosaic if true mosaic will be applied to the sprite
*/
void oamSetMosaicEnabled(OamState* oam, int id, bool mosaic)
{
    sassert(oam == &oamMain || oam == &oamSub, "oamSetMosaicEnabled() oam must be &oamMain or &oamSub");
    sassert(id >= 0 && id < SPRITE_COUNT, "oamSetMosaicEnabled() index is out of bounds, must be 0-127");
    oam->oamMemory[id].isMosaic = mosaic ? true : false;
}

/**
*    \brief Hides the sprites in the supplied range: if count is zero all 128 sprites will be hidden
*    \param oam must be: &oamMain or &oamSub
*    \param start The first index to clear
*    \param count The number of sprites to clear
*/
void oamClear(OamState *oam, int start, int count);


static inline
/**
    \brief Hides a single sprite.

    \param oam      the oam engine, must be &oamMain or &oamSub.
    \param index    the index of the sprite, must be 0-127.
*/
void oamClearSprite(OamState *oam, int index)
{
    sassert(index >= 0 && index < SPRITE_COUNT, "oamClearSprite() index is out of bounds, must be 0-127");
    oam->oamMemory[index].attribute[0] = ATTR0_DISABLED;
}


/**
*    \brief causes oam memory to be updated...must be called during vblank if using oam api
*    \param oam must be: &oamMain or &oamSub
*/
void oamUpdate(OamState* oam);

/**
*    \brief sets the specified rotation scale entry
*    \param oam must be: &oamMain or &oamSub
*    \param rotId the rotation entry to set
*    \param angle the ccw angle to rotate [-32768 - 32767]
*    \param sx the inverse scale factor in the x direction
*    \param sy the inverse scale factor in the y direction
*/
void oamRotateScale(OamState* oam, int rotId, int angle, int sx, int sy);

static inline
/**
* \brief allows you to directly sets the affine transformation matrix.
*
* with this, you have more freedom to set the matrix, but it might be more difficult to use if
* you're not used to affine transformation matrix. this will erase the previous matrix stored at rotId.
*
* \param oam	The oam engine, must be &oamMain or &oamSub.
* \param rotId	The id of the rotscale item you want to change, must be 0-31.
* \param hdx	The change in x per horizontal pixel.
* \param hdy	The change in y per horizontal pixel.
* \param vdx	The change in x per vertical pixel.
* \param vdy	The change in y per vertical pixel.
*/
void oamAffineTransformation(OamState* oam, int rotId, int hdx, int hdy, int vdx, int vdy)
{
	sassert(rotId >= 0 && rotId < 32, "oamAffineTransformation() rotId is out of bounds, must be 0-31");

	oam->oamRotationMemory[rotId].hdx = hdx;
	oam->oamRotationMemory[rotId].vdx = vdx;
	oam->oamRotationMemory[rotId].hdy = hdy;
	oam->oamRotationMemory[rotId].vdy = vdy;
}

/**
	\brief determines the number of fragments in the allocation engine
	\param oam must be: &oamMain or &oamSub
	\return the number of fragments.
*/
int oamCountFragments(OamState *oam);



void oamAllocReset(OamState *oam);


#ifdef __cplusplus
}
#endif

#endif // _libnds_sprite_h_
