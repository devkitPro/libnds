#include <nds/system.h>

extern char *fake_heap_end;

void __libnds_exit(int rc) {

	struct __bootstub *bootcode = (struct __bootstub *)fake_heap_end;

	if (bootcode->bootsig == BOOTSIG) {
		
#ifdef ARM9
		bootcode->arm9reboot();
#endif
#ifdef ARM7
		bootcode->arm7reboot();
#endif
	}
	
	while(1);
}
