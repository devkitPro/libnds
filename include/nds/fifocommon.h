#ifndef FIFOCOMMON_H
#define FIFOCOMMON_H

/*! \file fifocommon.h 
\brief low level FIFO API. 
*/

// Some aspects of this configuration can be changed...

// FIFO_CHANNEL_BITS - number of bits used to specify the channel in a packet - default=4
#define FIFO_CHANNEL_BITS				4

// FIFO_MAX_DATA_WORDS - maximum number of words that can be sent in a fifo message
#define FIFO_MAX_DATA_WORDS				32

// FIFO_RIGOROUS_ERROR_CHECKING - Verify all internal buffer transactions, mostly for debugging this library. Unless there's memory corruption this shouldn't be enabled normally.
// If there is an error, the lib will call int fifoError(char *, ...) - which isn't defined by the fifo lib. So it's best to handle it if you want to debug. :)
// All of the errors trapped represent serious problems, so it's not a bad idea to halt in fifoError()
#define FIFO_RIGOROUS_ERROR_CHECKING	1

// FIFO_BUFFER_ENTRIES - number of words that can be stored temporarily while waiting to deque them - default=256 arm7, 512 arm9
#ifdef ARM9
#define FIFO_BUFFER_ENTRIES				512
#else // ARM7
#define FIFO_BUFFER_ENTRIES				256
#endif

// Note about memory commitments:
// The memory overhead of this library (per CPU) is:
// 16 + (Num Channels)*32 + FIFO_BUFFER_ENTRIES*8
// for 16 channels and 256 entries, this is 16+512+2048 = 2576 bytes of ram.
// Some padding may be added by the compiler, though.

// Common interface: Both the arm7 and arm9 have the same set of functions.
// for sending/receiving, true=success - for checking things, true=exists
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
// FifoDatamsgHandlerFunc - Provides callback with the number of words sent and the callback's user data
// This callback must call fifoGetData to actually retrieve the data. If it doesn't, the data will be destroyed on return.
typedef void (*FifoDatamsgHandlerFunc)(int num_words, void * userdata);

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

/*! \fn fifoSendDatamsg(int channel, int num_words, u32 * data_array)
	\param channel channel number to send to
	\param num_words number of 32bit words to send
	\param data_array pointer to data array
	
	\brief Send a sequence of 32bit words to the other CPU
	
	num_words can be between 0 and FIFO_MAX_DATA_WORDS (see above) - sending 0 words can be useful sometimes ...

*/
bool fifoSendDatamsg(int channel, int num_words, u32 * data_array);

// Note about setting handlers: if a handler is already set, the function will FAIL and return false unless you're trying to unset the handler.
//  Alternately, use the FifoForce*Handler functions, to set regardless - these are specificly designed to try to prevent misuse.
// Another note on setting handlers: Setting the handler for a channel feeds the queue of buffered messages to the new handler, if there are any unread messages.
// FifoSetAddressHandler - Set a callback to receive incoming address messages on a specific channel.
bool fifoSetAddressHandler(int channel, FifoAddressHandlerFunc newhandler, void * userdata);
// FifoSetValue16Handler - Set a callback to receive incoming value16 messages on a specific channel.
bool fifoSetValue32Handler(int channel, FifoValue32HandlerFunc newhandler, void * userdata);
// FifoSetDatamsgHandler - Set a callback to receive incoming data sequences on a specific channel.
bool fifoSetDatamsgHandler(int channel, FifoDatamsgHandlerFunc newhandler, void * userdata);

// fifoForceAddressHandler - Set a callback to receive incoming address messages on a specific channel. (and always succeed)
void fifoForceAddressHandler(int channel, FifoAddressHandlerFunc newhandler, void * userdata);
// fifoForceValue16Handler - Set a callback to receive incoming value16 messages on a specific channel. (and always succeed)
void fifoForceValue32Handler(int channel, FifoValue32HandlerFunc newhandler, void * userdata);
// FifoForceDatamsgHandler - Set a callback to receive incoming data sequences on a specific channel. (and always succeed)
void fifoForceDatamsgHandler(int channel, FifoDatamsgHandlerFunc newhandler, void * userdata);

// Check functions see if there are any of a specific type of data in queue.
// Note: these functions will never return true if there is a callback for the specific channel and type of data being checked.
bool fifoCheckAddress(int channel);
bool fifoCheckValue32(int channel);
bool fifoCheckDatamsg(int channel);
// fifoCheckDatamsgLength - gets the number of words in the queue for the first data entry. (or -1 if there are no entries)
int fifoCheckDatamsgLength(int channel);
// fifoCountDatamsg - returns the number of data messages in queue for a specific channel
int fifoCountDatamsg(int channel);

// fifoGetAddress - Get the first address in queue for a specific channel (or NULL if there is none)
void * fifoGetAddress(int channel);
// fifoGetValue32 - Get the first value32 in queue for a specific channel (will return 0 if there is no message)
u32 fifoGetValue32(int channel);
// fifoGetDatamsg - Read a data message - returns the number of words written (or -1 if there is no message)
//  Warning: If your buffer is not big enough, you may lose data! Check the data length first if you're not sure what the size is.
int fifoGetDatamsg(int channel, int buffersize, u32 * destbuffer);


// fifoGetBufferUsage - get the number of words in the buffer used, out of FIFO_BUFFER_ENTRIES
int fifoGetBufferUsage();

// fifoGetLostWordCount - get the number of words lost (due to buffer being completely full) - if this is ever >0, there is lost data and possibly desync problems.
int fifoGetLostWordCount();



// Internal functions - not for general public use.
void fifoInternalSendInterrupt();
void fifoInternalRecvInterrupt();
void fifoInternalProtect();
void fifoInternalUnprotect();
bool fifoInternalSend(u32 firstword, int extrawordcount, u32 * wordlist);
void fifoInternalBeginSend();
u32 fifoInternalRecvPeek();
u32 fifoInternalRecvPopFirst();
int fifoInternalRecvWordCount();
bool fifoInternalRecvPushBack(u32 word);
bool fifoInternalSendEmpty();
u32 fifoInternalSendDeque();
bool fifoInternalSendEnqueue(u32 word);
int fifoInternalAllocFreeBlock();
void fifoInternalFreeBlock(int blockid);
bool fifoInternalFreeCheck(int num_words);

#ifdef __cplusplus
};
#endif

// And some aspects of the configuration can't be changed
//  please don't edit below this line.

#define FIFO_ADDRESSDATA_SHIFT			0
#define FIFO_MINADDRESSDATABITS			22
#define FIFO_ADDRESSDATA_MASK			0x003FFFFF
#define FIFO_ADDRESSBASE				0x02000000
#define FIFO_ADDRESSCOMPATIBLE			0xFFC00000

#define FIFO_NUM_CHANNELS				(1<<FIFO_CHANNEL_BITS)
#define FIFO_CHANNEL_SHIFT				(32-FIFO_CHANNEL_BITS)
#define FIFO_CHANNEL_MASK				((1<<FIFO_CHANNEL_BITS)-1)


// addressbit indicates presence of an address
#define FIFO_ADDRESSBIT_SHIFT			(FIFO_CHANNEL_SHIFT-1)
#define FIFO_ADDRESSBIT					(1<<FIFO_ADDRESSBIT_SHIFT)
// immediatebit indicates presence of an immediate, if there is no address.
#define FIFO_IMMEDIATEBIT_SHIFT			(FIFO_CHANNEL_SHIFT-2)
#define FIFO_IMMEDIATEBIT				(1<<FIFO_IMMEDIATEBIT_SHIFT)
// extrabit indicates presence of an extra word for an immediate.
#define FIFO_EXTRABIT_SHIFT				(FIFO_CHANNEL_SHIFT-3)
#define FIFO_EXTRABIT					(1<<FIFO_EXTRABIT_SHIFT)


#define FIFO_VALUE32_MASK				(FIFO_EXTRABIT-1)


#define FIFO_BUFFER_TERMINATE			0xFFFF
#define FIFO_BUFFER_NEXTMASK			0xFFFF


// some guards to prevent misuse
#if ((FIFO_MINADDRESSDATABITS + FIFO_CHANNEL_BITS + 1) > 32)
#error "Too many channel bits - control word isn't big enough for address packet"
#endif


// some helpers

#define FIFO_PACK_ADDRESS(channel, address) \
	( ((channel)<<FIFO_CHANNEL_SHIFT) | FIFO_ADDRESSBIT | \
	  (((u32)(address)>>FIFO_ADDRESSDATA_SHIFT)&FIFO_ADDRESSDATA_MASK) )

#define FIFO_VALUE32_NEEDEXTRA(value32) \
	( ((value32)&(~FIFO_VALUE32_MASK))!=0 )

#define FIFO_PACK_VALUE32(channel, value32) \
	( ((channel)<<FIFO_CHANNEL_SHIFT) | FIFO_IMMEDIATEBIT | \
	(((value32))&FIFO_VALUE32_MASK) )

#define FIFO_PACK_VALUE32_EXTRA(channel) \
	( ((channel)<<FIFO_CHANNEL_SHIFT) | FIFO_IMMEDIATEBIT | FIFO_EXTRABIT )

#define FIFO_IS_ADDRESS_COMPATIBLE(address) \
	( ((u32)(address)&FIFO_ADDRESSCOMPATIBLE) == FIFO_ADDRESSBASE )

#define FIFO_PACK_DATAMSG_HEADER(channel, numwords) \
	( ((channel)<<FIFO_CHANNEL_SHIFT) | ((numwords)&FIFO_VALUE32_MASK) )



#define FIFO_IS_ADDRESS(dataword) (((dataword)&FIFO_ADDRESSBIT)!=0)

#define FIFO_IS_VALUE32(dataword) \
	( (((dataword)&FIFO_ADDRESSBIT)==0) && (((dataword)&FIFO_IMMEDIATEBIT)!=0) )

#define FIFO_IS_DATA(dataword) \
	(((dataword)&(FIFO_ADDRESSBIT|FIFO_IMMEDIATEBIT))==0)

#define FIFO_UNPACK_CHANNEL(dataword) \
	( ((dataword)>>FIFO_CHANNEL_SHIFT)&FIFO_CHANNEL_MASK )

#define FIFO_UNPACK_ADDRESS(dataword) \
	( (void *)((((dataword)&FIFO_ADDRESSDATA_MASK)<<FIFO_ADDRESSDATA_SHIFT) | FIFO_ADDRESSBASE) )

#define FIFO_UNPACK_VALUE32_NEEDEXTRA(dataword) \
	( ((dataword)&FIFO_EXTRABIT)!=0 )

#define FIFO_UNPACK_VALUE32_NOEXTRA(dataword) \
	((dataword)&FIFO_VALUE32_MASK)

#define FIFO_UNPACK_DATALENGTH(dataword) \
	((dataword)&FIFO_VALUE32_MASK)


// fifo buffer helpers

#define FIFO_BUFFERCONTROL_UNUSED		0
#define FIFO_BUFFERCONTROL_SENDWORD		1
#define FIFO_BUFFERCONTROL_RECVWORD		2
#define FIFO_BUFFERCONTROL_ADDRESS		3
#define FIFO_BUFFERCONTROL_VALUE32		4
#define FIFO_BUFFERCONTROL_DATASTART	5
#define FIFO_BUFFERCONTROL_DATA			6

#define FIFO_BUFFER_DATA(index) fifo_buffer[(index)*2+1]
#define FIFO_BUFFER_GETNEXT(index) (fifo_buffer[(index)*2]&FIFO_BUFFER_NEXTMASK)
#define FIFO_BUFFER_GETCONTROL(index) (fifo_buffer[(index)*2]>>28)
#define FIFO_BUFFER_GETEXTRA(index) ((fifo_buffer[(index)*2]>>16)&0xFFF)
#define FIFO_BUFFER_SETCONTROL(index,next,control,extra) \
	fifo_buffer[(index)*2] = ( ((next)&FIFO_BUFFER_NEXTMASK) | ((control)<<28) | (((extra)&0xFFF)<<16) )
#define FIFO_BUFFER_SETNEXT(index, next) \
	fifo_buffer[(index)*2] = ( ((next)&FIFO_BUFFER_NEXTMASK) | (fifo_buffer[(index)*2]& (~FIFO_BUFFER_NEXTMASK)) )


#endif
