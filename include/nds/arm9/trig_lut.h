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

/*!	\file trig_lut.h

	\brief fixed point trig functions
*/


#ifdef __cplusplus
extern "C" {
#endif
/*! \fn cosFixed( int angle )
	\param angle
	
	\brief fixed point cosine
	
	
*/
int cosFixed(int angle);
/*! \fn sinFixed( int angle )
	\param angle
	
	\brief fixed point sine
	
	
*/
int sinFixed(int angle);

/*! \fn tanFixed( int angle )
	\param angle
	
	\brief fixed point tangent
	
	
*/
int tanFixed(int angle);

#ifdef __cplusplus
}
#endif


#endif 
