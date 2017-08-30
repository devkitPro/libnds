/*---------------------------------------------------------------------------------

	Copyright (C) 2017
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

---------------------------------------------------------------------------------*/
#include <nds/asminc.h>

	.text
	.align 4

	.thumb

@---------------------------------------------------------------------------------
BEGIN_ASM_FUNC swiSHA1InitTWL
@---------------------------------------------------------------------------------
	swi	0x24
	bx	lr

@---------------------------------------------------------------------------------
BEGIN_ASM_FUNC swiSHA1UpdateTWL
@---------------------------------------------------------------------------------
	swi	0x25
	bx	lr

@---------------------------------------------------------------------------------
BEGIN_ASM_FUNC swiSHA1FinalTWL
@---------------------------------------------------------------------------------
	swi	0x26
	bx	lr

@---------------------------------------------------------------------------------
BEGIN_ASM_FUNC swiSHA1CalcTWL
@---------------------------------------------------------------------------------
	swi	0x27
	bx	lr
