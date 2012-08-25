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
\brief the master include file for nds applications.
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
 - \ref dynamicArray.h "General Purpose dynamic array implementation"
 - \ref linkedlist.h "General purpose linked list implementation"

 \section peripheral_api Custom Peripherals
 - \ref rumble.h "Rumble Pack"
 - \ref ndsmotion.h "DS Motion Pack"
 - \ref piano.h "DS Easy Piano Controller"

 \section debug_api Debugging
 - \ref console.h "Debug via printf to DS screen or NO$GBA"
 - \ref debug.h "Send message to NO$GBA"
 - \ref sassert.h "simple assert"



 \section external_links Usefull links
 - <a href="http://www.devkitpro.org/">devkitPro</a>
 - <a href="http://wiki.devkitpro.org/index.php/Main_Page">devkitPro wiki</a>
 - <a href="http://sourceforge.net/projects/devkitpro/">devkitPro sourceforge page</a>
 - <a href="http://nocash.emubase.de/gbatek.htm">gbatek, gba and nds hardware documentation.</a> (<a href="http://forum.gbadev.org/viewtopic.php?t=16867">V2.6a download</a>)

*/


//adding the example page.
/*!
 <!-- EXAMPLES -->
    <!-- hello world -->
	\example hello_world/source/main.cpp

    <!-- backgrounds -->
    \example Graphics/Backgrounds/all_in_one/source/main.cpp
	\example Graphics/Backgrounds/all_in_one/source/advanced.cpp
	\example Graphics/Backgrounds/all_in_one/source/basic.cpp
	\example Graphics/Backgrounds/all_in_one/source/scrolling.cpp
	\example Graphics/Backgrounds/all_in_one/source/handmade.cpp
	\example Graphics/Backgrounds/16bit_color_bmp/source/template.cpp
	\example Graphics/Backgrounds/256_color_bmp/source/main.cpp
	\example Graphics/Backgrounds/Double_Buffer/source/main.cpp
	\example Graphics/Backgrounds/rotation/source/main.cpp
	
    <!-- sprites -->
    \example Graphics/Sprites/allocation_test/source/main.c
	\example Graphics/Sprites/animate_simple/source/template.c
	\example Graphics/Sprites/bitmap_sprites/source/main.cpp
 	\example Graphics/Sprites/fire_and_sprites/source/main.cpp
	\example Graphics/Sprites/simple/source/template.c
	\example Graphics/Sprites/sprite_extended_palettes/source/template.c
	\example Graphics/Sprites/sprite_rotate/source/template.c
	
	<!-- Graphics Effects -->
	\example Graphics/Effects/windows/source/template.c

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
	\example Graphics/3D/Display_List_2/source/main.cpp
	\example Graphics/3D/Env_Mapping/source/main.cpp
	\example Graphics/3D/BoxTest/source/main.cpp
	\example Graphics/3D/Ortho/source/main.cpp
	\example Graphics/3D/Paletted_Cube/source/main.cpp
	\example Graphics/3D/Picking/source/main.cpp
	\example Graphics/3D/Simple_Quad/source/main.cpp
	\example Graphics/3D/Simple_Tri/source/main.cpp
	\example Graphics/3D/Textured_Cube/source/main.cpp	
	\example Graphics/3D/Textured_Quad/source/main.cpp
	\example Graphics/3D/Toon_Shading/source/main.cpp	
	
	<!-- RTC -->
	\example time/RealTimeClock/source/main.c
	
	<!-- Timers -->
	\example time/stopwatch/source/main.c
	\example time/timercallback/source/main.c
	

	<!-- capture -->
	\example capture/ScreenShot/source/main.cpp
	\example capture/ScreenShot/source/screenshot.cpp

	<!-- sound api -->
	\example audio/micrecord/source/micrecord.c
	\example audio/maxmod/audio_modes/source/main.c
	\example audio/maxmod/basic_sound/source/MaxModExample.c
	\example audio/maxmod/reverb/source/main.c
	\example audio/maxmod/song_events_example/source/template.c
	\example audio/maxmod/song_events_example2/source/template.c
	\example audio/maxmod/streaming/source/main.c

	<!-- debugging -->
	\example debugging/exceptionTest/source/exceptionTest.c

	<!-- printing -->
	\example Graphics/Printing/ansi_console/source/main.c
	\example Graphics/Printing/console_windows/source/main.c
	\example Graphics/Printing/custom_font/source/main.c
	\example Graphics/Printing/print_both_screens/source/template.c
	\example Graphics/Printing/rotscale_text/source/main.c

	<!-- filesystem -->
	\example filesystem/libfat/libfatdir/source/directory.c
	\example filesystem/nitrofs/nitrodir/source/directory.c

	<!-- dswifi -->
	\example dswifi/ap_search/source/template.c
	\example dswifi/autoconnect/source/autoconnect.c
	\example dswifi/httpget/source/httpget.c

	<!-- card -->
	\example card/eeprom/source/main.cpp

	<!-- ds_motion -->
	\example ds_motion/source/main.c
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
#include "nds/input.h"


//---------------------------------------------------------------------------------
#ifdef ARM9
//---------------------------------------------------------------------------------

#include "nds/arm9/dynamicArray.h"
#include "nds/arm9/linkedlist.h"
#include "nds/arm9/background.h"
#include "nds/arm9/boxtest.h"
#include "nds/arm9/cache.h"
#include "nds/arm9/console.h"
#include "nds/arm9/decompress.h"
#include "nds/arm9/exceptions.h"
#include "nds/arm9/guitarGrip.h"
#include "nds/arm9/image.h"
#include "nds/arm9/input.h"
#include "nds/arm9/keyboard.h"
#include "nds/arm9/math.h"
#include "nds/arm9/paddle.h"
#include "nds/arm9/pcx.h"
#include "nds/arm9/piano.h"
#include "nds/arm9/rumble.h"
#include "nds/arm9/sassert.h"
#include "nds/arm9/sound.h"
#include "nds/arm9/sprite.h"
#include "nds/arm9/window.h"
#include "nds/arm9/trig_lut.h"
#include "nds/arm9/video.h"
#include "nds/arm9/videoGL.h"

//---------------------------------------------------------------------------------
#endif // #ifdef ARM9
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
#ifdef ARM7
//---------------------------------------------------------------------------------

#include "nds/arm7/audio.h"
#include "nds/arm7/clock.h"
#include "nds/arm7/input.h"
#include "nds/arm7/i2c.h"
#include "nds/arm7/serial.h"
#include "nds/arm7/touch.h"

//---------------------------------------------------------------------------------
#endif // #ifdef ARM7
//---------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif // NDS_INCLUDE


