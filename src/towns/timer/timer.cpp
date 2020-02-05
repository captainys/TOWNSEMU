#include "timer.h"
#include "towns.h"
#include "pic.h"
#include "cpputil.h"



void TownsTimer::State::PowerOn(void)
{
	Reset();
}
void TownsTimer::State::Reset(void)
{
	lastTickTimeInNS=0;
	for(auto &ch : channels)
	{
		ch.mode=0;
		ch.lastCmd=0;
		ch.counterInitialValue=0;
		ch.counter=0;
		ch.latchedCounter=0;
		ch.increment=1;
		ch.bcd=false;
		ch.OUT=false;
		ch.latched=false;
	}
	for(auto &b : TMMSK)
	{
		b=false;
	}
	for(auto &b : TMOUT)
	{
		b=false;
	}
	SOUND=true;
}
void TownsTimer::State::Latch(unsigned int ch)
{
	// i8254 data sheet tells that the first-latched value stays until it is unlatched by reading.
	auto &CH=channels[ch&7];
	if(true!=CH.latched)
	{
		CH.latched=true;
		CH.latchedCounter=CH.counter;
	}
}
unsigned short TownsTimer::State::ReadLatchedCounter(unsigned int ch) const
{
	auto &CH=channels[ch&7];
	CH.latched=false;
	return CH.latchedCounter;
}
void TownsTimer::State::SetChannelCounterLow(unsigned int ch,unsigned int value)
{
	auto &CH=channels[ch&7];
	CH.counterInitialValue&=0xff00;
	CH.counterInitialValue|=(value&0xff);
	switch(CH.mode)
	{
	case 0:
		CH.OUT=false;
		break;
	case 3:
		CH.OUT=true;
		CH.counterInitialValue+=(value&1); // Force it to be even number.
		break;
	}
	CH.counter=CH.counterInitialValue;
}
void TownsTimer::State::SetChannelCounterHigh(unsigned int ch,unsigned int value)
{
	auto &CH=channels[ch&7];
	CH.counterInitialValue&=0x00ff;
	CH.counterInitialValue|=((value&0xff)<<8);
	CH.counter=CH.counterInitialValue;
	switch(CH.mode)
	{
	case 0:
		CH.OUT=false;
		break;
	case 3:
		CH.OUT=true;
		break;
	}
}
void TownsTimer::State::ProcessControlCommand(unsigned int ch,unsigned int cmd)
{
	auto &CH=channels[ch&7];
	CH.lastCmd=cmd;
	CH.bcd=(0!=(cmd&1));
}
void TownsTimer::State::TickIn(unsigned int nTick)
{
	for(int ch=0; ch<NUM_CHANNELS_ACTUAL; ++ch)
	{
		auto &CH=channels[ch];
		auto increment=nTick*CH.increment;
		switch(CH.mode)
		{
		case 0:
			if(CH.counter<=increment)
			{
				CH.OUT=true;
				CH.counter=CH.counterInitialValue;
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
				CH.counter=CH.counterInitialValue;
			}
			else
			{
				CH.counter-=increment;
			}
			break;
		}
	}
}


////////////////////////////////////////////////////////////


/* virtual */ void TownsTimer::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsTimer::Reset(void)
{
	state.Reset();

	// [2] pp. 77
	//   In FM Towns Channel 0,2,3 is set to Mode 3, and Channel 1 Mode 0.
	//   My guess is it is meant Channel 0,2,"4" are in Mode 3.  Channel 3 is not supposed to be used.
	//   Confirmed MOD=3 is written to channel 0.
	//   How come?  My only guess is that FM Towns is not directly using OUT signal from i8253.
	//   Instead it probably uses rising edge of OUT to flip TM1OUT/SOUND to 1.
	state.channels[0].mode=3;
	state.channels[1].mode=0;
	state.channels[2].mode=3;
	state.channels[3].mode=3;
	state.channels[4].mode=3;
	state.channels[5].mode=3;

	state.channels[4].increment=4;  // Only channel 4 counts up 1.2288MHz.  Others 307.2KHz.
}
/* virtual */ void TownsTimer::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_TIMER0_COUNT_LOW://         0x40,
		state.SetChannelCounterLow(0,data);
		break;
	case TOWNSIO_TIMER0_COUNT_HIGH://        0x41,
		state.SetChannelCounterHigh(0,data);
		break;
	case TOWNSIO_TIMER1_COUNT_LOW://         0x42,
		state.SetChannelCounterLow(1,data);
		break;
	case TOWNSIO_TIMER1_COUNT_HIGH://        0x43,
		state.SetChannelCounterLow(1,data);
		break;
	case TOWNSIO_TIMER2_COUNT_LOW://         0x44,
		state.SetChannelCounterLow(2,data);
		break;
	case TOWNSIO_TIMER2_COUNT_HIGH://        0x45,
		state.SetChannelCounterLow(2,data);
		break;
	case TOWNSIO_TIMER_0_1_2_CTRL://         0x46,
		{
			unsigned ch=((data>>6)&3);
			state.ProcessControlCommand(ch,data&0x3f);
		}
		break;
	case TOWNSIO_TIMER3_COUNT_LOW://         0x50,
		state.SetChannelCounterLow(3,data);
		break;
	case TOWNSIO_TIMER3_COUNT_HIGH://        0x51,
		state.SetChannelCounterLow(3,data);
		break;
	case TOWNSIO_TIMER4_COUNT_LOW://         0x52,
		state.SetChannelCounterLow(4,data);
		break;
	case TOWNSIO_TIMER4_COUNT_HIGH://        0x53,
		state.SetChannelCounterLow(4,data);
		break;
	case TOWNSIO_TIMER5_COUNT_LOW://         0x54,
		state.SetChannelCounterLow(5,data);
		break;
	case TOWNSIO_TIMER5_COUNT_HIGH://        0x55,
		state.SetChannelCounterLow(5,data);
		break;
	case TOWNSIO_TIMER_3_4_5_CTRL://         0x56,
		{
			unsigned ch=3+((data>>6)&3);
			state.ProcessControlCommand(ch,data&0x3f);
		}
		break;
	case TOWNSIO_TIMER_INT_CTRL_INT_REASON://0x60,
		state.TMMSK[0]=(0!=(data&1));
		state.TMMSK[1]=(0!=(data&2));
		state.SOUND=(0!=(data&4));
		if(0!=(data&0x80))
		{
			state.TMOUT[0]=false;
		}
		break;
	}
}
/* virtual */ unsigned int TownsTimer::IOReadByte(unsigned int ioport)
{
	unsigned char data=0xff;
	switch(ioport)
	{
	case TOWNSIO_TIMER0_COUNT_LOW://         0x40,
		data=(state.channels[0].latchedCounter&0xff);
		break;
	case TOWNSIO_TIMER0_COUNT_HIGH://        0x41,
		data=((state.channels[0].latchedCounter>>8)&0xff);
		break;
	case TOWNSIO_TIMER1_COUNT_LOW://         0x42,
		data=(state.channels[1].latchedCounter&0xff);
		state.TMOUT[1]=false;
		break;
	case TOWNSIO_TIMER1_COUNT_HIGH://        0x43,
		data=((state.channels[1].latchedCounter>>8)&0xff);
		state.TMOUT[1]=false;
		break;
	case TOWNSIO_TIMER2_COUNT_LOW://         0x44,
		data=(state.channels[2].latchedCounter&0xff);
		break;
	case TOWNSIO_TIMER2_COUNT_HIGH://        0x45,
		data=((state.channels[2].latchedCounter>>8)&0xff);
		break;
	case TOWNSIO_TIMER_0_1_2_CTRL://         0x46,
		break;
	case TOWNSIO_TIMER3_COUNT_LOW://         0x50,
		data=(state.channels[3].latchedCounter&0xff);
		break;
	case TOWNSIO_TIMER3_COUNT_HIGH://        0x51,
		data=((state.channels[3].latchedCounter>>8)&0xff);
		break;
	case TOWNSIO_TIMER4_COUNT_LOW://         0x52,
		data=(state.channels[4].latchedCounter&0xff);
		break;
	case TOWNSIO_TIMER4_COUNT_HIGH://        0x53,
		data=((state.channels[4].latchedCounter>>8)&0xff);
		break;
	case TOWNSIO_TIMER5_COUNT_LOW://         0x54,
		data=(state.channels[5].latchedCounter&0xff);
		break;
	case TOWNSIO_TIMER5_COUNT_HIGH://        0x55,
		data=((state.channels[5].latchedCounter>>8)&0xff);
		break;
	case TOWNSIO_TIMER_3_4_5_CTRL://         0x56,
		break;
	case TOWNSIO_TIMER_INT_CTRL_INT_REASON://0x60,
		data=0;
		data|=(state.TMOUT[0] ? 0x01 : 0);
		data|=(state.TMOUT[1] ? 0x02 : 0);
		data|=(state.TMMSK[0] ? 0x04 : 0);
		data|=(state.TMMSK[1] ? 0x08 : 0);
		data|=(state.SOUND ? 0x10 : 0);
		break;
	}
	return data;
}

/* virtual */ void TownsTimer::RunScheduledTask(unsigned long long int townsTime)
{
	if(0==state.lastTickTimeInNS)
	{
		state.lastTickTimeInNS=townsTime;
	}
	else if(state.lastTickTimeInNS+TICK_INTERVAL<=townsTime)
	{
		auto nTick=(townsTime-state.lastTickTimeInNS)/TICK_INTERVAL;
		state.lastTickTimeInNS+=nTick*TICK_INTERVAL;

		bool IRQ=false;
		bool OUT[2]={state.channels[0].OUT,state.channels[1].OUT};
		state.TickIn((unsigned int)nTick);
		for(unsigned int ch=0; ch<2; ++ch)
		{
			if(true!=OUT[ch] && true==state.channels[ch].OUT)
			{
				state.TMOUT[ch]=true;
				if(true==state.TMMSK[ch])
				{
					IRQ=true;
				}
			}
		}
		if(true==IRQ)
		{
			// picPtr->InterruptRequest(TOWNSIRQ_TIMER);
		}
	}
}

std::vector <std::string> TownsTimer::GetStatusText(void) const
{
	std::string newline;
	std::vector <std::string> text;
	text.push_back("Programmable Timer (i8253 x2)");

	for(auto &CH : state.channels)
	{
		unsigned int ch=(unsigned int)(&CH-state.channels);
		text.push_back(newline);
		text.back()+="[";
		text.back()+=cpputil::Ubtox(ch);
		text.back()+="]:";

		text.back()+="LastCMD_MOD=";
		text.back()+=cpputil::Ubtox((CH.lastCmd>>1)&7);
		text.back()+="  Actual_MOD=";
		text.back()+=cpputil::Ubtox(CH.mode);
		text.back()+="  BCD=";
		text.back()+=(CH.bcd ? "1" : "0");
		text.back()+="  RL=";
		text.back()+=cpputil::Ubtox((CH.lastCmd>>4)&3);
		text.back()+="  CTR=";
		text.back()+=cpputil::Ustox(CH.counter);
		text.back()+="  CTR0=";
		text.back()+=cpputil::Ustox(CH.counterInitialValue);
		text.back()+="  LATCHED=";
		text.back()+=((CH.latched) ? "1" : "0");
		text.back()+="  LATCHEDCTR=";
		text.back()+=cpputil::Ustox(CH.latchedCounter);
		text.back()+="  OUT=";
		text.back()+=((CH.OUT) ? "1" : "0");
	}
	text.push_back(newline);
	text.back()+="TM0INT:";
	text.back()+=(state.TMMSK[0] ? "Enabled" : "Disabled");
	text.back()+="  TM1INT:";
	text.back()+=(state.TMMSK[1] ? "Enabled" : "Disabled");
	text.back()+="  TM0OUT:";
	text.back()+=(state.TMOUT[0] ? "1" : "0");
	text.back()+="  TM1OUT:";
	text.back()+=(state.TMOUT[1] ? "1" : "0");
	text.back()+="  SOUND:";
	text.back()+=(state.SOUND ? "1" : "0");
	return text;
}
