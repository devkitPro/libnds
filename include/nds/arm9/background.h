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
    Background control is provided via an API or Direct register access.  Usually these methods can be mixed.  However,
    scrolling, scaling, and rotation will have unexpected results if API and Direct Register access is mixed.  Effort is
    being directed at ensuring the API can access all hardware features without limitation.

    - \ref background_api_group "API Components"
    - \ref background_register_group "Register Access Components"

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





#ifndef _libnds_background_h_
#define _libnds_background_h_

#include <nds/ndstypes.h>
#include <nds/arm9/video.h>
#include <nds/arm9/sassert.h>
#include <nds/memory.h>
#include <nds/dma.h>




/*! \defgroup background_register_group "Background Register Access"
\brief Background related register definitions and bit defines
@{
*/

/*!	\brief register overlay for scroll registers */
typedef struct bg_scroll {
	u16 x;/*!< x scroll*/
	u16 y;/*!< y scroll*/
} bg_scroll;

/*!	\brief register overlay for affine matrix registers */
typedef struct bg_transform {
    s16 hdx; //!< The change in x per horizontal pixel.
    s16 vdx; //!< The change in x per vertical pixel.
    s16 hdy; //!< The change in y per horizontal pixel.
    s16 vdy; //!< The change in x per vertical pixel.
    s32 dx;  //!< map x value which corresponds to the screen origin
    s32 dy;  //!< map y value which corresponds to the screen origin
} bg_transform;

/*!	\brief register overlay for background attribute registers */
typedef struct bg_attribute {
    u16 control[4];/*!< Background control registers*/
    bg_scroll scroll[4];/*!< Background scroll registers*/
    bg_transform bg2_rotation;/*!< Background 2 affine matrix*/
    bg_transform bg3_rotation;/*!< Background 3 affine matrix*/
} bg_attribute;


/*! \brief The shift to apply to map base when storing it in a background control register */
#define MAP_BASE_SHIFT 8
/*! \brief The shift to apply to tile base when storing it in a background control register */
#define TILE_BASE_SHIFT 2
/*! \brief Macro to set the tile base in background control */
#define BG_TILE_BASE(base) ((base) << TILE_BASE_SHIFT)
/*! \brief Macro to set the map base in background control */
#define BG_MAP_BASE(base)  ((base) << MAP_BASE_SHIFT)
/*! \brief Macro to set the graphics base in background control */
#define BG_BMP_BASE(base)  ((base) << MAP_BASE_SHIFT)
/*! \brief Macro to set the priority in background control */
#define BG_PRIORITY(n) (n)

/*! \brief Macro to set the palette entry of a 16 bit tile index */
#define TILE_PALETTE(n) ((n)<<12)
/*! \brief The horizontal flip bit for a 16 bit tile index */
#define TILE_FLIP_H BIT(10)
/*! \brief The vertical flip bit for a 16 bit tile index  */
#define TILE_FLIP_V BIT(11)

/*! \brief Overlay for 8-bit tile map entries */
typedef struct TileMapEntry8 {
  u8 index;
} TileMapEntry8;

/*! \brief Overlay for 16-bit tile map entries */
typedef struct TileMapEntry16 {
  u16 index:10;
  u16 hflip:1;
  u16 vflip:1;
  u16 palette:4;
} PACKED TileMapEntry16;

/*!	\brief Bit defines for the background control registers */
typedef enum
{
   BG_32x32    =  (0 << 14), /*!< \brief 32 x 32 tile text background */
   BG_64x32    =  (1 << 14), /*!< \brief 64 x 32 tile text background */
   BG_32x64    =  (2 << 14), /*!< \brief 32 x 64 tile text background */
   BG_64x64    =  (3 << 14), /*!< \brief 64 x 64 tile text background */

   BG_RS_16x16   =  (0 << 14),  /*!< \brief 16 x 16 tile affine (rotation & scale) background */
   BG_RS_32x32   =  (1 << 14),  /*!< \brief 32 x 32 tile affine (rotation & scale) background */
   BG_RS_64x64   =  (2 << 14),  /*!< \brief 64 x 64 tile affine (rotation & scale) background */
   BG_RS_128x128 =  (3 << 14),  /*!< \brief 128 x 128 tile affine (rotation & scale) background */

   BG_BMP8_128x128  = ((0 << 14) | BIT(7)), /*!< \brief 128x128 pixel 8-bit bitmapped background */
   BG_BMP8_256x256  = ((1 << 14) | BIT(7)), /*!< \brief 256x256 pixel 8-bit bitmapped background */
   BG_BMP8_512x256  = ((2 << 14) | BIT(7)), /*!< \brief 512x256 pixel 8-bit bitmapped background */
   BG_BMP8_512x512  = ((3 << 14) | BIT(7)), /*!< \brief 512 pixel 8-bit bitmapped background */
   BG_BMP8_1024x512 = BIT(14),				/*!< \brief 1024x512 pixel 8-bit Large bitmapped background (Mode 6 of main engine only)*/
   BG_BMP8_512x1024 = 0,					/*!< \brief 512x1024 pixel 8-bit Large bitmapped background (Mode 6 of main engine only)*/

   BG_BMP16_128x128  = ((0 << 14) | BIT(7) | BIT(2)),/*!< \brief 128x128 pixel 16-bit bitmapped background */
   BG_BMP16_256x256  = ((1 << 14) | BIT(7) | BIT(2)),/*!< \brief 256x256 pixel 16-bit bitmapped background */
   BG_BMP16_512x256  = ((2 << 14) | BIT(7) | BIT(2)),/*!< \brief 512x256 pixel 16-bit bitmapped background */
   BG_BMP16_512x512  = ((3 << 14) | BIT(7) | BIT(2)),/*!< \brief 512x512 pixel 16-bit bitmapped background */

   BG_MOSAIC_ON   = (BIT(6)),	/*!< \brief mosaic enable */
   BG_MOSAIC_OFF  = (0),		/*!< \brief mosaic disable*/

   BG_PRIORITY_0  = (0),/*!< \brief Lower priority will be rendered on top */
   BG_PRIORITY_1  = (1),/*!< \brief Lower priority will be rendered on top */
   BG_PRIORITY_2  = (2),/*!< \brief Lower priority will be rendered on top */
   BG_PRIORITY_3  = (3),/*!< \brief Lower priority will be rendered on top */

   BG_WRAP_OFF    = (0),		/*!< \brief Disable wrapping (no effect on text backgrounds...always wrapped) */
   BG_WRAP_ON     = (1 << 13),	/*!< \brief Enable wrapping (no effect on text backgrounds...always wrapped) */

   BG_PALETTE_SLOT0 = 0, /*!< \brief Use slot 0 of extended palettes */
   BG_PALETTE_SLOT1 = 0, /*!< \brief Use slot 1 of extended palettes */
   BG_PALETTE_SLOT2 = BIT(13), /*!< \brief Use slot 2 of extended palettes */
   BG_PALETTE_SLOT3 = BIT(13), /*!< \brief Use slot 3 of extended palettes */

   BG_COLOR_256		=	0x80,/*!< \brief 256 color text background */
   BG_COLOR_16		=	0x00/*!< \brief 16x16 color text background */

}BackgroundControl;


/*@}*/

/*! \defgroup main_display_registers "Main Engine"
\brief Main Engine Background registers
\ingroup background_register_group
*/

/*@{*/

/*! \brief Overlay for main screen background attributes.
	Setting the properties of this struct directly sets background registers.
*/
#define BACKGROUND           (*((bg_attribute *)0x04000008))

/*! \brief Overlay for main screen background scroll registers.
	Setting the properties of this struct directly sets background registers.
*/
#define BG_OFFSET ((bg_scroll *)(0x04000010))

/*! \brief A macro which returns a u16* pointer to background map ram (Main Engine) */
#define BG_MAP_RAM(base)		((u16*)(((base)*0x800) + 0x06000000))
/*! \brief A macro which returns a u16* pointer to background tile ram (Main Engine) */
#define BG_TILE_RAM(base)		((u16*)(((base)*0x4000) + 0x06000000))
/*! \brief A macro which returns a u16* pointer to background graphics memory ram (Main Engine) */
#define BG_BMP_RAM(base)		((u16*)(((base)*0x4000) + 0x06000000))

/* A macro which returns a u16* pointer to background tile ram (Main Engine)
use BG_TILE_RAM unless you really can't */
#define CHAR_BASE_BLOCK(n)			(((n)*0x4000)+ 0x06000000)
/* A macro which returns a u16* pointer to background Map ram (Main Engine)
use BG_MAP_RAM unless you really can't*/
#define SCREEN_BASE_BLOCK(n)		(((n)*0x800) + 0x06000000)


/*! \brief Access to all Main screen background control registers via: BGCTRL[x]
<A HREF="http://nocash.emubase.de/gbatek.htm#dsvideobgmodescontrol">GBATEK Reference</A>
*/
#define	BGCTRL			( (vu16*)0x4000008)

/*! \brief Background 0 Control register (main engine)
<A HREF="http://nocash.emubase.de/gbatek.htm#dsvideobgmodescontrol">GBATEK Reference</A>
*/
#define	REG_BG0CNT		(*(vu16*)0x4000008)

/*! \brief Background 1 Control register (main engine)
<A HREF="http://nocash.emubase.de/gbatek.htm#dsvideobgmodescontrol">GBATEK Reference</A>
*/
#define	REG_BG1CNT		(*(vu16*)0x400000A)

/*! \brief Background 2 Control register (main engine)
<A HREF="http://nocash.emubase.de/gbatek.htm#dsvideobgmodescontrol">GBATEK Reference</A>
*/
#define	REG_BG2CNT		(*(vu16*)0x400000C)

/*! \brief Background 3 Control register (main engine)
<A HREF="http://nocash.emubase.de/gbatek.htm#dsvideobgmodescontrol">GBATEK Reference</A>
*/
#define	REG_BG3CNT		(*(vu16*)0x400000E)


#define	REG_BGOFFSETS	( (vu16*)0x4000010)
/*! \brief Background 0 horizontal scroll register (main engine)*/
#define	REG_BG0HOFS		(*(vu16*)0x4000010)
/*! \brief Background 0 vertical scroll register (main engine)*/
#define	REG_BG0VOFS		(*(vu16*)0x4000012)
/*! \brief Background 1 horizontal scroll register (main engine)*/
#define	REG_BG1HOFS		(*(vu16*)0x4000014)
/*! \brief Background 1 vertical scroll register (main engine)*/
#define	REG_BG1VOFS		(*(vu16*)0x4000016)
/*! \brief Background 2 horizontal scroll register (main engine)*/
#define	REG_BG2HOFS		(*(vu16*)0x4000018)
/*! \brief Background 2 vertical scroll register (main engine)*/
#define	REG_BG2VOFS		(*(vu16*)0x400001A)
/*! \brief Background 3 horizontal scroll register (main engine)*/
#define	REG_BG3HOFS		(*(vu16*)0x400001C)
/*! \brief Background 3 vertical scroll register (main engine)*/
#define	REG_BG3VOFS		(*(vu16*)0x400001E)
/*! \brief Background 2 Affine transform (main engine)*/
#define	REG_BG2PA		(*(vs16*)0x4000020)
/*! \brief Background 2 Affine transform (main engine)*/
#define	REG_BG2PB		(*(vs16*)0x4000022)
/*! \brief Background 2 Affine transform (main engine)*/
#define	REG_BG2PC		(*(vs16*)0x4000024)
/*! \brief Background 2 Affine transform (main engine)*/
#define	REG_BG2PD		(*(vs16*)0x4000026)

/*! \brief Background 2 Screen Offset (main engine)*/
#define	REG_BG2X		(*(vs32*)0x4000028)

/*! \brief Background 2 Screen Offset (main engine)*/
#define	REG_BG2Y		(*(vs32*)0x400002C)
/*! \brief Background 3 Affine transform (main engine)*/
#define	REG_BG3PA		(*(vs16*)0x4000030)
/*! \brief Background 3 Affine transform (main engine)*/
#define	REG_BG3PB		(*(vs16*)0x4000032)
/*! \brief Background 3 Affine transform (main engine)*/
#define	REG_BG3PC		(*(vs16*)0x4000034)
/*! \brief Background 3 Affine transform (main engine)*/
#define	REG_BG3PD		(*(vs16*)0x4000036)

/*! \brief Background 3 Screen Offset (main engine)*/
#define	REG_BG3X		(*(vs32*)0x4000038)
/*! \brief Background 3 Screen Offset (main engine)*/
#define	REG_BG3Y		(*(vs32*)0x400003C)
/*@}*/

/*! \defgroup sub_display_registers "Sub Engine"
\brief Sub Engine Background registers
\ingroup background_register_group
*/

/*@{*/

/*! \brief Overlay for sub screen background attributes.  Setting the properties of this
struct directly sets background registers.
*/
#define BACKGROUND_SUB       (*((bg_attribute *)0x04001008))

/*! \brief Overlay for sub screen background scroll registers.  Setting the properties of this
struct directly sets background registers.
*/
#define BG_OFFSET_SUB ((bg_scroll *)(0x04001010))

/*! \brief A macro which returns a u16* pointer to background map ram (Sub Engine) */
#define BG_MAP_RAM_SUB(base)	((u16*)(((base)*0x800) + 0x06200000))
/*! \brief A macro which returns a u16* pointer to background tile ram (Sub Engine) */
#define BG_TILE_RAM_SUB(base)	((u16*)(((base)*0x4000) + 0x06200000))
/*! \brief A macro which returns a u16* pointer to background graphics ram (Sub Engine) */
#define BG_BMP_RAM_SUB(base)	((u16*)(((base)*0x4000) + 0x06200000))

/* A macro which returns a u16* pointer to background Map ram (Sub Engine)
use BG_MAP_RAM_SUB unless you really can't */
#define SCREEN_BASE_BLOCK_SUB(n)	(((n)*0x800) + 0x06200000)
/* A macro which returns a u16* pointer to background tile ram (Sub Engine)
use BG_TILE_RAM_SUB unless you really can't */
#define CHAR_BASE_BLOCK_SUB(n)		(((n)*0x4000)+ 0x06200000)


/*! \brief Access to all Sub screen background control registers via: BGCTRL[x]
<A HREF="http://nocash.emubase.de/gbatek.htm#dsvideobgmodescontrol">GBATEK Reference</A>
*/
#define	BGCTRL_SUB				( (vu16*)0x4001008)

/*! \brief Background 0 Control register (sub engine)
<A HREF="http://nocash.emubase.de/gbatek.htm#dsvideobgmodescontrol">GBATEK Reference</A>
*/
#define	REG_BG0CNT_SUB		(*(vu16*)0x4001008)

/*! \brief Background 1 Control register (sub engine)
<A HREF="http://nocash.emubase.de/gbatek.htm#dsvideobgmodescontrol">GBATEK Reference</A>
*/
#define	REG_BG1CNT_SUB		(*(vu16*)0x400100A)

/*! \brief Background 2 Control register (sub engine)
<A HREF="http://nocash.emubase.de/gbatek.htm#dsvideobgmodescontrol">GBATEK Reference</A>
*/
#define	REG_BG2CNT_SUB		(*(vu16*)0x400100C)

/*! \brief Background 3 Control register (sub engine)
<A HREF="http://nocash.emubase.de/gbatek.htm#dsvideobgmodescontrol">GBATEK Reference</A>
*/
#define	REG_BG3CNT_SUB		(*(vu16*)0x400100E)


#define	REG_BGOFFSETS_SUB	( (vu16*)0x4001010)
/*! \brief Background 0 horizontal scroll register (sub engine)*/
#define	REG_BG0HOFS_SUB		(*(vu16*)0x4001010)
/*! \brief Background 0 vertical scroll register (sub engine)*/
#define	REG_BG0VOFS_SUB		(*(vu16*)0x4001012)
/*! \brief Background 1 horizontal scroll register (sub engine)*/
#define	REG_BG1HOFS_SUB		(*(vu16*)0x4001014)
/*! \brief Background 1 vertical scroll register (sub engine)*/
#define	REG_BG1VOFS_SUB		(*(vu16*)0x4001016)
/*! \brief Background 2 horizontal scroll register (sub engine)*/
#define	REG_BG2HOFS_SUB		(*(vu16*)0x4001018)
/*! \brief Background 2 vertical scroll register (sub engine)*/
#define	REG_BG2VOFS_SUB		(*(vu16*)0x400101A)
/*! \brief Background 3 horizontal scroll register (sub engine)*/
#define	REG_BG3HOFS_SUB		(*(vu16*)0x400101C)
/*! \brief Background 3 vertical scroll register (sub engine)*/
#define	REG_BG3VOFS_SUB		(*(vu16*)0x400101E)

/*! \brief Background 2 Affine transform (sub engine)*/
#define	REG_BG2PA_SUB		(*(vs16*)0x4001020)
/*! \brief Background 2 Affine transform (sub engine)*/
#define	REG_BG2PB_SUB		(*(vs16*)0x4001022)
/*! \brief Background 2 Affine transform (sub engine)*/
#define	REG_BG2PC_SUB		(*(vs16*)0x4001024)
/*! \brief Background 2 Affine transform (sub engine)*/
#define	REG_BG2PD_SUB		(*(vs16*)0x4001026)

/*! \brief Background 2 Screen Offset (sub engine)*/
#define	REG_BG2X_SUB		(*(vs32*)0x4001028)
/*! \brief Background 2 Screen Offset (sub engine)*/
#define	REG_BG2Y_SUB		(*(vs32*)0x400102C)

/*! \brief Background 3 Affine transform (sub engine)*/
#define	REG_BG3PA_SUB		(*(vs16*)0x4001030)
/*! \brief Background 3 Affine transform (sub engine)*/
#define	REG_BG3PB_SUB		(*(vs16*)0x4001032)
/*! \brief Background 3 Affine transform (sub engine)*/
#define	REG_BG3PC_SUB		(*(vs16*)0x4001034)
/*! \brief Background 3 Affine transform (sub engine)*/
#define	REG_BG3PD_SUB		(*(vs16*)0x4001036)

/*! \brief Background 3 Screen Offset (sub engine)*/
#define	REG_BG3X_SUB		(*(vs32*)0x4001038)
/*! \brief Background 3 Screen Offset (sub engine)*/
#define	REG_BG3Y_SUB		(*(vs32*)0x400103C)
/*@}*/


/*! \defgroup background_api_group "Background API Access"
@{
*/

//background state
typedef struct BgState
{
	int angle;
	s32 centerX;
	s32 centerY;
	s32 scaleX;
	s32 scaleY;
	s32 scrollX;
	s32 scrollY;
	int size;//currently isn't used for anything...
	int type;
	bool dirty;
}BgState;


//id -> register look up tables
extern vuint16* bgControl[8];
extern bg_scroll* bgScrollTable[8];
extern bg_transform* bgTransform[8];
extern BgState bgState[8];

//! Allowed background types, used in bgInit and bgInitSub.
typedef enum
{
	BgType_Text8bpp, 	//!< 8bpp Tiled background with 16 bit tile indexes and no allowed rotation or scaling
	BgType_Text4bpp, 	//!< 4bpp Tiled background with 16 bit tile indexes and no allowed rotation or scaling
	BgType_Rotation, 	//!< Tiled background with 8 bit tile indexes Can be scaled and rotated
	BgType_ExRotation, 	//!< Tiled background with 16 bit tile indexes Can be scaled and rotated
	BgType_Bmp8, 		//!< Bitmap background with 8 bit color values which index into a 256 color palette
	BgType_Bmp16 		//!< Bitmap background with 16 bit color values of the form aBBBBBGGGGGRRRRR (if 'a' is set the pixel will be rendered...if not the pixel will be transparent)
}BgType;


/**
 * \brief Allowed background Sizes
 * The lower 16 bits of these defines can be used directly to set the background control register bits
 * \ingroup api_group
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


/*!	\brief Must be called once per frame to update scroll/scale/and rotation of backgrounds.
*/
void bgUpdate(void);


static inline
/*!	\brief Sets the rotation angle of the specified background and updates the transform matrix.
	\param id
		background id returned from bgInit or bgInitSub
	\param angle
		the angle of counter clockwise rotation (0 to 511)
*/
void bgSetRotate(int id, int angle)
{
   bgState[id].angle = angle;
   bgState[id].dirty = true;
}


static inline
/*! \brief Rotates the background counter clockwise by the specified angle. (this rotation is cumulative)
	\param id
		background id returned from bgInit or bgInitSub
	\param angle
		the angle of counter clockwise rotation (-32768 to 32767)
*/
void bgRotate(int id, int angle)
{
	sassert(!bgIsText(id), "Cannot Rotate a Text Background");

	bgSetRotate(id, angle + bgState[id].angle);
}


static inline
/*!	\brief Sets the rotation and scale of the background and update background control registers
	\param id
		background id returned from bgInit or bgInitSub
	\param angle
		the angle of counter clockwise rotation (-32768 to 32767)
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
	bgState[id].scrollY = scrollY;

  	bgState[id].centerX = rotCenterX;
	bgState[id].centerY = rotCenterY;

	bgState[id].angle = angle;

	bgState[id].dirty = true;
}

static inline
/*!	\brief Sets the rotation and scale of the background and update background control registers
	\param id
		background id returned from bgInit or bgInitSub
	\param angle
		the angle of counter clockwise rotation (-32768 to 32767)
	\param sx
		the 24.8 bit fractional fixed point horizontal scaling to apply
	\param sy
		the 24.8 bit fractional fixed point vertical scaling to apply
*/
void bgSetRotateScale(int id, int angle, s32 sx, s32 sy)
{
	bgState[id].scaleX = sx;
	bgState[id].scaleY = sy;
	bgState[id].angle = angle;

	bgState[id].dirty = true;
}


static inline
/*!	\brief Sets the scale of the specified background
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

	bgState[id].dirty = true;
}


static inline
/*!	\brief Initializes a background on the main display
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
		the 2k offset into vram the tile map will be placed
		<br>--OR--<br>
		the 16k offset into vram the bitmap data will be placed for bitmap backgrounds
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
/*!	\brief Initializes a background on the sub display
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
		the 2k offset into vram the tile map will be placed
		<br>--OR--<br>
		the 16k offset into vram the bitmap data will be placed for bitmap backgrounds
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
/*!	\brief allows direct access to background control for the chosen layer, returns a pointer
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
	sassert(id >= 0 && id <= 7, "bgSetControlBits(), id must be the number returned from bgInit or bgInitSub");
	*bgControl[id] |= bits;
	return bgControl[id];
}

static inline
/*!	\brief Clears the specified bits from the backgrounds control register
	\param id
		background id returned from bgInit or bgInitSub
	\param bits
		sets the specified bits to clear in the backgrounds control register
*/
void bgClearControlBits(int id, u16 bits)
{
	sassert(id >= 0 && id <= 7, "bgClearControlBits(), id must be the number returned from bgInit or bgInitSub");
	*bgControl[id] &= ~bits;
}

static inline
/*!	\brief turns wrap on for a background.
	has no effect on text backgrounds, which are always wrapped.
	\param id background id returned from bgInit or bgInitSub
*/
void bgWrapOn(int id)
{
	bgSetControlBits(id, BIT(13));
}

static inline
/*!	\brief turns wrap off for a background.
	has no effect on text backgrounds, which are always wrapped.
	\param id background id returned from bgInit or bgInitSub
*/
void bgWrapOff(int id)
{
	bgClearControlBits(id, BIT(13));
}


static inline
/*!	\brief Sets the background priority
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
/*!	\brief Sets the background map base
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
/*!	\brief Sets the background map base
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
/*!	\brief Sets the scroll hardware to the specified location (fixed point)
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

	bgState[id].dirty = true;
}

static inline
/*!	\brief Sets the scroll hardware to the specified location
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
/*!	\brief Enables mosaic on the specified background
	\param id background id returned from bgInit or bgInitSub
*/
void bgMosaicEnable(int id)
{
	*bgControl[id] |= BIT(6);
}

static inline
/*!	\brief Disables mosaic on the specified background
	\param id background id returned from bgInit or bgInitSub
*/
void bgMosaicDisable(int id)
{
	*bgControl[id] &= ~BIT(6);
}

static inline
/*!	\brief Sets the horizontal and vertical mosaic values for all backgrounds
	\param dx horizontal mosaic value (between 0 and 15)
	\param dy vertical mosaic value (between 0 and 15)
*/
void bgSetMosaic(unsigned int dx, unsigned int dy)
{
	sassert(dx < 16 && dy < 16, "Mosaic range is 0 to 15");

	mosaicShadow = ( mosaicShadow & 0xff00) | (dx | (dy << 4));
	REG_MOSAIC = mosaicShadow;
}

static inline
/*!	\brief Sets the horizontal and vertical mosaic values for all backgrounds (Sub Display)
	\param dx horizontal mosaic value (between 0 and 15)
	\param dy vertical mosaic value (between 0 and 15)
*/
void bgSetMosaicSub(unsigned int dx, unsigned int dy)
{
	sassert(dx < 16 && dy < 16, "Mosaic range is 0 to 15");

	mosaicShadowSub = ( mosaicShadowSub & 0xff00) | (dx | (dy << 4));
	REG_MOSAIC_SUB = mosaicShadowSub;
}

static inline
/*!	\brief Gets the background priority
	\param id background id returned from bgInit or bgInitSub
	\return background priority
*/
int bgGetPriority(int id)
{
	return *bgControl[id] & 3;
}

static inline
/*!	\brief Gets the current map base for the supplied background
	\param id background id returned from bgInit or bgInitSub
	\return background map base
	\note this is the integer offset of the base not a pointer to the map
*/
int bgGetMapBase(int id)
{
	return (*bgControl[id] >> MAP_BASE_SHIFT) & 31;
}

static inline
/*!	\brief Gets the background tile base
	\param id background id returned from bgInit or bgInitSub
	\return background tile base
*/
int bgGetTileBase(int id)
{
	return (*bgControl[id] >> TILE_BASE_SHIFT) & 15;
}

static inline
/*!	\brief Gets a pointer to the background map
	\param id background id returned from bgInit or bgInitSub
	\return A pointer to the map
*/
u16* bgGetMapPtr(int id)
{
	return (id < 4) ? ((u16*)BG_MAP_RAM(bgGetMapBase(id))) : ((u16*)BG_MAP_RAM_SUB(bgGetMapBase(id)));
}

static inline
/*!	\brief Gets a pointer to the background graphics
	\param id background id returned from bgInit or bgInitSub
	\return A pointer to the tile graphics or bitmap graphics
*/
u16* bgGetGfxPtr(int id)
{
	if(bgState[id].type < BgType_Bmp8)
		return (id < 4) ? (u16*)(BG_TILE_RAM(bgGetTileBase(id))) : ((u16*)BG_TILE_RAM_SUB(bgGetTileBase(id)));
	else
		return (id < 4) ? (u16*)(BG_GFX + 0x2000 * (bgGetMapBase(id))) : (u16*)(BG_GFX_SUB + 0x2000 * (bgGetMapBase(id)));
}


static inline
/*!	\brief Scrolls the background by the specified relative values (fixed point)
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
/*!	\brief Scrolls the background by the specified relative values
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
	bgScrollf(id, dx << 8, dy << 8);
}

static inline
/*!	\brief Shows the current background via the display control register
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
/*!	\brief Hides the current background via the display control register
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
/*!	\brief Sets the center of rotation for the supplied background (fixed point)
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

	bgState[id].dirty = true;
}

static inline
/*!	\brief Sets the center of rotation for the supplied background
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


static inline
/**	\brief directly sets the affine matrix and scroll registers of a background.

	With this, you have more freedom to set the matrix, but it might be more difficult to use if
	you're not used to affine transformation matrix.
	This will ignore (but not erase) any values set using the bg rotating, scaling or center functions.

	\param id		The id returned by bgInit or bgInitSub.
	\param hdx		The change in x per horizontal pixel.
	\param vdx		The change in x per vertical pixel.
	\param hdy		The change in y per horizontal pixel.
	\param vdy		The change in y per vertical pixel.
	\param scrollx	The horizontal scroll/offset value of the background.
	\param scrolly	The vertical scroll/offset value of the background.
*/
void bgSetAffineMatrixScroll(int id, int hdx, int vdx, int hdy, int vdy, int scrollx, int scrolly)
{
	sassert(!bgIsText(id), "Text Backgrounds have no affine matrix and scroll registers.");

	bgTransform[id]->hdx = hdx;
	bgTransform[id]->vdx = vdx;
	bgTransform[id]->hdy = hdy;
	bgTransform[id]->vdy = vdy;

	bgTransform[id]->dx = scrollx;
	bgTransform[id]->dy = scrolly;

	bgState[id].dirty = false;
}

static inline
/** \brief  Enable extended palettes
  
  When extended palettes are enabled all tiled backgrounds which utilize 
  16 bit map entries will use extended palettes.  Everything else will continue
  to use standard palette memory.

  Each tile on the screen may chose one of 16 256-color palettes.  Each background 
  has its own set of 16 palettes meaning you can have 4*16*256 colors on screen 

  Each background uses 8K of palette memory starting at the base of the vram bank
  you allocate (which bank is up to you within limits, see the vram usage table
  to determine which banks can be mapped for textures).  These 8K blocks are often
  refered to as "slots" with each background getting its own slot.  

   By default, Background 0 uses slot 0 ... Background 3 uses slot 3.  It is possible
   to assign Background 0 to slot 2 and Background 1 to slot 3 (only these two are configurable)
  
  For more information: <a href="http://nocash.emubase.de/gbatek.htm#dsvideoextendedpalettes">gbatek</a>
 */
void bgExtPaletteEnable(void) 
{ 
  REG_DISPCNT |= DISPLAY_BG_EXT_PALETTE; 
}

static inline
/** \brief  Enable extended palettes
 
  When extended palettes are enabled all tiled backgrounds which utilize 
  16 bit map entries will use extended palettes.  Everything else will continue
  to use standard palette memory.

  Each tile on the screen may chose one of 16 256-color palettes.  Each background 
  has its own set of 16 palettes meaning you can have 4*16*256 colors on screen 

  Each background uses 8K of palette memory starting at the base of the vram bank
  you allocate (which bank is up to you within limits, see the vram usage table
  to determine which banks can be mapped for textures).  These 8K blocks are often
  refered to as "slots" with each background getting its own slot.  

   By default, Background 0 uses slot 0 ... Background 3 uses slot 3.  It is possible
   to assign Background 0 to slot 2 and Background 1 to slot 3 (only these two are configurable)
  
  For more information: <a href="http://nocash.emubase.de/gbatek.htm#dsvideoextendedpalettes">gbatek</a>

*/
void bgExtPaletteEnableSub(void) 
{ 
  REG_DISPCNT_SUB |= DISPLAY_BG_EXT_PALETTE; 
}

static inline
/** \brief  Disable extended palettes
 
  When extended palettes are enabled all tiled backgrounds which utilize 
  16 bit map entries will use extended palettes.  Everything else will continue
  to use standard palette memory.

  Each tile on the screen may chose one of 16 256-color palettes.  Each background 
  has its own set of 16 palettes meaning you can have 4*16*256 colors on screen 

  Each background uses 8K of palette memory starting at the base of the vram bank
  you allocate (which bank is up to you within limits, see the vram usage table
  to determine which banks can be mapped for textures).  These 8K blocks are often
  refered to as "slots" with each background getting its own slot.  

   By default, Background 0 uses slot 0 ... Background 3 uses slot 3.  It is possible
   to assign Background 0 to slot 2 and Background 1 to slot 3 (only these two are configurable)
  
  For more information: <a href="http://nocash.emubase.de/gbatek.htm#dsvideoextendedpalettes">gbatek</a>

*/
void bgExtPaletteDisable(void) 
{ 
  REG_DISPCNT &= ~DISPLAY_BG_EXT_PALETTE; 
}

static inline
/** \brief  Disable extended palettes
 
  When extended palettes are enabled all tiled backgrounds which utilize 
  16 bit map entries will use extended palettes.  Everything else will continue
  to use standard palette memory.

  Each tile on the screen may chose one of 16 256-color palettes.  Each background 
  has its own set of 16 palettes meaning you can have 4*16*256 colors on screen 

  Each background uses 8K of palette memory starting at the base of the vram bank
  you allocate (which bank is up to you within limits, see the vram usage table
  to determine which banks can be mapped for textures).  These 8K blocks are often
  refered to as "slots" with each background getting its own slot.  

   By default, Background 0 uses slot 0 ... Background 3 uses slot 3.  It is possible
   to assign Background 0 to slot 2 and Background 1 to slot 3 (only these two are configurable)
  
  For more information: <a href="http://nocash.emubase.de/gbatek.htm#dsvideoextendedpalettes">gbatek</a>

*/
void bgExtPaletteDisableSub(void) 
{ 
  REG_DISPCNT_SUB &= ~DISPLAY_BG_EXT_PALETTE; 
}


#ifdef __cplusplus
}
#endif

/*@}*/


#endif
