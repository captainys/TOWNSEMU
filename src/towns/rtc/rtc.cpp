#include <time.h>

#include "device.h"
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
			case REG_10HOUR://5 // Bit 2 is AM/PM
				data|=(state.hour24 ? 0x08 : 0);
				data|=(tm->tm_hour>=12 ? 0x04 : 0);
				{
					int hour;
					if(state.hour24)
					{
						hour=tm->tm_hour;
					}
					else
					{
						hour=tm->tm_hour%12;
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
				data|=(tm->tm_mon%10);
				break;
			case REG_10MONTH://0x0A
				data|=(tm->tm_mon/10);
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

