/*---------------------------------------------------------------------------------
	$Id: videoGL.c,v 1.13 2005-09-19 20:59:47 dovoto Exp $

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

	$Log: not supported by cvs2svn $
	Revision 1.12  2005/08/23 17:06:10  wntrmute
	converted all endings to unix
	
	Revision 1.11  2005/08/22 08:05:53  wntrmute
	moved inlines to separate file
	
	Revision 1.10  2005/08/01 23:18:22  wntrmute
	adjusted headers for logging
	
	Revision 1.9  2005/07/29 05:19:55  dovoto
	Had s and t swapped in glTextCoord2f
	
	Revision 1.8  2005/07/27 15:54:58  wntrmute
	Synchronise with ndslib.
	
	Added f32 version of glTextCoord
	fixed glBindTexture (0,0) now clears fomatting
	
	Revision 1.7  2005/07/27 02:20:05  wntrmute
	resynchronise with ndslib
	Updated GL with float wrappers for NeHe
	
	Revision 1.6  2005/07/14 08:00:57  wntrmute
	resynchronise with ndslib
	

---------------------------------------------------------------------------------*/

#include <nds/jtypes.h>
#include <nds/memory.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>


static uint16 enable_bits = GL_TEXTURE_2D | (1<<13) | (1<<14);


//---------------------------------------------------------------------------------
void glEnable(int bits) {
//---------------------------------------------------------------------------------
	enable_bits |= bits | (GL_TEXTURE_2D|GL_TOON_HIGHLIGHT|GL_OUTLINE|GL_ANTIALIAS);
	GFX_CONTROL = enable_bits;
}


//---------------------------------------------------------------------------------
void glDisable(int bits) {
//---------------------------------------------------------------------------------
	enable_bits &= ~(bits | (GL_TEXTURE_2D|GL_TOON_HIGHLIGHT|GL_OUTLINE|GL_ANTIALIAS));
	GFX_CONTROL = enable_bits;
}

//---------------------------------------------------------------------------------
void glLoadMatrix4x4(m4x4 * m) {
//---------------------------------------------------------------------------------
	MATRIX_LOAD4x4 = m->m[0];
	MATRIX_LOAD4x4 = m->m[1];
	MATRIX_LOAD4x4 = m->m[2];
	MATRIX_LOAD4x4 = m->m[3];

	MATRIX_LOAD4x4 = m->m[4];
	MATRIX_LOAD4x4 = m->m[5];
	MATRIX_LOAD4x4 = m->m[6];
	MATRIX_LOAD4x4 = m->m[7];

	MATRIX_LOAD4x4 = m->m[8];
	MATRIX_LOAD4x4 = m->m[9];
	MATRIX_LOAD4x4 = m->m[10];
	MATRIX_LOAD4x4 = m->m[11];

	MATRIX_LOAD4x4 = m->m[12];
	MATRIX_LOAD4x4 = m->m[13];
	MATRIX_LOAD4x4 = m->m[14];
	MATRIX_LOAD4x4 = m->m[15];
}

//---------------------------------------------------------------------------------
void glLoadMatrix4x3(m4x3* m) {
//---------------------------------------------------------------------------------
	MATRIX_LOAD4x3 = m->m[0];
	MATRIX_LOAD4x3 = m->m[1];
	MATRIX_LOAD4x3 = m->m[2];
	MATRIX_LOAD4x3 = m->m[3];

	MATRIX_LOAD4x3 = m->m[4];
	MATRIX_LOAD4x3 = m->m[5];
	MATRIX_LOAD4x3 = m->m[6];
	MATRIX_LOAD4x3 = m->m[7];

	MATRIX_LOAD4x3 = m->m[8];
	MATRIX_LOAD4x3 = m->m[9];
	MATRIX_LOAD4x3 = m->m[10];
	MATRIX_LOAD4x3 = m->m[11];
}

//---------------------------------------------------------------------------------
void glMultMatrix4x4(m4x4* m) {
//---------------------------------------------------------------------------------
	MATRIX_MULT4x4 = m->m[0];
	MATRIX_MULT4x4 = m->m[1];
	MATRIX_MULT4x4 = m->m[2];
	MATRIX_MULT4x4 = m->m[3];

	MATRIX_MULT4x4 = m->m[4];
	MATRIX_MULT4x4 = m->m[5];
	MATRIX_MULT4x4 = m->m[6];
	MATRIX_MULT4x4 = m->m[7];

	MATRIX_MULT4x4 = m->m[8];
	MATRIX_MULT4x4 = m->m[9];
	MATRIX_MULT4x4 = m->m[10];
	MATRIX_MULT4x4 = m->m[11];

	MATRIX_MULT4x4 = m->m[12];
	MATRIX_MULT4x4 = m->m[13];
	MATRIX_MULT4x4 = m->m[14];
	MATRIX_MULT4x4 = m->m[15];
}


//---------------------------------------------------------------------------------
void glMultMatrix4x3(m4x3* m) {
//---------------------------------------------------------------------------------
	MATRIX_MULT4x3 = m->m[0];
	MATRIX_MULT4x3 = m->m[1];
	MATRIX_MULT4x3 = m->m[2];
	MATRIX_MULT4x3 = m->m[3];

	MATRIX_MULT4x3 = m->m[4];
	MATRIX_MULT4x3 = m->m[5];
	MATRIX_MULT4x3 = m->m[6];
	MATRIX_MULT4x3 = m->m[7];

	MATRIX_MULT4x3 = m->m[8];
	MATRIX_MULT4x3 = m->m[9];
	MATRIX_MULT4x3 = m->m[10];
	MATRIX_MULT4x3 = m->m[11];
 
}

//---------------------------------------------------------------------------------
void glMultMatrix3x3(m3x3* m) {
//---------------------------------------------------------------------------------
	MATRIX_MULT3x3 = m->m[0];
	MATRIX_MULT3x3 = m->m[1];
	MATRIX_MULT3x3 = m->m[2];
  
	MATRIX_MULT3x3 = m->m[3];
	MATRIX_MULT3x3 = m->m[4];
	MATRIX_MULT3x3 = m->m[5];
  
	MATRIX_MULT3x3 = m->m[6];
	MATRIX_MULT3x3 = m->m[7];
	MATRIX_MULT3x3 = m->m[8];
}

//---------------------------------------------------------------------------------
// Integer rotation (not gl standard)
//	based on 512 degree circle
//---------------------------------------------------------------------------------
void glRotateZi(int angle) {
//---------------------------------------------------------------------------------
	f32 sine = SIN[angle &  LUT_MASK];
	f32 cosine = COS[angle & LUT_MASK];

	MATRIX_MULT3x3 = cosine;
	MATRIX_MULT3x3 = sine;
	MATRIX_MULT3x3 = 0;

	MATRIX_MULT3x3 = - sine;
	MATRIX_MULT3x3 = cosine;
	MATRIX_MULT3x3 = 0;
  
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = intof32(1);
}

//---------------------------------------------------------------------------------
void glRotateYi(int angle) {
//---------------------------------------------------------------------------------
	f32 sine = SIN[angle &  LUT_MASK];
	f32 cosine = COS[angle & LUT_MASK];

	MATRIX_MULT3x3 = cosine;
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = -sine;
  
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = intof32(1);
	MATRIX_MULT3x3 = 0;
  
	MATRIX_MULT3x3 = sine;
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = cosine;
}

//---------------------------------------------------------------------------------
void glRotateXi(int angle) {
//---------------------------------------------------------------------------------
	f32 sine = SIN[angle &  LUT_MASK];
	f32 cosine = COS[angle & LUT_MASK];

	MATRIX_MULT3x3 = intof32(1);
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = 0;

	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = cosine;
	MATRIX_MULT3x3 = sine;
  
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = -sine;
	MATRIX_MULT3x3 = cosine;
}

//---------------------------------------------------------------------------------
void glRotatef32i(int angle, f32 x, f32 y, f32 z) {
//---------------------------------------------------------------------------------

	f32 axis[3];
	f32 sine = SIN[angle &  LUT_MASK];
	f32 cosine = COS[angle & LUT_MASK];
	f32 one_minus_cosine = intof32(1) - cosine;

	axis[0]=x;
	axis[1]=y;
	axis[2]=z;

	normalizef32(axis);   // should require passed in normalized?

	MATRIX_MULT3x3 = cosine + mulf32(one_minus_cosine, mulf32(axis[0], axis[0]));
	MATRIX_MULT3x3 = mulf32(one_minus_cosine, mulf32(axis[0], axis[1])) - mulf32(axis[2], sine);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]), axis[2]) + mulf32(axis[1], sine);

	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]),  axis[1]) + mulf32(axis[2], sine);
	MATRIX_MULT3x3 = cosine + mulf32(mulf32(one_minus_cosine, axis[1]), axis[1]);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[1]), axis[2]) - mulf32(axis[0], sine);

	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]), axis[2]) - mulf32(axis[1], sine);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[1]), axis[2]) + mulf32(axis[0], sine);
	MATRIX_MULT3x3 = cosine + mulf32(mulf32(one_minus_cosine, axis[2]), axis[2]);

}

 


//---------------------------------------------------------------------------------
void glRotatef32(float angle, f32 x, f32 y, f32 z) {
//---------------------------------------------------------------------------------
    glRotatef32i((int)(angle * LUT_SIZE / 360.0), x, y, z);
}

//---------------------------------------------------------------------------------
void glRotatef(float angle, float x, float y, float z) {
//---------------------------------------------------------------------------------
	glRotatef32(angle, floatof32(x), floatof32(y), floatof32(z));
}


//---------------------------------------------------------------------------------
//	rotations wrapped in float...mainly for testing
//---------------------------------------------------------------------------------
void glRotateX(float angle) {
//---------------------------------------------------------------------------------
	glRotateXi((int)(angle * LUT_SIZE / 360.0));
}

//---------------------------------------------------------------------------------
void glRotateY(float angle) {
//---------------------------------------------------------------------------------
	glRotateYi((int)(angle * LUT_SIZE / 360.0));
}

//---------------------------------------------------------------------------------
void glRotateZ(float angle) {
//---------------------------------------------------------------------------------
	glRotateZi((int)(angle * LUT_SIZE / 360.0));
}

//---------------------------------------------------------------------------------
//float wrappers for porting
//---------------------------------------------------------------------------------
void glVertex3f(float x, float y, float z) {
//---------------------------------------------------------------------------------
	glVertex3v16(floatov16(x), floatov16(y), floatov16(z));
}

//---------------------------------------------------------------------------------
void glTexCoord2f(float s, float t) {
//---------------------------------------------------------------------------------
	glTexCoord2t16(floatot16(s*127), floatot16(t*127));
}

//---------------------------------------------------------------------------------
void glColor3f(float r, float g, float b) {
//---------------------------------------------------------------------------------
	glColor3b((uint8)(r*255), (uint8)(g*255), (uint8)(b*255));
}

//---------------------------------------------------------------------------------
void glScalef(float x, float y, float z) {
//---------------------------------------------------------------------------------
	MATRIX_SCALE = floatof32(x);
	MATRIX_SCALE = floatof32(y);
	MATRIX_SCALE = floatof32(z);
}

//---------------------------------------------------------------------------------
void glTranslatef(float x, float y, float z) {
//---------------------------------------------------------------------------------
	MATRIX_TRANSLATE = floatof32(x);
    MATRIX_TRANSLATE = floatof32(y);
    MATRIX_TRANSLATE = floatof32(z);
}

//---------------------------------------------------------------------------------
void glNormal3f(float x, float y, float z) {
//---------------------------------------------------------------------------------
	if(x >= 1 || x <= -1) x *= .95;
	if(y >= 1 || y <= -1) y *= .95;
	if(z >= 1 || z <= -1) z *= .95;

	glNormal(NORMAL_PACK(floatov10(x), floatov10(y), floatov10(z)));
}



//---------------------------------------------------------------------------------
void glOrthof32(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar) {
//---------------------------------------------------------------------------------
	glMatrixMode(GL_PROJECTION);

	MATRIX_LOAD4x4 = divf32(intof32(2), right - left);     
	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = 0;      
	MATRIX_LOAD4x4 = divf32(right + left, right - left);

	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = divf32(intof32(2), top - bottom);     
	MATRIX_LOAD4x4 = 0;    
	MATRIX_LOAD4x4 = divf32(top + bottom, top - bottom); 
   
	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = divf32(intof32(-2), zFar - zNear);     
	MATRIX_LOAD4x4 = divf32(zFar + zNear, zFar - zNear);
   
	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = floatof32(1.0F);
	
	glStoreMatrix(0);
}

//---------------------------------------------------------------------------------
void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar) {
//---------------------------------------------------------------------------------
	glOrthof32(floatof32(left), floatof32(right), floatof32(bottom), floatof32(top), floatof32(zNear), floatof32(zFar));

}

//---------------------------------------------------------------------------------
// Fixed point look at function, it appears to work as expected although 
//	testing is recomended
//---------------------------------------------------------------------------------
void gluLookAtf32(f32 eyex, f32 eyey, f32 eyez, f32 lookAtx, f32 lookAty, f32 lookAtz, f32 upx, f32 upy, f32 upz)  { 
//---------------------------------------------------------------------------------
	f32 side[3], forward[3], up[3]; 

	forward[0] = lookAtx - eyex; 
	forward[1] = lookAty - eyey; 
	forward[2] = lookAtz - eyez; 

	normalizef32(forward); 

	up[0] = upx; 
	up[1] = upy; 
	up[2] = upz; 

	crossf32(forward, up, side); 

	normalizef32(side); 

	crossf32(side, forward, up); 

	glMatrixMode(GL_MODELVIEW); 

 
	// should we use MATRIX_MULT4x3 as in ogl|es?? 

	MATRIX_LOAD4x3 =  side[0]; 
	MATRIX_LOAD4x3 =  up[0]; 
	MATRIX_LOAD4x3 = -forward[0]; 

	MATRIX_LOAD4x3 =  side[1]; 
	MATRIX_LOAD4x3 =  up[1]; 
	MATRIX_LOAD4x3 = -forward[1]; 

	MATRIX_LOAD4x3 =  side[2]; 
	MATRIX_LOAD4x3 =  up[2]; 
	MATRIX_LOAD4x3 = -forward[2]; 

	MATRIX_LOAD4x3 = -eyex; 
	MATRIX_LOAD4x3 = -eyey; 
	MATRIX_LOAD4x3 = -eyez; 

}

 


//---------------------------------------------------------------------------------
//  glu wrapper for standard float call
//---------------------------------------------------------------------------------
void gluLookAt(	float eyex, float eyey, float eyez,
				float lookAtx, float lookAty, float lookAtz,
				float upx, float upy, float upz) {
//---------------------------------------------------------------------------------
	gluLookAtf32(floatof32(eyex), floatof32(eyey), floatof32(eyez), floatof32(lookAtx), floatof32(lookAty), floatof32(lookAtz),
					floatof32(upx), floatof32(upy), floatof32(upz));
}

//---------------------------------------------------------------------------------
//	frustrum has only been tested as part of perspective
//---------------------------------------------------------------------------------
void gluFrustumf32(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
//---------------------------------------------------------------------------------
  
	glMatrixMode(GL_PROJECTION);

	MATRIX_LOAD4x4 = divf32(2*near, right - left);     
	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = divf32(right + left, right - left);      
	MATRIX_LOAD4x4 = 0;

	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = divf32(2*near, top - bottom);     
	MATRIX_LOAD4x4 = divf32(top + bottom, top - bottom);      
	MATRIX_LOAD4x4 = 0;
   
	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = -divf32(far + near, far - near);     
	MATRIX_LOAD4x4 = floatof32(-1.0F);
   
	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = 0;  
	MATRIX_LOAD4x4 = -divf32(2 * mulf32(far, near), far - near);  
	MATRIX_LOAD4x4 = 0;
	
	glStoreMatrix(0);
}


//---------------------------------------------------------------------------------
//  Frustrum wrapper
//---------------------------------------------------------------------------------
void gluFrustum(float left, float right, float bottom, float top, float near, float far) {
//---------------------------------------------------------------------------------
	gluFrustumf32(floatof32(left), floatof32(right), floatof32(bottom), floatof32(top), floatof32(near), floatof32(far));
}

//---------------------------------------------------------------------------------
//	Fixed point perspective setting
//---------------------------------------------------------------------------------
void gluPerspectivef32(int fovy, f32 aspect, f32 zNear, f32 zFar) {
//---------------------------------------------------------------------------------
	f32 xmin, xmax, ymin, ymax;

	ymax = mulf32(zNear, TAN[fovy & LUT_MASK]);
	ymin = -ymax;
	xmin = mulf32(ymin, aspect);
	xmax = mulf32(ymax, aspect);

	gluFrustumf32(xmin, xmax, ymin, ymax, zNear, zFar);
}

//---------------------------------------------------------------------------------
//  glu wrapper for floating point
//---------------------------------------------------------------------------------
void gluPerspective(float fovy, float aspect, float zNear, float zFar) {
//---------------------------------------------------------------------------------
	gluPerspectivef32((int)(fovy * LUT_SIZE / 360.0), floatof32(aspect), floatof32(zNear), floatof32(zFar));    
}


//---------------------------------------------------------------------------------
void glMaterialf(int mode, rgb color) {
//---------------------------------------------------------------------------------
	static uint32 diffuse_ambient = 0;
	static uint32 specular_emission = 0;

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
void glResetMatrixStack(void) {
//---------------------------------------------------------------------------------
	// stack overflow ack ?
	GFX_STATUS |= 1 << 15;

	// pop the stacks to the top...seems projection stack is only 1 deep??
	glMatrixMode(GL_PROJECTION);
	glPopMatrix((GFX_STATUS>>13) & 1);
  
	// 31 deep modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix((GFX_STATUS >> 8) & 0x1F);
}


//---------------------------------------------------------------------------------
void glSetOutlineColor(int id, rgb color) {
//---------------------------------------------------------------------------------
	GFX_EDGE_TABLE[id] = color;
}


//---------------------------------------------------------------------------------
void glSetToonTable(uint16 *table) {
//---------------------------------------------------------------------------------
	int i;
	for( i = 0; i < 32; i++ )
		GFX_TOON_TABLE[i] = table[i];
}


//---------------------------------------------------------------------------------
void glSetToonTableRange(int start, int end, rgb color) {
//---------------------------------------------------------------------------------
	int i;
	for( i = start; i <= end; i++ )
		GFX_TOON_TABLE[i] = color;
}


//---------------------------------------------------------------------------------
void glReset(void) {
//---------------------------------------------------------------------------------
	while (GFX_STATUS & (1<<27)); // wait till gfx engine is not busy
  
	// Clear the FIFO
	GFX_STATUS |= (1<<29);

	// Clear overflows for list memory
	GFX_CONTROL = enable_bits = ((1<<12) | (1<<13)) | GL_TEXTURE_2D;
	glResetMatrixStack();

	GFX_TEX_FORMAT = 0;
	GFX_POLY_FORMAT = 0;
  
	glMatrixMode(GL_PROJECTION);
	glIdentity();

	glMatrixMode(GL_MODELVIEW);
	glIdentity();
}

//---------------------------------------------------------------------------------
// Texture
//---------------------------------------------------------------------------------
// Texture globals
//---------------------------------------------------------------------------------

uint32 textures[MAX_TEXTURES];

uint32 activeTexture = 0;

uint32* nextBlock = (uint32*)0x06800000;


//---------------------------------------------------------------------------------
void glResetTextures(void) {
//---------------------------------------------------------------------------------
	activeTexture = 0;
	nextBlock = (uint32*)0x06800000;
}

//---------------------------------------------------------------------------------
//	glGenTextures creates integer names for your table
//	takes n as the number of textures to generate and 
//	a pointer to the names array that it needs to fill.
//  Returns 1 if succesful and 0 if out of texture names
//---------------------------------------------------------------------------------
int glGenTextures(int n, int *names) {
//---------------------------------------------------------------------------------
	static int name = 1;

	int index = 0;

	for(index = 0; index < n; index++) {

		if(name >= MAX_TEXTURES)
			return 0;
		else
			names[index] = name++;
	}

	return 1;
}

//---------------------------------------------------------------------------------
// glBindTexure sets the current named
//	texture to the active texture.  Target 
//	is ignored as all DS textures are 2D
//---------------------------------------------------------------------------------
void glBindTexture(int target, int name) {
//---------------------------------------------------------------------------------
	if (name == 0) 
		GFX_TEX_FORMAT = 0; 
	else 
		GFX_TEX_FORMAT = textures[name]; 

	
	activeTexture = name;
}

//---------------------------------------------------------------------------------
void glTexCoord2f32(f32 u, f32 v) { 
//---------------------------------------------------------------------------------
	int x=0,y=0; 
   
	x = ((0x00700000) & textures[activeTexture]) >> 20; 
	y = ((0x03800000) & textures[activeTexture]) >> 23; 

	glTexCoord2t16(f32tot16 (mulf32(u,intof32(1<<(3+x)))), f32tot16 (mulf32(v,intof32(1<<(3+y))))); 
}


//---------------------------------------------------------------------------------
// glTexParameter although named the same 
//	as its gl counterpart it is not compatible
//	Effort may be made in the future to make it so.
//---------------------------------------------------------------------------------
void glTexParameter(	uint8 sizeX, uint8 sizeY,
											uint32* addr,
											uint8 mode,
											uint32 param) {
//---------------------------------------------------------------------------------
	textures[activeTexture] = param | (sizeX << 20) | (sizeY << 23) | (((uint32)addr >> 3) & 0xFFFF) | (mode << 26);
}


//---------------------------------------------------------------------------------
uint16* vramGetBank(uint16 *addr) {
//---------------------------------------------------------------------------------
	if(addr >= VRAM_A && addr < VRAM_B)
		return VRAM_A;
	else if(addr >= VRAM_B && addr < VRAM_C)
		return VRAM_B;
	else if(addr >= VRAM_C && addr < VRAM_D)
		return VRAM_C;
	else if(addr >= VRAM_D && addr < VRAM_E)
		return VRAM_D;
	else if(addr >= VRAM_E && addr < VRAM_F)
		return VRAM_E;
	else if(addr >= VRAM_F && addr < VRAM_G)
		return VRAM_F;
	else if(addr >= VRAM_G && addr < VRAM_H)
		return VRAM_H;
	else if(addr >= VRAM_H && addr < VRAM_I)
		return VRAM_H;
	else return VRAM_I;
}


//---------------------------------------------------------------------------------
int vramIsTextureBank(uint16 *addr) {
//---------------------------------------------------------------------------------
   uint16* vram = vramGetBank(addr);

   if(vram == VRAM_A)
   {
      if((VRAM_A_CR & 3) == ((VRAM_A_TEXTURE) & 3))
         return 1;
      else return 0;
   }
   else if(vram == VRAM_B)
   {
      if((VRAM_B_CR & 3) == ((VRAM_B_TEXTURE) & 3))
         return 1;
      else return 0;
   }
   else if(vram == VRAM_C)
   {
      if((VRAM_C_CR & 3) == ((VRAM_C_TEXTURE) & 3))
         return 1;
      else return 0;
   }
   else if(vram == VRAM_D)
   {
      if((VRAM_D_CR & 3) == ((VRAM_D_TEXTURE) & 3))
         return 1;
      else return 0;
   }
   else
      return 0;
   
} 
//---------------------------------------------------------------------------------
uint32* getNextTextureSlot(int size) {
//---------------------------------------------------------------------------------
   uint32* result = nextBlock;
   nextBlock += size >> 2;

   //uh-oh...out of texture memory in this bank...find next one assigned to textures
   while(!vramIsTextureBank((uint16*)nextBlock - 1) && nextBlock <= (uint32*)VRAM_E)
   {
      nextBlock = (uint32*)vramGetBank((uint16*)result) + (0x20000 >> 2); //next bank
      result = nextBlock;        
      nextBlock += size >> 2;
   }

   if(nextBlock > (uint32*)VRAM_E)
      return 0;

   else return result;   

} 

//---------------------------------------------------------------------------------
// Similer to glTextImage2D from gl it takes a pointer to data
//	Empty fields and target are unused but provided for code compatibility.
//	type is simply the texture type (GL_RGB, GL_RGB8 ect...)
//---------------------------------------------------------------------------------
int glTexImage2D(	int target, int empty1, int type,
									int sizeX, int sizeY,
									int empty2, int param,
									uint8* texture) {
//---------------------------------------------------------------------------------
	uint16 alpha = 0;
	uint32 size = 0;
	uint16 palette = 0;
	uint32* addr;
	uint32 vramTemp;

	size = 1 << (sizeX + sizeY + 6) ;
	
	if(type == GL_RGB)
	{
		alpha = (1 << 15);
		type--;
	}
	
	switch (type)
	{
	case GL_RGBA:
		size = size << 1;
		break;
	case GL_RGB4:
		size = size >> 2;
		palette = 4 * 2;
		break;
	case GL_RGB16:
		size = size >> 1;
		palette = 16 * 2;
		break;
	case GL_RGB256:
		palette = 256 * 2;
		break;
	default:
		break;
	}
	
	addr = getNextTextureSlot(size);
	
	if(!addr)
		return 0;

	glTexParameter(sizeX, sizeY, addr, type, param);
	
	//unlock texture memory
	vramTemp = vramSetMainBanks(VRAM_A_LCD,VRAM_B_LCD,VRAM_C_LCD,VRAM_D_LCD);

	dmaCopyWords(3, (uint32*)texture, addr , size);
	
	vramRestorMainBanks(vramTemp);

	/*if(palette)
	{
		u32* temp = (u32)(texture+size);

		vramSetBankE(VRAM_E_LCD);

		
		swiCpuCopy( texture + size, VRAM_E, COPY_MODE_WORD | palette);

		vramSetBankE(VRAM_E_TEX_PALETTE);
	}
*/
	return 1;
}


//---------------------------------------------------------------------------------
void glTexLoadPal(u16* pal, u8 count) {
//---------------------------------------------------------------------------------
	vramSetBankE(VRAM_E_LCD);
		
	dmaCopyWords(3, (u32*)pal, (u32*)VRAM_E , count);

	vramSetBankE(VRAM_E_TEX_PALETTE);

}

