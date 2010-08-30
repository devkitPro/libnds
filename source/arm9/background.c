/*---------------------------------------------------------------------------------

background.c -- DS Background Control

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

#include <nds/arm9/background.h>
#include <nds/arm9/trig_lut.h>

#include <string.h>

//const char* BgUsage =
//"______________________________\n"
//"|Mode | BG0 | BG1 | BG2 | BG3 |\n"
//"|  0  |  T  |  T  |  T  |  T  |\n"
//"|  1  |  T  |  T  |  T  |  R  |\n"
//"|  2  |  T  |  T  |  R  |  R  |\n"
//"|  3  |  T  |  T  |  T  |  E  |\n"
//"|  4  |  T  |  T  |  R  |  E  |\n"
//"|  5  |  T  |  T  |  E  |  E  |\n"
//"|_____|_____|_____|_____|_____|\n"
//"T = Text\n"
//"R = Rotation\n"
//"E = Extended Rotation (Bitmap or tiled)\n";

//look up tables for smoothing register access between the two
//displays
vu16* bgControl[8] = {
	&REG_BG0CNT,
	&REG_BG1CNT,
	&REG_BG2CNT,
	&REG_BG3CNT,
	&REG_BG0CNT_SUB,
	&REG_BG1CNT_SUB,
	&REG_BG2CNT_SUB,
	&REG_BG3CNT_SUB,
};

bg_scroll* bgScrollTable[8] = {
	&BG_OFFSET[0],
	&BG_OFFSET[1],
	&BG_OFFSET[2],
	&BG_OFFSET[3],

	&BG_OFFSET_SUB[0],
	&BG_OFFSET_SUB[1],
	&BG_OFFSET_SUB[2],
	&BG_OFFSET_SUB[3]
};

bg_transform* bgTransform[8] = {
	(bg_transform*)0,
	(bg_transform*)0,
	(bg_transform*)0x04000020,
	(bg_transform*)0x04000030,

	(bg_transform*)0,
	(bg_transform*)0,
	(bg_transform*)0x04001020,
	(bg_transform*)0x04001030,
};

BgState bgState[8];

bool bgIsTextLut[8];

bool bgIsText(int id) {
	return bgIsTextLut[id];
}

bool checkIfText(int id) {
	if(id < 2 || (id > 3 && id < 6)) return true;

	u8 mode = (id < 4) ? (videoGetMode() & 7) : (videoGetModeSub() & 7);

	if(!mode) return true;

	if(mode == 1 || mode == 3) {
		return id == 3 || id == 7 ? false : true;
	}

	return false;
}

void bgUpdate(void) {

	int i = 0;

	for(i = 0; i < 8; i++)
	{
		if(!bgState[i].dirty) continue;

		if(bgIsTextLut[i])
		{

			bgScrollTable[i]->x = bgState[i].scrollX >> 8;
			bgScrollTable[i]->y = bgState[i].scrollY >> 8;

		}
		else
		{
			s16 angleSin;
			s16 angleCos;

			s32 pa, pb, pc, pd;

			// Compute sin and cos
			angleSin = sinLerp(bgState[i].angle);
			angleCos = cosLerp(bgState[i].angle);

			// Set the background registers
			pa = ( angleCos * bgState[i].scaleX ) >> 12;
			pb = (-angleSin * bgState[i].scaleX ) >> 12;
			pc = ( angleSin * bgState[i].scaleY ) >> 12;
			pd = ( angleCos * bgState[i].scaleY ) >> 12;

			bgTransform[i]->hdx = pa;
			bgTransform[i]->vdx = pb;
			bgTransform[i]->hdy = pc;
			bgTransform[i]->vdy = pd;

			bgTransform[i]->dx  = (bgState[i].scrollX) - ((pa * bgState[i].centerX + pb * bgState[i].centerY) >> 8);
			bgTransform[i]->dy  = (bgState[i].scrollY) - ((pc * bgState[i].centerX + pd * bgState[i].centerY) >> 8);
		}

		bgState[i].dirty = false;
	}
}



//initializes and enables the appropriate background with the supplied attributes
//returns an id which must be supplied to the remainder of the background functions
int bgInit_call(int layer, BgType type, BgSize size, int mapBase, int tileBase) {

	BGCTRL[layer] = BG_MAP_BASE(mapBase) | BG_TILE_BASE(tileBase)
		| size | ((type == BgType_Text8bpp) ? BG_COLOR_256 : 0);

	memset(&bgState[layer], 0, sizeof(BgState) );

	if(type != BgType_Text8bpp && type != BgType_Text4bpp) {
		bgSetScale(layer, 1 << 8, 1 << 8);
		bgRotate(layer, 0);
	}

	bgState[layer].type = type;
	bgState[layer].size = size;

	videoBgEnable(layer);

	bgIsTextLut[layer] = checkIfText(layer);

	bgState[layer].dirty = true;

	bgUpdate();

	return layer;
}


int bgInitSub_call(int layer, BgType type, BgSize size, int mapBase, int tileBase) {

	BGCTRL_SUB[layer] = BG_MAP_BASE(mapBase) | BG_TILE_BASE(tileBase)
		| size | ((type == BgType_Text8bpp) ? BG_COLOR_256 : 0) ;

	memset(&bgState[layer + 4], 0, sizeof(BgState) );

	if(type != BgType_Text8bpp && type != BgType_Text4bpp) {
		bgSetScale(layer + 4, 1 << 8, 1 << 8);
		bgRotate(layer + 4, 0);
	}

	bgState[layer + 4].type = type;
	bgState[layer + 4].size = size;

	videoBgEnableSub(layer);
	bgIsTextLut[layer + 4] = checkIfText(layer + 4);

	bgState[layer + 4].dirty = true;

	bgUpdate();

	return layer + 4;
}

