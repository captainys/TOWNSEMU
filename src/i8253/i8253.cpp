#include <iostream>
#include "i8253.h"

void i8253::PowerOn(void)
{
	Reset();
}
void i8253::Reset(void)
{
	lastTickTimeInNS=0;
	for(auto &ch : channels)
	{
		ch.mode=0;
		ch.lastCmd=0;
		ch.counterInitialValue=0;
		ch.counter=0;
		ch.counting=false;
		ch.latchedCounter=0;
		ch.increment=1;
		ch.bcd=false;
		ch.OUT=false;
		ch.latched=false;

		ch.RL=1;
		ch.accessLow=true;
	}
}
void i8253::Latch(unsigned int ch)
{
	// i8254 data sheet tells that the first-latched value stays until it is unlatched by reading.
	// i8253 data sheet does not tell this way.  However it says:
	//   'it is mandatroy to complete the entire read operation as programmed.'
	//   pp.3-58 in "MODE Register for Latching Control."  Therefore, it probably is the same thing as i8254.
	auto &CH=channels[ch&7];
	if(true!=CH.latched)
	{
		CH.latched=true;
		CH.latchedCounter=CH.counter;
	}
}
unsigned short i8253::ReadLatchedCounter(unsigned int ch) const
{
	auto &CH=channels[ch&7];
	CH.latched=false;
	return CH.latchedCounter;
}
void i8253::SetChannelCounter(unsigned int ch,unsigned int value)
{
	auto &CH=channels[ch&7];
	if(true==CH.accessLow)
	{
		CH.counterInitialValue&=0xff00;
		CH.counterInitialValue|=(value&0xff);
		if(0==CH.mode)
		{
			// i8253 data sheet tells that "Rewriting a counter register during counting results
			// in the following: (1) Write 1st byte stops the counting. (2) Write 2nd byte starts the
			// new count."  However, it seems that it should start counting immediately after
			// writing to the lower byte if RL=1.
			// However, Page 3-57 also tells that "it must be loaded with the number of bytes
			// programmed in the MODE control word (RL0, ROl1)."  The implication is the counter is
			// 8 bit if RL=1, in which case it makes sense to start counting when LSB is written.
			if(1!=CH.RL)
			{
				CH.counting=false;
			}
			else
			{
				CH.counting=true;
			}
		}
	}
	else
	{
		CH.counterInitialValue&=0xff;
		CH.counterInitialValue|=((value&0xff)<<8);
		if(0==CH.mode)
		{
			CH.counting=true;
		}
	}
	switch(CH.mode)
	{
	default:
		std::cout << __FUNCTION__ << "At this time i8253 supports only modes 0 and 3" << std::endl;
		break;
	case 0:
		CH.OUT=false;
		break;
	case 3:
		CH.OUT=true;
		if(true==CH.accessLow)
		{
			CH.counterInitialValue&=~1; // Force it to be even number.
		}
		break;
	}
	CH.counter=CH.counterInitialValue;

	// Don't do it until the end.  CH.accessLow is checked inside switch(CH.mode).
	if(3==CH.RL)
	{
		CH.accessLow=(true==CH.accessLow ? false : true);
	}
}

void i8253::SetChannelMode(unsigned int ch,unsigned int mode)
{
	auto &CH=channels[ch&7];
	CH.mode=mode;
	if(0==mode)
	{
		CH.counting=false;
	}
	else
	{
		CH.counting=true;
	}
}

unsigned int i8253::ReadChannelCounter(unsigned int ch) // accessLow may flip.  Not to be const.
{
	unsigned int data=0;
	auto &CH=channels[ch&7];
	if(true==CH.latched)
	{
		if(true==CH.accessLow)
		{
			data=CH.latchedCounter&0xff;
		}
		else
		{
			data=(CH.latchedCounter>>8)&0xff;
			// i8254 data sheet tells it unlatches when the counter is read.
			// i8253 data sheet does not seem to say it.  But, presumably the same.
			// D-Return assumes this behavior.
			CH.latched=false;
		}
	}
	else
	{
		if(true==CH.accessLow)
		{
			data=CH.counter&0xff;
		}
		else
		{
			data=(CH.counter>>8)&0xff;
		}
	}
	if(3==CH.RL)
	{
		CH.accessLow=(true==CH.accessLow ? false : true);
	}
	return data;
}
void i8253::ProcessControlCommand(unsigned int cmd)
{
	auto ch=(cmd>>6)&3;
	if(ch!=3)
	{
		cmd&=0x3F;

		auto &CH=channels[ch&7];
		CH.lastCmd=cmd;
		CH.bcd=(0!=(cmd&1));

		SetChannelMode(ch,(cmd>>1)&7);

		auto RL=(cmd>>4)&3;
		if(0==RL)
		{
			if(3!=ch)
			{
				Latch(ch);
			}
		}
		else
		{
			CH.RL=RL;
			if(1==RL)
			{
				CH.accessLow=true;
			}
			else if(2==RL)
			{
				CH.accessLow=false;
			}
			else if(3==RL)
			{
				CH.accessLow=true;
			}
		}
	}
}
void i8253::TickIn(unsigned int nTick)
{
	for(auto &CH : channels)
	{
		// What to do when counterInitialValue==0?
		// [12] Source code of Artane's FM Towns emulator project https://github.com/Artanejp
		// It suggests to take it as 65536.  I follow Artane's implementation.
		if(true==CH.counting)
		{
			auto increment=nTick*CH.increment;
			switch(CH.mode)
			{
			default:
				std::cout << __FUNCTION__ << "At this time i8253 supports only modes 0 and 3" << std::endl;
				break;
			case 0:
				if(CH.counter<=increment)
				{
					CH.OUT=true;
					CH.counting=false;
					// Towns EUP player expect that the counter does not reset to the initial value.
					// Instead, it should stop couting in mode 0.
					// CH.counter=(0!=CH.counterInitialValue ? CH.counterInitialValue : 65535);
				}
				else
				{
					CH.counter-=increment;
				}
				break;
			case 3:
				increment*=2;
				if(CH.counter<=increment)
				{
					CH.OUT=(true==CH.OUT ? false : true);
					CH.counter=(0!=CH.counterInitialValue ? CH.counterInitialValue : 65535);
				}
				else
				{
					CH.counter-=increment;
				}
				break;
			}
		}
	}
}

void i8253::SerializeV0(std::vector <unsigned char> &data) const
{
	PushUint64(data,lastTickTimeInNS);
	for(auto &ch : channels)
	{
		PushUint16(data,ch.mode);
		PushUint16(data,ch.lastCmd);
		PushUint16(data,ch.counter);
		PushUint16(data,ch.counterInitialValue);
		PushUint16(data,ch.latchedCounter);
		PushUint16(data,ch.increment);
		PushBool(data,ch.OUT);
		PushBool(data,ch.counting);
		PushBool(data,ch.latched);
		PushBool(data,ch.bcd);

		PushUint32(data,ch.RL);
		PushBool(data,ch.accessLow);
	}
}
bool i8253::DeserializeV0(const unsigned char *&data)
{
	lastTickTimeInNS=ReadUint64(data);
	for(auto &ch : channels)
	{
		ch.mode=ReadUint16(data);
		ch.lastCmd=ReadUint16(data);
		ch.counter=ReadUint16(data);
		ch.counterInitialValue=ReadUint16(data);
		ch.latchedCounter=ReadUint16(data);
		ch.increment=ReadUint16(data);
		ch.OUT=ReadBool(data);
		ch.counting=ReadBool(data);
		ch.latched=ReadBool(data);
		ch.bcd=ReadBool(data);

		ch.RL=ReadUint32(data);
		ch.accessLow=ReadBool(data);
	}
	return true;
}
