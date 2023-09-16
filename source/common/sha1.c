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

#include <nds/sha1.h>
#include <nds/system.h>

//---------------------------------------------------------------------------------
void swiSHA1Init(swiSHA1context_t *ctx) {
//---------------------------------------------------------------------------------
	ctx->hash_block = NULL;
	if (isDSiMode()) svcSha1InitTWL(ctx);
}

//---------------------------------------------------------------------------------
void swiSHA1Update(swiSHA1context_t *ctx, const void *data, size_t len) {
//---------------------------------------------------------------------------------
	if (isDSiMode()) svcSha1UpdateTWL(ctx, data, len);

}

//---------------------------------------------------------------------------------
void swiSHA1Final(void *digest, swiSHA1context_t *ctx) {
//---------------------------------------------------------------------------------
	if (isDSiMode()) svcSha1DigestTWL(digest, ctx);
}

//---------------------------------------------------------------------------------
void swiSHA1Calc(void *digest, const void *data, size_t len) {
//---------------------------------------------------------------------------------
	if (isDSiMode()) svcSha1CalcTWL(digest, data, len);
}

//---------------------------------------------------------------------------------
bool swiSHA1Verify(const void *digest1, const void *digest2) {
//---------------------------------------------------------------------------------
	if (isDSiMode()) {
		return svcSha1VerifyTWL(digest1, digest2);
	} else {
		return false;
	}

}
