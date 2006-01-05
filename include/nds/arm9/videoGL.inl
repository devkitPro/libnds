#ifdef NO_GL_INLINE

#define GL_STATIC_INL

#else

#define GL_STATIC_INL static inline 

#endif

#include <nds/memory.h>

GL_STATIC_INL void glBegin(int mode) { GFX_BEGIN = mode; }

GL_STATIC_INL void glEnd( void) { GFX_END = 0; }

GL_STATIC_INL void glClearColor(uint8 red, uint8 green, uint8 blue) { BG_PALETTE[0] = RGB15(red, green, blue);}

GL_STATIC_INL void glClearDepth(uint16 depth) { GFX_CLEAR_DEPTH = depth; }

GL_STATIC_INL void glColor3b(uint8 red, uint8 green, uint8 blue) { GFX_COLOR = (vuint32)RGB15(red>>3, green>>3, blue>>3); }


GL_STATIC_INL void glColor(rgb color) { GFX_COLOR = (vuint32)color; }

//---------------------------------------------------------------------------------
GL_STATIC_INL void glVertex3v16(v16 x, v16 y, v16 z) {
//---------------------------------------------------------------------------------
	GFX_VERTEX16 = (y << 16) | (x & 0xFFFF);
	GFX_VERTEX16 = ((uint32)(uint16)z);
}


//---------------------------------------------------------------------------------
GL_STATIC_INL void glVertex2v16(int yx, v16 z) {
//---------------------------------------------------------------------------------
	GFX_VERTEX16 = yx;
	GFX_VERTEX16 = (z);
}

GL_STATIC_INL void glTexCoord2t16(t16 u, t16 v) { GFX_TEX_COORD = TEXTURE_PACK(u,v); }

GL_STATIC_INL void glTexCoord1i(uint32 uv) { GFX_TEX_COORD = uv; }

GL_STATIC_INL void glPushMatrix(void) { MATRIX_PUSH = 0; }

GL_STATIC_INL void glPopMatrix(int32 index) { MATRIX_POP = index; }

GL_STATIC_INL void glRestoreMatrix(int32 index) { MATRIX_RESTORE = index; }


GL_STATIC_INL void glStoreMatrix(int32 index) { MATRIX_STORE = index; }

//---------------------------------------------------------------------------------
GL_STATIC_INL void glScalev(GLvector* v) { 
//---------------------------------------------------------------------------------
	MATRIX_SCALE = v->x;
	MATRIX_SCALE = v->y;
	MATRIX_SCALE = v->z;
}


//---------------------------------------------------------------------------------
GL_STATIC_INL void glTranslatev(GLvector* v) {
//---------------------------------------------------------------------------------
	MATRIX_TRANSLATE = v->x;
	MATRIX_TRANSLATE = v->y;
	MATRIX_TRANSLATE = v->z;
}

//---------------------------------------------------------------------------------
GL_STATIC_INL void glTranslate3f32(f32 x, f32 y, f32 z) {
//---------------------------------------------------------------------------------
	MATRIX_TRANSLATE = x;
	MATRIX_TRANSLATE = y;
	MATRIX_TRANSLATE = z;
}

//---------------------------------------------------------------------------------
GL_STATIC_INL void glScalef32(f32 factor) {
//---------------------------------------------------------------------------------
	MATRIX_SCALE = factor;
	MATRIX_SCALE = factor;
	MATRIX_SCALE = factor;
}

//---------------------------------------------------------------------------------
GL_STATIC_INL void glTranslatef32(f32 delta) {
//---------------------------------------------------------------------------------
	MATRIX_TRANSLATE = delta;
	MATRIX_TRANSLATE = delta;
	MATRIX_TRANSLATE = delta;
}

//---------------------------------------------------------------------------------
GL_STATIC_INL void glLight(int id, rgb color, v10 x, v10 y, v10 z) {
//---------------------------------------------------------------------------------
	id = (id & 3) << 30;
	GFX_LIGHT_VECTOR = id | ((z & 0x3FF) << 20) | ((y & 0x3FF) << 10) | (x & 0x3FF);
	GFX_LIGHT_COLOR = id | color;
}

GL_STATIC_INL void glNormal(uint32 normal) { GFX_NORMAL = normal; }

GL_STATIC_INL void glLoadIdentity(void) { MATRIX_IDENTITY = 0; }
GL_STATIC_INL void glIdentity(void) { MATRIX_IDENTITY = 0; }

GL_STATIC_INL void glMatrixMode(int mode) { MATRIX_CONTROL = mode; }

GL_STATIC_INL void glViewPort(uint8 x1, uint8 y1, uint8 x2, uint8 y2) { GFX_VIEWPORT = (x1) + (y1 << 8) + (x2 << 16) + (y2 << 24); }

GL_STATIC_INL void glFlush(void) { GFX_FLUSH = 2; }

//---------------------------------------------------------------------------------
GL_STATIC_INL void glMaterialShinyness(void) {
//---------------------------------------------------------------------------------
	uint32 shiny32[128/4];
	uint8  *shiny8 = (uint8*)shiny32;

	int i;

	for (i = 0; i < 128 * 2; i += 2)
		shiny8[i>>1] = i;

	for (i = 0; i < 128 / 4; i++)
		GFX_SHININESS = shiny32[i];
}

GL_STATIC_INL void glPolyFmt(int alpha) // obviously more to this
{
  GFX_POLY_FORMAT = alpha;
}

//---------------------------------------------------------------------------------
//Display lists have issues that need to resolving.
//There seems to be some issues with list size.
//---------------------------------------------------------------------------------
GL_STATIC_INL void glCallList(u32* list)
//---------------------------------------------------------------------------------
{
	u32 count = *list++;

	while(count--)
		GFX_FIFO = *list++;

	//this works sometimes??
//	DMA_SRC(0) = (uint32)list;
//	DMA_DEST(0) = 0x4000400;
//	DMA_CR(0) = DMA_FIFO | count;
//
//	while(DMA_CR(0) & DMA_BUSY);

}
