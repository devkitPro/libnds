#ifndef FIFOCOMMON_H
#define FIFOCOMMON_H

#include "ndstypes.h"

/*! \file fifocommon.h
	\brief low level FIFO API.
*/

//! Enum values for the different fifo channels.
typedef enum {
	FIFO_PM			= 0,	/*!< \brief fifo channel reserved for power management. */
	FIFO_SOUND		= 1,	/*!< \brief fifo channel reserved for sound access. */
	FIFO_SYSTEM		= 2,	/*!< \brief fifo channel reserved for system functions. */
	FIFO_MAXMOD		= 3,	/*!< \brief fifo channel reserved for the maxmod library. */
	FIFO_DSWIFI		= 4,	/*!< \brief fifo channel reserved for the dswifi library. */
	FIFO_SDMMC		= 5,	/*!< \brief fifo channel reserved for dsi sdmmc control. */
	FIFO_FIRMWARE	= 6,	/*!< \brief fifo channel reserved for firmware access. */
	FIFO_RSVD_01	= 7,	/*!< \brief fifo channel reserved for future use. */
	FIFO_USER_01	= 8,	/*!< \brief fifo channel available for users. */
	FIFO_USER_02	= 9,	/*!< \brief fifo channel available for users. */
	FIFO_USER_03	= 10,	/*!< \brief fifo channel available for users. */
	FIFO_USER_04	= 11,	/*!< \brief fifo channel available for users. */
	FIFO_USER_05	= 12,	/*!< \brief fifo channel available for users. */
	FIFO_USER_06	= 13,	/*!< \brief fifo channel available for users. */
	FIFO_USER_07	= 14,	/*!< \brief fifo channel available for users. */
	FIFO_USER_08	= 15,	/*!< \brief fifo channel available for users. */
} FifoChannels;


//! Enum values for the fifo sound commands.
typedef enum {
   	SOUND_SET_PAN			= 0 << 20,
	SOUND_SET_VOLUME		= 1 << 20,
	SOUND_SET_FREQ			= 2 << 20,
	SOUND_SET_WAVEDUTY		= 3 << 20,
	SOUND_MASTER_ENABLE		= 4 << 20,
	SOUND_MASTER_DISABLE	= 5 << 20,
	SOUND_PAUSE				= 6 << 20,
	SOUND_RESUME			= 7 << 20,
	SOUND_KILL				= 8 << 20,
	SOUND_SET_MASTER_VOL	= 9 << 20,
	MIC_STOP		 		= 10 << 20
} FifoSoundCommand;


//! Enum values for the fifo system commands.
typedef enum {
	SYS_REQ_TOUCH,
	SYS_REQ_KEYS,
	SYS_REQ_TIME,
	SYS_SET_TIME,
	SDMMC_INSERT,
	SDMMC_REMOVE
} FifoSystemCommands;

typedef enum {
	SDMMC_HAVE_SD,
	SDMMC_SD_START,
	SDMMC_SD_IS_INSERTED,
	SDMMC_SD_STOP
} FifoSdmmcCommands;

typedef enum {
	FW_READ,
	FW_WRITE
} FifoFirmwareCommands;

//! Enum values for the fifo power management commands.
typedef enum {
	PM_REQ_ON				= (1<<16),
	PM_REQ_OFF				= (2<<16),
	PM_REQ_LED				= (3<<16),
	PM_REQ_SLEEP			= (4<<16),
	PM_REQ_SLEEP_DISABLE	= (5<<16),
	PM_REQ_SLEEP_ENABLE		= (6<<16),
	PM_REQ_BATTERY			= (7<<16),
	PM_DSI_HACK				= (8<<16)
}FifoPMCommands;

//! Enum values for the fifo wifi commands.
typedef enum {
	WIFI_ENABLE,
	WIFI_DISABLE,
	WIFI_SYNC,
	WIFI_STARTUP
} FifoWifiCommands;


//! Power Management LED blink mode control bits.
typedef enum {
	PM_LED_ON		= 0,	/*!< \brief  Steady on */
	PM_LED_SLEEP	= 1,	/*!< \brief Blinking, mostly off */
	PM_LED_BLINK	= 3,	/*!< \brief Blinking, mostly on */
}PM_LedBlinkMode;


#ifdef __cplusplus
extern "C" {
#endif

#if FIFO_RIGOROUS_ERROR_CHECKING
	int fifoError(char *, ...); // expected to be defined externally.
#endif


/*!
	\brief fifo callback function pointer with the sent address and the callback's user data.

	The handler is called when new data arrives.

	\note callback functions are called from interrupt level, but are well secured. not too much caution is necessary,
			but don't call alloc, free or printf from within them, just to be safe.
*/
typedef void (*FifoAddressHandlerFunc)(void * address, void * userdata);


/*!
	\brief fifo callback function pointer with the sent value and the callback's user data.

	The handler is called when new data arrives.

	\note callback functions are called from interrupt level, but are well secured. not too much caution is necessary,
			but don't call alloc, free or printf from within them, just to be safe.
*/
typedef void (*FifoValue32HandlerFunc)(u32 value32, void * userdata);


/*!
	\brief fifo callback function pointer with the number of bytes sent and the callback's user data

	The handler is called when new data arrives.
	This callback must call fifoGetData to actually retrieve the data. If it doesn't, the data will be destroyed on return.

	\note callback functions are called from interrupt level, but are well secured. not too much caution is necessary,
			but don't call alloc, free or printf from within them, just to be safe.
*/
typedef void (*FifoDatamsgHandlerFunc)(int num_bytes, void * userdata);

/*!
	\brief Initializes the fifo system.

	Attempts to sync with the other CPU, if it fails, fifo services won't be provided.

	\note call irqInit() before calling this function.

	\return true if syncing worked, false if something went wrong.
*/
bool fifoInit();


/*!
	\brief Send an address to an channel.

	Transmits an address in the range 0x02000000-0x023FFFFF to the other CPU.

	\param channel channel number to send to.
	\param address address to send.

	\return true if the address has been send, false if something went wrong.
*/
bool fifoSendAddress(int channel, void *address);


/*!
	\brief Send a 32bit value.

	Transmits a 32bit value to the other CPU.

	\param channel channel number to send to
	\param value32 32bit value to send

	\return true if the value has been send, false if something went wrong.

	\note Transfer is more efficient if the top 8 bits are zero. So sending smaller values or bitmasks that don't include the top bits is preferred.

*/
bool fifoSendValue32(int channel, u32 value32);


/*!
	\brief Send a sequence of bytes to the other CPU.

	num_bytes can be between 0 and FIFO_MAX_DATA_BYTES - sending 0 bytes can be useful sometimes...

	\param channel channel number to send to
	\param num_bytes number of bytes to send
	\param data_array pointer to data array

	\return true if the data message has been send, false if something went wrong.
*/
bool fifoSendDatamsg(int channel, int num_bytes, u8 * data_array);


/*!
	\brief Set user address message callback.

	Set a callback to receive incoming address messages on a specific channel.

	\param channel channel number to send to.
	\param newhandler a function pointer to the new handler function.
	\param userdata a pointer that will be passed on to the handler when it will be called.

	\return true if the handler has been set, false if something went wrong.

	\note Setting the handler for a channel feeds the queue of buffered messages to the new handler, if there are any unread messages.
*/
bool fifoSetAddressHandler(int channel, FifoAddressHandlerFunc newhandler, void * userdata);


/*!
	\brief Set user value32 message callback.

	Set a callback to receive incoming value32 messages on a specific channel.

	\param channel channel number to send to.
	\param newhandler a function pointer to the new handler function.
	\param userdata a pointer that will be passed on to the handler when it will be called.

	\return true if the handler has been set, false if something went wrong.

	\note Setting the handler for a channel feeds the queue of buffered messages to the new handler, if there are any unread messages.
*/
bool fifoSetValue32Handler(int channel, FifoValue32HandlerFunc newhandler, void * userdata);


/*!
	\brief Set user data message callback.

	Set a callback to receive incoming data messages on a specific channel.

	\param channel channel number to send to.
	\param newhandler a function pointer to the new handler function.
	\param userdata a pointer that will be passed on to the handler when it will be called.

	\return true if the handler has been set, false if something went wrong.

	\note Setting the handler for a channel feeds the queue of buffered messages to the new handler, if there are any unread messages.
*/
bool fifoSetDatamsgHandler(int channel, FifoDatamsgHandlerFunc newhandler, void * userdata);



/*!
	\brief checks if there is any addresses in the fifo queue.

	\param channel the channel to check.

	\return true if there is any addresses in the queue and if there isn't an address handler in place for the channel.
*/
bool fifoCheckAddress(int channel);


/*!
	\brief checks if there is any values in the fifo queue.

	\param channel the channel to check.

	\return true if there is any values in the queue and if there isn't a value handler in place for the channel.
*/
bool fifoCheckValue32(int channel);


/*!
	\brief checks if there is any data messages in the fifo queue.

	\param channel the channel to check.

	\return true if there is any data messages in the queue and if there isn't a data message handler in place for the channel.
*/
bool fifoCheckDatamsg(int channel);


/*!
	\brief gets the number of bytes in the queue for the first data entry.

	\param channel the channel to check.

	\return the number of bytes in the queue for the first data entry, or -1 if there are no entries.
*/
int fifoCheckDatamsgLength(int channel);


/*!
	\brief Get the first address in queue for a specific channel.

	\param channel the channel to check.

	\return the first address in queue, or NULL if there is none.
*/
void * fifoGetAddress(int channel);


/*!
	\brief Get the first value32 in queue for a specific channel.

	\param channel the channel to check.

	\return the first value32 in queue, or 0 if there is no message.
*/
u32 fifoGetValue32(int channel);


/*!
	\brief Reads a data message in a given buffer and returns the number of bytes written.

	\param channel the channel to check.
	\param buffersize the size of the buffer where the message will be copied to.
	\param destbuffer a pointer to the buffer where the message will be copied to.

	\return the number of bytes written, or -1 if there is no message.

	\warning If your buffer is not big enough, you may lose data! Check the data length first if you're not sure what the size is.
*/
int fifoGetDatamsg(int channel, int buffersize, u8 * destbuffer);


#ifdef __cplusplus
};
#endif


#endif
