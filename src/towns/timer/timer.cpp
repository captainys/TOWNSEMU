/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <algorithm>
#include <limits>
#include <utility>

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
		ch.counting=false;
		ch.latchedCounter=0;
		ch.increment=1;
		ch.bcd=false;
		ch.OUT=false;
		ch.latched=false;

		ch.RL=1;
		ch.accessLow=true;
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
	SOUND_MEMIO = false;
	buzzerPhase = 0;
}
void TownsTimer::State::Latch(unsigned int ch)
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
unsigned short TownsTimer::State::ReadLatchedCounter(unsigned int ch) const
{
	auto &CH=channels[ch&7];
	CH.latched=false;
	return CH.latchedCounter;
}
void TownsTimer::State::SetChannelCounter(unsigned int ch,unsigned int value)
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
	case 0:
		CH.OUT=false;
		break;
	case 3:
		CH.OUT=true;
		if(true==CH.accessLow)
		{
			CH.counterInitialValue+=(value&1); // Force it to be even number.
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

void TownsTimer::State::SetChannelMode(unsigned int ch,unsigned int mode)
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

unsigned int TownsTimer::State::ReadChannelCounter(unsigned int ch) // accessLow may flip.  Not to be const.
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
void TownsTimer::State::ProcessControlCommand(unsigned int ch,unsigned int cmd)
{
	auto &CH=channels[ch&7];
	CH.lastCmd=cmd;
	CH.bcd=(0!=(cmd&1));

	auto RL=(cmd>>4)&3;
	if(0==RL)
	{
		Latch(ch);
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
void TownsTimer::State::TickIn(unsigned int nTick)
{
	for(int ch=0; ch<NUM_CHANNELS_ACTUAL; ++ch)
	{
		// What to do when counterInitialValue==0?
		// [12] Source code of Artane's FM Towns emulator project https://github.com/Artanejp
		// It suggests to take it as 65536.  I follow Artane's implementation.
		auto &CH=channels[ch];
		if(true==CH.counting)
		{
			auto increment=nTick*CH.increment;
			switch(CH.mode)
			{
			case 0:
				if(CH.counter<=increment)
				{
					CH.OUT=true;
					CH.counter=(0!=CH.counterInitialValue ? CH.counterInitialValue : 65535);
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


////////////////////////////////////////////////////////////

TownsTimer::TownsTimer(class FMTownsCommon *townsPtr,class TownsPIC *picPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
	this->picPtr=picPtr;
}

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
	state.SetChannelMode(0,3);
	state.SetChannelMode(1,0);
	state.SetChannelMode(2,3);
	state.SetChannelMode(3,3);
	state.SetChannelMode(4,3);
	state.SetChannelMode(5,3);

	state.channels[4].increment=4;  // Only channel 4 counts up 1.2288MHz.  Others 307.2KHz.
}
/* virtual */ void TownsTimer::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_TIMER0_COUNT://             0x40,
		state.SetChannelCounter(0,data);
		break;
	case TOWNSIO_TIMER1_COUNT://             0x42,
		state.SetChannelCounter(1,data);
		state.TMOUT[1]=false;
		UpdatePICRequest();
		break;
	case TOWNSIO_TIMER2_COUNT://             0x44,
		state.SetChannelCounter(2,data);
		break;
	case TOWNSIO_TIMER_0_1_2_CTRL://         0x46,
		{
			unsigned ch=((data>>6)&3);
			state.ProcessControlCommand(ch,data&0x3f);
		}
		break;
	case TOWNSIO_TIMER3_COUNT://             0x50,
		state.SetChannelCounter(3,data);
		break;
	case TOWNSIO_TIMER4_COUNT://             0x52,
		state.SetChannelCounter(4,data);
		break;
	case TOWNSIO_TIMER5_COUNT://             0x54,
		state.SetChannelCounter(5,data);
		break;
	case TOWNSIO_TIMER_3_4_5_CTRL://         0x56,
		{
			unsigned ch=3+((data>>6)&3);
			state.ProcessControlCommand(ch,data&0x3f);
		}
		break;
	case TOWNSIO_TIMER_INT_CTRL_INT_REASON://0x60,
		{
			bool prevBeep=state.SOUND;
			state.TMMSK[0]=(0!=(data&1));
			state.TMMSK[1]=(0!=(data&2));
			state.SOUND=(0!=(data&4));
			if(0!=(data&0x80))
			{
				state.TMOUT[0]=false;
			}
			if(true==breakOnBeep && true!=prevBeep && true==state.SOUND)
			{
				townsPtr->debugger.ExternalBreak("Beep On");
			}
		}
		UpdatePICRequest();
		break;
	}
}
/* virtual */ unsigned int TownsTimer::IOReadByte(unsigned int ioport)
{
	unsigned char data=0xff;
	switch(ioport)
	{
	case TOWNSIO_TIMER0_COUNT://             0x40,
		data=state.ReadChannelCounter(0);
		break;
	case TOWNSIO_TIMER1_COUNT://             0x42,
		data=state.ReadChannelCounter(1);
		state.TMOUT[1]=false;
		break;
	case TOWNSIO_TIMER2_COUNT://             0x44,
		data=state.ReadChannelCounter(2);
		break;
	case TOWNSIO_TIMER_0_1_2_CTRL://         0x46,
		break;
	case TOWNSIO_TIMER3_COUNT://             0x50,
		data=state.ReadChannelCounter(3);
		break;
	case TOWNSIO_TIMER4_COUNT://             0x52,
		data=state.ReadChannelCounter(4);
		break;
	case TOWNSIO_TIMER5_COUNT://             0x54,
		data=state.ReadChannelCounter(5);
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

void TownsTimer::UpdatePICRequest(void) const
{
	auto IRQBit=((state.TMOUT[0] && state.TMMSK[0]) || (state.TMOUT[1] && state.TMMSK[1]));
	picPtr->SetInterruptRequestBit(TOWNSIRQ_TIMER,IRQBit);
}

std::vector <std::string> TownsTimer::GetStatusText(void) const
{
	std::string newline;
	std::vector <std::string> text;
	text.push_back("Programmable Timer (i8253 x2)");

	for(int ch=0; ch<NUM_CHANNELS_ACTUAL; ++ch)
	{
		auto &CH=state.channels[ch];
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

void TownsTimer::ControlBuzzerByMemoryIO(bool on)
{
	state.SOUND_MEMIO = on;
}

bool TownsTimer::IsBuzzerPlaying() const
{
	return state.SOUND || state.SOUND_MEMIO;
}

/*
 * Features not implemented:
 * - BCD mode
 * - non-50% duty cycle on odd counter
 * - 4.6875Hz sound on zero counter
 */
std::pair<uint32_t, std::vector<unsigned char>> TownsTimer::MakeBuzzerWave(int ms)
{
	// New version of clang doesn't like it.
	// static_assert(std::numeric_limits<uint16_t>::max() > BUZZER_SAMPLING_RATE(), "BUZZER_SAMPLING_RATE too large.");
	// static_assert(std::numeric_limits<int32_t>::max() / BUZZER_VOLUME() > TIMER_CLOCK_HZ(), "BUZZER_VOLUME too large.");

	size_t samples = BUZZER_SAMPLING_RATE() * ms / 1000;

	std::vector<unsigned char> vec;
	vec.resize(samples * 4);

	uint16_t counter = state.channels[2].counterInitialValue;
	if (counter < 2) {
		return std::make_pair(BUZZER_SAMPLING_RATE(), std::move(vec));
	}

	uint32_t period = counter * BUZZER_SAMPLING_RATE();
	uint32_t phase = state.buzzerPhase;

	int32_t i0 = std::min(phase, period - phase);

	auto t = vec.begin();
	for (size_t n = 0; n < samples; ++n) {
		phase = (phase + TIMER_CLOCK_HZ()) % period;

		int32_t i1 = std::min(phase, period - phase);

		int16_t value = BUZZER_VOLUME() * (i1 - i0) / int32_t( TIMER_CLOCK_HZ() );
		*t++ = value & 0xff;
		*t++ = (value >> 8) & 0xff;
		*t++ = value & 0xff;
		*t++ = (value >> 8) & 0xff;

		i0 = i1;
	}

	state.buzzerPhase = phase;

	return std::make_pair(BUZZER_SAMPLING_RATE(), std::move(vec));
}

/* virtual */ uint32_t TownsTimer::SerializeVersion(void) const
{
	return 0;
}
/* virtual */ void TownsTimer::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	PushUint64(data,state.lastTickTimeInNS);
	for(auto &ch : state.channels)
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

	PushBool(data,state.TMMSK[0]);
	PushBool(data,state.TMMSK[1]);
	PushBool(data,state.TMOUT[0]);
	PushBool(data,state.TMOUT[1]);
	PushBool(data,state.SOUND);
	PushBool(data,state.SOUND_MEMIO);

	PushUint32(data,state.buzzerPhase);
}
/* virtual */ bool TownsTimer::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	state.lastTickTimeInNS=ReadUint64(data);
	for(auto &ch : state.channels)
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

	state.TMMSK[0]=ReadBool(data);
	state.TMMSK[1]=ReadBool(data);
	state.TMOUT[0]=ReadBool(data);
	state.TMOUT[1]=ReadBool(data);
	state.SOUND=ReadBool(data);
	state.SOUND_MEMIO=ReadBool(data);

	state.buzzerPhase=ReadUint32(data);

	return true;
}
