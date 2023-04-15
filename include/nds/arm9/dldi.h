/*
 dldi.h

 Copyright (c) 2006 Michael "Chishm" Chisholm
	
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.
  3. The name of the author may not be used to endorse or promote products derived
     from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef NDS_DLDI_INCLUDE
#define NDS_DLDI_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include "../disc_io.h"
#include <calico/dev/dldi.h>

#define FIX_ALL						DLDI_FIX_ALL
#define FIX_GLUE					DLDI_FIX_GLUE
#define FIX_GOT						DLDI_FIX_GOT
#define FIX_BSS						DLDI_FIX_BSS

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
