/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <time.h>

#include "device.h"
#include "towns.h"
#include "townsdef.h"
#include "cpputil.h"
#include "rtc.h"



void TownsRTC::State::PowerOn(void)
{
	Reset();
}
void TownsRTC::State::Reset(void)
{
	state=STATE_NONE;
	hour24=false;
	registerLatch=0;
	lastDataWrite=0;
}


////////////////////////////////////////////////////////////

TownsRTC::TownsRTC(class FMTownsCommon *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
}

/* virtual */ void TownsRTC::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsRTC::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsRTC::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_RTC_DATA://                 0x70,
		state.lastDataWrite=data;
		break;
	case TOWNSIO_RTC_COMMAND://              0x80,
		if(STATE_NONE==state.state)
		{
			if(0x80==(data&0x80))
			{
				state.state=STATE_COMMAND;
			}
		}
		else if(STATE_COMMAND==state.state)
		{
			if(0x80!=(data&0x80))
			{
				state.state=STATE_NONE;
			}
			else if(0x81==data)
			{
				state.registerLatch=state.lastDataWrite;
			}
			else if(0x82==data)
			{
				// Write data.
				if(state.registerLatch==REG_10HOUR)
				{
					state.hour24=((state.lastDataWrite&0x10)==0x10);
				}
			}
			else if(0x84==data)
			{
				// Read data.
			}
		}
		break;
	}
}
/* virtual */ unsigned int TownsRTC::IOReadByte(unsigned int ioport)
{
	unsigned char data=0xff;
	switch(ioport)
	{
	case TOWNSIO_RTC_DATA://                 0x70,
		data=0x80;
		if(state.state!=STATE_NONE)
		{
			auto t=time(nullptr);
			auto tm=localtime(&t);
			switch(state.registerLatch)
			{
			case REG_1SEC://0
				data|=(tm->tm_sec%10);
				break;
			case REG_10SEC://1
				data|=(tm->tm_sec/10);
				break;
			case REG_MIN://2
				data|=(tm->tm_min%10);
				break;
			case REG_10MIN://3
				data|=(tm->tm_min/10);
				break;
			case REG_HOUR://4
				{
					int hour=tm->tm_hour;
					if(true!=state.hour24 && 12<hour)
					{
						hour-=12;
					}
					data=hour%10;
				}
				break;
			case REG_10HOUR://5 // Bit 2 is AM/PM
				data|=(state.hour24 ? 0x08 : 0);
				data|=(tm->tm_hour>=12 ? 0x04 : 0);
				{
					int hour=tm->tm_hour;
					if(true!=state.hour24 && 12<hour)
					{
						hour-=12;
					}
					if(REG_HOUR==state.registerLatch)
					{
						data|=(hour%10);
					}
					else if(REG_10HOUR==state.registerLatch)
					{
						data|=(hour/10);
					}
				}
				break;
			case REG_WKDAY://6  // 0 to 6
				data|=tm->tm_wday;
				break;
			case REG_1DAY://7
				data|=(tm->tm_mday%10);
				break;
			case REG_10DAY://8
				data|=(tm->tm_mday/10);
				break;
			case REG_MONTH://9
				data|=((tm->tm_mon+1)%10);
				break;
			case REG_10MONTH://0x0A
				data|=((tm->tm_mon+1)/10);
				break;
			case REG_YEAR://0x0B
				data|=(tm->tm_year%10);
				break;
			case REG_10YEAR://0x0C
				data|=(tm->tm_year/10);
				break;
			}
		}
		break;
	case TOWNSIO_RTC_COMMAND://              0x80,
		break;
	}
	return data;
}


/* virtual */ uint32_t TownsRTC::SerializeVersion(void) const
{
	return 0;
}
/* virtual */ void TownsRTC::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	PushUint32(data,state.state);
	PushBool(data,state.hour24); // If true, return 00:00 to 23:59 scale
	PushUint32(data,state.registerLatch);
	PushUint32(data,state.lastDataWrite);
}
/* virtual */ bool TownsRTC::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	state.state=ReadUint32(data);
	state.hour24=ReadBool(data); // If true, return 00:00 to 23:59 scale
	state.registerLatch=ReadUint32(data);
	state.lastDataWrite=ReadUint32(data);
	return true;
}
