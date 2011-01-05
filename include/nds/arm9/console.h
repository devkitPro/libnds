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
Provides stdio integration for printing to the DS screen as well as debug print
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
consoleDebugInit(DebugDevice_NOCASH);
fprintf(stderr, "debug message in no$gba window %i", stuff);

OR

consoleDebugInit(DebugDevice_CONSOLE);
fprintf(stderr, "debug message on DS console screen");
</pre>

The print console must be initialized to use DB_CONSOLE

</div>
*/

#ifndef CONSOLE_H
#define CONSOLE_H



#include <nds/ndstypes.h>
#include <nds/arm9/background.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool(* ConsolePrint)(void* con, char c);

//! a font struct for the console.
typedef struct ConsoleFont
{
	u16* gfx;			//!< A pointer to the font graphics (will be loaded by consoleInit() if loadGraphics is true
	u16* pal;			//!< A pointer to the font palette (will be loaded by consoleInit() if loadGraphics is true
	u16  numColors;		//!< Number of colors in the font palette

	u8 bpp;				//!< Bits per pixel in the font graphics

	u16 asciiOffset;	//!<  Offset to the first valid character in the font table
	u16 numChars;		//!< Number of characters in the font graphics
	bool convertSingleColor;	/*!< If true font is treated as a single color font where all non zero pixels are set to
										a value of 15 or 255 (4bpp / 8bpp respectivly).
										This ensures only one palette entry is utilized for font rendering.*/
}ConsoleFont;

/** \brief console structure used to store the state of a console render context.

Default values from consoleGetDefault();
<div class="fixedFont"><pre>
PrintConsole defaultConsole =
{
	//Font:
	{
		(u16*)default_font_bin, //font gfx
		0, //font palette
		0, //font color count
		4, //bpp
		0, //first ascii character in the set
		128, //number of characters in the font set
		true, //convert to single color
	},
	0, //font background map
	0, //font background gfx
	31, //map base
	0, //char base
	0, //bg layer in use
	-1, //bg id
	0,0, //cursorX cursorY
	0,0, //prevcursorX prevcursorY
	32, //console width
	24, //console height
	0,  //window x
	0,  //window y
	32, //window width
	24, //window height
	3, //tab size
	0, //font character offset
	0, //selected palette
	0,  //print callback
	false, //console initialized
	true, //load graphics
};
</pre></div>
*/
typedef struct PrintConsole
{
	ConsoleFont font;	//!< font of the console.

	u16* fontBgMap;		/*!< Pointer to the bg layer map if used.  Is set by bgInit if bgId is valid*/
	u16* fontBgGfx;		/*!< Pointer to the bg layer graphics if used.  Is set by bgInit if bgId is valid*/

	u8 mapBase;			/*!< Map base set by console init based on background setup */
	u8 gfxBase;			/*!< Tile graphics base set by console init based on background setup */

	u8 bgLayer;			/*!< Bg layer used by the background */
	int bgId;			/*!< bgId, should be set with a call to bgInit() or bgInitSub()*/

	int cursorX;		/*!< Current X location of the cursor (as a tile offset by default) */
	int cursorY;		/*!< Current Y location of the cursor (as a tile offset by default) */

	int prevCursorX;	/*!< Internal state */
	int prevCursorY;	/*!< Internal state */

	int consoleWidth;	/*!< Width of the console hardware layer in tiles */
	int consoleHeight;	/*!< Height of the console hardware layer in tiles  */

	int windowX;		/*!< Window X location in tiles (not implemented) */
	int windowY;		/*!< Window Y location in tiles (not implemented) */
	int windowWidth;	/*!< Window width in tiles (not implemented) */
	int windowHeight;	/*!< Window height in tiles (not implemented) */

	int tabSize;		/*!< Size of a tab*/

	u16 fontCharOffset;	/*!< Offset to the first graphics tile in background memory (in case your font is not loaded at a graphics base boundary)*/


	u16 fontCurPal;		/*!< The current palette used by the engine (only applies to 4bpp text backgrounds) */

	ConsolePrint PrintChar;		/*!< callback for printing a character. Should return true if it has handled rendering the graphics
									(else the print engine will attempt to render via tiles) */

	bool consoleInitialised;	/*!< True if the console is initialized */
	bool loadGraphics;			/*!< True if consoleInit should attempt to load font graphics into background memory */
}PrintConsole;


//! Console debug devices supported by libnds.
typedef enum
{
	DebugDevice_NULL = 0x0,		//!< swallows prints to stderr
	DebugDevice_NOCASH = 0x1,	//!< Directs stderr debug statements to no$gba debug window
	DebugDevice_CONSOLE = 0x02	//!< Directs stderr debug statements to DS console window
}DebugDevice;

/*!	\brief Loads the font into the console
	\param console pointer to the console to update, if NULL it will update the current console
	\param font the font to load
*/
void consoleSetFont(PrintConsole* console, ConsoleFont* font);

/*!	\brief Sets the print window
	\param console console to set, if NULL it will set the current console window
	\param x x location of the window
	\param y y location of the window
	\param width width of the window
	\param height height of the window
*/
void consoleSetWindow(PrintConsole* console, int x, int y, int width, int height);

/*!	\brief Gets a pointer to the console with the default values
	this should only be used when using a single console or without changing the console that is returned, other wise use consoleInit()
	\return A pointer to the console with the default values
*/
PrintConsole* consoleGetDefault(void);

/*!	\brief Make the specified console the render target
	\param console A pointer to the console struct (must have been initialized with consoleInit(PrintConsole* console)
	\return a pointer to the previous console
*/
PrintConsole *consoleSelect(PrintConsole* console);

/*!	\brief Initialise the console.
	\param console A pointer to the console data to initialze (if it's NULL, the default console will be used)
	\param layer background layer to use
	\param type the type of the background
	\param size the size of the background
	\param mapBase the map base
	\param tileBase the tile graphics base
	\param mainDisplay if true main engine is used, otherwise false
	\param loadGraphics if true the default font graphics will be loaded into the layer
	\return A pointer to the current console.
*/
PrintConsole* consoleInit(PrintConsole* console, int layer, BgType type, BgSize size, int mapBase, int tileBase, bool mainDisplay, bool loadGraphics);

/*!	\brief Initialize the console to a default state for prototyping.
	This function sets the console to use sub display, VRAM_C, and BG0 and enables MODE_0_2D on the
	sub display.  It is intended for use in prototyping applications which need print ability and not actual
	game use.  Print functionality can be utilized with just this call.
	\return A pointer to the current PrintConsole.
*/
PrintConsole* consoleDemoInit(void);

//! Clears the screan by using iprintf("\x1b[2J");
void consoleClear(void);

/*!	\brief Initializes debug console output on stderr to the specified device
	\param device The debug device (or devices) to output debug print statements to
*/
void consoleDebugInit(DebugDevice device);

#ifdef __cplusplus
}
#endif

#endif
