/*---------------------------------------------------------------------------------
	$Id: ndsmotion.c,v 1.2 2007-01-09 06:23:43 dovoto Exp $

	DS Motion Card functionality
	
	Copyright (C) 2007
		Michael Noland (joat)
		Jason Rogers (dovoto)
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

	$Log: not supported by cvs2svn $

---------------------------------------------------------------------------------*/
#include <nds.h>

#define CARD_WaitBusy()   while (CARD_CR1 & /*BUSY*/0x80);

// enables SPI bus at 4.19 MHz
#define SPI_On() CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40 | 0;

// disables SPI bus
#define SPI_Off() CARD_CR1 = 0;

// global variables for the sensor parameters
#define  MOTION_OFFSET 2048 	// output of axis at 0 acceleration - this can vary slightly from sensor to sensor
#define  MOTION_SENS   819		// sensitivity of axis (how many counts the output changes with 1 g) - this can vary slightly from sensor to sensor
#define  GYRO_OFFSET   1680		// zero rate bias (offset) of the gyro - this can vary slightly from sensor to sensor
#define  GYRO_SENS     825		// sensitivity of gyro (how many counts the output changes with 1000 degrees/second rotation) - this can vary slightly from sensor to sensor


// sends and receives 1 byte on the SPI bus
unsigned char motion_spi(unsigned char in_byte){

	unsigned char out_byte;
	CARD_EEPDATA = in_byte; // send the output byte to the SPI bus
	CARD_WaitBusy(); // wait for transmission to complete
	out_byte=CARD_EEPDATA; // read the input byte from the SPI bus
	return out_byte;
}

// turn on the accelerometer
void motion_enable() {
	sysSetBusOwners(true, true);
	// send 0x04, 0x04
	SPI_On()
	motion_spi(0x04); // command to write to control register
	motion_spi(0x04); // enable
	SPI_Off()
}

// read the X acceleration
signed int motion_read_x() {
	signed int output = 0;
	SPI_On()
	motion_spi(0x00); // command to convert X axis
	swiDelay(625); // wait at least 40 microseconds for the A-D conversion
	output = ( (motion_spi(0x00)<<8)|motion_spi(0x00) )>>4; // read 16 bits and store as a 12 bit number
	SPI_Off()
	return output;
}

// read the Y acceleration
signed int motion_read_y() {
	signed int output = 0;
	SPI_On()
	motion_spi(0x02); // command to convert Y axis
	swiDelay(625); // wait at least 40 microseconds for the A-D conversion
	output = ( (motion_spi(0x00)<<8)|motion_spi(0x00) )>>4; // read 16 bits and store as a 12 bit number
	SPI_Off()
	return output;
}

// read the Z acceleration
signed int motion_read_z() {
	signed int output = 0;
	SPI_On()
	motion_spi(0x01); // command to convert Z axis
	swiDelay(625); // wait at least 40 microseconds for the A-D conversion
	output = ( (motion_spi(0x00)<<8)|motion_spi(0x00) )>>4; // read 16 bits and store as a 12 bit number
	SPI_Off()
	return output;
}

// read the Z rotational speed
signed int motion_read_gyro() {
	signed int output = 0;
	SPI_On()
	motion_spi(0x07); // command to convert analog Auxilliary input (the gyro)
	swiDelay(625); // wait at least 40 microseconds for the A-D conversion
	output = ( (motion_spi(0x00)<<8)|motion_spi(0x00) )>>4; // read 16 bits and store as a 12 bit number
	SPI_Off()
	return output;
}

// checks whether a DS Motion Card is plugged in
int motion_is_inserted(){
    int plugged_in = 1;
    SPI_On()
    motion_spi(0x03); //command to read from KXB5 control register
    // send dummy byte to receive contents of control register
    // if there is nothing plugged in, this will return 255
	// emulators will return 0
    // if the DS Motion Card is plugged in, it will return a smaller nonzero number
    int control_reg = motion_spi(0x00);
	if( control_reg == 255 || control_reg == 0) plugged_in = 0;
    SPI_Off()
    return plugged_in;
}

//converts raw acceleration value to mili G (where g is 9.8 m/s*s)
int motion_acceleration(int accel){
	return (accel - MOTION_OFFSET) * 1000 / MOTION_SENS;
}

//converts raw rotation value to degrees per second
int motion_rotation(int rotation){
	return (rotation - GYRO_OFFSET) * 1000 / GYRO_SENS;
}
