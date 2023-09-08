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

#include "../ndstypes.h"
#include <calico/arm/common.h>
#include <calico/nds/mm_env.h>

/** \file
	\brief functions to handle hardware exceptions.
*/

#define EXCEPTION_VECTOR	(*(VoidFn *)MM_ENV_EXCPT_VECTOR)

#ifdef __cplusplus
extern "C" {
#endif

//! Exception context structure.
typedef struct ExcptContext {
	u32 r[16];
	u32 cpsr;
	u32 spsr;
	u32 cp15cr;
	u32 excpt_cpsr;
} ExcptContext;

//! Exception handler function type.
typedef void (* ExcptHandler)(ExcptContext* ctx, unsigned flags);

//! sets a custom hardware exception handler.
static inline void setExceptionHandler(ExcptHandler handler)
{
	extern ExcptHandler g_excptHandler;
	void __libnds_excpt(void);

	EXCEPTION_VECTOR = __libnds_excpt;
	g_excptHandler = handler;
}

//! sets the default hardware exception handler.
void defaultExceptionHandler(void);

//! prints the given exception context.
void guruMeditationDump(ExcptContext* ctx, unsigned flags);

//! decodes the fault address from a given opcode.
u32 getExceptionAddress(const ExcptContext* ctx, u32 opcodeAddress);

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------------
#endif // _exceptions_h_
//---------------------------------------------------------------------------------
