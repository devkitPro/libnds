/*---------------------------------------------------------------------------------

	ARM7 AES

	Copyright (C) 2017
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/

#ifndef AES_ARM7_INCLUDE
#define AES_ARM7_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <nds/ndstypes.h>

typedef struct aes_keyslot {
	vu8 normalkey[16];
	vu8 key_x[16];
	vu8 key_y[16];
} aes_keyslot_t;


#define REG_AES_CNT	(*(vu32*)0x04004400)

#define AES_WRFIFO_FLUSH (1<<10)
#define AES_RDFIFO_FLUSH (1<<11)

#define AES_CNT_DMA_WRITE_SIZE(size) ((size & 3) << 12)
#define AES_CNT_DMA_READ_SIZE(size)  ((size & 3) << 14)

#define AES_CNT_CCM_SIZE(size)       ((size & 3) << 16)

#define AES_CCM_PASSTRHOUGH (1<<19)

#define AES_CNT_KEY_APPLY (1<<24)

#define AES_CNT_KEYSLOT(slot) ((slot & 3) << 26)

#define AES_CNT_MODE(mode)    ((mode & 3) << 28)

#define AES_CNT_IRQ       (1<<30)

#define AES_CNT_ENABLE    (1<<31)

#define REG_AES_BLKCNT (*(vu32*)0x4004404)

#define REG_AES_WRFIFO (*(vu32*)0x4004408)
#define REG_AES_RDFIFO (*(vu32*)0x400440c)

#define REG_AES_IV  ((vu8*)0x4004420)
#define REG_AES_MAC ((vu8*)0x4004430)

#define AES_KEYSLOT  ((aes_keyslot_t *)0x4004440)
#define AES_KEYSLOT0 (*(aes_keyslot_t *)0x4004440)
#define AES_KEYSLOT1 (*(aes_keyslot_t *)0x4004470)
#define AES_KEYSLOT2 (*(aes_keyslot_t *)0x40044A0)
#define AES_KEYSLOT3 (*(aes_keyslot_t *)0x40044D0)

#ifdef __cplusplus
}
#endif

#endif // AES_ARM7_INCLUDE