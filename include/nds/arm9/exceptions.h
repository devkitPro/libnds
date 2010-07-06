/*---------------------------------------------------------------------------------

  Copyright (C) 2005
  	Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

---------------------------------------------------------------------------------*/
#ifndef _exceptions_h_
#define _exceptions_h_
//---------------------------------------------------------------------------------


/** \file
	\brief functions to handle hardware exceptions.
*/

#define EXCEPTION_VECTOR	(*(VoidFn *)(0x2FFFD9C))

#ifdef __cplusplus
extern "C" {
#endif


extern VoidFn exceptionC[];//shouldn't this be a pointer instead of an array?
extern u32 exceptionStack;

//! an array with a copy of all the registers of when the exception occured.
extern s32 exceptionRegisters[];


void enterException(void);

//! sets a custom hardware exception handler.
void setExceptionHandler(VoidFn handler);

//! sets the default hardware exception handler.
void defaultExceptionHandler();

//! returns the cpu status register.
u32 getCPSR();

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------------
#endif // _exceptions_h_
//---------------------------------------------------------------------------------
