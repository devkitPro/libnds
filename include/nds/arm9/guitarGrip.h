/*---------------------------------------------------------------------------------

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

/*! \file guitarGrip.h
   \brief guitar grip device slot-2 addon support.
*/
#ifndef GUITARGRIP_HEADER_INCLUDE
#define GUITARGRIP_HEADER_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#define GUITARGRIP_GREEN BIT(6)
#define GUITARGRIP_RED BIT(5)
#define GUITARGRIP_YELLOW BIT(4)
#define GUITARGRIP_BLUE BIT(3)


/*! \fn bool guitarGripIsInserted()
    \brief Check for the guitar grip
    \return true if that's what is in the slot-2
*/
bool guitarGripIsInserted();

/*! \fn void guitarGripScanKeys()
    \brief Obtain the current guitar grip state.
    Call this function once per main loop to use the guitarGrip functions.
*/
void guitarGripScanKeys();

//! Obtains the current guitar grip keys held state
u8 guitarGripKeysHeld();

//! Obtains the current guitar grip keys pressed state
u8 guitarGripKeysDown();

//! Obtains the current guitar grip keys released state
u8 guitarGripKeysUp();


#ifdef __cplusplus
}
#endif

#endif

