/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>

#include "device.h"
#include "townsdef.h"
#include "towns.h"
#include "cpputil.h"
#include "dmac.h"



bool TownsDMAC::State::Channel::AUTI(void) const
{
	return 0!=(modeCtrl&0x10);
}


////////////////////////////////////////////////////////////


void TownsDMAC::State::PowerOn(void)
{
	Reset();
}
void TownsDMAC::State::Reset(void)
{
	for(auto &c : ch)
	{
		c.baseCount=0;
		c.currentCount=0;
		c.baseAddr=0;
		c.currentAddr=0;
		c.modeCtrl=0;
	}

	bitSize=16;
	BASE=true;
	SELCH=0;
	devCtrl[0]=0;
	devCtrl[1]=0;
	temporaryReg[0]=0;
	temporaryReg[1]=0;
	req=0;
	mask=0;
}


////////////////////////////////////////////////////////////

TownsDMAC::TownsDMAC(class FMTowns *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
	debugBreakOnDMACRequest=false;
}

/* virtual */ void TownsDMAC::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsDMAC::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsDMAC::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_DMAC_INITIALIZE://          0xA0,
		if(0!=(data&1))
		{
			state.Reset();
		}
		if(0!=(data&2))
		{
			state.bitSize=16;
		}
		else
		{
			state.bitSize=8;
		}
		break;
	case TOWNSIO_DMAC_CHANNEL://             0xA1,
		state.BASE=(0!=(data&4));
		state.SELCH=data&3;
		break;
	case TOWNSIO_DMAC_COUNT_LOW://           0xA2,
		state.ch[state.SELCH].currentCount&=0xff00;
		state.ch[state.SELCH].currentCount|=(data&0xff);
		if(true!=state.BASE)
		{
			state.ch[state.SELCH].baseCount=state.ch[state.SELCH].currentCount;
		}
		break;
	case TOWNSIO_DMAC_COUNT_HIGH://          0xA3,
		state.ch[state.SELCH].currentCount&=0xff;
		state.ch[state.SELCH].currentCount|=((data&0xff)<<8);
		if(true!=state.BASE)
		{
			state.ch[state.SELCH].baseCount=state.ch[state.SELCH].currentCount;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_LOWEST://      0xA4,
		state.ch[state.SELCH].currentAddr&=0xffffff00;
		state.ch[state.SELCH].currentAddr|=(data&0xff);
		if(true!=state.BASE)
		{
			state.ch[state.SELCH].baseAddr=state.ch[state.SELCH].currentAddr;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_MIDLOW://      0xA5,
		state.ch[state.SELCH].currentAddr&=0xffff00ff;
		state.ch[state.SELCH].currentAddr|=((data&0xff)<<8);
		if(true!=state.BASE)
		{
			state.ch[state.SELCH].baseAddr=state.ch[state.SELCH].currentAddr;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_MIDHIGH://     0xA6,
		state.ch[state.SELCH].currentAddr&=0xff00ffff;
		state.ch[state.SELCH].currentAddr|=((data&0xff)<<16);
		if(true!=state.BASE)
		{
			state.ch[state.SELCH].baseAddr=state.ch[state.SELCH].currentAddr;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_HIGHEST://     0xA7,
		state.ch[state.SELCH].currentAddr&=0x00ffffff;
		state.ch[state.SELCH].currentAddr|=((data&0xff)<<24);
		if(true!=state.BASE)
		{
			state.ch[state.SELCH].baseAddr=state.ch[state.SELCH].currentAddr;
		}
		break;
	case TOWNSIO_DMAC_DEVICE_CONTROL_LOW://  0xA8,
		data&=0x08;
		data|=DEVICE_CONTROL_LOW_FIXED;
		state.devCtrl[0]=data;
		break;
	case TOWNSIO_DMAC_DEVICE_CONTROL_HIGH:// 0xA9,
		state.devCtrl[1]=data;
		break;
	case TOWNSIO_DMAC_MODE_CONTROL://        0xAA,
		state.ch[state.SELCH].modeCtrl=data;
		if(state.ch[state.SELCH].modeCtrl&0x20)
		{
			townsPtr->cpu.Abort("DMAC: ADIR bit not supported.");
		}
		break;
	case TOWNSIO_DMAC_STATUS://              0xAB,
		break;
	case TOWNSIO_DMAC_TEMPORARY_REG_LOW://   0xAC,
		break;
	case TOWNSIO_DMAC_TEMPORARY_REG_HIGH://  0xAD,
		break;
	case TOWNSIO_DMAC_REQUEST://             0xAE,
		state.req=data;
		if(true==debugBreakOnDMACRequest)
		{
			townsPtr->debugger.ExternalBreak("DMAC Received a Request");
		}
		break;
	case TOWNSIO_DMAC_MASK://                0xAF,
		state.mask=data;
		break;
	}
}
/* virtual */ unsigned int TownsDMAC::IOReadByte(unsigned int ioport)
{
	unsigned char data=0;
	switch(ioport)
	{
	case TOWNSIO_DMAC_INITIALIZE://          0xA0,
		break;
	case TOWNSIO_DMAC_CHANNEL://             0xA1,
		return (1<<state.SELCH)|(state.BASE ? 0x10 : 0);
	case TOWNSIO_DMAC_COUNT_LOW://           0xA2,
		{
			auto &CH=state.ch[state.SELCH];
			auto value=(true==state.BASE ? CH.baseCount : CH.currentCount);
			return value&0xFF;
		}
		break;
	case TOWNSIO_DMAC_COUNT_HIGH://          0xA3,
		{
			auto &CH=state.ch[state.SELCH];
			auto value=(true==state.BASE ? CH.baseCount : CH.currentCount);
			return (value>>8)&0xFF;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_LOWEST://      0xA4,
		{
			auto &CH=state.ch[state.SELCH];
			auto value=(true==state.BASE ? CH.baseAddr : CH.currentAddr);
			return value&0xFF;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_MIDLOW://      0xA5,
		{
			auto &CH=state.ch[state.SELCH];
			auto value=(true==state.BASE ? CH.baseAddr : CH.currentAddr);
			return (value>>8)&0xFF;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_MIDHIGH://     0xA6,
		{
			auto &CH=state.ch[state.SELCH];
			auto value=(true==state.BASE ? CH.baseAddr : CH.currentAddr);
			return (value>>16)&0xFF;
		}
		break;
	case TOWNSIO_DMAC_ADDRESS_HIGHEST://     0xA7,
		{
			auto &CH=state.ch[state.SELCH];
			auto value=(true==state.BASE ? CH.baseAddr : CH.currentAddr);
			return (value>>24)&0xFF;
		}
		break;
	case TOWNSIO_DMAC_DEVICE_CONTROL_LOW://  0xA8,
		break;
	case TOWNSIO_DMAC_DEVICE_CONTROL_HIGH:// 0xA9,
		break;
	case TOWNSIO_DMAC_MODE_CONTROL://        0xAA,
		break;
	case TOWNSIO_DMAC_STATUS://              0xAB,
		break;
	case TOWNSIO_DMAC_TEMPORARY_REG_LOW://   0xAC,
		break;
	case TOWNSIO_DMAC_TEMPORARY_REG_HIGH://  0xAD,
		break;
	case TOWNSIO_DMAC_REQUEST://             0xAE,
		break;
	case TOWNSIO_DMAC_MASK://                0xAF,
		break;
	}
	return 0xff;
}

TownsDMAC::State::Channel *TownsDMAC::GetDMAChannel(unsigned int ch)
{
	ch&=3;
	if(0==(state.mask&(1<<ch)))
	{
		return &state.ch[ch];
	}
	return nullptr;
}
const TownsDMAC::State::Channel *TownsDMAC::GetDMAChannel(unsigned int ch) const
{
	ch&=3;
	if(0==(state.mask&(1<<ch)))
	{
		return &state.ch[ch];
	}
	return nullptr;
}

unsigned int TownsDMAC::DeviceToMemory(State::Channel *DMACh,const std::vector <unsigned char> &data)
{
	unsigned int i;
	auto &mem=townsPtr->mem;
	for(i=0; i<data.size() && 0<=DMACh->currentCount && DMACh->currentCount<=DMACh->baseCount; ++i)
	{
		mem.StoreByte(DMACh->currentAddr,data[i]);
		++DMACh->currentAddr;
		--DMACh->currentCount;
	}
	if(DMACh->currentCount+1==0 && true==DMACh->AUTI()) // :-(  Maybe I should use signed integer for counts.
	{
		DMACh->currentCount=DMACh->baseCount;
	}
	return i;
}
std::vector <unsigned char> TownsDMAC::MemoryToDevice(State::Channel *DMACh,unsigned int length)
{
	std::vector <unsigned char> data;
	unsigned int i;
	auto &mem=townsPtr->mem;
	data.resize(length);
	for(i=0; i<length && 0<=DMACh->currentCount && DMACh->currentCount<=DMACh->baseCount; ++i)
	{
		data[i]=mem.FetchByte(DMACh->currentAddr);
		++DMACh->currentAddr;
		--DMACh->currentCount;
	}
	data.resize(i);
	if(DMACh->currentCount+1==0 && true==DMACh->AUTI()) // :-(  Maybe I should use signed integer for counts.
	{
		DMACh->currentCount=DMACh->baseCount;
	}
	return data;
}

std::vector <std::string> TownsDMAC::GetStateText(void) const
{
	std::string line;
	std::vector <std::string> text;

	text.push_back(line);
	text.back()="DMAC";

	const std::string device[4]=
	{
		"FD    ",
		"SCSI  ",
		"PRN   ",
		"CD-ROM",
	};

	for(auto &c : state.ch)
	{
		unsigned int channelId=(unsigned int)(&c-state.ch);
		text.push_back(line);
		text.back()="CH"+cpputil::Ubtox(channelId)+"("+device[channelId]+"):";
		text.back()+=" MODE="+cpputil::Ubtox(c.modeCtrl);
		text.back()+=" BASEAD="+cpputil::Uitox(c.baseAddr);
		text.back()+=" CURRAD="+cpputil::Uitox(c.currentAddr);
		text.back()+=" BASECT="+cpputil::Uitox(c.baseCount);
		text.back()+=" CURRCT="+cpputil::Uitox(c.currentCount);
	}

	text.push_back(line);
	text.back()="TFRSIZE="+cpputil::Ubtox(state.bitSize/8);
	text.back()+=" BASE=";
	text.back()+=(state.BASE ? "1" : "0");
	text.back()+=" SELCH="+cpputil::Ubtox(state.SELCH);
	text.back()+=" DEVCTL="+cpputil::Ubtox(state.devCtrl[1])+cpputil::Ubtox(state.devCtrl[0]);
	text.back()+=" REQ="+cpputil::Ubtox(state.req);
	text.back()+=" MASK="+cpputil::Ubtox(state.mask);

	return text;
}

