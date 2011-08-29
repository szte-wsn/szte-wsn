/*
 * Copyright (c) 2011, University of Szeged
 * All rights reserved.
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
 * - Neither the name of the copyright holder nor the names of
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
 * Author: Andras Biro
 */

generic module AtmegaPinChangeP(){
  uses interface HplAtmegaPinChange;
  provides interface GpioInterrupt[uint8_t pin];
}
implementation{
  uint8_t isRising;
  
  /* Enables the interrupt */
  async command error_t GpioInterrupt.enableRisingEdge[uint8_t pin](){
    isRising |= 1<<pin;
    call HplAtmegaPinChange.setMask(call HplAtmegaPinChange.getMask() | (1<<pin));
    call HplAtmegaPinChange.enable();
    return SUCCESS;
  }
  
  async command error_t GpioInterrupt.enableFallingEdge[uint8_t pin](){
    isRising &= ~(1<<pin);
    call HplAtmegaPinChange.setMask(call HplAtmegaPinChange.getMask() | (1<<pin));
    call HplAtmegaPinChange.enable();
    return SUCCESS;
  }

  /* Disables the interrupt */
  async command error_t GpioInterrupt.disable[uint8_t pin](){
    uint8_t mask = call HplAtmegaPinChange.getMask() & ~(1<<pin);
    call HplAtmegaPinChange.setMask(mask);
    if(mask==0)
      call HplAtmegaPinChange.disable();
    return SUCCESS;
  }
  
  /* Signalled when any of the enabled pins changed */
  async event void HplAtmegaPinChange.fired(){
    uint8_t mask=call HplAtmegaPinChange.getMask();
    uint8_t pins=call HplAtmegaPinChange.getPins();
    if( ( mask & (1<<0) ) &&                             //interrupt enabled
        ( ( (pins & (1<<1)) && (isRising & (1<<1)) ) ||  //pin is high and rising edge int is enabled
        ( ~(pins & (1<<1)) && ~(isRising & (1<<1)) ) ) ) //pin is low and falling edge int is enabled
      signal GpioInterrupt.fired[1]();
    if( ( mask & (1<<2) ) &&                             //interrupt enabled
        ( ( (pins & (1<<2)) && (isRising & (1<<2)) ) ||  //pin is high and rising edge int is enabled
        ( ~(pins & (1<<2)) && ~(isRising & (1<<2)) ) ) ) //pin is low and falling edge int is enabled
      signal GpioInterrupt.fired[2]();
    if( ( mask & (1<<3) ) &&                             //interrupt enabled
        ( ( (pins & (1<<3)) && (isRising & (1<<3)) ) ||  //pin is high and rising edge int is enabled
        ( ~(pins & (1<<3)) && ~(isRising & (1<<3)) ) ) ) //pin is low and falling edge int is enabled
      signal GpioInterrupt.fired[3]();
    if( ( mask & (1<<4) ) &&                             //interrupt enabled
        ( ( (pins & (1<<4)) && (isRising & (1<<4)) ) ||  //pin is high and rising edge int is enabled
        ( ~(pins & (1<<4)) && ~(isRising & (1<<4)) ) ) ) //pin is low and falling edge int is enabled
      signal GpioInterrupt.fired[4]();
    if( ( mask & (1<<5) ) &&                             //interrupt enabled
        ( ( (pins & (1<<5)) && (isRising & (1<<5)) ) ||  //pin is high and rising edge int is enabled
        ( ~(pins & (1<<5)) && ~(isRising & (1<<5)) ) ) ) //pin is low and falling edge int is enabled
      signal GpioInterrupt.fired[5]();
    if( ( mask & (1<<6) ) &&                             //interrupt enabled
        ( ( (pins & (1<<6)) && (isRising & (1<<6)) ) ||  //pin is high and rising edge int is enabled
        ( ~(pins & (1<<6)) && ~(isRising & (1<<6)) ) ) ) //pin is low and falling edge int is enabled
      signal GpioInterrupt.fired[6]();
    if( ( mask & (1<<7) ) &&                             //interrupt enabled
        ( ( (pins & (1<<7)) && (isRising & (1<<7)) ) ||  //pin is high and rising edge int is enabled
        ( ~(pins & (1<<7)) && ~(isRising & (1<<7)) ) ) ) //pin is low and falling edge int is enabled
      signal GpioInterrupt.fired[7]();
  }
  
  default async event void GpioInterrupt.fired[uint8_t pin]() {}
}
