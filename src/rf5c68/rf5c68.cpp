/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <cstring>
#include "rf5c68.h"
#include "cpputil.h"



inline void WordOp_Add(unsigned char *ptr,int value)
{
	value+=cpputil::GetSignedWord(ptr);

	if(value<-32767)
	{
		value=-32767;
	}
	else if(32767<value)
	{
		value=32767;
	}
	cpputil::PutWord(ptr,(value&0xFFFF));
}

static inline int Gain(int a,int b)
{
	int c=a+b;
	if(c<-32768 || 32767<c)
	{
		return c/2;
	}
	else
	{
		return c;
	}
}

// Linear interpolation
static inline int lerp_int(int start, int end, float t)
{
	if (t >= 1.0)
	{
		return end;
	}
	else if(t <= 0.0)
	{
		return start;
	}
	float ret = (1.0 - t) * start + t * end;
	return (int)ret;
}


RF5C68::RF5C68()
{
	state.waveRAM.resize(WAVERAM_SIZE);
	Clear();
}

void RF5C68::Clear(void)
{
	for(auto &b : state.waveRAM)
	{
		b=0xff;
	}
	for(auto &ch : state.ch)
	{
		ch.ENV=255;
		ch.PAN=0;
		ch.ST=0;
		ch.FD=0;
		ch.LS=0;
		ch.playPtr=0;
		ch.playPtrLeftOver=0;
		ch.repeatAfterThisSegment=false;

		ch.IRQAfterThisPlayBack=false;
		ch.IRQBank=0;
	}
	state.playing=false;
	state.Bank=0;
	state.CB=0;
	state.chOnOff=0xff;  // Active LOW
	state.IRQBank=0;
	state.IRQBankMask=0;
	state.timeBalance=0;
	state.Lout_prev=0;
	state.Rout_prev=0;
}

RF5C68::StartAndStopChannelBits RF5C68::WriteControl(unsigned char value)
{
	if(0x40&value)
	{
		state.CB=(value&7);
	}
	else
	{
		auto WB=(value&0x0f);
		state.Bank=WB;
		state.Bank<<=12;
	}

	StartAndStopChannelBits startStop;
	if(0x80&value)
	{
		if(true!=state.playing)
		{
			startStop.chStartPlay=~state.chOnOff; // Active LOW
		}
		state.playing=true;
	}
	else
	{
		startStop.chStopPlay=~state.chOnOff; // Active LOW
		state.playing=false;
	}
	return startStop;
}
RF5C68::StartAndStopChannelBits RF5C68::WriteChannelOnOff(unsigned char value)
{
	StartAndStopChannelBits startStop;
	if(true==state.playing)
	{
		startStop.chStartPlay=(state.chOnOff&(~value)); // Active LOW:  prev==1(not playing) && now==0(playing)
		startStop.chStopPlay=((~state.chOnOff)&value);  // Active Low:  prev==0(playing) && now==1(not playing)
	}
	state.chOnOff=value;
	return startStop;
}
void RF5C68::WriteIRQBankMask(unsigned char value)
{
	state.IRQBankMask=value;
}
void RF5C68::WriteENV(unsigned char value)
{
	state.ch[state.CB].ENV=value;
}
void RF5C68::WritePAN(unsigned char value)
{
	state.ch[state.CB].PAN=value;
}
void RF5C68::WriteFDL(unsigned char value)
{
	state.ch[state.CB].FD&=0xFF00;
	state.ch[state.CB].FD|=value;
}
void RF5C68::WriteFDH(unsigned char value)
{
	state.ch[state.CB].FD&=0xFF;
	state.ch[state.CB].FD|=(value<<8);
}
void RF5C68::WriteLSL(unsigned char value)
{
	state.ch[state.CB].LS&=0xFF00;
	state.ch[state.CB].LS|=value;
}
void RF5C68::WriteLSH(unsigned char value)
{
	state.ch[state.CB].LS&=0xFF;
	state.ch[state.CB].LS|=(value<<8);
}
void RF5C68::WriteST(unsigned char value)
{
	state.ch[state.CB].ST=value;
	state.ch[state.CB].playPtr=(value<<8);
	state.ch[state.CB].playPtrLeftOver=0;
}

void RF5C68::WriteRegister(unsigned char reg,unsigned char data,uint64_t VMTimeInNS)
{
	if(true==useScheduling)
	{
		WriteRegisterSchedule(reg,data,VMTimeInNS);
		if(REG_CONTROL==reg)
		{
			WriteControl(data);
			if(0==(0x40&data)) // Bank must be switched with no delay.
			{
				auto WB=(data&0x0f);
				state.Bank=WB;
				state.Bank<<=12;
			}
		}
	}
	else
	{
		ReallyWriteRegister(reg,data,VMTimeInNS);
	}
}
RF5C68::StartAndStopChannelBits RF5C68::ReallyWriteRegister(unsigned int reg,unsigned int data,uint64_t)
{
	StartAndStopChannelBits startStop;
	switch(reg)
	{
	case REG_ENV: //0,
		WriteENV(data);
		break;
	case REG_PAN: //1,
		WritePAN(data);
		break;
	case REG_FDL: //2,
		WriteFDL(data);
		break;
	case REG_FDH: //3,
		WriteFDH(data);
		break;
	case REG_LSL: //4,
		WriteLSL(data);
		break;
	case REG_LSH: //5,
		WriteLSH(data);
		break;
	case REG_ST: //6,
		WriteST(data);
		break;
	case REG_CONTROL: //7,
		WriteControl(data);
		break;
	case REG_CH_ON_OFF: //8,
		{
			startStop=WriteChannelOnOff(data);
			if(0!=startStop.chStopPlay)
			{
				for(unsigned int ch=0; ch<NUM_CHANNELS; ++ch)
				{
					if(0!=(startStop.chStopPlay&(1<<ch)))
					{
						PlayStopped(ch);
					}
					if(0!=(startStop.chStartPlay&(1<<ch)))
					{
						state.ch[ch].playPtr=(state.ch[ch].ST<<8);
						state.ch[ch].playPtrLeftOver=0;
					}
				}
			}
		}
		break;
	}
	return startStop;
}
void RF5C68::WriteRegisterSchedule(unsigned int reg,unsigned int value,uint64_t systemTimeInNS)
{
	RegWriteLog rwl;
	rwl.reg=reg;
	rwl.data=value;
	rwl.systemTimeInNS=systemTimeInNS;
	regWriteSched.push_back(rwl);
}
void RF5C68::FlushRegisterSchedule(void)
{
	for(auto sched : regWriteSched)
	{
		ReallyWriteRegister(sched.reg,sched.data,sched.systemTimeInNS);
	}
	regWriteSched.clear();
}

std::vector <std::string> RF5C68::GetStatusText(void) const
{
	std::vector <std::string> text;

	text.push_back("RF5C68");

	for(int ch=0; ch<NUM_CHANNELS; ++ch)
	{
		std::string s;
		s="CH"+cpputil::Ubtox(ch)+":";

		s+="ENV="+cpputil::Ubtox(state.ch[ch].ENV)+" ";
		s+="PAN="+cpputil::Ubtox(state.ch[ch].PAN)+" ";
		s+="ST="+cpputil::Ubtox(state.ch[ch].ST)+" ";
		s+="FD="+cpputil::Ustox(state.ch[ch].FD)+" ";
		s+="LS="+cpputil::Ustox(state.ch[ch].LS)+" ";
		s+="PlayPtr="+cpputil::Ustox(state.ch[ch].playPtr)+" ";
		s+="LeftOver="+cpputil::Ustox(state.ch[ch].playPtrLeftOver);

		text.push_back(s);
	}

	std::string s;
	s="PLAYING=";
	s+=(true==state.playing ? "1 " : "0 ");
	s+="BANK="+cpputil::Ustox(state.Bank)+" ";
	s+="CB="+cpputil::Ubtox(state.CB)+" ";
	s+="CHOnOff="+cpputil::Ubtox(state.chOnOff);
	text.push_back(s);

	s="IRQ=";
	s+=(true==state.IRQ() ? "1 " : "0 ");
	s+="IRQBank="+cpputil::Ubtox(state.IRQBank)+" ";
	s+="IRQBankMask="+cpputil::Ubtox(state.IRQBankMask)+" ";
	text.push_back(s);

	return text;
}

unsigned int RF5C68::MakeWaveForNumSamples(unsigned char waveBuf[],unsigned int numSamples,int outSamplingRate,uint64_t lastWAVGenTime)
{
	std::memset(waveBuf,0,numSamples*4);
	return AddWaveForNumSamples(waveBuf,numSamples,outSamplingRate,lastWAVGenTime);
}

unsigned int RF5C68::AddWaveForNumSamples(unsigned char waveBuf[],unsigned int numSamples,int outSamplingRate,uint64_t lastWAVGenTime)
{
	unsigned int numPlayingCh=0,playingCh[NUM_CHANNELS];
	unsigned int LvolCh[NUM_CHANNELS],RvolCh[NUM_CHANNELS],pcmAddr[NUM_CHANNELS];

	auto Reinit=[&](unsigned int chFlags)
	{
		numPlayingCh=0;
		for(unsigned int chNum=0; chNum<NUM_CHANNELS; ++chNum)
		{
			auto &ch=state.ch[chNum];
			LvolCh[chNum]=(ch.PAN&0x0F);
			RvolCh[chNum]=((ch.PAN>>4)&0x0F);
			LvolCh[chNum]=(LvolCh[chNum]*ch.ENV);
			RvolCh[chNum]=(RvolCh[chNum]*ch.ENV);
			if(0!=(chFlags&(1<<chNum)))
			{
				pcmAddr[chNum]=(ch.playPtr<<FD_BIT_SHIFT)|ch.playPtrLeftOver;
			}
			ch.repeatAfterThisSegment=false;
			if(0<ch.FD && 0==(state.chOnOff&(1<<chNum)))
			{
				playingCh[numPlayingCh++]=chNum;
			}
		}
	};

	Reinit(0xff);

	unsigned int regSchedPtr=0;
	auto VMTime=lastWAVGenTime;

	unsigned int nFilled=0;
	auto wavePtr=waveBuf;
	while(nFilled<numSamples && 0<numPlayingCh)
	{
		while(regSchedPtr<regWriteSched.size())
		{
			if(VMTime<regWriteSched[regSchedPtr].systemTimeInNS)
			{
				break;
			}
			auto startStop=ReallyWriteRegister(regWriteSched[regSchedPtr].reg,regWriteSched[regSchedPtr].data,regWriteSched[regSchedPtr].systemTimeInNS);

			if(0!=(startStop.chStartPlay|startStop.chStopPlay))
			{
				Reinit(startStop.chStartPlay);
			}

			++regSchedPtr;
		}

		int Lout=0,Rout=0;
		for(int i=numPlayingCh-1; 0<=i; --i)
		{
			auto chNum=playingCh[i];

			auto &ch=state.ch[chNum];
			unsigned int Lvol=LvolCh[chNum];
			unsigned int Rvol=RvolCh[chNum];

			auto readAddr=(pcmAddr[chNum]>>FD_BIT_SHIFT);
			auto data=state.waveRAM[readAddr];

			// If the data is LOOP_STOP_CODE, try rewind and then get the data.
			// If the data is still LOOP_STOP_CODE after rewinding, it should be taken as zero.
			// The logic is dirty.  The real chip should be doing much simpler logic.  I need to clean it.
			if(LOOP_STOP_CODE==data)
			{
				if(0xFFF==(readAddr&0xFFF))
				{
					auto bank=((pcmAddr[chNum]>>FD_BIT_SHIFT>>BANK_SHIFT)&0x0F);
					ch.IRQAfterThisPlayBack=true;
					ch.IRQBank=bank;
				}
				pcmAddr[chNum]=(ch.LS<<FD_BIT_SHIFT);
				readAddr=(pcmAddr[chNum]>>FD_BIT_SHIFT);
				data=state.waveRAM[readAddr];
			}

			bool loopStop=(LOOP_STOP_CODE==data);
			if(true!=loopStop)
			{
				int L=(data&0x7F);
				int R=L;
				L*=Lvol;
				R*=Rvol;
				L>>=4;
				R>>=4;
				if(data&0x80)
				{
					L=-L;
					R=-R;
				}

				if(true!=chMute[chNum])
				{
					L=(L*state.volume)/8192;
					R=(R*state.volume)/8192;
					Lout=Gain(Lout,L);
					Rout=Gain(Rout,R);
				}

				auto prevBank=((pcmAddr[chNum]>>FD_BIT_SHIFT>>BANK_SHIFT)&0x0F);
				pcmAddr[chNum]+=ch.FD;
				auto bank=((pcmAddr[chNum]>>FD_BIT_SHIFT>>BANK_SHIFT)&0x0F);
				if(prevBank!=bank)
				{
					ch.IRQAfterThisPlayBack=true;
					ch.IRQBank=prevBank;
					if(0==bank)
					{
						pcmAddr[chNum]=0;
					}
				}

				// If a loop-stop code is between this byte and the next byte, it should be caught.
				for(auto scanAddr=(pcmAddr[chNum]>>FD_BIT_SHIFT);
				    scanAddr<WAVERAM_SIZE && scanAddr<((pcmAddr[chNum]+ch.FD)>>FD_BIT_SHIFT);
				    ++scanAddr)
				{
					if(LOOP_STOP_CODE==state.waveRAM[scanAddr&(WAVERAM_SIZE-1)])
					{
						readAddr=(scanAddr&(WAVERAM_SIZE-1));
						loopStop=true;
						break;
					}
				}
			}

			if(true==loopStop)
			{
				// Should it fire an IRQ on loop-stop?
				// Not firing IRQ on loopStop breaks Strike Commander voice (in fact, it will wait for IRQ forever).
				// Firing IRQ will overrun wave-data of Sim City 2000.
				// Probably, IRQ should be fired when RF5C68 reads the last byte of the bank.  If there is no loop stop, it is
				// just crossing the bank border.
				// But, also it should fire IRQ when the loop stop is at 0x?FFF.  This condition seems to be correct.
				if(0xFFF==(readAddr&0xFFF))
				{
					auto bank=((pcmAddr[chNum]>>FD_BIT_SHIFT>>BANK_SHIFT)&0x0F);
					ch.IRQAfterThisPlayBack=true;
					ch.IRQBank=bank;
				}

				pcmAddr[chNum]=(ch.LS<<FD_BIT_SHIFT);
				if(LOOP_STOP_CODE==state.waveRAM[pcmAddr[chNum]>>FD_BIT_SHIFT]) // Infinite Loop
				{
					playingCh[i]=playingCh[numPlayingCh-1];
					--numPlayingCh;
					goto NEXTCHANNEL;
				}
			}
		NEXTCHANNEL:
			;
		}

		float time = 0;
		float time_add = static_cast<float>(SAMPLING_RATE) / state.timeBalance;

		while(0<=state.timeBalance && nFilled<numSamples)
		{
			//WordOp_Add(wavePtr  ,Lout);
			//WordOp_Add(wavePtr+2,Rout);
			WordOp_Add(wavePtr  ,lerp_int(state.Lout_prev,Lout,time));
			WordOp_Add(wavePtr+2,lerp_int(state.Rout_prev,Rout,time));
			time+=time_add;
			state.timeBalance-=SAMPLING_RATE;
			wavePtr+=4;
			++nFilled;

			if(regSchedPtr<regWriteSched.size())
			{
				VMTime+=1000000000/outSamplingRate;
			}
		}
		state.timeBalance+=outSamplingRate;

		state.Lout_prev=Lout;
		state.Rout_prev=Rout;
	}

	for(unsigned int chNum=0; chNum<NUM_CHANNELS; ++chNum)
	{
		auto &ch=state.ch[chNum];
		ch.playPtr=(pcmAddr[chNum]>>FD_BIT_SHIFT);
		ch.playPtrLeftOver=(pcmAddr[chNum]&((1<<FD_BIT_SHIFT)-1));
	}


	for(auto i=regSchedPtr; i<regWriteSched.size(); ++i)
	{
		ReallyWriteRegister(regWriteSched[i].reg,regWriteSched[i].data,regWriteSched[i].systemTimeInNS);
	}
	regWriteSched.clear();


	return nFilled;
}

bool RF5C68::IsPlaying(void) const
{
	return true==state.playing && 0xFF!=(state.chOnOff&0xFF);
}

void RF5C68::PlayStopped(unsigned int chNum)
{
	auto &ch=state.ch[chNum];
	ch.playPtr=(ch.ST<<8);
	ch.playPtrLeftOver=0;
}

void RF5C68::SetIRQBank(unsigned int bank)
{
	bank>>=1;
	if(0!=((1<<bank)&state.IRQBankMask))
	{
		state.IRQBank|=(1<<bank);
	}
}

void RF5C68::SetUpNextSegment(unsigned int chNum)
{
	auto &ch=state.ch[chNum];
	if(true==ch.repeatAfterThisSegment)
	{
		ch.playPtr=ch.LS;
		ch.playPtrLeftOver=0;
	}
	else
	{
		ch.playPtr+=0x1000;
		ch.playPtr&=0xF000;
		ch.playPtrLeftOver=0;
	}
}
