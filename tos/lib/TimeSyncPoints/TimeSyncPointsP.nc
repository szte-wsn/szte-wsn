/** Copyright (c) 2009, University of Szeged
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
* Author: Zoltan Kincses
*/

#include "TimeSyncPoints.h"

module TimeSyncPointsP
{
  provides
  {
    interface TimeSyncPoints;
    interface StdControl;
    interface Set<uint16_t> as SetInterval;
  }
  uses
  {	
    interface Receive as TimeSyncReceive;
    interface TimeSyncPacket<TMilli, uint32_t> as TimeSyncPacketMilli;
    interface PacketTimeStamp<TMilli, uint32_t> as PacketTimeStampMilli;
    interface TimeSyncAMSend<TMilli, uint32_t> as TimeSyncAMSendMilli;
    interface AMPacket;
    interface Timer<TMilli>;
    interface LocalTime<TMilli>;
  }
}
implementation
{

  message_t syncMessage;
  uint16_t period=TIMESYNCPOINTS_PERIOD;
  
  command error_t StdControl.start()
  {
      if(!call Timer.isRunning())
      {
          call Timer.startPeriodic(((uint32_t)period)<<10);
      }
      return SUCCESS;
  }
  
  command error_t StdControl.stop()
  {
      if (call Timer.isRunning())
      {
          call Timer.stop();
      }
      return SUCCESS;
  }
  
  command void SetInterval.set(uint16_t value){
    period=value;
    if(call Timer.isRunning())
    {
      call Timer.startPeriodic(((uint32_t)period)<<10);
    }
  }

  event void Timer.fired()
  {
      timeSyncPointsMsg_t* packet = (timeSyncPointsMsg_t*)(call TimeSyncAMSendMilli.getPayload(&syncMessage, sizeof(timeSyncPointsMsg_t)));
      packet->timeStamp = call LocalTime.get();
      call TimeSyncAMSendMilli.send(AM_BROADCAST_ADDR, &syncMessage, sizeof(timeSyncPointsMsg_t),packet->timeStamp);
  }
  
  event message_t* TimeSyncReceive.receive(message_t* msg,void* payload, uint8_t len)
  {
      if (call Timer.isRunning()&&call TimeSyncPacketMilli.isValid(msg))
      {
        timeSyncPointsMsg_t* syncMsg = (timeSyncPointsMsg_t*)payload;
        signal TimeSyncPoints.syncPoint(call TimeSyncPacketMilli.eventTime(msg), call AMPacket.source(msg), syncMsg->timeStamp);
      }
      return msg;
  }
  
  event void TimeSyncAMSendMilli.sendDone(message_t* Bufptr, error_t err){}
  default event void TimeSyncPoints.syncPoint(uint32_t local, am_addr_t nodeId, uint32_t remote){}
}

