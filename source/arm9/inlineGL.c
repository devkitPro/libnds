/*---------------------------------------------------------------------------------
	$Id: inlineGL.c,v 1.3 2007-02-10 05:47:37 gabebear Exp $

	GL inlines

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

	$Log: not supported by cvs2svn $
	Revision 1.2  2007/02/10 05:13:46  gabebear
	- emptied videoGL.inl into videoGL.h so that doxygen works and because having the ability to remove inline functions isn't important anymore(I think).
	- emptied inlineGL.c; inlineGL.c and videoGL.inl should be deleted
	- inlined most functions. GCC "should" be able to work out if something that is inlined in the code really shouldn't be and create object code for the function. I believe I made pretty sane choices so I doubt this will be an issue.
	- fixed TEXTURE_PACK so that u and v are correct.
	- removed glTexCoord1i because it's not terribly useful and the fix to TEXTURE_PACK will make programs that use this and another glTexCoord fail, this was the only function that had S and T mapped correctly. The function does map to the OpenGL call pretty well, but it wasn't used properly in the examples. In the examples it was used to set two texcoords, which was more confusing than just using GFX_TEX_COORD directly. The OpenGL version only sets the S texcoord and sets T to 0.
	- GL_LIGHTING was defined, I deleted it. GL_LIGHTING is used with glEnable to swap between coloring vertices with normals/materials and coloring with the current color. The DS doesn't handle vertex colors like this. The color of a vertex on the DS is determined by the last command that computes a vertex color. The two commands that set vertex colors are glNormal() and glColor(), whichever was last executed determines the color of the vertex drawn. Implementing the OpenGL way of doing this on the DS would be a rather big waste of time and the current define didn't make any sense anyway.
	- removed glTranslate3f32(delta) because it doesn't map to an OpenGL call, or even make much sense. I doubt anyone was using it.
	- removed all depreciated functions
	- linking issues kept me from typing the sizeX/sizeY params on glTexImage2d() and glTexParameter() to GL_TEXTURE_SIZE_ENUM. I'm not sure how to fix this without inlining glTexImage2d() and glTexParameter().
	
	Revision 1.1  2005/08/22 08:05:53  wntrmute
	moved inlines to separate file
	

---------------------------------------------------------------------------------*/


// this file doesn't have anything in it anymore, it can be deleted
