#include "vgmrecorder.h"
#include "ym2612.h"

void VGMRecorder::CaptureYM2612InitialCondition(uint64_t VMTime,const class YM2612 &ym2612)
{
	for(unsigned int reg=0x22; reg<=0x28; ++reg)
	{
		WriteRegister(VMTime,REG_YM2612_CH0,reg,ym2612.state.regSet[0][reg]);
	}
	for(unsigned int reg=0x30; reg<=0xB0; ++reg)
	{
		WriteRegister(VMTime,REG_YM2612_CH0,reg,ym2612.state.regSet[0][reg]);
	}
	for(unsigned int reg=0x30; reg<=0xB0; ++reg)
	{
		WriteRegister(VMTime,REG_YM2612_CH3,reg,ym2612.state.regSet[1][reg]);
	}
}
