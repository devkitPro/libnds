/*---------------------------------------------------------------------------------
	$Id: ndsmotion.h,v 1.1 2007-01-09 06:11:54 dovoto Exp $

	Microphone control for the ARM7

	Copyright (C) 2007
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

---------------------------------------------------------------------------------*/

/*! \file motion.h 
\brief interface code for the ds motion card 

*/
#ifndef NDS_MOTION_INCLUDE
#define NDS_MOTION_INCLUDE
//---------------------------------------------------------------------------------




#ifdef __cplusplus
extern "C" {
#endif

/*! \fn void motion_enable()
\brief  turn on the accelerometer
*/
void motion_enable();

/*! \fn void motion_enable()
\brief  read the X acceleration
*/
signed int motion_read_x();

/*! \fn signed int motion_read_y()
\brief  read the Y acceleration
*/
signed int motion_read_y();

/*! \fn signed int motion_read_z()
\brief  read the Z acceleration
*/
signed int motion_read_z();

/*! \fn signed int motion_read_gyro()
\brief  read the Z rotational speed
*/
signed int motion_read_gyro();

/*! \fn int motion_is_inserted()
\brief check whether a DS Motion Card is plugged in
*/
int motion_is_inserted();

/*! \fn int motion_acceleration(int accel)
\brief converts raw acceleration value to mili G (where g is 9.8 m/s*s)
*/
int motion_acceleration(int accel);

/*! \fn int motion_rotation(int rotation)
\brief converts raw rotation value to degrees per second
*/
int motion_rotation(int rotation);

#ifdef __cplusplus
}
#endif

#endif
