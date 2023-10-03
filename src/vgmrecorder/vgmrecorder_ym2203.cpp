#include "vgmrecorder.h"
#include "ym2612.h"

void VGMRecorder::CaptureYM2203InitialCondition(uint64_t VMTime,const class YM2612 &ym2612,const unsigned char ay8910part[16])
{
	for(unsigned int reg=0; reg<16; ++reg)
	{
		WriteRegister(VMTime,REG_YM2203,reg,ay8910part[reg]);
	}
	for(unsigned int reg=0x22; reg<=0x28; ++reg)
	{
		WriteRegister(VMTime,REG_YM2203,reg,ym2612.state.regSet[0][reg]);
	}
	for(unsigned int reg=0x30; reg<=0xB0; ++reg)
	{
		WriteRegister(VMTime,REG_YM2203,reg,ym2612.state.regSet[0][reg]);
	}
}
