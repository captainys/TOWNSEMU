#ifndef I8251_TO_SOCKET_H_IS_INCLUDED
#define I8251_TO_SOCKET_H_IS_INCLUDED

#include "i8251.h"
#include "yssocket.h"


class i8251toSocketClient : public i8251::Client, public YsSocketClient
{
public:
	/*! i8251 class will call this function to see if the client is ready to receive a byte.
	*/
	virtual bool TxRDY(void) override;

	/*! i8251 class will call this function when a data is written from the VM.
	    This function is called when the VM writes a byte to the data register.
	    It can happen regardless of TxRDY.
	*/
	virtual void Tx(unsigned char data) override;

	/*! i8251 class will call when the VM sets stop bits.
	    stopBits is one of STOPBITS_NONE, STOPBITS_1, STOPBITS_1HALF, or STOPBITS_2.
	*/
	virtual void SetStopBits(unsigned char stopBits) override;

	/*!
	*/
	virtual void SetParity(bool enabled,bool evenParity) override;

	/*! bitsPerData is 5,6,7, or 8.
	*/
	virtual void SetDataLength(unsigned char bitsPerData) override;

	/*! This function will be called when VM changes the baud rate.
	*/
	virtual void SetBaudRate(unsigned int baudRate) override;

	/*! i8251 will call this function when a command byte is written from the VM.
	    This can be used if the data is forwarded to an actual serial port.
	*/
	virtual void Command(bool RTS,bool DTR,bool BREAK) override;

	/*! It should return true if there is a data transmitted from the host to VM.
	*/
	virtual bool RxRDY(void) override;

	/*! It should return a data transmitted from the host to VM.
	    This function is called when the VM reads the data bus of i8251 regardless of RxRDY.
	*/
	virtual unsigned char Rx(void) override;

	/*! It should return a data to be transmitted in the next Rx(), but not change the state.
	*/
	virtual unsigned char PeekRx(void) const override;

	/*!
	*/
	virtual bool DSR(void) override;
};


#endif
