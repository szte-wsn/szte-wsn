#include "Messages.h"

configuration BenchmarkControlC {
  
  uses {
    interface Boot;
    interface BenchmarkCore;
    interface StdControl as CoreControl;
    interface Init as CoreInit;
  } 
 
}

implementation {
  components BenchmarkControlP as Comm;
  
  // Radiocommunication
  components new AMReceiverC(AM_CTRLMSG_T)    	    as RxCtrl;
  components new AMReceiverC(AM_SETUPMSG_T)    	    as RxSetup;
  
  components new DirectAMSenderC(AM_SYNCMSG_T)      as TxSync;
  components new DirectAMSenderC(AM_DATAMSG_T)      as TxData;
  
  Comm.RxCtrl -> RxCtrl;
  Comm.RxSetup -> RxSetup;  
  Comm.TxSync -> TxSync;
  Comm.TxData -> TxData;
    
  Comm.Boot = Boot;
  
  Comm.BenchmarkCore = BenchmarkCore;
  Comm.CoreControl = CoreControl;
  Comm.CoreInit = CoreInit;
  
  components ActiveMessageC;
  Comm.AMControl -> ActiveMessageC;
  Comm.Packet -> ActiveMessageC;
  
}
