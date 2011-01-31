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

/*! \file paddle.h
   \brief paddle device slot-2 addon support.
*/
#ifndef PADDLE_HEADER_INCLUDE
#define PADDLE_HEADER_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif


/*! \fn bool paddleIsInserted()
    \brief Check for the paddle
    \return true if that's what is in the slot-2
*/
bool paddleIsInserted();

/*! \fn void paddleRead()
    \brief Obtain the current paddle state
    \return a u16 containing a 12bit number (fixed point fraction), incrementing for clockwise rotations and decrementing for counterclockwise
*/
u16 paddleRead();

//! Resets the paddle device. May change the current value to 0xFFF, 0x000, or 0x001. May perform other unknown internal reset operations. Normally not needed.
void paddleReset();


#ifdef __cplusplus
}
#endif

#endif

