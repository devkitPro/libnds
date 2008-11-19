#include <nds/arm7/input.h>
#include <nds/arm7/touch.h>
#include <nds/arm7/system.h>
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
	SystemInputMsg input = {0};

	u16 keys= REG_KEYXY;

	input.keys = keys;

	if(keys & KEY_TOUCH)
	{
		penDown = false;	
	}
	else
	{
		input.keys |= KEY_TOUCH;

		if(penDown)
		{
			touchReadXY(&tempPos);	
			
			if(tempPos.rawx && tempPos.rawy)
			{
				input.keys &= ~KEY_TOUCH;
				input.touch = tempPos;
			}
			else
			{
				penDown = false;
			}
		}
		else
		{
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
		sleep();
		sleepCounter = 0;
	}

	input.type = SYS_INPUT_MESSAGE; //set message type

	fifoSendDatamsg(FIFO_SYSTEM, sizeof(input), (u8*)&input);
}