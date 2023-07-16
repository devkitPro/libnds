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
#include <calico/nds/arm7/aes.h>

typedef struct aes_keyslot {
	u8 normalkey[16];
	u8 key_x[16];
	u8 key_y[16];
} aes_keyslot_t;

enum {
	AES_MODE_CCM_decrypt = AesMode_CcmDecrypt,
	AES_MODE_CCM_encrypt = AesMode_CcmEncrypt,
	AES_MODE_CTR = AesMode_Ctr,
};

enum {
	CCM_MAC_SIZE_2,
	CCM_MAC_SIZE_4,
	CCM_MAC_SIZE_6,
	CCM_MAC_SIZE_8,
	CCM_MAC_SIZE_10,
	CCM_MAC_SIZE_12,
	CCM_MAC_SIZE_14,
	CCM_MAC_SIZE_16,
};

#define AES_CNT_DMA_WRITE_SIZE(size) AES_WRFIFO_DMA_SIZE(size)
#define AES_CNT_DMA_READ_SIZE(size)  AES_RDFIFO_DMA_SIZE(size)

#define AES_CNT_MAC_SIZE(size)       ((((_x)/2-1)&7)<<16)

#define AES_CNT_KEY_APPLY AES_KEY_SELECT

#define AES_CNT_KEYSLOT(slot) AES_KEY_SLOT(slot)

#define AES_CNT_MODE(mode)    AES_MODE(mode)

#define AES_CNT_IRQ       AES_IRQ_ENABLE

#define AES_CNT_ENABLE    AES_ENABLE

#define REG_AES_BLKCNT REG_AES_LEN

#define AES_KEYSLOT  ((aes_keyslot_t *)(MM_IO + IO_AES_SLOTxKEY(0)))
#define AES_KEYSLOT0 MK_REG(aes_keyslot_t, IO_AES_SLOTxKEY(0))
#define AES_KEYSLOT1 MK_REG(aes_keyslot_t, IO_AES_SLOTxKEY(1))
#define AES_KEYSLOT2 MK_REG(aes_keyslot_t, IO_AES_SLOTxKEY(2))
#define AES_KEYSLOT3 MK_REG(aes_keyslot_t, IO_AES_SLOTxKEY(3))

#ifdef __cplusplus
}
#endif

#endif // AES_ARM7_INCLUDE