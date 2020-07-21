#ifndef I8251_IS_INCLUDED
#define I8251_IS_INCLUDED
/* { */

class i8251
{
public:
	enum
	{
		MODESEL_ASYNC_S= 0xC0,
		MODESEL_ASYNC_S2=0x80,
		MODESEL_ASYNC_S1=0x40,
		MODESEL_ASYNC_S_SHIFT=6,

		MODESEL_SYNC_SCS=0x80,
		MODESEL_SYNC_ESD=0x40,

		MODESEL_EP=      0x20,  // Even Parity
		MODESEL_PEN=     0x10,  // Parity Enabled
		MODESEL_L=       0x0C,
		MODESEL_L2=      0x08,
		MODESEL_L1=      0x04,
		MODESEL_L_SHIFT=    2,
		MODESEL_B=       0x03,
		MODESEL_B2=      0x02,
		MODESEL_B1=      0x01,
		MODESEL_B_SHIFT=    0,

		CMD_WRITE_EH=   0x80,
		CMD_WRITE_RESET=0x40,
		CMD_WRITE_RTS=  0x20,
		CMD_WRITE_CLRER=0x10,
		CMD_WRITE_SBRK= 0x08,
		CMD_WRITE_RXEN= 0x04,
		CMD_WRITE_DTR=  0x02,
		CMD_WRITE_TXEN= 0x01,

		STATUS_READ_TXRDY= 0x01,
		STATUS_READ_RXRDY= 0x02,
		STATUS_READ_TXEMPTY= 0x04,
		STATUS_READ_PE=    0x08,
		STATUS_READ_OE=    0x10,
		STATUS_READ_FE=    0x20,
		STATUS_READ_SYNDET=0x40,
		STATUS_READ_DSR=   0x80,
	};

	enum
	{
		STOPBITS_NONE =0,
		STOPBITS_1    =1,
		STOPBITS_1HALF=2,
		STOPBITS_2    =3,
	};

	/*! Client class should implement a device connected from the serial port.
	*/
	class Client
	{
	public:
		/*! i8251 class will call this function to see if the client is ready to receive a byte.
		*/
		virtual bool TxRDY(void)=0;

		/*! i8251 class will call this function when a data is written from the VM.
		    This function is called when the VM writes a byte to the data register.
		    It can happen regardless of TxRDY.
		*/
		virtual void Tx(unsigned char data)=0;

		/*! i8251 class will call when the VM sets stop bits.
		    stopBits is one of STOPBITS_NONE, STOPBITS_1, STOPBITS_1HALF, or STOPBITS_2.
		*/
		virtual void SetStopBits(unsigned char stopBits)=0;

		/*!
		*/
		virtual void SetParity(bool enabled,bool evenParity)=0;

		/*! bitsPerData is 5,6,7, or 8.
		*/
		virtual void SetDataLength(unsigned char bitsPerData)=0;

		/*! This function will be called when VM changes the baud rate.
		*/
		virtual void SetBaudRate(unsigned int baudRate)=0;

		/*! i8251 will call this function when a command byte is written from the VM.
		    This can be used if the data is forwarded to an actual serial port.
		*/
		virtual void Command(bool RTS,bool DTR,bool BREAK)=0;

		/*! It should return true if there is a data transmitted from the host to VM.
		*/
		virtual bool RxRDY(void)=0;

		/*! It should return a data transmitted from the host to VM.
		    This function is called when the VM reads the data bus of i8251 regardless of RxRDY.
		*/
		virtual unsigned char Rx(void)=0;

		/*!
		*/
		virtual bool DSR(void)=0;
	};

	class State
	{
	public:
		long long int lastTxTime; // In nano seconds.
		long long int lastRxTime; // in nano seconds.

		bool immediatelyAfterReset=true;

		// Must depend on the baud rate, but the emulator can prescribe here.
		unsigned int nanoSecondsPerByte=8583; // 8583:Roubnly 1M bps.  Emulator can do anything.

		// Baud rate for recording VM setting.
		unsigned int baudRate=19200;

		// Asynchronous mode states
		unsigned char stopBits;
		unsigned char preScale;   // Prescale==0 means synchronous mode.

		// Synchronous mode states
		bool SCS;
		bool ESD;

		// Common states
		unsigned char dataLength; // Number of bits. 5,6,7 or 8.  Only supports 8-bit mode.
		bool evenParity;
		bool parityEnabled;

		bool RxEN,TxEN;
		bool RxRDY,TxRDY;
		bool SYNDET,FE,OE,PE,TxEMPTY;
		bool RTS,DTR,BREAK;
	};

	Client *clientPtr=nullptr;

	State state;

	i8251();

	void Reset(void);

	/*! Recommended to Update(VMTime); before calling this function.
	*/
	void VMWriteCommnand(unsigned char data);

	/*! Recommended to Update(VMTime); before calling this function.
	*/
	unsigned char VMReadState(void) const;

	void VMWriteData(unsigned char data,long long int VMTime);
	unsigned char VMReadData(long long int VMTime);

	/*! Updates the device state.
	    newT is the time in nano seconds.
	*/
	void Update(long long int newVMTime);

	bool RxRDY(void) const;
	bool TxRDY(void) const;
	bool TxEMPTY(void) const;
	bool DSR(void) const;
};

/* } */
#endif
