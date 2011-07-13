#include <nds/system.h>
#include <nds/bios.h>
#include <nds/arm7/sdmmc.h>

int sdmmc_curdevice = -1;
u32 sdmmc_cid[4];
vu32 sdmmc_cardready = 0;

static int sdmmc_timeout = 0;
static int sdmmc_gotcmd8reply = 0;
static int sdmmc_sdhc = 0;

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

	sdmmc_mask16(0x1E, 0x807F, 0);
	sdmmc_mask16(0x1C, 0x0001, 0);
	sdmmc_mask16(0x1C, 0x0004, 0);
	sdmmc_mask16(0x22, 0x807F, 0);
	sdmmc_timeout = 0;

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
	}

	sdmmc_mask16(0x22, 0, 0x807F);

	return 0;
}

//---------------------------------------------------------------------------------
int sdmmc_cardinserted() {
//---------------------------------------------------------------------------------
	return sdmmc_cardready;
}

//---------------------------------------------------------------------------------
void sdmmc_controller_init() {
//---------------------------------------------------------------------------------
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
}

//---------------------------------------------------------------------------------
void sdmmc_send_acmd41(u32 arg, u32 *resp) {
//---------------------------------------------------------------------------------
	sdmmc_send_command(55, 0x0000, 0x000);
	*resp = sdmmc_read16(REG_SDRESP0) | (sdmmc_read16(REG_SDRESP1)<<16);

	sdmmc_send_command(41, (u16)arg, (arg>>16));

	while( (REG_DISPSTAT & DISPSTAT_CHK_VBLANK) == 0 );
	while( (REG_DISPSTAT & DISPSTAT_CHK_VBLANK) != 0 );
	
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

	sdmmc_write16(REG_SDCLKCTL, 0x20);
	sdmmc_write16(REG_SDOPT, 0x40EA); // XXX: document me!
	sdmmc_write16(0x02, 0x400);
	sdmmc_curdevice = 0;

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

	return 0;
}


//---------------------------------------------------------------------------------
void sdmmc_clkdelay0() {
//---------------------------------------------------------------------------------
	swiDelay(1330);
}

//---------------------------------------------------------------------------------
void sdmmc_clkdelay1() {
//---------------------------------------------------------------------------------
	// card dependent, may need tweaking
	swiDelay(16000);
}

//---------------------------------------------------------------------------------
void sdmmc_clkdelay() {
//---------------------------------------------------------------------------------
	swiDelay(1330);
}

//---------------------------------------------------------------------------------
int sdmmc_sdcard_readsector(u32 sector_no, void *out) {
//---------------------------------------------------------------------------------
	u16 *out16 = (u16*)out;
	u16 resp0, resp1;
	int i;

	if(!sdmmc_sdhc)
		sector_no *= 512;

	sdmmc_mask16(REG_SDCLKCTL, 0, 0x100);
	sdmmc_clkdelay();
	sdmmc_write16(REG_SDBLKLEN, 0x200);
	
	// CMD17 - read single block
	sdmmc_send_command(17, sector_no & 0xffff, (sector_no >> 16));
	if(sdmmc_timeout) {
		sdmmc_clkdelay1();
		sdmmc_clkdelay0();
		sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);
		return 1;
	}

	resp0 = sdmmc_read16(REG_SDRESP0);
	resp1 = sdmmc_read16(REG_SDRESP1);	

	resp0 = sdmmc_read16(REG_SDSTATUS1);

	while (!(sdmmc_read16(REG_SDSTATUS1) & 0x100));

	resp0 = sdmmc_read16(REG_SDSTATUS1);
	
	sdmmc_clkdelay1();

	sdmmc_mask16(REG_SDSTATUS1, 1, 0);
	
	for(i = 0; i < 0x100; i++) {
		out16[i] = sdmmc_read16(REG_SDFIFO);
	}

	sdmmc_clkdelay0();
	sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);

	return 0;
}

//---------------------------------------------------------------------------------
int sdmmc_sdcard_readsectors(u32 sector_no, int numsectors, void *out) {
//---------------------------------------------------------------------------------
	u16 *out16 = (u16*)out;
	u16 resp0, resp1;
	int i;

	if(!sdmmc_sdhc)
		sector_no *= 512;

	sdmmc_write16(REG_SDSTOP, 0x100);
	sdmmc_write16(REG_SDBLKCOUNT, numsectors);
	sdmmc_mask16(REG_SDCLKCTL, 0, 0x100);
	sdmmc_clkdelay();
	sdmmc_write16(REG_SDBLKLEN, 0x200);	
	
	// CMD18 - read multiple blocks
	sdmmc_send_command(18, sector_no & 0xffff, (sector_no >> 16));
	if(sdmmc_timeout) {
		sdmmc_clkdelay1();
		sdmmc_clkdelay0();
		sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);
		return 1;
	}

	resp0 = sdmmc_read16(REG_SDRESP0);
	resp1 = sdmmc_read16(REG_SDRESP1);	
	resp0 = sdmmc_read16(REG_SDSTATUS1);

	while (!(sdmmc_read16(REG_SDSTATUS1) & 0x100));

	resp0 = sdmmc_read16(REG_SDSTATUS1);
	
	sdmmc_clkdelay1();

	sdmmc_mask16(REG_SDSTATUS1, 1, 0);
	
	for(i = 0; i < 0x100*numsectors; i++) {
		out16[i] = sdmmc_read16(REG_SDFIFO);
	}

	sdmmc_clkdelay0();
	sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);

	return 0;
}

//---------------------------------------------------------------------------------
// This writing code is broken.
// Clk randomly dies, causing this code to hang.
// This has the potential to kill the FS if it hangs while writing FAT metadata etc, 
// do not enable this unless you know what you're doing!
//---------------------------------------------------------------------------------
#ifdef ENABLEWR
//---------------------------------------------------------------------------------
int sdmmc_sdcard_writesector(u32 sector_no, void *in) {
//---------------------------------------------------------------------------------
	u16 *in16 = (u16*)in;
	u16 resp0, resp1;
	int i;

	if(!sdmmc_sdhc)sector_no *= 512;

	sdmmc_clkdelay0();
	sdmmc_mask16(REG_SDCLKCTL, 0, 0x100);
	sdmmc_clkdelay();
	sdmmc_write16(REG_SDBLKLEN, 0x200);	
	
	// CMD24 - write single block
	sdmmc_send_command(24, sector_no & 0xffff, (sector_no >> 16));
	if(sdmmc_timeout) {
		sdmmc_clkdelay1();
		sdmmc_clkdelay0();
		sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);
		return 1;
	}

	resp0 = sdmmc_read16(REG_SDRESP0);
	resp1 = sdmmc_read16(REG_SDRESP1);	

	resp0 = sdmmc_read16(REG_SDSTATUS1);

	while (!(sdmmc_read16(REG_SDSTATUS1) & 0x200));

	resp0 = sdmmc_read16(REG_SDSTATUS1);

	sdmmc_clkdelay1();

	sdmmc_mask16(REG_SDSTATUS1, 1, 0);
	
	for(i = 0; i < 0x100; i++) {
		sdmmc_write16(REG_SDFIFO, in16[i]);
	}

	sdmmc_clkdelay0();
	sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);

	return 0;
}

//---------------------------------------------------------------------------------
int sdmmc_sdcard_writesectors(u32 sector_no, int numsectors, void *in) {
//---------------------------------------------------------------------------------
	u16 *in16 = (u16*)in;
	u16 resp0, resp1;
	int i;

	if(!sdmmc_sdhc)
		sector_no *= 512;

	sdmmc_write16(REG_SDSTOP, 0x100);
	sdmmc_write16(REG_SDBLKCOUNT, numsectors);
	sdmmc_mask16(REG_SDCLKCTL, 0, 0x100);
	sdmmc_clkdelay();
	sdmmc_write16(REG_SDBLKLEN, 0x200);	
	
	// CMD25 - write multiple blocks
	sdmmc_send_command(25, sector_no & 0xffff, (sector_no >> 16));
	if(sdmmc_timeout) {
		sdmmc_clkdelay1();
		sdmmc_clkdelay0();
		sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);
		return 1;
	}

	resp0 = sdmmc_read16(REG_SDRESP0);
	resp1 = sdmmc_read16(REG_SDRESP1);	
	resp0 = sdmmc_read16(REG_SDSTATUS1);

	while (!(sdmmc_read16(REG_SDSTATUS1) & 0x200));

	resp0 = sdmmc_read16(REG_SDSTATUS1);
	
	sdmmc_mask16(REG_SDSTATUS1, 1, 0);
	
	for(i = 0; i < 0x100*numsectors; i++) {
		sdmmc_write16(REG_SDFIFO, in16[i]);
	}

	sdmmc_clkdelay0();
	sdmmc_mask16(REG_SDCLKCTL, 0x100, 0);

	return 0;
}
#endif

