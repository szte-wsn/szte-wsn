module HplImpP {
  provides interface Atm128Spi as SPI;
  
  uses {
    interface GeneralIO as SS;
    interface GeneralIO as SCK;
    interface GeneralIO as MOSI;
    interface GeneralIO as MISO;
  }
}
implementation {

  async command void SPI.initMaster() {
    //UBRR0H = 0;
    //UBRR0L = 0;

    call SCK.makeOutput();    
    call SS.makeOutput();
    call SS.set();
    call MOSI.makeOutput();
    call MISO.makeInput();
    call SPI.setMasterBit(TRUE);
  }

  async command void SPI.initSlave() {
    //dummy
    call SPI.initMaster();
  }

  async command void SPI.sleep() {
  //    call SS.set();	// why was this needed?
  }

  async command uint8_t SPI.read()        { return UDR0; }
  async command void SPI.write(uint8_t d) { UDR0 = d; }

  default async event void SPI.dataReady(uint8_t d) {}
  AVR_ATOMIC_HANDLER(USART0_UDRE_vect) {
      signal SPI.dataReady(call SPI.read());
  }

   async command bool SPI.isInterruptPending() {
    return READ_BIT(UCSR0A, UDRE0);
  }

  async command bool SPI.isInterruptEnabled () {                
    return READ_BIT(UCSR0B, UDRIE0);
  }

  async command void SPI.enableInterrupt(bool enabled) {
    if (enabled) {
      SET_BIT(UCSR0B, UDRIE0);
     // call Mcu.update();
    }
    else {
      CLR_BIT(UCSR0B, UDRIE0);
      //call Mcu.update();
    }
  }

  async command bool SPI.isSpiEnabled() {
    return (UCSR0B & (1 << RXEN0 | 1 << TXEN0))?TRUE:FALSE;
  }
  
  async command void SPI.enableSpi(bool enabled) {
    if (enabled) {
      UCSR0B |= (1 << RXEN0) | (1 << TXEN0) /*| (1<<RXCIE0)*/;
     // call Mcu.update();
    }
    else {
      UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0) /*| (1<<RXCIE0)*/);
     // call Mcu.update();
    }
  }

  /* UDORD bit */
  async command void SPI.setDataOrder(bool lsbFirst) {
    if (lsbFirst) {
      SET_BIT(UCSR0C, UDORD0);
    }
    else {
      CLR_BIT(UCSR0C, UDORD0);
    }
  }
  
  async command bool SPI.isOrderLsbFirst() {
    return READ_BIT(UCSR0C, UDORD0);
  }

  /* MSTR bit */
  async command void SPI.setMasterBit(bool isMaster) {
    /* Only for backward compatibility */
    UCSR0C = (1<<UMSEL01) | (1<<UMSEL00);
  }
  async command bool SPI.isMasterBitSet() {
    return TRUE; // Hence this mode only provides master mode operation 
  }

 /* UCPOL bit */
  async command void SPI.setClockPolarity(bool highWhenIdle) {
    if (highWhenIdle) {
      //SET_BIT(UCSR0C, UCPOL0);
      UCSR0C |= (1 << UCPOL0) | (1 << UMSEL01) | (1 << UMSEL00);
    }
    else {
      //CLR_BIT(UCSR0C, UCPOL0);
      UCSR0C |= (1 << UMSEL01) | (1 << UMSEL00);
    }
  }
  
   async command bool SPI.getClockPolarity() {
    return READ_BIT(UCSR0C, UCPOL0);
  }

   /* UCPHA bit */
  async command void SPI.setClockPhase(bool sampleOnTrailing) {
    if (sampleOnTrailing) {
      //SET_BIT(UCSR0C, UCPHA0);
      UCSR0C |= (1 << UCPHA0) | (1 << UMSEL01) | (1 << UMSEL00);
    }
    else {call SCK.makeOutput();
      //CLR_BIT(UCSR0C, UCPHA0);
      UCSR0C |= (1 << UMSEL01) | (1 << UMSEL00);
    }
  }
  async command bool SPI.getClockPhase() {
    return READ_BIT(UCSR0C, UCPHA0);
  }

  async command uint8_t SPI.getClock () {                
    return PLATFORM_MHZ * 1000 / (2* UBRR0 +1);
  }
  
  async command void SPI.setClock (uint8_t Kbps) {
    UBRR0 = (((uint32_t)PLATFORM_MHZ * 1000000) / (2 * (uint32_t)Kbps * 1000)) - 1;
  }

  async command bool SPI.hasWriteCollided() {
    return FALSE; //dummy
  }

  async command bool SPI.isMasterDoubleSpeed() {
    return FALSE; //dummy
  }

  async command void SPI.setMasterDoubleSpeed(bool on) {
   //dummy
  }

  async command bool SPI.isTransferDone() {
    return UCSR0A & (1<<RXC0);
  }
}
