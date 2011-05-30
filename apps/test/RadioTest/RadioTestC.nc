 
#include "Timer.h"
#include "RadioTest.h"
#include "message.h"

#define DIAGMSG_STR(p) if( call DiagMsg.record() ) { call DiagMsg.str(p); call DiagMsg.send(); }
#define DIAGMSG_UINT(p) if( call DiagMsg.record() ) { call DiagMsg.uint8(p); call DiagMsg.send();}

module RadioTestC @safe() {
  uses {
    interface Leds;
    interface Boot;
    interface Timer<TMilli> as MilliTimer;
    interface DiagMsg;
    interface RadioState;
  }
}
implementation {

  event void Boot.booted() {  
    DIAGMSG_STR("booted");
    call MilliTimer.startPeriodic(1000);
  }

 
  event void MilliTimer.fired() {
    DIAGMSG_STR("MT fired");
  }

  async event void RadioState.done() {
    DIAGMSG_STR("RadioState done");
  }

}



