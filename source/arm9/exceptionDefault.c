/*---------------------------------------------------------------------------------

  Copyright (C) 2005
	Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

---------------------------------------------------------------------------------*/

#include <nds/ndstypes.h>
#include <nds/memory.h>

#include <nds/arm9/video.h>
#include <nds/arm9/console.h>
#include <nds/arm9/exceptions.h>
#include <nds/arm9/background.h>

#include <calico/system/thread.h>
#include <calico/system/dietprint.h>

#include <unistd.h>

//---------------------------------------------------------------------------------
static void excptPrint(const char* buf, size_t size) {
//---------------------------------------------------------------------------------
	if (buf) {
		write(1, buf, size);
	} else {
		for (size_t i = 0; i < size; i ++) {
			char spc = ' ';
			write(1, &spc, 1);
		}
	}
}

//---------------------------------------------------------------------------------
static void defaultHandler(ExcptContext* ctx, unsigned flags) {
//---------------------------------------------------------------------------------
	consoleDemoInit();
	dietPrintSetFunc(excptPrint);

	BG_PALETTE_SUB[0] = RGB15(31,0,0);
	BG_PALETTE_SUB[255] = RGB15(31,31,31);

	guruMeditationDump(ctx, flags);

	Thread* cur = threadGetSelf();
	dietPrint("thrd: %08lX   tp : %08lX\n", (u32)cur, (u32)cur->tp);

	u32* stack = (u32*)ctx->r[13];
	for (unsigned i = 0; i < 8; i ++) {
		dietPrint("\n  %08lX:  %08lX %08lX", (u32)&stack[i*2], stack[i*2], stack[(i*2)+1]);
	}
}

//---------------------------------------------------------------------------------
void defaultExceptionHandler(void) {
//---------------------------------------------------------------------------------
	setExceptionHandler(defaultHandler) ;
}
