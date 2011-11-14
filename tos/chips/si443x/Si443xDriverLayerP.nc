/*
* Copyright (c) 2007, Vanderbilt University
* Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Miklos Maroti
* Author: Krisztian Veress
*/

#include <Si443xDriverLayer.h>
#include <Tasklet.h>
#include <RadioAssert.h>
#include <TimeSyncMessageLayer.h>
#include <RadioConfig.h>

#warning "Using: Si443xDriverLayerP"

module Si443xDriverLayerP
{
	provides
	{
		interface Init as PlatformInit @exactlyonce();
		interface Init as SoftwareInit @exactlyonce();

		interface RadioState;
		interface RadioSend;
		interface RadioReceive;
		interface RadioCCA;
		interface RadioPacket;

		interface PacketField<uint8_t> as PacketTransmitPower;
		interface PacketField<uint8_t> as PacketRSSI;
		interface PacketField<uint8_t> as PacketTimeSyncOffset;
		interface PacketField<uint8_t> as PacketLinkQuality;
		interface LinkPacketMetadata;
#ifdef MOPT_ENABLE		
		interface ModemOptimizer;
#endif

	}

	uses
	{

		interface GeneralIO as SDN;
		interface GeneralIO as NSEL;

#ifdef SI443X_GPIOCAPTURE
		interface GpioCapture as IRQ;
#else
		interface GpioInterrupt as IRQ;
		#define captureFallingEdge enableFallingEdge
#endif
		interface FastSpiByte;
		interface Resource as SpiResource;

		interface BusyWait<TMicro, uint16_t>;
		interface LocalTime<TRadio>;

		interface Si443xDriverConfig as Config;

		interface PacketFlag as TransmitPowerFlag;
		interface PacketFlag as RSSIFlag;
		interface PacketFlag as TimeSyncFlag;

		interface PacketTimeStamp<TRadio, uint32_t>;

		interface Tasklet;
		interface RadioAlarm;
		
#ifdef RADIO_DEBUG
		interface Boot;
		interface DiagMsg;
		interface Leds;
#endif
	}
}

implementation
{

/* ----------------- DEBUGGER FUNCTIONS AND HELPERS  -----------------*/
#ifdef RADIO_DEBUG

tasklet_norace uint8_t DM_ENABLE = FALSE;
	
#define DIAGMSG_STR(PSTR,STR)	\
		atomic { if( DM_ENABLE && call DiagMsg.record() ) { \
			call DiagMsg.str(PSTR);\
			call DiagMsg.str(STR); \
			call DiagMsg.send(); \
		}}

#define DIAGMSG_REG_READ(REG,VALUE)	\
		atomic { if( DM_ENABLE && call DiagMsg.record() ) { \
			call DiagMsg.str("R");\
			call DiagMsg.hex8(REG);\
			call DiagMsg.hex8(VALUE);\
			call DiagMsg.send(); \
		}}

#define DIAGMSG_REG_WRITE(REG,VALUE)	\
		atomic {if( DM_ENABLE && call DiagMsg.record() ) { \
			call DiagMsg.str("W");\
			call DiagMsg.hex8(REG);\
			call DiagMsg.hex8(VALUE);\
			call DiagMsg.send(); \
		}}

#define DIAGMSG_CHIP()	\
		atomic { if( DM_ENABLE && call DiagMsg.record() ) { \
			call DiagMsg.str("C");\
			call DiagMsg.hex8(chip.state);\
			call DiagMsg.hex8(chip.cmd);\
			call DiagMsg.send(); \
		}}

#define DIAGMSG_VAR(PSTR,VAR) \
		atomic { if( DM_ENABLE && call DiagMsg.record() ) { \
			call DiagMsg.str(PSTR);\
			call DiagMsg.hex8(VAR); \
			call DiagMsg.send(); \
		}}

#else
#define DIAGMSG_STR(A,B)
#define DIAGMSG_REG_READ(A,B)
#define DIAGMSG_REG_WRITE(A,B)
#define DIAGMSG_CHIP()
#define DIAGMSG_VAR(A,B)
#endif

/*----------------- STATE -----------------*/

	enum
	{
		STATE_POR = 0,
		STATE_SLEEP = 1,
		STATE_READY = 2,
		STATE_TUNE = 3,
		STATE_RX = 4,
		STATE_TX = 5,

		CMD_NONE = 0,			// the state machine has stopped
		CMD_TURNOFF = 1,		// goto SLEEP state
		CMD_STANDBY = 2,		// goto READY state		
		CMD_TURNON = 3,		// goto RX state

		CMD_CHANNEL = 8,		// change channel

		CMD_RX_WAIT = 10,		// wait for data in the RX fifo
		CMD_RX_FINISH = 12,
		CMD_RX_ABORT = 13,

		CMD_TX_FINISH = 20,	// finish transmitting

		CMD_FINISH_CCA = 30,	// finish clear chanel assesment
		CMD_RESET = 31,
	};

	enum {
		POR_TIME = (uint16_t)30000,
		CCA_REQUEST_TIME = (uint16_t)(140 * RADIO_ALARM_MICROSEC),
		TX_SFD_DELAY = (uint16_t)(176 * RADIO_ALARM_MICROSEC),
		RX_SFD_DELAY = (uint16_t)(8 * RADIO_ALARM_MICROSEC),
	};

	tasklet_norace struct {
		uint8_t state;
		uint8_t cmd;
	} chip;

	norace bool radioIrq;

	tasklet_norace uint8_t txPower;
	tasklet_norace uint8_t channel;

	message_t rxMsgBuffer;
	tasklet_norace message_t*	rxMsg;

	tasklet_norace message_t*	txMsg;
	tasklet_norace uint8_t 		plbyte;			// the payload byte index in the RX/TX message we need to write/read
	tasklet_norace uint8_t 		txEmptyThresh;	
	tasklet_norace uint16_t		capturedTime;

	tasklet_norace uint8_t rssiClear;
	tasklet_norace uint8_t rssiBusy;

/*----------------- MESSAGE HANDLING -----------------*/

	si443x_header_t* getHeader(message_t* msg)
	{
		return ((void*)msg) + call Config.headerLength(msg);
	}

	void* getPayload(message_t* msg)
	{
		return ((void*)msg) + call RadioPacket.headerLength(msg);
	}

	si443x_metadata_t* getMeta(message_t* msg)
	{
		return ((void*)msg) + sizeof(message_t) - call RadioPacket.metadataLength(msg);
	}

/*----------------- REGISTER -----------------*/

	inline void writeRegister(uint8_t reg, uint8_t value)
	{
		RADIO_ASSERT( call SpiResource.isOwner() );
		RADIO_ASSERT( reg == (reg & SI443X_SPI_REGMASK) );

		call NSEL.clr();
		call FastSpiByte.splitWrite(SI443X_SPI_WRITE | reg);
		call FastSpiByte.splitReadWrite(value);
		call FastSpiByte.splitRead();
		call NSEL.set();
	}


	inline uint8_t readRegister(uint8_t reg)
	{
		RADIO_ASSERT( call SpiResource.isOwner() );
		RADIO_ASSERT( reg == (reg & SI443X_SPI_REGMASK) );

		call NSEL.clr();
		call FastSpiByte.splitWrite(SI443X_SPI_READ | reg);
		call FastSpiByte.splitReadWrite(0);
		reg = call FastSpiByte.splitRead();
		call NSEL.set();
		return reg;
	}


/*----------------- LOW LEVEL FUNCTIONS -----------------*/

	inline void _clearFifo(uint8_t fifobits) {
		uint8_t old = readRegister(SI443X_CTRL_2);
		writeRegister(SI443X_CTRL_2, old |  fifobits );
		writeRegister(SI443X_CTRL_2, old & (~fifobits) );
	}

	inline void _setPower(uint8_t power) {
		writeRegister(SI443X_TXPOWER, SI443X_LNA | (power & SI443X_RFPOWER_MASK));
	}

	inline void _setPktLength(uint8_t length) {
		writeRegister(SI443X_PKTLEN, length);
	}

	inline void _reset() {
		DIAGMSG_STR("reset","");
		readRegister(SI443X_INT_1);
		readRegister(SI443X_INT_2);

		// previous interrupts mess up the PCINT handler
		call IRQ.captureFallingEdge();
		writeRegister(SI443X_CTRL_1, SI443X_CTRL1_SWRESET | SI443X_CTRL1_READY );
		call BusyWait.wait(POR_TIME);

		// we might get interrupts here, MUST ignore them
		call IRQ.disable();
		readRegister(SI443X_INT_1);
		readRegister(SI443X_INT_2);
		call IRQ.captureFallingEdge();
	}

	inline void _standby() {
		DIAGMSG_STR("standby","");

		// tricky reset of interrupts
		// we might get interrupts here, MUST ignore them

		call IRQ.disable();
		writeRegister(SI443X_IEN_1,SI443X_I_NONE);
		writeRegister(SI443X_IEN_2,SI443X_I_NONE);
		writeRegister(SI443X_IEN_1,SI443X_I_ALL);
		writeRegister(SI443X_IEN_2,SI443X_I_ALL);
		writeRegister(SI443X_IEN_1,SI443X_I_NONE);
		writeRegister(SI443X_IEN_2,SI443X_I_NONE);
		call IRQ.captureFallingEdge();

		// we instantly enter standby, NO interrupt will come
		writeRegister(SI443X_CTRL_1,SI443X_CTRL1_STANDBY);
		
		chip.state = STATE_SLEEP;
	}

	inline void _ready() {
		DIAGMSG_STR("ready","");

		writeRegister(SI443X_IEN_1, SI443X_I_ALL);
		writeRegister(SI443X_IEN_2, SI443X_I_ALL);
		// we instantly enter ready, NO interrupt will come
		writeRegister(SI443X_CTRL_1, SI443X_CTRL1_READY);
		
		chip.state = STATE_READY;		
	}

	inline void _tune() {
		writeRegister(SI443X_CTRL_1, SI443X_CTRL1_TUNE);
		chip.state = STATE_TUNE;
	}

	inline void _transmit()
	{
		//DIAGMSG_STR("transmit","");

		writeRegister(SI443X_IEN_1, SI443X_I1_FIFOERROR | SI443X_I1_TXFIFOEMPTY | SI443X_I1_PKTSENT);
		writeRegister(SI443X_IEN_2, SI443X_I_NONE);
		readRegister(SI443X_INT_1);
		readRegister(SI443X_INT_2);
		writeRegister(SI443X_CTRL_1, SI443X_CTRL1_TRANSMIT | SI443X_CTRL1_TUNE);
		
		chip.state = STATE_TX;		
	}

	inline void _receive()
	{
		//DIAGMSG_STR("receive","");
		RADIO_ASSERT( chip.state != STATE_RX );

		writeRegister(SI443X_IEN_1, SI443X_I1_FIFOERROR | SI443X_I1_RXFIFOFULL | SI443X_I1_PKTRECEIVED | SI443X_I1_CRCERROR); 
		writeRegister(SI443X_IEN_2, SI443X_I2_SYNCDETECT );
		readRegister(SI443X_INT_1);
		readRegister(SI443X_INT_2);
		writeRegister(SI443X_CTRL_1, SI443X_CTRL1_RECEIVE | SI443X_CTRL1_READY );
		
		chip.state = STATE_RX;
	}

	void _frequencyChange(uint8_t f10MHz, uint16_t fKHz) {
		uint16_t freq;
		
		RADIO_ASSERT( f10MHz >= 24 && f10MHz <= 95 );
		RADIO_ASSERT( fKHz >= 0 && fKHz <= 9999 );
	
		// fb (refer to AN440.pdf (Si443x Register Descriptions) page 55.)
		freq = ( f10MHz >= 48 ) ? ( f10MHz - 48 + SI443X_FREQ_HBSEL) : f10MHz - 24;	
		writeRegister(0x75, SI443X_FREQ_BAND_MISC | (uint8_t)freq );
			
		// fc
		freq = ( f10MHz >= 48 ) ? (uint16_t)(( (uint32_t)fKHz * 5) >> 4) : (uint16_t)(( (uint32_t)fKHz *5) >> 5);
		writeRegister(0x77,(uint8_t)freq);
		writeRegister(0x76,(uint8_t)(freq >> 8));
			
		// frequency offset
		writeRegister(0x73,0x00);
		writeRegister(0x74,0x00);
		
	
	}

	inline void _channel() {
		writeRegister(SI443X_CHANNEL_SELECT,channel);
	}

#ifdef MOPT_ENABLE		
	typedef struct si443x_modem_optimizer_t {
		//	uint8_t 	modulation;
		//	bool 		manchester;
		//	bool		afc;
		uint8_t	datarate;	
		uint8_t 	carrier_10mhz;
		uint16_t	carrier_khz;
		//	uint16_t	freqdeviation;
		uint16_t	ookrxbw;
	} si443x_modem_optimizer_t;
	
	si443x_modem_optimizer_t mopt;
	
	command void ModemOptimizer.reset() {
		rssiClear = 50;
		rssiBusy = 250;
		txPower = SI443X_DEF_RFPOWER & SI443X_RFPOWER_MASK;
	}
	
	command void ModemOptimizer.configure(uint32_t id) {

		signal ModemOptimizer.configured();
	}
	
	command uint32_t ModemOptimizer.configCount() {
		return 10;
	}
#endif

	void _setupModem()
	{
		DIAGMSG_STR("setup","");

		writeRegister(SI443X_TXFIFO_EMPTY, SI443X_TXFIFO_EMPTY_THRESH);
		writeRegister(SI443X_TXFIFO_FULL, SI443X_TXFIFO_FULL_THRESH);
		writeRegister(SI443X_RXFIFO_FULL, SI443X_RXFIFO_FULL_THRESH);


		writeRegister(0x08, 0x10);	    // multi receive
		writeRegister(0x6D, 0x1F);	    // max power, LNA switch set

		writeRegister( 0x1C, 0x9A );
		writeRegister( 0x1D, 0x3C );
		writeRegister( 0x1E, 0x02 );
		writeRegister( 0x1F, 0x00 );
		writeRegister( 0x20, 0x77 );
		writeRegister( 0x21, 0x20 );
		writeRegister( 0x22, 0x2B );
		writeRegister( 0x23, 0xB1 );
		writeRegister( 0x24, 0x10 );
		writeRegister( 0x25, 0x59 );
		
		writeRegister( 0x2A, 0xFF );
		writeRegister( 0x2C, 0x18 );
		writeRegister( 0x2D, 0x4E );
		writeRegister( 0x2E, 0x2A );
		
		writeRegister( 0x30, 0x8D );
		writeRegister( 0x32, 0x00 );
		writeRegister( 0x33, 0x00 );
		writeRegister( 0x34, 0x08 );
		writeRegister( 0x35, 0x2A );
		
		writeRegister( 0x58, 0x80 );
		writeRegister( 0x69, 0x60 );
		writeRegister( 0x6E, 0x41 );
		writeRegister( 0x6F, 0x89 );
		writeRegister( 0x70, 0x2F );
		writeRegister( 0x71, 0x21 );
		writeRegister( 0x72, 0xA0 );
		
		writeRegister( 0x75, 0x4B );    // carrier freq
		writeRegister( 0x76, 0x7D );
		writeRegister( 0x77, 0x00 );
		
		_frequencyChange(SI443X_BASE_FREQ_10MHZ, SI443X_BASE_FREQ_KHZ);		
		writeRegister(SI443X_CHANNEL_STEPSIZE, SI443X_CHANNEL_STEP_10KHZ);
		writeRegister(SI443X_CHANNEL_SELECT, SI443X_DEF_CHANNEL);
		
/*
		writeRegister(0x08, 0x10);		// multi receive
		writeRegister(0x6D, 0x1F);		// max power, LNA switch set

		writeRegister( 0x1C, 0x8A );
		writeRegister( 0x1D, 0x3C );
		writeRegister( 0x1E, 0x02 );
		writeRegister( 0x1F, 0x00 );
		writeRegister( 0x20, 0x18 );
		writeRegister( 0x21, 0x02 );
		writeRegister( 0x22, 0xB8 );
		writeRegister( 0x23, 0x52 );
		writeRegister( 0x24, 0x15 );
		writeRegister( 0x25, 0x57 );

		writeRegister( 0x2A, 0xFF );
		writeRegister( 0x2C, 0x18 );
		writeRegister( 0x2D, 0x02 );
		writeRegister( 0x2E, 0x26 );

		writeRegister( 0x30, 0x8D );
		writeRegister( 0x32, 0x00 );
		writeRegister( 0x33, 0x00 );
		writeRegister( 0x34, 0x08 );
		writeRegister( 0x35, 0x2A );

		writeRegister( 0x58, 0xED );
		writeRegister( 0x69, 0x60 );
		writeRegister( 0x6E, 0x41 );
		writeRegister( 0x6F, 0x48 );
		writeRegister( 0x70, 0x0F );
		writeRegister( 0x71, 0x21 );
		writeRegister( 0x72, 0xA0 );

		_frequencyChange(SI443X_BASE_FREQ_10MHZ, SI443X_BASE_FREQ_KHZ);
		writeRegister(SI443X_CHANNEL_STEPSIZE, SI443X_CHANNEL_STEP_10KHZ);
		writeRegister(SI443X_CHANNEL_SELECT, SI443X_DEF_CHANNEL);*/
		
	}

	uint8_t _readRssi() {
		uint8_t r1,r2,r3;
		atomic {
			r1 = readRegister(SI443X_RSSI);
			r2 = readRegister(SI443X_RSSI);
			r3 = readRegister(SI443X_RSSI);
		}
		return ( r1 != r2 ) ? r3 : r1;
	}

/*----------------- SPI -----------------*/

	event void SpiResource.granted()
	{
		call Tasklet.schedule();
	}

	bool isSpiAcquired()
	{
		if( call SpiResource.isOwner() || SUCCESS == call SpiResource.immediateRequest() ) {
			return TRUE;
		}
		else {
			call SpiResource.request();
			return FALSE;
		}
	}

	task void releaseSpi()
	{
		call SpiResource.release();
	}

/*----------------- TASKLET HANDLER -----------------*/

#ifdef SI443X_GPIOCAPTURE
	async event void IRQ.captured(uint16_t time) {
		RADIO_ASSERT( ! radioIrq );
		atomic {
			radioIrq = TRUE;
			capturedTime = time;
		}
		call Tasklet.schedule();
	}
#else
	async event void IRQ.fired() {
		RADIO_ASSERT( ! radioIrq );
		atomic {
			radioIrq = TRUE;
			capturedTime = call LocalTime.get();
		}
		call Tasklet.schedule();
	}
#endif

	default tasklet_async event void RadioSend.sendDone(error_t error) { }
	default tasklet_async event void RadioSend.ready() { }
	void _downloadMessage();


/* ######################################################################################################################
   ###################################################################################################################### */

	void _fillTxFifo(uint8_t maxload) {
		uint8_t remains;
		
		//DIAGMSG_STR("fill","txfifo");
		RADIO_ASSERT( call SpiResource.isOwner() );
		RADIO_ASSERT( chip.state == STATE_RX || chip.state == STATE_TX );
		RADIO_ASSERT( maxload <= SI443X_FIFO_SIZE && maxload > 0 );
		RADIO_ASSERT( txMsg != NULL );
		
		// how many bytes are still not loaded into the FIFO
		remains = getHeader(txMsg)->length - plbyte; 
		
		// NB.: Here we intensively assume that sizeof(si443x_header_t) == 1.
		// If not, data := txMsg + sizeof(si443x_header_t)
		// and the data to be written into the FIFO should be data[plbyte++] instead of data[++plbyte]
		if ( remains != 0 ) {
			uint8_t* data = (uint8_t*)txMsg;
		
			// if we can load the msg till the end
			if ( maxload > remains ) {
				maxload = remains;
			}
			remains -= maxload;
			
			call NSEL.clr();
			call FastSpiByte.splitWrite(SI443X_SPI_WRITE | SI443X_FIFO);
			while ( maxload-- != 0 ) {
				call FastSpiByte.splitReadWrite( data[++plbyte] );
			}
			call FastSpiByte.splitRead();
			call NSEL.set();
		}
	}
	
	/*----------------- TRANSMIT -----------------*/

	tasklet_async command error_t RadioSend.send(message_t* msg)
	{
		uint8_t misc;
		void* timesync;
		uint32_t time32;
		
		#ifdef MOPT_ENABLE 
		signal ModemOptimizer.sendRequest();
		#endif
		
		if( chip.cmd != CMD_NONE || chip.state != STATE_RX || ! isSpiAcquired() ) {
			DIAGMSG_STR("send","BUSY");
			
			#ifdef MOPT_ENABLE
			signal ModemOptimizer.sendBusy();
			#endif
			
			return EBUSY;
		}
		
		// get the required RF power setting
		misc = (call PacketTransmitPower.isSet(msg) ? call PacketTransmitPower.get(msg) : SI443X_DEF_RFPOWER) & SI443X_RFPOWER_MASK;
		if( misc != txPower )
		{
			txPower = misc;
			_setPower(txPower);
		}

		// get payload size
		misc = getHeader(msg)->length;
		_setPktLength(misc);
		
		time32 = call LocalTime.get();
		timesync = call PacketTimeSyncOffset.isSet(msg) ? ((void*)msg) + call PacketTimeSyncOffset.get(msg) : (void*)NULL;
		
		// If timesync is active, prevent the last 4 bytes (timesync) to be loaded
		if ( timesync != NULL ) {
			misc -= sizeof(timesync_relative_t);
			// NB.: Since we inform the driver that the packet length is lets say L, and 
			// we fill in the FIFO maximum L-4 bytes, it may happen that L-4 < SI443X_TXFIFO_EMPTY_THRESH.
			// In this case neither a TX FIFO Almost Empty Interrupt nor a Packet Sent interrupt will fire,
			// instead we would have a FIFO Underflow Error.
			//
			// -> We must guarantee that a FIFO EMPTY interrupt will fire
			if ( misc < txEmptyThresh ) {
				txEmptyThresh = misc;
				writeRegister(SI443X_TXFIFO_EMPTY, txEmptyThresh);
			}
		}
		// Fill the first chunk of the message into the FIFO
		if ( misc > SI443X_FIFO_SIZE )
			misc = SI443X_FIFO_SIZE;

		// init data for fillTxFifo calls
		txMsg = msg;
		plbyte = 0;
		_fillTxFifo( misc );
		
		RADIO_ASSERT( chip.state == STATE_RX );
		
		// RSSI Clear Channel Assessment
		if( (call Config.requiresRssiCca(msg) && ( _readRssi() > ( (rssiClear >> 1) + (rssiBusy >> 1) )) ) ) {
			DIAGMSG_STR("send","BUSY-2");
			#ifdef MOPT_ENABLE
			signal ModemOptimizer.sendBusy();
			#endif
			_clearFifo(SI443X_CLEAR_TX_FIFO);
			return EBUSY;
		}
		
		if ( chip.cmd != CMD_NONE || radioIrq ) {
			DIAGMSG_STR("send","BUSY-3");
			#ifdef MOPT_ENABLE
			signal ModemOptimizer.sendBusy();
			#endif
			_clearFifo(SI443X_CLEAR_TX_FIFO);
			return EBUSY;
		}

		atomic {
			_tune();
			_transmit();
			time32 += (int16_t)(call RadioAlarm.getNow() + TX_SFD_DELAY) - (int16_t)(time32);
		}
		
		if( timesync != 0 )
			*(timesync_relative_t*)timesync = (*(timesync_absolute_t*)timesync) - time32;
		
		call PacketTimeStamp.set(msg,time32);
		/*
		#ifdef RADIO_DEBUG
		if( DM_ENABLE && call DiagMsg.record() )
		{
			call DiagMsg.str("tstamp2");
			call DiagMsg.uint32(time32);
			call DiagMsg.chr(call PacketTimeStamp.isValid(txMsg) ? '1' : '0');
			call DiagMsg.uint32(call PacketTimeStamp.timestamp(txMsg));
			call DiagMsg.uint16(call RadioAlarm.getNow());
			call DiagMsg.send();
		}
		#endif	
		*/
		#ifdef MOPT_ENABLE
		signal ModemOptimizer.sendAccept();
		#endif
		
		chip.cmd = CMD_TX_FINISH;
		return SUCCESS;
	}


	void serviceRadio()
	{
		uint8_t irq1, irq2;
		uint32_t time32;
		radioIrq = FALSE;
		
		irq1 = readRegister(SI443X_INT_1);
		irq2 = readRegister(SI443X_INT_2);

		//DIAGMSG_VAR("irq1",irq1);
		//DIAGMSG_VAR("irq2",irq2);
		
		if ( chip.state == STATE_RX ) {

			if ( irq1 & SI443X_I1_PKTRECEIVED ) {
				DIAGMSG_STR("Int","Pkt Received");
				RADIO_ASSERT( chip.cmd == CMD_RX_WAIT || chip.cmd == CMD_RX_FINISH || chip.cmd == CMD_RX_ABORT );

				// the most likely place for clear channel
				rssiClear = ( _readRssi() >> 1 ) + (rssiClear >> 1);
				_downloadMessage();
				if ( chip.cmd != CMD_RX_ABORT ) {
					rxMsg = signal RadioReceive.receive(rxMsg);
					#ifdef MOPT_ENABLE
					signal ModemOptimizer.receiveSuccess();
					#endif
				}
				chip.cmd = CMD_NONE;
			}
			
			if ( irq2 & SI443X_I2_SYNCDETECT ) {
				uint8_t temp;
				uint16_t time;
				atomic time = capturedTime;
				#ifdef MOPT_ENABLE
				signal ModemOptimizer.receiveSync();
				#endif
				DIAGMSG_STR("Int","Sync Detected");
				RADIO_ASSERT( chip.cmd == CMD_NONE || chip.cmd == CMD_FINISH_CCA );

				if( chip.cmd == CMD_FINISH_CCA )
				{
					signal RadioCCA.done(FAIL);
					chip.cmd = CMD_NONE;
				}
			
				// the most likely place for busy channel
				temp = _readRssi();
				rssiBusy = (temp >> 1) + (rssiBusy >> 1);
				call PacketRSSI.set(rxMsg, temp);
			
				// set the timestamp
				time32 = call LocalTime.get();
				time32 += (int16_t)(time - RX_SFD_DELAY) - (int16_t)(time32);
				call PacketTimeStamp.set(rxMsg, time32);
				
				chip.cmd = CMD_RX_WAIT;
			}

			if ( irq1 & SI443X_I1_RXFIFOFULL ) {
				DIAGMSG_STR("Int","RxFifo Full");
				DIAGMSG_VAR("cmd",chip.cmd);
				RADIO_ASSERT( chip.cmd == CMD_RX_WAIT || chip.cmd == CMD_RX_FINISH || chip.cmd == CMD_RX_ABORT );
				_downloadMessage();
			}
			
			if ( irq1 & SI443X_I1_CRCERROR ) {
				DIAGMSG_STR("Int","CRC Error");
				#ifdef MOPT_ENABLE
				signal ModemOptimizer.receiveCRC();
				#endif
				_downloadMessage();
				chip.cmd = CMD_NONE;
			}

			if ( irq1 & SI443X_I1_FIFOERROR ) {
				#ifdef MOPT_ENABLE
				signal ModemOptimizer.receiveError();			
				#endif
				DIAGMSG_STR("Int","FIFO Error");
				chip.cmd = CMD_NONE;
			}
			
		} else if ( chip.state == STATE_TX ) {
			RADIO_ASSERT( chip.cmd == CMD_TX_FINISH );

			if ( irq1 & SI443X_I1_TXFIFOEMPTY ) {
				DIAGMSG_STR("Int","TxFifo Empty");
				_fillTxFifo(SI443X_FIFO_SIZE - txEmptyThresh - 1);
			}

			if ( irq1 & SI443X_I1_PKTSENT ) {
				void* timesync;
				DIAGMSG_STR("Int","Pkt Sent");
				
				#ifdef RADIO_DEBUG
				if( DM_ENABLE && call DiagMsg.record() )
				{
					uint32_t freq = 0;
					uint8_t band = readRegister(0x75);
					uint8_t multi = ( (band & SI443X_FREQ_HBSEL) != 0) ? 20 : 10;
					uint8_t fc1 = readRegister(0x76);
					uint8_t fc2 = readRegister(0x77);
					uint16_t fc = (((uint16_t)fc1) << 8 ) + fc2;

					freq += ((uint32_t)(band & SI443X_FREQ_BAND_MASK) + 24)*multi*1000;
					freq += ((((uint32_t)fc) * multi ) >> 6);						
					freq += (uint32_t)readRegister(0x79) * readRegister(0x7A) * 10;
										
					call DiagMsg.str("freq");
					call DiagMsg.uint32( freq );
					call DiagMsg.str("kHz");
					call DiagMsg.send();
				}
				#endif	
	
				// restore the absolute value of timesync
				timesync = call PacketTimeSyncOffset.isSet(txMsg) ? ((void*)txMsg) + call PacketTimeSyncOffset.get(txMsg) : (void*)NULL;
				if( timesync != 0 )
					*(timesync_absolute_t*)timesync = (*(timesync_relative_t*)timesync) + call PacketTimeStamp.timestamp(txMsg);
			
				signal RadioSend.sendDone(SUCCESS);
				#ifdef MOPT_ENABLE
				signal ModemOptimizer.sendSuccess();
				#endif
		/*		
#ifdef RADIO_DEBUG
				if( DM_ENABLE && call DiagMsg.record() )
				{
					call DiagMsg.str("tstamp3");
					call DiagMsg.uint32(call PacketTimeStamp.isValid(txMsg) ? call PacketTimeStamp.timestamp(txMsg) : 0);
					call DiagMsg.uint16(call RadioAlarm.getNow());
					call DiagMsg.send();
				}
#endif				
*/

				_receive();
				chip.cmd = CMD_NONE;
				atomic txMsg = NULL;
			}
			if ( irq1 & SI443X_I1_FIFOERROR ) {	
				DIAGMSG_STR("Int","Fifo Error");
				_receive();
				#ifdef MOPT_ENABLE
				signal ModemOptimizer.sendError();
				#endif
				signal RadioSend.sendDone(FAIL);
				chip.cmd = CMD_NONE;
			}
		}

		/** MISC */
		if ( irq2 & SI443X_I2_RSSI && chip.cmd == CMD_FINISH_CCA ) {
			DIAGMSG_STR("Int","RSSI");
			signal RadioCCA.done(FAIL);
			chip.cmd = CMD_NONE;
		}

		if ( irq2 & SI443X_I2_POR ) {
			DIAGMSG_STR("Int","Power On Reset");
			chip.cmd = CMD_RESET;
		}
	}
	
	void _downloadMessage() {

		uint8_t hdrlen, remains;
		uint8_t fifoload = SI443X_RXFIFO_FULL_THRESH + 1;
		
		// NB.: Here we intensively assume that sizeof(si443x_header_t) == 1.
		// If not, data := rxMsg + sizeof(si443x_header_t)
		// and the data to be read from the FIFO should be data[plbyte++] instead of data[++plbyte]
		uint8_t* data = (uint8_t*)rxMsg;
		
		//DIAGMSG_STR("dload","msg");
		RADIO_ASSERT( call SpiResource.isOwner() );
		RADIO_ASSERT( chip.cmd == CMD_RX_WAIT || chip.cmd == CMD_RX_FINISH || chip.cmd == CMD_RX_ABORT );

		call NSEL.clr();
		call FastSpiByte.write(SI443X_SPI_READ | SI443X_FIFO);

		// if first call
		if ( chip.cmd == CMD_RX_WAIT ) {
			hdrlen = call Config.headerPreloadLength();

			// read packet length
			remains = call FastSpiByte.write(0);
			call NSEL.set();
			DIAGMSG_VAR("pktlen",remains);
			
			call NSEL.clr();
			call FastSpiByte.write(SI443X_SPI_READ | SI443X_FIFO);

			call RadioPacket.setPayloadLength(rxMsg, remains);
			plbyte = 0;
			
			// if correct length
			if ( remains >= 3 && remains <= call RadioPacket.maxPayloadLength() ) {
				if( remains < hdrlen )
					hdrlen = remains;

				// initiate the reading
				call FastSpiByte.splitWrite(0);

				// we are going to read hdrlen bytes
				// Note: we have already read 1B (pktlen) from FIFO
				fifoload -= hdrlen+1;
				remains -= hdrlen;

				// read header
				while( --hdrlen != 0 )
					data[++plbyte] = call FastSpiByte.splitReadWrite(0);
				data[++plbyte] = call FastSpiByte.splitRead();

				chip.cmd = (signal RadioReceive.header(rxMsg)) ? CMD_RX_FINISH : CMD_RX_ABORT;
			} else
				chip.cmd = CMD_RX_ABORT;
		} else {
			remains = getHeader(rxMsg)->length - plbyte;
		}
		RADIO_ASSERT(fifoload > 0);
		
		// compute how much data can be read from the FIFO
		if ( remains < fifoload ) {
			fifoload = remains;
		}
		
		if ( fifoload > 0 ) {
			call FastSpiByte.splitWrite(0);
			while( --fifoload != 0 )
				data[++plbyte] = call FastSpiByte.splitReadWrite(0);
			data[++plbyte] = call FastSpiByte.splitRead();
		}
		call NSEL.set();
	}


	tasklet_async event void Tasklet.run()
	{
		if( radioIrq && isSpiAcquired() )
			serviceRadio();

		if( chip.cmd != CMD_NONE && isSpiAcquired() )
		{
			if ( chip.cmd <= CMD_CHANNEL ) {
				switch ( chip.cmd ) {
					case CMD_CHANNEL:	_channel();	break;
					case CMD_TURNOFF:	_standby();	break;
					case CMD_STANDBY:	_ready();	break;
					case CMD_TURNON:	_receive();	break;
					default:
						RADIO_ASSERT(FALSE);
				}
				signal RadioState.done();
				chip.cmd = CMD_NONE;

			} else if ( chip.cmd == CMD_RESET ) {
				_reset();
				_setupModem();
				_standby();
				chip.cmd = CMD_NONE;
			}
		}

		if( chip.cmd == CMD_NONE && ( chip.state == STATE_READY || chip.state == STATE_RX ) && ! radioIrq )
			signal RadioSend.ready();

		if( chip.cmd == CMD_NONE )
			post releaseSpi();
	}


/* ######################################################################################################################
   ###################################################################################################################### */


/*----------------- DRIVER CONTROL -----------------*/

	command error_t PlatformInit.init()
	{
		call NSEL.makeOutput();
		call NSEL.set();
		call IRQ.disable();

		return SUCCESS;
	}

#ifdef RADIO_DEBUG	
	event void Boot.booted() {
		DM_ENABLE = TRUE;
		DIAGMSG_STR("booted","!");

	}
#endif

	command error_t SoftwareInit.init()
	{
		
		// these are just good approximates
		rssiClear = 50;
		rssiBusy = 250;
		rxMsg = &rxMsgBuffer;
		txMsg = NULL;
		plbyte = 0;
		txEmptyThresh = SI443X_TXFIFO_EMPTY_THRESH;
		
		chip.state = STATE_POR;
		chip.cmd = CMD_RESET;

		txPower = SI443X_DEF_RFPOWER & SI443X_RFPOWER_MASK;
		channel = SI443X_DEF_CHANNEL;

		return call SpiResource.request();
	}

	tasklet_async command error_t RadioState.turnOff()
	{
		if( chip.cmd != CMD_NONE )
			return EBUSY;
		else if( chip.state == STATE_SLEEP )
			return EALREADY;

		chip.cmd = CMD_TURNOFF;
		call Tasklet.schedule();
		return SUCCESS;
	}

	tasklet_async command error_t RadioState.standby()
	{
		if( chip.cmd != CMD_NONE )
			return EBUSY;
		else if( chip.state == STATE_READY )
			return EALREADY;

		chip.cmd = CMD_STANDBY;
		call Tasklet.schedule();
		return SUCCESS;
	}

	tasklet_async command error_t RadioState.turnOn()
	{
		if( chip.cmd != CMD_NONE )
			return EBUSY;
		else if( chip.state == STATE_RX )
			return EALREADY;

		chip.cmd = CMD_TURNON;
		call Tasklet.schedule();
		return SUCCESS;
	}

	tasklet_async command uint8_t RadioState.getChannel() { return channel; }

	tasklet_async command error_t RadioState.setChannel(uint8_t c)
	{
		if( chip.cmd != CMD_NONE )
			return EBUSY;
		else if( channel == c )
			return EALREADY;

		channel = c;
		chip.cmd = CMD_CHANNEL;
		call Tasklet.schedule();
		return SUCCESS;
	}

	tasklet_async command error_t RadioCCA.request()
	{
		if( chip.cmd != CMD_NONE || chip.state != STATE_RX || ! isSpiAcquired() || ! call RadioAlarm.isFree() )
			return EBUSY;

		writeRegister(SI443X_CCA_THRES, (rssiClear >> 1) + (rssiBusy >> 1));
		writeRegister(SI443X_IEN_2, readRegister(SI443X_IEN_2) | SI443X_I2_RSSI );
		chip.cmd = CMD_FINISH_CCA;					
		call RadioAlarm.wait(CCA_REQUEST_TIME);

		return SUCCESS;
	}

	default tasklet_async event void RadioCCA.done(error_t error) { }

	tasklet_async event void RadioAlarm.fired()
	{
		RADIO_ASSERT( chip.cmd == CMD_FINISH_CCA || chip.cmd == CMD_NONE );
		RADIO_ASSERT( chip.state == STATE_RX );
		
		if ( chip.cmd == CMD_FINISH_CCA ) {
			RADIO_ASSERT ( isSpiAcquired() );
			writeRegister(SI443X_IEN_2, readRegister(SI443X_IEN_2) & (~SI443X_I2_RSSI) );
	
			signal RadioCCA.done( SUCCESS );
			chip.cmd = CMD_NONE;
		}
		call Tasklet.schedule();
	}

/*----------------- RadioPacket -----------------*/

	async command uint8_t RadioPacket.headerLength(message_t* msg)
	{
		return call Config.headerLength(msg) + sizeof(si443x_header_t);
	}

	async command uint8_t RadioPacket.payloadLength(message_t* msg)
	{
		return getHeader(msg)->length;
	}

	async command void RadioPacket.setPayloadLength(message_t* msg, uint8_t length)
	{
		RADIO_ASSERT( length != 0 );
		RADIO_ASSERT( call RadioPacket.headerLength(msg) + length + call RadioPacket.metadataLength(msg) <= sizeof(message_t) );

		getHeader(msg)->length = length;
	}

	async command uint8_t RadioPacket.maxPayloadLength()
	{
		RADIO_ASSERT( call Config.maxPayloadLength() - sizeof(si443x_header_t) <= 255 );

		return call Config.maxPayloadLength() - sizeof(si443x_header_t);
	}

	async command uint8_t RadioPacket.metadataLength(message_t* msg)
	{
		return call Config.metadataLength(msg) + sizeof(si443x_metadata_t);
	}

	async command void RadioPacket.clear(message_t* msg)
	{
		// all flags are automatically cleared
	}

/*----------------- PacketTransmitPower -----------------*/

	async command bool PacketTransmitPower.isSet(message_t* msg)
	{
		return call TransmitPowerFlag.get(msg);
	}

	async command uint8_t PacketTransmitPower.get(message_t* msg)
	{
		return getMeta(msg)->power;
	}

	async command void PacketTransmitPower.clear(message_t* msg)
	{
		call TransmitPowerFlag.clear(msg);
	}

	async command void PacketTransmitPower.set(message_t* msg, uint8_t value)
	{
		call TransmitPowerFlag.set(msg);
		getMeta(msg)->power = value;
	}

/*----------------- PacketRSSI -----------------*/

	async command bool PacketRSSI.isSet(message_t* msg)
	{
		return call RSSIFlag.get(msg);
	}

	async command uint8_t PacketRSSI.get(message_t* msg)
	{
		return getMeta(msg)->rssi;
	}

	async command void PacketRSSI.clear(message_t* msg)
	{
		call RSSIFlag.clear(msg);
	}

	async command void PacketRSSI.set(message_t* msg, uint8_t value)
	{
		// just to be safe if the user fails to clear the packet
		call TransmitPowerFlag.clear(msg);

		call RSSIFlag.set(msg);
		getMeta(msg)->rssi = value;
	}

/*----------------- PacketTimeSyncOffset -----------------*/

	async command bool PacketTimeSyncOffset.isSet(message_t* msg)
	{
		return call TimeSyncFlag.get(msg);
	}

	async command uint8_t PacketTimeSyncOffset.get(message_t* msg)
	{
		return call RadioPacket.headerLength(msg) + call RadioPacket.payloadLength(msg) - sizeof(timesync_absolute_t);
	}

	async command void PacketTimeSyncOffset.clear(message_t* msg)
	{
		call TimeSyncFlag.clear(msg);
	}

	async command void PacketTimeSyncOffset.set(message_t* msg, uint8_t value)
	{
		// we do not store the value, the time sync field is always the last 4 bytes
		RADIO_ASSERT( call PacketTimeSyncOffset.get(msg) == value );

		call TimeSyncFlag.set(msg);
	}

/*----------------- PacketLinkQuality -----------------*/

	async command bool PacketLinkQuality.isSet(message_t* msg)
	{
		return TRUE;
	}

	async command uint8_t PacketLinkQuality.get(message_t* msg)
	{
		return getMeta(msg)->lqi;
	}

	async command void PacketLinkQuality.clear(message_t* msg)
	{
	}

	async command void PacketLinkQuality.set(message_t* msg, uint8_t value)
	{
		getMeta(msg)->lqi = value;
	}
/*----------------- LinkPacketMetadata -----------------*/

	async command bool LinkPacketMetadata.highChannelQuality(message_t* msg)
	{
		return call PacketLinkQuality.get(msg) > 200;
	}
}

