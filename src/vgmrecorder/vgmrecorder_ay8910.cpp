#include "vgmrecorder.h"
#include "ym2612.h"

void VGMRecorder::CaptureAY8910InitialCondition(uint64_t VMTime,const unsigned char regs[16])
{
	for(unsigned int reg=0; reg<16; ++reg)
	{
		WriteRegister(VMTime,REG_AY8910,reg,regs[reg]);
	}
}
