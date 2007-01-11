/*---------------------------------------------------------------------------------
	$Id: videoGL.h,v 1.24 2007-01-11 05:35:41 dovoto Exp $

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
	Revision 1.23  2006/08/03 04:56:31  dovoto
	Added gluPickMatrix() ...untested
	
	Revision 1.22  2006/05/08 03:19:51  dovoto
	Added glGetTexturePointer which allows the user to retreive a pointer to texture memory for the named texture.
	
	Small fix to the texture reset code.  The texture name is now reset as well.
	
	Revision 1.21  2006/01/05 08:13:26  dovoto
	Fixed gluLookAt (again)
	Major update to palette handling (likely a breaking change if you were using the gl texture palettes from before)
	
	Revision 1.20  2005/11/27 04:23:19  joatski
	Renamed glAlpha to glAlphaFunc (old name is present but deprecated)
	Added new texture formats
	Added glCutoffDepth
	Changed type input of glClearDepth to fixed12d3, added conversion functions
	
	Revision 1.19  2005/11/26 20:33:00  joatski
	Changed spelling of fixed-point macros.  Old ones are present but deprecated.
	Fixed difference between GL_RGB and GL_RGBA
	Added GL_GET_WIDTH and GL_GET_HEIGHT
	
	Revision 1.17  2005/11/18 14:31:58  wntrmute
	corrected parameter for doxygen in glLoadMatrix4x3
	
	Revision 1.16  2005/11/14 12:01:27  wntrmute
	Correct spelling and parameters in doxygen comments
	
	Revision 1.15  2005/11/07 04:44:25  dovoto
	Corrected some spelling
	
	Revision 1.14  2005/11/07 04:16:24  dovoto
	Added glGetInt and glGetFixed, Fixed glOrtho, Began Doxygenation
	
	Revision 1.13  2005/11/03 23:34:14  wntrmute
	killed vector type, replaced with GLvector
	
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
/*! \file videoGL.h 
\brief openGL (ish) interface to DS 3D hardware. 

*/
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

//////////////////////////////////////////////////////////////////////
// Fixed point / floating point / integer conversion macros
//////////////////////////////////////////////////////////////////////

// Used for depth (glClearDepth, glCutoffDepth)
typedef uint16 fixed12d3;

#define int_to_12d3(n)    ((n) << 3)
#define float_to_12d3(n)  ((fixed12d3)((n) * (1 << 3)))
#define GL_MAX_DEPTH      0x7FFF

//////////////////////////////////////////////////////////////////////

#define inttof32(n)          ((n) << 12)
#define f32toint(n)          ((n) >> 12)
#define floattof32(n)        ((f32)((n) * (1 << 12)))
#define f32tofloat(n)        (((float)(n)) / (float)(1<<12))

typedef short int t16;       // text coordinate 12.4 fixed point

#define f32tot16(n)          ((t16)(n >> 8))
#define inttot16(n)          ((n) << 4)
#define t16toint(n)          ((n) >> 4)
#define floattot16(n)        ((t16)((n) * (1 << 4)))
#define TEXTURE_PACK(u,v)    (((u) << 16) | (v & 0xFFFF))

typedef short int v16;       // vertex 4.12 fixed format
#define inttov16(n)          ((n) << 12)
#define f32tov16(n)          (n)
#define v16toint(n)          ((n) >> 12)
#define floattov16(n)         ((v16)((n) * (1 << 12)))
#define VERTEX_PACK(x,y)		 (((y) << 16) | ((x) & 0xFFFF))

typedef short int v10;       // vertex .10 fixed point
#define inttov10(n)          ((n) << 9)
#define f32tov10(n)          ((v10)(n >> 3))
#define v10toint(n)          ((n) >> 9)
#define floattov10(n)        ((v10)((n) * (1 << 9)))
#define NORMAL_PACK(x,y,z)   (((x) & 0x3FF) | (((y) & 0x3FF) << 10) | ((z) << 20))

//////////////////////////////////////////////////////////////////////
// deprecated versions of some macros, remove in a few versions -- joat
//////////////////////////////////////////////////////////////////////

f32 intof32(int n) __attribute__((deprecated));
f32 floatof32(float n) __attribute__((deprecated));

t16 intot16(int n) __attribute__((deprecated));
t16 floatot16(float n) __attribute__((deprecated));

v16 intov16(int n) __attribute__((deprecated));
v16 floatov16(float n) __attribute__((deprecated));

v10 intov10(int n) __attribute__((deprecated));
v10 floatov10(float n) __attribute__((deprecated));

//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////

#define GL_FALSE     0
#define GL_TRUE      1

//////////////////////////////////////////////////////////////////////
// glBegin constants
//////////////////////////////////////////////////////////////////////

#define GL_TRIANGLE        0
#define GL_QUAD            1
#define GL_TRIANGLES       0
#define GL_QUADS           1
#define GL_TRIANGLE_STRIP  2
#define GL_QUAD_STRIP      3

//////////////////////////////////////////////////////////////////////
// glMatrixMode constants
//////////////////////////////////////////////////////////////////////

#define GL_PROJECTION      0
#define GL_POSITION        1
#define GL_MODELVIEW       2
#define GL_TEXTURE         3

//////////////////////////////////////////////////////////////////////
// glMaterialf constants
//////////////////////////////////////////////////////////////////////

#define GL_AMBIENT              0x01
#define GL_DIFFUSE              0x02
#define GL_AMBIENT_AND_DIFFUSE  0x03
#define GL_SPECULAR             0x04
#define GL_SHININESS            0x08
#define GL_EMISSION             0x10

////////////////////////////////////////////////////////////
// glPolyFmt constants
////////////////////////////////////////////////////////////

#define POLY_ALPHA(n)  ((n) << 16)
#define POLY_TOON_SHADING     0x20
#define POLY_CULL_BACK        0x80
#define POLY_CULL_FRONT       0x40
#define POLY_CULL_NONE        0xC0
#define POLY_ID(n)	((n)<<24)

#define POLY_FORMAT_LIGHT0      0x1
#define POLY_FORMAT_LIGHT1      0x2
#define POLY_FORMAT_LIGHT2      0x4
#define POLY_FORMAT_LIGHT3      0x8

////////////////////////////////////////////////////////////
// glTexImage2d constants
////////////////////////////////////////////////////////////

#define GL_TEXTURE_2D   1

// size bits
#define TEXTURE_SIZE_8     0
#define TEXTURE_SIZE_16    1 
#define TEXTURE_SIZE_32    2
#define TEXTURE_SIZE_64    3
#define TEXTURE_SIZE_128   4
#define TEXTURE_SIZE_256   5
#define TEXTURE_SIZE_512   6
#define TEXTURE_SIZE_1024  7 

// parameter bits
#define GL_TEXTURE_WRAP_S (1 << 16)
#define GL_TEXTURE_WRAP_T (1 << 17)
#define GL_TEXTURE_FLIP_S (1 << 18)
#define GL_TEXTURE_FLIP_T (1 << 19)

// these two are the same thing!
#define GL_TEXTURE_COLOR0_TRANSPARENT (1<<29)
#define GL_TEXTURE_ALPHA_MASK (1 << 29)

#define TEXGEN_OFF      (0<<30)  //unmodified texcoord
#define TEXGEN_TEXCOORD (1<<30)  //texcoord * texture-matrix
#define TEXGEN_NORMAL   (2<<30)  //normal * texture-matrix
#define TEXGEN_POSITION (3<<30)  //vertex * texture-matrix

// mode bits
#define GL_RGB32_A3   1 // 32 color palette, 3 bits of alpha
#define GL_RGB4       2 // 4 color palette
#define GL_RGB16      3 // 16 color palette
#define GL_RGB256     4 // 256 color palette
#define GL_COMPRESSED 5 // compressed texture
#define GL_RGB8_A5    6 // 8 color palette, 5 bits of alpha
#define GL_RGBA       7 // 15 bit direct color, 1 bit of alpha
#define GL_RGB        8 // 15 bit direct color, alpha bit autoset to 1

////////////////////////////////////////////////////////////
// glEnable / glDisable constants
////////////////////////////////////////////////////////////

#define GL_TOON_HIGHLIGHT (1<<1)
#define GL_ALPHA_TEST     (1<<2)  // enables/disables fragment testing of post-blend alpha >= gfx_alpha_test
#define GL_BLEND          (1<<3)
#define GL_ANTIALIAS      (1<<4)  //not fully figured out
#define GL_OUTLINE        (1<<5)

////////////////////////////////////////////////////////////
// glGet constants
////////////////////////////////////////////////////////////

typedef enum {
	GL_GET_VERTEX_RAM_COUNT,	// returns a count of vertexes currently stored in hardware vertex ram
	GL_GET_POLYGON_RAM_COUNT,	// returns a count of polygons currently stored in hardware polygon ram
	GL_GET_MATRIX_ROTATION,		// returns the current 3x3 rotation matrix
	GL_GET_MATRIX_POSITION,		// returns the current 4x4 position matrix
	GL_GET_MATRIX_PROJECTION,	// returns the current 4x4 projection matrix
	GL_GET_MATRIX_MODELVIEW,	// returns the current 4x4 modelview matrix
	GL_GET_TEXTURE_WIDTH,		// returns the width of the currently bound texture
	GL_GET_TEXTURE_HEIGHT		// returns the height of the currently bound texture
} GL_GET_TYPE;

////////////////////////////////////////////////////////////
// Misc. constants
////////////////////////////////////////////////////////////

#define MAX_TEXTURES 2048  //this should be enough ! but feel free to change
#define GL_LIGHTING    1   // no idea what this is for / who defined it


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
#define FIFO_PAL_FORMAT			REG2ID(GFX_PAL_FORMAT)

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


/*! \fn void glEnable(int bits)
\brief Enables various gl states (blend, alpha test, etc..)
\param bits A bit mask of desired attributes
*/
void glEnable(int bits);
/*! \fn void glDisable(int bits)
\brief Disables various gl states (blend, alpha test, etc..)
\param bits A bit mask of desired attributes
*/

void glDisable(int bits);
/*! \fn void glLoadMatrix4x4(m4x4 * m)
\brief Loads a 4x4 matrix into the current matrix
\param m a pointer to a 4x4 matrix
*/

void glLoadMatrix4x4(m4x4 * m);
/*! \fn void glLoadMatrix4x3(m4x3 * m)
\brief Loads a 4x3 matrix into the current matrix
\param m a pointer to a 4x4 matrix
*/

void glLoadMatrix4x3(m4x3 * m);
/*! \fn void glMultMatrix4x4(m4x4 * m)
\brief Multiplies the current matrix by m
\param m a pointer to a 4x4 matrix
*/

void glMultMatrix4x4(m4x4 * m);
/*! \fn void glMultMatrix4x3(m4x3 * m)
\brief multiplies the current matrix by
\param m a pointer to a 4x3 matrix
*/

void glMultMatrix4x3(m4x3 * m);
/*! \fn void glMultMatrix3x3(m3x3 * m)
\brief multiplies the current matrix by m
\param m a pointer to a 3x3 matrix
*/

void glMultMatrix3x3(m3x3 * m);
/*! \fn void glRotateXi(int angle)
\brief Rotates the current modelview matrix by angle degrees about the x axis 
\param angle The angle to rotate by (angle is 0-511)
*/

void glRotateXi(int angle);
/*! \fn void glRotateYi(int angle)
\brief Rotates the current modelview matrix by angle degrees about the y axis 
\param angle The angle to rotate by (angle is 0-511)
*/

void glRotateYi(int angle);
/*! \fn void glRotateZi(int angle)
\brief Rotates the current modelview matrix by angle degrees about the z axis 
\param angle The angle to rotate by (angle is 0-511)
*/

void glRotateZi(int angle);
/*! \fn void glRotateX(float angle)
\brief Rotates the current modelview matrix by angle degrees about the x axis 
\param angle The angle to rotate by 
*/

void glRotateX(float angle);
/*! \fn void glRotateY(float angle)
\brief Rotates the current modelview matrix by angle degrees about the y axis 
\param angle The angle to rotate by 
*/

void glRotateY(float angle);
/*! \fn void glRotateZ(float angle)
\brief Rotates the current modelview matrix by angle degrees about the z axis 
\param angle The angle to rotate by 
*/

void glRotateZ(float angle);
/*! \fn void glRotatef32i(int angle, f32 x, f32 y, f32 z)
\brief Rotates the model view matrix by angle about the specified unit vector
\param angle The angle to rotate by
\param x X component of the unit vector axis.
\param y Y component of the unit vector axis.
\param z Z component of the unit vector axis.
*/

void glRotatef32i(int angle, f32 x, f32 y, f32 z);
/*! \fn void glOrthof32(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
\brief Places projection matrix into ortho graphic mode
*/

void glOrthof32(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);
/*! \fn void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
\brief Places projection matrix into ortho graphic mode
*/

void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
/*! \fn void gluLookAtf32(f32 eyex, f32 eyey, f32 eyez, f32 lookAtx, f32 lookAty, f32 lookAtz, f32 upx, f32 upy, f32 upz)
\brief Places the camera at the specified location and orientation (fixed point version)
\param eyex (eyex, eyey, eyez) Location of the camera.
\param eyey (eyex, eyey, eyez) Location of the camera.
\param eyez (eyex, eyey, eyez) Location of the camera.
\param lookAtx (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param lookAty (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param lookAtz (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param upx <upx, upy, upz> Unit vector describing which direction is up for the camera.
\param upy <upx, upy, upz> Unit vector describing which direction is up for the camera.
\param upz <upx, upy, upz> Unit vector describing which direction is up for the camera.
*/

void gluLookAtf32(f32 eyex, f32 eyey, f32 eyez, f32 lookAtx, f32 lookAty, f32 lookAtz, f32 upx, f32 upy, f32 upz);
/*! \fn void gluLookAt(float eyex, float eyey, float eyez, float lookAtx, float lookAty, float lookAtz, float upx, float upy, float upz)
\brief Places the camera at the specified location and orientation (floating point version)
\param eyex (eyex, eyey, eyez) Location of the camera.
\param eyey (eyex, eyey, eyez) Location of the camera.
\param eyez (eyex, eyey, eyez) Location of the camera.
\param lookAtx (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param lookAty (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param lookAtz (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param upx <upx, upy, upz> Unit vector describing which direction is up for the camera.
\param upy <upx, upy, upz> Unit vector describing which direction is up for the camera.
\param upz <upx, upy, upz> Unit vector describing which direction is up for the camera.
*/

void gluLookAt(float eyex, float eyey, float eyez, float lookAtx, float lookAty, float lookAtz, float upx, float upy, float upz);
/*! \fn void gluFrustumf32(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
\brief Specifies the viewing frustum for the projection matrix (fixed point version)
\param left left right top and bottom describe a rectangle located at the near clipping plane
\param right left right top and bottom describe a rectangle located at the near clipping plane
\param top left right top and bottom describe a rectangle located at the near clipping plane
\param bottom left right top and bottom describe a rectangle located at the near clipping plane
\param near Location of a the near clipping plane (parallel to viewing window)
\param far Location of a the far clipping plane (parallel to viewing window)
*/

void gluFrustumf32(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
/*! \fn void gluFrustum(float left, float right, float bottom, float top, float near, float far)
\brief Specifies the viewing frustum for the projection matrix (floating point version)
\param left left right top and bottom describe a rectangle located at the near clipping plane
\param right left right top and bottom describe a rectangle located at the near clipping plane
\param top left right top and bottom describe a rectangle located at the near clipping plane
\param bottom left right top and bottom describe a rectangle located at the near clipping plane
\param near Location of a the near clipping plane (parallel to viewing window)
\param far Location of a the far clipping plane (parallel to viewing window)
*/

void gluFrustum(float left, float right, float bottom, float top, float near, float far);
/*! \fn void gluPerspectivef32(int fovy, f32 aspect, f32 zNear, f32 zFar)
\brief Utility function which sets up the projection matrix (fixed point version)
\param fovy Specifies the field of view in degrees (0 -511) 
\param aspect Specifies the aspect ratio of the screen (normally screen width/screen height)
\param zNear Specifies the near clipping plane
\param zFar Specifies the far clipping plane
*/

void gluPerspectivef32(int fovy, f32 aspect, f32 zNear, f32 zFar);
/*! \fn void gluPerspective(float fovy, float aspect, float zNear, float zFar)
\brief Utility function which sets up the projection matrix (floating point version)
\param fovy Specifies the field of view in degrees  
\param aspect Specifies the aspect ratio of the screen (normally screen width/screen height)
\param zNear Specifies the near clipping plane
\param zFar Specifies the far clipping plane
*/

void gluPerspective(float fovy, float aspect, float zNear, float zFar);
/*! \fn void gluPickMatrix(int x, int y, int width, int height, int viewport[4])
\brief Utility function which generates a picking matrix for selection
\param x 2D x of center  (touch x normally)
\param y 2D y of center  (touch y normally)
\param width width in pixels of the window (3 or 4 is a good number)
\param height height in pixels of the window (3 or 4 is a good number)
\param viewport the current viewport (normaly [0, 0, 255, 191])
*/

void gluPickMatrix(int x, int y, int width, int height, int viewport[4]);
/*! \fn int glTexImage2D(int target, int empty1, int type, int sizeX, int sizeY, int empty2, int param, uint8* texture)
\brief Loads a 2D texture into texture memory and sets the currently bound texture ID to the attributes specified
*/

int glTexImage2D(int target, int empty1, int type, int sizeX, int sizeY, int empty2, int param, uint8* texture);

  /*! \fn void glTexLoadPal(u16* pal, u16 count, u32 addr )
  \brief Loads a palette into the specified texture addr
  */

  void glTexLoadPal(u16* pal, u16 count, u32 addr );
  /*! \fn int gluTexLoadPal(u16* pal, u16 count, uint8 format)
  \brief Loads a palette into the next available palette slot, returns
the addr on
  success or -1
  */

  int gluTexLoadPal(u16* pal, u16 count, uint8 format);
  /*! \fn void glColorTable(uint8 format, uint32 addr)
  \brief Establishes the location of the current palette.
  */

  /*! \fn void glGetTexParameter(void)
  \brief Returns the active texture parameter (constructed from
internal call to
  glTexParameter)
  */
  u32 glGetTexParameter();
/*! \fn void* glGetTexturePointer(	int name)
\brief returns the address alocated to the texure named by name
*/
void* glGetTexturePointer(	int name);

/*! \fn void glColorTable(uint8 format, uint32 addr)
\brief 
*/
  void glColorTable(uint8 format, uint32 addr);
/*! \fn void glBindTexture(int target, int name)
\brief Binds the state machine to the specified texture ID
*/

void glBindTexture(int target, int name);
/*! \fn int glGenTextures(int n, int *names)
\brief Creates room for the specified number of textures
*/

int glGenTextures(int n, int *names);
/*! \fn void glResetTextures(void)
\brief Resets the gl texture state freeing all texture memory
*/

void glResetTextures(void);
/*! \fn void glTexCoord2f32(f32 u, f32 v)
\brief Sends texture coordinates to graphics chip
*/

void glTexCoord2f32(f32 u, f32 v);
/*! \fn void glMaterialf(int mode, rgb color)
\brief specify the material properties to be used in rendering lit polygons
*/

void glMaterialf(int mode, rgb color);
/*! \fn void glResetMatrixStack(void)
\brief Resets matrix stack to top level
*/

void glResetMatrixStack(void);
/*! \fn void glSetOutlineColor(int id, rgb color)
\brief Specifies an edge color for polygons
*/

void glSetOutlineColor(int id, rgb color);
/*! \fn void glSetToonTable(uint16 *table)
\brief Species a toon table
*/

void glSetToonTable(uint16 *table);
/*! \fn void glSetToonTableRange(int start, int end, rgb color)
\brief Sets the toon table range
*/

void glSetToonTableRange(int start, int end, rgb color);
/*! \fn void glReset(void)
\brief Resets the gl state machine (must be called once per frame)
*/

void glReset(void);

/*! \fn void glGetInt(GL_GET_TYPE param, int* i)
\brief Grabs various integer state variables from openGL

\param param The state variable to retrieve
\param i A pointer with room to hold the requested data


*/
void glGetInt(GL_GET_TYPE param, int* i);

/*! \fn void glGetFixed(GL_GET_TYPE param, fixed* f)
\brief Grabs various fixed point state variables from openGL

\param param The state variable to retrieve
\param f A pointer with room to hold the requested data


*/
void glGetFixed(GL_GET_TYPE param, fixed* f);

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
	void glClearDepth(fixed12d3 depth);
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

void glSetAlpha(int alpha) __attribute__((deprecated));
void glAlphaFunc(int alphaThreshold);

void glCutoffDepth(fixed12d3 depth);

#ifdef __cplusplus
}
#endif

#endif

