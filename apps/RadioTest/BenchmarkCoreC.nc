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
  
  components ActiveMessageC, MACInterfaceC;
  Core.MACInterface -> MACInterfaceC;
  Core.Packet -> ActiveMessageC;
  Core.Ack -> ActiveMessageC;
  
  components new TimerMilliC() as Timer;
  Core.TestTimer -> Timer;
  
  components LedsC;
  Core.Leds -> LedsC;
  
  components new VirtualizeTimerC(TMilli,MAX_TIMER_COUNT) as TTimer;
  components new TimerMilliC() as TTimerFrom;
  TTimer.TimerFrom -> TTimerFrom;
  Core.TriggerTimer -> TTimer;
  
  components RandomMlcgC;
  Core.Random -> RandomMlcgC;
  Core.RandomInit -> RandomMlcgC;
  
  StdControl = Core;
  BenchmarkCore = Core;
  Init = Core;

}
