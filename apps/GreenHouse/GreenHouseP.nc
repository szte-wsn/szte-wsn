#include <Timer.h>
#include "GreenHouse.h"
/**
komment teszt
*/
module GreenHouseP
{
	uses
	{
		interface Boot;
		interface Leds;
		interface StdControl as CtpControl;
		interface RootControl;
		interface SplitControl as RadioControl;
		interface SplitControl as SerialControl;
		//Szenzor�rt�k periodikus leolvas�s�t v�gz� interf�szek
		interface Timer<TMilli>;
		interface Read<uint16_t>;
		//CTP protokollal val� kapcsolatot megval�s�t� interf�szek
		interface CollectionPacket;
		interface CtpInfo;
		interface CtpCongestion;
		interface Send as CtpSend;
		interface Receive as CtpReceive;
		//Soros kapcsolatot megval�s�t� interf�sz
		interface AMSend as SerialSend;
		//A csomagforgalom bufferel�s�t megval�s�t� interf�szek
		interface Queue<message_t*> as QueueCtp;
		interface Queue<message_t*> as QueueSerial;
		interface Queue<message_t*> as QueueTemp;
		//Rendszer diagnosztiz�l�s
		interface DiagMsg;
	}
}

implementation
{
	uint16_t seqno;				//A szenzorlapka �ltal k�ld�tt csomagok sz�ma
	uint8_t readings = 0;		//0 - NREADINGS k�z�tt v�ltozhat
	
	//message_t a_queueCtp[QUEUE_SIZE];
	//message_t a_queueSerial[QUEUE_SIZE];
	message_t a_queueTemp[QUEUE_SIZE];
	message_t*	m_temp;
	
	//Kommunik�ci�s csatorn�k foglalt�s�g jelz� "flag"-ek
	bool CTPsendBusy = FALSE;
	bool UARTsendBusy = FALSE;
	//Kommunik�ci�s csatorn�kon val� k�ld�st v�gz� taszkok
	task void ctpSendTask();
	task void uartSendTask();

	//Piros LED = HIBA
	void errorBlink()	{ call Leds.led0Toggle(); }
	//Z�ld LED = M�R�S
	void radioreadBlink()	{ call Leds.led1Toggle(); }
	//K�k LED = SOROS PORTRA K�LD�S HIBA
	void serialErrorOn(){ call Leds.led2On(); }
	void serialErrorOff(){ call Leds.led2Off(); }
	
	event void Boot.booted()
	{
		int i;
		call SerialControl.start();
		call RadioControl.start();
		call CtpControl.start();

		if (TOS_NODE_ID % 200 == 0) {
			call RootControl.setRoot();
		}
		
		seqno = 0;
		//K�ld�si sorok felt�lt�se mutat�kkal
		for(i = 0; i < QUEUE_SIZE; i++)
		{
			/*
			call QueueCtp.enqueue( &a_queueCtp[i] );
			call QueueSerial.enqueue( &a_queueSerial[i] );
			*/
			call QueueTemp.enqueue( &a_queueTemp[i] );
		}
		m_temp = NULL;
		//Id�z�t� elind�t�sa
		call Timer.startPeriodic(DEFAULT_INTERVAL);
	}
	
	event void RadioControl.startDone(error_t err)
	{
		if( err != SUCCESS )
			errorBlink();
	}
	event void SerialControl.startDone(error_t err)
	{
		if( err != SUCCESS )
			errorBlink();
	}
	
	event void RadioControl.stopDone(error_t err) {}
	event void SerialControl.stopDone(error_t err) {}
	//Esem�ny: Id�z�t� lej�rt
	event void Timer.fired()
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("Timer fired");
			call DiagMsg.send();
		}
		
		if( readings < NREADINGS )
		{
			call Read.read();
			//readings++;	//Read.readDone() n�veli
		
			radioreadBlink();
		}
	}
	//Esem�ny: szenzor leolvas�sa k�sz
	event void Read.readDone(error_t result, uint16_t val )
	{
		GHMsg*	gh_temp;
		//Ha nincs akt�v buffer
		if(	m_temp == NULL )
		{
			//�s a szabad bufferek list�ja nem �res (azaz van szabad buffer)
			if( !call QueueTemp.empty() )
				//lefoglalunk egy buffert.
				m_temp = call QueueTemp.dequeue();
			else
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("QueueTemp is empty...");
					call DiagMsg.send();
				}
				return;				//Egy�bk�nt eldobjuk az adatot.
			}
		}
		
		gh_temp = (GHMsg*)call CtpSend.getPayload(m_temp, sizeof(GHMsg));
		//Ha siker�lt leolvasni a szenzor�rt�ket
		if(result == SUCCESS)
		{
			gh_temp->data[readings++] = val;
			
			//Ha �sszegy�lt egy csomagnyi m�rt adat
			if(readings == NREADINGS)
			{
				//Az �zenet beker�l a CTP k�ld�si sorba
				if( call QueueCtp.enqueue( m_temp ) == SUCCESS )
				{
					m_temp = NULL;
					readings = 0;
					post ctpSendTask();		//K�ld�s CTP-nek...
					
					if( call DiagMsg.record() )
					{
						call DiagMsg.str("Read.readDone.");
						call DiagMsg.str("QueueCtp size:");
						call DiagMsg.uint8(call QueueCtp.size());
						call DiagMsg.send();
					}
				}
				else
				{
					if( call DiagMsg.record() )
					{
						call DiagMsg.str("CTP packet dropped...");
						call DiagMsg.send();
					}
				}
				
			}
		}
	} 
	//Taszk: csomag k�ld�se a CTP motornak
	task void ctpSendTask()
	{
		GHMsg*	gh_temp;
		am_addr_t parent;
		uint16_t metric;
		
		//Ha van elk�ldend� �zenet
		if( !call QueueCtp.empty() )
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("CTP send task.");
				call DiagMsg.str("QueueCtp size:");
				call DiagMsg.uint8(call QueueCtp.size());
				call DiagMsg.send();
			}
			//Be�ll�tjuk a t�bbi mez� �rt�k�t
			gh_temp = (GHMsg*)call CtpSend.getPayload( call QueueCtp.head(), sizeof(GHMsg) );
			
			if( call CtpInfo.getParent(&parent) == FAIL )
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("getParent failure...");
					call DiagMsg.send();
				}
			}
			if( call CtpInfo.getEtx(&metric) == FAIL )
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("getEtx failure...");
					call DiagMsg.send();
				}
			}
			gh_temp->source = TOS_NODE_ID;
			gh_temp->parent = parent;
			gh_temp->metric = metric;
			gh_temp->seqno = seqno;
		}
		else
			return;
			
		//Ha szabad a r�di�
		if(!CTPsendBusy)
		{
			//�s a h�l�zati r�teg elfogadja a csomagot
			if ( call CtpSend.send( call QueueCtp.head(), sizeof(GHMsg) ) == SUCCESS )
			{
				CTPsendBusy = TRUE;
				seqno++;
			}
			else
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("CTP packet was not accepted to send...");
					call DiagMsg.send();
				}
				errorBlink();
			}
		}
		//post ctpSendTask();
	}
	//Esem�ny: k�ld�s befejezve
	event void CtpSend.sendDone(message_t* m, error_t err)
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("CtpSend.sendDone");
			call DiagMsg.send();
		}
		//Sikeres k�ld�s eset�n a mutat�t kiker�l a CTP sorb�l,
		//�s �tker�l az szabad mutat�k list�j�ba.
		if (err == SUCCESS)
		{
			call QueueCtp.dequeue();
			call QueueTemp.enqueue(m);
		}
		else
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("Failure sending packet...");
				call DiagMsg.send();
			}
			errorBlink();
		}
		
		//post ctpSendTask();
		
		CTPsendBusy = FALSE;
		//readings = 0;
		
	}
	//Esem�ny: csomag �rkezett
	//Csak a CTP gy�kere kapja meg.
	event message_t* CtpReceive.receive(message_t* msg, void* payload, uint8_t len)
	{
		message_t* p_ret;
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("CtpReceive.receive");
			call DiagMsg.send();
		}
		//�zenet "m�sol�sa"
		if( call QueueSerial.enqueue( msg ) == SUCCESS )
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("CtpReceive.receive");
				call DiagMsg.str("QueueSerial size:");
				call DiagMsg.uint8(call QueueSerial.size());
				call DiagMsg.send();
			}
			p_ret = msg;
		}
		//Ha van �res buffer, azzal t�r�nk vissza,
		//hogy a fogadott csomagot legyen id� feldolgozni.
		else if( !call QueueTemp.empty() )
			p_ret = call QueueTemp.dequeue();
		else
			p_ret = msg;
			
		post uartSendTask();	//K�ld�s PC-re...
		/*
		if (UARTsendBusy)
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("Serial busy...");
				call DiagMsg.send();
			}
		}
		*/
		return p_ret;
	}
	//Taszk: csomag k�ld�se a soros portra
	task void uartSendTask()
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("UART sender task.");
			call DiagMsg.send();
		}
		//Ha van elk�ldend� csomag, �s szabad a csatorna.
		if( !call QueueSerial.empty() && !UARTsendBusy )
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("UART sender task.");
				call DiagMsg.str("QueueSerial size:");
				call DiagMsg.uint8(call QueueSerial.size());
				call DiagMsg.send();
			}
			if( call SerialSend.send(AM_BROADCAST_ADDR, call QueueSerial.head(), sizeof(GHMsg)) == SUCCESS )
			{
				UARTsendBusy = TRUE;
				serialErrorOff();
			}
			else
			{
				if( call DiagMsg.record() )
				{
					call DiagMsg.str("UART packet was not accepted to send...");
					call DiagMsg.send();
				}
				serialErrorOn();
			}		
		}
		post uartSendTask();
	}
	//Esem�ny: soros porton val� k�ld�s befejezve
	event void SerialSend.sendDone(message_t *msg, error_t error)
	{
		//GHMsg* gh_temp;
		
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("SerialSend.sendDone");
			call DiagMsg.send();
		}
		//Sikeres k�ld�s eset�n a csomag mutat� visszaker�l a szabad bufferek list�j�ba
		if (error == SUCCESS)
		{
			call QueueSerial.dequeue();
			call QueueTemp.enqueue( msg );
			serialErrorOff();
			/*
			gh_temp = (GHMsg*)call SerialSend.getPayload(msg, sizeof(GHMsg));
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("SerialSend.sendDone");
				call DiagMsg.uint16( gh_temp -> source);
				call DiagMsg.uint16( gh_temp -> seqno);
				call DiagMsg.uint16( gh_temp -> parent);
				call DiagMsg.uint16( gh_temp -> metric);
				call DiagMsg.uint16s( (uint16_t*)(gh_temp -> data), NREADINGS );
				call DiagMsg.send();
			}
			*/
		}
		else
		{
			if( call DiagMsg.record() )
			{
				call DiagMsg.str("SerialSend.sendDone error.");
				call DiagMsg.send();
			}
			serialErrorOn();
		}
		
		UARTsendBusy = FALSE;
	}
}