#include <nds/arm7/input.h>
#include <nds/arm7/touch.h>
#include <nds/system.h>
#include <nds/touch.h>
#include <nds/fifocommon.h>
#include <nds/fifomessages.h>
#include <nds/ipc.h>
#include <nds/ndstypes.h>

enum{
	KEY_TOUCH = (1<<6),
	KEY_LID = (1<<7)
}Keys;

void inputGetAndSend(void){

	static bool penDown = false;
	static int sleepCounter = 0;

	touchPosition tempPos = {0};
	FifoMessage msg = {0};

	u16 keys= REG_KEYXY;


	if(!touchPenDown()) {
		keys |= KEY_TOUCH;
  	} else {
		keys &= ~KEY_TOUCH;
	}

	msg.SystemInput.keys = keys;

	if(keys & KEY_TOUCH) {
		penDown = false;	
	} else {
		msg.SystemInput.keys |= KEY_TOUCH;

		if(penDown) {
			touchReadXY(&tempPos);	
			
			if(tempPos.rawx && tempPos.rawy) {
				msg.SystemInput.keys &= ~KEY_TOUCH;
				msg.SystemInput.touch = tempPos;
			} else {
				penDown = false;
			}
		} else {
			penDown = true;
		}
	}	

	if(keys & KEY_LID) 
		sleepCounter++;
	else
		sleepCounter = 0;

	//sleep if lid has been closed for 20 frames
	if(sleepCounter >= 20) 
	{
		systemSleep();
		sleepCounter = 0;
	}

	msg.type = SYS_INPUT_MESSAGE; //set message type

	fifoSendDatamsg(FIFO_SYSTEM, sizeof(msg), (u8*)&msg);
}