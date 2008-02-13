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
const char* BgUsage = 
"______________________________\n"
"|Mode | BG0 | BG1 | BG2 |BG3 |\n"
"|  0  |  T  |  T  |  T  |  T |\n"
"|  1  |  T  |  T  |  T  |  R |\n"
"|  2  |  T  |  T  |  R  |  R |\n"
"|  3  |  T  |  T  |  T  |  E |\n"
"|  4  |  T  |  T  |  R  |  E |\n"
"|  5  |  T  |  T  |  E  |  E |\n"
"-----------------------------\n"
"T = Text\n"
"R = Rotation\n"
"E = Extended Rotation (Bitmap or tiled)\n";

//look up tables for smoothing register access between the two 
//displays
vuint16* bgControl[8] = 
{
	&BG0_CR,
	&BG1_CR,
	&BG2_CR,
	&BG3_CR,
	&SUB_BG0_CR,
	&SUB_BG1_CR,
	&SUB_BG2_CR,
	&SUB_BG3_CR,
};

bg_scroll* bgScrollTable[8] = 
{
	&BG_OFFSET[0],
	&BG_OFFSET[1],
	&BG_OFFSET[2],
	&BG_OFFSET[3],
	
	&BG_OFFSET_SUB[0],
	&BG_OFFSET_SUB[1],
	&BG_OFFSET_SUB[2],
	&BG_OFFSET_SUB[3]
};

bg_rotation* bgRotation[8] = 
{
	(bg_rotation*)0,
	(bg_rotation*)0,
	(bg_rotation*)0x04000020,
	(bg_rotation*)0x04000030,
	
	(bg_rotation*)0,
	(bg_rotation*)0,
	(bg_rotation*)0x04001020,
	(bg_rotation*)0x04001030,
};

BgState bgState[8];



bool bgIsText(int id)
{
	if(id < 2 || (id > 3 && id < 6)) return true;
	
	u8 mode = (id < 4) ? (videoGetMode() & 7) : (videoGetModeSub() & 7);
	
	switch (mode)
	{
		case 0:
			return true;
		case 1:
		case 3:
			return id == 3 || id == 7 ? true : false;	
	}
	
	return false;
}


//rotates the background to the supplied angle
void bgSetRotate(int id, int angle)
{
	s16 angleSin;
	s16 angleCos;
 
	s32 pa, pb, pc, pd; 
	
	// wrap angle
	bgState[id].angle = angle & 0x1ff;

	// Compute sin and cos
	angleSin = sinFixed(angle);//SIN[bgState[id].angle];
	angleCos = cosFixed(angle);//COS[bgState[id].angle];
 
	// Set the background registers
	pa = ( angleCos * bgState[id].scaleX ) >> 12;
	pb = (-angleSin * bgState[id].scaleX ) >> 12;
	pc = ( angleSin * bgState[id].scaleY ) >> 12;
	pd = ( angleCos * bgState[id].scaleY ) >> 12;

	bgRotation[id]->xdx = pa;
	bgRotation[id]->xdy = pb;
	bgRotation[id]->ydx = pc;
	bgRotation[id]->ydy = pd;

	bgRotation[id]->centerX  = (bgState[id].scrollX) - ((pa * bgState[id].centerX + pb * bgState[id].centerY) >> 8);
	bgRotation[id]->centerY  = (bgState[id].scrollY) - ((pc * bgState[id].centerX + pd * bgState[id].centerY) >> 8);
	
}

//initializes and enables the appropriate background with the supplied attributes
//returns an id which must be supplied to the remainder of the background functions

int bgInit_call(int layer, BgType type, BgSize size, int mapBase, int tileBase)
{
	
	BGCTRL[layer] = BG_MAP_BASE(mapBase) | BG_TILE_BASE(tileBase) 
					| size | ((type == BgType_Text) ? BG_256_COLOR : 0);
	
	memset(&bgState[layer], sizeof(BgState), 0);

	if(type != BgType_Text)
	{		
		bgState[layer].scaleX = 1 << 8;
		bgState[layer].scaleY = 1 << 8;
		bgRotate(layer, 0);
	}

	bgState[layer].type = type;
	bgState[layer].size = size;
	
	videoBgEnable(layer);
	
	return layer;
}


int bgInitSub_call(int layer, BgType type, BgSize size, int mapBase, int tileBase)
{
 	BGCTRL_SUB[layer] = BG_MAP_BASE(mapBase) | BG_TILE_BASE(tileBase) 
					| size | ((type == BgType_Text) ? BG_256_COLOR : 0) ;

	memset(&bgState[layer + 4], sizeof(BgState), 0);

	if(type != BgType_Text)
	{		
		bgSetScale(layer + 4, 1 << 8, 1 << 8);
	}
	
	bgState[layer + 4].type = type;
	bgState[layer + 4].size = size;
	
	videoBgEnableSub(layer);

	return layer + 4;
} 

