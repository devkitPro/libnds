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
// define libnds types in terms of stdint
#include <stdint.h>
#include <stdbool.h>

//---------------------------------------------------------------------------------
// libgba compatible section macros
//---------------------------------------------------------------------------------
#define ITCM_CODE	__attribute__((section(".itcm"), long_call))

#define DTCM_DATA	__attribute__((section(".dtcm")))
#define DTCM_BSS	__attribute__((section(".sbss")))

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

//! 8 bit unsigned integer.
typedef uint8_t		uint8;
//! 16 bit unsigned integer.
typedef uint16_t	uint16;
//! 32 bit unsigned integer.
typedef uint32_t	uint32;
//! 64 bit unsigned integer.
typedef uint64_t	uint64;

//! 8 bit signed integer.
typedef int8_t		int8;
//! 16 bit signed integer.
typedef int16_t		int16;
//! 32 bit signed integer.
typedef int32_t		int32;
//! 64 bit signed integer.
typedef int64_t		int64;

//! 32 bit signed floating point number.
typedef float		float32;
//! 64 bit signed floating point number.
typedef double		float64;

//! 8 bit volatile unsigned integer.
typedef volatile uint8_t	vuint8;
//! 16 bit volatile unsigned integer.
typedef volatile uint16_t	vuint16;
//! 32 bit volatile unsigned integer.
typedef volatile uint32_t	vuint32;
//! 64 bit volatile unsigned integer.
typedef volatile uint64_t	vuint64;

//! 8 bit volatile signed integer.
typedef volatile int8_t		vint8;
//! 16 bit volatile signed integer.
typedef volatile int16_t	vint16;
//! 32 bit volatile signed integer.
typedef volatile int32_t	vint32;
//! 64 bit volatile signed integer.
typedef volatile int64_t	vint64;

//! 32 bit volatile signed floating point number.
typedef volatile float32        vfloat32;
//! 64 bit volatile signed floating point number.
typedef volatile float64        vfloat64;

//! 8 bit unsigned integer.
typedef uint8_t		byte;

//! 8 bit unsigned integer.
typedef uint8_t		u8;
//! 16 bit unsigned integer.
typedef uint16_t	u16;
//! 32 bit unsigned integer.
typedef uint32_t	u32;
//! 64 bit unsigned integer.
typedef uint64_t	u64;

//! 8 bit signed integer.
typedef int8_t		s8;
//! 16 bit signed integer.
typedef int16_t		s16;
//! 32 bit signed integer.
typedef int32_t		s32;
//! 64 bit signed integer.
typedef int64_t		s64;

//! 8 bit volatile unsigned integer.
typedef volatile u8          vu8;
//! 16 bit volatile unsigned integer.
typedef volatile u16         vu16;
//! 32 bit volatile unsigned integer.
typedef volatile u32         vu32;
//! 64 bit volatile unsigned integer.
typedef volatile u64         vu64;

//! 8 bit volatile signed integer.
typedef volatile s8           vs8;
//! 16 bit volatile signed integer.
typedef volatile s16          vs16;
//! 32 bit volatile signed integer.
typedef volatile s32          vs32;
//! 64 bit volatile signed integer.
typedef volatile s64          vs64;



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
