/*---------------------------------------------------------------------------------
	$Id: videoGL.c,v 1.28 2007-01-31 22:57:28 gabebear Exp $

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
	Revision 1.27  2007/01/30 00:15:48  gabebear
	 - got rid of extra flags in glEnable and glDisable
	 - added glInit() which does pretty much the same thing that glReset did. It just initializes the GL-state at the start of the program and then never needs called again. Initializing the state explicitly should make code more stable when using different boot methods that may have fiddled with default states.
	 - depreciated glReset, because glInit does the same job better, setting up everythign per frame was a waste
	 - glInit sets up the read-plane(a.ka. clear-color) as blank instead of a bmp, and sets it's so that it is totally opaque with a poly-ID of zero. This lets antialiasing, and outlining work with simple glEnables!!!
	 - Changed glClearColor so that it sets the rear-plane ID instead of palette[0]
	 - added glClearAlpha() that sets the alpha of the rear-plane
	 - added glClearPolyID() that sets how things get outlined and antialiased
	
	I haven't tested this against the examples, another patch is on the way to fix the examples
	
	Revision 1.26  2007/01/20 00:30:48  dovoto
	Updated the glTexCoord2f() to pull texture size from state...texture sizes other than 128x128 should now work with the floating point version.
	
	Revision 1.25  2007/01/14 11:31:22  wntrmute
	bogus fixed types removed from libnds
	
	Revision 1.24  2007/01/11 05:35:41  dovoto
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
	
	Revision 1.23  2006/08/03 04:59:08  dovoto
	Added gluPickMatrix... (untested)
	
	Revision 1.22  2006/05/08 03:23:32  dovoto
	*** empty log message ***
	
	Revision 1.20  2006/05/08 03:19:51  dovoto
	Added glGetTexturePointer which allows the user to retreive a pointer to texture memory for the named texture.
	
	Small fix to the texture reset code.  The texture name is now reset as well.
	
	Revision 1.19  2006/01/05 08:13:26  dovoto
	Fixed gluLookAt (again)
	Major update to palette handling (likely a breaking change if you were using the gl texture palettes from before)
	
	Revision 1.18  2005/11/27 04:23:19  joatski
	Renamed glAlpha to glAlphaFunc (old name is present but deprecated)
	Added new texture formats
	Added glCutoffDepth
	Changed type input of glClearDepth to fixed12d3, added conversion functions
	
	Revision 1.17  2005/11/26 20:33:00  joatski
	Changed spelling of fixed-point macros.  Old ones are present but deprecated.
	Fixed difference between GL_RGB and GL_RGBA
	Added GL_GET_WIDTH and GL_GET_HEIGHT
	
	Revision 1.15  2005/11/07 04:16:24  dovoto
	Added glGetInt and glGetFixed, Fixed glOrtho, Began Doxygenation
	
	Revision 1.14  2005/10/13 16:32:09  dovoto
	Altered glTexLoadPal to accept a texture slot to allow multiple texture palettes.
	
	Revision 1.13  2005/09/19 20:59:47  dovoto
	Added glOrtho and glOrthof32.  No change to interrupts.h
	
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
#include <nds/bios.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>


// holds the current state of the graphics control register, initialized in glInit()
static uint16 gfx_control_bits = 0;

// holds the current state of the clear color register, initialized in glInit()
static uint32 clear_bits = 0;

//---------------------------------------------------------------------------------
void glEnable(int bits) {
//---------------------------------------------------------------------------------
	gfx_control_bits |= bits;
	GFX_CONTROL = gfx_control_bits;
}


//---------------------------------------------------------------------------------
void glDisable(int bits) {
//---------------------------------------------------------------------------------
	gfx_control_bits &= ~bits;
	GFX_CONTROL = gfx_control_bits;
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
	int32 sine = SIN[angle &  LUT_MASK];
	int32 cosine = COS[angle & LUT_MASK];

	MATRIX_MULT3x3 = cosine;
	MATRIX_MULT3x3 = sine;
	MATRIX_MULT3x3 = 0;

	MATRIX_MULT3x3 = - sine;
	MATRIX_MULT3x3 = cosine;
	MATRIX_MULT3x3 = 0;
  
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = inttof32(1);
}

//---------------------------------------------------------------------------------
void glRotateYi(int angle) {
//---------------------------------------------------------------------------------
	int32 sine = SIN[angle &  LUT_MASK];
	int32 cosine = COS[angle & LUT_MASK];

	MATRIX_MULT3x3 = cosine;
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = -sine;
  
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = inttof32(1);
	MATRIX_MULT3x3 = 0;
  
	MATRIX_MULT3x3 = sine;
	MATRIX_MULT3x3 = 0;
	MATRIX_MULT3x3 = cosine;
}

//---------------------------------------------------------------------------------
void glRotateXi(int angle) {
//---------------------------------------------------------------------------------
	int32 sine = SIN[angle &  LUT_MASK];
	int32 cosine = COS[angle & LUT_MASK];

	MATRIX_MULT3x3 = inttof32(1);
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
void glRotatef32i(int angle, int32 x, int32 y, int32 z) {
//---------------------------------------------------------------------------------

	int32 axis[3];
	int32 sine = SIN[angle &  LUT_MASK];
	int32 cosine = COS[angle & LUT_MASK];
	int32 one_minus_cosine = inttof32(1) - cosine;

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
void glRotatef32(float angle, int32 x, int32 y, int32 z) {
//---------------------------------------------------------------------------------
    glRotatef32i((int)(angle * LUT_SIZE / 360.0), x, y, z);
}

//---------------------------------------------------------------------------------
void glRotatef(float angle, float x, float y, float z) {
//---------------------------------------------------------------------------------
	glRotatef32(angle, floattof32(x), floattof32(y), floattof32(z));
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
	glVertex3v16(floattov16(x), floattov16(y), floattov16(z));
}



//---------------------------------------------------------------------------------
void glColor3f(float r, float g, float b) {
//---------------------------------------------------------------------------------
	glColor3b((uint8)(r*255), (uint8)(g*255), (uint8)(b*255));
}

//---------------------------------------------------------------------------------
void glScalef(float x, float y, float z) {
//---------------------------------------------------------------------------------
  MATRIX_SCALE = floattof32(x);
  MATRIX_SCALE = floattof32(y);
  MATRIX_SCALE = floattof32(z);
}

//---------------------------------------------------------------------------------
void glTranslatef(float x, float y, float z) {
//---------------------------------------------------------------------------------
  MATRIX_TRANSLATE = floattof32(x);
  MATRIX_TRANSLATE = floattof32(y);
  MATRIX_TRANSLATE = floattof32(z);
}

//---------------------------------------------------------------------------------
void glNormal3f(float x, float y, float z) {
//---------------------------------------------------------------------------------
	if(x >= 1 || x <= -1) x *= .95;
	if(y >= 1 || y <= -1) y *= .95;
	if(z >= 1 || z <= -1) z *= .95;

	glNormal(NORMAL_PACK(floattov10(x), floattov10(y), floattov10(z)));
}



//---------------------------------------------------------------------------------
void glOrthof32(int32 left, int32 right, int32 bottom, int32 top, int32 zNear, int32 zFar) {
//---------------------------------------------------------------------------------
	MATRIX_MULT4x4 = divf32(inttof32(2), right - left);     
	MATRIX_MULT4x4 = 0;  
	MATRIX_MULT4x4 = 0;      
	MATRIX_MULT4x4 = 0;//

	MATRIX_MULT4x4 = 0;  
	MATRIX_MULT4x4 = divf32(inttof32(2), top - bottom);     
	MATRIX_MULT4x4 = 0;    
	MATRIX_MULT4x4 = 0;//
   
	MATRIX_MULT4x4 = 0;  
	MATRIX_MULT4x4 = 0;  
	MATRIX_MULT4x4 = divf32(inttof32(-2), zFar - zNear);     
	MATRIX_MULT4x4 = 0;
   
	MATRIX_MULT4x4 = -divf32(right + left, right - left);//0;  
	MATRIX_MULT4x4 = -divf32(top + bottom, top - bottom); //0;  
	MATRIX_MULT4x4 = -divf32(zFar + zNear, zFar - zNear);//0;  
	MATRIX_MULT4x4 = floattof32(1.0F);
	
	glStoreMatrix(0);
}

//---------------------------------------------------------------------------------
void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar) {
//---------------------------------------------------------------------------------
	glOrthof32(floattof32(left), floattof32(right), floattof32(bottom), floattof32(top), floattof32(zNear), floattof32(zFar));
}

//---------------------------------------------------------------------------------
// Fixed point look at function, it appears to work as expected although 
//	testing is recomended
//---------------------------------------------------------------------------------
void gluLookAtf32(int32 eyex, int32 eyey, int32 eyez, int32 lookAtx, int32 lookAty, int32 lookAtz, int32 upx, int32 upy, int32 upz)  { 
//---------------------------------------------------------------------------------
	int32 side[3], forward[3], up[3], eye[3];

	forward[0] = eyex - lookAtx; 
	forward[1] = eyey - lookAty; 
	forward[2] = eyez - lookAtz; 

	normalizef32(forward); 

	up[0] = upx; 
	up[1] = upy; 
	up[2] = upz; 
	eye[0] = eyex; 
	eye[1] = eyey; 
	eye[2] = eyez; 

	crossf32(up, forward, side); 

	normalizef32(side); 

	// Recompute local up
	crossf32(forward, side, up);

	glMatrixMode(GL_MODELVIEW); 

 
	// should we use MATRIX_MULT4x3? 
	MATRIX_LOAD4x3 = side[0]; 
	MATRIX_LOAD4x3 = up[0]; 
	MATRIX_LOAD4x3 = forward[0]; 

	MATRIX_LOAD4x3 = side[1]; 
	MATRIX_LOAD4x3 = up[1]; 
	MATRIX_LOAD4x3 = forward[1]; 

	MATRIX_LOAD4x3 = side[2]; 
	MATRIX_LOAD4x3 = up[2]; 
	MATRIX_LOAD4x3 = forward[2]; 

	MATRIX_LOAD4x3 = -dotf32(eye,side); 
	MATRIX_LOAD4x3 = -dotf32(eye,up); 
	MATRIX_LOAD4x3 = -dotf32(eye,forward); 

}

 


//---------------------------------------------------------------------------------
//  glu wrapper for standard float call
//---------------------------------------------------------------------------------
void gluLookAt(	float eyex, float eyey, float eyez,
				float lookAtx, float lookAty, float lookAtz,
				float upx, float upy, float upz) {
//---------------------------------------------------------------------------------
	gluLookAtf32(floattof32(eyex), floattof32(eyey), floattof32(eyez),
               floattof32(lookAtx), floattof32(lookAty), floattof32(lookAtz),
					     floattof32(upx), floattof32(upy), floattof32(upz));
}

//---------------------------------------------------------------------------------
//	frustrum has only been tested as part of perspective
//---------------------------------------------------------------------------------
void glFrustumf32(int32 left, int32 right, int32 bottom, int32 top, int32 near, int32 far) {
//---------------------------------------------------------------------------------
	MATRIX_MULT4x4 = divf32(2*near, right - left);
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = divf32(right + left, right - left);
	MATRIX_MULT4x4 = 0;
	
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = divf32(2*near, top - bottom);
	MATRIX_MULT4x4 = divf32(top + bottom, top - bottom);
	MATRIX_MULT4x4 = 0;
	
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = -divf32(far + near, far - near);
	MATRIX_MULT4x4 = floattof32(-1.0F);
	MATRIX_MULT4x4 = 0;
	
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = -divf32(2 * mulf32(far, near), far - near);
	MATRIX_MULT4x4 = 0;
	
	glStoreMatrix(0);
}


//---------------------------------------------------------------------------------
//  Frustrum wrapper
//---------------------------------------------------------------------------------
void glFrustum(float left, float right, float bottom, float top, float near, float far) {
//---------------------------------------------------------------------------------
	glFrustumf32(floattof32(left), floattof32(right), floattof32(bottom), floattof32(top), floattof32(near), floattof32(far));
}

//---------------------------------------------------------------------------------
//	Fixed point perspective setting
//---------------------------------------------------------------------------------
void gluPerspectivef32(int fovy, int32 aspect, int32 zNear, int32 zFar) {
//---------------------------------------------------------------------------------
	int32 xmin, xmax, ymin, ymax;

	ymax = mulf32(zNear, TAN[fovy & LUT_MASK]);
	ymin = -ymax;
	xmin = mulf32(ymin, aspect);
	xmax = mulf32(ymax, aspect);

	glFrustumf32(xmin, xmax, ymin, ymax, zNear, zFar);
}

//---------------------------------------------------------------------------------
//  glu wrapper for floating point
//---------------------------------------------------------------------------------
void gluPerspective(float fovy, float aspect, float zNear, float zFar) {
//---------------------------------------------------------------------------------
	gluPerspectivef32((int)(fovy * LUT_SIZE / 360.0), floattof32(aspect), floattof32(zNear), floattof32(zFar));    
}

//---------------------------------------------------------------------------------
// Sets the pick matrix for 3D selection
//---------------------------------------------------------------------------------
void gluPickMatrix(int x, int y, int width, int height, int viewport[4]) {
	//---------------------------------------------------------------------------------
	MATRIX_MULT4x4 = inttof32(viewport[2]) / width;
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = inttof32(viewport[3]) / height;
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = inttof32(1);
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = inttof32(viewport[2] + ((viewport[0] - x)<<1)) / width;
	MATRIX_MULT4x4 = inttof32(viewport[3] + ((viewport[1] - y)<<1)) / height;
	MATRIX_MULT4x4 = 0;
	MATRIX_MULT4x4 = inttof32(1);
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
void glInit(void) {
//---------------------------------------------------------------------------------
	while (GFX_STATUS & (1<<27)); // wait till gfx engine is not busy
  
	// Clear the FIFO
	GFX_STATUS |= (1<<29);

	// Clear overflows for list memory
	glResetMatrixStack();

	// reset the control bits
	GFX_CONTROL = gfx_control_bits = 0;

	//reset the rear-plane(a.k.a. clear color) to black, ID=0, and opaque
	GFX_CLEAR_COLOR = clear_bits = 0x001F0000;

	//reset where textures are stored
	glResetTextures();

	GFX_TEX_FORMAT = 0;
	GFX_POLY_FORMAT = 0;
  
	glMatrixMode(GL_PROJECTION);
	glIdentity();

	glMatrixMode(GL_MODELVIEW);
	glIdentity();

	glMatrixMode(GL_TEXTURE);
	glIdentity();
}

//---------------------------------------------------------------------------------
void glReset(void) {
//---------------------------------------------------------------------------------
	glInit();
}

//---------------------------------------------------------------------------------
// Texture
//---------------------------------------------------------------------------------
// Texture globals
//---------------------------------------------------------------------------------

uint32 textures[MAX_TEXTURES];

uint32 activeTexture = 0;

uint32* nextBlock = (uint32*)0x06800000;
uint32  nextPBlock = 0;
int nameCount = 1;
//---------------------------------------------------------------------------------
void glResetTextures(void) {
//---------------------------------------------------------------------------------
	activeTexture = 0;
	nextBlock = (uint32*)0x06800000;
	nextPBlock = 0;
	nameCount = 1;
}

//---------------------------------------------------------------------------------
//	glGenTextures creates integer names for your table
//	takes n as the number of textures to generate and 
//	a pointer to the names array that it needs to fill.
//  Returns 1 if succesful and 0 if out of texture names
//---------------------------------------------------------------------------------

int glGenTextures(int n, int *names) {
//---------------------------------------------------------------------------------
	

	int index = 0;

	for(index = 0; index < n; index++) {

		if(nameCount >= MAX_TEXTURES)
			return 0;
		else
			names[index] = nameCount++;
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
// glColorTable establishes the location of the current palette.
//	Roughly follows glColorTableEXT. Association of palettes with 
//	named textures is left to the application. 
//---------------------------------------------------------------------------------
void glColorTable( uint8 format, uint32 addr ) {
//---------------------------------------------------------------------------------
	GFX_PAL_FORMAT = addr>>(4-(format==GL_RGB4));
}
                     
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
void glTexCoord2f32(int32 u, int32 v) { 
//---------------------------------------------------------------------------------
  int x, y; 
   
  x = ((textures[activeTexture]) >> 20) & 7; 
  y = ((textures[activeTexture]) >> 23) & 7; 

  glTexCoord2t16(f32tot16 (mulf32(u,inttof32(8<<x))), f32tot16 (mulf32(v,inttof32(8<<y)))); 
}

//---------------------------------------------------------------------------------
void glTexCoord2f(float s, float t) {
//---------------------------------------------------------------------------------
	int x = ((textures[activeTexture]) >> 20) & 7; 
    int y = ((textures[activeTexture]) >> 23) & 7; 
    
    glTexCoord2t16(floattot16(s*(8 << x)), floattot16(t*(8<<y)));
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
//glGetTexturePointer gets a pointer to vram which contains the texture
//
//---------------------------------------------------------------------------------
void* glGetTexturePointer(	int name) {
//---------------------------------------------------------------------------------
	return (void*) ((textures[name] & 0xFFFF) << 3);
}

//---------------------------------------------------------------------------------
u32 glGetTexParameter(){
//---------------------------------------------------------------------------------
  return textures[activeTexture];
}


//---------------------------------------------------------------------------------
inline uint32 alignVal( uint32 val, uint32 to )
{
  return (val & (to-1))? (val & ~(to-1)) + to : val;
}
//---------------------------------------------------------------------------------
int getNextPaletteSlot(u16 count, uint8 format){
//---------------------------------------------------------------------------------
  // ensure the result aligns on a palette block for this format
  uint32 result = alignVal(nextPBlock, 1<<(4-(format==GL_RGB4)));
  
  // convert count to bytes and align to next (smallest format) palette block
  count = alignVal( count<<1, 1<<3 ); 

  // ensure that end is within palette video mem
  if( result+count > 0x10000 )   // VRAM_F - VRAM_E
    return -1;

  nextPBlock = result+count;
  return (int)result;
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
//  Empty fields and target are unused but provided for code compatibility.
//  type is simply the texture type (GL_RGB, GL_RGB8 ect...)
//---------------------------------------------------------------------------------
int glTexImage2D(int target, int empty1, int type,
                 int sizeX, int sizeY,
                 int empty2, int param,
                 uint8* texture) {
//---------------------------------------------------------------------------------
  uint32 size = 0;
  uint32* addr;
  uint32 vramTemp;

  size = 1 << (sizeX + sizeY + 6);

  switch (type) {
    case GL_RGB:
    case GL_RGBA:
      size = size << 1;
      break;
    case GL_RGB4:
      size = size >> 2;
      break;
    case GL_RGB16:
      size = size >> 1;
      break;

    default:
      break;
  }
  
  addr = getNextTextureSlot(size);
  
  if(!addr)
    return 0;

  // unlock texture memory
  vramTemp = vramSetMainBanks(VRAM_A_LCD,VRAM_B_LCD,VRAM_C_LCD,VRAM_D_LCD);

  if (type == GL_RGB) {
    // We do GL_RGB as GL_RGBA, but we set each alpha bit to 1 during the copy
    u16 * src = (u16*)texture;
    u16 * dest = (u16*)addr;
    
    glTexParameter(sizeX, sizeY, addr, GL_RGBA, param);
    
    while (size--) {
      *dest++ = *src | (1 << 15);
      src++;
    }
  } else {
    // For everything else, we do a straight copy
    glTexParameter(sizeX, sizeY, addr, type, param);
    swiCopy((uint32*)texture, addr , size / 4 | COPY_MODE_WORD);
  }

  vramRestoreMainBanks(vramTemp);

  return 1;
}

 //---------------------------------------------------------------------------------
void glTexLoadPal(u16* pal, u16 count, u32 addr) {
 //---------------------------------------------------------------------------------
 	vramSetBankE(VRAM_E_LCD);
 		
	swiCopy( pal, &VRAM_E[addr>>1] , count / 2 | COPY_MODE_WORD);
 
 	vramSetBankE(VRAM_E_TEX_PALETTE);
}
 
//---------------------------------------------------------------------------------
int gluTexLoadPal(u16* pal, u16 count, uint8 format) {
//---------------------------------------------------------------------------------
  int addr = getNextPaletteSlot(count, format);
  
  if( addr>=0 )
    glTexLoadPal(pal, count, (u32) addr);
  
  return addr;
 }

//---------------------------------------------------------------------------------
void glGetInt(GL_GET_TYPE param, int* i) {
//---------------------------------------------------------------------------------
  switch (param) {
    case GL_GET_POLYGON_RAM_COUNT:
      *i = GFX_POLYGON_RAM_USAGE;
      break;
    case GL_GET_VERTEX_RAM_COUNT:
      *i = GFX_VERTEX_RAM_USAGE;
      break;
    case GL_GET_TEXTURE_WIDTH:
      *i = 8 << (((textures[activeTexture]) >> 20) & 7);
      break;
    case GL_GET_TEXTURE_HEIGHT:
      *i = 8 << (((textures[activeTexture]) >> 23) & 7);
      break;
    default:
      break;
  }
}

//---------------------------------------------------------------------------------
void glGetFixed(GL_GET_TYPE param, int32* f) {
	//---------------------------------------------------------------------------------
	int i;
	switch (param) {
		case GL_GET_MATRIX_ROTATION:
			while(GFX_BUSY); // wait until the graphics engine has stopped to read matrixes
			for(i = 0; i < 9; i++) f[i] = MATRIX_READ_ROTATION[i];
			break;
		case GL_GET_MATRIX_MODELVIEW:
			while(GFX_BUSY); // wait until the graphics engine has stopped to read matrixes
			for(i = 0; i < 16; i++) f[i] = MATRIX_READ_MODELVIEW[i];
			break;
		case GL_GET_MATRIX_PROJECTION:
			glMatrixMode(GL_POSITION);
			glPushMatrix(); // save the current state of the position matrix
			glLoadIdentity(); // load an identity matrix into the position matrix so that the modelview matrix = projection matrix
			while(GFX_BUSY); // wait until the graphics engine has stopped to read matrixes
			for(i = 0; i < 16; i++) f[i] = MATRIX_READ_MODELVIEW[i]; // read out the projection matrix
			glPopMatrix(1); // restore the position matrix
			break;
		case GL_GET_MATRIX_POSITION:
			glMatrixMode(GL_PROJECTION);
			glPushMatrix(); // save the current state of the projection matrix
			glLoadIdentity(); // load a identity matrix into the projection matrix so that the modelview matrix = position matrix
			while(GFX_BUSY); // wait until the graphics engine has stopped to read matrixes
			for(i = 0; i < 16; i++) f[i] = MATRIX_READ_MODELVIEW[i]; // read out the position matrix
			glPopMatrix(1); // restore the projection matrix
			break;
		default: 
			break;
	}
}

//////////////////////////////////////////////////////////////////////
// Function replacements for the obsolete coversion macros
// remove in a revision or two -- joat
// Needed because the depreciate attribute doesn't work on macros.
//////////////////////////////////////////////////////////////////////

int32 intof32(int n)      { return ((n) << 12); }
int32 floatof32(float n)  { return ((int32)((n) * (1 << 12))); }

t16 intot16(int n)      { return n << 4; }
t16 floatot16(float n)  { return ((t16)((n) * (1 << 4))); }

v16 intov16(int n)     { return ((n) << 12); }
v16 floatov16(float n) { return ((v16)((n) * (1 << 12))); }

v10 intov10(int n)     { return ((n) << 9); }
v10 floatov10(float n) { return ((v10)((n) * (1 << 9))); }

//////////////////////////////////////////////////////////////////////

// deprecated, remove in a couple versions -- joat
void glSetAlpha(int alpha) {
  GFX_ALPHA_TEST = alpha;
}

//////////////////////////////////////////////////////////////////////

void glAlphaFunc(int alphaThreshold) {
  GFX_ALPHA_TEST = alphaThreshold;
}

//////////////////////////////////////////////////////////////////////

void glCutoffDepth(fixed12d3 depth) {
  GFX_CUTOFF_DEPTH = depth;
}

//////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
void glClearColor(uint8 red, uint8 green, uint8 blue) {
//---------------------------------------------------------------------------------
	GFX_CLEAR_COLOR = clear_bits = (clear_bits & 0xFFFF0000) | RGB15(red, green, blue);
}

//---------------------------------------------------------------------------------
void glClearAlpha(uint8 alpha) {
//---------------------------------------------------------------------------------
	GFX_CLEAR_COLOR = clear_bits = (clear_bits & 0x001F0000) | (( alpha & 0x1F ) << 16 );
}

//---------------------------------------------------------------------------------
void glClearPolyID(uint8 ID) {
//---------------------------------------------------------------------------------
	GFX_CLEAR_COLOR = clear_bits = (clear_bits & 0x3F000000) | (( ID & 0x3F ) << 24 );
}



