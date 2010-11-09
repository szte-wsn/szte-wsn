#include "Messages.h"

configuration BenchmarkCoreC {

  provides {
    interface StdControl;
    interface BenchmarkCore;
    interface Init;
  }

}

implementation {
  components BenchmarkCoreP as Core;
  
  components new DirectAMSenderC(AM_TESTMSG_T)	    as TxTest;
  components new AMReceiverC(AM_TESTMSG_T)    	    as RxTest;
  Core.RxTest -> RxTest;
  Core.TxTest -> TxTest;
  
  components ActiveMessageC;
  Core.LowPowerListening -> ActiveMessageC; 
  Core.AMPacket -> ActiveMessageC;
  Core.Packet -> ActiveMessageC;
  Core.Ack -> ActiveMessageC;
  
  components new TimerMilliC() as Timer;
  Core.TestTimer -> Timer;
  
  components LedsC;
  Core.Leds -> LedsC;
  
  StdControl = Core;
  BenchmarkCore = Core;
  Init = Core;
  

}
