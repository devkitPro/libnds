#include <nds/disc_io.h>
#include <nds/fifocommon.h>
#include <nds/fifomessages.h>
#include <nds/system.h>
#include <nds/arm9/cache.h>

//---------------------------------------------------------------------------------
bool sdio_Startup() {
//---------------------------------------------------------------------------------
	if (!REG_DSIMODE) return false;

	fifoSendValue32(FIFO_SYSTEM,SYS_HAVE_SD);
	while(!fifoCheckValue32(FIFO_SYSTEM));
	int result = fifoGetValue32(FIFO_SYSTEM);

	if(result==0) return false;
	
	// calling arm7 init code makes the SD fail
	// this means non SDHC only support for now
	return true;

	fifoSendValue32(FIFO_SYSTEM,SYS_SD_START);
	result = fifoGetValue32(FIFO_SYSTEM);
	
	return result == 0;
}

//---------------------------------------------------------------------------------
bool sdio_IsInserted() {
//---------------------------------------------------------------------------------
	if (!REG_DSIMODE) return false;
	return true;
}
 
//---------------------------------------------------------------------------------
bool sdio_ReadSectors(sec_t sector, sec_t numSectors,void* buffer) {
//---------------------------------------------------------------------------------
	if (!REG_DSIMODE) return false;
	FifoMessage msg;

	DC_FlushRange(buffer,numSectors * 512);

	msg.type = SYS_SD_READ_SECTORS;
	msg.sdParams.startsector = sector;
	msg.sdParams.numsectors = numSectors;
	msg.sdParams.buffer = buffer;
	
	fifoSendDatamsg(FIFO_SYSTEM, sizeof(msg), (u8*)&msg);

	while(!fifoCheckValue32(FIFO_SYSTEM));

	int result = fifoGetValue32(FIFO_SYSTEM);
	
	return result == 0;
}

//---------------------------------------------------------------------------------
bool sdio_WriteSectors(sec_t sector, sec_t numSectors,const void* buffer) {
//---------------------------------------------------------------------------------
	if (!REG_DSIMODE) return false;
	FifoMessage msg;

	DC_FlushRange(buffer,numSectors * 512);

	msg.type = SYS_SD_WRITE_SECTORS;
	msg.sdParams.startsector = sector;
	msg.sdParams.numsectors = numSectors;
	msg.sdParams.buffer = (void*)buffer;
	
	fifoSendDatamsg(FIFO_SYSTEM, sizeof(msg), (u8*)&msg);

	while(!fifoCheckValue32(FIFO_SYSTEM));

	int result = fifoGetValue32(FIFO_SYSTEM);
	
	return result == 0;
}


//---------------------------------------------------------------------------------
bool sdio_ClearStatus() {
//---------------------------------------------------------------------------------
	if (!REG_DSIMODE) return false;
	return true;
}

//---------------------------------------------------------------------------------
bool sdio_Shutdown() {
//---------------------------------------------------------------------------------
	if (!REG_DSIMODE) return false;
	return true;
}

const DISC_INTERFACE __io_dsisd = {
	DEVICE_TYPE_DSI_SD,
	FEATURE_MEDIUM_CANREAD,
	(FN_MEDIUM_STARTUP)&sdio_Startup,
	(FN_MEDIUM_ISINSERTED)&sdio_IsInserted,
	(FN_MEDIUM_READSECTORS)&sdio_ReadSectors,
	(FN_MEDIUM_WRITESECTORS)&sdio_WriteSectors,
	(FN_MEDIUM_CLEARSTATUS)&sdio_ClearStatus,
	(FN_MEDIUM_SHUTDOWN)&sdio_Shutdown
};
