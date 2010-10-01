/*---------------------------------------------------------------------------------

	Copyright (C) 2005
		Jason Rogers (dovoto)
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
/*! \file debug.h
\brief Currently only used to send debug messages to NO$GBA debug window

<div class="fileHeader">
On the ARM 9 this functionality is best accessed via the console studio integration.
- \ref console.h "Debug Messages via stdio"

</div>
*/

#ifndef NDS_DEBUG_INCLUDE
#define NDS_DEBUG_INCLUDE

void nocashWrite(const char *message, int len);
/*! \brief Send a message to the no$gba debug window 
\param message The message to send
*/
void nocashMessage(const char *message);

#endif // NDS_DEBUG_INCLUDE

