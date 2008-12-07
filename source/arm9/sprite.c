#include <nds/arm9/sprite.h>

#include <nds/arm9/input.h>
#include <nds/arm9/console.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/cache.h>
#include <nds/dma.h>
#include <nds/bios.h>

#include <stdio.h>	// for NULL 

SpriteEntry OamMemorySub[128];
SpriteEntry OamMemory[128];

OamState oamMain = {
	-1, 
	0, 
	NULL, 
	32, 
	{OamMemory}
};

OamState oamSub = {
	-1, 
	0, 
	NULL, 
	32, 
	{OamMemorySub}
};

//---------------------------------------------------------------------------------
void oamInit(OamState *oam, SpriteMapping mapping, bool extPalette) {
//---------------------------------------------------------------------------------
	int i;
	int extPaletteFlag = extPalette ? 1 : 0;

	oam->gfxOffsetStep = (mapping & 3) + 5;

	dmaFillWords(0, oam->oamMemory, sizeof(OamMemory));

	for(i = 0; i < 128; i++) {
		oam->oamMemory[i].isHidden = true;
	}

	for(i = 0; i < 32; i++) {
		oam->oamRotationMemory[i].hdx = (1<<8);
		oam->oamRotationMemory[i].vdy = (1<<8);
	}

	swiWaitForVBlank();

	DC_FlushRange(oam->oamMemory, sizeof(OamMemory));

	if(oam == &oamMain) {
		dmaCopy(oam->oamMemory, OAM, sizeof(OamMemory));
		
		REG_DISPCNT &= ~DISPLAY_SPRITE_ATTR_MASK;
		REG_DISPCNT |= DISPLAY_SPR_ACTIVE | (mapping & 0xffffff0) | extPaletteFlag;      
	} else {
		dmaCopy(oam->oamMemory, OAM_SUB, sizeof(OamMemory));

		REG_DISPCNT_SUB &= ~DISPLAY_SPRITE_ATTR_MASK;
		REG_DISPCNT_SUB |= DISPLAY_SPR_ACTIVE | (mapping & 0xffffff0) | extPaletteFlag;      
	}
}

//---------------------------------------------------------------------------------
void oamDisable(OamState *oam) {
//---------------------------------------------------------------------------------
	if(oam == &oamMain) {
		REG_DISPCNT &= ~DISPLAY_SPR_ACTIVE;
	} else {
		REG_DISPCNT_SUB &= ~DISPLAY_SPR_ACTIVE;
	}
}

//---------------------------------------------------------------------------------
void oamEnable(OamState *oam) {
//---------------------------------------------------------------------------------
	if(oam == &oamMain) {
		REG_DISPCNT |= DISPLAY_SPR_ACTIVE;
	} else {
		REG_DISPCNT_SUB |= DISPLAY_SPR_ACTIVE;
	}
}

//---------------------------------------------------------------------------------
u16* oamGetGfxPtr(OamState *oam, int gfxOffsetIndex) {
//---------------------------------------------------------------------------------
	if(gfxOffsetIndex < 0) return NULL;

	if(oam == &oamMain) {
		return &SPRITE_GFX[(gfxOffsetIndex << oam->gfxOffsetStep) >> 1];
	} else {
		return &SPRITE_GFX_SUB[(gfxOffsetIndex << oam->gfxOffsetStep) >> 1];
	}
}


//---------------------------------------------------------------------------------
void oamClear(OamState *oam, int start, int count) {
//---------------------------------------------------------------------------------
	int i = 0;

	if(count == 0) {
		count = 128;
		start = 0;
	}

	for(i = start; i < count + start; i++) {

		oam->oamMemory[i].attribute[0] = ATTR0_DISABLED;
	}

}

//---------------------------------------------------------------------------------
unsigned int oamGfxPtrToOffset(OamState *oam, const void* offset) {
//---------------------------------------------------------------------------------
	unsigned int temp = (unsigned int)offset;

	if(oam == &oamMain)
		temp -= (unsigned int)SPRITE_GFX;
	else
		temp -= (unsigned int)SPRITE_GFX_SUB;

	temp >>= oam->gfxOffsetStep;

	return temp;
}

//---------------------------------------------------------------------------------
void oamSet(OamState* oam,	int id,  int x, int y, int priority,
 							int palette_alpha, SpriteSize size,SpriteColorFormat format,
 							const void* gfxOffset,
 							int affineIndex,
 							bool sizeDouble, bool hide) {  
//---------------------------------------------------------------------------------

	if(hide) {
		oam->oamMemory[id].attribute[0] = ATTR0_DISABLED;
		return;
	}

	oam->oamMemory[id].shape = (size >> 12) & 0x3;
	oam->oamMemory[id].size = (size >> 14) & 0x3;
	oam->oamMemory[id].gfxIndex = oamGfxPtrToOffset(oam, gfxOffset);
	oam->oamMemory[id].x = x;
	oam->oamMemory[id].y = y;
	oam->oamMemory[id].palette = palette_alpha; 
	oam->oamMemory[id].priority = priority;

	if(affineIndex >= 0 && affineIndex < 32) {
		oam->oamMemory[id].rotationIndex = affineIndex;
		oam->oamMemory[id].isSizeDouble = sizeDouble;
		oam->oamMemory[id].isRotateScale = true;
	} else {
		oam->oamMemory[id].rotationIndex = 0;
		oam->oamMemory[id].isSizeDouble = false;
		oam->oamMemory[id].isRotateScale = false;
	}

	if(format != SpriteColorFormat_Bmp) {
		oam->oamMemory[id].colorMode = format;
	} else {  
		oam->oamMemory[id].blendMode = format;
	}
}

//---------------------------------------------------------------------------------
void oamUpdate(OamState* oam) {
//---------------------------------------------------------------------------------
	DC_FlushRange(oam->oamMemory, sizeof(OamMemory));

	if(oam == &oamMain) {
		dmaCopy(oam->oamMemory, OAM, sizeof(OamMemory));
	} else {
		dmaCopy(oam->oamMemory, OAM_SUB, sizeof(OamMemory));
	}
}

//---------------------------------------------------------------------------------
void oamRotateScale(OamState* oam, int rotId, int angle, int sx, int sy) {
//---------------------------------------------------------------------------------
	int ss = sinLerp(angle);
	int cc = cosLerp(angle);

	oam->oamRotationMemory[rotId].hdx = cc*sx>>12; 
	oam->oamRotationMemory[rotId].hdy =-ss*sx>>12;
	oam->oamRotationMemory[rotId].vdx = ss*sy>>12;;
	oam->oamRotationMemory[rotId].vdy = cc*sy>>12;
}

