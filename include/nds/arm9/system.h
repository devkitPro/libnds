#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <nds/fifocommon.h>

#ifdef __cplusplus
extern "C" {
#endif


void systemSleep(void);
void powerOn(PM_Bits bits);
void powerOff(PM_Bits bits);
void ledBlink(PM_LedBlinkMode bm);
void systemMsgHandler(int bytes, void* user_data);
void powerValueHandler(u32 value, void* data);

#ifdef __cplusplus
}
#endif

#endif
