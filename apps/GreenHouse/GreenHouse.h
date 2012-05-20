#ifndef GREEN_HOUSE_H
#define GREEN_HOUSE_H

#include <AM.h>
#include <message.h>

enum
{
	CL_TEST = 0xee,
	/*M�r�sek sz�ma elk�ld�tt csomagonk�nt.*/
	/*N�vel�se eset�n esetleg meg kell n�velni a message_t m�ret�t (alap�rtelmezetten: 29 b�jt*/
	NREADINGS = 2,
	/*K�ld�si sorok m�rete.*/
	QUEUE_SIZE = 10,
	/*Szenzorok mintav�telez�si peri�dusa*/
	DEFAULT_INTERVAL = 1000,
	/*Akt�v �zenet azonos�t�ja a csomagt�pushoz.*/
	AM_GREENHOUSE = 0x77
};

/*A h�l�zaton tov�bb�tott �zenett�pus.*/
typedef nx_struct greenhouse {
  nx_am_addr_t source;
  nx_uint16_t seqno;
  nx_am_addr_t parent;
  nx_uint16_t metric;
  nx_uint16_t data[NREADINGS];
} GHMsg;

#endif /* GREEN_HOUSE_H */
