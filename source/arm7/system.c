#include <nds/system.h>
#include <nds/fifocommon.h>
#include <nds/ndstypes.h>
#include <nds/interrupts.h>
#include <nds/bios.h>
#include <nds/arm7/clock.h>

bool sleepIsEnabled = true;

void powerValueHandler(u32 value, void* user_data) {
	u32 temp;
	u32 ie_save;
	
	switch(value & 0xFFFF0000) {
		//power control
	case PM_REQ_LED:
		temp = readPowerManagement(PM_CONTROL_REG);
		temp &= ~(3 << 4); //clear led bits
		temp |= value & 0xFF;
		writePowerManagement(PM_CONTROL_REG, temp);
		break;
	case PM_REQ_ON:
		temp = readPowerManagement(PM_CONTROL_REG);
		writePowerManagement(PM_CONTROL_REG, temp | (value & 0xFFFF));
		break;
	case PM_REQ_OFF:
		temp = readPowerManagement(PM_CONTROL_REG) & (~(value & 0xFFFF));
		writePowerManagement(PM_CONTROL_REG, temp);
		break;

	case PM_REQ_SLEEP:
			
		ie_save = REG_IE;
		// Turn the speaker down.
		swiChangeSoundBias(0,0x400);
		// Save current power state.
		int power = readPowerManagement(PM_CONTROL_REG);
		// Set sleep LED.
		writePowerManagement(PM_CONTROL_REG, PM_LED_CONTROL(1));
		// Register for the lid interrupt.
		REG_IE = IRQ_LID;

		// Power down till we get our interrupt.
		swiSleep(); //waits for PM (lid open) interrupt

		//100ms
		swiDelay(838000);
		
		// Restore the interrupt state.
		REG_IE = ie_save;

		// Restore power state.
		writePowerManagement(PM_CONTROL_REG, power);

		// Turn the speaker up.
		swiChangeSoundBias(1,0x400); 

		// update clock tracking
		resyncClock();
		break;

	case PM_REQ_SLEEP_DISABLE:
		sleepIsEnabled = false;
		break;

	case PM_REQ_SLEEP_ENABLE:
		sleepIsEnabled = true;
		break;
	}
}

void systemSleep(void) {
	if(!sleepIsEnabled) return;
	//puts arm9 to sleep which then notifies arm7 above (which causes arm7 to sleep)
	fifoSendValue32(FIFO_PM, PM_REQ_SLEEP);
}

int sleepEnabled(void) {
	return sleepIsEnabled;
}

void installSystemFIFO(void) {
	fifoSetValue32Handler(FIFO_PM, powerValueHandler, 0);
}

