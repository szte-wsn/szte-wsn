interface MACInterface {

  command void enter(message_t* packet, uint16_t* data);
  
  command void beforeSend(message_t* packet, uint16_t* data);
  
  command void afterSend(message_t* packet, uint16_t* data);
  
  command void exit(message_t* packet, uint16_t* data);

}
