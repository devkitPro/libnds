/*---------------------------------------------------------------------------------

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
#ifndef RSA_H_INCLUDE
#define RSA_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include "nds/ndstypes.h"
#include <stddef.h>

typedef struct swiRSAHeapContext {
	void *heapStart;
	void *heapEnd;
	size_t heapSize;
} swiRSAHeapContext_t;


typedef struct swiRSAbuffers {
	void *dst;
	const void *sig;
	const void *key;
} swiRSAbuffers_t;

int swiRSAInitHeap(swiRSAHeapContext_t *ctx, void *heapStart, size_t heapSize);
int swiRSADecryptRAW(swiRSAHeapContext_t *ctx, swiRSAbuffers_t *rsabuffers, size_t len);
int swiRSADecrypt(swiRSAHeapContext_t *ctx, void *dst, const void *sig, const void *key);
int swiRSADecryptPGP(swiRSAHeapContext_t *ctx, void *dst, const void *sig, const void *key);

#ifdef __cplusplus
}
#endif


#endif // RSA_H_INCLUDE