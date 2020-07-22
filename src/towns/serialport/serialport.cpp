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
void TownsSerialPort::DefaultClient::SetUpXMODEMtoVM(const std::vector <unsigned char> &data)
{
	fileTfrMode=FILETFR_XMODEM_TO_VM;
	fileTfrPtr=0;
	fileTfrData=data;
	XMODEM_EOT_SENT=false;
	toVM.clear();
}
void TownsSerialPort::DefaultClient::SetUpXMODEMfromVM(std::string hostRecvFName)
{
	fileTfrMode=FILETFR_XMODEM_FROM_VM;
	fileTfrData.clear();
	fromVM.clear();
	toVMPtr=toVM.size();
	toVM.push_back(CODE_NAK);
	this->hostRecvFName=hostRecvFName;
}
/* virtual */ bool TownsSerialPort::DefaultClient::TxRDY(void)
{
	// Always ready to accept a data.
	return true;
}
/* virtual */ void TownsSerialPort::DefaultClient::Tx(unsigned char data)
{
	// It is Tx from the VM point of view.  It is Rx from the client point of view.
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
		// std::cout << "Tx from VM:" << cpputil::Ubtox(data) << std::endl;
		if(CODE_NAK==data || CODE_ACK==data)
		{
			if(CODE_CAN==data)
			{
				// Make it available for the next attempt.
				fileTfrPtr=0;
				XMODEM_EOT_SENT=false;
			}
			else if(CODE_NAK==data && 128<=fileTfrPtr)
			{
				fileTfrPtr-=128;
			}
			if(CODE_ACK==data && fileTfrData.size()<=fileTfrPtr)
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
				unsigned int blk=((fileTfrPtr/128)+1)&255;
				toVMPtr=toVM.size();
				toVM.push_back(CODE_SOH);
				toVM.push_back(blk);
				toVM.push_back(~blk);
				unsigned int checkSum=0;
				for(unsigned int i=0; i<128; ++i)
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
				fileTfrPtr+=128;
				toVM.push_back(checkSum&0xFF);
			}
		}
		break;
	case FILETFR_XMODEM_FROM_VM:
		if(0==fromVM.size())
		{
			if(CODE_SOH==data)
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
			fromVM.push_back(data);
			if(132==fromVM.size())
			{
				unsigned int sum=0;
				for(int i=3; i<3+128; ++i)
				{
					sum+=fromVM[i];
				}
				if(fromVM[1]!=(255&(~fromVM[2])) || fromVM[131]!=(sum&255))
				{
					toVM.push_back(CODE_NAK);
				}
				else
				{
					for(int i=3; i<3+128; ++i)
					{
						fileTfrData.push_back(fromVM[i]);
					}
					toVM.push_back(CODE_ACK);
				}
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



TownsSerialPort::TownsSerialPort(class FMTowns *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
	state.intel8251.clientPtr=&defaultClient;
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

