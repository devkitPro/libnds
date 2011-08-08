#include <nds/system.h>
#include <nds/bios.h>
#include <nds/arm7/sdmmc.h>
#include <nds/interrupts.h>
#include <nds/fifocommon.h>
#include <nds/fifomessages.h>

u32 sdmmc_cid[4];
vu32 sdmmc_cardready = 0;

static int sdmmc_timeout = 0;
static int sdmmc_gotcmd8reply = 0;
static int sdmmc_sdhc = 0;

static vu32 sdmmcirq_transfer = 0;
static u16 *sdmmcirq_buffer = 0;
static int sdmmcirq_bufpos = 0;
static u16 sdmmcirq_blocks = 0;
static int sdmmcirq_oldirq = 0;
static int sdmmcirq_abort = 0;

static void set_irqhandler(vu32 rw, u16 *buf, u16 num_sectors);
static int wait_irqhandlerfinish();
static void sdmmc_irqhandler();

//---------------------------------------------------------------------------------
int sdmmc_send_command(u16 cmd, u16 arg0, u16 arg1) {
//---------------------------------------------------------------------------------
	u16 is_stat0, was_stat0;
	u16 is_stat1, was_stat1;

	sdmmc_write16(REG_SDCMDARG0, arg0);
	sdmmc_write16(REG_SDCMDARG1, arg1);

	was_stat1 = sdmmc_read16(REG_SDSTATUS1);

	while(sdmmc_read16(REG_SDSTATUS1) & 0x4000);

	is_stat1 = sdmmc_read16(REG_SDSTATUS1);

	sdmmc_mask16(REG_SDSTATUS1, 0x807F, 0);
	sdmmc_mask16(REG_SDSTATUS0, 0x0001, 0);
	sdmmc_mask16(REG_SDSTATUS0, 0x0004, 0);
	sdmmc_mask16(REG_SDSTATUS1, 0x100, 0);
	sdmmc_mask16(REG_SDSTATUS1, 0x200, 0);
	sdmmc_mask16(REG_SDSTOP, 1, 0);
	sdmmc_mask16(0x22, 0x807F, 0);
	sdmmc_mask16(0x20, 0x4, 0);
	sdmmc_timeout = 0;

	if(cmd==17 || cmd==18) {
		if((sdmmc_read16(0x100) & 2)==0)sdmmc_mask16(0x22, 0x100, 0);
	}
	if(cmd==24 || cmd==25) {
		if((sdmmc_read16(0x100) & 2)==0)sdmmc_mask16(0x22, 0x200, 0);
	}

	sdmmc_write16(REG_SDCMD, cmd);

	if (cmd != 0) {
		was_stat0 = sdmmc_read16(REG_SDSTATUS0);

		if (cmd != 0x5016) {
			while((sdmmc_read16(REG_SDSTATUS0) & 5) == 0) {
				if(sdmmc_read16(REG_SDSTATUS1) & 0x40) {
					sdmmc_mask16(REG_SDSTATUS1, 0x40, 0);
					sdmmc_timeout = 1;
					break;
				}
			}
		}

		is_stat0 = sdmmc_read16(REG_SDSTATUS0);
		sdmmc_mask16(REG_SDSTATUS0, 5, 0);
	}

	if(cmd!=17 && cmd!=18 && cmd!=24 && cmd!=25)sdmmc_mask16(0x22, 0, 0x807F);

	return 0;
}

//---------------------------------------------------------------------------------
int sdmmc_cardinserted() {
//---------------------------------------------------------------------------------
	return sdmmc_cardready;
}

//---------------------------------------------------------------------------------
void sdmmc_init_irq() {
//---------------------------------------------------------------------------------
	set_irqhandler(0, 0, 0);
	irqSetAUX(IRQ_SDMMC, sdmmc_irqhandler);
	irqEnableAUX(IRQ_SDMMC);
}

//---------------------------------------------------------------------------------
void sdmmc_controller_init() {
//---------------------------------------------------------------------------------
	int oldIME = enterCriticalSection();
	// Reset
	sdmmc_write16(0x100, 0x0402);
	sdmmc_write16(0x100, 0x0000);
	sdmmc_write16(0x104, 0x0000);
	sdmmc_write16(0x108, 0x0001);

	// InitIP
	sdmmc_mask16(REG_SDRESET, 0x0001, 0x0000);
	sdmmc_mask16(REG_SDRESET, 0x0000, 0x0001);
	sdmmc_mask16(REG_SDSTOP, 0x0001, 0x0000);

	// Reset
	sdmmc_mask16(REG_SDOPT, 0x0005, 0x0000);

	// EnableInfo
	sdmmc_mask16(REG_SDSTATUS0, 0x0018, 0x0000);
	sdmmc_mask16(0x20, 0x0018, 0x0000); // ??
	sdmmc_mask16(0x20, 0x0000, 0x0018);

	sdmmc_init_irq();
	leaveCriticalSection(oldIME);
}


//---------------------------------------------------------------------------------
static void set_irqhandler(vu32 rw, u16 *buf, u16 num_sectors) {
//---------------------------------------------------------------------------------
	sdmmcirq_buffer = buf;
	sdmmcirq_blocks = num_sectors;
	sdmmcirq_transfer = rw;

	sdmmcirq_oldirq = REG_IME;
	REG_IME = 1;
}


//---------------------------------------------------------------------------------
static int wait_irqhandlerfinish() {
//---------------------------------------------------------------------------------
	int ret;
	while(sdmmcirq_transfer);
	REG_IME = sdmmcirq_oldirq;

	ret = sdmmcirq_abort;
	sdmmcirq_abort = 0;
	return ret;
}

//---------------------------------------------------------------------------------
static void sdmmc_irqhandler() {
//---------------------------------------------------------------------------------
	int i;
	//u32 irqs;
	u32 status;
	u32 errstat;
	if(sdmmcirq_transfer==0)return;

	//irqs = sdmmc_read16(0x100);
	status = sdmmc_read16(REG_SDSTATUS0);

	errstat = sdmmc_read16(REG_SDSTATUS1) & 0x807f;//STATUS errors mask
	if(errstat) {
		sdmmc_mask16(REG_SDSTATUS1, errstat, 0);
		//sdmmc_mask16(0x22, 0x0, errstat);

		sdmmcirq_transfer = 0;
		sdmmcirq_bufpos = 0;
		sdmmcirq_abort = 1;
		sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);
		fifoSendValue32(FIFO_SDMMC, 1);
		return;
	}

	if(sdmmcirq_blocks==0) {
		if(status & 0x4) {
			sdmmcirq_transfer = 0;
			sdmmcirq_bufpos = 0;

			sdmmc_mask16(REG_SDSTATUS0, 0x4, 0);

			sdmmc_mask16(0x22, 0, 0x807F | 0x4000);
			sdmmc_mask16(0x20, 0, 0x4);
			if((sdmmc_read16(0x22) & 0x100)==0)sdmmc_mask16(0x22, 0, 0x100);
			if((sdmmc_read16(0x22) & 0x200)==0)sdmmc_mask16(0x22, 0, 0x200);
		}
		sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);
		fifoSendValue32(FIFO_SDMMC, 0);
		return;
	}

	if(sdmmcirq_transfer==1) {
		sdmmc_mask16(REG_SDSTATUS1, 0x100, 0);
		//if((sdmmc_read16(0x22) & 0x100))sdmmc_mask16(0x22, 0x100, 0x0);
	}
	if(sdmmcirq_transfer==2) {
		sdmmc_mask16(REG_SDSTATUS1, 0x200, 0);
		//if((sdmmc_read16(0x22) & 0x200))sdmmc_mask16(0x22, 0x200, 0x0);
		if(sdmmcirq_blocks==1)
		{
			sdmmc_mask16(0x100, 0x1800, 0x0);
			sdmmc_mask16(0x22, 0x0, 0x100);
			sdmmc_mask16(0x22, 0x0, 0x200);
		}
	}

	for(i=0; i<0x100; i++) {
		if(sdmmcirq_transfer==1)sdmmcirq_buffer[sdmmcirq_bufpos] = sdmmc_read16(REG_SDFIFO);
		if(sdmmcirq_transfer==2)sdmmc_write16(REG_SDFIFO, sdmmcirq_buffer[sdmmcirq_bufpos]);
		sdmmcirq_bufpos++;
	}

	sdmmcirq_blocks--;
}

//---------------------------------------------------------------------------------
void sdmmc_send_acmd41(u32 arg, u32 *resp) {
//---------------------------------------------------------------------------------
	sdmmc_send_command(55, 0x0000, 0x000);
	*resp = sdmmc_read16(REG_SDRESP0) | (sdmmc_read16(REG_SDRESP1)<<16);

	sdmmc_send_command(41, (u16)arg, (arg>>16));
	
	*resp = sdmmc_read16(REG_SDRESP0) | (sdmmc_read16(REG_SDRESP1)<<16);
}

//---------------------------------------------------------------------------------
int sdmmc_sdcard_init() {
//---------------------------------------------------------------------------------
	u16 sdaddr;
	u32 resp0;
	u32 resp1;
	u32 resp2;
	u32 resp3;
	u32 resp4;
	u32 resp5;
	u32 resp6;
	u32 resp7;

	u32 ocr, ccs, hcs, response;

	sdmmc_cardready = 0;

	int oldIME = enterCriticalSection();

	sdmmc_write16(REG_SDCLKCTL, 0x20);
	sdmmc_write16(REG_SDOPT, 0x40EA); // XXX: document me!
	sdmmc_write16(0x02, 0x400);

	sdmmc_mask16(REG_SDCLKCTL, 0, 0x100);
	sdmmc_write16(REG_SDCLKCTL, sdmmc_read16(REG_SDCLKCTL));
	sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);
	sdmmc_mask16(REG_SDCLKCTL, 0, 0x200);
	sdmmc_mask16(REG_SDCLKCTL, 0, 0x100);

	// CMD0
	sdmmc_send_command(0x0000, 0x0000, 0x0000);
	sdmmc_send_command(0x0408, 0x01aa, 0x0000);
	sdmmc_gotcmd8reply = 1;

	if(sdmmc_timeout)
		sdmmc_gotcmd8reply = 0;

	if(sdmmc_gotcmd8reply)
		hcs = (1<<30);
	else
		hcs = 0;

	ocr = 0x00ff8000;

	while (1) {
		sdmmc_send_acmd41(ocr | hcs, &response);

		if (response & 0x80000000)
			break;
	}

	ccs = response & (1<<30);

	if(ccs && hcs) 
		sdmmc_sdhc = 1;
	else
		sdmmc_sdhc = 0;

	// CMD2 - get CID
	sdmmc_send_command(2, 0, 0);
	
	resp0 = sdmmc_read16(REG_SDRESP0);
	resp1 = sdmmc_read16(REG_SDRESP1);
	resp2 = sdmmc_read16(REG_SDRESP2);
	resp3 = sdmmc_read16(REG_SDRESP3);
	resp4 = sdmmc_read16(REG_SDRESP4);
	resp5 = sdmmc_read16(REG_SDRESP5);
	resp6 = sdmmc_read16(REG_SDRESP6);
	resp7 = sdmmc_read16(REG_SDRESP7);

	sdmmc_cid[0] = resp0 | (u32)(resp1<<16);
	sdmmc_cid[1] = resp2 | (u32)(resp3<<16);
	sdmmc_cid[2] = resp4 | (u32)(resp5<<16);
	sdmmc_cid[3] = resp6 | (u32)(resp7<<16);

	// CMD3
	sdmmc_send_command(3, 0, 0);
	
	resp0 = sdmmc_read16(REG_SDRESP0);
	resp1 = sdmmc_read16(REG_SDRESP1);
	
	sdaddr = resp1;
	
	// CMD9 - get CSD
	sdmmc_send_command(9, 0, sdaddr);
	
	resp0 = sdmmc_read16(REG_SDRESP0);
	resp1 = sdmmc_read16(REG_SDRESP1);
	resp2 = sdmmc_read16(REG_SDRESP2);
	resp3 = sdmmc_read16(REG_SDRESP3);
	resp4 = sdmmc_read16(REG_SDRESP4);
	resp5 = sdmmc_read16(REG_SDRESP5);
	resp6 = sdmmc_read16(REG_SDRESP6);
	resp7 = sdmmc_read16(REG_SDRESP7);
	
	sdmmc_write16(REG_SDCLKCTL, 0x100);	
	
	// CMD7
	sdmmc_send_command(7, 0, sdaddr);
	
	resp0 = sdmmc_read16(REG_SDRESP0);
	resp1 = sdmmc_read16(REG_SDRESP1);
	
	// CMD55
	sdmmc_send_command(55, 0, sdaddr);
	
	// ACMD6
	sdmmc_send_command(6, 2, 0);
	
	resp0 = sdmmc_read16(REG_SDRESP0);
	resp1 = sdmmc_read16(REG_SDRESP1);

	sdmmc_send_command(13, 0, sdaddr);
	
	resp0 = sdmmc_read16(REG_SDRESP0);
	resp1 = sdmmc_read16(REG_SDRESP1);

	sdmmc_send_command(16, 0x200, 0x0);
	
	resp0 = sdmmc_read16(REG_SDRESP0);
	resp1 = sdmmc_read16(REG_SDRESP1);	
	
	sdmmc_write16(REG_SDCLKCTL, sdmmc_read16(REG_SDCLKCTL));
	sdmmc_write16(REG_SDBLKLEN, 0x200);

	sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);

	sdmmc_cardready = 1;
	leaveCriticalSection(oldIME);

	return 0;
}

//---------------------------------------------------------------------------------
void sdmmc_sdcard_readsector(u32 sector_no, void *out) {
//---------------------------------------------------------------------------------
	u16 *out16 = (u16*)out;
//	int ret;

	if(!sdmmc_sdhc)
		sector_no *= 512;

	sdmmc_mask16(0x100, 2, 0);
	sdmmc_write16(0xd8, 0);

	sdmmc_mask16(REG_SDCLKCTL, 0, 0x100);
	sdmmc_write16(REG_SDBLKLEN, 0x200);
	
	set_irqhandler(1, out16, 1);

	// CMD17 - read single block
	sdmmc_send_command(17, sector_no & 0xffff, (sector_no >> 16));
	if(sdmmc_timeout) {
		sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);
		fifoSendValue32(FIFO_SDMMC, 1);
	}
}

//---------------------------------------------------------------------------------
void sdmmc_sdcard_readsectors(u32 sector_no, u32 numsectors, void *out) {
//---------------------------------------------------------------------------------
	u16 *out16 = (u16*)out;
//	int ret;

	if(numsectors==1) {
		return sdmmc_sdcard_readsector(sector_no, out);
	}

	if(!sdmmc_sdhc)
		sector_no *= 512;

	sdmmc_mask16(0x100, 2, 0);
	sdmmc_write16(0xd8, 0);

	sdmmc_write16(REG_SDSTOP, 0x100);
	sdmmc_write16(REG_SDBLKCOUNT, numsectors);
	sdmmc_mask16(REG_SDCLKCTL, 0, 0x100);
	sdmmc_write16(REG_SDBLKLEN, 0x200);	
	
	set_irqhandler(1, out16, numsectors);

	// CMD18 - read multiple blocks
	sdmmc_send_command(18, sector_no & 0xffff, (sector_no >> 16));
	if(sdmmc_timeout) {
		sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);
		fifoSendValue32(FIFO_SDMMC, 1);
	}
}

//---------------------------------------------------------------------------------
void sdmmc_sdcard_writesector(u32 sector_no, void *in) {
//---------------------------------------------------------------------------------
	u16 *in16 = (u16*)in;
//	int ret;

	if(!sdmmc_sdhc)
		sector_no *= 512;

	sdmmc_mask16(0x100, 2, 0);
	sdmmc_write16(0xd8, 0);

	sdmmc_mask16(REG_SDCLKCTL, 0, 0x100);
	sdmmc_write16(REG_SDBLKLEN, 0x200);	
	
	set_irqhandler(2, in16, 1);

	// CMD24 - write single block
	sdmmc_send_command(24, sector_no & 0xffff, (sector_no >> 16));
	if(sdmmc_timeout) {
		sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);
		fifoSendValue32(FIFO_SDMMC, 1);
	}

}

//---------------------------------------------------------------------------------
void sdmmc_sdcard_writesectors(u32 sector_no, u32 numsectors, void *in) {
//---------------------------------------------------------------------------------
	u16 *in16 = (u16*)in;

	if(numsectors==1) {
		return sdmmc_sdcard_writesector(sector_no, in);
	}

	if(!sdmmc_sdhc)
		sector_no *= 512;

	sdmmc_mask16(0x100, 2, 0);
	sdmmc_write16(0xd8, 0);

	sdmmc_write16(REG_SDSTOP, 0x100);
	sdmmc_write16(REG_SDBLKCOUNT, numsectors);
	sdmmc_mask16(REG_SDCLKCTL, 0, 0x100);
	sdmmc_write16(REG_SDBLKLEN, 0x200);	
	
	set_irqhandler(2, in16, numsectors);

	// CMD25 - write multiple blocks
	sdmmc_send_command(25, sector_no & 0xffff, (sector_no >> 16));
	if(sdmmc_timeout) {
		sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);
		fifoSendValue32(FIFO_SDMMC, 1);
	}
}

//---------------------------------------------------------------------------------
void sdmmcMsgHandler(int bytes, void *user_data) {
//---------------------------------------------------------------------------------
	FifoMessage msg;
	int retval;

	fifoGetDatamsg(FIFO_SDMMC, bytes, (u8*)&msg);
	
	switch (msg.type) {

	case SDMMC_SD_READ_SECTORS:
		sdmmc_sdcard_readsectors(msg.sdParams.startsector, msg.sdParams.numsectors, msg.sdParams.buffer);
		break;
	case SDMMC_SD_WRITE_SECTORS:
		sdmmc_sdcard_writesectors(msg.sdParams.startsector, msg.sdParams.numsectors, msg.sdParams.buffer);
		break;
	
	}
}

//---------------------------------------------------------------------------------
void sdmmcValueHandler(u32 value, void* user_data) {
//---------------------------------------------------------------------------------
	int result;
	
	switch(value) {

	case SDMMC_HAVE_SD:
		result = sdmmc_read16(REG_SDSTATUS0);
		fifoSendValue32(FIFO_SDMMC, result);
		break;

	case SDMMC_SD_START:
		if (sdmmc_read16(REG_SDSTATUS0) == 0) {
			result = 1;
		} else {
			sdmmc_controller_init();
			result = sdmmc_sdcard_init();
		}
		fifoSendValue32(FIFO_SDMMC, result);
		break;

	case SDMMC_SD_IS_INSERTED:
		result = sdmmc_cardinserted();
		fifoSendValue32(FIFO_SDMMC, result);
		break;

	case SDMMC_SD_STOP:
		break;

	}
}


