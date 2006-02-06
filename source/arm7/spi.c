/*---------------------------------------------------------------------------------

  Copyright (C) 2005
    Michael Noland (joat)

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

  $Log: not supported by cvs2svn $
  

---------------------------------------------------------------------------------*/

//////////////////////////////////////////////////////////////////////

#include <nds/arm7/serial.h>

//////////////////////////////////////////////////////////////////////

int writePowerManagement(int reg, int command) {
  // Write the register / access mode (bit 7 sets access mode)
  while (SERIAL_CR & SPI_BUSY);
  SERIAL_CR = SPI_ENABLE | SPI_BAUD_1MHZ | SPI_BYTE_MODE | SPI_CONTINUOUS | SPI_DEVICE_POWER;
  SERIAL_DATA = reg;

  // Write the command / start a read
  while (SERIAL_CR & SPI_BUSY);
  SERIAL_CR = SPI_ENABLE | SPI_BAUD_1MHZ | SPI_BYTE_MODE | SPI_DEVICE_POWER;
  SERIAL_DATA = command;

  // Read the result
  while (SERIAL_CR & SPI_BUSY);
  return SERIAL_DATA & 0xFF;
}

//////////////////////////////////////////////////////////////////////

void readFirmware(uint32 address, void * destination, uint32 size) {
  uint8 * buffer = (uint8 *)destination;

  // Read command
  while (SERIAL_CR & SPI_BUSY);
  SERIAL_CR = SPI_ENABLE | SPI_BYTE_MODE | SPI_CONTINUOUS | SPI_DEVICE_FIRMWARE;
  SERIAL_DATA = FIRMWARE_READ;
  while (SERIAL_CR & SPI_BUSY);

  // Set the address
  SERIAL_DATA = (address>>16) & 0xFF;
  while (SERIAL_CR & SPI_BUSY);
  SERIAL_DATA = (address>>8) & 0xFF;
  while (SERIAL_CR & SPI_BUSY);
  SERIAL_DATA = (address>>0) & 0xFF;
  while (SERIAL_CR & SPI_BUSY);

  // Read the data
  while (size--) {
    SERIAL_DATA = 0;
    while (SERIAL_CR & SPI_BUSY);
    *buffer++ = (SERIAL_DATA & 0xFF);
  }

  SERIAL_CR = 0;
}

//////////////////////////////////////////////////////////////////////
