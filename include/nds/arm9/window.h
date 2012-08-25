/*---------------------------------------------------------------------------------

	window.h -- definitions for object and background windowing

	Copyright (C) 2007
		Dave Murphy (WinterMute)
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
/*! \file window.h
    \brief windowing support functions for objects and backgrounds
*/

#include <nds/ndstypes.h>
#include <nds/arm9/video.h>
#include <nds/arm9/sprite.h>
#include <nds/arm9/background.h>
#include <nds/arm9/sassert.h>
#include <nds/memory.h>
#include <nds/dma.h>

/*!	\brief the supported windows*/
typedef enum {
  WINDOW_0    = DISPLAY_WIN0_ON,	//!< Window 0.
  WINDOW_1    = DISPLAY_WIN1_ON,	//!< Window 1
  WINDOW_OBJ  = DISPLAY_SPR_WIN_ON, //!< Object window
  WINDOW_OUT  = BIT(16),			//!< Area outside all windows
 
}WINDOW;

#define WINDOW_MASK  (WINDOW_0|WINDOW_1|WINDOW_OBJ) 

static inline 
/**
*    \brief Enable the specified window(s)
*    \param window The window to set bounds on (may be ORed together)
*/
void windowEnable(WINDOW w)     { REG_DISPCNT     |= w & WINDOW_MASK;    }

static inline 
/**
*    \brief Enable the specified window(s)
*    \param window The window to set bounds on (may be ORed together)
*/
void windowDisable(WINDOW w)    { REG_DISPCNT     &= ~(w & WINDOW_MASK); }

static inline
/**
*    \brief Enable the specified window(s)
*    \param window The window to set bounds on (may be ORed together)
*/
void windowEnableSub(WINDOW w)  { REG_DISPCNT_SUB |= w & WINDOW_MASK;    }

static inline
/**
*    \brief Enable the specified window(s)
*    \param window The window to set bounds on (may be ORed together)
*/
void windowDisableSub(WINDOW w) { REG_DISPCNT_SUB &= ~(w & WINDOW_MASK); }

/**
*    \brief Set the windows bounds
*    \param window The window to set bounds on
*    \param left The X coordinate of the left hand side of the rectangle
*    \param top The Y coordinate of the top of the rectangle
*    \param right The X coordinate of the right hand side of the rectangle
*    \param bottom The Y coordinate of the bottom of the rectangle
*/
void windowSetBounds(WINDOW window, u8 left, u8 top, u8 right, u8 bottom);

/**
*    \brief Set the windows bounds (Sub engine)
*    \param window The window to set bounds on
*    \param left The X coordinate of the left hand side of the rectangle
*    \param top The Y coordinate of the top of the rectangle
*    \param right The X coordinate of the right hand side of the rectangle
*    \param bottom The Y coordinate of the bottom of the rectangle
*/
void windowSetBoundsSub(WINDOW window, u8 left, u8 top, u8 right, u8 bottom);



/*!	\brief Enables the window on the supplied background.
	\param id
		background id returned from bgInit or bgInitSub
	\param window
		the the window to enable
*/
void bgWindowEnable(int id, WINDOW window);
/*!	\brief Disables the window on the supplied background.
	\param id
		background id returned from bgInit or bgInitSub
	\param window
		the the window to disable
*/
void bgWindowDisable(int id, WINDOW window);

/**
*    \brief Enables the specified window.
*    \param oam must be: &oamMain or &oamSub
*    \param the window to enable
*/
void oamWindowEnable(OamState* oam, WINDOW w);

/**
*    \brief Disables the specified window.
*    \param oam must be: &oamMain or &oamSub
*    \param the window to disable
*/
void oamWindowDisable(OamState* oam, WINDOW w);