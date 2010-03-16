/*---------------------------------------------------------------------------------

  Copyright (C) 2005 - 2010
    Michael Noland (joat)
	Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1.  The origin of this software must not be misrepresented; you
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product
      documentation would be appreciated but is not required.
  2.  Altered source versions must be plainly marked as such, and
      must not be misrepresented as being the original software.
  3.  This notice may not be removed or altered from any source
      distribution.

---------------------------------------------------------------------------------*/

#include <nds/arm7/serial.h>
#include <nds/interrupts.h>

//---------------------------------------------------------------------------------
int writePowerManagement(int reg, int command) {
//---------------------------------------------------------------------------------
	int oldIME=enterCriticalSection();
	// Write the register / access mode (bit 7 sets access mode)
	while (REG_SPICNT & SPI_BUSY);
	REG_SPICNT = SPI_ENABLE | SPI_BAUD_1MHz | SPI_BYTE_MODE | SPI_CONTINUOUS | SPI_DEVICE_POWER;
	REG_SPIDATA = reg;

	// Write the command / start a read
	while (REG_SPICNT & SPI_BUSY);
	REG_SPICNT = SPI_ENABLE | SPI_BAUD_1MHz | SPI_BYTE_MODE | SPI_DEVICE_POWER;
	REG_SPIDATA = command;

	// Read the result
	while (REG_SPICNT & SPI_BUSY);

	leaveCriticalSection(oldIME);

	return REG_SPIDATA & 0xFF;
}

//---------------------------------------------------------------------------------
void readFirmware(u32 address, void * destination, u32 size) {
//---------------------------------------------------------------------------------
	int oldIME=enterCriticalSection();
	u8 *buffer = destination;
	// Read command
	while (REG_SPICNT & SPI_BUSY);
	REG_SPICNT = SPI_ENABLE | SPI_BYTE_MODE | SPI_CONTINUOUS | SPI_DEVICE_FIRMWARE;
	REG_SPIDATA = FIRMWARE_READ;
	while (REG_SPICNT & SPI_BUSY);

	// Set the address
	REG_SPIDATA = (address>>16) & 0xFF;
	while (REG_SPICNT & SPI_BUSY);
	REG_SPIDATA = (address>>8) & 0xFF;
	while (REG_SPICNT & SPI_BUSY);
	REG_SPIDATA = (address) & 0xFF;
	while (REG_SPICNT & SPI_BUSY);

	u32 i;
	// Read the data
	for(i=0;i<size;i++) {
		REG_SPIDATA = 0;
		while (REG_SPICNT & SPI_BUSY);
		buffer[i] = (REG_SPIDATA & 0xFF);
	}

	REG_SPICNT = 0;
	leaveCriticalSection(oldIME);
}

