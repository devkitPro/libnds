/*---------------------------------------------------------------------------------

	Video registers and defines

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

---------------------------------------------------------------------------------*/

/*!	\file video.h

	\brief DS video functions
*/

#ifndef VIDEO_ARM9_INCLUDE
#define VIDEO_ARM9_INCLUDE


#ifndef ARM9
#error Video is only available on the ARM9
#endif

#include <nds/ndstypes.h>
#include <nds/arm9/sassert.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


// macro creates a 15 bit color from 3x5 bit components
/** \brief Macro to convert 5 bit r g b components into a single 15 bit RGB tripplet */
#define RGB15(r,g,b)  ((r)|((g)<<5)|((b)<<10))
#define RGB5(r,g,b)  ((r)|((g)<<5)|((b)<<10))
#define RGB8(r,g,b)  (((r)>>3)|(((g)>>3)<<5)|(((b)>>3)<<10))
/** \brief Macro to convert 5 bit r g b components plus 1 bit alpha into a single 16 bit ARGB tripplet */
#define ARGB16(a, r, g, b) ( ((a) << 15) | (r)|((g)<<5)|((b)<<10))

/** \brief Screen height in pixels */
#define SCREEN_HEIGHT 192 
/** \brief Screen width in pixels */
#define SCREEN_WIDTH  256
//	Vram Control
#define VRAM_CR			(*(vuint32*)0x04000240)
#define VRAM_A_CR		(*(vuint8*)0x04000240)
#define VRAM_B_CR		(*(vuint8*)0x04000241)
#define VRAM_C_CR		(*(vuint8*)0x04000242)
#define VRAM_D_CR		(*(vuint8*)0x04000243)
#define VRAM_E_CR		(*(vuint8*)0x04000244)
#define VRAM_F_CR		(*(vuint8*)0x04000245)
#define VRAM_G_CR		(*(vuint8*)0x04000246)
#define WRAM_CR			(*(vuint8*)0x04000247)
#define VRAM_H_CR		(*(vuint8*)0x04000248)
#define VRAM_I_CR		(*(vuint8*)0x04000249)

#define VRAM_ENABLE		(1<<7)

#define VRAM_OFFSET(n)	((n)<<3)

/** \brief Allowed VRAM bank A modes */
typedef enum {
	VRAM_A_LCD	=	0,
	VRAM_A_MAIN_BG  = 1,
	VRAM_A_MAIN_BG_0x06000000	= 1 | VRAM_OFFSET(0),
	VRAM_A_MAIN_BG_0x06020000	= 1 | VRAM_OFFSET(1),
	VRAM_A_MAIN_BG_0x06040000	= 1 | VRAM_OFFSET(2),
	VRAM_A_MAIN_BG_0x06060000	= 1 | VRAM_OFFSET(3),
	VRAM_A_MAIN_SPRITE = 2,
	VRAM_A_MAIN_SPRITE_0x06400000	= 2,
	VRAM_A_MAIN_SPRITE_0x06420000	= 2 | VRAM_OFFSET(1),
	VRAM_A_TEXTURE = 3,
	VRAM_A_TEXTURE_SLOT0	= 3 | VRAM_OFFSET(0),
	VRAM_A_TEXTURE_SLOT1	= 3 | VRAM_OFFSET(1),
	VRAM_A_TEXTURE_SLOT2	= 3 | VRAM_OFFSET(2),
	VRAM_A_TEXTURE_SLOT3	= 3 | VRAM_OFFSET(3)
} VRAM_A_TYPE;

/** \brief Allowed VRAM bank B modes */
typedef enum {
	VRAM_B_LCD = 0,
	VRAM_B_MAIN_BG	= 1 | VRAM_OFFSET(1),
	VRAM_B_MAIN_BG_0x06000000	= 1 | VRAM_OFFSET(0),
	VRAM_B_MAIN_BG_0x06020000	= 1 | VRAM_OFFSET(1),
	VRAM_B_MAIN_BG_0x06040000	= 1 | VRAM_OFFSET(2),
	VRAM_B_MAIN_BG_0x06060000	= 1 | VRAM_OFFSET(3),
	VRAM_B_MAIN_SPRITE	= 2 | VRAM_OFFSET(1),
	VRAM_B_MAIN_SPRITE_0x06400000	= 2,
	VRAM_B_MAIN_SPRITE_0x06420000	= 2 | VRAM_OFFSET(1),
	VRAM_B_TEXTURE	= 3 | VRAM_OFFSET(1),
	VRAM_B_TEXTURE_SLOT0	= 3 | VRAM_OFFSET(0),
	VRAM_B_TEXTURE_SLOT1	= 3 | VRAM_OFFSET(1),
	VRAM_B_TEXTURE_SLOT2	= 3 | VRAM_OFFSET(2),
	VRAM_B_TEXTURE_SLOT3	= 3 | VRAM_OFFSET(3)
} VRAM_B_TYPE;

/** \brief Allowed VRAM bank C modes */
typedef enum {
	VRAM_C_LCD = 0,
	VRAM_C_MAIN_BG  = 1 | VRAM_OFFSET(2),
	VRAM_C_MAIN_BG_0x06000000	= 1 | VRAM_OFFSET(0),
	VRAM_C_MAIN_BG_0x06020000	= 1 | VRAM_OFFSET(1),
	VRAM_C_MAIN_BG_0x06040000	= 1 | VRAM_OFFSET(2),
	VRAM_C_MAIN_BG_0x06060000	= 1 | VRAM_OFFSET(3),
	VRAM_C_ARM7	= 2,
	VRAM_C_ARM7_0x06000000 = 2,
	VRAM_C_ARM7_0x06020000 = 2 | VRAM_OFFSET(1),
	VRAM_C_SUB_BG	= 4,
	VRAM_C_SUB_BG_0x06200000	= 4 | VRAM_OFFSET(0),
	VRAM_C_SUB_BG_0x06220000	= 4 | VRAM_OFFSET(1),
	VRAM_C_SUB_BG_0x06240000	= 4 | VRAM_OFFSET(2),
	VRAM_C_SUB_BG_0x06260000	= 4 | VRAM_OFFSET(3),
	VRAM_C_TEXTURE	= 3 | VRAM_OFFSET(2),
	VRAM_C_TEXTURE_SLOT0	= 3 | VRAM_OFFSET(0),
	VRAM_C_TEXTURE_SLOT1	= 3 | VRAM_OFFSET(1),
	VRAM_C_TEXTURE_SLOT2	= 3 | VRAM_OFFSET(2),
	VRAM_C_TEXTURE_SLOT3	= 3 | VRAM_OFFSET(3)
} VRAM_C_TYPE;

/** \brief Allowed VRAM bank D modes */
typedef enum {
	VRAM_D_LCD = 0,
	VRAM_D_MAIN_BG  = 1 | VRAM_OFFSET(3),
	VRAM_D_MAIN_BG_0x06000000  = 1 | VRAM_OFFSET(0),
	VRAM_D_MAIN_BG_0x06020000  = 1 | VRAM_OFFSET(1),
	VRAM_D_MAIN_BG_0x06040000  = 1 | VRAM_OFFSET(2),
	VRAM_D_MAIN_BG_0x06060000  = 1 | VRAM_OFFSET(3),
	VRAM_D_ARM7 = 2 | VRAM_OFFSET(1),
	VRAM_D_ARM7_0x06000000 = 2,
	VRAM_D_ARM7_0x06020000 = 2 | VRAM_OFFSET(1),
	VRAM_D_SUB_SPRITE  = 4,
	VRAM_D_TEXTURE = 3 | VRAM_OFFSET(3),
	VRAM_D_TEXTURE_SLOT0 = 3 | VRAM_OFFSET(0),
	VRAM_D_TEXTURE_SLOT1 = 3 | VRAM_OFFSET(1),
	VRAM_D_TEXTURE_SLOT2 = 3 | VRAM_OFFSET(2),
	VRAM_D_TEXTURE_SLOT3 = 3 | VRAM_OFFSET(3)
} VRAM_D_TYPE;

/** \brief Allowed VRAM bank E modes */
typedef enum {
	VRAM_E_LCD             = 0,
	VRAM_E_MAIN_BG         = 1,
	VRAM_E_MAIN_SPRITE     = 2,
	VRAM_E_TEX_PALETTE     = 3,
	VRAM_E_BG_EXT_PALETTE  = 4,
	VRAM_E_OBJ_EXT_PALETTE = 5,
} VRAM_E_TYPE;

/** \brief Allowed VRAM bank F modes */
typedef enum {
	VRAM_F_LCD             = 0,
	VRAM_F_MAIN_BG         = 1,
	VRAM_F_MAIN_SPRITE     = 2,
	VRAM_F_MAIN_SPRITE_0x06000000     = 2,
	VRAM_F_MAIN_SPRITE_0x06004000     = 2 | VRAM_OFFSET(1),
	VRAM_F_MAIN_SPRITE_0x06010000     = 2 | VRAM_OFFSET(2),
	VRAM_F_MAIN_SPRITE_0x06014000     = 2 | VRAM_OFFSET(3),
	VRAM_F_TEX_PALETTE     = 3,
	VRAM_F_BG_EXT_PALETTE  = 4,
	VRAM_F_BG_EXT_PALETTE_SLOT01 = 4 | VRAM_OFFSET(0),
	VRAM_F_BG_EXT_PALETTE_SLOT23 = 4 | VRAM_OFFSET(1),
	VRAM_F_OBJ_EXT_PALETTE = 5,
} VRAM_F_TYPE;

/** \brief Allowed VRAM bank G modes */
typedef enum {
	VRAM_G_LCD             = 0,
	VRAM_G_MAIN_BG         = 1,
	VRAM_G_MAIN_SPRITE     = 2,
	VRAM_G_MAIN_SPRITE_0x06000000     = 2,
	VRAM_G_MAIN_SPRITE_0x06004000     = 2 | VRAM_OFFSET(1),
	VRAM_G_MAIN_SPRITE_0x06010000     = 2 | VRAM_OFFSET(2),
	VRAM_G_MAIN_SPRITE_0x06014000     = 2 | VRAM_OFFSET(3),
	VRAM_G_TEX_PALETTE     = 3,
	VRAM_G_BG_EXT_PALETTE  = 4,
	VRAM_G_BG_EXT_PALETTE_SLOT01 = 4 | VRAM_OFFSET(0),
	VRAM_G_BG_EXT_PALETTE_SLOT23 = 4 | VRAM_OFFSET(1),
	VRAM_G_OBJ_EXT_PALETTE = 5,
} VRAM_G_TYPE;

/** \brief Allowed VRAM bank H modes */
typedef enum {
	VRAM_H_LCD                = 0,
	VRAM_H_SUB_BG             = 1,
	VRAM_H_SUB_BG_EXT_PALETTE = 2,
} VRAM_H_TYPE;

/** \brief Allowed VRAM bank I modes */
typedef enum {
	VRAM_I_LCD                    = 0,
	VRAM_I_SUB_BG                 = 1,
	VRAM_I_SUB_SPRITE             = 2,
	VRAM_I_SUB_SPRITE_EXT_PALETTE = 3,
}VRAM_I_TYPE;


/** \brief an array of 256 15-bit RGB values*/
typedef u16 _palette[256];

/** \brief An array of 16 256-color palettes */
typedef _palette _ext_palette[16];

/** \brief Used for accessing vram E as an external palette */
#define VRAM_E_EXT_PALETTE ((_ext_palette *)VRAM_E)

/** \brief Used for accessing vram F as an external palette */
#define VRAM_F_EXT_PALETTE ((_ext_palette *)VRAM_F)

/** \brief Used for accessing vram G as an external palette */
#define VRAM_G_EXT_PALETTE ((_ext_palette *)VRAM_G)

/** \brief Used for accessing vram H as an external palette */
#define VRAM_H_EXT_PALETTE ((_ext_palette *)VRAM_H)

/** \brief Set the main 4 bank modes. 
*    \param a mapping mode of VRAM_A
*    \param b mapping mode of VRAM_B
*    \param c mapping mode of VRAM_C
*    \param d mapping mode of VRAM_D
*    \return the previous mode
*/
uint32 vramSetMainBanks(VRAM_A_TYPE a, VRAM_B_TYPE b, VRAM_C_TYPE c, VRAM_D_TYPE d);

/** \brief Set the main 4 bank modes. 
*    \param vramTemp restores the main 4 banks to the value encoded in vramTemp (returned from vramSetMainBanks)
*/
void vramRestoreMainBanks(uint32 vramTemp);

/** \brief Set bank A to the indicated mapping. 
*    \param a the mapping of the bank
*/
void vramSetBankA(VRAM_A_TYPE a);

/** \brief Set bank B to the indicated mapping. 
*    \param b the mapping of the bank
*/
void vramSetBankB(VRAM_B_TYPE b);

/** \brief Set bank C to the indicated mapping. 
*    \param c the mapping of the bank
*/
void vramSetBankC(VRAM_C_TYPE c);

/** \brief Set bank D to the indicated mapping. 
*    \param d the mapping of the bank
*/
void vramSetBankD(VRAM_D_TYPE d);

/** \brief Set bank E to the indicated mapping. 
*    \param e the mapping of the bank
*/
void vramSetBankE(VRAM_E_TYPE e);

/** \brief Set bank F to the indicated mapping. 
*    \param f the mapping of the bank
*/
void vramSetBankF(VRAM_F_TYPE f);

/** \brief Set bank G to the indicated mapping. 
*    \param g the mapping of the bank
*/
void vramSetBankG(VRAM_G_TYPE g);

/** \brief Set bank H to the indicated mapping. 
*    \param h the mapping of the bank
*/
void vramSetBankH(VRAM_H_TYPE h);

/** \brief Set bank I to the indicated mapping. 
*    \param i the mapping of the bank
*/
void vramSetBankI(VRAM_I_TYPE i);


// Display control registers
#define DISPLAY_CR       (*(vuint32*)0x04000000)
#define SUB_DISPLAY_CR   (*(vuint32*)0x04001000)

#define ENABLE_3D    (1<<3)
#define DISPLAY_ENABLE_SHIFT 8
#define DISPLAY_BG0_ACTIVE    (1 << 8)
#define DISPLAY_BG1_ACTIVE    (1 << 9)
#define DISPLAY_BG2_ACTIVE    (1 << 10)
#define DISPLAY_BG3_ACTIVE    (1 << 11)
#define DISPLAY_SPR_ACTIVE    (1 << 12)
#define DISPLAY_WIN0_ON       (1 << 13)
#define DISPLAY_WIN1_ON       (1 << 14)
#define DISPLAY_SPR_WIN_ON    (1 << 15)

/** \enum  VideoMode
* \brief The allowed video modes of the 2D procesors <br>
* 
*______________________________<br>
*|Mode | BG0 | BG1 | BG2 |BG3 |<br>
*|  0  |  T  |  T  |  T  |  T |<br>
*|  1  |  T  |  T  |  T  |  R |<br>
*|  2  |  T  |  T  |  R  |  R |<br>
*|  3  |  T  |  T  |  T  |  E |<br>
*|  4  |  T  |  T  |  R  |  E |<br>
*|  5  |  T  |  T  |  E  |  E |<br>
*|  6  |     |     |  L  |    |<br>
*-----------------------------<br>
*T = Text<br>
*R = Rotation<br>
*E = Extended Rotation (Bitmap or tiled)<br>
*L = Large Bitmap Mode <br>
*BG 0 can be used as a 3D surface on the main engine<br>
*/
typedef enum
{
 MODE_0_2D    =  0x10000, /** \brief 4 2D backgrounds */
 MODE_1_2D    =  0x10001, /** \brief 4 2D backgrounds */
 MODE_2_2D    =  0x10002, /** \brief 4 2D backgrounds*/
 MODE_3_2D    =  0x10003, /** \brief 4 2D backgrounds */
 MODE_4_2D    =  0x10004, /** \brief 4 2D backgrounds */
 MODE_5_2D    =  0x10005, /** \brief 4 2D backgrounds */
 MODE_6_2D    =  0x10006, /** \brief 4 2D backgrounds */
 MODE_0_3D    = (0x10000 | DISPLAY_BG0_ACTIVE | ENABLE_3D), /** \brief 3 2D backgrounds 1 3D background (Main engine only)*/
 MODE_1_3D    = (0x10001 | DISPLAY_BG0_ACTIVE | ENABLE_3D), /** \brief 3 2D backgrounds 1 3D background (Main engine only)*/
 MODE_2_3D    = (0x10002 | DISPLAY_BG0_ACTIVE | ENABLE_3D), /** \brief 3 2D backgrounds 1 3D background (Main engine only)*/
 MODE_3_3D    = (0x10003 | DISPLAY_BG0_ACTIVE | ENABLE_3D), /** \brief 3 2D backgrounds 1 3D background (Main engine only)*/
 MODE_4_3D    = (0x10004 | DISPLAY_BG0_ACTIVE | ENABLE_3D), /** \brief 3 2D backgrounds 1 3D background (Main engine only)*/
 MODE_5_3D    = (0x10005 | DISPLAY_BG0_ACTIVE | ENABLE_3D), /** \brief 3 2D backgrounds 1 3D background (Main engine only)*/
 MODE_6_3D    = (0x10006 | DISPLAY_BG0_ACTIVE | ENABLE_3D), /** \brief 3 2D backgrounds 1 3D background (Main engine only)*/

 MODE_FIFO    =  (3<<16),/** \brief video display from main memory */

 MODE_FB0     = (0x00020000), /** \brief video display directly from VRAM_A in LCD mode */
 MODE_FB1     = (0x00060000), /** \brief video display directly from VRAM_B in LCD mode */
 MODE_FB2	  = (0x000A0000), /** \brief video display directly from VRAM_C in LCD mode */
 MODE_FB3	  = (0x000E0000)  /** \brief video display directly from VRAM_D in LCD mode */

}VideoMode;

// main display only










#define DISPLAY_SPR_HBLANK	   (1 << 23)

#define DISPLAY_SPR_1D_LAYOUT	(1 << 4)

#define DISPLAY_SPR_1D				(1 << 4)
#define DISPLAY_SPR_2D				(0 << 4)
#define DISPLAY_SPR_1D_BMP			(4 << 4)
#define DISPLAY_SPR_2D_BMP_128		(0 << 4)
#define DISPLAY_SPR_2D_BMP_256		(2 << 4)


#define DISPLAY_SPR_1D_SIZE_32		(0 << 20)
#define DISPLAY_SPR_1D_SIZE_64		(1 << 20)
#define DISPLAY_SPR_1D_SIZE_128		(2 << 20)
#define DISPLAY_SPR_1D_SIZE_256		(3 << 20)
#define DISPLAY_SPR_1D_BMP_SIZE_128	(0 << 22)
#define DISPLAY_SPR_1D_BMP_SIZE_256	(1 << 22)

//mask to clear all attributes related to sprites from display control
#define DISPLAY_SPRITE_ATTR_MASK  ((7 << 4) | (7 << 20) | (1 << 31))

#define DISPLAY_SPR_EXT_PALETTE		(1 << 31)
#define DISPLAY_BG_EXT_PALETTE		(1 << 30)

#define DISPLAY_SCREEN_OFF     (1 << 7)

// The next two defines only apply to MAIN 2d engine
// In tile modes, this is multiplied by 64KB and added to BG_TILE_BASE
// In all bitmap modes, it is not used.
#define DISPLAY_CHAR_BASE(n) (((n)&7)<<24)

// In tile modes, this is multiplied by 64KB and added to BG_MAP_BASE
// In bitmap modes, this is multiplied by 64KB and added to BG_BMP_BASE
// In large bitmap modes, this is not used
#define DISPLAY_SCREEN_BASE(n) (((n)&7)<<27)

static inline
/** \brief the main 2D engine video mode 
*    \param mode the video mode to set
*/
void videoSetMode( uint32 mode)  { DISPLAY_CR = mode; }

static inline
/** \brief the sub 2D engine video mode 
*    \param mode the video mode to set
*/
void videoSetModeSub( uint32 mode)  { SUB_DISPLAY_CR = mode; }

static inline
/** \brief return the main 2D engine video mode 
*    \return the video mode
*/
int videoGetMode() {return (DISPLAY_CR & 0x30007);}

static inline
/** \brief return the main 2D engine video mode 
*    \return the video mode
*/
int videoGetModeSub() {return (SUB_DISPLAY_CR & 0x30007);}

static inline 
/** \brief determine if 3D is enabled
*    \return true if 3D is enabled
*/
bool video3DEnabled() {return (DISPLAY_CR & ENABLE_3D) ? true : false;}

static inline 
/** \brief enables the specified background on the main engine
*    \param number the background number (0-3)
*/
void videoBgEnable(int number) {DISPLAY_CR |= 1 << (DISPLAY_ENABLE_SHIFT + number);}

static inline 
/** \brief enables the specified background on the sub engine
*    \param number the background number (0-3)
*/
void videoBgEnableSub(int number) {SUB_DISPLAY_CR |= 1 << (DISPLAY_ENABLE_SHIFT + number);}

static inline 
/** \brief disables the specified background on the main engine
*    \param number the background number (0-3)
*/
void videoBgDisable(int number) {DISPLAY_CR &= ~(1 << (DISPLAY_ENABLE_SHIFT + number));}

static inline 
/** \brief disables the specified background on the sub engine
*    \param number the background number (0-3)
*/
void videoBgDisableSub(int number) {SUB_DISPLAY_CR &= ~(1 << (DISPLAY_ENABLE_SHIFT + number));}


#define BRIGHTNESS     (*(vuint16*)0x0400006C)
#define SUB_BRIGHTNESS (*(vuint16*)0x0400106C)


// Window 0
#define WIN0_X0        (*(vuint8*)0x04000041)
#define WIN0_X1        (*(vuint8*)0x04000040)
#define WIN0_Y0        (*(vuint8*)0x04000045)
#define WIN0_Y1        (*(vuint8*)0x04000044)

// Window 1
#define WIN1_X0        (*(vuint8*)0x04000042)
#define WIN1_X1        (*(vuint8*)0x04000043)
#define WIN1_Y0        (*(vuint8*)0x04000047)
#define WIN1_Y1        (*(vuint8*)0x04000046)

#define WIN_IN         (*(vuint16*)0x04000048)
#define WIN_OUT        (*(vuint16*)0x0400004A)

// Window 0
#define SUB_WIN0_X0    (*(vuint8*)0x04001041)
#define SUB_WIN0_X1    (*(vuint8*)0x04001040)
#define SUB_WIN0_Y0    (*(vuint8*)0x04001045)
#define SUB_WIN0_Y1    (*(vuint8*)0x04001044)

// Window 1
#define SUB_WIN1_X0    (*(vuint8*)0x04001042)
#define SUB_WIN1_X1    (*(vuint8*)0x04001043)
#define SUB_WIN1_Y0    (*(vuint8*)0x04001047)
#define SUB_WIN1_Y1    (*(vuint8*)0x04001046)

#define SUB_WIN_IN     (*(vuint16*)0x04001048)
#define SUB_WIN_OUT    (*(vuint16*)0x0400104A)

#define MOSAIC_CR      (*(vuint16*)0x0400004C)
#define SUB_MOSAIC_CR  (*(vuint16*)0x0400104C)

#define BLEND_CR       (*(vuint16*)0x04000050)
#define BLEND_AB       (*(vuint16*)0x04000052)
#define BLEND_Y        (*(vuint16*)0x04000054)

#define SUB_BLEND_CR   (*(vuint16*)0x04001050)
#define SUB_BLEND_AB   (*(vuint16*)0x04001052)
#define SUB_BLEND_Y    (*(vuint16*)0x04001054)

#define BLEND_NONE         (0<<6)
#define BLEND_ALPHA        (1<<6)
#define BLEND_FADE_WHITE   (2<<6)
#define BLEND_FADE_BLACK   (3<<6)

#define BLEND_SRC_BG0      (1<<0)
#define BLEND_SRC_BG1      (1<<1)
#define BLEND_SRC_BG2      (1<<2)
#define BLEND_SRC_BG3      (1<<3)
#define BLEND_SRC_SPRITE   (1<<4)
#define BLEND_SRC_BACKDROP (1<<5)

#define BLEND_DST_BG0      (1<<8)
#define BLEND_DST_BG1      (1<<9)
#define BLEND_DST_BG2      (1<<10)
#define BLEND_DST_BG3      (1<<11)
#define BLEND_DST_SPRITE   (1<<12)
#define BLEND_DST_BACKDROP (1<<13)

// Display capture control

#define	REG_DISPCAPCNT		(*(vuint32*)0x04000064)
#define REG_DISP_MMEM_FIFO	(*(vuint32*)0x04000068)

#define DCAP_ENABLE    BIT(31)
#define DCAP_MODE(n)   (((n) & 3) << 29)
#define DCAP_DST(n)    (((n) & 3) << 26)
#define DCAP_SRC(n)    (((n) & 3) << 24)
#define DCAP_SIZE(n)   (((n) & 3) << 20)
#define DCAP_OFFSET(n) (((n) & 3) << 18)
#define DCAP_BANK(n)   (((n) & 3) << 16)
#define DCAP_B(n)      (((n) & 0x1F) << 8)
#define DCAP_A(n)      (((n) & 0x1F) << 0)


// 3D core control

#define GFX_CONTROL           (*(vuint16*) 0x04000060)

#define GFX_FIFO              (*(vuint32*) 0x04000400)
#define GFX_STATUS            (*(vuint32*) 0x04000600)
#define GFX_COLOR             (*(vuint32*) 0x04000480)

#define GFX_VERTEX10          (*(vuint32*) 0x04000490)
#define GFX_VERTEX_XY         (*(vuint32*) 0x04000494)
#define GFX_VERTEX_XZ         (*(vuint32*) 0x04000498)
#define GFX_VERTEX_YZ         (*(vuint32*) 0x0400049C)
#define GFX_VERTEX_DIFF       (*(vuint32*) 0x040004A0)

#define GFX_VERTEX16          (*(vuint32*) 0x0400048C)
#define GFX_TEX_COORD         (*(vuint32*) 0x04000488)
#define GFX_TEX_FORMAT        (*(vuint32*) 0x040004A8)
#define GFX_PAL_FORMAT        (*(vuint32*) 0x040004AC)

#define GFX_CLEAR_COLOR       (*(vuint32*) 0x04000350)
#define GFX_CLEAR_DEPTH       (*(vuint16*) 0x04000354)

#define GFX_LIGHT_VECTOR      (*(vuint32*) 0x040004C8)
#define GFX_LIGHT_COLOR       (*(vuint32*) 0x040004CC)
#define GFX_NORMAL            (*(vuint32*) 0x04000484)

#define GFX_DIFFUSE_AMBIENT   (*(vuint32*) 0x040004C0)
#define GFX_SPECULAR_EMISSION (*(vuint32*) 0x040004C4)
#define GFX_SHININESS         (*(vuint32*) 0x040004D0)

#define GFX_POLY_FORMAT       (*(vuint32*) 0x040004A4)
#define GFX_ALPHA_TEST        (*(vuint16*) 0x04000340)

#define GFX_BEGIN			(*(vuint32*) 0x04000500)
#define GFX_END				(*(vuint32*) 0x04000504)
#define GFX_FLUSH			(*(vuint32*) 0x04000540)
#define GFX_VIEWPORT		(*(vuint32*) 0x04000580)
#define GFX_TOON_TABLE		((vuint16*)  0x04000380)
#define GFX_EDGE_TABLE		((vuint16*)  0x04000330)
#define GFX_FOG_COLOR		(*(vuint32*) 0x04000358)
#define GFX_FOG_OFFSET		(*(vuint32*) 0x0400035C)
#define GFX_FOG_TABLE		((vuint8*)   0x04000360)
#define GFX_BOX_TEST		(*(vint32*)  0x040005C0)
#define GFX_POS_TEST		(*(vuint32*) 0x040005C4)
#define GFX_POS_RESULT		((vint32*)   0x04000620)

#define GFX_BUSY (GFX_STATUS & BIT(27))

#define GFX_VERTEX_RAM_USAGE	(*(uint16*)  0x04000606)
#define GFX_POLYGON_RAM_USAGE	(*(uint16*)  0x04000604)

#define GFX_CUTOFF_DEPTH		(*(uint16*)0x04000610)

// Matrix processor control

#define MATRIX_CONTROL		(*(vuint32*)0x04000440)
#define MATRIX_PUSH			(*(vuint32*)0x04000444)
#define MATRIX_POP			(*(vuint32*)0x04000448)
#define MATRIX_SCALE		(*(vint32*) 0x0400046C)
#define MATRIX_TRANSLATE	(*(vint32*) 0x04000470)
#define MATRIX_RESTORE		(*(vuint32*)0x04000450)
#define MATRIX_STORE		(*(vuint32*)0x0400044C)
#define MATRIX_IDENTITY		(*(vuint32*)0x04000454)
#define MATRIX_LOAD4x4		(*(vint32*) 0x04000458)
#define MATRIX_LOAD4x3		(*(vint32*) 0x0400045C)
#define MATRIX_MULT4x4		(*(vint32*) 0x04000460)
#define MATRIX_MULT4x3		(*(vint32*) 0x04000464)
#define MATRIX_MULT3x3		(*(vint32*) 0x04000468)

//matrix operation results
#define MATRIX_READ_CLIP		((vint32*) (0x04000640))
#define MATRIX_READ_VECTOR		((vint32*) (0x04000680))
#define POINT_RESULT			((vint32*) (0x04000620))
#define VECTOR_RESULT			((vuint16*)(0x04000630))

#ifdef __cplusplus
}
#endif

#endif
