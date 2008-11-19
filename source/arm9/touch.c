/*---------------------------------------------------------------------------------

	touch screen input code

 	Copyright (C) 2005 - 2008
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
#include <nds/ipc.h>
#include <nds/arm9/input.h>
#include <nds/fifocommon.h>
#include <libnds_internal.h>


//---------------------------------------------------------------------------------
void touchRead(touchPosition *data) {
//---------------------------------------------------------------------------------

	if ( !data ) return;

	data->rawx = __transferRegion()->touchX;
	data->rawy = __transferRegion()->touchY;
	data->px = __transferRegion()->touchXpx;
	data->py = __transferRegion()->touchYpx;
	data->z1 = __transferRegion()->touchZ1;
	data->z2 = __transferRegion()->touchZ2;


}

//---------------------------------------------------------------------------------
touchPosition touchReadXY() {
//---------------------------------------------------------------------------------
	
	touchPosition touchPos;

	touchRead(&touchPos);
	return touchPos;

}
