#include "physmem.h"
#include "ramrom.h"
#include "cpputil.h"
#include "townsdef.h"



void TownsPhysicalMemory::KanjiROMAccess::Reset()
{
	JISCodeHigh=0;
	JISCodeLow=0;
	row=0;
}


////////////////////////////////////////////////////////////


void TownsPhysicalMemory::State::Reset(void)
{
	sysRomMapping=true;
	FMRVRAM=true; // [2] pp.91
	kanjiROMAccess.Reset();
}


////////////////////////////////////////////////////////////


/* virtual */ void TownsPhysicalMemory::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_FMR_VRAM_OR_MAINRAM: // 0x404
		state.FMRVRAM=((0x80&data)==0);
		break;
	}
}
/* virtual */ unsigned int TownsPhysicalMemory::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_FMR_VRAM_OR_MAINRAM: // 0x404
		return (true==state.FMRVRAM ? 0 : 0x80);
	}
}


////////////////////////////////////////////////////////////


TownsPhysicalMemory::TownsPhysicalMemory()
{
	takeJISCodeLog=false;
}

bool TownsPhysicalMemory::LoadROMImages(const char dirName[])
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

void TownsPhysicalMemory::SetMainRAMSize(long long int size)
{
	state.RAM.resize(size);
}

void TownsPhysicalMemory::SetVRAMSize(long long int size)
{
	state.VRAM.resize(size);
}

void TownsPhysicalMemory::SetSpriteRAMSize(long long int size)
{
	state.spriteRAM.resize(size);
}

void TownsPhysicalMemory::SetWaveRAMSize(long long int size)
{
	state.waveRAM.resize(size);
}

/* virtual */ void TownsPhysicalMemory::Reset(void)
{
	state.Reset();
}

