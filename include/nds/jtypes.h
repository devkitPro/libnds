/*---------------------------------------------------------------------------------
	$Id: jtypes.h,v 1.8 2005-07-29 00:55:04 wntrmute Exp $

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

#define ASSERT(arg) 

#define BIT(n) (1 << (n))

typedef unsigned char           uint8;
typedef unsigned short int      uint16;
typedef unsigned int            uint32;
typedef unsigned long long int  uint64;

typedef signed char             int8;
typedef signed short int        int16;
typedef signed int              int32;
typedef signed long long int    int64;

typedef float                   float32;
typedef double                  float64;

typedef volatile uint8          vuint8;
typedef volatile uint16         vuint16;
typedef volatile uint32         vuint32;
typedef volatile uint64         vuint64;

typedef volatile int8           vint8;
typedef volatile int16          vint16;
typedef volatile int32          vint32;
typedef volatile int64          vint64;

typedef volatile float32        vfloat32;
typedef volatile float64        vfloat64;

typedef uint8                   byte;

typedef int32                   fixed;
typedef int64                   dfixed;

typedef volatile int32          vfixed;


typedef unsigned char           u8;
typedef unsigned short int      u16;
typedef unsigned int            u32;
typedef unsigned long long int  u64;

typedef signed char             s8;
typedef signed short int        s16;
typedef signed int              s32;
typedef signed long long int    s64;

typedef volatile u8          vu8;
typedef volatile u16         vu16;
typedef volatile u32         vu32;
typedef volatile u64         vu64;

typedef volatile s8           vs8;
typedef volatile s16          vs16;
typedef volatile s32          vs32;
typedef volatile s64          vs64;

#ifndef __cplusplus
/** C++ compatible bool for C

*/
typedef enum { false, true } bool;
#endif


typedef void (* VoidFunctionPointer)(void);
typedef void (* fp)(void);



//---------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------

