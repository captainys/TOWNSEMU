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
	}
	state.playing=false;
	state.Bank=0;
	state.CB=0;
	state.chOnOff=0xff;  // Active LOW
}

unsigned char RF5C68::WriteControl(unsigned char value)
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

	unsigned char chStartPlay=0xff; // Active LOW
	if(0x80&value)
	{
		if(true!=state.playing)
		{
			chStartPlay=~state.chOnOff;
		}
		state.playing=true;
	}
	else
	{
		state.playing=false;
	}
	return chStartPlay;
}
unsigned char RF5C68::WriteChannelOnOff(unsigned char value)
{
	if(true==state.playing)
	{
		auto chStartPlay=(state.chOnOff&(~value));
		state.chOnOff=value;
		return chStartPlay;
	}
	else
	{
		state.chOnOff=value;
		return 0;
	}
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
}

std::vector <std::string> RF5C68::GetStatusText(void) const
{
	std::vector <std::string> text;
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

	return text;
}

std::vector <unsigned char> RF5C68::Make19KHzWave(unsigned int chNum) const
{
	auto &ch=state.ch[chNum];
	std::vector <unsigned char> wave;

	for(unsigned int startAddr=(ch.ST<<(10+8)); startAddr<(WAVERAM_SIZE<<10); startAddr+=ch.FD)
	{
		auto data=state.waveRAM[startAddr>>10];
		if(0xff==data)
		{
			break;
		}

		char abs=(data&0x7F);
		if(data&0x80)
		{
			abs=-abs;
		}

		wave.push_back(abs);
		wave.push_back(abs);
		wave.push_back(abs);
		wave.push_back(abs);

		if(0xff==data)
		{
			break;
		}
	}

	return wave;
}
