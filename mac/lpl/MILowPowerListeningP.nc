module MILowPowerListeningP @safe() {
  provides interface MACInterface;
  uses interface LowPowerListening;
}

implementation {
  
  command void MACInterface.enter(message_t* packet, uint16_t* data) {
    call LowPowerListening.setLocalWakeupInterval(data[0]);
    call LowPowerListening.setRemoteWakeupInterval(packet,data[0]);
  }
  
  command void MACInterface.beforeSend(message_t* packet, uint16_t* data) {
    call LowPowerListening.setRemoteWakeupInterval(packet,data[0]);
  }
  
  command void MACInterface.afterSend(message_t* packet, uint16_t* data) {
  }
  
  command void MACInterface.exit(message_t* packet, uint16_t* data) {
    call LowPowerListening.setLocalWakeupInterval(0);
    call LowPowerListening.setRemoteWakeupInterval(packet,0);
  }
  
}
