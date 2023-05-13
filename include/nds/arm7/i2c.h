/*---------------------------------------------------------------------------------

	I2C control for the ARM7

	Copyright (C) 2011
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
#ifndef I2C_ARM7_INCLUDE
#define I2C_ARM7_INCLUDE

#ifndef ARM7
#error i2c header is for ARM7 only
#endif

#include <nds/ndstypes.h>
#include <calico/nds/arm7/i2c.h>
#include <calico/nds/arm7/mcu.h>

#define REG_I2CDATA	REG_I2C_DATA
#define REG_I2CCNT	REG_I2C_CNT

static inline void i2cWaitBusy() {
	while(REG_I2CCNT & 0x80);
}

enum i2cDevices {
	I2C_CAM0	= 0x7A,
	I2C_CAM1	= 0x78,
	I2C_UNK1	= 0xA0,
	I2C_UNK2	= 0xE0,
	I2C_PM		= I2cDev_MCU,
	I2C_UNK3	= 0x40,
	I2C_GPIO	= 0x90
};

// Registers for Power Management (I2C_PM)
#define I2CREGPM_BATUNK		McuReg_Version
#define I2CREGPM_PWRIF		McuReg_IrqFlags
#define I2CREGPM_PWRCNT		McuReg_DoReset
#define I2CREGPM_MMCPWR		McuReg_Config
#define I2CREGPM_BATTERY	McuReg_BatteryState
#define I2CREGPM_CAMLED		McuReg_CamLed
#define I2CREGPM_VOL		McuReg_VolumeLevel
#define I2CREGPM_RESETFLAG	McuReg_WarmbootFlag

static inline u8 i2cWriteRegister(u8 device, u8 reg, u8 data)
{
	i2cLock();
	bool ret = i2cWriteRegister8((I2cDevice)device, reg, data);
	i2cUnlock();
	return ret;
}

static inline u8 i2cReadRegister(u8 device, u8 reg)
{
	i2cLock();
	u8 ret = i2cReadRegister8((I2cDevice)device, reg);
	i2cUnlock();
	return ret;
}

#endif // I2C_ARM7_INCLUDE