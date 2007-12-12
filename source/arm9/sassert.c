/*---------------------------------------------------------------------------------

	sassert.c -- definitons for DS assertions

	Copyright (C) 2007
		Jason Rogers (Dovoto) 

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
#include <nds/arm9/console.h>

#include <stdio.h>


void __sassert(const char *fileName, int lineNumber, const char* conditionString, const char* message)
{
    consoleDemoInit();
    iprintf("\x1b[j\x1b[42mAssertion!\n\x1b[39mFile: \n%s\n\nLine: %d\n\nCondition:\n%s\n\n\x1b[41m%s",fileName, lineNumber, conditionString, message);

    //todo: exit properly
	while(1);  
}
 
