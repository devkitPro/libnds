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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <calico/dev/disc_io.h>
#include <calico/dev/dldi.h>

typedef DldiHeader DLDI_INTERFACE;

/// Determine if a given DLDI driver structure is valid
bool dldiIsValid(const DLDI_INTERFACE* io);

/// Find a DLDI driver area within a given binary
DLDI_INTERFACE* dldiFindDriverArea(void* bin, size_t bin_size);

/// Adjust the pointer addresses within a DLDI driver for the target virtual address
void dldiFixDriverAddresses(DLDI_INTERFACE* io, uptr dldi_vaddr);

/// Patch a DLDI driver into a given area
bool dldiApplyPatch(DLDI_INTERFACE* area, const DLDI_INTERFACE* io);

/// Patch a binary using a given DLDI driver
MK_INLINE bool dldiPatchBinary(void* bin, size_t bin_size, const DLDI_INTERFACE* io)
{
	DLDI_INTERFACE* area = dldiFindDriverArea(bin, bin_size);
	return area ? dldiApplyPatch(area, io) : false;
}

#ifdef __cplusplus
}
#endif
