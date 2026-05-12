/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */


#include "towns.h"
#include "rex3586.h"
#include "miscutil.h"
#include "real_network.h"

RatocREX3586::RatocREX3586(class FMTownsCommon *ptr) : Device(ptr)
{
	realNet=new RealNetwork;
}

RatocREX3586::~RatocREX3586()
{
	delete realNet;
}

void RatocREX3586::StartRealNetwork(void)
{
	realNet->Start();
}


void RatocREX3586::PowerOn(void)
{
	Reset();
}

void RatocREX3586::Reset(void)
{
	state.ROMReadPtr=0;
	state.RXIntEN=false;
	state.TXIntEN=false;
	for(auto &row : state.regs)
	{
		for(auto &col : row)
		{
			col=0xFF;
		}
	}
}

void RatocREX3586::RealPolling(void)
{
	
}

void RatocREX3586::UpdatePIC(void)
{
	bool irr=(true==state.RXIntEN && 0!=state.RXPacket.size()); // What about TX?  What can trigger TX INT?
	FMTownsCommon *towns=(FMTownsCommon *)vmPtr;
	towns->pic.SetInterruptRequestBit(state.INTNum,irr);
}

void RatocREX3586::ReceivePacket(size_t len,const uint8_t data[])
{
	state.RXPacket.push_back(0x20); // Linux at1700.c requires the (first_byte&0xF0)==0x20.  PD3586.COM assumes error if b0 is set.
	state.RXPacket.push_back(0);    // Meaning unknown

	state.RXPacket.push_back(len);
	state.RXPacket.push_back(len>>8);

	state.RXPacket.insert(state.RXPacket.end(),data,data+len);
	UpdatePIC();

	if(true==var.monitorRxPacket)
	{
		std::cout << "RX: " << state.RXPacket.size() << "\n";
		for(auto str : miscutil::MakeDump(state.RXPacket.size(),state.RXPacket.data()))
		{
			std::cout << str << "\n";
		}
	}
}

void RatocREX3586::IOWriteByte(unsigned int ioport,unsigned int data)
{
	state.regs[state.GetCurrentRegisterBank()][ioport-TOWNSIO_LAN_REX3586_TX_STATUS]=data;

	switch(ioport)
	{
	case TOWNSIO_LAN_REX3586_TX_STATUS: //	0x7000,
		if(0x82==(data&0x82))
		{
			// Looks like it clears TX status.
			// state.TXPacket.clear();
			UpdatePIC();
		}
		break;
	case TOWNSIO_LAN_REX3586_RX_STATUS: //	0x7001,
		if(0x81==(data&0x81))
		{
			// Looks like it clears RX status.
			// state.RXPacket.clear();  But doesn't look to clear the data.
			UpdatePIC();
		}
		break;
	case TOWNSIO_LAN_REX3586_TX_INTEN: //	0x7002,
		state.TXIntEN=(0x82==(data&0x82));
		UpdatePIC();
		break;
	case TOWNSIO_LAN_REX3586_RX_INTEN: //	0x7003,
		state.RXIntEN=(0x81==(data&0x81));
		UpdatePIC();
		break;
	case TOWNSIO_LAN_REX3586_TX_MODE: //	0x7004,
		break;
	case TOWNSIO_LAN_REX3586_RX_MODE: //	0x7005,
		break;
	case TOWNSIO_LAN_REX3586_CONFIG0: //	0x7006,
		state.config[0]=data;
		break;
	case TOWNSIO_LAN_REX3586_CONFIG1: //	0x7007,
		state.config[1]=data;
		// Bits 2 and 3 looks to be the register bank.
		break;
	case TOWNSIO_LAN_REX3586_ROM: //        0x7010,
		if(0==(data&0x0F))
		{
			state.ROMReadPtr=0;
		}

		// According to Linux FM TOWNS REX 3586 driver, high-4 bits control the IRQ.
		switch(data&0x10)
		{
		case 0x10:
			state.INTNum=4;
			break;
		case 0x20:
			state.INTNum=5;
			break;
		case 0x40:
			state.INTNum=10;
			break;
		case 0x80:
			state.INTNum=14;
			break;
		}
		break;
	}

	if(2==state.GetCurrentRegisterBank())
	{
		switch(ioport)
		{
		// Reg Bank 2
		case TOWNSIO_LAN_REX3586_BUFFMEMPORT_L: //0x7008,
			state.TXPacket.push_back(data);
			break;
		case TOWNSIO_LAN_REX3586_BUFFMEMPORT_H: //0x7009,
			state.TXPacket.push_back(data);
			break;
		case TOWNSIO_LAN_REX3586_TX_START: //	0x700A,
			if(0x81==(data&0x81)) // Apparently it is the trigger.
			{
				if(true==var.monitorTxPacket)
				{
					for(auto str : miscutil::MakeDump(state.TXPacket.size(),state.TXPacket.data()))
					{
						std::cout << str << "\n";
					}
				}
				net.TransmitPacket(state.TXPacket.size(),state.TXPacket.data(),this);
				state.TXPacket.clear();
				UpdatePIC();
			}
			break;
		case TOWNSIO_LAN_REX3586_16COLL: //		0x700B,
			break;
		case TOWNSIO_LAN_REX3586_DMAEN: //		0x700C,
			break;
		case TOWNSIO_LAN_REX3586_DMABURST: //	0x700D,
			break;
		case TOWNSIO_LAN_REX3586_SELF_RX: //	0x700E,
			break;
		case TOWNSIO_LAN_REX3586_TRCV_STATUS: //0x700F,
			break;
		}
	}
}

unsigned int RatocREX3586::IOReadByte(unsigned int ioport)
{
	unsigned int data=state.regs[state.GetCurrentRegisterBank()][ioport-TOWNSIO_LAN_REX3586_TX_STATUS];
	switch(ioport)
	{
	case TOWNSIO_LAN_REX3586_TX_STATUS: //	0x7000,
		break;
	case TOWNSIO_LAN_REX3586_RX_STATUS: //	0x7001,
		if(0==state.RXPacket.size())
		{
			data=0;
		}
		else
		{
			data=0xFF;
		}
		break;
	case TOWNSIO_LAN_REX3586_TX_INTEN: //	0x7002,
		break;
	case TOWNSIO_LAN_REX3586_RX_INTEN: //	0x7003,
		break;
	case TOWNSIO_LAN_REX3586_TX_MODE: //	0x7004,
		break;
	case TOWNSIO_LAN_REX3586_RX_MODE: //	0x7005,
		// RD3586.COM reads data when RX_MODE&0x40==0
		// 4700:00000D57 A840                      TEST    AL,40H
		// 4700:00000D59 740B                      JE      00000D66(Go on to read)
		// at1700.c reads data in the following while loop
		// while ((inb(ioaddr + RX_MODE) & 0x40) == 0)
		// Bit 6 needs to be zero when there is data.
		if(0==state.RXPacket.size())
		{
			data|=0x40;
		}
		else
		{
			data&=~0x40;
		}
		break;
	case TOWNSIO_LAN_REX3586_CONFIG0: //	0x7006,
		data=state.config[0];
		break;
	case TOWNSIO_LAN_REX3586_CONFIG1: //	0x7007,
		data=state.config[1];
		break;
	case TOWNSIO_LAN_REX3586_ROM: //        0x7010,
		// First 4 bits: 0
		if(state.ROMReadPtr<4)
		{
			data=0;
		}
		// Next 16 bits:  0xFFFF
		else if(state.ROMReadPtr<20)
		{
			data=1;
		}
		// Next 48 bits: 0x?????????? (Mac address)
		else if(state.ROMReadPtr<68)
		{
			auto bit=state.ROMReadPtr-20;
			data=(state.MAC>>bit)&1;
		}
		// Next  8 bits: 0xA2
		else if(state.ROMReadPtr<76)
		{
			auto bit=state.ROMReadPtr-68;
			unsigned short data=0xA2;
			data=(data>>bit)&1;
		}
		// Next 152 bits:  All 1
		else if(state.ROMReadPtr<232)
		{
			data=1;
		}
		// Next 24 bits: 0x4C414E ("LAN")
		else if(state.ROMReadPtr<252)
		{
			auto bit=state.ROMReadPtr-232;
			unsigned int data=0x4C414E;
			data=(data>>bit)&1;
		}
		// Last 4 bits: All 0
		else
		{
			data=0;
		}

		switch(state.INTNum)
		{
		case 4:
			data|=0x10;
			break;
		case 5:
			data|=0x20;
			break;
		case 10:
			data|=0x40;
			break;
		case 14:
			data|=0x80;
			break;
		}

		++state.ROMReadPtr;
		state.ROMReadPtr&=0xFF;
		break;
	}
	if(2==state.GetCurrentRegisterBank())
	{
		switch(ioport)
		{
		// Reg Bank 2
		case TOWNSIO_LAN_REX3586_BUFFMEMPORT_L: //0x7008,
			if(0<state.RXPacket.size())
			{
				data=state.RXPacket[0];
				state.RXPacket.erase(state.RXPacket.begin());
			}
			else
			{
				data=0;
			}
			break;
		case TOWNSIO_LAN_REX3586_BUFFMEMPORT_H: //0x7009,
			if(0<state.RXPacket.size())
			{
				data=state.RXPacket[0];
				state.RXPacket.erase(state.RXPacket.begin());
			}
			else
			{
				data=0;
			}
			break;
		case TOWNSIO_LAN_REX3586_TX_START: //	0x700A,
			break;
		case TOWNSIO_LAN_REX3586_16COLL: //		0x700B,
			break;
		case TOWNSIO_LAN_REX3586_DMAEN: //		0x700C,
			break;
		case TOWNSIO_LAN_REX3586_DMABURST: //	0x700D,
			break;
		case TOWNSIO_LAN_REX3586_SELF_RX: //	0x700E,
			break;
		case TOWNSIO_LAN_REX3586_TRCV_STATUS: //0x700F,
			break;
		}
	}
	return data;
}

unsigned int RatocREX3586::IOReadWord(unsigned int ioport)
{
	if(2==state.GetCurrentRegisterBank() && TOWNSIO_LAN_REX3586_BUFFMEMPORT_L==ioport)
	{
		if(2<=state.RXPacket.size())
		{
			uint16_t data=state.RXPacket[0]|(state.RXPacket[1]<<8);
			state.RXPacket.erase(state.RXPacket.begin());
			state.RXPacket.erase(state.RXPacket.begin());
			return data;
		}
		else if(1==state.RXPacket.size())
		{
			uint16_t data=state.RXPacket[0];
			state.RXPacket.erase(state.RXPacket.begin());
			return data;
		}
		else
		{
			return 0;
		}
	}
	return Device::IOReadWord(ioport);
}

uint32_t RatocREX3586::SerializeVersion(void) const
{
	return 0; // **** Not state-save target yet.  Uncomment townsstate.cpp in DeviceToLoad/SaveState when ready.
}

void RatocREX3586::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	 // **** Not state-save target yet.  Uncomment townsstate.cpp in DeviceToLoad/SaveState when ready.
}

bool RatocREX3586::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	 // **** Not state-save target yet.  Uncomment townsstate.cpp in DeviceToLoad/SaveState when ready.
	return true;
}
