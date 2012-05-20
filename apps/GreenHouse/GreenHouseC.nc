#include "GreenHouse.h"

configuration GreenHouseC{
}
implementation{

	components GreenHouseP, MainC;
	components LedsC;
	components new TimerMilliC();
	//Szenzor komponenes (h�m�rs�klet)
	components new SensirionSht11C();
	//K�t komponens az adatgy�jt�sben val� r�szv�telhez.*/
	components CollectionC as Collector;
	components new CollectionSenderC(CL_TEST);
	//Adatk�ld�shez haszn�lt komponenesek.
	components ActiveMessageC;
	components SerialActiveMessageC;
	/*	
		Csomagok kezel�s�re haszn�lt sor adatt�pusok.
		Tulajdonk�ppen bufferter�letet biztos�tanak, ami t�l gyors k�ld�si r�ta eset�n 
		a csomagok ideiglenes t�rol�s�ra alkalmas.
	*/
	components new QueueC(message_t*, QUEUE_SIZE) as QueueCtp;
	components new QueueC(message_t*, QUEUE_SIZE) as QueueSerial;
	components new QueueC(message_t*, QUEUE_SIZE) as QueueTemp;
	
	//Konzolra val� kiirat�sra val� komponens.
	components DiagMsgC;
	
	GreenHouseP.Boot	-> MainC;
	GreenHouseP.Leds	-> LedsC;
	GreenHouseP.Timer	-> TimerMilliC;
	GreenHouseP.Read	-> SensirionSht11C.Temperature;
	
	GreenHouseP.CtpControl		-> Collector;
	GreenHouseP.RootControl		-> Collector;
	GreenHouseP.CollectionPacket-> Collector;
	GreenHouseP.CtpInfo 		-> Collector;
	GreenHouseP.CtpCongestion 	-> Collector;
	GreenHouseP.CtpReceive 		-> Collector.Receive[CL_TEST];
	GreenHouseP.CtpSend 		-> CollectionSenderC;
	
	GreenHouseP.RadioControl 	-> ActiveMessageC;
	GreenHouseP.SerialControl 	-> SerialActiveMessageC;
	
	GreenHouseP.SerialSend -> SerialActiveMessageC.AMSend[AM_GREENHOUSE];
	
	GreenHouseP.QueueCtp	-> QueueCtp;
	GreenHouseP.QueueSerial	-> QueueSerial;
	GreenHouseP.QueueTemp	-> QueueTemp;
	
	GreenHouseP.DiagMsg -> DiagMsgC;
}