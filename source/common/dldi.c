/*
 dldi.c

 Copyright (c) 2006 Michael "Chishm" Chisholm
 Copyright (c) 2023 fincs

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

#include <calico/arm/common.h>
#include <nds/dldi.h>

// Stored backwards to prevent it being picked up by DLDI patchers
static const char s_dldiMagicBackwards[DLDI_MAGIC_STRING_LEN] = {
	'\0', 'm', 'h', 's', 'i', 'h', 'C', ' '
};

bool dldiIsValid(const DLDI_INTERFACE* io)
{
	if (io->magic_num != DLDI_MAGIC_VAL) {
		return false;
	}

	for (unsigned i = 0; i < DLDI_MAGIC_STRING_LEN; i++) {
		if (io->magic_str[i] != s_dldiMagicBackwards[DLDI_MAGIC_STRING_LEN-1-i]) {
			return false;
		}
	}

	return true;
}

DLDI_INTERFACE* dldiFindDriverArea(void* bin, size_t bin_size)
{
	bin_size -= sizeof(DLDI_INTERFACE)-1;
	for (size_t i = 0; i < bin_size; i += 4) {
		// Check if the current address is a valid DLDI driver
		DLDI_INTERFACE* io = (DLDI_INTERFACE*)((u8*)bin + i);
		if (dldiIsValid(io)) {
			// Ensure the claimed DLDI header doesn't go out of bounds
			size_t rem_sz = bin_size - i;
			if (rem_sz >= (1U << io->alloc_sz_log2)) {
				return io;
			}
		}
	}

	return NULL;
}

static void _dldiRelocateRegion(DLDI_INTERFACE* io, uptr start_vaddr, uptr end_vaddr, sptr adjustment)
{
	uptr dldi_start_vaddr = io->dldi_start;
	uptr dldi_end_vaddr   = io->dldi_end;

	uptr* start = (uptr*)(start_vaddr - dldi_start_vaddr + (uptr)io);
	uptr* end   = (uptr*)(end_vaddr   - dldi_start_vaddr + (uptr)io);

	for (uptr* cur = start; cur != end; cur ++) {
		uptr addr = *cur + adjustment;
		if (addr >= dldi_start_vaddr && addr < dldi_end_vaddr) {
			*cur = addr;
		}
	}
}

void dldiFixDriverAddresses(DLDI_INTERFACE* io, uptr dldi_vaddr)
{
	sptr adjustment = dldi_vaddr - io->dldi_start;
	if (adjustment == 0) {
		return;
	}

	// Correct all pointers to the offsets from the location of this interface
	io->dldi_start += adjustment;
	io->dldi_end   += adjustment;
	io->glue_start += adjustment;
	io->glue_end   += adjustment;
	io->got_start  += adjustment;
	io->got_end    += adjustment;
	io->bss_start  += adjustment;
	io->bss_end    += adjustment;

	io->disc.startup      = (void*)((uptr)io->disc.startup      + adjustment);
	io->disc.isInserted   = (void*)((uptr)io->disc.isInserted   + adjustment);
	io->disc.readSectors  = (void*)((uptr)io->disc.readSectors  + adjustment);
	io->disc.writeSectors = (void*)((uptr)io->disc.writeSectors + adjustment);
	io->disc.clearStatus  = (void*)((uptr)io->disc.clearStatus  + adjustment);
	io->disc.shutdown     = (void*)((uptr)io->disc.shutdown     + adjustment);

	// Fix all addresses within the DLDI
	if (io->fix_flags & DLDI_FIX_ALL) {
		_dldiRelocateRegion(io, io->dldi_start, io->dldi_end, adjustment);
	} else {
		// Fix the interworking glue section
		if (io->fix_flags & DLDI_FIX_GLUE) {
			_dldiRelocateRegion(io, io->glue_start, io->glue_end, adjustment);
		}

		// Fix the global offset table section
		if (io->fix_flags & DLDI_FIX_GOT) {
			_dldiRelocateRegion(io, io->got_start, io->got_end, adjustment);
		}
	}

	// Clear the BSS
	if (io->fix_flags & DLDI_FIX_BSS) {
		armFillMem32((void*)(io->bss_start - io->dldi_start + (uptr)io), 0, io->bss_end - io->bss_start);
	}
}

bool dldiApplyPatch(DLDI_INTERFACE* area, const DLDI_INTERFACE* io)
{
	uptr target_vaddr = area->dldi_start;
	unsigned target_sz_log2 = area->alloc_sz_log2;

	if (target_sz_log2 < io->driver_sz_log2) {
		return false;
	}

	armCopyMem32(area, io, 1U << io->driver_sz_log2);
	area->alloc_sz_log2 = target_sz_log2;

	dldiFixDriverAddresses(area, target_vaddr);
	return true;
}
