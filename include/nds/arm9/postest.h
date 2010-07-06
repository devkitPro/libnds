/*---------------------------------------------------------------------------------

PosTest.c -- Code for performing hardware position testing

Copyright (C) 2007
Gabe Ghearing (gabebear)

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

/*! \file postest.h
\brief Position Test Functions.<BR>

<div class="fileHeader">
The position test multiplies a given vector by the position matrix and returns the coords(x,y,z,w). The position test is really quick, about 10x faster than a box test.
</div>
*/

#ifndef POS_TEST_INCLUDE
#define POS_TEST_INCLUDE

#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>


GL_STATIC_INL
/*! \brief true if the hardware is currently performing a position/vertex/box test.
	\return whether a test is being performed
*/
bool PosTestBusy()
{
	return (GFX_STATUS & BIT(0))!=0;
}

GL_STATIC_INL
/*! \brief Starts a position test asynchronously
	\param x specifies x offset from the current modelview matrix
	\param y specifies y offset from the current modelview matrix
	\param z specifies z offset from the current modelview matrix
*/
void PosTest_Asynch(v16 x, v16 y, v16 z)
{
	GFX_POS_TEST = VERTEX_PACK(x, y);
	GFX_POS_TEST = z;
}


GL_STATIC_INL
/*! \brief Performs a position test
	\param x specifies x offset from the current modelview matrix
	\param y specifies y offset from the current modelview matrix
	\param z specifies z offset from the current modelview matrix
*/
void PosTest(v16 x, v16 y, v16 z)
{
	PosTest_Asynch(x,y,z);
	while(PosTestBusy());
}

GL_STATIC_INL
/*! \brief Returns the distance from the camera of the last position test, pretty darn useful.
	\return W magnitude
*/
int32 PosTestWresult()
{
	return GFX_POS_RESULT[3];
}

GL_STATIC_INL
/*! \brief Returns absolute X position of the last position test (location if the modelview matrix was identity)
	\return Absolute X position
*/
int32 PosTestXresult()
{
	return GFX_POS_RESULT[0];
}

GL_STATIC_INL
/*! \brief Returns absolute Y position of the last position test (location if the modelview matrix was identity)
	\return Absolute Y position
*/
int32 PosTestYresult()
{
	return GFX_POS_RESULT[1];
}

GL_STATIC_INL
/*! \brief Returns absolute Z position of the last position test (location if the modelview matrix was identity)
	\return Absolute Z position
*/
int32 PosTestZresult()
{
	return GFX_POS_RESULT[2];
}

#endif // ifndef POS_TEST_INCLUDE

