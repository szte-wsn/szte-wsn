
#include "SerialListener.h"
#include "qextserialport.h"
#include <QtDebug>

SerialListener::SerialListener()
{
	port = NULL;
	badPacketCount = 0;
	timerId = -1;
}

SerialListener::~SerialListener()
{

}

void SerialListener::disconnectPort(const char * message)
{
	if( timerId >= 0 )
	{
		killTimer(timerId);
		timerId = -1;
	}

	if( port != NULL )
	{
		disconnect(port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

		port->close();
		delete port;
		port = NULL;
	}

	emit showNotification(message);
}

void SerialListener::onPortChanged(const QString & portName, int baudRate)
{
	if( port != NULL || timerId >= 0 )
		disconnectPort("Disconnected.");

	if( portName == "" || baudRate == 0 )
		return;

	if( portName == "simulated" )
	{
		timerId = startTimer(20);
		emit showNotification("Generating simulated messages.");
		return;
	}

	port = new QextSerialPort(portName, QextSerialPort::EventDriven);

	if( baudRate == 57600 )
		port->setBaudRate(BAUD57600);
	else if( baudRate == 115200 )
		port->setBaudRate(BAUD115200);
	else if( baudRate == 230400 )
		port->setBaudRate(BAUD230400);
	else if( baudRate == 460800 )
		port->setBaudRate(BAUD460800);
	else if( baudRate == 921600 )
		port->setBaudRate(BAUD921600);

	port->setFlowControl(FLOW_OFF);
	port->setParity(PAR_NONE);
	port->setDataBits(DATA_8);
	port->setStopBits(STOP_1);

	if( port->open(QIODevice::ReadWrite) != true )
		disconnectPort("Port could not be opened.");
	else
	{
		badPacketCount = -1;
		partialPacket.clear();

		emit showNotification("Connected.");
		connect(port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	}

}

void SerialListener::onReadyRead()
{
	QByteArray bytes = port->readAll();

	for(int i = 0; i < bytes.size(); ++i)
	{
		char c = bytes.at(i);

		if( escaped )
		{
			escaped = false;
			c ^= (char)0x20;
			partialPacket.append(c);
		}
		else if( c == (char)0x7d )
			escaped = true;
		else if( c == (char)0x7e )
		{
			if( partialPacket.size() > 0 )
			{
				receiveRawPacket(partialPacket);
				partialPacket.clear();
			}
		}
		else if( partialPacket.size() > 1000 )
		{
			disconnectPort("Too long packet, disconnected.");
			return;
		}
		else
			partialPacket.append(c);
	}
}

int calcCrc(int crc, char b)
{
	crc = crc ^ ((int)b << 8);

	for(int i = 0; i < 8; i++)
	{
		if( (crc & 0x8000) == 0x8000 )
			crc = crc << 1 ^ 0x1021;
		else
			crc = crc << 1;
	}

	return crc & 0xffff;
}

void SerialListener::receiveRawPacket(const QByteArray & packet)
{
	int size = packet.size();

	if( size < 3 || packet.at(0) != (char)0x45 )
	{
		if( ++badPacketCount > 100 )
			disconnectPort("Too many bad packets, disconnected.");
		else if( badPacketCount > 0 )
			emit showNotification("Bad packet format.", 1000);

		return;
	}

	int crc = 0;
	for(int i = 0; i < size - 2; ++i)
		crc = calcCrc(crc, packet.at(i));

	crc = calcCrc(crc, packet.at(size-1));
	crc = calcCrc(crc, packet.at(size-2));

	if( crc != 0 )
	{
		if( ++badPacketCount > 100 )
			disconnectPort("Too many bad packets, disconnected.");
		else
			emit showNotification("Bad packet crc.", 1000);

		return;
	}

	badPacketCount = 0;

	QByteArray decoded(packet.data() + 1, size - 3);
	receiveTosPacket(decoded);
}

void SerialListener::receiveTosPacket(const QByteArray & packet)
{
	if( packet.size() >= 8 && packet.at(0) == (char)0x00 )
	{
		int length = packet.at(5) & 0xFF;

		if( packet.size() != length + 8 )
		{
			emit showNotification("Bad packet length.");
			return;
		}

		ActiveMessage msg;

		msg.dest = ((packet.at(1) & 0xFF) << 8) + (packet.at(2) & 0xFF);
		msg.source = ((packet.at(3) & 0xFF) << 8) + (packet.at(4) & 0xFF);
		msg.group = packet.at(6) & 0xFF;
		msg.type = packet.at(7) & 0xFF;
		msg.payload.append(packet.data() + 8, length);

		emit receiveMessage(msg);
	}
}

void SerialListener::timerEvent(QTimerEvent *event)
{
	ActiveMessage msg;

//	TODO: generated valid message
	msg.dest = 65545;
	msg.source = 13;
	msg.group = 0;
	msg.type = 12;
	msg.payload.append('a');

	emit receiveMessage(msg);
}

unsigned char ActiveMessage::getByte(int index) const
{
	return (unsigned char)payload.at(index);
}

unsigned short ActiveMessage::getShort(int index) const
{
	unsigned short a = ((payload.at(index) & 0xFF) << 8) + (payload.at(index) & 0xFF);
	return a;
}

unsigned int ActiveMessage::getInt(int index) const
{
	unsigned int a = getShort(index);
	a <<= 16;
	return a + getShort(index + 2);
}

