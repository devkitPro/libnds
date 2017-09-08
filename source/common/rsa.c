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

#include <nds/rsa.h>
#include <nds/system.h>

int swiRSAInitHeapTWL(swiRSAHeapContext_t *ctx, void *heapStart, size_t heapSize);
int swiRSADecryptRAWTWL(swiRSAHeapContext_t *ctx, swiRSAbuffers_t *rsabuffers, size_t len);
int swiRSADecryptTWL(swiRSAHeapContext_t *ctx, void *dst, const void *sig, const void *key);
int swiRSADecryptPGPTWL(swiRSAHeapContext_t *ctx, void *dst, const void *sig, const void *key);

//---------------------------------------------------------------------------------
int swiRSAInitHeap(swiRSAHeapContext_t *ctx, void *heapStart, size_t heapSize) {
//---------------------------------------------------------------------------------
	if (isDSiMode()) return swiRSAInitHeapTWL(ctx, heapStart, heapSize);
	return 0;
}

//---------------------------------------------------------------------------------
int swiRSADecryptRAW(swiRSAHeapContext_t *ctx, swiRSAbuffers_t *rsabuffers, size_t len) {
//---------------------------------------------------------------------------------
	if (isDSiMode()) return swiRSADecryptRAWTWL(ctx, rsabuffers, len);
	return 0;
}

//---------------------------------------------------------------------------------
int swiRSADecrypt(swiRSAHeapContext_t *ctx, void *dst, const void *sig, const void *key) {
//---------------------------------------------------------------------------------
	if (isDSiMode()) return swiRSADecryptTWL(ctx, dst, sig, key);
	return 0;
}

//---------------------------------------------------------------------------------
int swiRSADecryptPGP(swiRSAHeapContext_t *ctx, void *dst, const void *sig, const void *key) {
//---------------------------------------------------------------------------------
	if (isDSiMode()) return swiRSADecryptPGPTWL(ctx, dst, sig, key);
	return 0;
}

