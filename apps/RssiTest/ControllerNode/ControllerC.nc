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

#include "Packet.h"

module ControllerC
{
	uses
	{
		interface SplitControl;
		interface Boot;
		interface Timer<TMilli>;
		interface Leds;
		interface Packet;
		interface AMSend;
		interface Receive;
	}

}
implementation
{
	message_t msg;
	bool stopSend;
	uint8_t nodeNum;
	
	inline void send_data()
	{
		controlpacket_t *strpkt = (controlpacket_t*)(call Packet.getPayload(&msg, sizeof(controlpacket_t)));
		strpkt->nodeID=TOS_NODE_ID;
		strpkt->instr='r';
		call AMSend.send(AM_BROADCAST_ADDR, &msg, sizeof(controlpacket_t));
		call Leds.led0Toggle();
	}
	
	event void Boot.booted()
	{
		call SplitControl.start();
	}
	
	event void SplitControl.startDone(error_t err)
	{
		if (err==SUCCESS)
		{
			//nothing to do
		}
		else
		{
			call SplitControl.start();
		}
	}
	
	event void SplitControl.stopDone(error_t err)
	{
		//no action
	}
	
	event message_t* Receive.receive(message_t* ctrlmsg,void* payload, uint8_t len)
	{
		if (len == sizeof(controlpacket_t))
		{
			controlpacket_t *ctrlpkt = (controlpacket_t*)payload;
			if(ctrlpkt->nodeID==TOS_NODE_ID && ctrlpkt->instr=='s')
			{
				stopSend=FALSE;
				send_data();
				call Timer.startOneShot(1000);
			}
		}
		return ctrlmsg;	
			
    }

    event void Timer.fired()
    {
    	stopSend = TRUE;
    }

    event void AMSend.sendDone(message_t* bufPtr, error_t error)
    {
	   if (&msg==bufPtr && !stopSend)
	   {
		send_data();
	   }
    }
}

