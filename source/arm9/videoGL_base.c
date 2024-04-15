/*---------------------------------------------------------------------------------

	Video API vaguely similar to OpenGL

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

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.


---------------------------------------------------------------------------------*/

#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>

u32 glCurClearColor;

//---------------------------------------------------------------------------------
void glRotatef32i(int angle, s32 x, s32 y, s32 z) {
//---------------------------------------------------------------------------------
	s32 axis[3];
	s32 sine = sinLerp(angle);//SIN[angle &  LUT_MASK];
	s32 cosine = cosLerp(angle);//COS[angle & LUT_MASK];
	s32 one_minus_cosine = inttof32(1) - cosine;

	axis[0]=x;
	axis[1]=y;
	axis[2]=z;

	normalizef32(axis);   // should require passed in normalized?

	MATRIX_MULT3x3 = cosine + mulf32(one_minus_cosine, mulf32(axis[0], axis[0]));
	MATRIX_MULT3x3 = mulf32(one_minus_cosine, mulf32(axis[0], axis[1])) + mulf32(axis[2], sine);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]), axis[2]) - mulf32(axis[1], sine);

	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]),  axis[1]) - mulf32(axis[2], sine);
	MATRIX_MULT3x3 = cosine + mulf32(mulf32(one_minus_cosine, axis[1]), axis[1]);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[1]), axis[2]) + mulf32(axis[0], sine);

	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]), axis[2]) + mulf32(axis[1], sine);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[1]), axis[2]) - mulf32(axis[0], sine);
	MATRIX_MULT3x3 = cosine + mulf32(mulf32(one_minus_cosine, axis[2]), axis[2]);
}

//---------------------------------------------------------------------------------
void glMaterialf(GL_MATERIALS_ENUM mode, rgb color) {
//---------------------------------------------------------------------------------
	static u32 diffuse_ambient = 0;
	static u32 specular_emission = 0;

	switch(mode) {
		case GL_AMBIENT:
			diffuse_ambient = (color << 16) | (diffuse_ambient & 0xFFFF);
			break;
		case GL_DIFFUSE:
			diffuse_ambient = color | (diffuse_ambient & 0xFFFF0000);
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			diffuse_ambient= color + (color << 16);
			break;
		case GL_SPECULAR:
			specular_emission = color | (specular_emission & 0xFFFF0000);
			break;
		case GL_SHININESS:
			break;
		case GL_EMISSION:
			specular_emission = (color << 16) | (specular_emission & 0xFFFF);
			break;
	}

	GFX_DIFFUSE_AMBIENT = diffuse_ambient;
	GFX_SPECULAR_EMISSION = specular_emission;
}

//---------------------------------------------------------------------------------
MK_WEAK void _glInitGlobals(void) {
//---------------------------------------------------------------------------------
	// Dummy implementation when not using the texture VRAM manager
}

//---------------------------------------------------------------------------------
void glInit(void) {
//---------------------------------------------------------------------------------
	powerOn(POWER_3D_CORE | POWER_MATRIX);	// enable 3D core & geometry engine

	// Initialize texture VRAM manager (if needed)
	_glInitGlobals();

	while (GFX_STATUS & (1<<27)); // wait till gfx engine is not busy

	// Clear the FIFO
	GFX_STATUS |= (1<<29);

	// Clear overflows from list memory
	glResetMatrixStack();

	// prime the vertex/polygon buffers
	glFlush(0);

	// reset the control bits
	GFX_CONTROL = 0;

	// reset the rear-plane(a.k.a. clear color) to black, ID=0, and opaque
	glCurClearColor = 0;
	glClearColor(0,0,0,31);
	glClearPolyID(0);

	// reset the depth to it's max
	glClearDepth(GL_MAX_DEPTH);

	GFX_TEX_FORMAT = 0;
	GFX_POLY_FORMAT = 0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
}
