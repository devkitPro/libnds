/*---------------------------------------------------------------------------------

	Copyright (C) 2005
		Michael Noland (Joat)
		Jason Rogers (Dovoto)
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

#include "nds/bios.h"
#include "nds/arm7/clock.h"
#include "nds/interrupts.h"
#include "nds/ipc.h"
#include "nds/system.h"
#include "libnds_internal.h"

#include <time.h>



// Delay (in swiDelay units) for each bit transfer
#define RTC_DELAY 48

// Pin defines on RTC_CR
#define CS_0    (1<<6)
#define CS_1    ((1<<6) | (1<<2))
#define SCK_0   (1<<5)
#define SCK_1   ((1<<5) | (1<<1))
#define SIO_0   (1<<4)
#define SIO_1   ((1<<4) | (1<<0))
#define SIO_out (1<<4)
#define SIO_in  (1)

//---------------------------------------------------------------------------------
void BCDToInteger(uint8 * data, uint32 length) {
//---------------------------------------------------------------------------------
	u32 i;
	for (i = 0; i < length; i++) {
		data[i] = (data[i] & 0xF) + ((data[i] & 0xF0)>>4)*10;
	}
}


//---------------------------------------------------------------------------------
void integerToBCD(uint8 * data, uint32 length) {
//---------------------------------------------------------------------------------
	u32 i;
	for (i = 0; i < length; i++) {
		int high, low;
		swiDivMod(data[i], 10, &high, &low);
		data[i] = (high<<4) | low;
	}
}

//---------------------------------------------------------------------------------
void rtcTransaction(uint8 * command, uint32 commandLength, uint8 * result, uint32 resultLength) {
//---------------------------------------------------------------------------------
	uint32 bit;
	uint8 data;

	// Raise CS
	RTC_CR8 = CS_0 | SCK_1 | SIO_1;
	swiDelay(RTC_DELAY);
	RTC_CR8 = CS_1 | SCK_1 | SIO_1;
	swiDelay(RTC_DELAY);

	// Write command byte (high bit first)
		data = *command++;

		for (bit = 0; bit < 8; bit++) {
			RTC_CR8 = CS_1 | SCK_0 | SIO_out | (data>>7);
			swiDelay(RTC_DELAY);

			RTC_CR8 = CS_1 | SCK_1 | SIO_out | (data>>7);
			swiDelay(RTC_DELAY);

			data = data << 1;
		}
	// Write parameter bytes (low bit first)
	for ( ; commandLength > 1; commandLength--) {
		data = *command++;

		for (bit = 0; bit < 8; bit++) {
			RTC_CR8 = CS_1 | SCK_0 | SIO_out | (data & 1);
			swiDelay(RTC_DELAY);

			RTC_CR8 = CS_1 | SCK_1 | SIO_out | (data & 1);
			swiDelay(RTC_DELAY);

			data = data >> 1;
		}
	}

	// Read result bytes (low bit first)
	for ( ; resultLength > 0; resultLength--) {
		data = 0;

		for (bit = 0; bit < 8; bit++) {
			RTC_CR8 = CS_1 | SCK_0;
			swiDelay(RTC_DELAY);

			RTC_CR8 = CS_1 | SCK_1;
			swiDelay(RTC_DELAY);

			if (RTC_CR8 & SIO_in) data |= (1 << bit);
		}
		*result++ = data;
	}

	// Finish up by dropping CS low
	RTC_CR8 = CS_0 | SCK_1;
	swiDelay(RTC_DELAY);
}


//---------------------------------------------------------------------------------
void rtcReset(void) {
//---------------------------------------------------------------------------------
	uint8 status;
	uint8 command[2];

	// Read the first status register
	command[0] = READ_STATUS_REG1;
	rtcTransaction(command, 1, &status, 1);

	// Reset the RTC if needed
	if (status & (STATUS_POC | STATUS_BLD)) {
		command[0] = WRITE_STATUS_REG1;
		command[1] = status | STATUS_RESET;
		rtcTransaction(command, 2, 0, 0);
	}
}


//---------------------------------------------------------------------------------
void rtcGetTimeAndDate(uint8 * time) {
//---------------------------------------------------------------------------------
	uint8 command, status;

	command = READ_TIME_AND_DATE;
	rtcTransaction(&command, 1, time, 7);

	command = READ_STATUS_REG1;
	rtcTransaction(&command, 1, &status, 1);

	if ( status & STATUS_24HRS ) {
		time[4] &= 0x3f;
	} else {

	}
	BCDToInteger(time,7);
}

//---------------------------------------------------------------------------------
void rtcSetTimeAndDate(uint8 * time) {
//---------------------------------------------------------------------------------
	uint8 command[8];
	
	int i;
	for ( i=0; i< 8; i++ ) {
		command[i+1] = time[i];
	}
	command[0] = WRITE_TIME_AND_DATE;
	// fixme: range checking on the data we tell it
	rtcTransaction(command, 8, 0, 0);
}

//---------------------------------------------------------------------------------
void rtcGetTime(uint8 * time) {
//---------------------------------------------------------------------------------
	uint8 command, status;

	command = READ_TIME;
	rtcTransaction(&command, 1, time, 3);

	command = READ_STATUS_REG1;
	rtcTransaction(&command, 1, &status, 1);
	if ( status & STATUS_24HRS ) {
		time[0] &= 0x3f;
	} else {

	}
	BCDToInteger(time,3);

}

//---------------------------------------------------------------------------------
void rtcSetTime(uint8 * time) {
//---------------------------------------------------------------------------------
	uint8 command[4];
	
	int i;
	for ( i=0; i< 3; i++ ) {
		command[i+1] = time[i];
	}
	command[0] = WRITE_TIME;
	// fixme: range checking on the data we tell it
	rtcTransaction(command, 4, 0, 0);
}

//---------------------------------------------------------------------------------
void syncRTC() {
//---------------------------------------------------------------------------------
	__transferRegion()->unixTime++;
}

/* Nonzero if `y' is a leap year, else zero. */
#define leap(y) (((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)

/* Number of leap years from 1970 to `y' (not including `y' itself). */
#define nleap(y) (((y) - 1969) / 4 - ((y) - 1901) / 100 + ((y) - 1601) / 400)

/* Additional leapday in February of leap years. */
#define leapday(m, y) ((m) == 1 && leap (y))

/* Accumulated number of days from 01-Jan up to start of current month. */
static const short ydays[] = {
  0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

/* Length of month `m' (0 .. 11) */
#define monthlen(m, y) (ydays[(m)+1] - ydays[m] + leapday (m, y))

//---------------------------------------------------------------------------------
static time_t __mktime( RTCtime *dstime ) {
//---------------------------------------------------------------------------------
	int years, months, days, hours, minutes, seconds;

	years = dstime->year + 2000;	/* year - 2000 -> year */
	months = dstime->month -1;		/* 0..11 */
	days = dstime->day - 1;			/* 1..31 -> 0..30 */
	hours = dstime->hours;			/* 0..23 */
	minutes = dstime->minutes;		/* 0..59 */
	seconds = dstime->seconds;		/* 0..61 in ANSI C. */

	/* Set `days' to the number of days into the year. */
	days += ydays[months] + (months > 1 && leap (years));

	/* Now calculate `days' to the number of days since Jan 1, 1970. */
	days = (unsigned)days + 365 * (unsigned)(years - 1970) + (unsigned)(nleap (years));

	return (time_t)(86400L * (unsigned long)days +
					3600L * (unsigned long)hours +
					(unsigned long)(60 * minutes + seconds));
}

//---------------------------------------------------------------------------------
void resyncClock() {
//---------------------------------------------------------------------------------
	RTCtime dstime;
	rtcGetTimeAndDate((uint8 *)&dstime);
	
	__transferRegion()->unixTime = __mktime(&dstime);
}

//---------------------------------------------------------------------------------
void initClockIRQ() {
//---------------------------------------------------------------------------------

	REG_RCNT = 0x8100;
	irqSet(IRQ_NETWORK, syncRTC);
	// Reset the clock if needed
	rtcReset();

	uint8 command[4];
	command[0] = READ_STATUS_REG2;
	rtcTransaction(command, 1, &command[1], 1);

	command[0] = WRITE_STATUS_REG2;
	command[1] = 0x41;
	rtcTransaction(command, 2, 0, 0);
	
	command[0] = WRITE_INT_REG1;
	command[1] = 0x01;
	rtcTransaction(command, 2, 0, 0);
	
	command[0] = WRITE_INT_REG2;
	command[1] = 0x00;
	command[2] = 0x21;
	command[3] = 0x35;
	rtcTransaction(command, 4, 0, 0);

	// Read all time settings on first start
	resyncClock();
}

