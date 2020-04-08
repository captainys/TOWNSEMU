#include "rf5c68.h"



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
	state.chOnOff=0;
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

	unsigned char chStartPlay=0;
	if(0x80&value)
	{
		if(true!=state.playing)
		{
			chStartPlay=state.chOnOff;
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
		auto chStartPlay=((~state.chOnOff)&value);
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
