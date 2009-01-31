/*---------------------------------------------------------------------------------

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
/*! \file nds.h
\brief the master include file for nds applications
*/

/*!
 \mainpage Libnds Documentation

 
 \section intro Introduction
 Welcome to the libnds reference documentation.

 \section video_2D_api 2D engine API 
 - \ref video.h "General video"
 - \ref background.h "2D Background Layers"
 - \ref sprite.h "2D Sprites"

 \section video_3D_api 3D engine API
  - \ref videoGL.h "OpenGL (ish)"
  - \ref boxtest.h "Box Test"
  - \ref postest.h "Position test"
 
 \section audio_api Audio API
 - \ref sound.h "Simple Sound Engine"
 - <a href="http://www.maxmod.org/ref">Maxmod</a>

 \section math_api Math
  - \ref math.h "Hardware Assisted Math"
  - \ref trig_lut.h "Fixed point trigenometry functions"

 \section memory_api Memory
 - \ref memory.h "General memory definitions"
 - \ref memory.h "nds and gba header structure"
 - \ref dma.h "Direct Memory Access"

 \section system_api System
 - \ref ndstypes.h "Custom DS types"
 - \ref system.h "Hardware Initilization"
 - \ref bios.h "Bios"
 - \ref cache.h "ARM 9 Cache"
 - \ref interrupts.h "Interrupts"
 - \ref fifocommon.h "Fifo"
 - \ref timers.h "Timers"
 
 \section user_io_api User Input/ouput
 - \ref arm9/input.h "Keypad and Touch pad"
 - \ref keyboard.h "Keyboard"
 - \ref console.h "Console and Debug Printing"

\section utility_api Utility
 - \ref decompress.h "Decompression"
 - \ref image.h "Image Manipulation"
 - \ref pcx.h "PCX file loader"
 - \ref dynamicArray.h "General Purpose Data Structures"

 \section peripheral_api Custom Peripherals
 - \ref rumble.h "Rumble Pack"
 - \ref ndsmotion.h "DS Motion Pack"

 \section debug_api Debugging
 - \ref console.h "Debug via printf to DS screen or NO$GBA"
 - \ref debug.h "Send message to NO$GBA"

 <!-- EXAMPLES -->
    <!-- backgrounds -->
	\example Graphics/Backgrounds/all_in_one/source/advanced.cpp
	\example Graphics/Backgrounds/all_in_one/source/basic.cpp
	\example Graphics/Backgrounds/all_in_one/source/scrolling.cpp
	\example Graphics/Backgrounds/all_in_one/source/handmade.cpp

    <!-- sprites -->
    \example Graphics/Sprites/allocation_test/source/main.c
	\example Graphics/Sprites/animate_simple/source/template.c
	\example Graphics/Sprites/bitmap_sprites/source/main.cpp
 	\example Graphics/Sprites/fire_and_sprites/source/main.cpp
	\example Graphics/Sprites/simple/source/template.c

	<!-- keyboard -->
	\example input/keyboard/keyboard_stdin/source/keymain.c
	\example input/keyboard/keyboard_async/source/template.c

	<!-- touchpad -->
	\example input/Touch_Pad/touch_area/source/template.c
	\example input/Touch_Pad/touch_look/source/main.cpp
	\example input/Touch_Pad/touch_test/source/main.c

	<!-- 3D -->
	\example Graphics/3D/nehe/lesson01/source/nehe1.cpp
	\example Graphics/3D/nehe/lesson02/source/nehe2.cpp
	\example Graphics/3D/nehe/lesson03/source/nehe3.cpp
	\example Graphics/3D/nehe/lesson04/source/nehe4.cpp
	\example Graphics/3D/nehe/lesson05/source/nehe5.cpp
	\example Graphics/3D/nehe/lesson06/source/nehe6.cpp
	\example Graphics/3D/nehe/lesson07/source/nehe7.cpp
	\example Graphics/3D/nehe/lesson08/source/nehe8.cpp
	\example Graphics/3D/nehe/lesson09/source/nehe9.cpp
	\example Graphics/3D/nehe/lesson10/source/nehe10.cpp
	\example Graphics/3D/nehe/lesson10b/source/nehe10b.cpp
	\example Graphics/3D/nehe/lesson11/source/nehe11.cpp
	\example Graphics/3D/3D_Both_Screens/source/template.c
	\example Graphics/3D/Display_List/source/main.cpp

	<!-- RTC -->
	\example RealTimeClock/Watch/source/main.cpp

	<!-- sound api -->
	\example audio/micrecord/source/micrecord.c

	<!-- hello world -->
	\example hello_world/source/main.cpp

	<!-- decompress -->
	\example Graphics/Backgrounds/16bit_color_bmp/source/template.cpp

	<!-- printing -->
	\example Graphics/Printing/ansi_console/source/main.c
	\example Graphics/Printing/console_windows/source/main.c
	\example Graphics/Printing/custom_font/source/main.c
	\example Graphics/Printing/print_both_screens/source/template.c
	\example Graphics/Printing/rotscale_text/source/main.c



*/

#ifndef NDS_INCLUDE
#define NDS_INCLUDE

#ifndef ARM7
#ifndef ARM9
#error Either ARM7 or ARM9 must be defined
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "nds/libversion.h"
#include "nds/ndstypes.h"
#include "nds/bios.h"
#include "nds/card.h"
#include "nds/debug.h"
#include "nds/dma.h"
#include "nds/interrupts.h"
#include "nds/ipc.h"
#include "nds/memory.h"
#include "nds/system.h"
#include "nds/timers.h"
#include "nds/fifocommon.h"
#include "nds/touch.h"
//---------------------------------------------------------------------------------
#ifdef ARM9
//---------------------------------------------------------------------------------

#include "nds/arm9/background.h"
#include "nds/arm9/boxtest.h"
#include "nds/arm9/cache.h"
#include "nds/arm9/console.h"
#include "nds/arm9/keyboard.h"
#include "nds/arm9/exceptions.h"
#include "nds/arm9/image.h"
#include "nds/arm9/input.h"
#include "nds/arm9/math.h"
#include "nds/arm9/pcx.h"
#include "nds/arm9/rumble.h"
#include "nds/arm9/sound.h"
#include "nds/arm9/trig_lut.h"
#include "nds/arm9/video.h"
#include "nds/arm9/videoGL.h"
#include "nds/arm9/sprite.h"
#include "nds/arm9/decompress.h"

//---------------------------------------------------------------------------------
#endif // #ifdef ARM9
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
#ifdef ARM7
//---------------------------------------------------------------------------------

#include "nds/arm7/audio.h"
#include "nds/arm7/clock.h"
#include "nds/arm7/serial.h"
#include "nds/arm7/touch.h"
#include "nds/arm7/input.h"

//---------------------------------------------------------------------------------
#endif // #ifdef ARM7
//---------------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif


#endif // NDS_INCLUDE


