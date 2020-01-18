#include "towns.h"



FMTowns::FMTowns()
{
	abort=false;
	allDevices.push_back(&io);
	allDevices.push_back(&mem);

	mem.SetMainRAMSize(4*1024*1024);

	// The values may be wrong.  I vaguely remember
	mem.SetVRAMSize(1024*1024);
	mem.SetSpriteRAMSize(512*1024);
	mem.SetWaveRAMSize(256*1024);

	Reset();
}

bool FMTowns::LoadROMImages(const char dirName[])
{
	if(true!=mem.LoadROMImages(dirName))
	{
		abort=true;
		abortReason="Unable to load ROM images.";
		return false;
	}
	return true;
}

void FMTowns::Reset(void)
{
	cpu.Reset();
	for(auto devPtr : allDevices)
	{
		devPtr->Reset();
	}
}

unsigned int FMTowns::FetchByteCS_EIP(int offset) const
{
	return cpu.FetchInstructionByte(offset,mem);
}

i486DX::Instruction FMTowns::FetchInstruction(void) const
{
	return cpu.FetchInstruction(mem);
}
