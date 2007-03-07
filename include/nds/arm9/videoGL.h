/*---------------------------------------------------------------------------------
	$Id: videoGL.h,v 1.39 2007-03-07 05:44:24 gabebear Exp $

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
	Revision 1.38  2007/02/24 21:08:40  gabebear
	changed default glFlush so it uses the Z value for depth instead of W value, the W value doesn't work for ortho views
	
	Revision 1.37  2007/02/22 06:24:25  gabebear
	-  changed glClearColor() so that it handles alpha as a fourth argument; the real OpenGL function takes alpha as a fourth argument.
	-  got rid of glClearAlpha() because glClearColor now handles this, glClearAlpha() was only recently added.
	- changed all array/pointer parameters that are constant in their functions to const so that you can pass arrays of constant data to those functions.
	- fixed glResetMatrixStack(); if a push or pop had just been executed it didn't work properly.
	- fixed some comments
	
	Revision 1.36  2007/02/18 20:13:55  wntrmute
	fix doxygen tag
	
	Revision 1.35  2007/02/12 03:15:53  gabebear
	- changed glCallList() so that it uses syncronous DMA; I think that this finally works reliably.
	- added doxygen comments for FIFO_* commands
	
	Revision 1.34  2007/02/11 13:18:26  wntrmute
	correct doxygen errors
	use GL_TEXTURE_TYPE_ENUM as type, not parameter
	
	Revision 1.33  2007/02/10 16:01:09  gabebear
	- fixed some comments
	- removed CVS revision comments from videoGL.h and videoGL.c that were dated 2005 and older
	
	Revision 1.32  2007/02/10 06:04:53  gabebear
	oops, included videoGL.h in videoGL.h...
	
	Revision 1.31  2007/02/10 05:23:19  gabebear
	doxygen doesn't like "static inline", so changed to using GL_STATIC_INL with define.
	
	Revision 1.30  2007/02/10 05:13:46  gabebear
	- emptied videoGL.inl into videoGL.h so that doxygen works and because having the ability to remove inline functions isn't important anymore(I think).
	- emptied inlineGL.c; inlineGL.c and videoGL.inl should be deleted
	- inlined most functions. GCC "should" be able to work out if something that is inlined in the code really shouldn't be and create object code for the function. I believe I made pretty sane choices so I doubt this will be an issue.
	- fixed TEXTURE_PACK so that u and v are correct.
	- removed glTexCoord1i because it's not terribly useful and the fix to TEXTURE_PACK will make programs that use this and another glTexCoord fail, this was the only function that had S and T mapped correctly. The function does map to the OpenGL call pretty well, but it wasn't used properly in the examples. In the examples it was used to set two texcoords, which was more confusing than just using GFX_TEX_COORD directly. The OpenGL version only sets the S texcoord and sets T to 0.
	- GL_LIGHTING was defined, I deleted it. GL_LIGHTING is used with glEnable to swap between coloring vertices with normals/materials and coloring with the current color. The DS doesn't handle vertex colors like this. The color of a vertex on the DS is determined by the last command that computes a vertex color. The two commands that set vertex colors are glNormal() and glColor(), whichever was last executed determines the color of the vertex drawn. Implementing the OpenGL way of doing this on the DS would be a rather big waste of time and the current define didn't make any sense anyway.
	- removed glTranslate3f32(delta) because it doesn't map to an OpenGL call, or even make much sense. I doubt anyone was using it.
	- removed all depreciated functions
	- linking issues kept me from typing the sizeX/sizeY params on glTexImage2d() and glTexParameter() to GL_TEXTURE_SIZE_ENUM. I'm not sure how to fix this without inlining glTexImage2d() and glTexParameter().
	
	Revision 1.29  2007/01/31 23:10:09  gabebear
	OK, I think that's the last typo in the stuff I just submitted... damn, well I'm sure I jinxed that
	
	Revision 1.28  2007/01/31 23:04:09  gabebear
	corrected problem in doxygen comments just submitted, I don't know why I just started thinking in base 1 instead of 0...
	
	Revision 1.27  2007/01/31 22:57:28  gabebear
	- corrected typo where glClearAlpha() was glClearAplpha()
	- added doxygen for glClear*** functions
	
	Revision 1.26  2007/01/30 00:15:48  gabebear
	 - got rid of extra flags in glEnable and glDisable
	 - added glInit() which does pretty much the same thing that glReset did. It just initializes the GL-state at the start of the program and then never needs called again. Initializing the state explicitly should make code more stable when using different boot methods that may have fiddled with default states.
	 - depreciated glReset, because glInit does the same job better, setting up everythign per frame was a waste
	 - glInit sets up the rear-plane(a.ka. clear-color) as blank instead of a bmp, and sets it's so that it is totally opaque with a poly-ID of zero. This lets antialiasing, and outlining work with simple glEnables!!!
	 - Changed glClearColor so that it sets the rear-plane ID instead of palette[0]
	 - added glClearAlpha() that sets the alpha of the rear-plane
	 - added glClearPolyID() that sets how things get outlined and antialiased
	
	I haven't tested this against the examples, another patch is on the way to fix the examples
	
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
	
	Revision 1.23  2006/08/03 04:56:31  dovoto
	Added gluPickMatrix() ...untested
	
	Revision 1.22  2006/05/08 03:19:51  dovoto
	Added glGetTexturePointer which allows the user to retreive a pointer to texture memory for the named texture.
	
	Small fix to the texture reset code.  The texture name is now reset as well.
	
	Revision 1.21  2006/01/05 08:13:26  dovoto
	Fixed gluLookAt (again)
	Major update to palette handling (likely a breaking change if you were using the gl texture palettes from before)

---------------------------------------------------------------------------------*/
/*! \file videoGL.h 
\brief openGL (ish) interface to DS 3D hardware. 
*/

#ifndef VIDEOGL_ARM9_INCLUDE
#define VIDEOGL_ARM9_INCLUDE

//---------------------------------------------------------------------------------

// for some reason doxygen doesn't like "static inline" but is fine with "GL_STATIC_INL"
#define GL_STATIC_INL static inline

#ifndef ARM9
#error 3D hardware is only available from the ARM9
#endif

//---------------------------------------------------------------------------------
#include <nds/jtypes.h>
#include <nds/arm9/video.h>
#include <nds/dma.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/arm9/math.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/cache.h>

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

typedef uint16 fixed12d3; /*!< \brief Used for depth (glClearDepth, glCutoffDepth) */

#define int_to_12d3(n)    ((n) << 3) /*!< \brief convert int to fixed12d3 */
#define float_to_12d3(n)  ((fixed12d3)((n) * (1 << 3))) /*!< \brief convert float to fixed12d3 */
#define GL_MAX_DEPTH      0x7FFF /*!< \brief the maximum value for type fixed12d3 */

//////////////////////////////////////////////////////////////////////

#define inttof32(n)          ((n) << 12) /*!< \brief convert int to f32 */
#define f32toint(n)          ((n) >> 12) /*!< \brief convert f32 to int */
#define floattof32(n)        ((int32)((n) * (1 << 12))) /*!< \brief convert float to f32 */
#define f32tofloat(n)        (((float)(n)) / (float)(1<<12)) /*!< \brief convert f32 to float */

typedef short int t16;        /*!< \brief text coordinate 12.4 fixed point */
#define f32tot16(n)          ((t16)(n >> 8)) /*!< \brief convert f32 to t16 */
#define inttot16(n)          ((n) << 4) /*!< \brief convert int to t16 */
#define t16toint(n)          ((n) >> 4) /*!< \brief convert t16 to int */
#define floattot16(n)        ((t16)((n) * (1 << 4))) /*!< \brief convert float to t16 */
#define TEXTURE_PACK(u,v)    ((u & 0xFFFF) | ((v) << 16)) /*!< \brief Pack 2 t16 texture coordinate values into a 32bit value */

typedef short int v16;       /*!< \brief vertex 4.12 fixed format */
#define inttov16(n)          ((n) << 12) /*!< \brief convert int to v16 */
#define f32tov16(n)          (n) /*!< \brief f32 to v16 */
#define v16toint(n)          ((n) >> 12) /*!< \brief convert v16 to int */
#define floattov16(n)        ((v16)((n) * (1 << 12))) /*!< \brief convert float to v16 */
#define VERTEX_PACK(x,y)     (((x) & 0xFFFF) | ((y) << 16)) /*!< \brief Pack to v16 values into one 32bit value */

typedef short int v10;       /*!< \brief normal .10 fixed point, NOT USED FOR 10bit VERTEXS!!!*/
#define inttov10(n)          ((n) << 9) /*!< \brief convert int to v10 */
#define f32tov10(n)          ((v10)(n >> 3)) /*!< \brief convert f32 to v10 */
#define v10toint(n)          ((n) >> 9) /*!< \brief convert v10 to int */
#define floattov10(n)        ((v10)((n) * (1 << 9))) /*!< \brief convert float to v10 */
#define NORMAL_PACK(x,y,z)   (((x) & 0x3FF) | (((y) & 0x3FF) << 10) | ((z) << 20)) /*!< \brief Pack 3 v10 normals into a 32bit value */

//////////////////////////////////////////////////////////////////////

typedef unsigned short rgb; /*!< \brief Holds a color value. 1bit alpha, 5bits red, 5bits green, 5bits blue. */

/*! \brief Holds a Matrix of 3x3 */
typedef struct {
	int32 m[9]; /*!< array that holds matrix */
} m3x3;

/*! \brief Holds a Matrix of 4x4 */
typedef struct {
	int32 m[16]; /*!< array that holds matrix */
} m4x4;

/*! \brief Holds a Matrix of 4x3 */
typedef struct {
	int32 m[12]; /*!< array that holds matrix */
} m4x3;

/*! \brief Holds a Vector<BR>related functions: glScalev(), glTranslatev() */
typedef struct {
	int32 x, y, z;
} GLvector;

//////////////////////////////////////////////////////////////////////

#define GL_FALSE     0
#define GL_TRUE      1

/*! \brief Enums selecting polygon draw mode<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A><BR>
related functions: glBegin() */
typedef enum {
	GL_TRIANGLES      = 0, /*!< draw triangles with each 3 vertices defining a triangle */ 
	GL_QUADS          = 1, /*!< draw quads with each 4 vertices defining a quad */
	GL_TRIANGLE_STRIP = 2, /*!< draw triangles with the first triangle defined by 3 vertices, then each additional triangle being defined by one additional vertex */
	GL_QUAD_STRIP     = 3, /*!< draw quads with the first quad being defined by 4 vertices, then each additional triangle being defined by 2 vertices. */
	GL_TRIANGLE       = 0, /*!< same as GL_TRIANGLES, old non-OpenGL version */
	GL_QUAD           = 1  /*!< same as GL_QUADS, old non-OpenGL version */
} GL_GLBEGIN_ENUM;

/*! \brief Enums selecting matrix mode<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply</A><BR>
related functions: glMatrixMode() */
typedef enum {
	GL_PROJECTION     = 0, /*!< used to set the Projection Matrix */
	GL_POSITION       = 1, /*!< used to set the Position Matrix */
	GL_MODELVIEW      = 2, /*!< used to set the Modelview Matrix */
	GL_TEXTURE        = 3  /*!< used to set the Texture Matrix */
} GL_MATRIX_MODE_ENUM;

/*! \brief Enums for setting up materials<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A><BR>
related functions: glMaterialf() */
typedef enum {
	GL_AMBIENT             = 0x01, /*!< sets the ambient color for the material. The color when the normal is not facing light*/
	GL_DIFFUSE             = 0x02, /*!< sets the diffuse color for the material. The color when the normal is facing light */
	GL_AMBIENT_AND_DIFFUSE = 0x03, /*!< sets the set ambient and diffuse colors for the material; just a two-in-one of the above.*/
	GL_SPECULAR            = 0x04, /*!< sets the specular color for the material. The glossy(highlight) color of the polygon */
	GL_SHININESS           = 0x08, /*!< sets the shininess color for the material. The color that shines back to the user. I have shiny pants! */
	GL_EMISSION            = 0x10  /*!< sets the emission color for the material. bright color that is indepentant of normals and lights*/
} GL_MATERIALS_ENUM;

/*! \brief Enums for setting how polygons will be displayed<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes</A><BR>
related functions: glPolyFmt(), glInit(), POLY_ALPHA(), POLY_ID() */
enum GL_POLY_FORMAT_ENUM {
	POLY_DECAL           = (1<<4), /*!< enable polygon decal mode */
	POLY_TOON_SHADING    = (1<<5), /*!< enable toon shading */
	POLY_CULL_BACK       = (1<<7), /*!< cull rear polygons */
	POLY_CULL_FRONT      = (1<<6), /*!< cull front polygons */
	POLY_CULL_NONE       = (3<<6), /*!< don't cull any polygons */
	POLY_FORMAT_LIGHT0   = (1<<0), /*!< enable light number 0 */
	POLY_FORMAT_LIGHT1   = (1<<1), /*!< enable light number 1 */
	POLY_FORMAT_LIGHT2   = (1<<2), /*!< enable light number 2 */
	POLY_FORMAT_LIGHT3   = (1<<3)  /*!< enable light number 3 */
};

/*! \brief Enums for size of a texture, specify one for horizontal and one for vertical
related functions: glTexImage2d(), glTexParameter() */
enum GL_TEXTURE_SIZE_ENUM {
	TEXTURE_SIZE_8    = 0, /*!< 8 texels */
	TEXTURE_SIZE_16   = 1, /*!< 16 texels */ 
	TEXTURE_SIZE_32   = 2, /*!< 32 texels */
	TEXTURE_SIZE_64   = 3, /*!< 64 texels */
	TEXTURE_SIZE_128  = 4, /*!< 128 texels */
	TEXTURE_SIZE_256  = 5, /*!< 256 texels */
	TEXTURE_SIZE_512  = 6, /*!< 512 texels */
	TEXTURE_SIZE_1024 = 7  /*!< 1024 texels */
};

/*! \brief Enums for texture parameters, such as texture wrapping and texture coord stuff<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes</A><BR>
related functions: glTexImage2d(), glTexParameter() */
enum GL_TEXTURE_PARAM_ENUM {
	GL_TEXTURE_WRAP_S = (1 << 16), /*!< wrap(repeat) texture on S axis */
	GL_TEXTURE_WRAP_T = (1 << 17), /*!< wrap(repeat) texture on T axis */
	GL_TEXTURE_FLIP_S = (1 << 18), /*!< flip texture on S axis when wrapping */
	GL_TEXTURE_FLIP_T = (1 << 19), /*!< flip texture on T axis when wrapping */
	GL_TEXTURE_COLOR0_TRANSPARENT = (1<<29), /*!< interpret color 0 as clear, same as GL_TEXTURE_ALPHA_MASK */
	GL_TEXTURE_ALPHA_MASK = (1 << 29), /*!< interpret color 0 as clear, same as GL_TEXTURE_COLOR0_TRANSPARENT */
	TEXGEN_OFF      = (0<<30), /*!< use unmodified texcoord */
	TEXGEN_TEXCOORD = (1<<30), /*!< multiply texcoords by the texture-matrix */
	TEXGEN_NORMAL   = (2<<30), /*!< set texcoords equal to normal * texture-matrix, used for spherical reflection mapping */
	TEXGEN_POSITION = (3<<30)  /*!< set texcoords equal to vertex * texture-matrix */
};

/*! \brief 3D Display Control Register Enums<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dtextureformats">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dtextureformats</A><BR>
related functions: glTexImage2d(), glTexParameter() */
typedef enum {
	GL_RGB32_A3   = 1, /*!< 32 color palette, 3 bits of alpha */
	GL_RGB4       = 2, /*!< 4 color palette */
	GL_RGB16      = 3, /*!< 16 color palette */
	GL_RGB256     = 4, /*!< 256 color palette */
	GL_COMPRESSED = 5, /*!< compressed texture */
	GL_RGB8_A5    = 6, /*!< 8 color palette, 5 bits of alpha */
	GL_RGBA       = 7, /*!< 15 bit direct color, 1 bit of alpha */
	GL_RGB        = 8  /*!< 15 bit direct color, manually sets alpha bit to 1 */
} GL_TEXTURE_TYPE_ENUM;

/*! \brief 3D Display Control Register Enums<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol">GBATEK http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol</A><BR>
related functions: glEnable(), glDisable(), glInit() */
enum DISP3DCNT_ENUM {
	GL_TEXTURE_2D      = (1<<0),  /*!< enable/disable textures on the geometry engine */
	GL_TOON_HIGHLIGHT  = (1<<1),  /*!< enable = Highlight shading; disable = Toon shading */
	GL_ALPHA_TEST      = (1<<2),  /*!< whether to use the alpha threshold set in glAlphaFunc() */
	GL_BLEND           = (1<<3),  /*!< enable/disable alpha blending */
	GL_ANTIALIAS       = (1<<4),  /*!< nable/disable edge antialiasing; polygons must have different polygon IDs for the effect to work and the rear plane must be clear */
	GL_OUTLINE         = (1<<5),  /*!< enable/disable edge coloring; the high 3bits of the polygon ID determine the color; glSetOutlineColor() sets the available colors */
	GL_COLOR_UNDERFLOW = (1<<12), /*!< enable = color buffer underflow, setting resets overflow flag; disable = no color buffer overflow */
	GL_POLY_OVERFLOW   = (1<<13), /*!< enable = polygon/vertex buffer overflow, setting resets overflow flag; disable = no polygon/vertex buffer overflow */
	GL_CLEAR_BMP       = (1<<14)  /*!< rear/clear plane is in BMP mode; disable = rear/color plane is in clear mode */
};

/*! \brief related functions: glGetInt(), glGetFixed() */
typedef enum {
	GL_GET_VERTEX_RAM_COUNT,    /*!< returns a count of vertexes currently stored in hardware vertex ram. Use glGetInt() to retrieve */
	GL_GET_POLYGON_RAM_COUNT,   /*!< returns a count of polygons currently stored in hardware polygon ram. Use glGetInt() to retrieve */
	GL_GET_MATRIX_ROTATION,     /*!< returns the current 3x3 rotation matrix. Use glGetFixed() to retrieve */
	GL_GET_MATRIX_POSITION,     /*!< returns the current 4x4 position matrix. Use glGetFixed() to retrieve */
	GL_GET_MATRIX_PROJECTION,   /*!< returns the current 4x4 projection matrix. Use glGetFixed() to retrieve */
	GL_GET_MATRIX_MODELVIEW,    /*!< returns the current 4x4 modelview matrix. Use glGetFixed() to retrieve */
	GL_GET_TEXTURE_WIDTH,       /*!< returns the width of the currently bound texture. Use glGetInt() to retrieve */
	GL_GET_TEXTURE_HEIGHT       /*!< returns the height of the currently bound texture. Use glGetInt() to retrieve */
} GL_GET_ENUM;


/*! \brief Enums for glFlush() Enums<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol">GBATEK http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol</A><BR>
related functions: glEnable(), glDisable(), glInit() */
enum GLFLUSH_ENUM {
	GL_TRANS_MANUALSORT = (1<<0), /*!< enable manual sorting of translucent polygons, otherwise uses Y-sorting */
	GL_WBUFFERING       = (1<<1)  /*!< enable W depth buffering of vertices, otherwise uses Z depth buffering */
};

////////////////////////////////////////////////////////////
// Misc. constants
////////////////////////////////////////////////////////////

#define MAX_TEXTURES 2048  //this should be enough ! but feel free to change
#define GL_LIGHTING    1   // no idea what this is for / who defined it

//---------------------------------------------------------------------------------
//Fifo commands
//---------------------------------------------------------------------------------

#define FIFO_COMMAND_PACK(c1,c2,c3,c4) (((c4) << 24) | ((c3) << 16) | ((c2) << 8) | (c1)) /*!< \brief packs four packed commands into a 32bit command for sending to the GFX FIFO */

#define REG2ID(r)				(u8)( ( ((u32)(&(r)))-0x04000400 ) >> 2 ) /*!< \brief converts a GFX command for use in a packed command list */

#define FIFO_NOP				REG2ID(GFX_FIFO) /*!< \brief packed command for nothing, just here to pad your command lists */
#define FIFO_STATUS				REG2ID(GFX_STATUS) /*!< \brief packed command for geometry engine status register<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dstatus">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dstatus</A> */
#define FIFO_COLOR				REG2ID(GFX_COLOR) /*!< \brief packed command for vertex color directly<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes</A> */

#define FIFO_VERTEX16			REG2ID(GFX_VERTEX16) /*!< \brief packed command for a vertex with 3 16bit paramaters(and 16bits of padding)<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A> */
#define FIFO_TEX_COORD			REG2ID(GFX_TEX_COORD) /*!< \brief packed command for a texture coordinate<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dtexturecoordinates">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dtexturecoordinates</A> */
#define FIFO_TEX_FORMAT			REG2ID(GFX_TEX_FORMAT) /*!< \brief packed command for texture format<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dtextureformats">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dtextureformats</A> */
#define FIFO_PAL_FORMAT			REG2ID(GFX_PAL_FORMAT) /*!< \brief packed command for texture palette attributes<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes</A> */

#define FIFO_CLEAR_COLOR		REG2ID(GFX_CLEAR_COLOR) /*!< \brief packed command for clear color of the rear plane<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3drearplane">GBATEK http://nocash.emubase.de/gbatek.htm#ds3drearplane</A> */
#define FIFO_CLEAR_DEPTH		REG2ID(GFX_CLEAR_DEPTH) /*!< \brief sets depth of the rear plane<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3drearplane">GBATEK http://nocash.emubase.de/gbatek.htm#ds3drearplane</A> */

#define FIFO_LIGHT_VECTOR		REG2ID(GFX_LIGHT_VECTOR) /*!< \brief packed command for direction of a light source<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */
#define FIFO_LIGHT_COLOR		REG2ID(GFX_LIGHT_COLOR) /*!< \brief packed command for color for a light<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */
#define FIFO_NORMAL				REG2ID(GFX_NORMAL) /*!< \brief packed command for normal for following vertices<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */

#define FIFO_DIFFUSE_AMBIENT	REG2ID(GFX_DIFFUSE_AMBIENT) /*!< \brief packed command for setting diffuse and ambient material properties for the following vertices<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */
#define FIFO_SPECULAR_EMISSION	REG2ID(GFX_SPECULAR_EMISSION) /*!< \brief packed command for setting specular and emmissive material properties for the following vertices<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */
#define FIFO_SHININESS			REG2ID(GFX_SHININESS) /*!< \brief packed command for setting the shininess table to be used for the following vertices<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A> */

#define FIFO_POLY_FORMAT		REG2ID(GFX_POLY_FORMAT) /*!< \brief packed command for setting polygon attributes<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes</A> */

#define FIFO_BEGIN				REG2ID(GFX_BEGIN) /*!< \brief packed command that starts a polygon vertex list<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A>*/
#define FIFO_END				REG2ID(GFX_END) /*!< \brief packed command that has no discernable effect, it's probably best to never use it since it bloats the size of the list.<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygondefinitionsbyvertices</A>*/
#define FIFO_FLUSH				REG2ID(GFX_FLUSH) /*!< \brief packed command that has the same effect as swiWaitForVBlank()<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol">GBATEK http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol</A> */
#define FIFO_VIEWPORT			REG2ID(GFX_VIEWPORT) /*!< \brief packed command for setting viewport<BR><A HREF="http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol">GBATEK http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol</A> */


#ifdef __cplusplus
extern "C" {
#endif
	
/*! \brief Sets texture coordinates for following vertices<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes</A>
\warning FLOAT VERSION!!!! please use glTexCoord2t16()
\param s S(a.k.a. U) texture coordinate (0.0 - 1.0)
\param t T(a.k.a. V) texture coordinate (0.0 - 1.0)*/
void glTexCoord2f(float s, float t);

/*! \brief Rotates the model view matrix by angle about the specified unit vector
\param angle The angle to rotate by
\param x X component of the unit vector axis.
\param y Y component of the unit vector axis.
\param z Z component of the unit vector axis. */
void glRotatef32i(int angle, int32 x, int32 y, int32 z);

/*! \brief Loads a 2D texture into texture memory and sets the currently bound texture ID to the attributes specified
\param target not used, just here for OpenGL compatibility
\param empty1 not used, just here for OpenGL compatibility
\param type The format of the texture
\param sizeX the horizontal size of the texture; valid sizes are enumerated in GL_TEXTURE_TYPE_ENUM
\param sizeY the vertical size of the texture; valid sizes are enumerated in GL_TEXTURE_TYPE_ENUM
\param empty2 not used, just here for OpenGL compatibility
\param param parameters for the texture
\param texture pointer to the texture data to load */
int glTexImage2D(int target, int empty1, GL_TEXTURE_TYPE_ENUM type, int sizeX, int sizeY, int empty2, int param, const uint8* texture);

/*! \brief Loads a palette into the specified texture addr
\param pal pointer to the palette to load
\param count the size of the palette
\param addr the offset in VRAM to load the palette */
void glTexLoadPal(const u16* pal, u16 count, u32 addr );

/*! \brief Loads a palette into the next available palette slot, returns the addr on success or -1
\param pal pointer to the palette to load
\param count the size of the palette
\param format the format of the texture */
int gluTexLoadPal(const u16* pal, u16 count, uint8 format);

/*! \brief Set parameters for the current texture. Although named the same as its gl counterpart, it is not compatible. Effort may be made in the future to make it so.
\param sizeX the horizontal size of the texture; valid sizes are enumerated in GL_TEXTURE_TYPE_ENUM
\param sizeY the vertical size of the texture; valid sizes are enumerated in GL_TEXTURE_TYPE_ENUM
\param addr offset into VRAM where you put the texture
\param mode the type of texture
\param param paramaters for the texture */
void glTexParameter(	uint8 sizeX, uint8 sizeY,
						const uint32* addr,
						GL_TEXTURE_TYPE_ENUM mode,
						uint32 param) ;

/*! \brief Returns the active texture parameter (constructed from internal call to glTexParameter) */
u32 glGetTexParameter();

/*! \brief returns the address alocated to the texure named by name 
\param name the name of the texture to get a pointer to */
void* glGetTexturePointer(	int name);

/*! \brief glBindTexure sets the current named texture to the active texture. Target is ignored as all DS textures are 2D
\param target ignored, only here for OpenGL compatability
\param name the name(int value) to set to the current texture */
void glBindTexture(int target, int name);

/*! \brief glColorTable establishes the location of the current palette. Roughly follows glColorTableEXT. Association of palettes with named textures is left to the application. */
void glColorTable(uint8 format, uint32 addr);

/*! \brief Creates room for the specified number of textures
\param n the number of textures to generate
\param names pointer to the names array to fill */
int glGenTextures(int n, int *names);

/*! \brief Resets the gl texture state freeing all texture memory */
void glResetTextures(void);

/*! \brief Sets texture coordinates for following vertices<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes</A>
\param u U(a.k.a. S) texture coordinate (0.0 - 1.0)
\param v V(a.k.a. T) texture coordinate (0.0 - 1.0)*/
void glTexCoord2f32(int32 u, int32 v);

/*! \brief specify the material properties to be used in rendering lit polygons
\param mode which material property to change
\param color the color to set for that material property */
void glMaterialf(GL_MATERIALS_ENUM mode, rgb color);

/*! \brief Initializes the gl state machine (must be called once before using gl calls) */
void glInit(void);

/*! \brief Grabs integer state variables from openGL
\param param The state variable to retrieve
\param i pointer with room to hold the requested data */
void glGetInt(GL_GET_ENUM param, int* i);

/*! \brief sets the color of the rear-plane(a.k.a Clear Color/Plane)
\param red component (0-31)
\param green component (0-31)
\param blue component (0-31)
\param alpha from 0(clear) to 31(opaque)*/
void glClearColor(uint8 red, uint8 green, uint8 blue, uint8 alpha);

/*! \brief sets the polygon ID of the rear-plane(a.k.a. Clear/Color Plane), useful for antialiasing and edge coloring
\param ID the polygon ID to give the rear-plane */
void glClearPolyID(uint8 ID);

#ifdef __cplusplus
}
#endif
		

















//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
////////////                                                                  ////////////
////////////                                                                  ////////////
////////////                                                                  ////////////
////////////                       INLINED FUNCTIONS                          ////////////
////////////                                                                  ////////////
////////////                                                                  ////////////
////////////                                                                  ////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////



/*! \brief used in glPolyFmt() to set the alpha level for the following polygons, set to 0 for wireframe mode
\param n the level of alpha (0-31) */
GL_STATIC_INL uint32 POLY_ALPHA(int n) { return (uint32)((n) << 16); };

/*! \brief used in glPolyFmt() to set the Polygon ID for the following polygons
\param n the ID to set for following polygons (0-63) */
GL_STATIC_INL uint32 POLY_ID(int n) { return (uint32)((n)<<24); };

/*! \brief Starts a polygon group
\param mode the draw mode for the polygon */
GL_STATIC_INL void glBegin(GL_GLBEGIN_ENUM mode) { GFX_BEGIN = mode; }

/*! \brief Ends a polygon group, this seems to be a dummy function that does absolutely nothing, feel free to never use it. */
GL_STATIC_INL void glEnd(void) { GFX_END = 0; }

/*! \brief generally set to GL_MAX_DEPTH. OK, I'm not really sure what this does --gabebear<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3drearplane">GBATEK http://nocash.emubase.de/gbatek.htm#ds3drearplane</A>
\param depth Something to do with the depth buffer, generally set to GL_MAX_DEPTH */
GL_STATIC_INL void glClearDepth(uint16 depth) { GFX_CLEAR_DEPTH = depth; }

/*! \brief Set the color for following vertices
\param red the red component (0-31)
\param green the green component (0-31)
\param blue the blue component (0-31) */

GL_STATIC_INL void glColor3b(uint8 red, uint8 green, uint8 blue) { GFX_COLOR = (vuint32)RGB15(red>>3, green>>3, blue>>3); }

/*! \brief Set the color for following vertices
\param color the 15bit color value */
GL_STATIC_INL void glColor(rgb color) { GFX_COLOR = (vuint32)color; }

/*! \brief specifies a vertex
\param x the x component for the vertex
\param y the y component for the vertex
\param z the z component for the vertex */
GL_STATIC_INL void glVertex3v16(v16 x, v16 y, v16 z) {
	GFX_VERTEX16 = (y << 16) | (x & 0xFFFF);
	GFX_VERTEX16 = ((uint32)(uint16)z);
}

/*! \brief Sets texture coordinates for following vertices<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dtextureattributes</A>
\param u U(a.k.a. S) texture coordinate in texels
\param v V(a.k.a. T) texture coordinate in texels */
GL_STATIC_INL void glTexCoord2t16(t16 u, t16 v) { GFX_TEX_COORD = TEXTURE_PACK(u,v); }

/*! \brief Pushs the current matrix onto the stack<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixstack">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixstack</A> */
GL_STATIC_INL void glPushMatrix(void) { MATRIX_PUSH = 0; }

/*! \brief Pops num matrices off the stack<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixstack">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixstack</A>
\param num the number to pop down the stack */
GL_STATIC_INL void glPopMatrix(int32 num) { MATRIX_POP = num; }

/*! \brief Restores the current matrix from a location in the stack<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixstack">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixstack</A>
\param index the place in the stack to restore to */
GL_STATIC_INL void glRestoreMatrix(int32 index) { MATRIX_RESTORE = index; }

/*! \brief Place the current matrix into the stack at a location<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixstack">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixstack</A>
\param index the place in the stack to put the current matrix */
GL_STATIC_INL void glStoreMatrix(int32 index) { MATRIX_STORE = index; }

/*! \brief multiply the current matrix by a translation matrix<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply</A>
\param v the vector to translate by */
GL_STATIC_INL void glScalev(const GLvector* v) { 
	MATRIX_SCALE = v->x;
	MATRIX_SCALE = v->y;
	MATRIX_SCALE = v->z;
}

/*! \brief multiply the current matrix by a translation matrix<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply</A>
\param v the vector to translate by */
GL_STATIC_INL void glTranslatev(const GLvector* v) {
	MATRIX_TRANSLATE = v->x;
	MATRIX_TRANSLATE = v->y;
	MATRIX_TRANSLATE = v->z;
}

/*! \brief multiply the current matrix by a translation matrix<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply</A>
\param x translation on the x axis
\param y translation on the y axis
\param z translation on the z axis */
GL_STATIC_INL void glTranslate3f32(int32 x, int32 y, int32 z) {
	MATRIX_TRANSLATE = x;
	MATRIX_TRANSLATE = y;
	MATRIX_TRANSLATE = z;
}

/*! \brief multiply the current matrix by a scale matrix<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply</A>
\param factor the factor to scale by */
GL_STATIC_INL void glScalef32(int32 factor) {
	MATRIX_SCALE = factor;
	MATRIX_SCALE = factor;
	MATRIX_SCALE = factor;
}

/*! \brief set a light up. Only parallel light sources are supported on the DS<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A>
\param id the number of the light to setup
\param color the color of the light
\param x the x component of the lights directional vector. Direction must be normalized
\param y the y component of the lights directional vector. Direction must be normalized
\param z the z component of the lights directional vector. Direction must be normalized */
GL_STATIC_INL void glLight(int id, rgb color, v10 x, v10 y, v10 z) {
	id = (id & 3) << 30;
	GFX_LIGHT_VECTOR = id | ((z & 0x3FF) << 20) | ((y & 0x3FF) << 10) | (x & 0x3FF);
	GFX_LIGHT_COLOR = id | color;
}

/*! \brief the normal to use for following vertices<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A>
\param normal the packed normal(3 * 10bit x, y, z) */
GL_STATIC_INL void glNormal(uint32 normal) { GFX_NORMAL = normal; }

/*! \brief loads an identity matrix to the current matrix, same as glIdentity(void) */
GL_STATIC_INL void glLoadIdentity(void) { MATRIX_IDENTITY = 0; }

/*! \brief loads an identity matrix to the current matrix, same as glLoadIdentity(void) */
GL_STATIC_INL void glIdentity(void) { MATRIX_IDENTITY = 0; }

/*! \brief change the current matrix mode<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply</A><BR>
\param mode the mode for the matrix */
GL_STATIC_INL void glMatrixMode(GL_MATRIX_MODE_ENUM mode) { MATRIX_CONTROL = mode; }

/*! \brief specify the viewport for following drawing, can be set several times per frame.<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol">GBATEK http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol</A>
\param x1 the left of the viewport
\param y1 the bottom of the viewport
\param x2 the right of the viewport
\param y2 the top of the viewport */
GL_STATIC_INL void glViewPort(uint8 x1, uint8 y1, uint8 x2, uint8 y2) { GFX_VIEWPORT = (x1) + (y1 << 8) + (x2 << 16) + (y2 << 24); }

/*! \brief Waits for a Vblank and swaps the buffers(like swiWaitForVBlank), but lets you specify some 3D options<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol">GBATEK http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol</A>
\param mode flags from GLFLUSH_ENUM for enabling Y-sorting of translucent polygons and W-Buffering of all vertices*/
GL_STATIC_INL void glFlush(uint32 mode) { GFX_FLUSH = mode; }

/*! \brief The DS uses a table for shinyness..this generates a half-ass one */
GL_STATIC_INL void glMaterialShinyness(void) {
	uint32 shiny32[128/4];
	uint8  *shiny8 = (uint8*)shiny32;
	
	int i;
	
	for (i = 0; i < 128 * 2; i += 2)
		shiny8[i>>1] = i;
	
	for (i = 0; i < 128 / 4; i++)
		GFX_SHININESS = shiny32[i];
}

/*! \brief throws a packed list of commands into the graphics FIFO via asyncronous DMA<BR>
The first 32bits is the length of the packed command list, followed by a the packed list.<BR>
If you want to do this really fast then write your own code that that does this synchronously and only flushes the cache when the list is changed<BR>
There is sometimes a problem when you pack the GFX_END command into a list, so don't. GFX_END is a dummy command and never needs called<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dgeometrycommands">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dgeometrycommands</A> */
GL_STATIC_INL void glCallList(const u32* list) {
	u32 count = *list++;
	
	// flush the area that we are going to DMA
	DC_FlushRange(list, count*4);
	
	// don't start DMAing while anything else is being DMAed because FIFO DMA is touchy as hell
	//    If anyone can explain this better that would be great. -- gabebear
	while((DMA_CR(0) & DMA_BUSY)||(DMA_CR(1) & DMA_BUSY)||(DMA_CR(2) & DMA_BUSY)||(DMA_CR(3) & DMA_BUSY));
	
	// send the packed list asynchronously via DMA to the FIFO
	DMA_SRC(0) = (uint32)list;
	DMA_DEST(0) = 0x4000400;
	DMA_CR(0) = DMA_FIFO | count;
	while(DMA_CR(0) & DMA_BUSY);
}

/*! \brief Set the parameters for polygons rendered on the current frame<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonattributes</A>
\param params the paramters to set for the polygons for the current frame. valid paramters are enumerated in GL_POLY_FORMAT_ENUM and in the functions POLY_ALPHA() and POLY_ID() */
GL_STATIC_INL void glPolyFmt(uint32 params) { GFX_POLY_FORMAT = params; }

/*! \brief Enables various gl states (blend, alpha test, etc..)
\param bits bit mask of desired attributes, attributes are enumerated in DISP3DCNT_ENUM */
GL_STATIC_INL void glEnable(int bits) { GFX_CONTROL |= bits; }

/*! \brief Disables various gl states (blend, alpha test, etc..)
\param bits bit mask of desired attributes, attributes are enumerated in DISP3DCNT_ENUM */
GL_STATIC_INL void glDisable(int bits) { GFX_CONTROL &= ~bits; }

/*! \brief Loads a 4x4 matrix into the current matrix
\param m pointer to a 4x4 matrix */
GL_STATIC_INL void glLoadMatrix4x4(const m4x4 *m) {
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

/*! \brief Loads a 4x3 matrix into the current matrix
\param m pointer to a 4x4 matrix */
GL_STATIC_INL void glLoadMatrix4x3(const m4x3 * m) {
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

/*! \brief Multiplies the current matrix by m
\param m pointer to a 4x4 matrix */
GL_STATIC_INL void glMultMatrix4x4(const m4x4 * m) {
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

/*! \brief multiplies the current matrix by
\param m pointer to a 4x3 matrix */
GL_STATIC_INL void glMultMatrix4x3(const m4x3 * m) {
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

/*! \brief multiplies the current matrix by m
\param m pointer to a 3x3 matrix */
GL_STATIC_INL void glMultMatrix3x3(const m3x3 * m) {
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

/*! \brief Rotates the current modelview matrix by angle about the x axis 
\param angle The angle to rotate by (angle is 0-511) */
GL_STATIC_INL void glRotateXi(int angle) {
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

/*! \brief Rotates the current modelview matrix by angle about the y axis 
\param angle The angle to rotate by (angle is 0-511) */
GL_STATIC_INL void glRotateYi(int angle) {
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

/*! \brief Rotates the current modelview matrix by angle about the z axis 
\param angle The angle to rotate by (angle is 0-511) */
GL_STATIC_INL void glRotateZi(int angle) {
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


/*! \brief Multiplies the current matrix into ortho graphic mode
\param left left vertical clipping plane
\param right right vertical clipping plane
\param bottom bottom vertical clipping plane
\param top top vertical clipping plane
\param zNear near clipping plane
\param zFar far clipping plane */
GL_STATIC_INL void glOrthof32(int32 left, int32 right, int32 bottom, int32 top, int32 zNear, int32 zFar) {
	MATRIX_MULT4x4 = divf32(inttof32(2), right - left);     
	MATRIX_MULT4x4 = 0;  
	MATRIX_MULT4x4 = 0;      
	MATRIX_MULT4x4 = 0;

	MATRIX_MULT4x4 = 0;  
	MATRIX_MULT4x4 = divf32(inttof32(2), top - bottom);     
	MATRIX_MULT4x4 = 0;    
	MATRIX_MULT4x4 = 0;

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

/*! \brief Places the camera at the specified location and orientation (fixed point version)
\param eyex (eyex, eyey, eyez) Location of the camera.
\param eyey (eyex, eyey, eyez) Location of the camera.
\param eyez (eyex, eyey, eyez) Location of the camera.
\param lookAtx (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param lookAty (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param lookAtz (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param upx <upx, upy, upz> Unit vector describing which direction is up for the camera.
\param upy <upx, upy, upz> Unit vector describing which direction is up for the camera.
\param upz <upx, upy, upz> Unit vector describing which direction is up for the camera. */
GL_STATIC_INL void gluLookAtf32(int32 eyex, int32 eyey, int32 eyez, int32 lookAtx, int32 lookAty, int32 lookAtz, int32 upx, int32 upy, int32 upz) {
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

/*! \brief Specifies the viewing frustum for the projection matrix (fixed point version)
\param left left right top and bottom describe a rectangle located at the near clipping plane
\param right left right top and bottom describe a rectangle located at the near clipping plane
\param top left right top and bottom describe a rectangle located at the near clipping plane
\param bottom left right top and bottom describe a rectangle located at the near clipping plane
\param near Location of a the near clipping plane (parallel to viewing window)
\param far Location of a the far clipping plane (parallel to viewing window) */
GL_STATIC_INL void glFrustumf32(int32 left, int32 right, int32 bottom, int32 top, int32 near, int32 far) {
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

/*! \brief Utility function which sets up the projection matrix (fixed point version)
\param fovy Specifies the field of view in degrees (0 -511) 
\param aspect Specifies the aspect ratio of the screen (normally screen width/screen height)
\param zNear Specifies the near clipping plane
\param zFar Specifies the far clipping plane */
GL_STATIC_INL void gluPerspectivef32(int fovy, int32 aspect, int32 zNear, int32 zFar) {
	int32 xmin, xmax, ymin, ymax;
	
	ymax = mulf32(zNear, TAN[fovy & LUT_MASK]);
	ymin = -ymax;
	xmin = mulf32(ymin, aspect);
	xmax = mulf32(ymax, aspect);
	
	glFrustumf32(xmin, xmax, ymin, ymax, zNear, zFar);
}

/*! \brief Utility function which generates a picking matrix for selection
\param x 2D x of center  (touch x normally)
\param y 2D y of center  (touch y normally)
\param width width in pixels of the window (3 or 4 is a good number)
\param height height in pixels of the window (3 or 4 is a good number)
\param viewport the current viewport (normaly {0, 0, 255, 191}) */
GL_STATIC_INL void gluPickMatrix(int x, int y, int width, int height, const int viewport[4]) {
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

/*! \brief Resets matrix stack to top level */
GL_STATIC_INL void glResetMatrixStack(void) {
	// stack overflow ack ?
	GFX_STATUS |= 1 << 15;
	
	// make sure there are no push/pops that haven't executed yet
	while(GFX_STATUS & BIT(14));
	
	// pop the stacks to the top...seems projection stack is only 1 deep??
	glMatrixMode(GL_PROJECTION);
	glPopMatrix((GFX_STATUS>>13) & 1);
	
	// 31 deep modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix((GFX_STATUS >> 8) & 0x1F);
}

/*! \brief Specifies an edge color for polygons 
\param id which outline color to set (0-7)
\param color the 15bit color to set */
GL_STATIC_INL void glSetOutlineColor(int id, rgb color) { GFX_EDGE_TABLE[id] = color; }

/*! \brief Loads a toon table 
\param pointer to the 32 color palette to load into the toon table*/
GL_STATIC_INL void glSetToonTable(const uint16 *table) {
	int i;
	for(i = 0; i < 32; i++ )
		GFX_TOON_TABLE[i] = table[i];
}

/*! \brief Sets a range of colors on the toon table
\param start the start of the range
\param end the end of the range
\param color the color to set for that range */
GL_STATIC_INL void glSetToonTableRange(int start, int end, rgb color) {
	int i;
	for(i = start; i <= end; i++ )
		GFX_TOON_TABLE[i] = color;
}

/*! \brief Grabs fixed format of state variables
\param param The state variable to retrieve
\param f pointer with room to hold the requested data */
GL_STATIC_INL void glGetFixed(const GL_GET_ENUM param, int32* f) {
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

/*! \brief set the minimum alpha value that will be used<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol">GBATEK http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol</A>
\param alphaThreshold minimum alpha value that will be used (0-15) */
GL_STATIC_INL void glAlphaFunc(int alphaThreshold) { GFX_ALPHA_TEST = alphaThreshold; }

/*! \brief set the minimum alpha value that will be used<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol">GBATEK http://nocash.emubase.de/gbatek.htm#ds3ddisplaycontrol</A>
\param depth polygons that are beyond this depth will not be drawn; 15bit value. */
GL_STATIC_INL void glCutoffDepth(fixed12d3 depth) { GFX_CUTOFF_DEPTH = depth; }









//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
////////////                                                                  ////////////
////////////                                                                  ////////////
////////////                                                                  ////////////
////////////                    INLINED FlOAT WRAPPERS                        ////////////
////////////                                                                  ////////////
////////////                                                                  ////////////
////////////                                                                  ////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//
//  All floating point functions except glTexCoord2f.
//    glTexCoord2f is a regular function because of the way texture memory is allocated.
//

/*! \brief specifies a vertex location
\warning FLOAT VERSION!!!! please use glVertex3v16()
\param x the x component of the vertex
\param y the y component of the vertex
\param z the z component of the vertex */
GL_STATIC_INL void glVertex3f(float x, float y, float z) {
	glVertex3v16(floattov16(x), floattov16(y), floattov16(z));
}

/*! \brief Rotate on an arbitrary axis
\warning FLOAT VERSION!!!! please use glRotatef32i()
\param angle the angle to rotate by
\param x the x component of the axis to rotate on
\param y the y component of the axis to rotate on
\param z the z component of the axis to rotate on */
GL_STATIC_INL void glRotatef32(float angle, int32 x, int32 y, int32 z) {
    glRotatef32i((int)(angle * LUT_SIZE / 360.0), x, y, z);
}

/*! \brief Rotate about an arbitrary axis
\warning FLOAT VERSION!!!! please use glRotatef32i()
\param angle the angle to rotate by
\param x the x component of the axis to rotate on
\param y the y component of the axis to rotate on
\param z the z component of the axis to rotate on */
GL_STATIC_INL void glRotatef(float angle, float x, float y, float z) {
	glRotatef32(angle, floattof32(x), floattof32(y), floattof32(z));
}

/*! \brief specify a color for following vertices
\warning FLOAT VERSION!!!! please use glColor3b()
\param r the red component of the color
\param g the green component of the color
\param b the blue component of the color */
GL_STATIC_INL void glColor3f(float r, float g, float b) {
	glColor3b((uint8)(r*255), (uint8)(g*255), (uint8)(b*255));
}

/*! \brief multiply the current matrix by a scale matrix<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply</A>
\warning FLOAT VERSION!!!! please use glScalev() or glScalef32()
\param x scaling on the x axis
\param y scaling on the y axis
\param z scaling on the z axis */
GL_STATIC_INL void glScalef(float x, float y, float z) {
	MATRIX_SCALE = floattof32(x);
	MATRIX_SCALE = floattof32(y);
	MATRIX_SCALE = floattof32(z);
}

/*! \brief multiply the current matrix by a translation matrix<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dmatrixloadmultiply</A>
\warning FLOAT VERSION!!!! please use glTranslate3f32()
\param x translation on the x axis
\param y translation on the y axis
\param z translation on the z axis */
GL_STATIC_INL void glTranslatef(float x, float y, float z) {
	MATRIX_TRANSLATE = floattof32(x);
	MATRIX_TRANSLATE = floattof32(y);
	MATRIX_TRANSLATE = floattof32(z);
}

/*! \brief the normal to use for following vertices<BR>
<A HREF="http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters">GBATEK http://nocash.emubase.de/gbatek.htm#ds3dpolygonlightparameters</A>
\warning FLOAT VERSION!!!! please use glNormal()
\param x x component of the normal, vector must be normalized
\param y y component of the normal, vector must be normalized
\param z z component of the normal, vector must be normalized */
GL_STATIC_INL void glNormal3f(float x, float y, float z) {
	if(x >= 1 || x <= -1) x *= .95;
	if(y >= 1 || y <= -1) y *= .95;
	if(z >= 1 || z <= -1) z *= .95;
	glNormal(NORMAL_PACK(floattov10(x), floattov10(y), floattov10(z)));
}

/*! \brief Rotates the current modelview matrix by angle degrees about the x axis
\warning FLOAT VERSION!!!! please use glRotateXi()
\param angle The angle to rotate by */
GL_STATIC_INL void glRotateX(float angle) {
	glRotateXi((int)(angle * LUT_SIZE / 360.0));
}

/*! \brief Rotates the current modelview matrix by angle degrees about the y axis
\warning FLOAT VERSION!!!! please use glRotateYi()
\param angle The angle to rotate by */
GL_STATIC_INL void glRotateY(float angle) {
	glRotateYi((int)(angle * LUT_SIZE / 360.0));
}

/*! \brief Rotates the current modelview matrix by angle degrees about the z axis
\warning FLOAT VERSION!!!! please use glRotateZi()
\param angle The angle to rotate by */
GL_STATIC_INL void glRotateZ(float angle) {
	glRotateZi((int)(angle * LUT_SIZE / 360.0));
}

/*! \brief Multiplies the current matrix into ortho graphic mode
\warning FLOAT VERSION!!!! please use glOrthof32()
\param left left vertical clipping plane
\param right right vertical clipping plane
\param bottom bottom vertical clipping plane
\param top top vertical clipping plane
\param zNear near clipping plane
\param zFar far clipping plane */
GL_STATIC_INL void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar) {
	glOrthof32(floattof32(left), floattof32(right), floattof32(bottom), floattof32(top), floattof32(zNear), floattof32(zFar));
}

/*! \brief Places the camera at the specified location and orientation (floating point version)
\warning FLOAT VERSION!!!! please use gluLookAtf32()
\param eyex (eyex, eyey, eyez) Location of the camera.
\param eyey (eyex, eyey, eyez) Location of the camera.
\param eyez (eyex, eyey, eyez) Location of the camera.
\param lookAtx (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param lookAty (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param lookAtz (lookAtx, lookAty, lookAtz) Where the camera is looking.
\param upx <upx, upy, upz> Unit vector describing which direction is up for the camera.
\param upy <upx, upy, upz> Unit vector describing which direction is up for the camera.
\param upz <upx, upy, upz> Unit vector describing which direction is up for the camera. */
GL_STATIC_INL void gluLookAt(	float eyex, float eyey, float eyez,
								float lookAtx, float lookAty, float lookAtz,
								float upx, float upy, float upz) {
	gluLookAtf32(floattof32(eyex), floattof32(eyey), floattof32(eyez),
				 floattof32(lookAtx), floattof32(lookAty), floattof32(lookAtz),
				 floattof32(upx), floattof32(upy), floattof32(upz));
}

/*! \brief Specifies the viewing frustum for the projection matrix (floating point version)
\warning FLOAT VERSION!!!! please use glFrustumf32()
\param left left right top and bottom describe a rectangle located at the near clipping plane
\param right left right top and bottom describe a rectangle located at the near clipping plane
\param top left right top and bottom describe a rectangle located at the near clipping plane
\param bottom left right top and bottom describe a rectangle located at the near clipping plane
\param near Location of a the near clipping plane (parallel to viewing window)
\param far Location of a the far clipping plane (parallel to viewing window) */
GL_STATIC_INL void glFrustum(float left, float right, float bottom, float top, float near, float far) {
	glFrustumf32(floattof32(left), floattof32(right), floattof32(bottom), floattof32(top), floattof32(near), floattof32(far));
}

/*! \brief Utility function which sets up the projection matrix (floating point version)
\warning FLOAT VERSION!!!! please use gluPerspectivef32()
\param fovy Specifies the field of view in degrees  
\param aspect Specifies the aspect ratio of the screen (normally screen width/screen height)
\param zNear Specifies the near clipping plane
\param zFar Specifies the far clipping plane */
GL_STATIC_INL void gluPerspective(float fovy, float aspect, float zNear, float zFar) {
	gluPerspectivef32((int)(fovy * LUT_SIZE / 360.0), floattof32(aspect), floattof32(zNear), floattof32(zFar));    
}




#endif // #ifndef VIDEOGL_ARM9_INCLUDE

