/*---------------------------------------------------------------------------------
	$Id: video.h,v 1.32 2007-01-14 11:41:51 wntrmute Exp $

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

	$Log: not supported by cvs2svn $
	Revision 1.31  2007/01/14 11:31:22  wntrmute
	bogus fixed types removed from libnds
	
	Revision 1.30  2007/01/11 05:35:41  dovoto
	Applied gabebear patch # 1632896
	fix gluPickMatrix()
	- no float / f32 version because all the parameters will always be regular ints
	- it actually works now
	
	fix gluFrustrumf32() and gluFrustum()
	- rename to glFrustrum because this is a GL function, not GLU (I'm breaking stuff...)
	- no longer changes matrix mode to projection (it's useful for more than projection)
	- multiplies instead of loads
	
	fix glOrthof32()
	- no longer changes matrix mode to projection (it's useful for more than projection)
	- multiplies instead of loads
	
	fix glGetFixed()
	- correctly waits for graphics engine to stop before getting any matrix
	- added ability to get projection and modelview matrices
	- fixed projection matrix get (it was grabbing modelview)
	- getting projection or position matrices now uses the matrix stack to preserve
	the other matrix. Not many people use the matrix stack and you would normally
	only do this at the beginning of a program so I doubt it will be a problem.
	
	Revision 1.29  2006/08/19 07:07:10  dovoto
	Found a few more mapping defines that were missing or ambigous
	
	Revision 1.28  2006/08/19 06:41:44  dovoto
	updated mappoing for vram A B main sprite and C, D arm7 vram
	
	Revision 1.25  2006/07/19 21:10:34  wntrmute
	add libgba compatibility macros
	
	Revision 1.24  2006/07/18 15:45:09  wntrmute
	added handy macros from libgba

	Revision 1.23  2006/07/04 12:10:57  wntrmute
	add macros for extended palette slots

	Revision 1.22  2006/07/02 17:08:17  wntrmute
	object scaling registers are signed

	Revision 1.21  2006/05/13 13:38:51  wntrmute
	updated for registers moved to main headers

	Revision 1.20  2006/02/10 00:17:55  desktopman
	Added MODE_FIFO for main memory to FIFO display mode

	Revision 1.19  2006/02/02 00:11:57  wntrmute
	corrected BG_64x32 & BG_32x64 defines

	Revision 1.18  2006/01/12 09:13:14  wntrmute
	remove duplicate ATTR0_BMP

	Revision 1.17  2006/01/05 08:13:26  dovoto
	Fixed gluLookAt (again)
	Major update to palette handling (likely a breaking change if you were using the gl texture palettes from before)

	Revision 1.16  2005/12/12 14:36:03  wntrmute
	*** empty log message ***

	Revision 1.15  2005/11/27 04:37:00  joatski
	Added DISP_CAPTURE and associated macros
	Added GFX_CUTOFF_DEPTH
	Renamed GFX_ALPHA to GFX_ALPHA_TEST

	Revision 1.14  2005/11/26 20:31:15  joatski
	vramRestorMainBanks changed to vramRestoreMainBanks
	Added some comment lines between defines

	Revision 1.13  2005/11/07 04:13:41  dovoto
	Added register definitions for box test and matrix reading

	Revision 1.12  2005/10/05 21:56:58  wntrmute
	corrected BG_BMP8_1024x512 & BG_BMP8_512x1024 defines

	Revision 1.11  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.10  2005/08/01 23:18:22  wntrmute
	adjusted headers for logging


---------------------------------------------------------------------------------*/

#ifndef VIDEO_ARM9_INCLUDE
#define VIDEO_ARM9_INCLUDE


#ifndef ARM9
#error Video is only available on the ARM9
#endif

#include <nds/jtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

// macro creates a 15 bit color from 3x5 bit components
#define RGB15(r,g,b)  ((r)|((g)<<5)|((b)<<10))
#define RGB5(r,g,b)  ((r)|((g)<<5)|((b)<<10))
#define RGB8(r,g,b)  (((r)>>3)|(((g)>>3)<<5)|(((b)>>3)<<10))


#define SCREEN_HEIGHT 192
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

typedef enum {
	VRAM_B_LCD = 0,
	VRAM_B_MAIN_BG	= 1 | VRAM_OFFSET(1),
	VRAM_B_MAIN_BG_0x06000000	= 1 | VRAM_OFFSET(0),
	VRAM_B_MAIN_BG_0x06020000	= 1 | VRAM_OFFSET(1),
	VRAM_B_MAIN_BG_0x06040000	= 1 | VRAM_OFFSET(2),
	VRAM_B_MAIN_BG_0x06060000	= 1 | VRAM_OFFSET(3),
	VRAM_B_MAIN_SPRITE	= 2 | VRAM_OFFSET(1),
	VRAM_B_MAIN_SPRITE_0x6400000	= 2,
	VRAM_B_MAIN_SPRITE_0x6420000	= 2 | VRAM_OFFSET(1),
	VRAM_B_TEXTURE	= 3 | VRAM_OFFSET(1),
	VRAM_B_TEXTURE_SLOT0	= 3 | VRAM_OFFSET(0),
	VRAM_B_TEXTURE_SLOT1	= 3 | VRAM_OFFSET(1),
	VRAM_B_TEXTURE_SLOT2	= 3 | VRAM_OFFSET(2),
	VRAM_B_TEXTURE_SLOT3	= 3 | VRAM_OFFSET(3)
} VRAM_B_TYPE;

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

typedef enum {
	VRAM_E_LCD             = 0,
	VRAM_E_MAIN_BG         = 1,
	VRAM_E_MAIN_SPRITE     = 2,
	VRAM_E_TEX_PALETTE     = 3,
	VRAM_E_BG_EXT_PALETTE  = 4,
	VRAM_E_OBJ_EXT_PALETTE = 5,
} VRAM_E_TYPE;

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
	VRAM_F_OBJ_EXT_PALETTE = 5,
} VRAM_F_TYPE;

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
	VRAM_G_OBJ_EXT_PALETTE = 5,
} VRAM_G_TYPE;

typedef enum {
	VRAM_H_LCD                = 0,
	VRAM_H_SUB_BG             = 1,
	VRAM_H_SUB_BG_EXT_PALETTE = 2,
} VRAM_H_TYPE;

typedef enum {
	VRAM_I_LCD                    = 0,
	VRAM_I_SUB_BG                 = 1,
	VRAM_I_SUB_SPRITE             = 2,
	VRAM_I_SUB_SPRITE_EXT_PALETTE = 3,
}VRAM_I_TYPE;


typedef u16 _palette[256];
typedef _palette _ext_palette[16];

#define VRAM_E_EXT_PALETTE ((_ext_palette *)VRAM_E)
#define VRAM_F_EXT_PALETTE ((_ext_palette *)VRAM_F)
#define VRAM_G_EXT_PALETTE ((_ext_palette *)VRAM_G)
#define VRAM_H_EXT_PALETTE ((_ext_palette *)VRAM_H)


uint32 vramSetMainBanks(VRAM_A_TYPE a, VRAM_B_TYPE b, VRAM_C_TYPE c, VRAM_D_TYPE d);
void vramRestoreMainBanks(uint32 vramTemp);
void vramRestorMainBanks(uint32 vramTemp) __attribute__((deprecated));

void vramSetBankA(VRAM_A_TYPE a);
void vramSetBankB(VRAM_B_TYPE b);
void vramSetBankC(VRAM_C_TYPE c);
void vramSetBankD(VRAM_D_TYPE d);
void vramSetBankE(VRAM_E_TYPE e);
void vramSetBankF(VRAM_F_TYPE f);
void vramSetBankG(VRAM_G_TYPE g);
void vramSetBankH(VRAM_H_TYPE h);
void vramSetBankI(VRAM_I_TYPE i);


// Display control registers
#define DISPLAY_CR       (*(vuint32*)0x04000000)
#define SUB_DISPLAY_CR   (*(vuint32*)0x04001000)

#define MODE_0_2D      0x10000
#define MODE_1_2D      0x10001
#define MODE_2_2D      0x10002
#define MODE_3_2D      0x10003
#define MODE_4_2D      0x10004
#define MODE_5_2D      0x10005

// main display only
#define MODE_6_2D      0x10006
#define MODE_FIFO      (3<<16)

#define ENABLE_3D    (1<<3)

#define DISPLAY_BG0_ACTIVE    (1 << 8)
#define DISPLAY_BG1_ACTIVE    (1 << 9)
#define DISPLAY_BG2_ACTIVE    (1 << 10)
#define DISPLAY_BG3_ACTIVE    (1 << 11)
#define DISPLAY_SPR_ACTIVE    (1 << 12)
#define DISPLAY_WIN0_ON       (1 << 13)
#define DISPLAY_WIN1_ON       (1 << 14)
#define DISPLAY_SPR_WIN_ON    (1 << 15)


// Main display only
#define MODE_0_3D    (MODE_0_2D | DISPLAY_BG0_ACTIVE | ENABLE_3D)
#define MODE_1_3D    (MODE_1_2D | DISPLAY_BG0_ACTIVE | ENABLE_3D)
#define MODE_2_3D    (MODE_2_2D | DISPLAY_BG0_ACTIVE | ENABLE_3D)
#define MODE_3_3D    (MODE_3_2D | DISPLAY_BG0_ACTIVE | ENABLE_3D)
#define MODE_4_3D    (MODE_4_2D | DISPLAY_BG0_ACTIVE | ENABLE_3D)
#define MODE_5_3D    (MODE_5_2D | DISPLAY_BG0_ACTIVE | ENABLE_3D)
#define MODE_6_3D    (MODE_6_2D | DISPLAY_BG0_ACTIVE | ENABLE_3D)

#define MODE_FB0    (0x00020000)
#define MODE_FB1    (0x00060000)
#define MODE_FB2	(0x000A0000)
#define MODE_FB3	(0x000E0000)

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


#define DISPLAY_SPR_EXT_PALETTE		(1 << 31)
#define DISPLAY_BG_EXT_PALETTE		(1 << 30)

#define DISPLAY_SCREEN_OFF     (1 << 7)

static inline
void videoSetMode( uint32 mode)  { DISPLAY_CR = mode; }
static inline
void videoSetModeSub( uint32 mode)  { SUB_DISPLAY_CR = mode; }

#define BRIGHTNESS     (*(vuint16*)0x0400006C)
#define SUB_BRIGHTNESS (*(vuint16*)0x0400106C)

#define BGCTRL			((vuint16*)0x04000008)
#define BG0_CR		(*(vuint16*)0x04000008)
#define BG1_CR		(*(vuint16*)0x0400000A)
#define BG2_CR		(*(vuint16*)0x0400000C)
#define BG3_CR		(*(vuint16*)0x0400000E)

#define BGCTRL_SUB		((vuint16*)0x04001008)
#define SUB_BG0_CR     (*(vuint16*)0x04001008)
#define SUB_BG1_CR     (*(vuint16*)0x0400100A)
#define SUB_BG2_CR     (*(vuint16*)0x0400100C)
#define SUB_BG3_CR     (*(vuint16*)0x0400100E)

#define BG_256_COLOR   (BIT(7))
#define BG_16_COLOR    (0)

#define BG_MOSAIC_ON   (BIT(6))
#define BG_MOSAIC_OFF  (0)

#define BG_PRIORITY(n) (n)
#define BG_PRIORITY_0  (0)
#define BG_PRIORITY_1  (1)
#define BG_PRIORITY_2  (2)
#define BG_PRIORITY_3  (3)

#define BG_TILE_BASE(base) ((base) << 2)
#define BG_MAP_BASE(base)  ((base) << 8)
#define BG_BMP_BASE(base)  ((base) << 8)

#define BG_MAP_RAM(base)  (((base)*0x800) + 0x06000000)
#define BG_MAP_RAM_SUB(base) (((base)*0x800) + 0x06200000)

#define BG_TILE_RAM(base) (((base)*0x4000) + 0x06000000)
#define BG_TILE_RAM_SUB(base) (((base)*0x4000) + 0x06200000)

#define BG_BMP_RAM(base)  (((base)*0x4000) + 0x06000000)
#define BG_BMP_RAM_SUB(base) (((base)*0x4000) + 0x06200000)

#define BG_WRAP_OFF    (0)
#define BG_WRAP_ON     (1 << 13)

#define BG_32x32       (0 << 14)
#define BG_64x32       (1 << 14)
#define BG_32x64       (2 << 14)
#define BG_64x64       (3 << 14)

#define BG_RS_16x16    (0 << 14)
#define BG_RS_32x32    (1 << 14)
#define BG_RS_64x64    (2 << 14)
#define BG_RS_128x128  (3 << 14)

#define BG_BMP8_128x128 (BG_RS_16x16 | BG_256_COLOR)
#define BG_BMP8_256x256 (BG_RS_32x32 | BG_256_COLOR)
#define BG_BMP8_512x256 (BG_RS_64x64 | BG_256_COLOR)
#define BG_BMP8_512x512 (BG_RS_128x128 | BG_256_COLOR)
#define BG_BMP8_1024x512 BIT(14)
#define BG_BMP8_512x1024 0

#define BG_BMP16_128x128 (BG_RS_16x16 | BG_256_COLOR | BIT(2))
#define BG_BMP16_256x256 (BG_RS_32x32 | BG_256_COLOR | BIT(2))
#define BG_BMP16_512x256 (BG_RS_64x64 | BG_256_COLOR | BIT(2))
#define BG_BMP16_512x512 (BG_RS_128x128 | BG_256_COLOR | BIT(2))

#define BG_PALETTE_SLOT0 0
#define BG_PALETTE_SLOT1 0
#define BG_PALETTE_SLOT2 BIT(13)
#define BG_PALETTE_SLOT3 BIT(13)

typedef struct {
	vu16 x;
	vu16 y;
} bg_scroll;

#define BG_OFFSET ((bg_scroll *)(0x04000010))

#define BG0_X0         (*(vuint16*)0x04000010)
#define BG0_Y0         (*(vuint16*)0x04000012)
#define BG1_X0         (*(vuint16*)0x04000014)
#define BG1_Y0         (*(vuint16*)0x04000016)
#define BG2_X0         (*(vuint16*)0x04000018)
#define BG2_Y0         (*(vuint16*)0x0400001A)
#define BG3_X0         (*(vuint16*)0x0400001C)
#define BG3_Y0         (*(vuint16*)0x0400001E)

#define BG2_XDX        (*(vuint16*)0x04000020)
#define BG2_XDY        (*(vuint16*)0x04000022)
#define BG2_YDX        (*(vuint16*)0x04000024)
#define BG2_YDY        (*(vuint16*)0x04000026)
#define BG2_CX         (*(vuint32*)0x04000028)
#define BG2_CY         (*(vuint32*)0x0400002C)

#define BG3_XDX        (*(vuint16*)0x04000030)
#define BG3_XDY        (*(vuint16*)0x04000032)
#define BG3_YDX        (*(vuint16*)0x04000034)
#define BG3_YDY        (*(vuint16*)0x04000036)
#define BG3_CX         (*(vuint32*)0x04000038)
#define BG3_CY         (*(vuint32*)0x0400003C)

#define SUB_BG0_X0     (*(vuint16*)0x04001010)
#define SUB_BG0_Y0     (*(vuint16*)0x04001012)
#define SUB_BG1_X0     (*(vuint16*)0x04001014)
#define SUB_BG1_Y0     (*(vuint16*)0x04001016)
#define SUB_BG2_X0     (*(vuint16*)0x04001018)
#define SUB_BG2_Y0     (*(vuint16*)0x0400101A)
#define SUB_BG3_X0     (*(vuint16*)0x0400101C)
#define SUB_BG3_Y0     (*(vuint16*)0x0400101E)

#define SUB_BG2_XDX    (*(vuint16*)0x04001020)
#define SUB_BG2_XDY    (*(vuint16*)0x04001022)
#define SUB_BG2_YDX    (*(vuint16*)0x04001024)
#define SUB_BG2_YDY    (*(vuint16*)0x04001026)
#define SUB_BG2_CX     (*(vuint32*)0x04001028)
#define SUB_BG2_CY     (*(vuint32*)0x0400102C)

#define SUB_BG3_XDX    (*(vuint16*)0x04001030)
#define SUB_BG3_XDY    (*(vuint16*)0x04001032)
#define SUB_BG3_YDX    (*(vuint16*)0x04001034)
#define SUB_BG3_YDY    (*(vuint16*)0x04001036)
#define SUB_BG3_CX     (*(vuint32*)0x04001038)
#define SUB_BG3_CY     (*(vuint32*)0x0400103C)

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

#define	DISP_CAPTURE  (*(vuint32*)0x04000064)

#define DCAP_ENABLE    BIT(31)
#define DCAP_MODE(n)   (((n) & 3) << 29)
#define DCAP_DST(n)    (((n) & 3) << 26)
#define DCAP_SRC(n)    (((n) & 3) << 24)
#define DCAP_SIZE(n)   (((n) & 3) << 20)
#define DCAP_OFFSET(n) (((n) & 3) << 18)
#define DCAP_BANK(n)   (((n) & 3) << 16)
#define DCAP_B(n)      (((n) & 0x1F) << 8)
#define DCAP_A(n)      (((n) & 0x1F) << 0)

// Background control defines

// BGxCNT defines ///
#define BG_MOSAIC_ENABLE    0x40
#define BG_COLOR_256      0x80
#define BG_COLOR_16        0x0

#define CHAR_BASE_BLOCK(n)    (((n)*0x4000)+0x6000000)
#define CHAR_BASE_BLOCK_SUB(n)    (((n)*0x4000)+0x6200000)
#define SCREEN_BASE_BLOCK(n)  (((n)*0x800)+0x6000000)
#define SCREEN_BASE_BLOCK_SUB(n)  (((n)*0x800)+0x6200000)

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

#define WRAPAROUND              0x1

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


// Sprite structures

typedef struct sSpriteEntry {
  uint16 attribute[3];
  uint16 filler;
} SpriteEntry, * pSpriteEntry;


typedef struct sSpriteRotation {
  uint16 filler1[3];
  int16 hdx;

  uint16 filler2[3];
  int16 hdy;

  uint16 filler3[3];
  int16 vdx;

  uint16 filler4[3];
  int16 vdy;
} SpriteRotation, * pSpriteRotation;

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
#define GFX_BOX_TEST		(*(vint32*)  0x040005C0)

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
#define MATRIX_READ_MODELVIEW	((vint32*) (0x04000640))
#define MATRIX_READ_ROTATION	((vint32*) (0x04000680))
#define POINT_RESULT			((vint32*) (0x04000620))
#define VECTOR_RESULT			((vuint16*)(0x04000630))

#ifdef __cplusplus
}
#endif

#endif
