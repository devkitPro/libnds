/*---------------------------------------------------------------------------------

	console functions

	Copyright (C) 2005
		Michael Noland (joat)
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

/*! \file console.h
    \brief nds stdio support.

<div class="fileHeader">
Provides studio integration for printing to the DS screen as well as debug print
functionality provided by stderr.

General usage is to initialize the console by:
consoleDemoInit()
or to customize the console usage by:
consoleInit()

The default instance utilizes the sub display, approximatly 15KiB of vram C starting
at tile base 0 and 2KiB of map at map base 30. 

Debug printing is performed by initializing the debug console via consoleDebugInit() as 
follows:

<pre>
consoleDebugInit(DB_NOCASH);
fprintf(stderr, "debug message in no$gba window %i", stuff);

OR

consoleDebugInit(DB_CONSOLE);
fprintf(stderr, "debug message on DS console screen");
</pre>

The print console must be initialized to use DB_CONSOL

</div>
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_USE_COLOR255 16


#include <nds/ndstypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum DebugDevice
 * \brief Console debug devices supported by libnds 
 */
typedef enum 
{
	DB_NOCASH = 0x1, /*!< Prints debug statements to no$gba debug window >*/
	DB_CONSOLE = 0x02 /*!< Prints debug statements to DS console window >*/
}DebugDevice;

/*! \fn void consoleInit(u16* font, u16* charBase, u16 numCharacters, u8 charStart, u16* map, u8 pal, u8 bitDepth)
	\brief Initialise the console.
	\param font	base address of the 16 color font to use
	\param charBase	VRAM address to load the font
	\param numCharacters number of characters in the font
	\param charStart ascii code of the first character in the font
	\param map base address of the map to use for printing
	\param pal 16 color palette index to use
	\param bitDepth 256/16 color tile flag. 
	
	Initializes the console with the given parameters. When pal is greater than 15 and
	bitDepth is 16 then	all non zero entries in the font are set to index 255. When bitDepth
	is not 16 then the font tiles are created as 8bit (256 color).
 
*/
void consoleInit(u16* font, u16* charBase, u16 numCharacters, u8 charStart, u16* map, u8 pal, u8 bitDepth);
/*! \fn void consoleInitDefault(u16* map, u16* charBase, u8 bitDepth)
	\brief Initialize the console with some default parameters.
	\param charBase	VRAM address to load the font
	\param map base address of the map to use for printing
	\param bitDepth 256/16 color tile flag

	This function calls consoleInit() with the default built in font and character ranges, the parameters given
	are as for that function. 
*/
void consoleInitDefault(u16* map, u16* charBase, u8 bitDepth);

/*! \fn void consoleDemoInit(void)
\brief Initialize the console to a default state for prototyping.
This function sets the console to use sub display, VRAM_C, and BG0 and enables MODE_0_2D on the
sub display.  It is intended for use in prototyping applications which need print ability and not actual
game use.  Print functionality can be utilized with just this call.
*/
void consoleDemoInit(void);
/*! \fn void consoleClear(void)
\brief Clears the screan by iprintf("\x1b[2J");
*/
void consoleClear(void);

/*! \fn void consoleDebugInit(DebugDevice device)
	\brief Initializes debug console output on stderr to the specified device
	\param device The debug device (or devices) to output debug print statements to
*/
void consoleDebugInit(DebugDevice device);

#ifdef __cplusplus
}
#endif

#endif
