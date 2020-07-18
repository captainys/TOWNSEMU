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



RF5C68::RF5C68()
{
	state.waveRAM.resize(WAVERAM_SIZE);
	Clear();
}

void RF5C68::Clear(void)
{
	for(auto &b : state.waveRAM)
	{
		b=0;
	}
	for(auto &ch : state.ch)
	{
		ch.ENV=0;
		ch.PAN=0;
		ch.ST=0;
		ch.FD=0;
		ch.LS=0;
		ch.IRQTimer=0.0;
		ch.playingBank=0;
		ch.playPtr=0;
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
		state.chOnOff=value;
	}
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
		s+="LS="+cpputil::Ustox(state.ch[ch].LS);

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

std::vector <unsigned char> RF5C68::Make19KHzWave(unsigned int chNum)
{
	auto &ch=state.ch[chNum];
	std::vector <unsigned char> wave;

	unsigned int Lvol=(ch.PAN&0x0F);
	unsigned int Rvol=((ch.PAN>>4)&0x0F);
	Lvol=(Lvol*ch.ENV)>>4;
	Rvol=(Rvol*ch.ENV)>>4;

	ch.repeatAfterThisSegment=false;
	if(0<ch.FD)
	{
		unsigned int endPtr=((ch.playPtr+0x1000)&(~0xfff));
		unsigned int startAddr=(ch.playPtr<<FD_BIT_SHIFT);
		unsigned int endAddr=(endPtr<<FD_BIT_SHIFT);
		unsigned int count=(endAddr-startAddr+ch.FD-1)/ch.FD;
		wave.resize(count*4);
		auto wavePtr=wave.data();
		for(unsigned int pcmAddr=startAddr; pcmAddr<endAddr; pcmAddr+=ch.FD)
		{
			auto data=state.waveRAM[pcmAddr>>FD_BIT_SHIFT];

			// 2020/07/18
			// Adding ch.FD to pcmAddr may skip a byte.
			// However, it should not skip a loop-stop byte.
			for(auto scanAddr=(pcmAddr>>FD_BIT_SHIFT); 
			    scanAddr<WAVERAM_SIZE &&
			    scanAddr<((pcmAddr+ch.FD)>>FD_BIT_SHIFT); 
			    ++scanAddr)
			{
				if(0xff==state.waveRAM[scanAddr])
				{
					ch.repeatAfterThisSegment=true;
					goto ENDLOOP;
				}
			}

			int L=(data&0x7F);
			int R=L;
			L*=Lvol;
			R*=Rvol;
			if(data&0x80)
			{
				L=-L;
				R=-R;
			}

			wavePtr[0]=(L&0xFF);
			wavePtr[1]=((L>>8)&0xFF);
			wavePtr[2]=(R&0xFF);
			wavePtr[3]=((R>>8)&0xFF);
			wavePtr+=4;
		}
	ENDLOOP:
		;
	}

	return wave;
}

unsigned int RF5C68::MakeWaveForNumSamples(unsigned char waveBuf[],unsigned int chNum,unsigned int numSamples)
{
	std::memset(waveBuf,0,numSamples*4);
	return AddWaveForNumSamples(waveBuf,chNum,numSamples);
}
unsigned int RF5C68::AddWaveForNumSamples(unsigned char waveBuf[],unsigned int chNum,unsigned int numSamples)
{
	unsigned int nFilled=0;

	auto &ch=state.ch[chNum];

	unsigned int Lvol=(ch.PAN&0x0F);
	unsigned int Rvol=((ch.PAN>>4)&0x0F);
	Lvol=(Lvol*ch.ENV)>>4;
	Rvol=(Rvol*ch.ENV)>>4;

	ch.repeatAfterThisSegment=false;
	if(0<ch.FD)
	{
		unsigned int pcmAddr=(ch.playPtr<<FD_BIT_SHIFT);;
		auto wavePtr=waveBuf;

		while(nFilled<numSamples)
		{
			auto data=state.waveRAM[pcmAddr>>FD_BIT_SHIFT];

			bool loopStop=(LOOP_STOP_CODE==data);
			if(true!=loopStop)
			{
				int L=(data&0x7F);
				int R=L;
				L*=Lvol;
				R*=Rvol;
				if(data&0x80)
				{
					L=-L;
					R=-R;
				}

				wavePtr[0]=(L&0xFF);
				wavePtr[1]=((L>>8)&0xFF);
				wavePtr[2]=(R&0xFF);
				wavePtr[3]=((R>>8)&0xFF);
				wavePtr+=4;

				++nFilled;

				auto prevBank=((pcmAddr>>FD_BIT_SHIFT>>BANK_SHIFT)&0x0F);
				pcmAddr+=ch.FD;
				auto bank=((pcmAddr>>FD_BIT_SHIFT>>BANK_SHIFT)&0x0F);
				if(prevBank!=bank)
				{
					ch.IRQAfterThisPlayBack=true;
					ch.IRQBank=prevBank;
					if(0==bank)
					{
						pcmAddr=0;
					}
				}

				// If a loop-stop code is between this byte and the next byte, it should be caught.
				for(auto scanAddr=(pcmAddr>>FD_BIT_SHIFT);
	   			    scanAddr<WAVERAM_SIZE && scanAddr<((pcmAddr+ch.FD)>>FD_BIT_SHIFT);
				    ++scanAddr)
				{
					if(LOOP_STOP_CODE==state.waveRAM[scanAddr])
					{
						loopStop=true;
						break;
					}
				}
			}

			if(true==loopStop)
			{
				pcmAddr=(ch.LS<<FD_BIT_SHIFT);
				if(LOOP_STOP_CODE==state.waveRAM[pcmAddr>>FD_BIT_SHIFT]) // Infinite Loop
				{
					break;
				}
			}
		}

		ch.playPtr=(pcmAddr>>FD_BIT_SHIFT);
	}
	return nFilled;
}

void RF5C68::PlayStarted(unsigned int chNum)
{
	auto &ch=state.ch[chNum];
	ch.playingBank=(ch.playPtr>>12);

	// How long does it take to play 4K samples?
	const unsigned int len=(4096<<FD_BIT_SHIFT);
	unsigned int FD=ch.FD;
	if(0==FD)
	{
		FD=1;
	}
	ch.IRQTimer=(double)len/(double)(ch.FD*FREQ);
}

void RF5C68::PlayStopped(unsigned int chNum)
{
	auto &ch=state.ch[chNum];
	ch.playPtr=(ch.ST<<8);
}

void RF5C68::SetIRQ(unsigned int chNum)
{
	auto &ch=state.ch[chNum];
	auto bank=(ch.playingBank>>1);
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
	}
	else
	{
		ch.playPtr+=0x1000;
		ch.playPtr&=0xF000;
	}
}
