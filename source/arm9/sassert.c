/*---------------------------------------------------------------------------------

	sassert.c -- definitons for DS assertions

	Copyright (C) 2013
		Jason Rogers (Dovoto)
		Michael Theall (mtheall)

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
#include <stdarg.h>

void __sassert(const char *fileName, int lineNumber, const char* conditionString, const char* format, ...)
{
    va_list ap;

    consoleDemoInit();

    iprintf("\x1b[j"               /* clear screen */
            "\x1b[42mAssertion!\n" /* print in green? */
            "\x1b[39mFile: \n"     /* print in default color */
            "%s\n\n"               /* print filename */
            "Line: %d\n\n"         /* print line number */
            "Condition:\n"
            "%s\n\n"               /* print condition message */
            "\x1b[41m",            /* change font color to red */
            fileName, lineNumber, conditionString);

    va_start(ap, format);
    viprintf(format, ap);
    va_end(ap);

    //todo: exit properly
    while(1);
}
