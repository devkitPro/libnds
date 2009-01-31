/*---------------------------------------------------------------------------------

	keyboard.h -- stdin integration for a simple keyboard

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
/*! \file keyboard.h
    \brief nds stdio keyboard integration.

<div class="fileHeader">The keyboard component allows the use of a default keyboard via stdin as well as 
direct via the functions exposed below.  The default behavior is a hidden keyboard 
that shows on a call to scanf(stdin, ...).

By default the keyboard uses background 3 of the sub display, consumes approximatly 40KiB
of background vram begining at tile base 1 and 2KB of map stored at map base 30.  The default 
is designed to function along side a default instance of the console print functionality.

To customize keyboard behavior and resource usage modify the keyboard structure returned by
keyboardGetDefault() or create your own keyboard.
</div>
*/


	


#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <nds/ndstypes.h>
#include <nds/arm9/background.h>

//!KeyChangedCallback
typedef void (*KeyChangeCallback)(int key);

/**
 * \enum KeyboardState
 * \brief states the keyboard can be in, currently only Lower and Upper supported
 */
typedef enum
{
	Lower = 0, /*!< Normal keyboard display (lowercase letters) >*/
	Upper = 1, /*!< Caps lock Held >*/
	Numeric = 2, /*!< Numeric only keypad (not provided by the default keyboard) >*/
	Reduced = 3 /*!< Reduced footprint keyboard (not provided by the default keyboard) >*/
}KeyboardState;

/**
 * \struct KeyMap
 * \brief defines a key mapping
 */
typedef struct 
{
   const u16* mapDataPressed;/*!< the map for keys pressed >*/
   const u16* mapDataReleased;/*!< the map for keys released >*/
   const int* keymap; /*!< the lookup table for x y grid location to corresponding key >*/
   int width; /*!< width of the keyboard in grid spaces >*/
   int height; /*!< height of the keyboard in grid spaces>*/
}KeyMap;

/**
 * \struct Keyboard
 * \brief describes a keyboard
 */
typedef struct 
{
	int background;/*!< Background number to use, after init() this contains the background ID >*/
	int keyboardOnSub; /*!< boolean to determine if keyboard is on sub screen or main >*/
    int offset_x; /*!< x offset of the map, can be used to center a custom keyboard >*/
	int offset_y; /*!< y offset of the map, can be used to center a custom keyboard >*/
	int grid_width; /*!< the grid width, this size will be used to translate x coordinate of touch to x coordinate in keymap >*/
	int grid_height;/*!< the grid height, this size will be used to translate y coordinate of touch to y coordinate in keymap >*/
	KeyboardState state; /*!< the state of the keyboard>*/
    int shifted;  /*!< true if shifted>*/
    int visible; /*!< true if visible>*/
	KeyMap* mappings[4];
	//KeyMap* lower; /*!< keymapping for lower case normal keyboard>*/
 //   KeyMap* upper;/*!< keymapping for shifted upper case normal keyboard>*/
 //   KeyMap* numeric; /*!< keymapping for numeric keypad>*/
 //   KeyMap* reduced; /*!< keymapping for reduced footprint keyboard>*/
	const u16* tiles; /*!< pointer to graphics tiles, cannot exceed 44KB with default base>*/
    u32 tileLen; /*!< length in bytes of graphics data>*/
    const u16* palette; /*!< pointer to the palette>*/
    u32 paletteLen; /*!< length in bytes of the palette data>*/
    int mapBase; /*!< map base to be used by the keyboard>*/
	int tileBase; /*!< tile base to be used by keyboard graphics>*/
	int tileOffset; /*!< tile offset (in bytes) to load graphics (map must be preadjusted) >*/
	u32 scrollSpeed; /*!<keyboard scroll speed on hide and show in pixels per frame (must be positive and 0 == instant on) */
   KeyChangeCallback OnKeyPressed; /*!< will be called on key press>*/
	KeyChangeCallback OnKeyReleased; /*!< will be called on key release >*/
}Keyboard;

#ifdef __cplusplus
extern "C" {
#endif

/*! \fn Keyboard* keyboardGetDefault(void)
	\brief Gets the default keyboard
*/
Keyboard* keyboardGetDefault(void);

/*! \fn Keyboard* keyboardInit(Keyboard* keyboard, int layer, BgType type, BgSize size, int mapBase, int tileBase, bool mainDisplay, bool loadGraphics);
	\brief initializes the keyboard system with the supplied keyboard
	\param keyboard the keyboard struct to initialize (can be NULL)
	\param layer the background layer to use
	\param type the background type to initialize
	\param size the background size to initialize
	\param mapBase the map base to use for the background
	\param tileBase the graphics tile base to use for the background
	\param mainDisplay if true the keyboard will render on the main display
	\param loadGraphics if true the keyboard graphics will be loaded
	\return returns the initialized keyboard struct
*/
Keyboard* keyboardInit(Keyboard* keyboard, int layer, BgType type, BgSize size, int mapBase, int tileBase, bool mainDisplay, bool loadGraphics);

/*! \fn Keyboard* keyboardDemoInit(void)
	\brief initializes the keyboard with default options
	
	Same as calling keyboardInit(NULL, 3, BgType_Text4bpp, BgSize_T_256x512, 20, 0, false, true)
*/
Keyboard* keyboardDemoInit(void);
/*! \fn void keyboardShow(void)
	\brief Displays the keyboard
*/
void keyboardShow(void);

/*! \fn void keyboardHide(void)
	\brief Hides the keyboard
*/
void keyboardHide(void);

/*! \fn int keyboardGetKey(int x, int y)
	\brief returns the ascii code for the key located at the supplied x and y. Will not
	effect keyboard shift state.
	\param x the pixel x location
	\param y the pixel y location
	\return the key pressed or NOKEY if user pressed outside the keypad
*/
int keyboardGetKey(int x, int y);

/*! \fn void keyboardGetString(char * buffer, int maxLen)
	\brief reads the input until a the return key is pressed or the maxLen 
	is exceeded.
	\param buffer a buffer to hold the input string
	\param maxLen the maximum length to read
*/
void keyboardGetString(char * buffer, int maxLen);

/*! \fn int keyboardGetChar(void)
	\brief Waits for user to press a key and returns the key pressed.  Use
	keyboardUpdate instead for async operation.
*/
int keyboardGetChar(void);

/*! \fn int keyboardUpdate(void)
	\brief Processes the keyboard. Should be called once per frame
	when using the keyboard in an async manner.
	\returns the ascii code of the key pressed or -1 if no key was pressed.
*/
int keyboardUpdate(void);

#ifdef __cplusplus
}
#endif

#endif
