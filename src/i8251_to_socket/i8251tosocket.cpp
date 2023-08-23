#include "i8251tosocket.h"



bool i8251toSocketClient::TxRDY(void)
{
	// Tx from VM point of view.  i.e., VM to outside.
	return (YSTRUE==IsConnected());
}
void i8251toSocketClient::Tx(unsigned char data)
{
	Send(1,&data,1);
}
void i8251toSocketClient::SetStopBits(unsigned char)
{
}
void i8251toSocketClient::SetParity(bool,bool)
{
}
void i8251toSocketClient::SetDataLength(unsigned char)
{
}
void i8251toSocketClient::SetBaudRate(unsigned int)
{
}
void i8251toSocketClient::Command(bool,bool,bool)
{
}
bool i8251toSocketClient::RxRDY(void)
{
	// Rx from VM point of view.  i.e., Outside to VM.
	CheckReceive();
	return (0<toVM.size());
}
unsigned char i8251toSocketClient::Rx(void)
{
	if(0<toVM.size())
	{
		auto data=toVM[0];
		toVM.erase(toVM.begin());
		return data;
	}
	return 0;
}
unsigned char i8251toSocketClient::PeekRx(void) const
{
	if(0<toVM.size())
	{
		return toVM[0];
	}
	return 0;
}
bool i8251toSocketClient::DSR(void)
{
	return true; // Always ready
}


YSRESULT i8251toSocketClient::Received(YSSIZE_T nBytes,unsigned char dat[])
{
	for(int i=0; i<nBytes; ++i)
	{
		toVM.push_back(dat[i]);
	}
	return YSOK;
}

YSRESULT i8251toSocketClient::ConnectionClosedByServer(void)
{
	return YSOK;
}
