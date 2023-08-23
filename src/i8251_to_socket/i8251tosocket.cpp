#include "i8251tosocket.h"



bool i8251toSocketClient::TxRDY(void)
{
	// Tx from VM point of view.  i.e., VM to outside.
	return false;
}
void i8251toSocketClient::Tx(unsigned char data)
{
}
void i8251toSocketClient::SetStopBits(unsigned char stopBits)
{
}
void i8251toSocketClient::SetParity(bool enabled,bool evenParity)
{
}
void i8251toSocketClient::SetDataLength(unsigned char bitsPerData)
{
}
void i8251toSocketClient::SetBaudRate(unsigned int baudRate)
{
}
void i8251toSocketClient::Command(bool RTS,bool DTR,bool BREAK)
{
}
bool i8251toSocketClient::RxRDY(void)
{
	// Rx from VM point of view.  i.e., Outside to VM.
	return false;
}
unsigned char i8251toSocketClient::Rx(void)
{
	return 0;
}
unsigned char i8251toSocketClient::PeekRx(void) const
{
	return 0;
}
bool i8251toSocketClient::DSR(void)
{
	return true; // Always ready
}


YSRESULT i8251toSocketClient::Received(YSSIZE_T nBytes,unsigned char dat[])
{
	return YSOK;
}

YSRESULT i8251toSocketClient::ConnectionClosedByServer(void)
{
	return YSOK;
}
