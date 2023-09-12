#include <nds/memory.h>
#include <nds/system.h>
#include <nds/arm9/paddle.h>
#include <calico/nds/gbacart.h>

//------------------------------------------------------------------------------
static void paddleSetBus() {
//------------------------------------------------------------------------------
	//setting the bus owner is not sufficient, as we need to ensure that the bus speeds are adequately slowed
	gbacartSetTiming(GBA_WAIT_SRAM_MASK | GBA_PHI_MASK,
		GBA_WAIT_SRAM_18 | //make sure SRAM is slow enough
		GBA_PHI_4_19 //a nonzero value for this is required for correct functioning. arkanoid uses 1. it is uncertain what other values will do.
	);
}

//------------------------------------------------------------------------------
bool paddleIsInserted() {
//------------------------------------------------------------------------------
	if(!gbacartIsOpen()) return false;

	//This is 0x96h is a GBA game is inserted
	if(GBA_HEADER.is96h == 0x96) return false;

	//paddle signifies itself this way (AD12 pulled low)
	if(*(vu16*)0x08000000 != 0xEFFF) return false;

	return true;
}

//------------------------------------------------------------------------------
u16 paddleRead() {
//------------------------------------------------------------------------------
 	paddleSetBus();
	return (*(vu8*)0x0A000000) | ((*(vu8*)0x0A000001)<<8);
}

//------------------------------------------------------------------------------
void paddleReset() {
//------------------------------------------------------------------------------
	(*(vu8*)0x0A000000) = 0;
}
