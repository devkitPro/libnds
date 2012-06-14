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
	\brief hardware coprocessor math instructions.
*/

#ifndef MATH_ARM9_INCLUDE
#define MATH_ARM9_INCLUDE

#include "nds/ndstypes.h"


#define REG_DIVCNT			(*(vu16*)(0x04000280))
#define REG_DIV_NUMER		(*(vs64*) (0x04000290))
#define REG_DIV_NUMER_L		(*(vs32*) (0x04000290))
#define REG_DIV_NUMER_H		(*(vs32*) (0x04000294))
#define REG_DIV_DENOM		(*(vs64*) (0x04000298))
#define REG_DIV_DENOM_L		(*(vs32*) (0x04000298))
#define REG_DIV_DENOM_H		(*(vs32*) (0x0400029C))
#define REG_DIV_RESULT		(*(vs64*) (0x040002A0))
#define REG_DIV_RESULT_L	(*(vs32*) (0x040002A0))
#define REG_DIV_RESULT_H	(*(vs32*) (0x040002A4))
#define REG_DIVREM_RESULT	(*(vs64*) (0x040002A8))
#define REG_DIVREM_RESULT_L	(*(vs32*) (0x040002A8))
#define REG_DIVREM_RESULT_H	(*(vs32*) (0x040002AC))

#define REG_SQRTCNT			(*(vu16*)(0x040002B0))
#define REG_SQRT_PARAM		(*(vs64*) (0x040002B8))
#define REG_SQRT_PARAM_L	(*(vs32*) (0x040002B8))
#define REG_SQRT_PARAM_H	(*(vs32*) (0x040002BC))
#define REG_SQRT_RESULT		(*(vu32*) (0x040002B4))

//  Math coprocessor modes

#define DIV_64_64			2
#define DIV_64_32			1
#define DIV_32_32			0
#define DIV_BUSY			(1<<15)

#define SQRT_64				1
#define SQRT_32				0
#define SQRT_BUSY			(1<<15)

//  Fixed point conversion macros

#define inttof32(n)          ((n) << 12) /*!< \brief convert int to f32 */
#define f32toint(n)          ((n) >> 12) /*!< \brief convert f32 to int */
#define floattof32(n)        ((int)((n) * (1 << 12))) /*!< \brief convert float to f32 */
#define f32tofloat(n)        (((float)(n)) / (float)(1<<12)) /*!< \brief convert f32 to float */

//  Fixed Point versions

static inline
/**
*    \brief Fixed point divide
*    \param num Takes 20.12 numerator.
*    \param den Takes 20.12 denominator.
*    \return returns 20.12 result.
*/
int32 divf32(int32 num, int32 den)
{
	REG_DIVCNT = DIV_64_32;

	while(REG_DIVCNT & DIV_BUSY);

	REG_DIV_NUMER = ((int64)num) << 12;
	REG_DIV_DENOM_L = den;

	while(REG_DIVCNT & DIV_BUSY);

	return (REG_DIV_RESULT_L);
}

static inline
/**
*   \brief Fixed point multiply
*   \param a Takes 20.12
*   \param b Takes 20.12
*   \return returns 20.12 result
*/
int32 mulf32(int32 a, int32 b)
{
	long long result = (long long)a * (long long)b;
	return (int32)(result >> 12);
}

static inline
/**
*   \brief Fixed point sqrt
*   \param a Takes 20.12
*   \return returns 20.12 result
*/
int32 sqrtf32(int32 a)
{
	REG_SQRTCNT = SQRT_64;

	while(REG_SQRTCNT & SQRT_BUSY);

	REG_SQRT_PARAM = ((int64)a) << 12;

	while(REG_SQRTCNT & SQRT_BUSY);

	return REG_SQRT_RESULT;
}

//  Integer versions

static inline
/**
*   \brief integer divide
*   \param num  numerator
*   \param den  denominator
*   \return returns 32 bit integer result
*/
int32 div32(int32 num, int32 den)
{
	REG_DIVCNT = DIV_32_32;

	while(REG_DIVCNT & DIV_BUSY);

	REG_DIV_NUMER_L = num;
	REG_DIV_DENOM_L = den;

	while(REG_DIVCNT & DIV_BUSY);

	return (REG_DIV_RESULT_L);
}

static inline
/**
*   \brief integer modulous
*   \param num  numerator
*   \param den  denominator
*   \return returns 32 bit integer remainder
*/
int32 mod32(int32 num, int32 den)
{
	REG_DIVCNT = DIV_32_32;

	while(REG_DIVCNT & DIV_BUSY);

	REG_DIV_NUMER_L = num;
	REG_DIV_DENOM_L = den;

	while(REG_DIVCNT & DIV_BUSY);

	return (REG_DIVREM_RESULT_L);
}

static inline
/**
*   \brief integer 64 bit divide
*   \param num  64 bit numerator
*   \param den  32 bit denominator
*   \return returns 32 bit integer result
*/
int32 div64(int64 num, int32 den)
{
	REG_DIVCNT = DIV_64_32;

	while(REG_DIVCNT & DIV_BUSY);

	REG_DIV_NUMER = num;
	REG_DIV_DENOM_L = den;

	while(REG_DIVCNT & DIV_BUSY);

	return (REG_DIV_RESULT_L);
}

static inline
/**
*   \brief integer 64 bit modulous
*   \param num  64 bit numerator
*   \param den  32 bit denominator
*   \return returns 32 bit integer remainder
*/
int32 mod64(int64 num, int32 den)
{
	REG_DIVCNT = DIV_64_32;

	while(REG_DIVCNT & DIV_BUSY);

	REG_DIV_NUMER = num;
	REG_DIV_DENOM_L = den;

	while(REG_DIVCNT & DIV_BUSY);

	return (REG_DIVREM_RESULT_L);
}

static inline
/**
*   \brief integer sqrt
*   \param a 32 bit integer argument
*   \return returns 32 bit integer result
*/
u32 sqrt32(int a)
{
	REG_SQRTCNT = SQRT_32;

	while(REG_SQRTCNT & SQRT_BUSY);

	REG_SQRT_PARAM_L = a;

	while(REG_SQRTCNT & SQRT_BUSY);

	return REG_SQRT_RESULT;
}

static inline
/**
*   \brief integer sqrt
*   \param a 64 bit integer argument
*   \return returns 32 bit integer result
*/
u32 sqrt64(long long a)
{
	REG_SQRTCNT = SQRT_64;

	while(REG_SQRTCNT & SQRT_BUSY);

	REG_SQRT_PARAM = a;

	while(REG_SQRTCNT & SQRT_BUSY);

	return REG_SQRT_RESULT;
}

static inline
/**
*   \brief 1.19.12 fixed point cross product function result = AxB
*   \param a pointer to fixed 3x3 matrix
*   \param b pointer to fixed 3x3 matrix
*   \param result pointer to fixed 3x3 matrix
* Cross product
* x = Ay * Bz - By * Az
* y = Az * Bx - Bz * Ax
* z = Ax * By - Bx * Ay
*/
void crossf32(int32 *a, int32 *b, int32 *result)
{
	result[0] = mulf32(a[1], b[2]) - mulf32(b[1], a[2]);
	result[1] = mulf32(a[2], b[0]) - mulf32(b[2], a[0]);
	result[2] = mulf32(a[0], b[1]) - mulf32(b[0], a[1]);
}


static inline
/**
*   \brief 1.19.12 fixed point dot product function result = A dot B
*   \param a pointer to fixed 3x3 matrix
*   \param b pointer to fixed 3x3 matrix
*   \return 32 bit integer result
* Dot Product
* result = Ax * Bx + Ay * By + Az * Bz
*/
int32 dotf32(int32 *a, int32 *b)
{
	return mulf32(a[0], b[0]) + mulf32(a[1], b[1]) + mulf32(a[2], b[2]);
}



static inline
/**
*   \brief 1.19.12 fixed point normalize function A = A  / |A|
*   \param a pointer to fixed 3x3 matrix
* Normalize
* Ax = Ax / mag
* Ay = Ay / mag
* Az = Az / mag
*/
void normalizef32(int32* a)
{
	// magnitude = sqrt ( Ax^2 + Ay^2 + Az^2 )
	int32 magnitude = sqrtf32( mulf32(a[0], a[0]) + mulf32(a[1], a[1]) + mulf32(a[2], a[2]) );

	a[0] = divf32(a[0], magnitude);
	a[1] = divf32(a[1], magnitude);
	a[2] = divf32(a[2], magnitude);
}


#endif
