/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <math.h>
#include <iostream>

#include "ym2612.h"



void YM2612::Slot::Clear(void)
{
	DT=0;
	MULTI=0;
	TL=0;
	KS=0;
	AR=0;
	AM=0;
	DR=0;
	SR=0;
	SL=0;
	RR=0;
	SSG_EG=0;
}
void YM2612::Channel::Clear()
{
	F_NUM=0;
	BLOCK=0;
	FB=0;
	CONNECT=0;
	L=1;
	R=1;
	AMS=0;
	PMS=0;
	usingSlot=0;
	for(auto &s : slots)
	{
		s.Clear();
	}
}

////////////////////////////////////////////////////////////

/*static*/ int YM2612::sineTable[YM2612::PHASE_STEPS];
/*static*/ unsigned int YM2612::TLtoDB100[128];   // 100 times dB
/*static*/ unsigned int YM2612::SLtoDB100[16];    // 100 times dB
/*static*/ unsigned int YM2612::DB100to4095Scale[9601]; // dB to 0 to 4095 scale
/*static*/ unsigned int YM2612::DB100from4095Scale[4096]; // 0 to 4095 scale to dB
/*static*/ unsigned int YM2612::linear4096to9600[4097]; // Linear 4096 scale to 9600 scale
/*static*/ unsigned int YM2612::linear9600to4096[9601]; // Linear 9600 scale to 4096 scale
/*static*/ const unsigned int YM2612::connToOutChannel[8][4]=
{
	{0,0,0,1},
	{0,0,0,1},
	{0,0,0,1},
	{0,0,0,1},
	{0,1,0,1},
	{0,1,1,1},
	{0,1,1,1},
	{1,1,1,1},
};

void YM2612::State::PowerOn(void)
{
	Reset();
}
void YM2612::State::Reset(void)
{
	deviceTimeInNS=0;
	lastTickTimeInNS=0;
	for(auto &c : channels)
	{
		c.Clear();
	}
	for(auto &f : F_NUM_3CH)
	{
		f=0;
	}
	for(auto &b : BLOCK_3CH)
	{
		b=0;
	}
	for(auto &f : F_NUM_6CH)
	{
		f=0;
	}
	for(auto &b : BLOCK_6CH)
	{
		b=0;
	}
	for(auto &r : reg)
	{
		r=0;
	}
	for(auto &t : timerCounter)
	{
		t=0;
	}
	for(auto &b : timerUp)
	{
		b=false;
	}
	for(auto &ch : channels)
	{
		for(auto &slot : ch.slots)
		{
			slot.lastDbX100Cache=0;
		}
	}
	playingCh=0;
	LFO=0;
	FREQCTRL=0;
}

////////////////////////////////////////////////////////////

YM2612::YM2612()
{
	std::cout << "YM2612 Emulator for Tsugaru [Tsugaru-Ben]" << std::endl;

	MakeSineTable();
	MakeTLtoDB100();
	MakeSLtoDB100();
	MakeDB100to4095Scale();
	MakeLinearScaleTable();
	MakeAttackProfileTable();
	PowerOn();
}
YM2612::~YM2612()
{
}

void YM2612::MakeSineTable(void)
{
	const double PI=3.14159265358979323;
	for(int i=0; i<PHASE_STEPS; ++i)
	{
		double a=2.0*PI*(double)i/(double)PHASE_STEPS;
		double s=sin(a);
		s*=(double)(UNSCALED_MAX);
		sineTable[i]=(int)s;
		// printf("%+-6d,",(int)s);
		// if(i%16==15)
		// {
		// 	printf("\n");
		// }
	}
}
void YM2612::MakeTLtoDB100(void)
{
	for(unsigned int TL=0; TL<128; ++TL)
	{
		TLtoDB100[TL]=TL*75;
	}
}
void YM2612::MakeSLtoDB100(void)
{
	for(unsigned int SL=0; SL<16; ++SL)
	{
		SLtoDB100[SL]=SL*300;
	}
}
void YM2612::MakeDB100to4095Scale(void)
{
	// To convert 0 to 96dB (log scale) to amplitude 0 to 4095,
	//    dB=20*log10(C*amplitude)
	//    96=20*log10(C*4095)
	//    4.8=log10(C*4095)
	//    10^4.8=C*4095
	//    C=(10^4.8)/4095.0
	const double C=pow(10.0,4.8)/4095.0;

	//    dB=20*log10(C*amplitude)
	//    log10(C*amplitude)=dB/20
	//    10^(dB/20)=C*amplitude
	//    amplitude=(10^(dB/20))/C

	for(int i=0; i<=9600; ++i)
	{
		double dB=(double)i/100.0;
		DB100to4095Scale[i]=(unsigned int)(pow(10.0,dB/20.0)/C);
		// if(0==i%100)
		// {
		// 	printf("%4ddB -> %4d\n",i/100,DB100to4095Scale[i]);
		// }
	}
	for(int i=0; i<4096; ++i)
	{
		double dB100=100.0*(20.0*log10(C*(double)i));
		DB100from4095Scale[i]=(unsigned int)dB100;
	}
}
void YM2612::MakeLinearScaleTable(void)
{
	for(unsigned int i=0; i<=4096; ++i)
	{
		linear4096to9600[i]=i*9600/4096;
	}
	for(unsigned int i=0; i<=9600; ++i)
	{
		linear9600to4096[i]=i*4096/9600;
	}
}

void YM2612::MakeAttackProfileTable(void)
{
	// YM2612 manual tells that the output level increases during the attack phase is exponential,
	// which can open up a lot of interpretations.  Is it like dB(t)=C*k^t  ?
	// By looking at fmgen by Cisc, it seems to be the difference from the peak output decreases
	// exponentially.
	//
	// If the level difference from the maximum level is diff(i),
	//     diff(i+1)=diff(i)*k   {0<t<1}
	// Then,
	//     diff(i)=diff(0)*(k^i) {0<t<1}
	// Let's say the maximum is 4096.  I want to say diff reaches 0 at t=4096, but well it won't be zero.
	// So, let's say diff reaches 1 at t=409t instead.  The initial difference is 4096.  Then,
	// 
	//     1=4096*t^4095, 
	// 
	// From there, I can calculate t.  And then values for the table.

	const double lastErr=0.014;
	const double t=pow(lastErr,1.0/4096.0);
	for(int i=0; i<4096; ++i)
	{
		double y=(1.0-pow(t,(double)i))/(1.0-lastErr);
		attackExp[i]=(int)(y*4096);
	}

	int j=0;
	for(int i=0; i<4096; ++i)
	{
		for(; j<=attackExp[i+1]; ++j)
		{
			attackExpInverse[j]=i;
		}
	}
}

void YM2612::PowerOn(void)
{
	state.PowerOn();
}
void YM2612::Reset(void)
{
	state.Reset();
}
unsigned int YM2612::WriteRegister(unsigned int channelBase,unsigned int reg,unsigned int value)
{
	if(true==takeRegLog)
	{
		RegWriteLog rwl;
		rwl.chBase=(unsigned char)channelBase;
		rwl.reg=(unsigned char)reg;
		rwl.data=(unsigned char)value;
		rwl.count=1;
		if(0<regWriteLog.size() &&
		   regWriteLog.back().chBase==rwl.chBase &&
		   regWriteLog.back().reg==rwl.reg &&
		   regWriteLog.back().data==rwl.data)
		{
			++regWriteLog.back().count;
		}
		else
		{
			regWriteLog.push_back(rwl);
		}
	}
	unsigned int chStartPlaying=65535;
	static const unsigned int slotTwist[4]={0,2,1,3};
	reg&=255;
	auto prev=state.reg[reg];
	state.reg[reg]=value;
	if(REG_TIMER_CONTROL==reg)
	{
		// [2] pp. 202 RESET bits will be cleared immediately after set.
		// .... I interpret it as RESET bits are always read to zero.
		state.reg[REG_TIMER_CONTROL]&=0xCF;

		// LOAD bits are mysterious.
		// [2] pp.201 tells that writing 1 to LOAD bit resets the counter and start counting.
		// Towns OS's behavior does not seem to be agree with it.
		// Timer A interrupt handler writes 0x3F to register 0x27.  If I implement as [2] pp.201,
		// Timer B counter is reset when Timer A is up, or vise versa.
		// Slower one of Timer A or B will never be up.
		//
		// There are some possibilities:
		// (1) The timer counter resets on the rising edge of LOAD.  When timer is up, LOAD will be cleared.
		//     Therefore, writing LOAD=1 when LOAD is already 1 does nothing.
		// (2) Towns OS writes (0118:[0727H])|0x15 for Timer A or (0118:[0727H])|0x2A for Timer B for resetting the counter.
		//     0118:[0727H] is a cached value (or read back) from YM2612 register 27H.  If LOAD bits are always zero when
		//     read, it won't reset the timer when resetting the other timer.
		// (3) The timer counter is reloaded on LOAD=1 only if the timer is up.
        //
		// There are some web sites such as:
		//   https://plutiedev.com/ym2612-registers#reg-27
		//   https://www.smspower.org/maxim/Documents/YM2612
		//   https://wiki.megadrive.org/index.php?title=YM2612_Registers
		// suggesting that LOAD bit means the timer is running.  If so, I guess (2) is unlikely.
		// With my elementary knowledge in FPGA programming, (1) looks to be more straight-forward.
		// Currently I go with (1).

		if(0==(prev&1) && 0!=(value&1)) // Load Timer A
		{
			unsigned int countHigh=state.reg[REG_TIMER_A_COUNT_HIGH];
			unsigned int countLow=state.reg[REG_TIMER_A_COUNT_LOW];
			auto count=(countHigh<<2)|(countLow&3);
			state.timerCounter[0]=count*TIMER_A_PER_TICK;
		}
		if(0==(prev&2) && 0!=(value&2)) // Load Timer B
		{
			state.timerCounter[1]=(unsigned int)(state.reg[REG_TIMER_B_COUNT])*TIMER_B_PER_TICK;
		}
		if(value&4) // Enable Timer A Flag
		{
		}
		if(value&8) // Enable Timer B Flag
		{
		}
		if(value&0x10) // Reset Timer A Flag
		{
			state.timerUp[0]=false;
		}
		if(value&0x20) // Reset Timer B Flag
		{
			state.timerUp[1]=false;
		}
	}
	else if(REG_KEY_ON_OFF==reg)
	{
		static unsigned int chTwist[8]={0,1,2,255,3,4,5,255};
		unsigned int ch=chTwist[value&7];
		if(ch<6)
		{
			unsigned int slotFlag=((value>>4)&0x0F);

			unsigned int onSlots=(~state.channels[ch].usingSlot)&slotFlag;
			unsigned int offSlots=(state.channels[ch].usingSlot&(~slotFlag))&0x0F;

			// Prob, this is the trigger to start playing.
			// F-BASIC386 first writes SLOT=0 then SLOT=0x0F.
			if(0!=onSlots)
			{
				// Play a tone
				KeyOn(ch,onSlots);
				chStartPlaying=ch;
			}
			if(0!=offSlots)
			{
				KeyOff(ch,offSlots);
			}

			state.channels[ch].usingSlot=slotFlag;
		}
	}
	else if(REG_LFO==reg)
	{
		state.LFO=(0!=(value&8));
		state.FREQCTRL=value&7;
	}
	else if(0xA8<=reg && reg<=0xAE) // Special 3CH F-Number/BLOCK
	{
		unsigned int slot=(reg&3);
		if(slot<3)
		{
			if(0==channelBase)
			{
				if(reg<0xAC)
				{
					state.F_NUM_3CH[slot]&=0xFF00;
					state.F_NUM_3CH[slot]|=value;
				}
				else
				{
					state.F_NUM_3CH[slot]&=0xFF;
					state.F_NUM_3CH[slot]|=((value&7)<<8);
					state.BLOCK_3CH[slot]=((value>>3)&7);
				}
			}
			else if(3==channelBase)
			{
				if(reg<0xAC)
				{
					state.F_NUM_6CH[slot]&=0xFF00;
					state.F_NUM_6CH[slot]|=value;
				}
				else
				{
					state.F_NUM_6CH[slot]&=0xFF;
					state.F_NUM_6CH[slot]|=((value&7)<<8);
					state.BLOCK_6CH[slot]=((value>>3)&7);
				}
			}
		}
	}
	else if(0x30<=reg && reg<=0x9E) // Per Channel per slot
	{
		unsigned int ch=(reg&3);
		if(ch<=2)
		{
			const unsigned int slot=slotTwist[((reg>>2)&3)];
			ch+=channelBase;
			switch(reg&0xF0)
			{
			case 0x30: // DT, MULTI
				state.channels[ch].slots[slot].DT=((value>>4)&7);
				state.channels[ch].slots[slot].MULTI=(value&15);
				break;
			case 0x40: // TL
				{
					auto prevTL=state.channels[ch].slots[slot].TL;
					state.channels[ch].slots[slot].TL=(value&0x7F);
					if(0!=(state.channels[ch].usingSlot&(1<<slot)))
					{
						auto prevLevel=127-prevTL;
						auto newLevel=127-state.channels[ch].slots[slot].TL;
						UpdateSlotEnvelope(state.channels[ch],state.channels[ch].slots[slot]);
						if(0!=prevLevel)
						{
							// Must be linear in dB scale.  To prepare for subsequent release phase.
							state.channels[ch].slots[slot].lastDbX100Cache*=newLevel;
							state.channels[ch].slots[slot].lastDbX100Cache/=prevLevel;
						}
					}
					else if(true==state.channels[ch].slots[slot].InReleasePhase)
					{
						auto prevLevel=127-prevTL;
						auto newLevel=127-state.channels[ch].slots[slot].TL;
						if(0!=prevLevel)
						{
							// Must be linear in dB scale.  lastDbX100Cache shouldn't matter already.
							state.channels[ch].slots[slot].ReleaseStartDbX100*=newLevel;
							state.channels[ch].slots[slot].ReleaseStartDbX100/=prevLevel;
						}
					}
				}
				break;
			case 0x50: // KS,AR
				state.channels[ch].slots[slot].KS=((value>>6)&3);
				state.channels[ch].slots[slot].AR=(value&0x1F);
				break;
			case 0x60: // AM,DR
				state.channels[ch].slots[slot].AM=((value>>7)&1);
				state.channels[ch].slots[slot].DR=(value&0x1F);
				break;
			case 0x70: // SR
				state.channels[ch].slots[slot].SR=(value&0x1F);
				break;
			case 0x80: // SL,RR
				state.channels[ch].slots[slot].SL=((value>>4)&0x0F);
				state.channels[ch].slots[slot].RR=(value&0x0F);
				if(0!=(state.channels[ch].usingSlot&(1<<slot)))
				{
					UpdateSlotEnvelope(state.channels[ch],state.channels[ch].slots[slot]);
				}
				else if(true==state.channels[ch].slots[slot].InReleasePhase)
				{
					UpdateRelease(state.channels[ch],state.channels[ch].slots[slot]);
				}
				break;
			case 0x90: // SSG-EG
				state.channels[ch].slots[slot].SSG_EG=(value&0x0F);
				break;
			}
		}
	}
	else if(0xA0<=reg && reg<=0xB6)
	{
		unsigned int ch=(reg&3);
		if(ch<=2)
		{
			unsigned int slot=slotTwist[((reg>>2)&3)];
			ch+=channelBase;
			switch(reg&0xFC)
			{
			case 0xA0: // F-Number1
				// [2] pp.211 Implies that writing to reg A0H to A2H triggers a tone to play.
				//     When setting the note, first write BLOCK and high 3-bits of F-Number (F-Number2),
				//     and then write lower 8-bits of F-Number (F-Number1).
				//     Or, is it REG_KEY_ON_OFF?
				state.channels[ch].F_NUM&=0xFF00;
				state.channels[ch].F_NUM|=value;
				UpdatePhase12StepSlot(state.channels[ch]);
				break;
			case 0xA4: // BLOCK,F-Number2
				state.channels[ch].F_NUM&=0x00FF;
				state.channels[ch].F_NUM|=((value&7)<<8);
				state.channels[ch].BLOCK=((value>>3)&7);
				break;
			case 0xB0: // FB, CONNECT
				state.channels[ch].FB=((value>>3)&7);
				state.channels[ch].CONNECT=(value&7);
				break;
			case 0xB4: // L,R,AMS,PMS
				state.channels[ch].L=((value>>7)&1);
				state.channels[ch].R=((value>>6)&1);
				state.channels[ch].AMS=((value>>4)&3);
				state.channels[ch].PMS=(value&7);
				break;
			}
		}
	}
	return chStartPlaying;
}
unsigned int YM2612::ReadRegister(unsigned int channelBase,unsigned int reg) const
{
	return state.reg[reg&255];
}
void YM2612::Run(unsigned long long int systemTimeInNS)
{
	if(0==state.deviceTimeInNS)
	{
		state.lastTickTimeInNS=systemTimeInNS;
		state.deviceTimeInNS=systemTimeInNS;
		return;
	}
	if(state.lastTickTimeInNS+TICK_DURATION_IN_NS<systemTimeInNS)
	{
		auto nTick=(systemTimeInNS-state.lastTickTimeInNS)/TICK_DURATION_IN_NS;
		state.lastTickTimeInNS+=nTick*TICK_DURATION_IN_NS;
		// See (1) in the above comment.
		if(0!=(state.reg[REG_TIMER_CONTROL]&0x01))
		{
			state.timerCounter[0]+=nTick;
			if(NTICK_TIMER_A<=state.timerCounter[0])
			{
				state.reg[REG_TIMER_CONTROL]&=(~0x01);
				if(0!=(state.reg[REG_TIMER_CONTROL]&0x04))
				{
					state.timerUp[0]=true;
				}
			}
		}
		if(0!=(state.reg[REG_TIMER_CONTROL]&0x02))
		{
			state.timerCounter[1]+=nTick;
			if(NTICK_TIMER_B<=state.timerCounter[1])
			{
				state.reg[REG_TIMER_CONTROL]&=(~0x02);
				if(0!=(state.reg[REG_TIMER_CONTROL]&0x08))
				{
					state.timerUp[1]=true;
				}
			}
		}
	}


	state.deviceTimeInNS=systemTimeInNS;
}
bool YM2612::TimerAUp(void) const
{
	return state.timerUp[0];
}
bool YM2612::TimerBUp(void) const
{
	return state.timerUp[1];
}
bool YM2612::TimerUp(unsigned int timerId) const
{
	switch(timerId&1)
	{
	default:
	case 0:
		return TimerAUp();
	case 1:
		return TimerBUp();
	}
}

/* static */ void YM2612::GetCarrierSlotFromConnection(int &numCarrierSlots,int carrierSlots[4],unsigned int connection)
{
	connection&=7;
	numCarrierSlots=connectionToOutputSlots[connection].nOutputSlots;
	carrierSlots[0]=connectionToOutputSlots[connection].slots[0];
	carrierSlots[1]=connectionToOutputSlots[connection].slots[1];
	carrierSlots[2]=connectionToOutputSlots[connection].slots[2];
	carrierSlots[3]=connectionToOutputSlots[connection].slots[3];
}



std::vector <std::string> YM2612::GetStatusText(void) const
{
	std::vector <std::string> text;
	char str[256];

	text.push_back("YM2612");

	for(int chNum=0; chNum<6; ++chNum)
	{
		auto &ch=state.channels[chNum];

		sprintf(str,"CH:%d  F_NUM=%-5d  BLOCK=%-2d  FB=%d  CONNECT=%d  L=%d  R=%d  AMS=%d  PMS=%d  ActiveSlots=%02x",
			chNum,
			ch.F_NUM,
			ch.BLOCK,
			ch.FB,
			ch.CONNECT,
			ch.L,
			ch.R,
			ch.AMS,
			ch.PMS,
			ch.usingSlot);
		text.push_back(str);

		int s=0;
		for(auto &slot : ch.slots)
		{
			sprintf(str,"SLOT:%d  DT=%d  MULTI=%-2d  TL=%-3d(%2ddB)  KS=%d  AR=%-2d  AM=%d  DR=%-2d  SR=%-2d  SL=%2d(%2ddB)  RR=%-2d  SSG_EG=%d",
				s,
				slot.DT,
				slot.MULTI,
				slot.TL,
				TLtoDB100[slot.TL]/100,
				slot.KS,
				slot.AR,
				slot.AM,
				slot.DR,
				slot.SR,
				slot.SL,
				SLtoDB100[slot.SL]/100,
				slot.RR,
				slot.SSG_EG);
			text.push_back(str);
			++s;
		}
	}


	sprintf(str,"TimerA Up=%d  Count Preset=0x%04x  Internal Count/Threshold=0x%08x/0x%08x",
		TimerAUp(),
		((state.reg[REG_TIMER_A_COUNT_HIGH]<<2)|(state.reg[REG_TIMER_A_COUNT_LOW]&3)),
		(state.timerCounter[0]&0xFFFFFFFF),
		NTICK_TIMER_A);
	text.push_back(str);


	sprintf(str,"TimerB Up=%d  Count Preset=0x%04x  Internal Count/Threshold=0x%08x/0x%08x",
		TimerBUp(),
		state.reg[REG_TIMER_B_COUNT],
		(state.timerCounter[1]&0xFFFFFFFF),
		NTICK_TIMER_B);
	text.push_back(str);


	sprintf(str,"Timer Control(Reg 0x%02x)=0x%02x  MODE:0x%02x  RST:0x%02x  ENA:0x%02x  LOAD:0x%02x",
		REG_TIMER_CONTROL,
		state.reg[REG_TIMER_CONTROL],
		((state.reg[REG_TIMER_CONTROL]>>6)&3),
		((state.reg[REG_TIMER_CONTROL]>>4)&3),
		((state.reg[REG_TIMER_CONTROL]>>2)&3),
		(state.reg[REG_TIMER_CONTROL]&3));
	text.push_back(str);


	sprintf(str,"LFO:%d  FREQ-CTRL:%d",state.LFO,state.FREQCTRL);
	text.push_back(str);

	return text;
}
