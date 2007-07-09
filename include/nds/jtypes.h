/*---------------------------------------------------------------------------------
	$Id: jtypes.h,v 1.16 2007-07-09 15:51:11 wntrmute Exp $

	jtypes.h -- Common types (and a few useful macros)

	Copyright (C) 2005
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

	$Log: not supported by cvs2svn $
	Revision 1.15  2007/01/14 11:31:22  wntrmute
	bogus fixed types removed from libnds
	
	Revision 1.14  2006/04/04 23:05:19  wntrmute
	Added pressure reading to touchscreen function
	
	Revision 1.13  2005/10/21 22:43:05  wntrmute
	Removed bogus ASSERT
	
	Revision 1.12  2005/09/03 17:07:25  wntrmute
	added function pointer for interrupts
	
	Revision 1.11  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.10  2005/08/22 08:09:48  wntrmute
	added f32 here

	Revision 1.9  2005/08/03 17:38:05  wntrmute
	added touch struct

	Revision 1.8  2005/07/29 00:55:04  wntrmute
	removed more useless section macros

	Revision 1.7  2005/07/29 00:40:50  wntrmute
	removed useless section macros
	added C++ compatible bool

	Revision 1.6  2005/07/28 03:54:19  dovoto
	Adjusted trig_lut so that cos and sin look tables are visible to all.

	Math.h no includes the trig_lut header.

	added some typedefs to jtypes for compatibility with ndslib.

	Revision 1.5  2005/07/22 17:39:53  wntrmute
	removed useless section macros

---------------------------------------------------------------------------------*/
#ifndef NDS_JTYPES_INCLUDE
#define NDS_JTYPES_INCLUDE
//---------------------------------------------------------------------------------


#define PACKED __attribute__ ((packed))
#define packed_struct struct PACKED

//---------------------------------------------------------------------------------
// libgba compatible section macros
//---------------------------------------------------------------------------------
#define ITCM_CODE	__attribute__((section(".itcm"), long_call))

#define DTCM_DATA	__attribute__((section(".dtcm")))
#define DTCM_BSS	__attribute__((section(".sbss")))
#define ALIGN(m)	__attribute__((aligned (m)))

#define PACKED __attribute__ ((packed))
#define packed_struct struct PACKED

//---------------------------------------------------------------------------------
// These are linked to the bin2o macro in the Makefile
//---------------------------------------------------------------------------------
#define GETRAW(name)      (name)
#define GETRAWSIZE(name)  ((int)name##_size)
#define GETRAWEND(name)  ((int)name##_end)

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define BIT(n) (1 << (n))

// define libnds types in terms of stdint
#include <stdint.h>

typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;

typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;

typedef float		float32;
typedef double		float64;

typedef volatile uint8_t	vuint8;
typedef volatile uint16_t	vuint16;
typedef volatile uint32_t	vuint32;
typedef volatile uint64_t	vuint64;

typedef volatile int8_t		vint8;
typedef volatile int16_t	vint16;
typedef volatile int32_t	vint32;
typedef volatile int64_t	vint64;

typedef volatile float32        vfloat32;
typedef volatile float64        vfloat64;

typedef uint8_t		byte;

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
typedef int64_t		s64;

typedef volatile u8          vu8;
typedef volatile u16         vu16;
typedef volatile u32         vu32;
typedef volatile u64         vu64;

typedef volatile s8           vs8;
typedef volatile s16          vs16;
typedef volatile s32          vs32;
typedef volatile s64          vs64;

typedef struct touchPosition {
	int16	x;
	int16	y;
	int16	px;
	int16	py;
	int16	z1;
	int16	z2;
} touchPosition;


#ifndef __cplusplus
/** C++ compatible bool for C

*/
typedef enum { false, true } bool;
#endif

// Handy function pointer typedefs
typedef void ( * IntFn)(void);
typedef void (* VoidFunctionPointer)(void);
typedef void (* fp)(void);

//---------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------

