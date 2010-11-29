
configuration MACInterfaceC {
  provides interface MACInterface;
}
implementation {
  
  components MINullP;
  MACInterface = MINullP;
  
}
