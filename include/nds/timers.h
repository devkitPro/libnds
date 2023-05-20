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

	\warning
	Timers 0 and 1 are used on the ARM7 by microphone and maxmod respectively.

	\warning
	Timers 2 and 3 are reserved by Calico for its tick scheduling system -
	DO NOT use them directly. Please refer to calico/system/tick.h.
*/


#ifndef NDS_TIMERS_INCLUDE
#define NDS_TIMERS_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif


#include <nds/ndstypes.h>
#include <calico/system/tick.h>
#include <calico/nds/timer.h>



//!	Returns a dereferenced pointer to the data register for timer control Register.
/*!	<b>Example Usage:</b> %TIMER_CR(x) = %TIMER_ENABLE | %ClockDivider_64;

	Possible bit defines:

	\see TIMER_ENABLE
	\see TIMER_IRQ_REQ
	\see TIMER_CASCADE
	\see ClockDivider
*/
#define TIMER_CR(n) REG_TMxCNT_H(n)

//!	Same as %TIMER_CR(0).
#define TIMER0_CR   REG_TMxCNT_H(0)
//!	Same as %TIMER_CR(1).
#define TIMER1_CR   REG_TMxCNT_H(0)


//!	Returns a dereferenced pointer to the data register for timer number "n".
/*!	\see TIMER_CR(n)
	\see TIMER_FREQ(n)

	%TIMER_DATA(n) when set will latch that value into the counter.  Every time the
	counter rolls over, %TIMER_DATA(0) will return to the latched value.  This allows
	you to control the frequency of the timer using the following formula:\n
		%TIMER_DATA(x) = -(BUS_CLOCK/(freq * divider));

	<b>Example Usage:</b>
	%TIMER_DATA(0) = value;  were 0 can be 0 through 3 and value is 16 bits.
*/
#define TIMER_DATA(n)  REG_TMxCNT_L(n)

//!	Same as %TIMER_DATA(0).
#define TIMER0_DATA    REG_TMxCNT_L(0)
//!	Same as %TIMER_DATA(1).
#define TIMER1_DATA    REG_TMxCNT_L(1)



//! the speed in which the timer ticks in hertz.
#define BUS_CLOCK TIMER_BASE_FREQ


#define TIMER_IRQ_REQ   TIMER_ENABLE_IRQ


//! allowable timer clock dividers.
typedef enum {
	ClockDivider_1    = TIMER_PRESCALER_1,		//!< divides the timer clock by 1 (~33513.982 kHz)
	ClockDivider_64   = TIMER_PRESCALER_64,		//!< divides the timer clock by 64 (~523.657 kHz)
	ClockDivider_256  = TIMER_PRESCALER_256,	//!< divides the timer clock by 256 (~130.914 kHz)
	ClockDivider_1024 = TIMER_PRESCALER_1024	//!< divides the timer clock by 1024 (~32.7284 kHz)
}ClockDivider;


//use the ClockDivider enum.
//	Causes the timer to count at 33.514 MHz.
#define TIMER_DIV_1     TIMER_PRESCALER_1
//	Causes the timer to count at (33.514 / 64) MHz.
#define TIMER_DIV_64    TIMER_PRESCALER_64
//	Causes the timer to count at (33.514 / 256) MHz.
#define TIMER_DIV_256   TIMER_PRESCALER_256
//	Causes the timer to count at (33.514 / 1024) MHz.
#define TIMER_DIV_1024  TIMER_PRESCALER_1024



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
	Callback is tied directly to interrupt table and called directly, resulting in less latency than the attached timer.
	\param timer 	The hardware timer to use (0 - 1).
	\param divider 	The timer channel clock divider (clock will tick at 33.513982 MHz / divider)
	\param ticks	The value to reload into the counter when the timer overflows (0x10000 - timer period expressed in ticks)
	\param callback The callback to be called when the timer expires (if null, no IRQ will be generated by the timer)
*/
void timerStart(unsigned timer, ClockDivider divider, u16 ticks, VoidFn callback);


/*!	\brief returns the ticks elapsed since the last call to timerElapsed().
	\param timer The hardware timer to use (0 - 1).
	\return The number of ticks which have elapsed since the last call to timerElapsed().
*/
u16 timerElapsed(unsigned timer);


static inline
/*!	\brief returns the raw ticks of the specified timer.
	\param timer The hardware timer to use (0 - 1).
	\return the raw ticks of the specified timer data register.
*/
u16 timerTick(unsigned timer) {
	return TIMER_DATA(timer);
}


/*!	\brief pauses the specified timer.
	\param timer The hardware timer to use (0 - 1).
	\return The number of ticks which have elapsed since the last call to timerElapsed().
*/
u16 timerPause(unsigned timer);


static inline
/*!	\brief unpauses the specified timer.
	\param timer The hardware timer to use (0 - 1).
*/
void timerUnpause(unsigned timer) {
	TIMER_CR(timer) |= TIMER_ENABLE;
}


/*!	\brief Stops the specified timer.
	\param timer The hardware timer to use (0 - 1).
	\return The number of ticks which have elapsed since the last call to timerElapsed().
*/
u16 timerStop(unsigned timer);

/*!	\brief Begins cpu Timing using two timers for 32bit resolution.
*/
void cpuStartTiming(int unused);


/*!	\brief returns the number of ticks which have elapsed since cpuStartTiming.
	\return The number of ticks which have elapsed since cpuStartTiming.
*/
u32 cpuGetTiming(void);

/*!	\brief ends cpu Timing.
	\return The number of ticks which have elapsed since cpuStartTiming.
*/
static inline
u32 cpuEndTiming(void) {
	return cpuGetTiming();
}

static inline
u32 timerTicks2usec(u32 ticks) {
	return (((u64)ticks)*1000000)/BUS_CLOCK;
}
static inline
u32 timerTicks2msec(u32 ticks) {
	return (((u64)ticks)*1000)/BUS_CLOCK;
}

//use the macro versions...
static inline u16 timerFreqToTicks_1(unsigned freq) {return TIMER_FREQ(freq);}
static inline u16 timerFreqToTicks_64(unsigned freq) {return TIMER_FREQ_64(freq);}
static inline u16 timerFreqToTicks_256(unsigned freq) {return TIMER_FREQ_256(freq);}
static inline u16 timerFreqToTicks_1024(unsigned freq) {return TIMER_FREQ_1024(freq);}



#ifdef __cplusplus
}
#endif

#endif //NDS_TIMERS_INCLUDE
