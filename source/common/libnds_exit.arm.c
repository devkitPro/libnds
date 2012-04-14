#include <nds/system.h>
#include <libnds_internal.h>

extern char *fake_heap_end;

void __attribute__((weak)) systemErrorExit(int rc) {
	
}

void __libnds_exit(int rc) {

	if (rc != 0) {
		systemErrorExit(rc);
	}
	struct __bootstub *bootcode = __transferRegion()->bootcode;

	if (bootcode->bootsig == BOOTSIG) {
		
#ifdef ARM9
		bootcode->arm9reboot();
#endif
#ifdef ARM7
		bootcode->arm7reboot();
#endif
	} else {
		systemShutDown();
	}
	
	while(1);
}
