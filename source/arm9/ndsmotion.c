/*---------------------------------------------------------------------------------
	$Id: ndsmotion.c,v 1.3 2007-01-10 16:47:30 dovoto Exp $

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
	Revision 1.2  2007/01/09 06:23:43  dovoto
	Fixed logging header for ndsmotion.h and ndsmotion.c
	

---------------------------------------------------------------------------------*/

#include <nds.h>
#include <nds/arm9/ndsmotion.h>

#define CARD_WaitBusy()   while (CARD_CR1 & /*BUSY*/0x80);

// enables SPI bus at 4.19 MHz
#define SPI_On() CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40 | 0;

// disables SPI bus
#define SPI_Off() CARD_CR1 = 0;

//these are the default calibration values for sensitivity and offset
MotionCalibration calibration = {2048, 2048, 2048, 1680, 819, 819, 819, 825};

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

//gets acceleration value in mili G (where g is 9.8 m/s*s)
int motion_acceleration_x(void){
	int accel = motion_read_x();
	return (accel - calibration.xoff) * 1000 / calibration.xsens;
}

//gets acceleration value in mili G (where g is 9.8 m/s*s)
int motion_acceleration_y(void){
	int accel = motion_read_y();
	return (accel - calibration.yoff) * 1000 / calibration.ysens;
}
//gets acceleration value in mili G (where g is 9.8 m/s*s)
int motion_acceleration_z(void){
	int accel = motion_read_z();
	return (accel - calibration.zoff) * 1000 / calibration.zsens;
}

//converts raw rotation value to degrees per second
int motion_rotation(){
	int rotation = motion_read_gyro();
	return (rotation - calibration.goff) * 1000 / calibration.gsens;
}

//this should be passed the raw reading at 1g for accurate
//acceleration calculations.  Default is 819
void motion_set_sens_x(int sens){
	calibration.xsens = sens - calibration.xoff;
}

//this should be passed the raw reading at 1g for accurate
//acceleration calculations.  Default is 819
void motion_set_sens_y(int sens){
	calibration.ysens = sens - calibration.yoff;
}

//this should be passed the raw reading at 1g for accurate
//acceleration calculations.  Default is 819
void motion_set_sens_z(int sens){
	calibration.zsens = sens - calibration.zoff;
}

//this should be passed the raw reading at 1g for accurate
//acceleration calculations.  Default is 819
void motion_set_sens_gyro(int sens){
	calibration.gsens = sens;
}

//this should be called when the axis is under no acceleration
//default is 2048
void motion_set_offs_x(){
	calibration.xoff = motion_read_x();
}

//this should be called when the axis is under no acceleration
//default is 2048
void motion_set_offs_y(){
	calibration.yoff = motion_read_y();
}

//this should be called when the axis is under no acceleration
//default is 2048
void motion_set_offs_z(){
	calibration.zoff = motion_read_z();
}

//this should be called when the axis is under no acceleration
//default is 2048
void motion_set_offs_gyro(){
	calibration.goff = motion_read_gyro();
}

MotionCalibration* motion_get_calibraion(){
	return &calibration;
}

void motion_set_calibration(MotionCalibration* cal){
	calibration.xsens = cal->xsens;
	calibration.ysens = cal->ysens;
	calibration.zsens = cal->zsens;
	calibration.gsens = cal->gsens;
	calibration.xoff = cal->xoff;
	calibration.yoff = cal->yoff;
	calibration.zoff = cal->zoff;
	calibration.goff = cal->goff;
}
