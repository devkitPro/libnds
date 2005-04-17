//////////////////////////////////////////////////////////////////////
//
// CP15.S -- CP15 control for the ARM9
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

.arm
.align 4

//////////////////////////////////////////////////////////////////////
// See DDI0155A page 2-6
//////////////////////////////////////////////////////////////////////

.global CP15_GetID
CP15_GetID:
		mrc	p15, 0, r0, c0, c0, 0
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155A page 2-7
//////////////////////////////////////////////////////////////////////

.global CP15_GetCacheType
CP15_GetCacheType:
		mrc	p15, 0, r0, c0, c0, 1
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155A page 2-9
//////////////////////////////////////////////////////////////////////

.global CP15_GetTCMSize
CP15_GetTCMSize:
		mrc	p15, 0, r0, c0, c0, 2
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155A page 2-11
//////////////////////////////////////////////////////////////////////

.global CP15_GetControl
.global CP15_SetControl

CP15_GetControl:
		mrc	p15, 0, r0, c1, c0, 0
		bx	lr

CP15_SetControl:
		mcr	p15, 0, r0, c1, c0, 0
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155 page 2-15
//////////////////////////////////////////////////////////////////////

.global CP15_GetDataCachable
.global CP15_GetInstructionCachable
.global CP15_SetDataCachable
.global CP15_SetInstructionCachable

CP15_GetDataCachable:
		mrc	p15, 0, r0, c2, c0, 0
		bx	lr

CP15_GetInstructionCachable:
		mrc	p15, 0, r0, c2, c0, 1
		bx	lr

CP15_SetDataCachable:
		mcr	p15, 0, r0, c2, c0, 0
		bx	lr

CP15_SetInstructionCachable:
		mcr	p15, 0, r0, c2, c0, 1
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155 page 2-15
//////////////////////////////////////////////////////////////////////

.global CP15_GetDataBufferable
.global CP15_SetDataBufferable

CP15_GetDataBufferable:
		mrc	p15, 0, r0, c3, c0, 0
		bx	lr

CP15_SetDataBufferable:
		mcr	p15, 0, r0, c3, c0, 0
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155 page 2-16
//////////////////////////////////////////////////////////////////////

.global CP15_GetDataPermissions
.global CP15_GetInstructionPermissions
.global CP15_SetDataPermissions
.global CP15_SetInstructionPermissions

CP15_GetDataPermissions:
		mrc	p15, 0, r0, c5, c0, 2
		bx	lr

CP15_GetInstructionPermissions:
		mrc	p15, 0, r0, c5, c0, 3
		bx	lr

CP15_SetDataPermissions:
		mcr	p15, 0, r0, c5, c0, 2
		bx	lr

CP15_SetInstructionPermissions:
		mcr	p15, 0, r0, c5, c0, 3
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155 page 2-19
//////////////////////////////////////////////////////////////////////

.global	CP15_GetRegion0
.global	CP15_GetRegion1
.global	CP15_GetRegion2
.global	CP15_GetRegion3
.global	CP15_GetRegion4
.global	CP15_GetRegion5
.global	CP15_GetRegion6
.global	CP15_GetRegion7
.global	CP15_SetRegion0
.global	CP15_SetRegion1
.global	CP15_SetRegion2
.global	CP15_SetRegion3
.global	CP15_SetRegion4
.global	CP15_SetRegion5
.global	CP15_SetRegion6
.global	CP15_SetRegion7

CP15_GetRegion0:
		mrc	p15, 0, r0, c6, c0, 0
		bx	lr
CP15_GetRegion1:
		mrc	p15, 0, r0, c6, c1, 0
		bx	lr
CP15_GetRegion2:
		mrc	p15, 0, r0, c6, c2, 0
		bx	lr
CP15_GetRegion3:
		mrc	p15, 0, r0, c6, c3, 0
		bx	lr
CP15_GetRegion4:
		mrc	p15, 0, r0, c6, c4, 0
		bx	lr
CP15_GetRegion5:
		mrc	p15, 0, r0, c6, c5, 0
		bx	lr
CP15_GetRegion6:
		mrc	p15, 0, r0, c6, c6, 0
		bx	lr
CP15_GetRegion7:
		mrc	p15, 0, r0, c6, c7, 0
		bx	lr


CP15_SetRegion0:
		mcr	p15, 0, r0, c6, c0, 0
		bx	lr
CP15_SetRegion1:
		mcr	p15, 0, r0, c6, c1, 0
		bx	lr
CP15_SetRegion2:
		mcr	p15, 0, r0, c6, c2, 0
		bx	lr
CP15_SetRegion3:
		mcr	p15, 0, r0, c6, c3, 0
		bx	lr
CP15_SetRegion4:
		mcr	p15, 0, r0, c6, c4, 0
		bx	lr
CP15_SetRegion5:
		mcr	p15, 0, r0, c6, c5, 0
		bx	lr
CP15_SetRegion6:
		mcr	p15, 0, r0, c6, c6, 0
		bx	lr
CP15_SetRegion7:
		mcr	p15, 0, r0, c6, c7, 0
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155 page 2-19
//////////////////////////////////////////////////////////////////////

.global CP15_FlushICache
.global CP15_FlushICacheEntry
.global CP15_PrefetchICacheLine
.global CP15_FlushDCache
.global CP15_FlushDCacheEntry
.global CP15_CleanDCacheEntry
.global CP15_CleanAndFlushDCacheEntry
.global CP15_CleanDCacheEntryByIndex
.global CP15_CleanAndFlushDCacheEntryByIndex

CP15_FlushICache:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c5, 0
		bx	lr

CP15_FlushICacheEntry:
		mcr	p15, 0, r0, c7, c5, 1
		bx	lr

CP15_PrefetchICacheLine:
		mcr	p15, 0, r0, c7, c13, 1
		bx	lr

CP15_FlushDCache:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c6, 0
		bx	lr

CP15_FlushDCacheEntry:
		mcr	p15, 0, r0, c7, c6, 1
		bx	lr

CP15_CleanDCacheEntry:
		mcr	p15, 0, r0, c7, c10, 1
		bx	lr

CP15_CleanAndFlushDCacheEntry:
		mcr	p15, 0, r0, c7, c14, 1
		bx	lr

CP15_CleanDCacheEntryByIndex:
		mcr	p15, 0, r0, c7, c10, 2
		bx	lr

CP15_CleanAndFlushDCacheEntryByIndex:
		mcr	p15, 0, r0, c7, c14, 2
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155 page 2-24
//////////////////////////////////////////////////////////////////////

.global CP15_DrainWriteBuffer
CP15_DrainWriteBuffer:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c10, 4
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155 page 2-24
//////////////////////////////////////////////////////////////////////

.global CP15_WaitForInterrupt
CP15_WaitForInterrupt:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c0, 4
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155 page 2-25
//////////////////////////////////////////////////////////////////////

.global CP15_GetDCacheLockdown
.global CP15_GetICacheLockdown
.global CP15_SetDCacheLockdown
.global CP15_SetICacheLockdown

CP15_GetDCacheLockdown:
		mrc	p15, 0, r0, c9, c0, 0
		bx	lr

CP15_GetICacheLockdown:
		mrc	p15, 0, r0, c9, c0, 1
		bx	lr

CP15_SetDCacheLockdown:
		mcr	p15, 0, r0, c9, c0, 0
		bx	lr

CP15_SetICacheLockdown:
		mcr	p15, 0, r0, c9, c0, 1
		bx	lr

//////////////////////////////////////////////////////////////////////
// See DDI0155 page 2-26
//////////////////////////////////////////////////////////////////////

.global CP15_GetDTCM
.global CP15_GetITCM
.global CP15_SetDTCM
.global CP15_SetITCM

CP15_GetDTCM:
		mrc	p15, 0, r0, c9, c1, 0
		bx	lr

CP15_GetITCM:
		mrc	p15, 0, r0, c9, c1, 1
		bx	lr

CP15_SetDTCM:
		mcr	p15, 0, r0, c9, c1, 0
		bx	lr

CP15_SetITCM:
		mcr	p15, 0, r0, c9, c1, 1
		bx	lr

//////////////////////////////////////////////////////////////////////
.global CP15_ITCMEnableDefault
CP15_ITCMEnableDefault:
		mcr	p15, 0, r0, c7, c5	

		mov r0, #0							@make sure protection regions are all disabled
		mcr p15, 0, r0, c6, c0, 0
		mcr p15, 0, r0, c6, c1, 0
		mcr p15, 0, r0, c6, c2, 0
		mcr p15, 0, r0, c6, c3, 0
		mcr p15, 0, r0, c6, c4, 0
		mcr p15, 0, r0, c6, c5, 0
		mcr p15, 0, r0, c6, c6, 0
		mcr p15, 0, r0, c6, c7, 0
		
		ldr r0, =0x3F					@set read write access on regon 0 and 1
		mcr p15, 0, r0, c6, c0, 0

		ldr r0, =0x3					@set all mem to read write			
		mcr p15, 0, r0, c5, c0, 3
	
		mov r0, #1      					@enable catching for region 1
		mcr p15, 0, r0, c2, c0, 1
	
		ldr r0, =0x5707D			@enable ITCM and Protection unit
		mrc p15, 0, r0, c1, c0, 0
		orr r0, r1, r0
		mcr p15, 0, r0, c1, c0, 0

.end
