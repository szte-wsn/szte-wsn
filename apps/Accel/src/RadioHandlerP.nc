#include "Timer.h"
#include "CtrlMsg.h"

module RadioHandlerP{

   uses {
		interface SplitControl as AMControl;
		interface Receive;
		interface Leds;
		interface Timer<TMilli> as TimerRadio;
   }
   
   provides {
		interface SplitControl;
   	}
}

implementation{
	
	// State
	bool SLEEP = TRUE;
	// Mode
	// TODO Sleep-awake-continuous

	command error_t SplitControl.stop(){
		// TODO Finish impl of stop
		// FIXME signal ?
		return call AMControl.stop();
	}

	command error_t SplitControl.start(){
		// FIXME Finish impl of start
		// FIXME signal ?
		return call AMControl.start();
	}
	

	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		if (len == sizeof(CtrlMsg)) { // TODO Enough?
			CtrlMsg* pkt = (CtrlMsg*)payload;
			call Leds.set(pkt->cmd); // TODO Check if meaningful?
		}
		return msg;
	}

	event void AMControl.stopDone(error_t error){
		if (error == SUCCESS) {
			SLEEP = TRUE;
			call Leds.led1Off();
			call TimerRadio.startOneShot(10000);
		}		
		else
			call Leds.led0On();
	}

	event void AMControl.startDone(error_t error){
		if (error == SUCCESS) {
			SLEEP = FALSE;
			call Leds.led1On();
			call TimerRadio.startOneShot(50);
		}		
		else
			call Leds.led0On();
	}

	event void TimerRadio.fired(){

		error_t error;

		if (SLEEP)
			error = call AMControl.start();
		else
			error = call AMControl.stop();
			
		if (error)
			call Leds.led0On();
	}
}