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

unsigned int Memory::FetchByte(unsigned int addr) const
{
	if(addr<state.RAM.size())
	{
		if(true==state.sysRomMapping && 0xF8000<=addr && addr<=0xFFFFF)
		{
			return sysRom[addr-0xC0000];
		}
		else
		{
			return state.RAM[addr];
		}
	}
	if(0x80000000<=addr && addr<0x80080000)
	{
		return state.VRAM[addr-0x80000000];
	}
	if(0xFFFC0000<=addr)
	{
		return sysRom[addr-0xFFFC0000];
	}
	return 0xFF;
}

/* virtual */ void Memory::Reset(void)
{
	state.sysRomMapping=true;
}

