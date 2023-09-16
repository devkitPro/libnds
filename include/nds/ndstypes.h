/*---------------------------------------------------------------------------------

	ndstypes.h -- Common types (and a few useful macros)

	Copyright (C) 2005 - 2008
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)
		Chris Double (doublec)

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
/*! \file ndstypes.h
	\brief Custom types employed by libnds
*/

#ifndef _NDSTYPES_INCLUDE
#define _NDSTYPES_INCLUDE
//---------------------------------------------------------------------------------
#include <calico/types.h>

//---------------------------------------------------------------------------------
// libgba compatible section macros
//---------------------------------------------------------------------------------
#define ITCM_CODE	__attribute__((section(".itcm"), long_call))

#define DTCM_DATA	__attribute__((section(".dtcm")))
#define DTCM_BSS	__attribute__((section(".sbss")))

#define TWL_CODE	__attribute__((section(".twl")))
#define TWL_DATA	__attribute__((section(".twl")))
#define TWL_BSS		__attribute__((section(".twl_bss")))

//! aligns a struct (and other types?) to m, making sure that the size of the struct is a multiple of m.
#define ALIGN(m)	__attribute__((aligned (m)))

//! packs a struct (and other types?) so it won't include padding bytes.
#define PACKED __attribute__ ((packed))
#define packed_struct struct PACKED

//---------------------------------------------------------------------------------
// These are linked to the bin2o macro in the Makefile
//---------------------------------------------------------------------------------
#define GETRAW(name)      (name)
#define GETRAWSIZE(name)  ((int)name##_size)
#define GETRAWEND(name)  ((int)name##_end)

/*!
	\brief returns a number with the nth bit set.
*/
#define BIT(n) (1 << (n))

typedef uint8_t  uint8   MK_DEPRECATED;
typedef uint16_t uint16  MK_DEPRECATED;
typedef uint32_t uint32  MK_DEPRECATED;
typedef uint64_t uint64  MK_DEPRECATED;
typedef int8_t   int8    MK_DEPRECATED;
typedef int16_t  int16   MK_DEPRECATED;
typedef int32_t  int32   MK_DEPRECATED;
typedef int64_t  int64   MK_DEPRECATED;
typedef float    float32 MK_DEPRECATED;
typedef double   float64 MK_DEPRECATED;

typedef volatile uint8_t  vuint8   MK_DEPRECATED;
typedef volatile uint16_t vuint16  MK_DEPRECATED;
typedef volatile uint32_t vuint32  MK_DEPRECATED;
typedef volatile uint64_t vuint64  MK_DEPRECATED;
typedef volatile int8_t   vint8    MK_DEPRECATED;
typedef volatile int16_t  vint16   MK_DEPRECATED;
typedef volatile int32_t  vint32   MK_DEPRECATED;
typedef volatile int64_t  vint64   MK_DEPRECATED;
typedef volatile float32  vfloat32 MK_DEPRECATED;
typedef volatile float64  vfloat64 MK_DEPRECATED;

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

// Handy function pointer typedefs
//! a function pointer that takes no arguments and doesn't return anything.
typedef void (* VoidFn)(void);

typedef void (* IntFn)(void);
typedef void (* fp)(void);

//---------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------
