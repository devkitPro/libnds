/*---------------------------------------------------------------------------------

	Copyright (C) 2005
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

---------------------------------------------------------------------------------*/


/*! \file timers.h
    \brief Contains defines, macros and functions for ARM7 and ARM9 timer operation.
    It also contains a simplified API for timer use and some cpu timing functions.

	The timers are fed with a 33.513982 MHz source on the ARM9 and ARM7.

	\note that dswifi will use timer 3 on the arm9, so don't use that if you use dswifi.
*/


#ifndef NDS_TIMERS_INCLUDE
#define NDS_TIMERS_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif


#include <nds/ndstypes.h>



//!	Returns a dereferenced pointer to the data register for timer control Register.
/*!	<b>Example Usage:</b> %TIMER_CR(x) = %TIMER_ENABLE | %ClockDivider_64;

	Possible bit defines:

	\see TIMER_ENABLE
	\see TIMER_IRQ_REQ
	\see TIMER_CASCADE
	\see ClockDivider
*/
#define TIMER_CR(n) (*(vu16*)(0x04000102+((n)<<2)))

//!	Same as %TIMER_CR(0).
#define TIMER0_CR   (*(vu16*)0x04000102)
//!	Same as %TIMER_CR(1).
#define TIMER1_CR   (*(vu16*)0x04000106)
//!	Same as %TIMER_CR(2).
#define TIMER2_CR   (*(vu16*)0x0400010A)
//!	Same as %TIMER_CR(3).
#define TIMER3_CR   (*(vu16*)0x0400010E)


//!	Returns a dereferenced pointer to the data register for timer number "n".
/*!	\see TIMER_CR(n)
	\see TIMER_FREQ(n)

	%TIMER_DATA(n) when set will latch that value into the counter.  Everytime the
	counter rolls over %TIMER_DATA(0) will return to the latched value.  This allows
	you to control the frequency of the timer using the following formula:\n
		%TIMER_DATA(x) = -(BUS_CLOCK/(freq * divider));

	<b>Example Usage:</b>
	%TIMER_DATA(0) = value;  were 0 can be 0 through 3 and value is 16 bits.
*/
#define TIMER_DATA(n)  (*(vu16*)(0x04000100+((n)<<2)))

//!	Same as %TIMER_DATA(0).
#define TIMER0_DATA    (*(vu16*)0x04000100)
//!	Same as %TIMER_DATA(1).
#define TIMER1_DATA    (*(vu16*)0x04000104)
//!	Same as %TIMER_DATA(2).
#define TIMER2_DATA    (*(vu16*)0x04000108)
//!	Same as %TIMER_DATA(3).
#define TIMER3_DATA    (*(vu16*)0x0400010C)



//! the speed in which the timer ticks in hertz.
#define BUS_CLOCK (33513982)


//!	Enables the timer.
#define TIMER_ENABLE    (1<<7)
//!	Causes the timer to request an Interupt on overflow.
#define TIMER_IRQ_REQ   (1<<6)
//!	When set will cause the timer to count when the timer below overflows (unavailable for timer 0).
#define TIMER_CASCADE   (1<<2)


//! allowable timer clock dividers.
typedef enum {
	ClockDivider_1 = 0,		//!< divides the timer clock by 1 (~33513.982 kHz)
	ClockDivider_64 = 1,	//!< divides the timer clock by 64 (~523.657 kHz)
	ClockDivider_256 = 2,	//!< divides the timer clock by 256 (~130.914 kHz)
	ClockDivider_1024 = 3	//!< divides the timer clock by 1024 (~32.7284 kHz)
}ClockDivider;


//use the ClockDivider enum.
//	Causes the timer to count at 33.514 Mhz.
#define TIMER_DIV_1     (0)
//	Causes the timer to count at (33.514 / 64) Mhz.
#define TIMER_DIV_64    (1)
//	Causes the timer to count at (33.514 / 256) Mhz.
#define TIMER_DIV_256   (2)
//	Causes the timer to count at (33.514 / 1024) Mhz.
#define TIMER_DIV_1024  (3)



/*!	\brief A macro that calculates %TIMER_DATA(n) settings for a given frequency of n.
	will calculate the correct value for %TIMER_DATA(n) given the frequency in hertz
	(number of times the timer should overflow per second).

	<b>Example Usage:</b>
	<pre>
		//calls the timerCallBack function 5 times per second.
		timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(5), timerCallBack);
	</pre>

	Max frequency is: 33554432Hz
	Min frequency is: 512Hz

	\note Use the appropriate macro depending on the used clock divider.
*/
#define TIMER_FREQ(n)    (-BUS_CLOCK/(n))


/*!	\brief A macro that calculates %TIMER_DATA(n) settings for a given frequency of n.
	will calculate the correct value for %TIMER_DATA(n) given the frequency in hertz
	(number of times the timer should overflow per second).

	<b>Example Usage:</b>
	<pre>
		//calls the timerCallBack function 5 times per second.
		timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(5), timerCallBack);
	</pre>

	Max frequency is: 524288Hz
	Min frequency is: 8Hz

	\note Use the appropriate macro depending on the used clock divider.
*/
#define TIMER_FREQ_64(n)  (-(BUS_CLOCK>>6)/(n))


/*!	\brief A macro that calculates %TIMER_DATA(n) settings for a given frequency of n.
	will calculate the correct value for %TIMER_DATA(n) given the frequency in hertz
	(number of times the timer should overflow per second).

	<b>Example Usage:</b>
	<pre>
		//calls the timerCallBack function 5 times per second.
		timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(5), timerCallBack);
	</pre>

	Max frequency is: 131072Hz
	Min frequency is: 2Hz

	\note Use the appropriate macro depending on the used clock divider.
*/
#define TIMER_FREQ_256(n) (-(BUS_CLOCK>>8)/(n))


/*!	\brief A macro that calculates %TIMER_DATA(n) settings for a given frequency of n.
	will calculate the correct value for %TIMER_DATA(n) given the frequency in hertz
	(number of times the timer should overflow per second).

	<b>Example Usage:</b>
	<pre>
		//calls the timerCallBack function 5 times per second.
		timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(5), timerCallBack);
	</pre>

	Max frequency is: 32768Hz
	Min frequency is: 0.5Hz

	\note Use the appropriate macro depending on the used clock divider.
*/
#define TIMER_FREQ_1024(n) (-(BUS_CLOCK>>10)/(n))




/*!	\brief start a hardware timer.
	Callback is tied directly to interupt table and called directly resulting in less latency than the attached timer.
	\param timer 	The hardware timer to use (0 - 3).
	\param divider 	The timer channel clock divider (clock will tick at 33.513982 Mhz / divider)
	\param ticks	The number of ticks which must elapse before the timer overflows
	\param callback The callback to be called when the timer expires (if null no irq will be generated by the timer)
*/
void timerStart(int timer, ClockDivider divider, u16 ticks, VoidFn callback);


/*!	\brief returns the ticks elapsed since the last call to timerElapsed().
	\param timer The hardware timer to use (0 - 3).
	\return The number of ticks which have elapsed since the last callto timerElapsed().
*/
u16 timerElapsed(int timer);


static inline
/*!	\brief returns the raw ticks of the specified timer.
	\param timer The hardware timer to use (0 - 3).
	\return the raw ticks of the specified timer data register.
*/
u16 timerTick(int timer) {
	return TIMER_DATA(timer);
}


/*!	\brief pauses the specified timer.
	\param timer The hardware timer to use (0 - 3).
	\return The number of ticks which have elapsed since the last callto timerElapsed().
*/
u16 timerPause(int timer);


static inline
/*!	\brief unpauses the specified timer.
	\param timer The hardware timer to use (0 - 3).
*/
void timerUnpause(int timer) {
	TIMER_CR(timer) |= TIMER_ENABLE;
}


/*!	\brief Stops the specified timer.
	\param timer The hardware timer to use (0 - 3).
	\return The number of ticks which have elapsed since the last callto timerElapsed().
*/
u16 timerStop(int timer);

/*!	\brief begins cpu Timing using two timers for 32bit resolution.
	\param timer The base hardware timer to use (0 - 2).
*/
void cpuStartTiming(int timer);


/*!	\brief returns the number of ticks which have elapsed since cpuStartTiming.
	\return The number of ticks which have elapsed since cpuStartTiming.
*/
u32 cpuGetTiming();

/*!	\brief ends cpu Timing.
	\return The number of ticks which have elapsed since cpuStartTiming.
*/
u32 cpuEndTiming();

static inline
u32 timerTicks2usec(u32 ticks) {
	return (((u64)ticks)*1000000)/BUS_CLOCK;
}
static inline
u32 timerTicks2msec(u32 ticks) {
	return (((u64)ticks)*1000)/BUS_CLOCK;
}

//use the macro versions...
static inline u16 timerFreqToTicks_1(int freq) {return -BUS_CLOCK / freq;}
static inline u16 timerFreqToTicks_64(int freq) {return (-BUS_CLOCK >> 6) / freq;}
static inline u16 timerFreqToTicks_256(int freq) {return (-BUS_CLOCK >> 8) / freq;}
static inline u16 timerFreqToTicks_1024(int freq) {return (-BUS_CLOCK >> 10) / freq;}



#ifdef __cplusplus
}
#endif

#endif //NDS_TIMERS_INCLUDE
