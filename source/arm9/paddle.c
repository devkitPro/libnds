#include <nds.h>
#include <nds/memory.h>

#define EXMEMCNT_MASK_SLOT2_ARM7 (1<<7)
#define EXMEMCNT_MASK_SLOT2_SRAM_TIME (3)
#define EXMEMCNT_SLOT2_SRAM_TIME_3 (3)
#define EXMEMCNT_MASK_SLOT2_CLOCKRATE (3<<5)
#define EXMEMCNT_SLOT2_CLOCKRATE_1 (1<<5)

//------------------------------------------------------------------------------
static void paddleSetBus() {
//------------------------------------------------------------------------------
	//setting the bus owner is not sufficient, as we need to ensure that the bus speeds are adequately slowed
	REG_EXMEMCNT &= ~EXMEMCNT_MASK_SLOT2_ARM7; //give ARM9 access
	REG_EXMEMCNT &= ~EXMEMCNT_MASK_SLOT2_SRAM_TIME;
	REG_EXMEMCNT |= EXMEMCNT_SLOT2_SRAM_TIME_3; //make sure SRAM is slow enough
	REG_EXMEMCNT &= ~EXMEMCNT_MASK_SLOT2_CLOCKRATE;
	REG_EXMEMCNT |= EXMEMCNT_SLOT2_CLOCKRATE_1; //a nonzero value for this is required for correct functioning. arkanoid uses 1. it is uncertain what other values will do.
}

//------------------------------------------------------------------------------
bool paddleIsInserted() {
//------------------------------------------------------------------------------
	paddleSetBus();

	//This is 0x96h is a GBA game is inserted
	if(GBA_HEADER.is96h == 0x96) return false;

	//paddle signifies itself this way
	if(*(vu16*)0x08000000 != 0xEFFF) return false;

	//let's check one more way just to be safe
	if(*(vu16*)0x0A000002 != 0x0000) return false;

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
	
