#include <nds.h>
#include <nds/memory.h>

static u8 guitar_keys = 0;
static u8 guitar_keys_old = 0;

//------------------------------------------------------------------------------
static void guitarGripSetBus() {
//------------------------------------------------------------------------------
	sysSetCartOwner(BUS_OWNER_ARM9);
}

//------------------------------------------------------------------------------
bool guitarGripIsInserted() {
//------------------------------------------------------------------------------
	guitarGripSetBus();

	//This is 0x96h is a GBA game is inserted
	if(GBA_HEADER.is96h == 0x96) return false;

	//guitar grip signifies itself this way
	if(*(vu16*)0x08000000 != 0xF9FF) return false;

	return true;
}

//------------------------------------------------------------------------------
void guitarGripScanKeys() {
//------------------------------------------------------------------------------
  guitarGripSetBus();
	guitar_keys_old = guitar_keys;
	guitar_keys = ~(*(vu8*)0x0A000000);
}

//------------------------------------------------------------------------------
u8 guitarGripKeysHeld() {
//------------------------------------------------------------------------------
	return guitar_keys;
}

//------------------------------------------------------------------------------
u16 guitarGripKeysDown() {
//------------------------------------------------------------------------------
	return guitar_keys & ~guitar_keys_old;
}

//------------------------------------------------------------------------------
u16 guitarGripKeysUp() {
//------------------------------------------------------------------------------
	return (guitar_keys ^ guitar_keys_old) & ~guitar_keys;
}

