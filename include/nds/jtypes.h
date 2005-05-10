//////////////////////////////////////////////////////////////////////
//
// jtypes.h -- Common types (and a few useful macros)
//
// version 0.1, February 14, 2005
//
//  Copyright (C) 2005 Michael Noland (joat) and Jason Rogers (dovoto)
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any
//  damages arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any
//  purpose, including commercial applications, and to alter it and
//  redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you
//     must not claim that you wrote the original software. If you use
//     this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and
//     must not be misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source
//     distribution.
//
// Changelog:
//   0.1: First version
//	 0.2: Added alternate typedefines (u8 u16 ect...)
//
//////////////////////////////////////////////////////////////////////

#ifndef NDS_JTYPES_INCLUDE
#define NDS_JTYPES_INCLUDE

//////////////////////////////////////////////////////////////////////

#define CODE_IN_IWRAM __attribute__ ((section (".iwram"), long_call))
#define VAR_IN_IWRAM __attribute__ ((section (".iwram")))
#define CODE_IN_EXRAM __attribute__ ((section (".ewram"), long_call))
#define VAR_IN_EXRAM __attribute__ ((section (".ewram")))
#define PACKED __attribute__ ((packed))
#define packed_struct struct PACKED

// libgba compatible section macros
#define IWRAM_CODE	__attribute__((section(".iwram"), long_call))
#define ITCM_CODE	__attribute__((section(".itcm"), long_call))
#define EWRAM_CODE	__attribute__((section(".ewram"), long_call))

#define IWRAM_DATA	__attribute__((section(".iwram")))
#define DTCM_DATA	__attribute__((section(".dtcm")))
#define EWRAM_DATA	__attribute__((section(".ewram")))
#define EWRAM_BSS	__attribute__((section(".sbss")))
#define ALIGN(m)	__attribute__((aligned (m)))

// These are linked to the bin2o macro in the Makefile
#define GETRAW(name)      (name)
#define GETRAWSIZE(name)  ((int)name##_end - (int)name)

#define ASSERT(arg) 

#define BIT(n) (1 << (n))
//////////////////////////////////////////////////////////////////////

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


/////////////////alt defines
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


//////////////////////////////////////////////////////////////////////

typedef void (* VoidFunctionPointer)(void);
typedef void (* fp)(void);


//////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////
