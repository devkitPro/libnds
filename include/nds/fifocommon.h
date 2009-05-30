#ifndef FIFOCOMMON_H
#define FIFOCOMMON_H

#include "ndstypes.h"

/*! \file fifocommon.h 
\brief low level FIFO API. 
*/

typedef enum {
   FIFO_PM		= 0,
   FIFO_SOUND	= 1,
   FIFO_SYSTEM	= 2, 
   FIFO_MAXMOD	= 3,
   FIFO_DSWIFI	= 4,
   FIFO_RSDV_01	= 5,
   FIFO_RSVD_02 = 6,
   FIFO_RSVD_03 = 7,
   FIFO_USER_01	= 8,
   FIFO_USER_02	= 9,
   FIFO_USER_03	= 10,
   FIFO_USER_04	= 11,
   FIFO_USER_05	= 12,
   FIFO_USER_06	= 13,
   FIFO_USER_07	= 14,
   FIFO_USER_08	= 15,
} FifoChannels;


typedef enum {
   	SOUND_SET_PAN		= 0 << 20,
	SOUND_SET_VOLUME	= 1 << 20,
	SOUND_SET_FREQ		= 2 << 20,
	SOUND_MASTER_ENABLE		= 3 << 20,
	SOUND_MASTER_DISABLE       = 4 << 20 ,
	SOUND_PAUSE			= 5 << 20 ,
	SOUND_RESUME		= 6 << 20 ,
	SOUND_KILL			= 7 << 20 ,
	SOUND_SET_MASTER_VOL	= 8 << 20 ,
	MIC_STOP		 = 9 << 20

} FifoSoundCommand;

typedef enum {
	SYS_REQ_TOUCH,
	SYS_REQ_KEYS,
	SYS_REQ_TIME,

} FifoSystemCommands;

typedef enum {
	PM_REQ_ON = (1<<16),
	PM_REQ_OFF  = (2<<16),
	PM_REQ_LED  = (3<<16),
	PM_REQ_SLEEP  = (4<<16),
	PM_REQ_SLEEP_DISABLE  = (5<<16),
	PM_REQ_SLEEP_ENABLE  = (6<<16)
}FifoPMCommands;

typedef enum {
	WIFI_ENABLE,
	WIFI_DISABLE,
	WIFI_SYNC,
	WIFI_STARTUP
} FifoWifiCommands;



/*! \enum PM_LedBlinkMode
\brief Power Management LED blink mode control bits 
*/
typedef enum {
	PM_LED_ON		=(0<<4),	/*!< \brief  Steady on */
	PM_LED_SLEEP	=(1<<4),	/*!< \brief Blinking, mostly off */
	PM_LED_BLINK	=(3<<4),	/*!< \brief Blinking, mostly on */
}PM_LedBlinkMode;


#ifdef __cplusplus
extern "C" {
#endif

#if FIFO_RIGOROUS_ERROR_CHECKING
	int fifoError(char *, ...); // expected to be defined externally.
#endif

// Handlers are called when new data arrives - they're called from interrupt level, but well secured so not too much caution is necessary.
// Except that you shouldn't alloc/free or printf from within them, just to be safe.
// FifoAddressHandlerFunc - Provides callback with the sent address and the callback's user data

typedef void (*FifoAddressHandlerFunc)(void * address, void * userdata);

// FifoValue32HandlerFunc - Provides callback with the sent value and the callback's user data
typedef void (*FifoValue32HandlerFunc)(u32 value32, void * userdata);
// FifoDatamsgHandlerFunc - Provides callback with the number of bytes sent and the callback's user data
// This callback must call fifoGetData to actually retrieve the data. If it doesn't, the data will be destroyed on return.
typedef void (*FifoDatamsgHandlerFunc)(int num_bytes, void * userdata);

/*! \fn  fifoInit()
	\brief Initializes the fifo system.

	Attempts to sync with the other CPU, if it fails, fifo services won't be provided.

	\note call irqInit() before calling this function.
*/
bool fifoInit();

/*! \fn fifoSendAddress(int channel, void *address)
	\param channel channel number to send to
	\param address address to send
	\brief Send an address
	
	Transmits an address in the range 0x02000000-0x023FFFFF to the other CPU.
*/
bool fifoSendAddress(int channel, void *address);

/*! \fn fifoSendValue32(int channel, u32 value32)
	\param channel channel number to send to
	\param value32 32bit value to send
	\brief Send a 32bit value
	
	Transmits a 32bit value to the other CPU.

	\note Transfer is more efficient if the top 8 bits are zero. So sending smaller values or bitmasks that don't include the top bits is preferred.

*/
bool fifoSendValue32(int channel, u32 value32);

/*! \fn fifoSendDatamsg(int channel, int num_bytes, u8 * data_array)
	\param channel channel number to send to
	\param num_bytes number of bytes to send
	\param data_array pointer to data array
	
	\brief Send a sequence of bytes to the other CPU
	
	num_bytes can be between 0 and FIFO_MAX_DATA_BYTES (see above) - sending 0 bytes can be useful sometimes ...

*/
bool fifoSendDatamsg(int channel, int num_bytes, u8 * data_array);

// Note about setting handlers: if a handler is already set, the function will FAIL and return false unless you're trying to unset the handler.
//  Alternately, use the FifoForce*Handler functions, to set regardless - these are specificly designed to try to prevent misuse.
// Another note on setting handlers: Setting the handler for a channel feeds the queue of buffered messages to the new handler, if there are any unread messages.
// FifoSetAddressHandler - Set a callback to receive incoming address messages on a specific channel.

/*! \fn fifoSetAddressHandler(int channel, FifoAddressHandlerFunc newhandler, void * userdata)
	\param channel
	\param newhandler
	\param userdata
	
	\brief Set user address message callback

	Set a callback to receive incoming address messages on a specific channel.
*/
bool fifoSetAddressHandler(int channel, FifoAddressHandlerFunc newhandler, void * userdata);

/*! \fn fifoSetValue32Handler(int channel, FifoValue32HandlerFunc newhandler, void * userdata)
	\param channel
	\param newhandler
	\param userdata
	
	\brief Set user value32 message callback

	Set a callback to receive incoming value32 messages on a specific channel.
*/
bool fifoSetValue32Handler(int channel, FifoValue32HandlerFunc newhandler, void * userdata);
// FifoSetDatamsgHandler - Set a callback to receive incoming data sequences on a specific channel.

/*! \fn fifoSetValue32Handler(int channel, FifoValue32HandlerFunc newhandler, void * userdata)
	\param channel
	\param newhandler
	\param userdata
	
	\brief Set user data message callback

	Set a callback to receive incoming data messages on a specific channel.
*/
bool fifoSetDatamsgHandler(int channel, FifoDatamsgHandlerFunc newhandler, void * userdata);


// Check functions see if there are any of a specific type of data in queue.
// Note: these functions will never return true if there is a callback for the specific channel and type of data being checked.
bool fifoCheckAddress(int channel);
bool fifoCheckValue32(int channel);
bool fifoCheckDatamsg(int channel);
// fifoCheckDatamsgLength - gets the number of bytes in the queue for the first data entry. (or -1 if there are no entries)
int fifoCheckDatamsgLength(int channel);

// fifoGetAddress - Get the first address in queue for a specific channel (or NULL if there is none)
void * fifoGetAddress(int channel);
// fifoGetValue32 - Get the first value32 in queue for a specific channel (will return 0 if there is no message)
u32 fifoGetValue32(int channel);
// fifoGetDatamsg - Read a data message - returns the number of bytes written (or -1 if there is no message)
//  Warning: If your buffer is not big enough, you may lose data! Check the data length first if you're not sure what the size is.
int fifoGetDatamsg(int channel, int buffersize, u8 * destbuffer);



#ifdef __cplusplus
};
#endif


#endif
