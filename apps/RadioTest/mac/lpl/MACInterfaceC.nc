
configuration MACInterfaceC {
  provides interface MACInterface;
}
implementation {
  
  components MILowPowerListeningP;
  components ActiveMessageC;
  
  MILowPowerListeningP.LowPowerListening -> ActiveMessageC;
  MACInterface = MILowPowerListeningP;
  
}
