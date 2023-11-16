#include "vgmrecorder.h"
#include "rf5c68.h"

void VGMRecorder::CaptureRF5C68InitialCondition(uint64_t VMTime,const class RF5C68 &rf5c68)
{
	WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CH_ON_OFF,0xFF);
	for(int bank=0; bank<0x10; ++bank)
	{
		unsigned char data=(true==rf5c68.state.playing ? 0x80 : 0);
		data|=bank;
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CONTROL,data);
		WritePCMMemoryBlock(VMTime,MEM_RF5C68,0,0x1000,rf5c68.state.waveRAM.data()+(bank<<12));
	}

	for(int ch=0; ch<RF5C68::NUM_CHANNELS; ++ch)
	{
		unsigned char data=(true==rf5c68.state.playing ? 0x80 : 0);
		data|=0x40; // MOD=CB
		data|=ch;
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CONTROL,data);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_ENV,rf5c68.state.ch[ch].ENV);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_PAN,rf5c68.state.ch[ch].PAN);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_FDL,rf5c68.state.ch[ch].FD&0xFF);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_FDH,(rf5c68.state.ch[ch].FD>>8)&0xFF);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_LSL,rf5c68.state.ch[ch].LS&0xFF);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_LSH,(rf5c68.state.ch[ch].LS>>8)&0xFF);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_ST,rf5c68.state.ch[ch].ST);
	}
	WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CH_ON_OFF,rf5c68.state.chOnOff);

	unsigned char data=(true==rf5c68.state.playing ? 0x80 : 0);
	data|=0x40; // MOD=CB
	data|=rf5c68.state.CB;
	WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CONTROL,data);

	data=(true==rf5c68.state.playing ? 0x80 : 0);
	data|=rf5c68.state.Bank; // MOD=WB
	WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CONTROL,data);
}
