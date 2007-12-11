#include <nds.h>

#include <stdio.h>


void __sassert(const char *fileName, int lineNumber, const char* conditionString, const char* message)
{
     consoleDemoInit();
     iprintf("\x1b[j\x1b[42mAssertion!\n\x1b[39mFile: \n%s\n\nLine: %d\n\nCondition:\n%s\n\n\x1b[41m%s",fileName, lineNumber, conditionString, message);
	 
	 //todo: exit properly
	 while(1);  
}
 
