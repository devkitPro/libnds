#include <nds/ndstypes.h>
#include <nds/interrupts.h>
#include <nds/ipc.h>
#include <nds/fifocommon.h>

//////////////////////////////////////////////////////////////////////////
// Global vars


// Note about memory commitments:
// The memory overhead of this library (per CPU) is:
// 16 + (Num Channels)*32 + FIFO_BUFFER_ENTRIES*8
// for 16 channels and 256 entries, this is 16+512+2048 = 2576 bytes of ram.
// Some padding may be added by the compiler, though.

// 28 bytes of global data
int fifo_freelist_start;
int fifo_initialized = 0;
int fifo_rcvdwords_start;
int fifo_rcvdwords_end;
int fifo_sendwords_start;
int fifo_sendwords_end;
int fifo_protect_levels = 0;
int fifo_send_status = 0;
int fifo_recv_status = 0;
int fifo_lost_words = 0; // if this ever increases, bad bad things happened.

// 32 bytes per channel
int			fifo_channel_start[FIFO_NUM_CHANNELS];
int			fifo_channel_end[FIFO_NUM_CHANNELS];

FifoAddressHandlerFunc		fifo_address_func[FIFO_NUM_CHANNELS];
void *						fifo_address_data[FIFO_NUM_CHANNELS];
FifoValue32HandlerFunc		fifo_value32_func[FIFO_NUM_CHANNELS];
void *						fifo_value32_data[FIFO_NUM_CHANNELS];
FifoDatamsgHandlerFunc		fifo_datamsg_func[FIFO_NUM_CHANNELS];
void *						fifo_datamsg_data[FIFO_NUM_CHANNELS];

// FIFO_BUFFER_ENTRIES*8 of global buffer space
u32			fifo_buffer[FIFO_BUFFER_ENTRIES*2];




// Setup functions

// fifoInit - Initializes the fifo system. Attempts to sync with the other CPU, if it fails, fifo services won't be provided.
bool fifoInit() {
	int i;
	irqDisable( IRQ_FIFO_EMPTY | IRQ_FIFO_NOT_EMPTY );
	fifo_initialized=0;
	fifo_freelist_start = 0;
	fifo_rcvdwords_end = fifo_rcvdwords_start = -1;
	fifo_sendwords_start = fifo_sendwords_end = -1;
	for(i=0;i<FIFO_NUM_CHANNELS;i++)
	{
		fifo_channel_start[i] = fifo_channel_end[i] = -1;
		fifo_address_data[i] = fifo_value32_data[i] = fifo_datamsg_data[i] = 0;
		fifo_address_func[i] = 0;
		fifo_value32_func[i] = 0;
		fifo_datamsg_func[i] = 0;
	}
	for(i=0;i<FIFO_BUFFER_ENTRIES;i++)
	{
		FIFO_BUFFER_DATA(i) = 0;
		FIFO_BUFFER_SETCONTROL(i, i+1, 0, 0);
	}
	FIFO_BUFFER_SETCONTROL(FIFO_BUFFER_ENTRIES-1, FIFO_BUFFER_TERMINATE, FIFO_BUFFERCONTROL_UNUSED, 0);

	// initialize fifo and sync to other cpu
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ | IPC_FIFO_SEND_IRQ;

	{
		int timeout_iterations = 10000;
		int needed_tocontinue = 3;
		int needed_toleave = 10;
		int exit_value = 14;
		int counted = 0;
		int my_value = 0, their_value;
		int delay = 10000;
		volatile int delaycount;
		REG_IPC_SYNC = 0;
		while(timeout_iterations>0)
		{
			// sync
			their_value = REG_IPC_SYNC&0x000F;
			if( ((my_value + 1)&15 ) == their_value ) counted++;
			my_value=(their_value+1)&15;
			REG_IPC_SYNC = my_value<<8;
			// exit conditions
			if( (their_value == exit_value) && counted>=needed_tocontinue ) break;
			if( counted >= needed_toleave ) break;
			// delay
			for(delaycount=0;delaycount<delay;delaycount++) {}
			timeout_iterations--;
		}
		if(timeout_iterations==0)
		{
			REG_IPC_SYNC=0;
			return false;
		}
		REG_IPC_SYNC = exit_value<<8;
	}

	fifo_protect_levels = 0;
	fifo_send_status = 0;
	fifo_recv_status = 0;
	fifo_lost_words = 0;
	irqSet(IRQ_FIFO_EMPTY,fifoInternalSendInterrupt);
	irqSet(IRQ_FIFO_NOT_EMPTY,fifoInternalRecvInterrupt);
	irqEnable( IRQ_FIFO_EMPTY | IRQ_FIFO_NOT_EMPTY );
	fifo_initialized=1;
	return true;
}

// Note about setting handlers: if a handler is already set, the function will FAIL and return false unless you're trying to unset the handler.
//  Alternately, use the FifoForce*Handler functions, to set regardless - these are specificly designed to try to prevent misuse.
// fifoSetAddressHandler - Set a callback to receive incoming address messages on a specific channel.
bool fifoSetAddressHandler(int channel, FifoAddressHandlerFunc newhandler, void * userdata)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	if(fifo_address_func[channel] && newhandler) return false;
	fifoForceAddressHandler(channel,newhandler,userdata);
	return true;
}
// fifoSetValue16Handler - Set a callback to receive incoming value16 messages on a specific channel.
bool fifoSetValue32Handler(int channel, FifoValue32HandlerFunc newhandler, void * userdata)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	if(fifo_value32_func[channel] && newhandler) return false;
	fifoForceValue32Handler(channel,newhandler,userdata);
	return true;
}

// fifoSetDatamsgHandler - Set a callback to receive incoming data sequences on a specific channel.
bool fifoSetDatamsgHandler(int channel, FifoDatamsgHandlerFunc newhandler, void * userdata)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	if(fifo_datamsg_func[channel] && newhandler) return false;
	fifoForceDatamsgHandler(channel,newhandler,userdata);
	return true;
}

// fifoForceAddressHandler - Set a callback to receive incoming address messages on a specific channel. (and always succeed)
void fifoForceAddressHandler(int channel, FifoAddressHandlerFunc newhandler, void * userdata)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return;
	fifoInternalProtect(); // must change both at once to not be subject to race conditions
	fifo_address_func[channel] = newhandler;
	fifo_address_data[channel] = userdata;
	if(newhandler)
	{
		while(fifoCheckAddress(channel))
		{
			newhandler( fifoGetAddress(channel), userdata );
		}
	}
	fifoInternalUnprotect();
}
// fifoForceValue16Handler - Set a callback to receive incoming value16 messages on a specific channel. (and always succeed)
void fifoForceValue32Handler(int channel, FifoValue32HandlerFunc newhandler, void * userdata)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return;
	fifoInternalProtect(); // must change both at once to not be subject to race conditions
	fifo_value32_func[channel] = newhandler;
	fifo_value32_data[channel] = userdata;
	if(newhandler)
	{
		while(fifoCheckValue32(channel))
		{
			newhandler( fifoGetValue32(channel), userdata );
		}
	}
	fifoInternalUnprotect();
}
// fifoForceDatamsgHandler - Set a callback to receive incoming data sequences on a specific channel. (and always succeed)
void fifoForceDatamsgHandler(int channel, FifoDatamsgHandlerFunc newhandler, void * userdata)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return;
	fifoInternalProtect(); // must change both at once to not be subject to race conditions
	fifo_datamsg_func[channel] = newhandler;
	fifo_datamsg_data[channel] = userdata;
	int msgcount;
	if(newhandler)
	{
		while( (msgcount=fifoCountDatamsg(channel)) >0 )
		{
			newhandler( fifoCheckDatamsgLength(channel), userdata );
			if(msgcount == fifoCountDatamsg(channel))
			{ // handler didn't remove message, so we'll do it.
				fifoGetDatamsg(channel,0,0);
			}
		}
	}
	fifoInternalUnprotect();
}



//////////////////////////////////////////////////////////////////////////
// Send / Receive functions


// fifoSendAddress - Send an address (from mainram only) to the other cpu (on a specific channel)
//  Address can be in the range of 0x02000000-0x023FFFFF
bool fifoSendAddress(int channel, void * address)
{
	u32 send_word;
	// check channel
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	// verify address
	if( !FIFO_IS_ADDRESS_COMPATIBLE(address) ) return false;
	// construct message
	send_word = FIFO_PACK_ADDRESS(channel, address);
	// send
	return fifoInternalSend(send_word, 0, 0);
}
// fifoSendValue32 - Sent a 32bit value to the other cpu (on a specific channel)
//  Note: Transfer is more efficient if the top 8 bits are zero. So sending smaller values or bitmasks that don't include the top bits is preferred.
bool fifoSendValue32(int channel, u32 value32)
{
	u32 send_first, send_extra[1];
	// check channel
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	// decide which path to take
	if( FIFO_VALUE32_NEEDEXTRA(value32) )
	{
		// construct message
		send_first = FIFO_PACK_VALUE32_EXTRA(channel);
		send_extra[0] = value32;
		// send message
		return fifoInternalSend(send_first, 1, send_extra);
	}
	else
	{
		// construct message
		send_first = FIFO_PACK_VALUE32(channel, value32);
		// send message
		return fifoInternalSend(send_first, 0, 0);
	}
}
// fifoSendDatamsg - Send a sequence of 32bit bytes to the other CPU (on a specific channel)
//  num_bytes can be between 0 and FIFO_MAX_DATA_BYTES (see above) - sending 0 words can be useful sometimes....
bool fifoSendDatamsg(int channel, int num_bytes, u8 * data_array)
{
	u32 send_first;
	// check channel
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	// check length
	if(num_bytes<0 || num_bytes>=FIFO_MAX_DATA_BYTES) return false;
	// check pointer (just for null)
	if(num_bytes>0 && !data_array) return false;
	// construct header
	send_first = FIFO_PACK_DATAMSG_HEADER(channel, num_bytes);
	// copy data into temporary buffer.
	u32 buffer_array[(FIFO_MAX_DATA_BYTES+3)/4];
	int num_words = (num_bytes+3)>>2;
	int i;
	buffer_array[num_bytes>>2]=0; // zero out last few bytes
	for(i=0;i<num_bytes;i++)
	{ // yes this is slow. but it's probably not that bad; Consider changing this to use memcpy.
		((u8*)buffer_array)[i] = data_array[i];
	}
	// send message
	return fifoInternalSend(send_first, num_words, buffer_array);
}

// Check functions see if there are any of a specific type of data in queue.
bool fifoCheckAddress(int channel)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	fifoInternalProtect();
	int block;
	block = fifo_channel_start[channel];
	if(block!=-1)
	{
		while(block!=FIFO_BUFFER_TERMINATE)
		{
			if(FIFO_BUFFER_GETCONTROL(block)==FIFO_BUFFERCONTROL_ADDRESS)
			{
				fifoInternalUnprotect();
				return true;
			}
			block = FIFO_BUFFER_GETNEXT(block);
		}
	}
	fifoInternalUnprotect();
	return false;
}

bool fifoCheckValue32(int channel)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	fifoInternalProtect();
	int block;
	block = fifo_channel_start[channel];
	if(block!=-1)
	{
		while(block!=FIFO_BUFFER_TERMINATE)
		{
			if(FIFO_BUFFER_GETCONTROL(block)==FIFO_BUFFERCONTROL_VALUE32)
			{
				fifoInternalUnprotect();
				return true;
			}
			block = FIFO_BUFFER_GETNEXT(block);
		}
	}
	fifoInternalUnprotect();
	return false;
}

bool fifoCheckDatamsg(int channel)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	fifoInternalProtect();
	int block;
	block = fifo_channel_start[channel];
	if(block!=-1)
	{
		while(block!=FIFO_BUFFER_TERMINATE)
		{
			if(FIFO_BUFFER_GETCONTROL(block)==FIFO_BUFFERCONTROL_DATASTART)
			{
				fifoInternalUnprotect();
				return true;
			}
			block = FIFO_BUFFER_GETNEXT(block);
		}
	}
	fifoInternalUnprotect();
	return false;
}

// fifoCheckDatamsgLength - gets the number of bytes in the queue for the first data entry. (or -1 if there are no entries)
int fifoCheckDatamsgLength(int channel)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	fifoInternalProtect();
	int block;
	block = fifo_channel_start[channel];
	if(block!=-1)
	{
		while(block!=FIFO_BUFFER_TERMINATE)
		{
			if(FIFO_BUFFER_GETCONTROL(block)==FIFO_BUFFERCONTROL_DATASTART)
			{
				int n_bytes = FIFO_BUFFER_GETEXTRA(block);
				fifoInternalUnprotect();
				return n_bytes;
			}
			block = FIFO_BUFFER_GETNEXT(block);
		}
	}
	fifoInternalUnprotect();
	return -1;
}

// fifoCountDatamsg - returns the number of data messages in queue for a specific channel
int fifoCountDatamsg(int channel)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	fifoInternalProtect();
	int block;
	int num_msgs = 0;
	block = fifo_channel_start[channel];
	if(block!=-1)
	{
		while(block!=FIFO_BUFFER_TERMINATE)
		{
			if(FIFO_BUFFER_GETCONTROL(block)==FIFO_BUFFERCONTROL_DATASTART)
			{
				num_msgs++;
			}
			block = FIFO_BUFFER_GETNEXT(block);
		}
	}
	fifoInternalUnprotect();
	return num_msgs;
}

// fifoGetAddress - Get the first address in queue for a specific channel (or NULL if there is none)
void * fifoGetAddress(int channel)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	fifoInternalProtect();
	int block, lastblock;
	lastblock=-1;
	block = fifo_channel_start[channel];
	if(block!=-1)
	{
		while(block!=FIFO_BUFFER_TERMINATE)
		{
			if(FIFO_BUFFER_GETCONTROL(block)==FIFO_BUFFERCONTROL_ADDRESS)
			{
				// unlink block, and return data
				void * data = (void *) FIFO_BUFFER_DATA(block);
				if(lastblock==-1) // check if at start of list
				{ // was first block
					fifo_channel_start[channel] = FIFO_BUFFER_GETNEXT(block);
					if(fifo_channel_start[channel]==FIFO_BUFFER_TERMINATE) fifo_channel_start[channel]=-1;
				}
				else
				{ // wasn't first block
					FIFO_BUFFER_SETNEXT(lastblock,FIFO_BUFFER_GETNEXT(block));
				}
				if(FIFO_BUFFER_GETNEXT(block)==FIFO_BUFFER_TERMINATE) // check if at end of list
				{ // was last block
					fifo_channel_end[channel]=lastblock;
				}
				fifoInternalFreeBlock(block);

				fifoInternalUnprotect();
				return data;
			}
			lastblock=block;
			block = FIFO_BUFFER_GETNEXT(block);
		}
	}
	fifoInternalUnprotect();
	return 0; // no address in queue
}
// fifoGetValue32 - Get the first value32 in queue for a specific channel (will return 0 if there is no message)
u32 fifoGetValue32(int channel)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	fifoInternalProtect();
	int block, lastblock;
	lastblock=-1;
	block = fifo_channel_start[channel];
	if(block!=-1)
	{
		while(block!=FIFO_BUFFER_TERMINATE)
		{
			if(FIFO_BUFFER_GETCONTROL(block)==FIFO_BUFFERCONTROL_VALUE32)
			{
				// unlink block, and return data
				u32 data = FIFO_BUFFER_DATA(block);
				if(lastblock==-1) // check if at start of list
				{ // was first block
					fifo_channel_start[channel] = FIFO_BUFFER_GETNEXT(block);
					if(fifo_channel_start[channel]==FIFO_BUFFER_TERMINATE) fifo_channel_start[channel]=-1;
				}
				else
				{ // wasn't first block
					FIFO_BUFFER_SETNEXT(lastblock,FIFO_BUFFER_GETNEXT(block));
				}
				if(FIFO_BUFFER_GETNEXT(block)==FIFO_BUFFER_TERMINATE) // check if at end of list
				{ // was last block
					fifo_channel_end[channel]=lastblock;
				}
				fifoInternalFreeBlock(block);

				fifoInternalUnprotect();
				return data;
			}
			lastblock=block;
			block = FIFO_BUFFER_GETNEXT(block);
		}
	}
	fifoInternalUnprotect();
	return 0; // no value32 in queue
}
// fifoGetDatamsg - Read a data message - returns the number of bytes written (or -1 if there is no message)
//  Warning: If your buffer is not big enough, you may lose data! Check the data length first if you're not sure what the size is.
int fifoGetDatamsg(int channel, int buffersize, u8 * destbuffer)
{
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return -1;
	if(buffersize>0 && destbuffer==0) return -1; // can't have null pointer if you expect to receive anything
	fifoInternalProtect();
	int block, lastblock;
	lastblock=-1;
	block = fifo_channel_start[channel];
	if(block!=-1)
	{
		while(block!=FIFO_BUFFER_TERMINATE)
		{
			if(FIFO_BUFFER_GETCONTROL(block)==FIFO_BUFFERCONTROL_DATASTART)
			{
				// unlink block, and return data
				int n_bytes, n_words, blockend, i;
				int n_copy, blocktmp;
				n_bytes = FIFO_BUFFER_GETEXTRA(block);
				n_words = (n_bytes+3)>>2;
				n_copy=n_bytes;
				if(n_copy>buffersize) n_copy=buffersize;
				blockend=block;
				for(i=1;i<n_words;i++) // figure out related blocks
				{
					blockend = FIFO_BUFFER_GETNEXT(blockend);
#if (FIFO_RIGOROUS_ERROR_CHECKING)
					if( blockend == FIFO_BUFFER_TERMINATE ) fifoError("FifoGetDatamsg: channel list terminated in the middle of a data message! ch%i start%i blockstart%i", channel, fifo_channel_start[channel], block );
					if( FIFO_BUFFER_GETCONTROL( blockend ) != FIFO_BUFFERCONTROL_DATA ) fifoError("FifoGetDatamsg: Incorrect block type in the middle of a data message! expected %i, got %i", FIFO_BUFFERCONTROL_DATA, FIFO_BUFFER_GETCONTROL( blockend ) );
					if( FIFO_BUFFER_GETEXTRA( blockend ) != n_words-i ) fifoError("FifoGetDatamsg: Sanity check failed in data message! expected %i, got %i", n_words-i, FIFO_BUFFER_GETEXTRA( blockend ) );
#endif
				}
				// unlink list of blocks
				if(lastblock==-1) // check if at start of list
				{ // was first block
					fifo_channel_start[channel] = FIFO_BUFFER_GETNEXT(blockend);
					if(fifo_channel_start[channel]==FIFO_BUFFER_TERMINATE) fifo_channel_start[channel]=-1;
				}
				else
				{ // wasn't first block
					FIFO_BUFFER_SETNEXT(lastblock,FIFO_BUFFER_GETNEXT(blockend));
				}
				if(FIFO_BUFFER_GETNEXT(blockend)==FIFO_BUFFER_TERMINATE) // check if at end of list
				{ // was last block
					fifo_channel_end[channel]=lastblock;
				}

				// copy data!
				blocktmp = block;
				for(i=0;i<n_copy;i++)
				{
					destbuffer[i] = FIFO_BUFFER_DATA_BYTE(blocktmp, i&3);
					if((i&3)==3) 
					{
						blocktmp = FIFO_BUFFER_GETNEXT(blocktmp);
					}
				}

				// free related blocks
				blocktmp = -1;
				while( blocktmp != blockend )
				{
					blocktmp = block;
					block = FIFO_BUFFER_GETNEXT(block);
					fifoInternalFreeBlock(blocktmp);
				}
				
				fifoInternalUnprotect();
				return n_copy;
			}
			lastblock=block;
			block = FIFO_BUFFER_GETNEXT(block);
		}
	}
	fifoInternalUnprotect();
	return 0; // no value32 in queue
}










// fifoGetBufferUsage - get the number of words in the buffer used, out of FIFO_BUFFER_ENTRIES
int fifoGetBufferUsage()
{
	fifoInternalProtect();
	int num_words;
	int block = fifo_freelist_start;
	if(block==-1) { fifoInternalUnprotect(); return FIFO_BUFFER_ENTRIES; }
	num_words=1;
	while(num_words>0 && FIFO_BUFFER_GETNEXT(block)!=FIFO_BUFFER_TERMINATE)
	{
		num_words++;
		block = FIFO_BUFFER_GETNEXT(block);
	}
	fifoInternalUnprotect();
	return FIFO_BUFFER_ENTRIES-num_words;
}

// fifoGetLostWordCount - get the number of words lost (due to buffer being completely full) - if this is ever >0, there is lost data and possibly desync problems.
int fifoGetLostWordCount()
{
	return fifo_lost_words;
}




//////////////////////////////////////////////////////////////////////////
// Internal functions
// Internal functions - not for general public use.


void fifoInternalSendInterrupt()
{
	fifo_protect_levels = 1; // prevent protect calls from clearing necessary bits. 
	// Protect is unnecessary because all interrupts are disabled in this handler
	if(fifoInternalSendEmpty())
	{ // no more words to send!
		fifo_send_status = 0; // unset flag (cause we know we won't get more interrupts now.)
	}
	else
	{
		fifo_send_status = 0; // unset the flag til we write something
		while( !(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL) && !fifoInternalSendEmpty() )
		{
			REG_IPC_FIFO_TX = fifoInternalSendDeque();
			fifo_send_status = 1; // we definitely will get another interrupt now
		}
	}
	fifo_protect_levels = 0;
}
void fifoInternalRecvInterrupt() // this is the complicated one!
{
	u32 data;
	int block;
	// first pipe all incoming data into our message queue
	fifo_protect_levels=1;
	while( !(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY) )
	{
		REG_IF = IRQ_FIFO_NOT_EMPTY; // clear IF right before we read element, to prevent unnecessary additional IRQs.
		data = REG_IPC_FIFO_RX;
		block = fifoInternalAllocFreeBlock(); // add data to recv queue
		if(block!=-1)
		{
			FIFO_BUFFER_SETCONTROL( block, FIFO_BUFFER_TERMINATE, FIFO_BUFFERCONTROL_RECVWORD, 0 );
			FIFO_BUFFER_DATA( block ) = data;
			if(fifo_rcvdwords_end==-1)
			{
				fifo_rcvdwords_start = fifo_rcvdwords_end = block;
			}
			else
			{
				FIFO_BUFFER_SETNEXT( fifo_rcvdwords_end, block );
				fifo_rcvdwords_end = block;
			}
		}
		else
		{ // oof, dropped word! bad things!
			fifo_lost_words++;
		}
	}
	fifo_protect_levels=0;

	// second (if we're topmost), route received data to it's correct destinations (and enable interrupts, to allow more data to be piped in.)
	// Only going to allow interrupts during user callbacks - The protect function knows when it's being called from within this IRQ and will
	// protect using IME when fifo_recv_status is 1. Because unsetting the interrupts and resetting them is inappropriate for when you're in an interrupt.
	if(fifo_recv_status == 0)
	{
		int channel;
		fifo_recv_status = 1;
		fifo_protect_levels = 1; // pretend we're protected by one level
		REG_IE |= IRQ_FIFO_NOT_EMPTY; // allow nested interrupts of self! (which won't nest further)

		// read fifo element, decide what to do with it
		while(fifo_rcvdwords_start != -1)
		{
			block = fifo_rcvdwords_start;
			data = FIFO_BUFFER_DATA( block );

			if(FIFO_IS_ADDRESS( data )) 
			{ // is an address - unpack it, and either send it to a handler or add it to a queue.
				channel = FIFO_UNPACK_CHANNEL( data );
				void * address = FIFO_UNPACK_ADDRESS( data );
				// unlink block from list
				fifo_rcvdwords_start = FIFO_BUFFER_GETNEXT( block );
				if(fifo_rcvdwords_start == FIFO_BUFFER_TERMINATE) fifo_rcvdwords_start = fifo_rcvdwords_end = -1;
				// check for handler
				if( fifo_address_func[channel] )
				{ // has a handler. Unlink block and call it
					fifoInternalFreeBlock( block );

					fifoInternalUnprotect(); // allow interrupts to occur during callback.
					fifo_address_func[channel]( address, fifo_address_data[channel] );
					fifoInternalProtect();
				}
				else
				{ // no handler, relink block into the end of the channel's queue.
					FIFO_BUFFER_SETCONTROL( block, FIFO_BUFFER_TERMINATE, FIFO_BUFFERCONTROL_ADDRESS, 0);
					FIFO_BUFFER_DATA( block ) = (u32) address;
					if(fifo_channel_end[channel]==-1)
					{ // list is empty
						fifo_channel_start[channel] = fifo_channel_end[channel] = block;
					}
					else
					{
						FIFO_BUFFER_SETNEXT( fifo_channel_end[channel], block );
						fifo_channel_end[channel] = block;
					}
				}
			}
			else if(FIFO_IS_VALUE32( data ))
			{ // is a value - unpack it and either send it to a handler or add it to a queue.
				int blockend; // last block in set
				u32 value32;
				// determine end of block list and if we're incomplete
				blockend = block;
				channel = FIFO_UNPACK_CHANNEL( data );
				if( FIFO_UNPACK_VALUE32_NEEDEXTRA( data ) )
				{
					if( (blockend=FIFO_BUFFER_GETNEXT( block )) == FIFO_BUFFER_TERMINATE )
					{
						break; // kill loop, incomplete dataset.
					}
					value32 = FIFO_BUFFER_DATA( blockend );
				} 
				else 
				{
					value32 = FIFO_UNPACK_VALUE32_NOEXTRA( data );
				}

				// unlink block set from list
				fifo_rcvdwords_start = FIFO_BUFFER_GETNEXT( blockend );
				if(fifo_rcvdwords_start == FIFO_BUFFER_TERMINATE) fifo_rcvdwords_start = fifo_rcvdwords_end = -1;
				// check for handler
				if( fifo_value32_func[channel] )
				{ // has a handler. Unlink block and call it
					fifoInternalFreeBlock( block );
					if(blockend!=block) fifoInternalFreeBlock( blockend );

					fifoInternalUnprotect(); // allow interrupts to occur during callback.
					fifo_value32_func[channel]( value32, fifo_value32_data[channel] );
					fifoInternalProtect();
				}
				else
				{ // no handler, relink block into the end of the channel's queue.
					if(blockend!=block) fifoInternalFreeBlock( blockend );
					FIFO_BUFFER_SETCONTROL( block, FIFO_BUFFER_TERMINATE, FIFO_BUFFERCONTROL_VALUE32, 0);
					FIFO_BUFFER_DATA( block ) = value32;
					if(fifo_channel_end[channel]==-1)
					{ // list is empty
						fifo_channel_start[channel] = fifo_channel_end[channel] = block;
					}
					else
					{
						FIFO_BUFFER_SETNEXT( fifo_channel_end[channel], block );
						fifo_channel_end[channel] = block;
					}
				}
			}
			else if(FIFO_IS_DATA( data ))
			{ // is a value - unpack it and either send it to a handler or add it to a queue.
				int blockend; // last block in set
				int blocktmp, i;
				int n_bytes, n_words;
				// determine end of block list and if we're incomplete
				blockend = block;
				channel = FIFO_UNPACK_CHANNEL( data );
				n_bytes = FIFO_UNPACK_DATALENGTH( data );
				n_words = (n_bytes+3)>>2;
				if(n_bytes>FIFO_MAX_DATA_BYTES)
				{ // invalid! how could this happen!?
					// unlink and pretend it didn't happen
					fifo_rcvdwords_start = FIFO_BUFFER_GETNEXT( block );
					if(fifo_rcvdwords_start == FIFO_BUFFER_TERMINATE) fifo_rcvdwords_start = fifo_rcvdwords_end = -1;
					fifoInternalFreeBlock( block );
					// *la la la la la LA LA LA LA*
					// TODO: should probably record error here.
					continue; // go on to the next one.
				}
				for(i=0;i<n_words;i++) // find last related block
				{
					blockend = FIFO_BUFFER_GETNEXT( blockend );
					if(blockend == FIFO_BUFFER_TERMINATE)
					{
						break; // insufficient words to read message; try again later.
					}
				}
				if(blockend == FIFO_BUFFER_TERMINATE)
				{
					break; // insufficient words to read message; break out of parent while loop
				}

				// unlink block set from list
				fifo_rcvdwords_start = FIFO_BUFFER_GETNEXT( blockend );
				if(fifo_rcvdwords_start == FIFO_BUFFER_TERMINATE) fifo_rcvdwords_start = fifo_rcvdwords_end = -1;

				// reformat and insert
				if(n_words==0)
				{ // no data words - special case
					FIFO_BUFFER_SETCONTROL( block, FIFO_BUFFER_TERMINATE, FIFO_BUFFERCONTROL_DATASTART, 0);
					FIFO_BUFFER_DATA( block ) = 0;
					if(fifo_channel_end[channel]==-1)
					{ // list is empty
						fifo_channel_start[channel] = fifo_channel_end[channel] = block;
					}
					else
					{
						FIFO_BUFFER_SETNEXT( fifo_channel_end[channel], block );
						fifo_channel_end[channel] = block;
					}
				}
				else
				{ // 1 or more data words
					// unlink first entry because it's unnecessary
					blocktmp = block;
					block = FIFO_BUFFER_GETNEXT( block );
					fifoInternalFreeBlock(blocktmp);
					// reformat blocks in list
					FIFO_BUFFER_SETCONTROL( block, FIFO_BUFFER_GETNEXT( block ), FIFO_BUFFERCONTROL_DATASTART, n_bytes);
					blocktmp = FIFO_BUFFER_GETNEXT( block );
					for(i=1;i<n_words;i++)
					{
						FIFO_BUFFER_SETCONTROL( blocktmp, FIFO_BUFFER_GETNEXT( blocktmp ), FIFO_BUFFERCONTROL_DATA, n_words-i);
						blocktmp = FIFO_BUFFER_GETNEXT( blocktmp );
					}
					FIFO_BUFFER_SETNEXT( blockend, FIFO_BUFFER_TERMINATE );
					// link it in to the end of the channel list.
					if(fifo_channel_end[channel]==-1)
					{ // list is empty
						fifo_channel_start[channel] = block;
						fifo_channel_end[channel] = blockend;
					}
					else
					{
						FIFO_BUFFER_SETNEXT( fifo_channel_end[channel], block );
						fifo_channel_end[channel] = blockend;
					}
				}

				// check for handler
				if( fifo_datamsg_func[channel] )
				{ // has a handler. call it.
					fifoInternalUnprotect(); // allow interrupts to occur during callback.
					fifo_datamsg_func[channel]( n_bytes, fifo_datamsg_data[channel] );
					fifoInternalProtect();

					// unlink message if the handler didn't.
					fifoGetDatamsg( channel, 0, 0 );
				}

			}
			else
			{ // no known type, unlink it!

				fifo_rcvdwords_start = FIFO_BUFFER_GETNEXT( block );
				if(fifo_rcvdwords_start == FIFO_BUFFER_TERMINATE) fifo_rcvdwords_start = fifo_rcvdwords_end = -1;
				fifoInternalFreeBlock( block );
				// TODO: should probably record error here.
			}
		}

		// ok stop this crazy ride.
		REG_IME=0;
		fifo_protect_levels=0; // fix vars for normal code.
		fifo_recv_status = 0;
	}
}

// On second thought, always use IME to protect/unprotect. Other interrupts using fifo functions are bad otherwise.
// plus these things are typically only protected for a handful of cycles, it won't affect anything but the most timing sensitive systems.
void fifoInternalProtect()
{
	if(fifo_protect_levels==0)
	{
		REG_IME = 0;
	}
	fifo_protect_levels++;
}
void fifoInternalUnprotect()
{
	if(fifo_protect_levels>0)
	{
		fifo_protect_levels--;
		if(!fifo_protect_levels)
		{
			REG_IME = 1;
		}
	}
}
bool fifoInternalSend(u32 firstword, int extrawordcount, u32 * wordlist)
{
	if(extrawordcount>0 && !wordlist) return false;
	if(extrawordcount<0 || extrawordcount>((FIFO_MAX_DATA_BYTES+3)/4)) return false;
	fifoInternalProtect();
	if(!fifoInternalFreeCheck( extrawordcount+1 ) ) { fifoInternalUnprotect(); return false; }
	fifoInternalSendEnqueue( firstword );
	int i;
	for(i=0;i<extrawordcount;i++)
	{
		fifoInternalSendEnqueue( wordlist[i] );
	}
	fifoInternalBeginSend();
	fifoInternalUnprotect();
	return true;
}

void fifoInternalBeginSend()
{
	fifoInternalProtect();
	if(fifo_send_status==0 && !fifoInternalSendEmpty()) // interrupt hasn't started sending data yet, or has finished and gone to sleep.
	{
		if(! (REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY) )
		{ // huh, there's stuff in the send fifo, but the flag isn't set.
			fifo_send_status = 1;
		}
		else
		{ // irq isn't active cause it ran out of stuff to send, flag it and kickstart it.
			fifo_send_status = 1;
			REG_IPC_FIFO_TX = fifoInternalSendDeque(); // send the first word in our queue.
		}
	}
	fifoInternalUnprotect();
}

u32 fifoInternalRecvPeek()
{
	fifoInternalProtect();
	// check first entry for existence
	if( fifo_rcvdwords_start == -1 ) { fifoInternalUnprotect(); return 0; }
	u32 firstword = FIFO_BUFFER_DATA( fifo_rcvdwords_start );
	fifoInternalUnprotect();
	return firstword;
}

u32 fifoInternalRecvPopFirst()
{
	fifoInternalProtect();
	// check first entry for existence
	if( fifo_rcvdwords_start == -1 ) { fifoInternalUnprotect(); return 0; } // queue is empty
	u32 firstword = FIFO_BUFFER_DATA( fifo_rcvdwords_start );
	if( fifo_rcvdwords_end == fifo_rcvdwords_start )
	{ // only a single entry
#if (FIFO_RIGOROUS_ERROR_CHECKING)
		if( FIFO_BUFFER_GETNEXT( fifo_rcvdwords_start ) != FIFO_BUFFER_TERMINATE ) fifoError("FifoInternalRecvPopFirst: list size is 1, but first entry %i has next of %i", fifo_rcvdwords_start, FIFO_BUFFER_GETNEXT( fifo_rcvdwords_start ) );
		if( FIFO_BUFFER_GETCONTROL( fifo_rcvdwords_start ) != FIFO_BUFFERCONTROL_RECVWORD ) fifoError("FifoInternalRecvPopFirst: Expected control type 2 but got %i (%i)", FIFO_BUFFER_GETCONTROL( fifo_rcvdwords_start ), fifo_rcvdwords_start );
#endif
		fifoInternalFreeBlock( fifo_rcvdwords_start );
		fifo_rcvdwords_start = fifo_rcvdwords_end = -1;
	}
	else
	{ // Multiple entries in the queue.
		int temp_entry = fifo_rcvdwords_start;
#if (FIFO_RIGOROUS_ERROR_CHECKING)
		if( FIFO_BUFFER_GETNEXT( fifo_rcvdwords_start ) == FIFO_BUFFER_TERMINATE ) fifoError("FifoInternalRecvPopFirst: list size >1, but first entry %i has terminating next", fifo_rcvdwords_start );
		if( FIFO_BUFFER_GETCONTROL( fifo_rcvdwords_start ) != FIFO_BUFFERCONTROL_RECVWORD ) fifoError("FifoInternalRecvPopFirst: Expected control type 2 but got %i (%i)", FIFO_BUFFER_GETCONTROL( fifo_rcvdwords_start ), fifo_rcvdwords_start );
#endif
		fifo_rcvdwords_start = FIFO_BUFFER_GETNEXT( temp_entry );
		fifoInternalFreeBlock( temp_entry );
	}
	fifoInternalUnprotect();
	return firstword;
}

int fifoInternalRecvWordCount()
{
	fifoInternalProtect();
	// check first entry for existence
	if( fifo_rcvdwords_start == -1 ) { fifoInternalUnprotect(); return 0; }
	// iterate through received word list.
	int n_words = 1;
	int cur_word = fifo_rcvdwords_start;
	while( FIFO_BUFFER_GETNEXT(cur_word) != FIFO_BUFFER_TERMINATE )
	{
#if (FIFO_RIGOROUS_ERROR_CHECKING)
		if( FIFO_BUFFER_GETCONTROL( cur_word ) != FIFO_BUFFERCONTROL_RECVWORD ) fifoError("FifoInternalRecvWordCount: Expected control type 2 but got %i (%i)", FIFO_BUFFER_GETCONTROL( cur_word ), cur_word );
#endif
		cur_word = FIFO_BUFFER_GETNEXT( cur_word );
		n_words++;
	}
#if (FIFO_RIGOROUS_ERROR_CHECKING)
	if( FIFO_BUFFER_GETCONTROL( cur_word ) != FIFO_BUFFERCONTROL_RECVWORD ) fifoError("FifoInternalRecvWordCount: Expected control type 2 but got %i (%i)", FIFO_BUFFER_GETCONTROL( cur_word ), cur_word );
	if( cur_word != fifo_rcvdwords_end ) fifoError("FifoInternalRecvWordCount: Last list entry %i is not the list end %i", cur_word, fifo_rcvdwords_end );
#endif
	fifoInternalUnprotect();
	return n_words;
}

bool fifoInternalRecvPushBack(u32 word)
{
	fifoInternalProtect();
	int new_block = fifoInternalAllocFreeBlock();
	if(new_block==-1) { fifoInternalUnprotect(); return false; } // bad things.

	if( fifo_rcvdwords_end == -1 )
	{ // buffer is empty
#if (FIFO_RIGOROUS_ERROR_CHECKING)
		if( fifo_rcvdwords_start != -1 ) fifoError("FifoInternalRecvPushBack: list is empty, but list start doesn't agree. %i", fifo_rcvdwords_start );
#endif
		FIFO_BUFFER_SETCONTROL(new_block,FIFO_BUFFER_TERMINATE,FIFO_BUFFERCONTROL_RECVWORD,0);
		FIFO_BUFFER_DATA(new_block)=word;
		fifo_rcvdwords_start = fifo_rcvdwords_end = new_block;
	}
	else
	{
#if (FIFO_RIGOROUS_ERROR_CHECKING)
		if( fifo_rcvdwords_start == -1 ) fifoError("FifoInternalRecvPushBack: list is not empty, but list start doesn't agree." );
#endif
		FIFO_BUFFER_SETCONTROL(new_block,FIFO_BUFFER_TERMINATE,FIFO_BUFFERCONTROL_RECVWORD,0);
		FIFO_BUFFER_DATA(new_block)=word;
		FIFO_BUFFER_SETNEXT( fifo_rcvdwords_end, new_block );
		fifo_rcvdwords_end = new_block;
	}
	fifoInternalUnprotect();
	return true;
}

bool fifoInternalSendEmpty()
{
	return fifo_sendwords_start==-1;
}
u32 fifoInternalSendDeque()
{
	fifoInternalProtect();
	// check first entry for existence
	if( fifo_sendwords_start == -1 ) { fifoInternalUnprotect(); return 0; } // queue is empty
	u32 firstword = FIFO_BUFFER_DATA( fifo_sendwords_start );
	if( fifo_sendwords_end == fifo_sendwords_start )
	{ // only a single entry
#if (FIFO_RIGOROUS_ERROR_CHECKING)
		if( FIFO_BUFFER_GETNEXT( fifo_sendwords_start ) != FIFO_BUFFER_TERMINATE ) fifoError("FifoInternalSendDeque: list size is 1, but first entry %i has next of %i", fifo_sendwords_start, FIFO_BUFFER_GETNEXT( fifo_sendwords_start ) );
		if( FIFO_BUFFER_GETCONTROL( fifo_sendwords_start ) != FIFO_BUFFERCONTROL_SENDWORD ) fifoError("FifoInternalSendDeque: Expected control type 1 but got %i (%i)", FIFO_BUFFER_GETCONTROL( fifo_sendwords_start ), fifo_sendwords_start );
#endif
		fifoInternalFreeBlock( fifo_sendwords_start );
		fifo_sendwords_start = fifo_sendwords_end = -1;
	}
	else
	{ // Multiple entries in the queue.
		int temp_entry = fifo_sendwords_start;
#if (FIFO_RIGOROUS_ERROR_CHECKING)
		if( FIFO_BUFFER_GETNEXT( fifo_sendwords_start ) == FIFO_BUFFER_TERMINATE ) fifoError("FifoInternalSendDeque: list size >1, but first entry %i has terminating next", fifo_sendwords_start );
		if( FIFO_BUFFER_GETCONTROL( fifo_sendwords_start ) != FIFO_BUFFERCONTROL_SENDWORD ) fifoError("FifoInternalSendDeque: Expected control type 1 but got %i (%i)", FIFO_BUFFER_GETCONTROL( fifo_sendwords_start ), fifo_sendwords_start );
#endif
		fifo_sendwords_start = FIFO_BUFFER_GETNEXT( temp_entry );
		fifoInternalFreeBlock( temp_entry );
	}
	fifoInternalUnprotect();
	return firstword;
}
bool fifoInternalSendEnqueue(u32 word)
{
	fifoInternalProtect();
	int new_block = fifoInternalAllocFreeBlock();
	if(new_block==-1) { fifoInternalUnprotect(); return false; } // bad things.

	if( fifo_sendwords_end == -1 )
	{ // buffer is empty
#if (FIFO_RIGOROUS_ERROR_CHECKING)
		if( fifo_sendwords_start != -1 ) fifoError("FifoInternalSendEnqueue: list is empty, but list start doesn't agree. %i", fifo_sendwords_start );
#endif
		FIFO_BUFFER_SETCONTROL(new_block,FIFO_BUFFER_TERMINATE,FIFO_BUFFERCONTROL_SENDWORD,0);
		FIFO_BUFFER_DATA(new_block)=word;
		fifo_sendwords_start = fifo_sendwords_end = new_block;
	}
	else
	{
#if (FIFO_RIGOROUS_ERROR_CHECKING)
		if( fifo_sendwords_start == -1 ) fifoError("FifoInternalSendEnqueue: list is not empty, but list start doesn't agree." );
#endif
		FIFO_BUFFER_SETCONTROL(new_block,FIFO_BUFFER_TERMINATE,FIFO_BUFFERCONTROL_SENDWORD,0);
		FIFO_BUFFER_DATA(new_block)=word;
		FIFO_BUFFER_SETNEXT( fifo_sendwords_end, new_block );
		fifo_sendwords_end = new_block;
	}
	fifoInternalUnprotect();
	return true;
}


int fifoInternalAllocFreeBlock()
{
	if(fifo_freelist_start==-1) return -1;
	int blockid = fifo_freelist_start;
	fifo_freelist_start = FIFO_BUFFER_GETNEXT(blockid);
	if(fifo_freelist_start==FIFO_BUFFER_TERMINATE) fifo_freelist_start=-1;
#if (FIFO_RIGOROUS_ERROR_CHECKING)
	if( FIFO_BUFFER_GETCONTROL( blockid ) != FIFO_BUFFERCONTROL_UNUSED ) fifoError("FifoInternalAllocFreeBlock: non-unused block in free list! %i (%i)", FIFO_BUFFER_GETCONTROL( blockid ), blockid );
#endif
	return blockid;
}

void fifoInternalFreeBlock(int blockid)
{
	if(blockid<0 || blockid>=FIFO_BUFFER_ENTRIES) return;
	FIFO_BUFFER_SETCONTROL( blockid, fifo_freelist_start, FIFO_BUFFERCONTROL_UNUSED, 0 );
	fifo_freelist_start=blockid;
}


bool fifoInternalFreeCheck(int num_words)
{
	fifoInternalProtect();
	int block = fifo_freelist_start;
	if(block==-1) { fifoInternalUnprotect(); return false; }
	num_words--;
	while(num_words>0 && FIFO_BUFFER_GETNEXT(block)!=FIFO_BUFFER_TERMINATE)
	{
		num_words--;
		block = FIFO_BUFFER_GETNEXT(block);
	}
	fifoInternalUnprotect();
	return (num_words<=0);
}



