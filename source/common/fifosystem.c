#include <nds/fifocommon.h>
#include <nds/ipc.h>
#include <nds/interrupts.h>
#include <nds/bios.h>

#include <string.h>

// Some aspects of this configuration can be changed...

// FIFO_CHANNEL_BITS - number of bits used to specify the channel in a packet - default=4
#define FIFO_CHANNEL_BITS				4

// FIFO_MAX_DATA_WORDS - maximum number of bytes that can be sent in a fifo message
#define FIFO_MAX_DATA_BYTES				128

// FIFO_RIGOROUS_ERROR_CHECKING - Verify all internal buffer transactions, mostly for debugging this library. Unless there's memory corruption this shouldn't be enabled normally.
// If there is an error, the lib will call int fifoError(char *, ...) - which isn't defined by the fifo lib. So it's best to handle it if you want to debug. :)
// All of the errors trapped represent serious problems, so it's not a bad idea to halt in fifoError()
// #define FIFO_RIGOROUS_ERROR_CHECKING	1

// FIFO_BUFFER_ENTRIES - number of words that can be stored temporarily while waiting to deque them - default=256 arm7, 512 arm9
#ifdef ARM9
#define FIFO_BUFFER_ENTRIES				256
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

// And some aspects of the configuration can't be changed
//  please don't edit below this line.

#define FIFO_ADDRESSDATA_SHIFT			0
#define FIFO_MINADDRESSDATABITS			24
#define FIFO_ADDRESSDATA_MASK			0x00FFFFFF
#define FIFO_ADDRESSBASE				0x02000000
#define FIFO_ADDRESSCOMPATIBLE			0xFF000000

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


#define FIFO_BUFFER_TERMINATE	0xFFFF
#define FIFO_BUFFER_NEXTMASK	0xFFFF

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

#define FIFO_PACK_DATAMSG_HEADER(channel, numbytes) \
	( ((channel)<<FIFO_CHANNEL_SHIFT) | ((numbytes)&FIFO_VALUE32_MASK) )


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

#define FIFO_BUFFER_DATA_BYTE(index, byteindex) ((u8*)(&fifo_buffer[(index)*2+1]))[(byteindex)]

#define FIFO_BUFFER_GETNEXT(index) (fifo_buffer[(index)*2]&FIFO_BUFFER_NEXTMASK)

#define FIFO_BUFFER_GETCONTROL(index) (fifo_buffer[(index)*2]>>28)

#define FIFO_BUFFER_GETEXTRA(index) ((fifo_buffer[(index)*2]>>16)&0xFFF)

#define FIFO_BUFFER_SETCONTROL(index,next,control,extra) \
	fifo_buffer[(index)*2] = ( ((next)&FIFO_BUFFER_NEXTMASK) | ((control)<<28) | (((extra)&0xFFF)<<16) )

#define FIFO_BUFFER_SETNEXT(index, next) \
	fifo_buffer[(index)*2] = ( ((next)&FIFO_BUFFER_NEXTMASK) | (fifo_buffer[(index)*2]& (~FIFO_BUFFER_NEXTMASK)) )


// FIFO_BUFFER_ENTRIES*8 of global buffer space
vu32	fifo_buffer[FIFO_BUFFER_ENTRIES*2];

typedef struct fifo_queue {
	vu16 head;
	vu16 tail;
} fifo_queue;
	
fifo_queue	fifo_address_queue[FIFO_NUM_CHANNELS];
fifo_queue	fifo_data_queue[FIFO_NUM_CHANNELS];
fifo_queue	fifo_value32_queue[FIFO_NUM_CHANNELS];

fifo_queue fifo_buffer_free = {0,FIFO_BUFFER_ENTRIES-1};
fifo_queue fifo_send_queue = { FIFO_BUFFER_TERMINATE, FIFO_BUFFER_TERMINATE};
fifo_queue fifo_receive_queue = { FIFO_BUFFER_TERMINATE, FIFO_BUFFER_TERMINATE};

vu32	fifo_freewords = FIFO_BUFFER_ENTRIES;

FifoAddressHandlerFunc		fifo_address_func[FIFO_NUM_CHANNELS];
void *						fifo_address_data[FIFO_NUM_CHANNELS];
FifoValue32HandlerFunc		fifo_value32_func[FIFO_NUM_CHANNELS];
void *						fifo_value32_data[FIFO_NUM_CHANNELS];
FifoDatamsgHandlerFunc		fifo_datamsg_func[FIFO_NUM_CHANNELS];
void *						fifo_datamsg_data[FIFO_NUM_CHANNELS];


// fifoSetAddressHandler - Set a callback to receive incoming address messages on a specific channel.
bool fifoSetAddressHandler(int channel, FifoAddressHandlerFunc newhandler, void * userdata) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;

	int oldIME = enterCriticalSection();

	fifo_address_func[channel] = newhandler;
	fifo_address_data[channel] = userdata;
	
	if(newhandler) {
		while(fifoCheckAddress(channel)) {
			newhandler( fifoGetAddress(channel), userdata );
		}
	}

	leaveCriticalSection(oldIME);

	return true;
}

// fifoSetValue32Handler - Set a callback to receive incoming value32 messages on a specific channel.
bool fifoSetValue32Handler(int channel, FifoValue32HandlerFunc newhandler, void * userdata) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;

	int oldIME = enterCriticalSection();

	fifo_value32_func[channel] = newhandler;
	fifo_value32_data[channel] = userdata;

	if(newhandler) {
		while(fifoCheckValue32(channel)) {
			newhandler( fifoGetValue32(channel) , userdata );
		}
	}


	leaveCriticalSection(oldIME);

	return true;
}

// fifoSetDatamsgHandler - Set a callback to receive incoming data sequences on a specific channel.
bool fifoSetDatamsgHandler(int channel, FifoDatamsgHandlerFunc newhandler, void * userdata) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;

	int oldIME = enterCriticalSection();

	fifo_datamsg_func[channel] = newhandler;
	fifo_datamsg_data[channel] = userdata;


	if(newhandler) {
		while(fifoCheckDatamsg(channel)) {
			int block = fifo_data_queue[channel].head;
			int n_bytes = FIFO_UNPACK_DATALENGTH(FIFO_BUFFER_DATA(block));
			newhandler(n_bytes, userdata);
			if (block == fifo_data_queue[channel].head) fifoGetDatamsg(channel,0,0);
		}
	}

	leaveCriticalSection(oldIME);

	return true;
}

static u32 fifo_allocBlock() {
	if (fifo_freewords == 0) return FIFO_BUFFER_TERMINATE;
	u32 entry = fifo_buffer_free.head;
	fifo_buffer_free.head = FIFO_BUFFER_GETNEXT(fifo_buffer_free.head);
	FIFO_BUFFER_SETCONTROL( entry, FIFO_BUFFER_TERMINATE, FIFO_BUFFERCONTROL_UNUSED, 0 );
	fifo_freewords--;
	return entry;
}

static u32 fifo_waitBlock() {
	u32 block;
	do {
		block = fifo_allocBlock();
		if (block == FIFO_BUFFER_TERMINATE) {
			REG_IPC_FIFO_CR |= IPC_FIFO_SEND_IRQ;
			REG_IME = 1;
			swiIntrWait(0,IRQ_FIFO_EMPTY);
			REG_IME = 0;
		}
	} while(block == FIFO_BUFFER_TERMINATE);
	return block;
}

static void fifo_freeBlock(u32 index) {
	FIFO_BUFFER_SETCONTROL( index, FIFO_BUFFER_TERMINATE, FIFO_BUFFERCONTROL_UNUSED, 0 );
	FIFO_BUFFER_SETCONTROL( fifo_buffer_free.tail, index, FIFO_BUFFERCONTROL_UNUSED, 0 );
	fifo_buffer_free.tail = index;
	fifo_freewords++;
}

static bool fifoInternalSend(u32 firstword, int extrawordcount, u32 * wordlist) {
	if(extrawordcount>0 && !wordlist) return false;
	if(fifo_freewords<extrawordcount+1) return false;
	if(extrawordcount<0 || extrawordcount>(FIFO_MAX_DATA_BYTES/4)) return false;

	int count = 0;
	int oldIME = enterCriticalSection();

	u32 head = fifo_waitBlock();
	if(fifo_send_queue.head == FIFO_BUFFER_TERMINATE) {
		fifo_send_queue.head = head;
	} else {
		FIFO_BUFFER_SETNEXT(fifo_send_queue.tail,head);		
	}
	FIFO_BUFFER_DATA(head)=firstword;
	fifo_send_queue.tail = head;

	while (count<extrawordcount) {
		u32 next = fifo_waitBlock();
		if(fifo_send_queue.head == FIFO_BUFFER_TERMINATE) {
			fifo_send_queue.head = next;
		} else {
			FIFO_BUFFER_SETNEXT(fifo_send_queue.tail,next);		
		}
		FIFO_BUFFER_DATA(next)=wordlist[count];
		count++;
		fifo_send_queue.tail = next;
	}

	REG_IPC_FIFO_CR |= IPC_FIFO_SEND_IRQ;

	leaveCriticalSection(oldIME);
	
	return true;
}

// fifoSendAddress - Send an address (from mainram only) to the other cpu (on a specific channel)
//  Address can be in the range of 0x02000000-0x02FFFFFF
bool fifoSendAddress(int channel, void *address) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	if( !FIFO_IS_ADDRESS_COMPATIBLE(address) ) return false;
	return fifoInternalSend(FIFO_PACK_ADDRESS(channel, address), 0, 0);
}

bool fifoSendValue32(int channel, u32 value32) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	u32 send_first, send_extra[1];

	if( FIFO_VALUE32_NEEDEXTRA(value32) ) {
		// construct message
		send_first = FIFO_PACK_VALUE32_EXTRA(channel);
		send_extra[0] = value32;
		// send message
		return fifoInternalSend(send_first, 1, send_extra);
	} else {
		// construct message
		send_first = FIFO_PACK_VALUE32(channel, value32);
		// send message
		return fifoInternalSend(send_first, 0, 0);
	}
}

bool fifoSendDatamsg(int channel, int num_bytes, u8 * data_array) {
  if(num_bytes == 0) {
    u32 send_first = FIFO_PACK_DATAMSG_HEADER(channel, 0);
    return fifoInternalSend(send_first, 0, NULL);
  }

	if(data_array==NULL) return false;
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	if(num_bytes<0 || num_bytes>=FIFO_MAX_DATA_BYTES) return false;

	
	int num_words = (num_bytes+3)>>2;

	u32 buffer_array[num_words];

	if (fifo_freewords<num_words+1) return false;
	buffer_array[num_words-1]=0; // zero out last few bytes
	memcpy(buffer_array,data_array,num_bytes);
	u32 send_first = FIFO_PACK_DATAMSG_HEADER(channel, num_bytes);
	return fifoInternalSend(send_first, num_words, buffer_array);
}


void * fifoGetAddress(int channel) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return NULL;
	int block = fifo_address_queue[channel].head;
	if (block == FIFO_BUFFER_TERMINATE) return NULL;
	int oldIME = enterCriticalSection();
	void *address = (void*)FIFO_BUFFER_DATA(block);
	fifo_address_queue[channel].head = FIFO_BUFFER_GETNEXT(block);
	fifo_freeBlock(block);
	leaveCriticalSection(oldIME);
	return address;
}
	
u32 fifoGetValue32(int channel) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return 0;
	int block = fifo_value32_queue[channel].head;
	if ( block == FIFO_BUFFER_TERMINATE) return 0;

	int oldIME = enterCriticalSection();
	u32 value32 = FIFO_BUFFER_DATA(block);
	fifo_value32_queue[channel].head = FIFO_BUFFER_GETNEXT(block);
	fifo_freeBlock(block);
	leaveCriticalSection(oldIME);
	return value32;
}

int fifoGetDatamsg(int channel, int buffersize, u8 * destbuffer) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return -1;
	int block = fifo_data_queue[channel].head;
	if (block == FIFO_BUFFER_TERMINATE) return -1;
	int oldIME = enterCriticalSection();

	int num_bytes = FIFO_BUFFER_GETEXTRA(block);
	int	num_words = (num_bytes+3)>>2;
	u32 buffer_array[num_words];

	int i,next;
	for(i=0; i<num_words;i++) {
		buffer_array[i] = FIFO_BUFFER_DATA(block);
		next=FIFO_BUFFER_GETNEXT(block);
		fifo_freeBlock(block);
		block=next;
		if(block==FIFO_BUFFER_TERMINATE) break;
	}
	fifo_data_queue[channel].head = block;

	if(buffersize<num_bytes) num_bytes=buffersize;
	memcpy(destbuffer,buffer_array,num_bytes);

	leaveCriticalSection(oldIME);
	return num_bytes;
	
}

bool fifoCheckAddress(int channel) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	return fifo_address_queue[channel].head != FIFO_BUFFER_TERMINATE;
}

bool fifoCheckDatamsg(int channel) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	return fifo_data_queue[channel].head != FIFO_BUFFER_TERMINATE;
}

bool fifoCheckValue32(int channel) {
	if(channel<0 || channel>=FIFO_NUM_CHANNELS) return false;
	return fifo_value32_queue[channel].head != FIFO_BUFFER_TERMINATE;
}

static void fifo_queueBlock(fifo_queue *queue, int head, int tail) {
	FIFO_BUFFER_SETNEXT(tail,FIFO_BUFFER_TERMINATE);
	if(queue->head == FIFO_BUFFER_TERMINATE ) {
		queue->head = head;
		queue->tail = tail;
	} else {
		FIFO_BUFFER_SETNEXT(queue->tail,head);
		queue->tail = tail;
	}

}

int processing=0;

static void fifoInternalRecvInterrupt() {
	REG_IE &= ~IRQ_FIFO_NOT_EMPTY;
	REG_IME=1;

	u32 data, block=FIFO_BUFFER_TERMINATE;

	while( !(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY) ) {
		
		REG_IME=0;
		block=fifo_allocBlock();
		if (block != FIFO_BUFFER_TERMINATE ) {
			FIFO_BUFFER_DATA(block)=REG_IPC_FIFO_RX;
			fifo_queueBlock(&fifo_receive_queue,block,block);
		}

		REG_IME=1;

	}

	REG_IME=0;
	REG_IE |= IRQ_FIFO_NOT_EMPTY;


	if (!processing && fifo_receive_queue.head != FIFO_BUFFER_TERMINATE) {

		processing = 1;
		REG_IME=1;
		
		do {
		
			block = fifo_receive_queue.head;
			data = FIFO_BUFFER_DATA(block);

			u32 channel = FIFO_UNPACK_CHANNEL(data);

			if ( (data & (FIFO_ADDRESSBIT | FIFO_IMMEDIATEBIT)) == (FIFO_ADDRESSBIT | FIFO_IMMEDIATEBIT) ) {

				if ((data & FIFO_ADDRESSDATA_MASK) == 0x4000c ) {
					REG_IPC_SYNC = 0x100;
					while((REG_IPC_SYNC&0x0f) != 1);
					REG_IPC_SYNC = 0;
					swiSoftReset();
				}
				
			} else if (FIFO_IS_ADDRESS(data)) {

				volatile void * address = FIFO_UNPACK_ADDRESS(data);

				REG_IME=0;
				fifo_receive_queue.head = FIFO_BUFFER_GETNEXT(block);
				if (fifo_address_func[channel]) {
					fifo_freeBlock(block);
					REG_IME=1;
					fifo_address_func[channel]( (void*)address, fifo_address_data[channel] );
				} else {
					FIFO_BUFFER_DATA(block)=(u32)address;
					fifo_queueBlock(&fifo_address_queue[channel],block,block);
				}

			} else if(FIFO_IS_VALUE32(data)) {

				u32 value32;

				if (FIFO_UNPACK_VALUE32_NEEDEXTRA(data)) {
					int next = FIFO_BUFFER_GETNEXT(block);
					if (next==FIFO_BUFFER_TERMINATE) break;
					block = next;
					value32 = FIFO_BUFFER_DATA(block);
					
				} else {
					value32 = FIFO_UNPACK_VALUE32_NOEXTRA(data);
				}
				REG_IME=0;
				fifo_receive_queue.head = FIFO_BUFFER_GETNEXT(block);
				if (fifo_value32_func[channel]) {
					fifo_freeBlock(block);
					REG_IME=1;
					fifo_value32_func[channel]( value32, fifo_value32_data[channel] );
				} else {
					FIFO_BUFFER_DATA(block)=value32;
					fifo_queueBlock(&fifo_value32_queue[channel],block,block);
				}

			} else if(FIFO_IS_DATA(data)) {

				int n_bytes = FIFO_UNPACK_DATALENGTH(data);
				int n_words = (n_bytes+3)>>2;
				int count=0;
				
				int end=block;

				while(count<n_words && FIFO_BUFFER_GETNEXT(end)!=FIFO_BUFFER_TERMINATE){
					end = FIFO_BUFFER_GETNEXT(end);
					count++;
				}


				if (count!=n_words) break;
				REG_IME=0;
				
				fifo_receive_queue.head = FIFO_BUFFER_GETNEXT(end);

				int tmp=FIFO_BUFFER_GETNEXT(block);
				fifo_freeBlock(block);
				

				FIFO_BUFFER_SETCONTROL(tmp, FIFO_BUFFER_GETNEXT(tmp), FIFO_BUFFERCONTROL_DATASTART, n_bytes);

				fifo_queueBlock(&fifo_data_queue[channel],tmp,end);
				if(fifo_datamsg_func[channel]) {
					block = fifo_data_queue[channel].head;
					REG_IME=1;
					fifo_datamsg_func[channel](n_bytes, fifo_datamsg_data[channel]);
					if (block == fifo_data_queue[channel].head) fifoGetDatamsg(channel,0,0);
				}
				

			} else {

				fifo_receive_queue.head = FIFO_BUFFER_GETNEXT(block);
				fifo_freeBlock(block);

			}

		} while( fifo_receive_queue.head != FIFO_BUFFER_TERMINATE);
		
		REG_IME = 0;
		processing = 0;
	}
}

static void fifoInternalSendInterrupt() {

	if ( fifo_send_queue.head == FIFO_BUFFER_TERMINATE ) {
		REG_IPC_FIFO_CR &= ~IPC_FIFO_SEND_IRQ;	// disable send irq
	} else {

		u32 head,next;

		head = fifo_send_queue.head;

		while( !(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL) ) {

			next = FIFO_BUFFER_GETNEXT(head);
			REG_IPC_FIFO_TX = FIFO_BUFFER_DATA(head);
			fifo_freeBlock(head);
			head = next;
			if (head == FIFO_BUFFER_TERMINATE) break;
		}

		fifo_send_queue.head = head;
		
	}
}

static volatile int __timeout = 0;

static void __timeoutvbl() {
	__timeout++;
}
bool fifoInit() {


	int i;

	REG_IPC_FIFO_CR = IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_EMPTY | IPC_FIFO_SEND_EMPTY;

	for(i=0;i<FIFO_NUM_CHANNELS;i++) {

		fifo_address_queue[i].head = FIFO_BUFFER_TERMINATE;
		fifo_address_queue[i].tail = FIFO_BUFFER_TERMINATE;

		fifo_data_queue[i].head = FIFO_BUFFER_TERMINATE;
		fifo_data_queue[i].tail = FIFO_BUFFER_TERMINATE;

		fifo_value32_queue[i].head = FIFO_BUFFER_TERMINATE;
		fifo_value32_queue[i].tail = FIFO_BUFFER_TERMINATE;

		fifo_address_data[i] = fifo_value32_data[i] = fifo_datamsg_data[i] = 0;
		fifo_address_func[i] = 0;
		fifo_value32_func[i] = 0;
		fifo_datamsg_func[i] = 0;
	}

	for(i=0;i<FIFO_BUFFER_ENTRIES-1;i++) {
		FIFO_BUFFER_DATA(i) = 0;
		FIFO_BUFFER_SETCONTROL(i, i+1, 0, 0);
	}
	FIFO_BUFFER_SETCONTROL(FIFO_BUFFER_ENTRIES-1, FIFO_BUFFER_TERMINATE, FIFO_BUFFERCONTROL_UNUSED, 0);

	#define __SYNC_START	0
	#define __SYNC_END		14
	#define __TIMEOUT		120

	int their_value=0,my_value=__SYNC_START,count=0;

	irqSet(IRQ_VBLANK,__timeoutvbl);
	irqEnable(IRQ_IPC_SYNC|IRQ_VBLANK);

	do {
		their_value = IPC_GetSync();
		if(their_value == __SYNC_END && count > 2) break;
		if(count>3) break;
		if( ((my_value + 1)&7 ) == their_value ) count++;
		my_value=(their_value+1)&7;
		IPC_SendSync(my_value);
		swiIntrWait(1,IRQ_IPC_SYNC|IRQ_VBLANK);
	} while (__timeout < __TIMEOUT);

	irqDisable(IRQ_IPC_SYNC);
	irqSet(IRQ_VBLANK,0);
	
	if (__timeout>= __TIMEOUT) {
		IPC_SendSync(0);
		return false;
	}

	IPC_SendSync(__SYNC_END);
	irqSet(IRQ_FIFO_EMPTY,fifoInternalSendInterrupt);
	irqSet(IRQ_FIFO_NOT_EMPTY,fifoInternalRecvInterrupt);
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;
	irqEnable(IRQ_FIFO_NOT_EMPTY|IRQ_FIFO_EMPTY);

	return true;
}
