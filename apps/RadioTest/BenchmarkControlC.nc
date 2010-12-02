/*
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
* Author: Krisztian Veress
*         veresskrisztian@gmail.com
*/
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
  #ifdef TOSSIM
    components new SerialAMReceiverC(AM_CTRLMSG_T)    	as RxCtrl;
    components new SerialAMReceiverC(AM_SETUPMSG_T)    	as RxSetup;
  
    components new SerialAMSenderC(AM_SYNCMSG_T)        as TxSync;
    components new SerialAMSenderC(AM_DATAMSG_T)        as TxData;
    
    components SerialActiveMessageC;
    Comm.SerialAMControl -> SerialActiveMessageC;
  #else
    components new AMReceiverC(AM_CTRLMSG_T)    	      as RxCtrl;
    components new AMReceiverC(AM_SETUPMSG_T)    	      as RxSetup;
  
    components new DirectAMSenderC(AM_SYNCMSG_T)        as TxSync;
    components new DirectAMSenderC(AM_DATAMSG_T)        as TxData;
  #endif
  
  components LedsC;
  Comm.Leds -> LedsC;
  
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
