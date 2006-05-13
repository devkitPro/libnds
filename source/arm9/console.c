/*---------------------------------------------------------------------------------
	$Id: console.c,v 1.17 2006-05-13 13:37:15 wntrmute Exp $

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
	Revision 1.16  2006/01/17 09:40:11  wntrmute
	corrected off by one error in console clear code
	
	Revision 1.15  2006/01/10 05:45:02  dovoto
	Added a consoleClear because i can....
	
	Revision 1.14  2005/12/11 22:49:53  wntrmute
	use con for console device name
	
	Revision 1.13  2005/11/07 04:11:53  dovoto
	Added consoleDemoInit for prototyping
	
	Revision 1.12  2005/10/26 05:22:34  bigredpimp
	Added Line Clearing escape sequences
	- "\x1b[K" & "\x1b[0K" = clear from cursor to end of line
	- "\x1b[1K" = clear from cursor to beginning of line
	- "\x1b[2K" = clear entire line
	
	Changed & Added to Screen Clearing escape sequences
	- "\x1b[0J" = clear from cursor to end of screen
	- "\x1b[1J" = clear from cursor to beginning of screen
	- "\x1b[2J" = clear whole screen and set cursor at 0,0
	
	Revision 1.11  2005/10/20 20:54:44  wntrmute
	doxygenation
	use siscanf
	
	Revision 1.10  2005/09/12 06:50:23  wntrmute
	removed *printAt
	added ansi escape sequences
	
	Revision 1.9  2005/09/04 00:24:44  wntrmute
	exposed consoleSetPos
	move iprintAt and printAt to separate files

	Revision 1.8  2005/08/31 01:10:33  wntrmute
	reworked console into stdio

	Revision 1.7  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.6  2005/08/22 08:09:05  wntrmute
	removed unnecessary headers

	Revision 1.5  2005/07/30 23:20:38  dovoto
	Fixed an error with printf for strings that end with a formating character.

	Revision 1.4  2005/07/14 08:00:57  wntrmute
	resynchronise with ndslib


---------------------------------------------------------------------------------*/

#include <nds/jtypes.h>
#include <nds/arm9/console.h>
#include <nds/arm9/video.h>
#include <nds/memory.h>
#include <default_font_bin.h>
#include <stdio.h>
#include <stdarg.h>

#include <sys/iosupport.h>

//	global console variables

#define CONSOLE_WIDTH 32
#define CONSOLE_HEIGHT 24
#define TAB_SIZE 3

//	map to print to
static u16* fontMap;

//	location of cursor
static int row, col;
static int savedX, savedY;

//	font may not start on a character base boundry
static u16 fontOffset;

//	the first character in the set (0 if you have a full set)
static u16 fontStart;

//	the 16-color palette to use
static u16 fontPal;

static int consoleInitialised = 0;

void consolePrintChar(char c);

//---------------------------------------------------------------------------------
int con_read(struct _reent *r,int fd,char *ptr,int len) {
//---------------------------------------------------------------------------------
	return -1;
}

//---------------------------------------------------------------------------------
int con_close(struct _reent *r,int fd) {
//---------------------------------------------------------------------------------
	return -1;
}

//---------------------------------------------------------------------------------
int con_open(struct _reent *r, void *fileStruct, const char *path,int flags,int mode) {
//---------------------------------------------------------------------------------
	if (consoleInitialised) return 0;
	return -1;
}

//---------------------------------------------------------------------------------
static void consoleCls(char mode) {
//---------------------------------------------------------------------------------

	int i = 0;
	int colTemp,rowTemp;

	switch (mode)
	{
		case '0':
		{
			colTemp = col;
			rowTemp = row;

			while(i++ < ((CONSOLE_HEIGHT * CONSOLE_WIDTH) - (rowTemp * CONSOLE_WIDTH + colTemp))) consolePrintChar(' ');

			col = colTemp;
			row = rowTemp;
			break;
		}	
		case '1':
		{
			colTemp = col;
			rowTemp = row;

			row = 0;
			col = 0;

			while (i++ < (rowTemp * CONSOLE_WIDTH + colTemp)) consolePrintChar(' ');

			col = colTemp;
			row = rowTemp;
			break;
		}	
		case '2':
		{
			row = 0;
			col = 0;

			while(i++ < CONSOLE_HEIGHT * CONSOLE_WIDTH) consolePrintChar(' ');

			row = 0;
			col = 0;
			break;
		}	
	}
}
//---------------------------------------------------------------------------------
static void consoleClearLine(char mode) {
//---------------------------------------------------------------------------------

	int i = 0;
	int colTemp;

	switch (mode)
	{
		case '0':
		{
			colTemp = col;

			while(i++ < (CONSOLE_WIDTH - colTemp)) {
				consolePrintChar(' ');
			}

			col = colTemp;

			break;
		}	
		case '1':
		{
			colTemp = col;

			col = 0;

			while(i++ < ((CONSOLE_WIDTH - colTemp)-2)) {
				consolePrintChar(' ');
			}

			col = colTemp;

			break;
		}	
		case '2':
		{
			colTemp = col;

			col = 0;

			while(i++ < CONSOLE_WIDTH) {
				consolePrintChar(' ');
			}

			col = colTemp;

			break;
		}
		default:
		{
			colTemp = col;

			while(i++ < (CONSOLE_WIDTH - colTemp)) {
				consolePrintChar(' ');
			}

			col = colTemp;

			break;
		}
	}
}
//---------------------------------------------------------------------------------
int con_write(struct _reent *r,int fd,const char *ptr,int len) {
//---------------------------------------------------------------------------------

	if (!consoleInitialised) return -1;

	char chr;

	int i, count = 0;
	char *tmp = (char*)ptr;

	if(!tmp || len<=0) return -1;

	i = 0;
	while(*tmp!='\0' && i<len) {

		chr = *(tmp++);
		i++; count++;

		if ( chr == 0x1b && *tmp == '[' ) {
			bool escaping = true;
			char *escapeseq	= tmp;
			int escapelen = 0;

			do {
				chr = *(tmp++);
				i++; count++; escapelen++;
				int parameter;
				switch (chr) {
					case 'H':
						siscanf(escapeseq,"[%d;%dH", &row, &col);
						escaping = false;
						break;
					case 'f':
						siscanf(escapeseq,"[%d;%df", &row, &col);
						escaping = false;
						break;
					case 'A':
						siscanf(escapeseq,"[%dA", &parameter);
						row =  (row - parameter) < 0 ? 0 : row - parameter;
						escaping = false;
						break;
					case 'B':
						siscanf(escapeseq,"[%dB", &parameter);
						row =  (row + parameter) > CONSOLE_HEIGHT - 1 ? CONSOLE_HEIGHT - 1 : row + parameter;
						escaping = false;
						break;
					case 'C':
						siscanf(escapeseq,"[%dC", &parameter);
						col =  (col + parameter) > CONSOLE_WIDTH - 1 ? CONSOLE_WIDTH - 1 : col + parameter;
						escaping = false;
						break;
					case 'D':
						siscanf(escapeseq,"[%dC", &parameter);
						col =  (col - parameter) < 0 ? 0 : col - parameter;
						escaping = false;
						break;
					case 'K':
						consoleClearLine(escapeseq[escapelen-2]);
						escaping = false;
						break;
					case 's':
						savedX = col;
						savedY = row;
						escaping = false;
						break;
					case 'u':
						col = savedX;
						row = savedY;
						escaping = false;
						break;
					case 'J':
						consoleCls(escapeseq[escapelen-2]);
						escaping = false;
					break;
				}
			} while (escaping);
		continue;
		}

		consolePrintChar(chr);
	}

	return count;
}

const devoptab_t dotab_stdout = {
	"con",
	0,
	con_open,
	con_close,
	con_write,
	con_read,
	NULL,
	NULL
};

const devoptab_t dotab_stderr = {
	"con",
	0,
	con_open,
	con_close,
	con_write,
	con_read,
	NULL,
	NULL
};


/*---------------------------------------------------------------------------------
	consoleInit
	param:
		font: 16 color font
		charBase: the location the font data will be loaded to
		numCharacters: count of characters in the font
		charStart: The ascii number of the first character in the font set
					if you have a full set this will be zero
		map: pointer to the map you will be printing to.
		pal: specifies the 16 color palette to use, if > 15 it will change all non-zero
			entries in the font to use palette index 255
---------------------------------------------------------------------------------*/
void consoleInit(	u16* font, u16* charBase,
					u16 numCharacters, u8 charStart,
					u16* map, u8 pal, u8 bitDepth) {
//---------------------------------------------------------------------------------
	int i;

	row = col = 0;

	fontStart = charStart;

	fontOffset = 0;

	fontMap = map;

	if(bitDepth == 16)
	{
		if(pal < 16)
		{
			fontPal = pal << 12;

			for (i = 0; i < numCharacters * 16; i++)
				charBase[i] = font[i];
		}
		else
		{
			fontPal = 15 << 12;

			for (i = 0; i < numCharacters * 16; i++)
			{
				u16 temp = 0;

				if(font[i] & 0xF)
					temp |= 0xF;
				if(font[i] & 0xF0)
					temp |= 0xF0;
				if(font[i] & 0xF00)
					temp |= 0xF00;
				if(font[i] & 0xF000)
					temp |= 0xF000;

				charBase[i] = temp;
			}
		}
	}//end if bitdepth
	else
	{
		fontPal = 0;
		for(i = 0; i < numCharacters * 16; i++)
		{
			u32 temp = 0;

			if(font[i] & 0xF)
				temp = 255;
			if(font[i] & 0xF0)
				temp |= 255 << 8;
			if(font[i] & 0xF00)
				temp |= 255 << 16;
			if(font[i] & 0xF000)
				temp |= 255 << 24;

			((u32*)charBase)[i] = temp;

		}
	}

	devoptab_list[STD_OUT] = &dotab_stdout;
	devoptab_list[STD_ERR] = &dotab_stderr;
	setvbuf(stderr, NULL , _IONBF, 0);
	setvbuf(stdout, NULL , _IONBF, 0);
	consoleCls('2');
	consoleInitialised = 1;

}

//---------------------------------------------------------------------------------
// Places the console in a default mode using bg0 of the sub display, and vram c for 
// font and map..this is provided for rapid prototyping and nothing more
void consoleDemoInit(void) {
//---------------------------------------------------------------------------------
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);	//sub bg 0 will be used to print text
	vramSetBankC(VRAM_C_SUB_BG); 

	SUB_BG0_CR = BG_MAP_BASE(31);

	BG_PALETTE_SUB[255] = RGB15(31,31,31);	//by default font will be rendered with color 255

	//consoleInit() is a lot more flexible but this gets you up and running quick
	consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(31), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);
}

//---------------------------------------------------------------------------------
void consoleInitDefault(u16* map, u16* charBase, u8 bitDepth) {
//---------------------------------------------------------------------------------
	consoleInit((u16*)default_font_bin, charBase, 256, 0, map, CONSOLE_USE_COLOR255, bitDepth);
}

//---------------------------------------------------------------------------------
static void newRow() {
//---------------------------------------------------------------------------------
	int i;
	row++;
	if(row >= CONSOLE_HEIGHT) {
		row--;

		for(i = CONSOLE_WIDTH; i < CONSOLE_HEIGHT * CONSOLE_WIDTH; i++) fontMap[i - CONSOLE_WIDTH] = fontMap[i];

		for(i = 0; i < CONSOLE_WIDTH; i++) fontMap[i + (CONSOLE_HEIGHT-1)*CONSOLE_WIDTH] = fontPal | (u16)(' ' + fontOffset - fontStart);

	}
}


//---------------------------------------------------------------------------------
void consolePrintChar(char c) {
//---------------------------------------------------------------------------------

	if(col >= CONSOLE_WIDTH) {
		col = 0;

		newRow();
	}

	switch(c) {

	case 10:
	case 11:
	case 12:
	case 13:
		newRow();
		col = 0;
		break;
	case 9:
		col += TAB_SIZE;
		break;
	default:
		fontMap[col + row * CONSOLE_WIDTH] = fontPal | (u16)(c + fontOffset - fontStart);
		col++;
		break;

	}
}

//---------------------------------------------------------------------------------
void consoleClear(void) {
//---------------------------------------------------------------------------------
	iprintf("\x1b[2J");
}


