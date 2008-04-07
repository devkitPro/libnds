/*---------------------------------------------------------------------------------

	math functions

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

---------------------------------------------------------------------------------*/
/*!	\file math.h
	   \brief hardware coprocessor instructions
*/

#ifndef MATH_ARM9_INCLUDE
#define MATH_ARM9_INCLUDE

#include "nds/ndstypes.h"



#define DIV_CR				(*(vuint16*)(0x04000280))
#define DIV_NUMERATOR64		(*(vint64*) (0x04000290))
#define DIV_NUMERATOR32		(*(vint32*) (0x04000290))
#define DIV_DENOMINATOR64	(*(vint64*) (0x04000298))
#define DIV_DENOMINATOR32	(*(vint32*) (0x04000298))
#define DIV_RESULT64		(*(vint64*) (0x040002A0))
#define DIV_RESULT32		(*(vint32*) (0x040002A0))
#define DIV_REMAINDER64		(*(vint64*) (0x040002A8))
#define DIV_REMAINDER32		(*(vint32*) (0x040002A8))

#define SQRT_CR				(*(vuint16*)(0x040002B0))
#define SQRT_PARAM64		(*(vint64*) (0x040002B8))
#define SQRT_RESULT32		(*(vint32*) (0x040002B4))
#define SQRT_PARAM32		(*(vint32*) (0x040002B8))

//  Math coprocessor modes

#define DIV_64_64			2
#define DIV_64_32			1
#define DIV_32_32			0
#define DIV_BUSY			(1<<15)

#define SQRT_64				1
#define SQRT_32				0
#define SQRT_BUSY			(1<<15)

//  Fixed Point versions

/**! \fn static int32 divf32(int32 num, int32 den)
*   \brief Fixed point divide
*   \param num Takes 20.12 numerator and denominator
*   \param den Takes 20.12 numerator and denominator
*   \return returns 20.12 result
*/
static inline int32 divf32(int32 num, int32 den)
{
	DIV_CR = DIV_64_32;

	while(DIV_CR & DIV_BUSY);

	DIV_NUMERATOR64 = ((int64)num) << 12;
	DIV_DENOMINATOR32 = den;

	while(DIV_CR & DIV_BUSY);

	return (DIV_RESULT32);
}


/**! \fn  static int32 mulf32(int32 a, int32 b)
*   \brief Fixed point multiply
*   \param a Takes 20.12 
*   \param b Takes 20.12 
*   \return returns 20.12 result
*/
static inline int32 mulf32(int32 a, int32 b)
{
	long long result = (long long)a*(long long)b;
	return (int32)(result >> 12);
}

/**! \fn static int32 sqrtf32(int32 a)
*   \brief Fixed point sqrt
*   \param a Takes 20.12 
*   \return returns 20.12 result
*/
static inline int32 sqrtf32(int32 a)
{
	SQRT_CR = SQRT_64;

	while(SQRT_CR & SQRT_BUSY);

	SQRT_PARAM64 = ((int64)a) << 12;

	while(SQRT_CR & SQRT_BUSY);

	return SQRT_RESULT32;
}

//  Integer versions

/**! \fn static int32 div32(int32 num, int32 den)
*   \brief integer divide
*   \param num  numerator
*   \param den  denominator
*   \return returns 32 bit integer result
*/
static inline int32 div32(int32 num, int32 den)
{
	DIV_CR = DIV_32_32;

	while(DIV_CR & DIV_BUSY);

	DIV_NUMERATOR32 = num;
	DIV_DENOMINATOR32 = den;

	while(DIV_CR & DIV_BUSY);

	return (DIV_RESULT32);
}

/**! \fn static int32 mod32(int32 num, int32 den)
*   \brief integer modulous
*   \param num  numerator
*   \param den  denominator
*   \return returns 32 bit integer remainder
*/
static inline int32 mod32(int32 num, int32 den)
{
	DIV_CR = DIV_32_32;

	while(DIV_CR & DIV_BUSY);

	DIV_NUMERATOR32 = num;
	DIV_DENOMINATOR32 = den;

	while(DIV_CR & DIV_BUSY);

	return (DIV_REMAINDER32);
}

/**! \fn static int32 div64(int64 num, int32 den)
*   \brief integer 64 bit divide
*   \param num  64 bit numerator
*   \param den  32 bit denominator
*   \return returns 32 bit integer result
*/
static inline int32 div64(int64 num, int32 den)
{
	DIV_CR = DIV_64_32;

	while(DIV_CR & DIV_BUSY);

	DIV_NUMERATOR64 = num;
	DIV_DENOMINATOR32 = den;

	while(DIV_CR & DIV_BUSY);

	return (DIV_RESULT32);
}

/**! \fn static int32 mod64(int64 num, int32 den)
*   \brief integer 64 bit modulous
*   \param num  64 bit numerator
*   \param den  32 bit denominator
*   \return returns 32 bit integer remainder
*/
static inline int32 mod64(int64 num, int32 den)
{
	DIV_CR = DIV_64_32;

	while(DIV_CR & DIV_BUSY);

	DIV_NUMERATOR64 = num;
	DIV_DENOMINATOR32 = den;

	while(DIV_CR & DIV_BUSY);

	return (DIV_REMAINDER32);
}

/**! \fn static int32 sqrt32(int a)
*   \brief integer sqrt
*   \param a 32 bit integer argument
*   \return returns 32 bit integer result
*/
static inline int32 sqrt32(int a)
{
	SQRT_CR = SQRT_32;

	while(SQRT_CR & SQRT_BUSY);

	SQRT_PARAM32 = a;

	while(SQRT_CR & SQRT_BUSY);

	return SQRT_RESULT32;
}


/**! \fn static void crossf32(int32 *a, int32 *b, int32 *result)
*   \brief 1.19.12 fixed point cross product function result = AxB
*   \param a pointer to fixed 3x3 matrix 
*   \param b pointer to fixed 3x3 matrix 
*   \param result pointer to fixed 3x3 matrix 
* Cross product
* x = Ay * Bz - By * Az
* y = Az * Bx - Bz * Ax
* z = Ax * By - Bx * Ay
*/
static inline void crossf32(int32 *a, int32 *b, int32 *result)
{
	result[0] = mulf32(a[1], b[2]) - mulf32(b[1], a[2]);
	result[1] = mulf32(a[2], b[0]) - mulf32(b[2], a[0]);
	result[2] = mulf32(a[0], b[1]) - mulf32(b[0], a[1]);
}


/**! \fn static int32 dotf32(int32 *a, int32 *b)
*   \brief 1.19.12 fixed point dot product function result = A dot B
*   \param a pointer to fixed 3x3 matrix 
*   \param b pointer to fixed 3x3 matrix 
*   \return 32 bit integer result 
* Dot Product
* result = Ax * Bx + Ay * By + Az * Bz
*/
static inline int32 dotf32(int32 *a, int32 *b)
{
	return mulf32(a[0], b[0]) + mulf32(a[1], b[1]) + mulf32(a[2], b[2]);
}



/**! \fn static void normalizef32(int32* a)
*   \brief 1.19.12 fixed point normalize function A = A  / |A|
*   \param a pointer to fixed 3x3 matrix 
* Normalize
* Ax = Ax / mag
* Ay = Ay / mag
* Az = Az / mag
*/
static inline void normalizef32(int32* a)
{
	// magnitude = sqrt ( Ax^2 + Ay^2 + Az^2 )
	int32 magnitude = sqrtf32( mulf32(a[0], a[0]) + mulf32(a[1], a[1]) + mulf32(a[2], a[2]) );

	a[0] = divf32(a[0], magnitude);
	a[1] = divf32(a[1], magnitude);
	a[2] = divf32(a[2], magnitude);
}
#endif

