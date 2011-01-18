/*---------------------------------------------------------------------------------

	Copyright (C) 2011
		Tobias Weyand (0xtob)

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

/*! \file piano.h
   \brief NDS Easy Piano option pack support.
*/
#ifndef PIANO_HEADER_INCLUDE
#define PIANO_HEADER_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#define PIANO_PAK   (*(vu16 *)0x09FFFFFE)

#define PIANO_C     BIT(0)
#define PIANO_CS    BIT(1)
#define PIANO_D     BIT(2)
#define PIANO_DS    BIT(3)
#define PIANO_E     BIT(4)
#define PIANO_F     BIT(5)
#define PIANO_FS    BIT(6)
#define PIANO_G     BIT(7)
#define PIANO_GS    BIT(8)
#define PIANO_A     BIT(9)
#define PIANO_AS    BIT(10)
#define PIANO_B     BIT(13)
#define PIANO_C2    BIT(14)

/*! \fn bool pianoIsInserted()
    \brief Check for piano option pack.
    \return true if the cart in the GBA slot is the piano option pack.
*/
bool pianoIsInserted();

/*! \fn void pianoScanKeys()
    \brief Obtain the current piano state.
    Call this function once per main loop to use the piano functions.
*/
void pianoScanKeys();

//! Obtains the current piano keys held state
u16 pianoKeysHeld();

//! Obtains the current piano keys pressed state
u16 pianoKeysDown();

//! Obtains the current piano keys released state
u16 pianoKeysUp();

#ifdef __cplusplus
}
#endif

#endif

