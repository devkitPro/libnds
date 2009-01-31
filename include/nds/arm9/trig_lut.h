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

	\brief fixed point trig functions.  Angle can be in the range of -32768 to
	32767.  There are 32768 degrees in the unit circle used by nds.  To convert 
	between standard degrees (360 per circle):

	angle = degreesToAngle(angleInDegrees);

	or 

	angle = angleInDegrees * 32768 / 360;

	This unit of measure is sometimes refered to as a binary radian (brad) or binary
	degree.  It allows for more precise representation of angle and faster calculation
	as the DS has no floating point processor.
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


/*! \fn s16 cosLerp( s16 angle )
	\param angle (-32768 to 32767)
	
	\brief fixed point cosine
	\return 4.12 fixed point number with the range [-1, 1]
*/

s16 cosLerp(s16 angle);
/*! \fn s16 sinLerp( s16 angle )
	\param angle (-32768 to 32767)
	
	\brief fixed point sine
	\return 4.12 fixed point number with the range [-1, 1]
	
*/
s16 sinLerp(s16 angle);

/*! \fn s32 tanLerp( s16 angle )
	\param angle (-32768 to 32767)

	\brief fixed point tangent
	\return 20.12 fixed point number with the range [-81.483, 524287.999]
	
*/
s32 tanLerp(s16 angle);

/*! \fn s16 acosLerp( s16 par )
	\param par parameter
	
	\brief fixed point arccos 
	\return s16 angle (-32768 to 32767)
	
*/
s16 acosLerp(s16 par);
/*! \fn s16 asinLerp( s16 par )
	\param par parameter
	
	\brief fixed point arcsin
	\return s16 angle (-32768 to 32767)
	
*/
s16 asinLerp(s16 par);



#ifdef __cplusplus
}
#endif

#endif
