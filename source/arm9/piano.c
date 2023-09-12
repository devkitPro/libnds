#include <nds/memory.h>
#include <nds/system.h>
#include <nds/arm9/piano.h>
#include <calico/nds/gbacart.h>

static u16 piano_keys = 0;
static u16 piano_keys_old = 0;

//------------------------------------------------------------------------------
bool pianoIsInserted() {
//------------------------------------------------------------------------------
	if (!gbacartIsOpen()) return false;

	// This is 0x96h is a GBA game is inserted
	if(GBA_HEADER.is96h == 0x96) return false;

	//piano signifies itself this way (AD11/AD12 pulled low)
	if(*(vu16*)0x08000000 != 0xE7FF) return false;

	return true;
}

//------------------------------------------------------------------------------
void pianoScanKeys() {
//------------------------------------------------------------------------------
	piano_keys_old = piano_keys;
	piano_keys = ~PIANO_PAK;
}

//------------------------------------------------------------------------------
u16 pianoKeysHeld() {
//------------------------------------------------------------------------------
	return piano_keys;
}

//------------------------------------------------------------------------------
u16 pianoKeysDown() {
//------------------------------------------------------------------------------
	return piano_keys & ~piano_keys_old;
}

//------------------------------------------------------------------------------
u16 pianoKeysUp() {
//------------------------------------------------------------------------------
	return (piano_keys ^ piano_keys_old) & ~piano_keys;
}
