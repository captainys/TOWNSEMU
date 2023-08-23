#ifndef SERIALPORT_IS_INCLUDED
#define SERIALPORT_IS_INCLUDED
/* { */

#include <vector>
#include "i8251.h"
#include "i8251tosocket.h"
#include "device.h"



class TownsSerialPort : public Device
{
private:
	class FMTownsCommon *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "RS232C";} // You must implement it.

	enum
	{
		CODE_SOH=0x01,
		CODE_STX=0x02,
		CODE_EOT=0x04,
		CODE_EOF=0x05, // Is it right?
		CODE_ACK=0x06,
		CODE_NAK=0x15,
		CODE_CAN=0x18,
	};

	enum
	{
		INTENABLE_TXRDY=0x01,
		INTENABLE_RXRDY=0x02,
		INTENABLE_SYNDET=0x04,
		INTENABLE_CS=0x08,
		INTENABLE_CI=0x10,
	};

	class State
	{
	public:
		i8251 intel8251;
		unsigned char INTEnableBits=0;
		bool INTbyTxRDY_RxRDY_SYNDET=false;

		void PowerOn(void);
		void Reset(void);

		void UpdateINTState(void);
	};

	class DefaultClient : public i8251::Client
	{
	public:
		long long int toVMPtr=0;
		std::vector <unsigned char> fromVM,toVM;

		enum
		{
			FILETFR_NONE,
			FILETFR_XMODEM_TO_VM,
			FILETFR_XMODEM1024_TO_VM,
			FILETFR_XMODEM_FROM_VM,
		};
		unsigned int fileTfrMode=FILETFR_NONE;
		unsigned int fileTfrPtr=0;
		std::vector <unsigned char> fileTfrData;
		bool XMODEM_EOT_SENT=false;
		bool XMODEM_USE_CRC=false;
		std::string hostRecvFName;

		void ClearXMODEM(void);
		void SetUpXMODEMtoVM(const std::vector <unsigned char> &data,uint32_t packetLength);
		void SetUpXMODEMfromVM(std::string hostRecvFName);
		void SetUpXMODEMCRCfromVM(std::string hostRecvFName);

		void XMODEM_TO_VM_TransferNextBlock(uint32_t packetLength);
		unsigned int XMODEM_CRC(unsigned char ptr[],unsigned int len);

		virtual bool TxRDY(void);
		virtual void Tx(unsigned char data);
		virtual void SetStopBits(unsigned char stopBits);
		virtual void SetParity(bool enabled,bool evenParity);
		virtual void SetDataLength(unsigned char bitsPerData);
		virtual void SetBaudRate(unsigned int baudRate);
		virtual void Command(bool RTS,bool DTR,bool BREAK);
		virtual bool RxRDY(void);
		virtual unsigned char Rx(void);
		virtual bool DSR(void);
	};

	State state;
	DefaultClient defaultClient;
	i8251toSocketClient socketClient;

	TownsSerialPort(class FMTownsCommon *townsPtr);

	bool ConnectSocketClient(std::string serverAddr);
	void DisconnectSocketClient(void);

	void UpdatePIC(void);

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void RunScheduledTask(unsigned long long int townsTime);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};

/* } */
#endif
