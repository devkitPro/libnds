//////////////////////////////////////////////////////////////////////
//
// video.h -- Video registers and defines
//
// version 0.1, February 14, 2005
//
//  Copyright (C) 2005 Michael Noland (joat) and Jason Rogers (dovoto)
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any
//  damages arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any
//  purpose, including commercial applications, and to alter it and
//  redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you
//     must not claim that you wrote the original software. If you use
//     this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and
//     must not be misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source
//     distribution.
//
// Changelog:
//   0.1: First version
//	 0.2: Fixed sprite mapping bug.  1D mapping should work now.  
//			Changed some register defines for consistency.
//
//////////////////////////////////////////////////////////////////////

#ifndef VIDEO_ARM9_INCLUDE
#define VIDEO_ARM9_INCLUDE

//////////////////////////////////////////////////////////////////////

#ifndef ARM9
#error Video is only available on the ARM9
#endif

//////////////////////////////////////////////////////////////////////

#include <NDS/jtypes.h>

//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

	
//////////////////////////////////////////////////////////////////////

// macro creates a 15 bit color from 3x5 bit components
#define RGB15(r,g,b)  ((r)|((g)<<5)|((b)<<10))


#define SCREEN_HEIGHT 192
#define SCREEN_WIDTH  256
//////////////////////////////////////////////////////////////////////
//	Vram Control
#define VRAM_CR			(*(vuint32*)0x04000240)
#define VRAM_A_CR       (*(vuint8*)0x04000240)
#define VRAM_B_CR       (*(vuint8*)0x04000241)
#define VRAM_C_CR       (*(vuint8*)0x04000242)
#define VRAM_D_CR       (*(vuint8*)0x04000243)
#define VRAM_E_CR       (*(vuint8*)0x04000244)
#define VRAM_F_CR       (*(vuint8*)0x04000245)
#define VRAM_G_CR       (*(vuint8*)0x04000246)
#define VRAM_H_CR       (*(vuint8*)0x04000247)
#define VRAM_I_CR       (*(vuint8*)0x04000248)

#define VRAM_ENABLE   (1<<7)


#define VRAM_OFFSET(n)  ((n)<<3)

typedef enum
{
	VRAM_A_LCD = 0,
	VRAM_A_MAIN_BG  = 1,
	VRAM_A_MAIN_SPRITE = 2,
	VRAM_A_TEXTURE = 3

}VRAM_A_TYPE;

typedef enum
{
	VRAM_B_LCD = 0,
	VRAM_B_MAIN_BG  = 1 | VRAM_OFFSET(1),
	VRAM_B_MAIN_SPRITE = 2,
	VRAM_B_TEXTURE = 3 | VRAM_OFFSET(1)

}VRAM_B_TYPE;	

typedef enum
{
	VRAM_C_LCD = 0,
	VRAM_C_MAIN_BG  = 1 | VRAM_OFFSET(2),
	VRAM_C_MAIN_SPRITE = 2,
	VRAM_C_SUB_BG  = 4,
	VRAM_C_TEXTURE = 3 | VRAM_OFFSET(2)

}VRAM_C_TYPE;

typedef enum
{
	VRAM_D_LCD = 0,
	VRAM_D_MAIN_BG  = 1 | VRAM_OFFSET(3),
	VRAM_D_MAIN_SPRITE = 2,
	VRAM_D_SUB_SPRITE = 4,
	VRAM_D_TEXTURE = 3 | VRAM_OFFSET(3)

}VRAM_D_TYPE;

typedef enum
{
	VRAM_E_LCD			=0,
	VRAM_E_TEX_PALETTE = 3
}VRAM_E_TYPE;

//////////////////////////////////////////////////
  uint32 vramSetMainBanks(VRAM_A_TYPE a, VRAM_B_TYPE b, VRAM_C_TYPE c, VRAM_D_TYPE d);


  void vramRestorMainBanks(uint32 vramTemp);

//////////////////////////////////////////////////
  void vramSetBankA(VRAM_A_TYPE a);

//////////////////////////////////////////////////
  void vramSetBankB(VRAM_B_TYPE b);

//////////////////////////////////////////////////
  void vramSetBankC(VRAM_C_TYPE c);

///////////////////////////////////////////////////
  void vramSetBankD(VRAM_D_TYPE d);

////////////////////////////////////////////////////
  void vramSetBankE(VRAM_E_TYPE e);

//////////////////////////////////////////////////////////////////////
// Display control registers
//////////////////////////////////////////////////////////////////////

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


#define DISPLAY_OAM_ACCESS    (1 << 5)
#define DISPLAY_SPR_1D_LAYOUT (1 << 4)
#define DISPLAY_SCREEN_OFF    (1 << 7)
#define DISPLAY_BG_EXT_PALETTE	BIT(30)

#define H_BLANK_OAM    (1<<5)

#define OBJ_MAP_2D    (0<<4)
#define OBJ_MAP_1D    (1<<4)

#define FORCED_BLANK  (1<<7)

#define videoSetMode(mode)  (DISPLAY_CR = (mode))
#define videoSetModeSub(mode)  (SUB_DISPLAY_CR = (mode))
//////////////////////////////////////////////////////////////////////

#define BRIGHTNESS     (*(vuint16*)0x0400006C)
#define SUB_BRIGHTNESS (*(vuint16*)0x0400106C)

//////////////////////////////////////////////////////////////////////

#define BG_CR		    ((vuint16*)0x04000008)
#define BG0_CR         (*(vuint16*)0x04000008)
#define BG1_CR         (*(vuint16*)0x0400000A)
#define BG2_CR         (*(vuint16*)0x0400000C)
#define BG3_CR         (*(vuint16*)0x0400000E)

#define SUB_BG_CR		((vuint16*)0x04001008)
#define SUB_BG0_CR     (*(vuint16*)0x04001008)
#define SUB_BG1_CR     (*(vuint16*)0x0400100A)
#define SUB_BG2_CR     (*(vuint16*)0x0400100C)
#define SUB_BG3_CR     (*(vuint16*)0x0400100E)

//////////////////////////////////////////////////////////////////////

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
#define BG_32x64       (1 << 14)
#define BG_64x32       (2 << 14)
#define BG_64x64       (3 << 14)

#define BG_RS_16x16    (0 << 14)
#define BG_RS_32x32    (1 << 14)
#define BG_RS_64x64    (2 << 14)
#define BG_RS_128x128  (3 << 14)

#define BG_BMP8_128x128 (BG_RS_16x16 | BG_256_COLOR)
#define BG_BMP8_256x256 (BG_RS_32x32 | BG_256_COLOR)
#define BG_BMP8_512x256 (BG_RS_64x64 | BG_256_COLOR)
#define BG_BMP8_512x512 (BG_RS_128x128 | BG_256_COLOR)
#define BG_BMP8_1024x512 0
#define BG_BMP8_512x1024 BIT(14)

#define BG_BMP16_128x128 (BG_RS_16x16 | BG_256_COLOR | BIT(2))
#define BG_BMP16_256x256 (BG_RS_32x32 | BG_256_COLOR | BIT(2))
#define BG_BMP16_512x256 (BG_RS_64x64 | BG_256_COLOR | BIT(2))
#define BG_BMP16_512x512 (BG_RS_128x128 | BG_256_COLOR | BIT(2))



//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////

#define MOSAIC_CR      (*(vuint16*)0x0400004C)
#define SUB_MOSAIC_CR  (*(vuint16*)0x0400104C)

//////////////////////////////////////////////////////////////////////

#define BLEND_CR       (*(vuint16*)0x04000050)
#define BLEND_AB       (*(vuint16*)0x04000052)
#define BLEND_Y        (*(vuint16*)0x04000054)

#define SUB_BLEND_CR   (*(vuint16*)0x04001050)
#define SUB_BLEND_AB   (*(vuint16*)0x04001052)
#define SUB_BLEND_Y    (*(vuint16*)0x04001054)

#define BLEND_NONE       (0<<6)
#define BLEND_ALPHA      (1<<6)
#define BLEND_FADE_WHITE (2<<6)
#define BLEND_FADE_BLACK (3<<6)

//////////////////////////////////////////////////////////////////////
// Background control defines
//////////////////////////////////////////////////////////////////////
///
///BGxCNT defines ///
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

//////////////////////////////////////////////////////////////////////
// Sprite control defines
//////////////////////////////////////////////////////////////////////

// Attribute 0 consists of 8 bits of Y plus the following flags:
#define ATTR0_NORMAL          (0<<8)
#define ATTR0_ROTSCALE        (1<<8)
#define ATTR0_DISABLED        (2<<8)
#define ATTR0_ROTSCALE_DOUBLE (3<<8)

#define ATTR0_TYPE_NORMAL     (0<<10)
#define ATTR0_TYPE_BLENDED    (1<<10)
#define ATTR0_TYPE_WINDOWED   (2<<10)

#define ATTR0_MOSAIC          (1<<12)

#define ATTR0_COLOR_16        (0<<13)
#define ATTR0_COLOR_256       (1<<13)

#define ATTR0_SQUARE          (0<<14)
#define ATTR0_WIDE            (1<<14)
#define ATTR0_TALL            (2<<14)
  
// Atribute 1 consists of 9 bits of X plus the following flags:
#define ATTR1_ROTDATA(n)      ((n)<<9)  // note: overlaps with flip flags
#define ATTR1_FLIP_X          (1<<12)
#define ATTR1_FLIP_Y          (1<<13)
#define ATTR1_SIZE_8          (0<<14)
#define ATTR1_SIZE_16         (1<<14)
#define ATTR1_SIZE_32         (2<<14)
#define ATTR1_SIZE_64         (3<<14)

// Atribute 2 consists of the following:
#define ATTR2_PRIORITY(n)     ((n)<<10)
#define ATTR2_PALETTE(n)      ((n)<<12)

//////////////////////////////////////////////////////////////////////
// Sprite structures
//////////////////////////////////////////////////////////////////////

typedef struct sSpriteEntry {
  uint16 attribute[3];
  uint16 filler;
} SpriteEntry, * pSpriteEntry;


typedef struct sSpriteRotation {
  uint16 filler1[3];
  uint16 hdx;

  uint16 filler2[3];
  uint16 hdy;  
    
  uint16 filler3[3];
  uint16 vdx;  

  uint16 filler4[3];
  uint16 vdy;
} SpriteRotation, * pSpriteRotation;

//////////////////////////////////////////////////////////////////////
// 3D core control
//////////////////////////////////////////////////////////////////////

#define GFX_CONTROL           (*(vuint16*) 0x04000060)

#define GFX_FIFO              (*(vuint32*) 0x04000400)  
#define GFX_STATUS            (*(vuint32*) 0x04000600)
#define GFX_COLOR             (*(vuint32*) 0x04000480)

#define GFX_VERTEX10          (*(vuint32*) 0x04000490)
#define GFX_VERTEX_XY          (*(vuint32*) 0x04000494)
#define GFX_VERTEX_XZ          (*(vuint32*) 0x04000498)
#define GFX_VERTEX_YZ          (*(vuint32*) 0x0400049C)
#define GFX_VERTEX_DIFF          (*(vuint32*) 0x040004A0)

#define GFX_VERTEX16          (*(vuint32*) 0x0400048C)
#define GFX_TEX_COORD         (*(vuint32*) 0x04000488)
#define GFX_TEX_FORMAT        (*(vuint32*) 0x040004A8)

#define GFX_CLEAR_COLOR       (*(vuint32*) 0x04000350)
#define GFX_CLEAR_DEPTH       (*(vuint16*) 0x04000354)

#define GFX_LIGHT_VECTOR      (*(vuint32*) 0x040004C8)
#define GFX_LIGHT_COLOR       (*(vuint32*) 0x040004CC)
#define GFX_NORMAL            (*(vuint32*) 0x04000484)

#define GFX_DIFFUSE_AMBIENT   (*(vuint32*) 0x040004C0)
#define GFX_SPECULAR_EMISSION (*(vuint32*) 0x040004C4)
#define GFX_SHININESS         (*(vuint32*) 0x040004D0)

#define GFX_POLY_FORMAT       (*(vuint32*) 0x040004A4)

#define GFX_BEGIN             (*(vuint32*) 0x04000500)
#define GFX_END               (*(vuint32*) 0x04000504)
#define GFX_FLUSH             (*(vuint32*) 0x04000540)
#define GFX_VIEWPORT          (*(vuint32*) 0x04000580)
#define GFX_TOON_TABLE		  ((vuint16*)  0x04000380)
#define GFX_EDGE_TABLE		  ((vuint16*)  0x04000330)

//////////////////////////////////////////////////////////////////////
// Matrix processor control
//////////////////////////////////////////////////////////////////////

#define MATRIX_CONTROL    (*(vuint32*)0x04000440)
#define MATRIX_PUSH       (*(vuint32*)0x04000444)
#define MATRIX_POP        (*(vuint32*)0x04000448)
#define MATRIX_SCALE      (*(vfixed*) 0x0400046C)
#define MATRIX_TRANSLATE  (*(vfixed*) 0x04000470)
#define MATRIX_RESTORE    (*(vuint32*)0x04000450)
#define MATRIX_STORE      (*(vuint32*)0x0400044C)
#define MATRIX_IDENTITY   (*(vuint32*)0x04000454)
#define MATRIX_LOAD4x4    (*(vfixed*) 0x04000458)
#define MATRIX_LOAD4x3    (*(vfixed*) 0x0400045C)
#define MATRIX_MULT4x4    (*(vfixed*) 0x04000460)
#define MATRIX_MULT4x3    (*(vfixed*) 0x04000464)
#define MATRIX_MULT3x3    (*(vfixed*) 0x04000468)

//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif

//////////////////////////////////////////////////////////////////////
