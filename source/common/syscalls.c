#include <nds/ndstypes.h>
#include <nds/bios.h>

extern bool __dsimode;

int swiDecompressLZSSVram(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream) {

	if (__dsimode) {
		return swiDecompressLZSSVramTWL(source,destination,toGetSize,stream);
	} else {
		return swiDecompressLZSSVramNTR(source,destination,toGetSize,stream);
	}
	
}
