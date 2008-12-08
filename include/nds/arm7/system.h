#ifndef __SYSTEM_H__
#define __SYSTEM_H__

// Power management registers
#define PM_CONTROL_REG    0
#define PM_BATTERY_REG    1
#define PM_AMPLIFIER_REG  2
#define PM_READ_REGISTER (1<<7)

// PM control register bits - LED control
#define PM_LED_CONTROL(m)    ((m)<<4)  // ?

#define PM_AMP_OFFSET  2
#define PM_GAIN_OFFSET 3

#define PM_GAIN_20 0
#define PM_GAIN_40 1
#define PM_GAIN_80 2
#define PM_GAIN_160 3

#define PM_AMP_ON      1
#define PM_AMP_OFF     0

// Warning: These functions use the SPI chain, and are thus 'critical'
// sections, make sure to disable interrupts during the call if you've
// got a VBlank IRQ polling the touch screen, etc...
#ifdef __cplusplus
extern "C" {
#endif
//install the fifo power handler
void installSystemFIFO(void);

//cause the ds to enter low power mode
void systemSleep(void);
int sleepEnabled(void);

// Read/write a power management register
int writePowerManagement(int reg, int command);

static inline
int readPowerManagement(int reg) {
	return writePowerManagement((reg)|PM_READ_REGISTER, 0);
}
#ifdef __cplusplus
}
#endif
#endif

