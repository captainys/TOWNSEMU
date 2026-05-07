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


RatocREX3586::RatocREX3586(class FMTownsCommon *ptr) : Device(ptr)
{
}

void RatocREX3586::PowerOn(void)
{
	Reset();
}

void RatocREX3586::Reset(void)
{
	state.ROMReadPtr=0;
	for(auto &row : state.regs)
	{
		for(auto &col : row)
		{
			col=0xFF;
		}
	}
}

void RatocREX3586::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_LAN_REX3586_TX_STATUS: //	0x7000,
		break;
	case TOWNSIO_LAN_REX3586_RX_STATUS: //	0x7001,
		break;
	case TOWNSIO_LAN_REX3586_TX_INTEN: //	0x7002,
		break;
	case TOWNSIO_LAN_REX3586_RX_INTEN: //	0x7003,
		break;
	case TOWNSIO_LAN_REX3586_TX_MODE: //	0x7004,
		break;
	case TOWNSIO_LAN_REX3586_RX_MODE: //	0x7005,
		break;
	case TOWNSIO_LAN_REX3586_CONFIG0: //	0x7006,
		break;
	case TOWNSIO_LAN_REX3586_CONFIG1: //	0x7007,
		break;
	// Reg Bank 2
	case TOWNSIO_LAN_REX3586_BUFFMEMPORT_L: //0x7008,
		break;
	case TOWNSIO_LAN_REX3586_BUFFMEMPORT_H: //0x7009,
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

	case TOWNSIO_LAN_REX3586_ROM: //        0x7010,
		if(0==data&0xF0)
		{
			state.ROMReadPtr=0;
		}
		break;
	}
}

unsigned int RatocREX3586::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_LAN_REX3586_TX_STATUS: //	0x7000,
		break;
	case TOWNSIO_LAN_REX3586_RX_STATUS: //	0x7001,
		break;
	case TOWNSIO_LAN_REX3586_TX_INTEN: //	0x7002,
		break;
	case TOWNSIO_LAN_REX3586_RX_INTEN: //	0x7003,
		break;
	case TOWNSIO_LAN_REX3586_TX_MODE: //	0x7004,
		break;
	case TOWNSIO_LAN_REX3586_RX_MODE: //	0x7005,
		break;
	case TOWNSIO_LAN_REX3586_CONFIG0: //	0x7006,
		break;
	case TOWNSIO_LAN_REX3586_CONFIG1: //	0x7007,
		break;
	// Reg Bank 2
	case TOWNSIO_LAN_REX3586_BUFFMEMPORT_L: //0x7008,
		break;
	case TOWNSIO_LAN_REX3586_BUFFMEMPORT_H: //0x7009,
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

	case TOWNSIO_LAN_REX3586_ROM: //        0x7010,
		break;
	}
	return 0xFF;
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
