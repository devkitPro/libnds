#ifndef __SDMMC_H__
#define __SDMMC_H__

#include <nds/ndstypes.h>

#define SDMMC_BASE	0x04004800

#define REG_SDSTATUS0	  0x1c
#define REG_SDSTATUS1	  0x1e
#define REG_SDRESET     0xe0
#define REG_SDCLKCTL    0x24 
#define REG_SDOPT       0x28
#define REG_SDCMD       0x00
#define REG_SDCMDARG    0x04
#define REG_SDCMDARG0   0x04
#define REG_SDCMDARG1  	0x06
#define REG_SDSTOP      0x08
#define REG_SDRESP      0x0c

#define REG_SDRESP0     0x0c
#define REG_SDRESP1     0x0e
#define REG_SDRESP2     0x10
#define REG_SDRESP3     0x12
#define REG_SDRESP4     0x14
#define REG_SDRESP5     0x16
#define REG_SDRESP6     0x18
#define REG_SDRESP7     0x1a

#define REG_SDBLKLEN    0x26
#define REG_SDBLKCOUNT  0x0a
#define REG_SDFIFO      0x30

//The below defines are from linux kernel drivers/mmc tmio_mmc.h.
/* Definitions for values the CTRL_STATUS register can take. */
#define TMIO_STAT_CMDRESPEND    0x00000001
#define TMIO_STAT_DATAEND       0x00000004
#define TMIO_STAT_CARD_REMOVE   0x00000008
#define TMIO_STAT_CARD_INSERT   0x00000010
#define TMIO_STAT_SIGSTATE      0x00000020
#define TMIO_STAT_WRPROTECT     0x00000080
#define TMIO_STAT_CARD_REMOVE_A 0x00000100
#define TMIO_STAT_CARD_INSERT_A 0x00000200
#define TMIO_STAT_SIGSTATE_A    0x00000400
#define TMIO_STAT_CMD_IDX_ERR   0x00010000
#define TMIO_STAT_CRCFAIL       0x00020000
#define TMIO_STAT_STOPBIT_ERR   0x00040000
#define TMIO_STAT_DATATIMEOUT   0x00080000
#define TMIO_STAT_RXOVERFLOW    0x00100000
#define TMIO_STAT_TXUNDERRUN    0x00200000
#define TMIO_STAT_CMDTIMEOUT    0x00400000
#define TMIO_STAT_RXRDY         0x01000000
#define TMIO_STAT_TXRQ          0x02000000
#define TMIO_STAT_ILL_FUNC      0x20000000
#define TMIO_STAT_CMD_BUSY      0x40000000
#define TMIO_STAT_ILL_ACCESS    0x80000000

void sdmmc_controller_init();
void sdmmc_initirq();
int sdmmc_cardinserted();

int sdmmc_sdcard_init();
void sdmmc_sdcard_readsector(u32 sector_no, void *out);
void sdmmc_sdcard_readsectors(u32 sector_no, u32 numsectors, void *out);
void sdmmc_sdcard_writesector(u32 sector_no, void *in);
void sdmmc_sdcard_writesectors(u32 sector_no, u32 numsectors, void *in);

extern u32 sdmmc_cid[];
extern int sdmmc_curdevice;

//---------------------------------------------------------------------------------
static inline u16 sdmmc_read16(u16 reg) {
//---------------------------------------------------------------------------------
	return *(vu16*)(SDMMC_BASE + reg);
}

//---------------------------------------------------------------------------------
static inline void sdmmc_write16(u16 reg, u16 val) {
//---------------------------------------------------------------------------------
	*(vu16*)(SDMMC_BASE + reg) = val;
}

//---------------------------------------------------------------------------------
static inline void sdmmc_mask16(u16 reg, u16 clear, u16 set) {
//---------------------------------------------------------------------------------
	u16 val = sdmmc_read16(reg);
	val &= ~clear;
	val |= set;
	sdmmc_write16(reg, val);
}

#endif
