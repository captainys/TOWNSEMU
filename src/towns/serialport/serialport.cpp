/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "serialport.h"
#include "townsdef.h"
#include "cpputil.h"

#include "i8251.h"



#include "towns.h"



void TownsSerialPort::DefaultClient::ClearXMODEM(void)
{
	fileTfrMode=FILETFR_NONE;
}
void TownsSerialPort::DefaultClient::SetUpXMODEMtoVM(const std::vector <unsigned char> &data,uint32_t packetLength)
{
	fileTfrMode=(128==packetLength ? FILETFR_XMODEM_TO_VM : FILETFR_XMODEM1024_TO_VM);
	fileTfrPtr=0;
	fileTfrData=data;
	while(0!=(fileTfrData.size()%packetLength))
	{
		fileTfrData.push_back(CODE_EOF);
	}
	XMODEM_EOT_SENT=false;
	toVM.clear();
}
void TownsSerialPort::DefaultClient::SetUpXMODEMfromVM(std::string hostRecvFName)
{
	fileTfrMode=FILETFR_XMODEM_FROM_VM;
	fileTfrData.clear();
	fromVM.clear();
	toVMPtr=toVM.size();
	XMODEM_USE_CRC=false;
	toVM.push_back(CODE_NAK);
	this->hostRecvFName=hostRecvFName;
}

void TownsSerialPort::DefaultClient::SetUpXMODEMCRCfromVM(std::string hostRecvFName)
{
	fileTfrMode=FILETFR_XMODEM_FROM_VM;
	fileTfrData.clear();
	fromVM.clear();
	toVMPtr=toVM.size();
	XMODEM_USE_CRC=true;
	toVM.push_back('C');
	this->hostRecvFName=hostRecvFName;
}

void TownsSerialPort::DefaultClient::XMODEM_TO_VM_TransferNextBlock(uint32_t packetLength)
{
	unsigned int blk=((fileTfrPtr/packetLength)+1)&255;
	toVMPtr=toVM.size();
	if(128==packetLength)
	{
		toVM.push_back(CODE_SOH);
	}
	else
	{
		toVM.push_back(CODE_STX);
	}
	toVM.push_back(blk);
	toVM.push_back(~blk);
	unsigned int checkSum=0;
	for(unsigned int i=0; i<packetLength; ++i)
	{
		if(fileTfrPtr+i<fileTfrData.size())
		{
			toVM.push_back(fileTfrData[fileTfrPtr+i]);
		}
		else
		{
			toVM.push_back(CODE_EOF);
		}
		checkSum+=toVM.back();
	}
	if(true==XMODEM_USE_CRC)
	{
		auto CRC=XMODEM_CRC(fileTfrData.data()+fileTfrPtr,packetLength);
		toVM.push_back((CRC>>8)&0xFF);
		toVM.push_back(CRC&0xFF);
	}
	else
	{
		toVM.push_back(checkSum&0xFF);
	}

	fileTfrPtr+=packetLength;
}

/* virtual */ bool TownsSerialPort::DefaultClient::TxRDY(void)
{
	// Always ready to accept a data.
	return true;
}
/* virtual */ void TownsSerialPort::DefaultClient::Tx(unsigned char data)
{
	uint32_t packetLength=128;

	// It is Tx from the VM point of view.  It is Rx from the host point of view.
	switch(fileTfrMode)
	{
	case FILETFR_NONE:
	default:
		// Just echo
		std::cout << "Tx from VM:" << cpputil::Ubtox(data) << std::endl;
		toVMPtr=toVM.size();
		toVM.push_back(data);
		if(0x0D==data)
		{
			toVM.push_back(0x0A);
		}
		break;
	case FILETFR_XMODEM_TO_VM:
	case FILETFR_XMODEM1024_TO_VM:
		packetLength=(FILETFR_XMODEM1024_TO_VM==fileTfrMode ? 1024 : 128);
		// std::cout << "Tx from VM:" << cpputil::Ubtox(data) << std::endl;
		if(0==fileTfrPtr && 'C'==data)
		{
			XMODEM_USE_CRC=true;
			XMODEM_TO_VM_TransferNextBlock(packetLength);
		}
		else if(0==fileTfrPtr && CODE_NAK==data)
		{
			XMODEM_USE_CRC=false;
			XMODEM_TO_VM_TransferNextBlock(packetLength);
		}
		else if(CODE_NAK==data || CODE_ACK==data || 'C'==data) // WINK2.EXP uses 'C' for the first two packets apparently.  Confirmed in WINK2 source XMODEM2.C line 463
		{
			if(CODE_CAN==data)
			{
				// Make it available for the next attempt.
				fileTfrPtr=0;
				XMODEM_EOT_SENT=false;
			}
			else if(CODE_NAK==data && packetLength<=fileTfrPtr)
			{
				fileTfrPtr-=packetLength;
				XMODEM_TO_VM_TransferNextBlock(packetLength);
			}
			else if(CODE_ACK==data || (true==XMODEM_USE_CRC && 'C'==data))
			{
				if(fileTfrData.size()<=fileTfrPtr)
				{
					if(true!=XMODEM_EOT_SENT)
					{
						// toVM.clear();
						toVM.push_back(CODE_EOT);
						XMODEM_EOT_SENT=true;
					}
					else
					{
						fileTfrMode=FILETFR_NONE;
					}
				}
				else
				{
					XMODEM_TO_VM_TransferNextBlock(packetLength);
				}
			}
		}
		break;
	case FILETFR_XMODEM_FROM_VM:
		if(0==fromVM.size())
		{
			if(CODE_SOH==data || CODE_STX==data)
			{
				fromVM.push_back(data);
			}
			else if(CODE_EOT==data)
			{
				toVM.push_back(CODE_ACK);
				fileTfrMode=FILETFR_NONE;
				if(true==cpputil::WriteBinaryFile(hostRecvFName,fileTfrData.size(),fileTfrData.data()))
				{
					std::cout << "Saved:" << hostRecvFName << std::endl;
				}
				else
				{
					std::cout << "!File Write Error!:" << hostRecvFName << std::endl;
				}
			}
		}
		else
		{
			const unsigned int packetSize=(CODE_SOH==fromVM[0] ? 128 : 1024);
			const unsigned int blockSize=(true==XMODEM_USE_CRC ? packetSize+5 : packetSize+4);
			fromVM.push_back(data);
			if(blockSize==fromVM.size())
			{
				if(true==XMODEM_USE_CRC)
				{
					auto CRC=XMODEM_CRC(fromVM.data()+3,packetSize);
					if(((CRC>>8)&0xFF)!=fromVM[blockSize-2] ||
					   ( CRC    &0xFF)!=fromVM[blockSize-1])
					{
						toVM.push_back(CODE_NAK);
						fromVM.clear();
						break;
					}
				}
				else
				{
					unsigned int sum=0;
					for(int i=3; i<3+packetSize; ++i)
					{
						sum+=fromVM[i];
					}
					if(fromVM[1]!=(255&(~fromVM[2])) || fromVM[blockSize-1]!=(sum&255))
					{
						toVM.push_back(CODE_NAK);
						fromVM.clear();
						break;
					}
				}

				for(int i=3; i<3+packetSize; ++i)
				{
					fileTfrData.push_back(fromVM[i]);
				}
				toVM.push_back(CODE_ACK);
				fromVM.clear();
			}
		}
		break;
	}
}
/* virtual */ void TownsSerialPort::DefaultClient::SetStopBits(unsigned char stopBits)
{
}
/* virtual */ void TownsSerialPort::DefaultClient::SetParity(bool enabled,bool evenParity)
{
}
/* virtual */ void TownsSerialPort::DefaultClient::SetDataLength(unsigned char bitsPerData)
{
}
/* virtual */ void TownsSerialPort::DefaultClient::SetBaudRate(unsigned int baudRate)
{
}
/* virtual */ void TownsSerialPort::DefaultClient::Command(bool RTS,bool DTR,bool BREAK)
{
}
/* virtual */ bool TownsSerialPort::DefaultClient::RxRDY(void)
{
	return (toVMPtr<toVM.size());
}
/* virtual */ unsigned char TownsSerialPort::DefaultClient::Rx(void)
{
	if(toVMPtr<toVM.size())
	{
		auto data=toVM[toVMPtr++];
		if(toVM.size()<=toVMPtr)
		{
			toVMPtr=0;
			toVM.clear();
		}
		return data;
	}
	return 0;
}
/* virtual */ bool TownsSerialPort::DefaultClient::DSR(void)
{
	return true; // Always ready
}

/* References:
     http://web.mit.edu/6.115/www/amulet/xmodem.htm
     http://mdfs.net/Info/Comp/Comms/CRC16.htm
*/
unsigned int TownsSerialPort::DefaultClient::XMODEM_CRC(unsigned char ptr[],unsigned int len)
{
	const unsigned int magicNumber=0x1021;
	unsigned int crc=0;
	for(unsigned int i=0; i<len; ++i)
	{
		unsigned int dat=ptr[i];
		dat<<=8;
		crc=crc^dat;
		for(unsigned int j=0; j<8; ++j)
		{
			if(crc&0x8000)
			{
				crc<<=1;
				crc^=magicNumber;
			}
			else
			{
				crc<<=1;
			}
		}
	}
	return crc;
}


////////////////////////////////////////////////////////////



void TownsSerialPort::State::Reset(void)
{
	intel8251.Reset();
	INTEnableBits=0;
}
void TownsSerialPort::State::PowerOn(void)
{
	intel8251.Reset();
	INTEnableBits=0;
}
void TownsSerialPort::State::UpdateINTState(void)
{
	INTbyTxRDY_RxRDY_SYNDET=
		(0!=(INTEnableBits&INTENABLE_TXRDY) && intel8251.TxRDY()) ||
		(0!=(INTEnableBits&INTENABLE_RXRDY) && intel8251.RxRDY()) ||
		(0!=(INTEnableBits&INTENABLE_SYNDET) && intel8251.SYNDET());
}



////////////////////////////////////////////////////////////



TownsSerialPort::TownsSerialPort(class FMTownsCommon *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
	state.intel8251.clientPtr=&defaultClient;
}
bool TownsSerialPort::ConnectSocketClient(std::string serverAddr)
{
	if(YSTRUE!=socketClient.IsConnected())
	{
		std::string ipAddr,portStr;
		bool colon=false;
		for(auto c : serverAddr)
		{
			if(':'==c)
			{
				colon=true;
			}
			else if(true!=colon)
			{
				ipAddr.push_back(c);
			}
			else
			{
				portStr.push_back(c);
			}
		}

		unsigned int port=cpputil::Atoi(portStr.c_str());

		if(YSOK==socketClient.Start(port) &&
		   YSTRUE==socketClient.Connect(ipAddr.c_str()))
		{
			state.intel8251.clientPtr=&socketClient;
			return true;
		}
	}
	return false;
}
void TownsSerialPort::DisconnectSocketClient(void)
{
	if(YSTRUE==socketClient.IsConnected())
	{
		socketClient.Disconnect();
		socketClient.Terminate();
		state.intel8251.clientPtr=&defaultClient;
	}
}
void TownsSerialPort::UpdatePIC(void)
{
	townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_RS232C,state.INTbyTxRDY_RxRDY_SYNDET);
}
/* virtual */ void TownsSerialPort::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsSerialPort::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsSerialPort::RunScheduledTask(unsigned long long int townsTime)
{
	state.intel8251.Update(townsTime);
	state.UpdateINTState();
	UpdatePIC();
	if(0!=(state.INTEnableBits&(INTENABLE_TXRDY|INTENABLE_RXRDY|INTENABLE_SYNDET)))
	{
		townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+state.intel8251.state.nanoSecondsPerByte);
	}
}
/* virtual */ void TownsSerialPort::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_RS232C_STATUS_COMMAND://=0xA02, // [2] pp.269
		state.intel8251.Update(townsPtr->state.townsTime);
		state.intel8251.VMWriteCommnand(data);
		UpdatePIC();
		break;
	case TOWNSIO_RS232C_DATA://=          0xA00, // [2] pp.274
		state.intel8251.VMWriteData(data,townsPtr->state.townsTime);
		state.UpdateINTState();
		UpdatePIC();
		break;
	case TOWNSIO_RS232C_INT_REASON:
		break;
	case TOWNSIO_RS232C_INT_CONTROL:
		state.INTEnableBits=data;
		state.UpdateINTState();
		UpdatePIC();
		if(0!=(data&(INTENABLE_TXRDY|INTENABLE_RXRDY|INTENABLE_SYNDET)))
		{
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+state.intel8251.state.nanoSecondsPerByte);
		}
		break;
	}
}
/* virtual */ unsigned int TownsSerialPort::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_RS232C_STATUS_COMMAND://=0xA02, // [2] pp.269
		{
			state.intel8251.Update(townsPtr->state.townsTime);
			auto data=state.intel8251.VMReadState();
			state.UpdateINTState();
			UpdatePIC();
			return data;
		}
		break;
	case TOWNSIO_RS232C_DATA://=          0xA00, // [2] pp.274
		return state.intel8251.VMReadData(townsPtr->state.townsTime);
		break;
	case TOWNSIO_RS232C_INT_REASON:
		break;
	case TOWNSIO_RS232C_INT_CONTROL:
		break;
	}
	return 0xff;
}


/* virtual */ uint32_t TownsSerialPort::SerializeVersion(void) const
{
	return 0;
}
/* virtual */ void TownsSerialPort::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	PushInt64(data,state.intel8251.state.lastTxTime);
	PushInt64(data,state.intel8251.state.lastRxTime);

	PushBool(data,state.intel8251.state.immediatelyAfterReset);

	PushUint32(data,state.intel8251.state.nanoSecondsPerByte);

	PushUint32(data,state.intel8251.state.baudRate);

	PushUint16(data,state.intel8251.state.stopBits);
	PushUint16(data,state.intel8251.state.preScale);

	PushBool(data,state.intel8251.state.SCS);
	PushBool(data,state.intel8251.state.ESD);

	PushUint16(data,state.intel8251.state.dataLength);
	PushBool(data,state.intel8251.state.evenParity);
	PushBool(data,state.intel8251.state.parityEnabled);

	PushBool(data,state.intel8251.state.RxEN);
	PushBool(data,state.intel8251.state.TxEN);
	PushBool(data,state.intel8251.state.RxRDY);
	PushBool(data,state.intel8251.state.TxRDY);
	PushBool(data,state.intel8251.state.SYNDET);
	PushBool(data,state.intel8251.state.FE);
	PushBool(data,state.intel8251.state.OE);
	PushBool(data,state.intel8251.state.PE);
	PushBool(data,state.intel8251.state.TxEMPTY);
	PushBool(data,state.intel8251.state.RTS);
	PushBool(data,state.intel8251.state.DTR);
	PushBool(data,state.intel8251.state.BREAK);

	PushUint16(data,state.INTEnableBits);
	PushBool(data,state.INTbyTxRDY_RxRDY_SYNDET);
}
/* virtual */ bool TownsSerialPort::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	state.intel8251.state.lastTxTime=ReadInt64(data);
	state.intel8251.state.lastRxTime=ReadInt64(data);

	state.intel8251.state.immediatelyAfterReset=ReadBool(data);

	state.intel8251.state.nanoSecondsPerByte=ReadUint32(data);

	state.intel8251.state.baudRate=ReadUint32(data);

	state.intel8251.state.stopBits=ReadUint16(data);
	state.intel8251.state.preScale=ReadUint16(data);

	state.intel8251.state.SCS=ReadBool(data);
	state.intel8251.state.ESD=ReadBool(data);

	state.intel8251.state.dataLength=ReadUint16(data);
	state.intel8251.state.evenParity=ReadBool(data);
	state.intel8251.state.parityEnabled=ReadBool(data);

	state.intel8251.state.RxEN=ReadBool(data);
	state.intel8251.state.TxEN=ReadBool(data);
	state.intel8251.state.RxRDY=ReadBool(data);
	state.intel8251.state.TxRDY=ReadBool(data);
	state.intel8251.state.SYNDET=ReadBool(data);
	state.intel8251.state.FE=ReadBool(data);
	state.intel8251.state.OE=ReadBool(data);
	state.intel8251.state.PE=ReadBool(data);
	state.intel8251.state.TxEMPTY=ReadBool(data);
	state.intel8251.state.RTS=ReadBool(data);
	state.intel8251.state.DTR=ReadBool(data);
	state.intel8251.state.BREAK=ReadBool(data);

	state.INTEnableBits=ReadUint16(data);
	state.INTbyTxRDY_RxRDY_SYNDET=ReadBool(data);

	return true;
}
