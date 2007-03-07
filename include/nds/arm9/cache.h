/*---------------------------------------------------------------------------------
	$Id: cache.h,v 1.6 2007-03-07 05:51:28 gabebear Exp $

	key input code -- provides slightly higher level input forming

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
	Revision 1.5  2006/07/04 01:45:04  wntrmute
	add instruction cache functions
	
	Revision 1.4  2005/11/14 11:51:21  wntrmute
	begin doxygenation

	Revision 1.3  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.2  2005/07/25 16:10:43  wntrmute
	updated file header


---------------------------------------------------------------------------------*/
/*! \file cache.h
\brief ARM9 cache control functions.

*/
#ifndef	_cache_h_
#define _cache_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nds/jtypes.h"


/*! \fn IC_InvalidateAll()
	\brief invalidate entire instruction cache.
*/
void	IC_InvalidateAll();

	
/*! \fn IC_InvalidateRange(const void *base, u32 size)
	\brief invalidate the instruction cache for a range of addresses.
	\param base base address of the region to invalidate
	\param size size of the region to invalidate.
*/
void	IC_InvalidateRange(const void *base, u32 size);


/*! \fn DC_FlushAll()
	\brief flush the entire data cache to memory.
*/
void	DC_FlushAll();


/*! \fn DC_FlushRange(const void *base, u32 size)
	\brief flush the data cache for a range of addresses to memory.
	\param base base address of the region to flush.
	\param size size of the region to flush.
*/
void	DC_FlushRange(const void *base, u32 size);


/*! \fn DC_InvalidateAll()
	\brief invalidate the entire data cache.
*/
void	DC_InvalidateAll();


/*! \fn DC_InvalidateRange(const void *base, u32 size)
	\brief invalidate the data cache for a range of addresses.
	\param base base address of the region to invalidate
	\param size size of the region to invalidate.
*/
void	DC_InvalidateRange(const void *base, u32 size);

#ifdef __cplusplus
}
#endif

#endif
