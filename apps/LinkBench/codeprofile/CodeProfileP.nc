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

#define ATOMIC_PERIODIC_TIME 1024

#define _MAX_(a,b) (((a) < (b)) ? (b) : (a))

module CodeProfileP @safe() {
  provides {
    interface StdControl;
    
    interface CodeProfile;
    interface Get<uint32_t> as MaxInterruptLength;
    interface Get<uint32_t> as MaxAtomicLength;
    interface Get<uint32_t> as MaxTaskLatency;
    
  }
  uses {
    interface Alarm<TMicro, uint32_t> as Alarm;
  }
}

implementation {

  uint32_t        mil;           // Maximum Interrupt Length
  uint32_t        mal;           // Maximum Atomic Length
  uint32_t        mtl;           // Maximum Task Latency
  
  uint32_t        mtl_offset;
  uint32_t        mal_offset;
  norace bool     alive;

  command uint32_t CodeProfile.getMaxInterruptLength()  { return mil; }
  command uint32_t CodeProfile.getMaxAtomicLength()     { atomic {return mal;} }
  command uint32_t CodeProfile.getMaxTaskLatency()      { return mtl; }

  command uint32_t MaxInterruptLength.get()             { return mil; }
  command uint32_t MaxAtomicLength.get()                { return mal; }
  command uint32_t MaxTaskLatency.get()                 { return mtl; }

  task void measureTask() {
    
    uint32_t t1 = call Alarm.getNow();
    uint32_t t2 = call Alarm.getNow();
    
    // The difference between two consecutive getNow() call can be
    // significantly greater than zero, if interrupt(s) occured in between. That
    // difference is proportional to the running time of the 
    // interrupt handler.
    mil = _MAX_(t2-t1,mil);
    
    // The difference between the posting time of this task (mtl_offset)
    // and the first expression's execution time ( t1 ) is the time
    // between two measureTask tasks.
    // This way, interleaving tasks' running time is measured.    
    mtl = _MAX_(t1-mtl_offset,mtl);

    if ( alive ) {
      mtl_offset = call Alarm.getNow();
      post measureTask();
    }
    
  }


  command error_t StdControl.start() {
    
    alive = TRUE;
    mil = mtl = 0;
    
    // Atomic Length Measurement Init
    atomic {
      mal = 0;
      call Alarm.stop();
      mal_offset = call Alarm.getNow();
      call Alarm.startAt(mal_offset, ATOMIC_PERIODIC_TIME);
    }
    
    mtl_offset = call Alarm.getNow();
    post measureTask();
    
    return SUCCESS;
  }
  
  command error_t StdControl.stop() {
    call Alarm.stop();
    alive = FALSE;
    return SUCCESS;
  }
  
  async event void Alarm.fired() {    
    // Get the time
    uint32_t delay = call Alarm.getNow();
  
    atomic {
      // When the alarm should have been fired?
      // This is also the base of the next fire target.
      mal_offset += ATOMIC_PERIODIC_TIME;

      // Compute the shift between now and the target    
      delay -= mal_offset;
    
      mal = _MAX_(delay,mal);  
    }    
      
    if ( alive )
      call Alarm.startAt(mal_offset,ATOMIC_PERIODIC_TIME);
  }
   
}
