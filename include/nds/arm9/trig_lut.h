/*---------------------------------------------------------------------------------

	Trig_lut.h provides access to external precompiled trig look up tables

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
#ifndef TRIG_LUT_H
#define TRIG_LUT_H

#include <nds/ndstypes.h>

/*!	\file trig_lut.h

	\brief fixed point trig functions.  Angle is computed useing a 512 degree circle
 */




#ifdef __cplusplus
extern "C" {
#endif

#define DEGREES_IN_CIRCLE (1 << 15)

#define fixedToInt(n, bits) ((int)((n)>>(bits)))
#define intToFixed(n, bits) ((int)((n)<<(bits)))

#define floatToFixed(n, bits) ((int)((n) * (float)(1<<(bits))))
#define fixedToFloat(n, bits) (((float)(n)) / (float)(1<<(bits)))

#define floorFixed(n, bits) ((int)((n) & ~(((1 << (bits)) - 1))))

#define degreesToAngle(degrees) ((degrees) * DEGREES_IN_CIRCLE / 360)
#define angleToDegrees(angle)   ((angle) * 360 / DEGREES_IN_CIRCLE)

//unsigned 16 bit fixed point with 15 bits fraction
typedef u16 fixed_u16_15;

//signed 16 bit fixed point with 12 bits fraction
typedef s16 fixed_s16_12;

//signed 32 bit fixed point with 15 bits fraction
typedef s32 fixed_s32_15;

//unsigned 32 bit fixed point with 16 bits fraction
typedef s32 fixed_s32_16;

/*! \fn fixed_s16_12 cosLerp( s16 angle )
	\param angle (0 to 65535)
	
	\brief fixed point cosine
	\return fixed_s16_12 fixed point number with the range [-1, 1]
*/

fixed_s16_12 cosLerp(s16 angle);
/*! \fn fixed_s16_12 sinLerp( s16 angle )
	\param angle
	
	\brief fixed point sine
	\return fixed_s16_12 fixed point number with the range [-1, 1]
	
*/
fixed_s16_12 sinLerp(s16 angle);

/*! \fn fixed_s16_12 tanLerp( s16 angle )
	\param angle
	
	\brief fixed point tangent
	\return fixed_s32_15 fixed point number with the range [-81.483, 524287.999]
	
*/
fixed_s32_15 tanLerp(s16 angle);

/*! \fn acosLerp( int par )
	\param parameter
	
	\brief fixed point arccos 
	\return f32 fixed point number with the range [0, 256]
	
*/
short acosLerp(int par);
/*! \fn asinLerp( int par )
	\param parameter
	
	\brief fixed point arcsin
	\return f32 fixed point number with the range [-128, 128]
	
*/
short asinLerp(int par);

/*! \fn atanLerp( int par )
	\param parameter
	
	\brief fixed point arctan
	\return f32 fixed point number with the range [-128, 128]
	
*/
short atanLerp(int par);

#ifdef __cplusplus
}
#endif

#endif
