/*---------------------------------------------------------------------------------
	$Id: videoGL.h,v 1.13 2005-11-03 23:34:14 wntrmute Exp $

	videoGL.h -- Video API vaguely similar to OpenGL

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
	Revision 1.12  2005/10/13 16:32:09  dovoto
	Altered glTexLoadPal to accept a texture slot to allow multiple texture palettes.
	
	Revision 1.11  2005/09/19 20:59:47  dovoto
	Added glOrtho and glOrthof32.  No change to interrupts.h
	
	Revision 1.10  2005/08/23 17:06:10  wntrmute
	converted all endings to unix
	
	Revision 1.9  2005/08/22 08:05:53  wntrmute
	moved inlines to separate file
	
	Revision 1.8  2005/07/27 15:54:57  wntrmute
	Synchronise with ndslib.
	
	Added f32 version of glTextCoord
	fixed glBindTexture (0,0) now clears fomatting
	
	Revision 1.7  2005/07/27 02:20:05  wntrmute
	resynchronise with ndslib
	Updated GL with float wrappers for NeHe
	
	Revision 1.6  2005/07/14 08:00:57  wntrmute
	resynchronise with ndslib

---------------------------------------------------------------------------------*/

#ifndef VIDEOGL_ARM9_INCLUDE
#define VIDEOGL_ARM9_INCLUDE

//---------------------------------------------------------------------------------

#ifndef ARM9
#error 3D hardware is only available from the ARM9
#endif

//---------------------------------------------------------------------------------
#include <nds/jtypes.h>
#include <nds/arm9/video.h>
#include <nds/dma.h>

/*---------------------------------------------------------------------------------
	lut resolution for trig functions
	(must be power of two and must be the same as LUT resolution)
	in other words dont change unless you also change your LUTs
---------------------------------------------------------------------------------*/
#define LUT_SIZE (512)
#define LUT_MASK (0x1FF)

#define GLuint u32
#define GLfloat float


#define intof32(n)           ((n) << 12)
#define f32toint(n)          ((n) >> 12)
#define floatof32(n)         ((f32)((n) * (1 << 12)))
#define f32tofloat(n)        (((float)(n)) / (float)(1<<12))

typedef short int t16;       // text coordinate 1.11.4 fixed point

#define f32tot16(n)             ((t16)(n >> 8))
#define intot16(n)           ((n) << 4)
#define t16toint(n)          ((n) >> 4)
#define floatot16(n)         ((t16)((n) * (1 << 4)))
#define TEXTURE_PACK(u,v)    (((u) << 16) | (v & 0xFFFF))

typedef short int v16;       // vertex 1.3.12 fixed format
#define intov16(n)           ((n) << 12)
#define f32tov16(n)             (n)
#define v16toint(n)          ((n) >> 12)
#define floatov16(n)         ((v16)((n) * (1 << 12)))
#define VERTEX_PACK(x,y)		(((y) << 16) | ((x) & 0xFFFF))


typedef short int v10;       // vertex 1.0.9 fixed point
#define intov10(n)           ((n) << 9)
#define f32tov10(n)          ((v10)(n >> 3))
#define v10toint(n)          ((n) >> 9)
#define floatov10(n)         ((v10)((n) * (1 << 9)))
#define NORMAL_PACK(x,y,z)   (((x) & 0x3FF) | (((y) & 0x3FF) << 10) | ((z) << 20))


typedef unsigned short rgb;

typedef struct {
  f32 m[9];
} m3x3;

typedef struct {
  f32 m[16];
} m4x4;

typedef struct {
  f32 m[12];
} m4x3;

typedef struct {
  f32 x,y,z;
} GLvector;



#define GL_FALSE				0
#define GL_TRUE					1

#define GL_TRIANGLE        0
#define GL_QUAD            1
#define GL_TRIANGLES        0
#define GL_QUADS            1
#define GL_TRIANGLE_STRIP  2
#define GL_QUAD_STRIP      3

#define GL_MODELVIEW       2
#define GL_PROJECTION      0
#define GL_TEXTURE         3

#define GL_AMBIENT              1
#define GL_DIFFUSE              2
#define GL_AMBIENT_AND_DIFFUSE  3
#define GL_SPECULAR             4
#define GL_SHININESS            8
#define GL_EMISSION             0x10

#define GL_LIGHTING				1

#define POLY_ALPHA(n)  ((n) << 16)
#define POLY_TOON_SHADING     0x20
#define POLY_CULL_BACK        0x80
#define POLY_CULL_FRONT       0x40
#define POLY_CULL_NONE        0xC0
#define POLY_ID(n)		((n)<<24)


#define POLY_FORMAT_LIGHT0      0x1
#define POLY_FORMAT_LIGHT1      0x2
#define POLY_FORMAT_LIGHT2      0x4
#define POLY_FORMAT_LIGHT3      0x8

#define MAX_TEXTURES 2048  //this should be enough ! but feel free to change

#define TEXTURE_SIZE_8     0
#define TEXTURE_SIZE_16    1 
#define TEXTURE_SIZE_32    2
#define TEXTURE_SIZE_64    3
#define TEXTURE_SIZE_128   4
#define TEXTURE_SIZE_256   5
#define TEXTURE_SIZE_512   6
#define TEXTURE_SIZE_1024  7 


#define TEXGEN_OFF			(0<<30)			//unmodified texcoord
#define TEXGEN_TEXCOORD		(1<<30)			//texcoord * texture-matrix
#define TEXGEN_NORMAL		(2<<30)			//normal * texture-matrix
#define TEXGEN_POSITION		(3<<30)			//vertex * texture-matrix

#define GL_TEXTURE_WRAP_S (1 << 16)
#define GL_TEXTURE_WRAP_T (1 << 17)
#define GL_TEXTURE_FLIP_S (1 << 18)
#define GL_TEXTURE_FLIP_T (1 << 19)

#define GL_TEXTURE_2D		1

#define GL_TOON_HIGHLIGHT	(1<<1)
#define GL_ANTIALIAS		(1<<4)			//not fully figured out
#define GL_OUTLINE			(1<<5)
#define GL_BLEND			(1<<3)
#define GL_ALPHA_TEST		(1<<2)
#define GL_TEXTURE_ALPHA_MASK (1 << 29)

#define GL_RGB		8
#define GL_RGBA		7	//15 bit color + alpha bit
#define GL_RGB4		2	//4 color palette
#define GL_RGB256	4	//256 color palette
#define GL_RGB16	3	//16 color palette
#define GL_COMPRESSED	5 //compressed texture

//---------------------------------------------------------------------------------
//Fifo commands
//---------------------------------------------------------------------------------
#define FIFO_COMMAND_PACK(c1,c2,c3,c4) (((c4) << 24) | ((c3) << 16) | ((c2) << 8) | (c1))

#define REG2ID(r)						(u8)( ( ((u32)(&(r)))-0x04000400 ) >> 2 )

#define FIFO_NOP				REG2ID(GFX_FIFO)  
#define FIFO_STATUS				REG2ID(GFX_STATUS)            
#define FIFO_COLOR				REG2ID(GFX_COLOR)            

#define FIFO_VERTEX16			REG2ID(GFX_VERTEX16)          
#define FIFO_TEX_COORD			REG2ID(GFX_TEX_COORD)         
#define FIFO_TEX_FORMAT			REG2ID(GFX_TEX_FORMAT)        

#define FIFO_CLEAR_COLOR		REG2ID(GFX_CLEAR_COLOR)       
#define FIFO_CLEAR_DEPTH		REG2ID(GFX_CLEAR_DEPTH)       

#define FIFO_LIGHT_VECTOR		REG2ID(GFX_LIGHT_VECTOR)      
#define FIFO_LIGHT_COLOR		REG2ID(GFX_LIGHT_COLOR)       
#define FIFO_NORMAL				REG2ID(GFX_NORMAL)            

#define FIFO_DIFFUSE_AMBIENT	REG2ID(GFX_DIFFUSE_AMBIENT)   
#define FIFO_SPECULAR_EMISSION	REG2ID(GFX_SPECULAR_EMISSION) 
#define FIFO_SHININESS			REG2ID(GFX_SHININESS)        

#define FIFO_POLY_FORMAT		REG2ID(GFX_POLY_FORMAT)       

#define FIFO_BEGIN				REG2ID(GFX_BEGIN)             
#define FIFO_END				REG2ID(GFX_END)               
#define FIFO_FLUSH				REG2ID(GFX_FLUSH)             
#define FIFO_VIEWPORT			REG2ID(GFX_VIEWPORT)          

#ifdef __cplusplus
extern "C" {
#endif



void glEnable(int bits);
void glDisable(int bits);
void glLoadMatrix4x4(m4x4 * m);
void glLoadMatrix4x3(m4x3 * m);
void glMultMatrix4x4(m4x4 * m);
void glMultMatrix4x3(m4x3 * m);
void glMultMatrix3x3(m3x3 * m);
void glRotateXi(int angle);
void glRotateYi(int angle);
void glRotateZi(int angle);
void glRotateX(float angle);
void glRotateY(float angle);
void glRotateZ(float angle);
void glRotatef32i(int angle, f32 x, f32 y, f32 z);

void glOrthof32(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);
void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar);

void gluLookAtf32(f32 eyex, f32 eyey, f32 eyez, f32 lookAtx, f32 lookAty, f32 lookAtz, f32 upx, f32 upy, f32 upz);
void gluLookAt(float eyex, float eyey, float eyez, float lookAtx, float lookAty, float lookAtz, float upx, float upy, float upz);
void gluFrustumf32(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
void gluFrustum(float left, float right, float bottom, float top, float near, float far);
void gluPerspectivef32(int fovy, f32 aspect, f32 zNear, f32 zFar);
void gluPerspective(float fovy, float aspect, float zNear, float zFar);

int glTexImage2D(int target, int empty1, int type, int sizeX, int sizeY, int empty2, int param, uint8* texture);
void glTexLoadPal(u16* pal, u8 count, u8 slot);
void glBindTexture(int target, int name);
int glGenTextures(int n, int *names);
void glResetTextures(void);
void glTexCoord2f32(f32 u, f32 v);

void glMaterialf(int mode, rgb color);
void glResetMatrixStack(void);
void glSetOutlineColor(int id, rgb color);
void glSetToonTable(uint16 *table);
void glSetToonTableRange(int start, int end, rgb color);
void glReset(void);

//---------------------------------------------------------------------------------
//float wrappers for porting
//---------------------------------------------------------------------------------
void glRotatef(float angle, float x, float y, float z);
void glVertex3f(float x, float y, float z);
void glTexCoord2f(float s, float t);
void glColor3f(float r, float g, float b);
void glScalef(float x, float y, float z);
void glTranslatef(float x, float y, float z);
void glNormal3f(float x, float y, float z);


#ifdef NO_GL_INLINE

	void glBegin(int mode);
	void glEnd( void);
	void glClearColor(uint8 red, uint8 green, uint8 blue);
	void glClearDepth(uint16 depth);
	void glColor3b(uint8 red, uint8 green, uint8 blue);
	void glColor(rgb color);
	void glVertex3v16(v16 x, v16 y, v16 z);
	void glVertex2v16(int yx, v16 z);
	void glTexCoord2t16(t16 u, t16 v);
	void glTexCoord1i(uint32 uv);
	void glPushMatrix(void);
	void glPopMatrix(int32 index);
	void glRestoreMatrix(int32 index);
	void glStoreMatrix(int32 index);
	void glScalev(GLvector* v);
	void glTranslatev(GLvector* v);
	void glTranslate3f32(f32 x, f32 y, f32 z);
	void glScalef32(f32 factor);
	void glTranslatef32(f32 delta);
	void glLight(int id, rgb color, v10 x, v10 y, v10 z);
	void glNormal(uint32 normal);
	void glIdentity(void);
	void glMatrixMode(int mode);
	void glViewPort(uint8 x1, uint8 y1, uint8 x2, uint8 y2);
	void glFlush(void);
	void glMaterialShinnyness(void);
	void glPolyFmt(int alpha); 

#else

	#include "videoGL.inl"
	
#endif  //endif #no inline
#ifdef __cplusplus
}
#endif

#endif

//////////////////////////////////////////////////////////////////////
