#include <nds/memory.h>
#include <nds/arm9/piano.h>

static u16 piano_keys = 0;
static u16 piano_keys_old = 0;


//------------------------------------------------------------------------------
bool pianoIsInserted() {
//------------------------------------------------------------------------------
	sysSetCartOwner(BUS_OWNER_ARM9);
	
	// This is 0x96h is a GBA game is inserted
	if(GBA_HEADER.is96h == 0x96) return 0;

	// When reading from GBA cart space without a GBA cart inserted
	// the values you read are 16-bit unsigned integers containing
	// (Address / 2), cp.
	// http://nocash.emubase.de/gbatek.htm#unpredictablethings.
	// The DS piano pulls down GBA pins A4 and A5
	// (cp. http://darkfader.net/gba/files/cartridge.txt), so
	// bits 4 and 5 are always 0. Thus, when the keyboard is
	// inserted, ANDing a value read from GBA cart space with 0x1800
	// must yield 0.
	return ( (PIANO_PAK & 0x1800 ) == 0 );
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

