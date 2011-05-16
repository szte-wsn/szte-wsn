/*
 * Copyright (c) 2004-2005 Crossbow Technology, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of Crossbow Technology nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @author Martin Turon <mturon@xbow.com>
 * @author Miklos Maroti
 */

#include "hardware.h"

module PlatformP @safe()
{
	provides interface Init;

	uses
	{
		interface Init as McuInit;
		interface Init as LedsInit;
    interface Init as RadioInit;
    interface Init as Stm25pInit;
    interface Init as SubInit;
    interface GeneralIO as FlashCS;
    interface GeneralIO as RadioCS;
	}
}

implementation
{
	error_t powerInit()
	{
		atomic
		{
			MCUCR = _BV(SE);	// Internal RAM, IDLE, rupt vector at 0x0002,
			
		}

		return SUCCESS;
	}

	command error_t Init.init()
	{
      error_t ok;


      MCUCR |= 1<<JTD;
      MCUCR |= 1<<JTD; 
      
          // set the clock prescaler
      atomic {
        // enable changing the prescaler
              CLKPR = 0x80;
        #if PLATFORM_MHZ == 16
              CLKPR=0x0F;
        #elif PLATFORM_MHZ == 8
              CLKPR = 0x00;
        #elif PLATFORM_MHZ == 4
              CLKPR = 0x01;
        #elif PLATFORM_MHZ == 2
              CLKPR = 0x02;
        #elif PLATFORM_MHZ == 1
              CLKPR = 0x03;
        #else
            #error "Unsupported MHZ"
        #endif
      }

      ok = call McuInit.init();
      ok = ecombine(ok, call LedsInit.init());
      ok = ecombine(ok, powerInit());
      ok = ecombine(ok, call SubInit.init());
      call RadioInit.init();
      call Stm25pInit.init();
      call FlashCS.makeOutput();
      call RadioCS.makeOutput();
      call FlashCS.clr();
      call RadioCS.clr();
      return ok;
	}
	
	default command error_t SubInit.init()
    {
        return SUCCESS;
    }

	default command error_t LedsInit.init()
	{
		return SUCCESS;
	}
}
