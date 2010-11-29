module MINullP @safe() {
  provides interface MACInterface;
}

implementation {
  
  command void MACInterface.enter(message_t* packet, uint16_t* data) {
  }
  
  command void MACInterface.beforeSend(message_t* packet, uint16_t* data) {
  }
  
  command void MACInterface.afterSend(message_t* packet, uint16_t* data) {
  }
  
  command void MACInterface.exit(message_t* packet, uint16_t* data) {
  }
  
}
