//////////////////////////////////////////////////////////////////////
//
// serial.h -- Serial communications defines and functions
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
//
//////////////////////////////////////////////////////////////////////

#ifndef SERIAL_ARM7_INCLUDE
#define SERIAL_ARM7_INCLUDE

#ifndef ARM7
#error Serial header is for ARM7 only
#endif

//////////////////////////////////////////////////////////////////////
// Networking ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#define R_CR            (*(vuint16*)0x04000134)
#define XKEYS           (*(vuint16*)0x04000136)
#define RTC_CR          (*(vuint16*)0x04000138)
#define RTC_CR8         (*(vuint8*)0x04000138)

#define SIO_CR          (*(vuint16*)0x04000128)

#define SIO_DATA8       (*(vuint8*)0x0400012A)
#define SIO_DATA32      (*(vuint32*)0x04000120)

#define SIO_DATA32      (*(vuint32*)0x04000120)
#define SIO_MULTI_0     (*(vuint16*)0x04000120)
#define SIO_MULTI_1     (*(vuint16*)0x04000122)
#define SIO_MULTI_2     (*(vuint16*)0x04000124)
#define SIO_MULTI_3     (*(vuint16*)0x04000126)
#define SIO_MULTI_SEND  (*(vuint16*)0x0400012A)

//////////////////////////////////////////////////////////////////////

#define SERIAL_CR      (*(vuint16*)0x040001C0)
#define SERIAL_CR32    (*(vuint32*)0x040001C0)
#define SERIAL_DATA    (*(vuint16*)0x040001C2)
#define SERIAL_DATA8   (*(vuint8*)0x040001C2)

#define SERIAL_ENABLE   0x8000
#define SERIAL_BUSY     0x80

//////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////
