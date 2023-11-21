/*------------------------------------------------------------------------------

dldi.h

Copyright (c) 2006 Michael Chisholm (Chishm)

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.
2.	Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.
3.	This notice may not be removed or altered from any source distribution.

------------------------------------------------------------------------------*/


#ifndef NDS_DLDI_INCLUDE
#define NDS_DLDI_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include "../disc_io.h"
#include "dldi_asm.h"
#define FIX_ALL						0x01
#define FIX_GLUE					0x02
#define FIX_GOT						0x04
#define FIX_BSS						0x08

#define DLDI_MAGIC_STRING_LEN 		8
#define DLDI_FRIENDLY_NAME_LEN 		48

extern const u32  DLDI_MAGIC_NUMBER;

// I/O interface with DLDI extensions
typedef struct DLDI_INTERFACE {
	u32 	magicNumber;
	char	magicString [DLDI_MAGIC_STRING_LEN];
	u8		versionNumber;
	u8		driverSize;			// log-2 of driver size in bytes
	u8		fixSectionsFlags;
	u8		allocatedSize;		// log-2 of the allocated space in bytes

	char	friendlyName [DLDI_FRIENDLY_NAME_LEN];
	
	// Pointers to sections that need address fixing
	void*	dldiStart;
	void*	dldiEnd;
	void*	interworkStart;
	void*	interworkEnd;
	void*	gotStart;
	void*	gotEnd;
	void*	bssStart;
	void*	bssEnd;
	
	// Original I/O interface data
	DISC_INTERFACE ioInterface;
} DLDI_INTERFACE;


/*
Pointer to the internal DLDI, not directly usable by libfat.
You'll need to set the bus permissions appropriately before using.
*/
extern const DLDI_INTERFACE* io_dldi_data;

/*
Return a pointer to the internal IO interface, 
setting up bus permissions in the process.
*/
extern const DISC_INTERFACE* dldiGetInternal (void);

/*
Determines if an IO driver is a valid DLDI driver
*/
extern bool dldiIsValid (const DLDI_INTERFACE* io);

/* 
Adjust the pointer addresses within a DLDI driver
*/
extern void dldiFixDriverAddresses (DLDI_INTERFACE* io);

/*
Load a DLDI from disc and set up the bus permissions.
This returns a type not directly usable in libfat,
but it does give extra information, such as a friendly name.
To use in libfat:
const DLDI_INTERFACE* loadedDldi = dldi_loadFromFile ("file");
loadedDldi->ioInterface.startup();
fatMount(&loadedDldi->ioInterface, "devname", 0);
*/
extern DLDI_INTERFACE* dldiLoadFromFile (const char* path);

/* 
Free resources used by a loaded DLDI. 
Remember to unmount and shutdown first:
fatUnmount("devname");
loadedDldi->ioInterface.shutdown();
dldiFree(loadedDldi);
*/
extern void dldiFree (DLDI_INTERFACE* dldi);

#ifdef __cplusplus
}
#endif
#endif // NDS_DLDI_INCLUDE
