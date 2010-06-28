/*---------------------------------------------------------------------------------

	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)
		Chris Double (doublec)

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

/*
	This file should be deprecated.

	All hardware register defines should be replaced with REG_ for consistency and namespacing

	http://forum.gbadev.org/viewtopic.php?t=4993

*/
#ifndef	NDS_REGISTERS_H
#define	NDS_REGISTERS_H

#warning "header provided for assistance in porting to new register names, do not use for release code"


#include <nds/ndstypes.h>

// math registers
#define DIV_CR				REG_DIVCNT

#define DIV_NUMERATOR64		REG_DIV_NUMER
#define DIV_NUMERATOR32		REG_DIV_NUMER_L

#define DIV_DENOMINATOR64	REG_DIV_DENOM
#define DIV_DENOMINATOR32	REG_DIV_DENOM_L

#define DIV_RESULT64		REG_DIV_RESULT
#define DIV_RESULT32		REG_DIV_RESULT_L

#define DIV_REMAINDER64 	REG_DIVREM_RESULT
#define DIV_REMAINDER32		REG_DIVREM_RESULT_L

#define SQRT_CR				REG_SQRTCNT

#define SQRT_PARAM64		REG_SQRT_PARAM
#define SQRT_PARAM32		REG_SQRT_PARAM_L

#define SQRT_RESULT32		REG_SQRT_RESULT


// graphics registers

#define DISPLAY_CR		REG_DISPCNT

#ifdef ARM9
#define WAIT_CR			REG_EXMEMCNT
#else
#define WAIT_CR			REG_EXMEMSTAT
#endif

#define DISP_SR			REG_DISPSTAT
#define DISP_Y			REG_VCOUNT

#define BG0_CR			REG_BG0CNT
#define BG1_CR			REG_BG1CNT
#define BG2_CR			REG_BG2CNT
#define BG3_CR			REG_BG3CNT

#define BG0_X0			REG_BG0HOFS
#define BG0_Y0			REG_BG0VOFS
#define BG1_X0			REG_BG1HOFS
#define BG1_Y0			REG_BG1VOFS
#define BG2_X0			REG_BG2HOFS
#define BG2_Y0			REG_BG2VOFS
#define BG3_X0			REG_BG3HOFS
#define BG3_Y0			REG_BG3VOFS

#define BG2_XDX			REG_BG2PA
#define BG2_XDY			REG_BG2PB
#define BG2_YDX			REG_BG2PC
#define BG2_YDY			REG_BG2PD
#define BG2_CX			REG_BG2X
#define BG2_CY			REG_BG2Y

#define BG3_XDX			REG_BG3PA
#define BG3_XDY			REG_BG3PB
#define BG3_YDX			REG_BG3PC
#define BG3_YDY			REG_BG3PD
#define BG3_CX			REG_BG3X
#define BG3_CY			REG_BG3Y

#define	REG_WIN0H		(*(vu16*)0x4000040)
#define	REG_WIN1H		(*(vu16*)0x4000042)
#define	REG_WIN0V		(*(vu16*)0x4000044)
#define	REG_WIN1V		(*(vu16*)0x4000046)
#define	REG_WININ		(*(vu16*)0x4000048)
#define	REG_WINOUT		(*(vu16*)0x400004A)


#define MOSAIC_CR		REG_MOSAIC

#define BLEND_CR		REG_BLDCNT
#define BLEND_AB		REG_BLDALPHA
#define BLEND_Y     	REG_BLDY

#define SUB_BLEND_CR	REG_BLDCNT_SUB
#define SUB_BLEND_AB	REG_BLDALPHA_SUB
#define SUB_BLEND_Y		REG_BLDY_SUB


#define SERIAL_CR       REG_SPICNT
#define SERIAL_DATA     REG_SPIDATA
#define SIO_CR          REG_SIOCNT
#define R_CR            REG_RCNT

#define	DISP_CAPTURE	REG_DISPCAPCNT

#define BRIGHTNESS		REG_MASTER_BRIGHT
#define SUB_BRIGHTNESS	REG_MASTER_BRIGHT_SUB

/*	secondary screen */
#define SUB_DISPLAY_CR   REG_DISPCNT_SUB

#define SUB_BG0_CR		REG_BG0CNT_SUB
#define SUB_BG1_CR		REG_BG1CNT_SUB
#define SUB_BG2_CR		REG_BG2CNT_SUB
#define SUB_BG3_CR		REG_BG3CNT_SUB

#define SUB_BG0_X0		REG_BG0HOFS_SUB
#define SUB_BG0_Y0		REG_BG0VOFS_SUB
#define SUB_BG1_X0		REG_BG1HOFS_SUB
#define SUB_BG1_Y0		REG_BG1VOFS_SUB
#define SUB_BG2_X0		REG_BG2HOFS_SUB
#define SUB_BG2_Y0		REG_BG2VOFS_SUB
#define SUB_BG3_X0		REG_BG3HOFS_SUB
#define SUB_BG3_Y0		REG_BG3VOFS_SUB

#define SUB_BG2_XDX		REG_BG2PA_SUB
#define SUB_BG2_XDY		REG_BG2PB_SUB
#define SUB_BG2_YDX		REG_BG2PC_SUB
#define SUB_BG2_YDY		REG_BG2PD_SUB
#define SUB_BG2_CX		REG_BG2X_SUB
#define SUB_BG2_CY		REG_BG2Y_SUB

#define SUB_BG3_XDX		REG_BG3PA_SUB
#define SUB_BG3_XDY		REG_BG3PB_SUB
#define SUB_BG3_YDX		REG_BG3PC_SUB
#define SUB_BG3_YDY		REG_BG3PD_SUB
#define SUB_BG3_CX		REG_BG3X_SUB
#define SUB_BG3_CY		REG_BG3Y_SUB

#define	REG_WIN0H_SUB		(*(vu16*)0x4001040)
#define	REG_WIN1H_SUB		(*(vu16*)0x4001042)
#define	REG_WIN0V_SUB		(*(vu16*)0x4001044)
#define	REG_WIN1V_SUB		(*(vu16*)0x4001046)
#define	REG_WININ_SUB		(*(vu16*)0x4001048)
#define	REG_WINOUT_SUB		(*(vu16*)0x400104A)

#define	SUB_MOSAIC_CR		REG_MOSAIC_SUB

#define	REG_BLDMOD_SUB	(*(vu16*)0x4001050)
#define	REG_COLV_SUB	(*(vu16*)0x4001052)
#define	REG_COLY_SUB	(*(vu16*)0x4001054)

/*common*/

#define	REG_DMA0SAD		(*(vu32*)0x40000B0)
#define	REG_DMA0SAD_L	(*(vu16*)0x40000B0)
#define	REG_DMA0SAD_H	(*(vu16*)0x40000B2)
#define	REG_DMA0DAD		(*(vu32*)0x40000B4)
#define	REG_DMA0DAD_L	(*(vu16*)0x40000B4)
#define	REG_DMA0DAD_H	(*(vu16*)0x40000B6)
#define	REG_DMA0CNT		(*(vu32*)0x40000B8)
#define	REG_DMA0CNT_L	(*(vu16*)0x40000B8)
#define	REG_DMA0CNT_H	(*(vu16*)0x40000BA)

#define	REG_DMA1SAD		(*(vu32*)0x40000BC)
#define	REG_DMA1SAD_L	(*(vu16*)0x40000BC)
#define	REG_DMA1SAD_H	(*(vu16*)0x40000BE)
#define	REG_DMA1DAD		(*(vu32*)0x40000C0)
#define	REG_DMA1DAD_L	(*(vu16*)0x40000C0)
#define	REG_DMA1DAD_H	(*(vu16*)0x40000C2)
#define	REG_DMA1CNT		(*(vu32*)0x40000C4)
#define	REG_DMA1CNT_L	(*(vu16*)0x40000C4)
#define	REG_DMA1CNT_H	(*(vu16*)0x40000C6)

#define	REG_DMA2SAD		(*(vu32*)0x40000C8)
#define	REG_DMA2SAD_L	(*(vu16*)0x40000C8)
#define	REG_DMA2SAD_H	(*(vu16*)0x40000CA)
#define	REG_DMA2DAD		(*(vu32*)0x40000CC)
#define	REG_DMA2DAD_L	(*(vu16*)0x40000CC)
#define	REG_DMA2DAD_H	(*(vu16*)0x40000CE)
#define	REG_DMA2CNT		(*(vu32*)0x40000D0)
#define	REG_DMA2CNT_L	(*(vu16*)0x40000D0)
#define	REG_DMA2CNT_H	(*(vu16*)0x40000D2)

#define	REG_DMA3SAD		(*(vu32*)0x40000D4)
#define	REG_DMA3SAD_L	(*(vu16*)0x40000D4)
#define	REG_DMA3SAD_H	(*(vu16*)0x40000D6)
#define	REG_DMA3DAD		(*(vu32*)0x40000D8)
#define	REG_DMA3DAD_L	(*(vu16*)0x40000D8)
#define	REG_DMA3DAD_H	(*(vu16*)0x40000DA)
#define	REG_DMA3CNT		(*(vu32*)0x40000DC)
#define	REG_DMA3CNT_L	(*(vu16*)0x40000DC)
#define	REG_DMA3CNT_H	(*(vu16*)0x40000DE)

#define	REG_TIME		( (vu16*)0x4000100)
#define	REG_TM0D		(*(vu16*)0x4000100)
#define	REG_TM0CNT		(*(vu16*)0x4000102)
#define	REG_TM1D		(*(vu16*)0x4000106)
#define	REG_TM2D		(*(vu16*)0x4000108)
#define	REG_TM2CNT		(*(vu16*)0x400010A)
#define	REG_TM3D		(*(vu16*)0x400010C)
#define	REG_TM3CNT		(*(vu16*)0x400010E)


#define	REG_SIOCNT		(*(vu16*)0x4000128)
#define	REG_SIOMLT_SEND	(*(vu16*)0x400012A)

#define	KEYS		REG_KEYINPUT
#define	KEYS_CR		REG_KEYCNT
//??? (sio defines, no link port though)
#define	REG_RCNT		(*(vu16*)0x4000134)
#define	REG_HS_CTRL		(*(vu16*)0x4000140)

/* Interupt	enable registers */
#define	IE			REG_IE
#define	IF			REG_IF
#define	IME			REG_IME

/*controls power  0x30f	is all on */
#define POWER_CR	REG_POWERCNT

/* ram	controllers	0x8	is enabled,	other bits have	to do with mapping */
#define	REG_VRAM_A_CR	(*(vu8*) 0x4000240)
#define	REG_VRAM_B_CR	(*(vu8*) 0x4000241)
#define	REG_VRAM_C_CR	(*(vu8*) 0x4000242)
#define	REG_VRAM_D_CR	(*(vu8*) 0x4000243)
#define	REG_VRAM_E_CR	(*(vu8*) 0x4000244)
#define	REG_VRAM_F_CR	(*(vu8*) 0x4000245)
#define	REG_VRAM_G_CR	(*(vu8*) 0x4000246)
#define	REG_VRAM_H_CR	(*(vu8*) 0x4000248)
#define	REG_VRAM_I_CR	(*(vu8*) 0x4000249)
#define	REG_WRAM_CNT	(*(vu8*) 0x4000247)




/*3D graphics stuff*/
#define	REG_GFX_FIFO		(*(vu32*) 0x4000400)
#define	REG_GFX_STATUS		(*(vu32*) 0x4000600)
#define	REG_GFX_CONTROL		(*(vu16*) 0x4000060)
#define	REG_COLOR			(*(vu32*) 0x4000480)
#define	REG_VERTEX16		(*(vu32*) 0x400048C)
#define	REG_TEXT_COORD		(*(vu32*) 0x4000488)
#define	REG_TEXT_FORMAT		(*(vu32*) 0x40004A8)


#define	REG_CLEAR_COLOR		(*(vu32*) 0x4000350)
#define	REG_CLEAR_DEPTH		(*(vu16*) 0x4000354)

#define	REG_LIGHT_VECTOR	(*(vu32*) 0x40004C8)
#define	REG_LIGHT_COLOR		(*(vu32*) 0x40004CC)
#define	REG_NORMAL			(*(vu32*) 0x4000484)

#define	REG_DIFFUSE_AMBIENT		(*(vu32*) 0x40004C0)
#define	REG_SPECULAR_EMISSION	(*(vu32*) 0x40004C4)
#define	REG_SHININESS			(*(vu32*) 0x40004D0)

#define	REG_POLY_FORMAT		(*(vu32*) 0x40004A4)

#define	REG_GFX_BEGIN		(*(vu32*) 0x4000500)
#define	REG_GFX_END			(*(vu32*) 0x4000504)
#define	REG_GFX_FLUSH		(*(vu32*) 0x4000540)
#define	REG_GFX_VIEWPORT	(*(vu32*) 0x4000580)

#define	REG_MTX_CONTROL		(*(vu32*) 0x4000440)
#define	REG_MTX_PUSH		(*(vu32*) 0x4000444)
#define	REG_MTX_POP			(*(vu32*) 0x4000448)
#define	REG_MTX_SCALE		(*(vint32*) 0x400046C)
#define	REG_MTX_TRANSLATE	(*(vint32*) 0x4000470)
#define	REG_MTX_RESTORE		(*(vu32*) 0x4000450)
#define	REG_MTX_STORE		(*(vu32*) 0x400044C)
#define	REG_MTX_IDENTITY	(*(vu32*) 0x4000454)
#define	REG_MTX_LOAD4x4		(*(volatile	f32*) 0x4000458)
#define	REG_MTX_LOAD4x3		(*(volatile	f32*) 0x400045C)
#define	REG_MTX_MULT4x4		(*(volatile	f32*) 0x4000460)
#define	REG_MTX_MULT4x3		(*(volatile	f32*) 0x4000464)
#define	REG_MTX_MULT3x3		(*(volatile	f32*) 0x4000468)

// Card	bus

#define	CARD_CR1		REG_AUXSPICNT
#define	CARD_CR1H		REG_AUXSPICNTH
#define	CARD_CR2		REG_ROMCTRL
#define CARD_EEPDATA	REG_AUXSPIDATA

#define	REG_CARD_DATA		(*(vu32*)0x04100000)
#define	REG_CARD_DATA_RD	(*(vu32*)0x04100010)

#define	REG_CARD_1B0		(*(vu32*)0x040001B0)
#define	REG_CARD_1B4		(*(vu32*)0x040001B4)
#define	REG_CARD_1B8		(*(vu16*)0x040001B8)
#define	REG_CARD_1BA		(*(vu16*)0x040001BA)

// sound

#define SOUND_CR			REG_SOUNDCNT
#define SOUND_MASTER_VOL	REG_MASTER_VOLUME

#define SOUND_BIAS			REG_SOUNDBIAS

#endif
