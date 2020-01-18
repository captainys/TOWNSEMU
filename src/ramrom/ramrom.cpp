#include "ramrom.h"
#include "cpputil.h"



bool Memory::LoadROMImages(const char dirName[])
{
	std::string fName;
	fName=cpputil::MakeFullPathName(dirName,"FMT_SYS.ROM");
	sysRom=cpputil::ReadBinaryFile(fName);
	if(0==sysRom.size())
	{
		Abort("Cannot read FMT_SYS.ROM");
		return false;
	}

	return true;
}

void Memory::SetMainRAMSize(long long int size)
{
	state.RAM.resize(size);
}

void Memory::SetVRAMSize(long long int size)
{
	state.VRAM.resize(size);
}

void Memory::SetSpriteRAMSize(long long int size)
{
	state.spriteRAM.resize(size);
}

void Memory::SetWaveRAMSize(long long int size)
{
	state.waveRAM.resize(size);
}

/* virtual */ void Memory::Reset(void)
{
	state.sysRomMapping=true;
}

