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
	dicRom=false;
	FMRVRAM=true; // [2] pp.91
	FMRVRAMMask=0x0F; // [2] pp.159
	FMRDisplayMode=0x77; // [2] pp.158
	FMRVRAMWriteOffset=0;
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
	case TOWNSIO_SYSROM_DICROM: // 0x480
		state.sysRomMapping=(0==(data&2));
		state.dicRom=(0!=(data&1));
		break;
	case TOWNSIO_FMR_VRAMMASK: // 0xFF81
		state.FMRVRAMMask=data;
		break;
	case TOWNSIO_FMR_VRAMDISPLAYMODE:
		state.FMRDisplayMode=data;
		break;
	case TOWNSIO_FMR_VRAMPAGESEL:
		state.FMRVRAMWriteOffset=(0!=(data&0x10) ? 0x40000 : 0);
		break;
	}
}
/* virtual */ unsigned int TownsPhysicalMemory::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_FMR_VRAM_OR_MAINRAM: // 0x404
		return (true==state.FMRVRAM ? 0 : 0x80);
	case TOWNSIO_SYSROM_DICROM: // 0x480
		{
			unsigned char byteData=0;
			if(true!=state.sysRomMapping)
			{
				byteData|=2;
			}
			if(true==state.dicRom)
			{
				byteData|=1;
			}
			return byteData;
		}
		break;
	case TOWNSIO_MEMSIZE:
		return (unsigned int)(state.RAM.size()/(1024*1024));
	case TOWNSIO_FMR_VRAMMASK: // 0xFF81
		return state.FMRVRAMMask;
	}
	return 0xFF;
}

TownsPhysicalMemory::TownsPhysicalMemory()
{
	takeJISCodeLog=false;
	state.Reset();
}

bool TownsPhysicalMemory::LoadROMImages(const char dirName[])
{
	std::string fName;
	fName=cpputil::MakeFullPathName(dirName,"FMT_SYS.ROM");
	sysRom=cpputil::ReadBinaryFile(fName);
	if(256*1024!=sysRom.size())
	{
		Abort("Cannot read FMT_SYS.ROM or incorrect file size.");
		return false;
	}

	fName=cpputil::MakeFullPathName(dirName,"FMT_DOS.ROM");
	dosRom=cpputil::ReadBinaryFile(fName);
	if(512*1024!=dosRom.size())
	{
		Abort("Cannot read FMT_DOS.ROM or incorrect file size.");
		return false;
	}

	fName=cpputil::MakeFullPathName(dirName,"FMT_FNT.ROM");
	fontRom=cpputil::ReadBinaryFile(fName);
	if(256*1024!=fontRom.size())
	{
		Abort("Cannot read FMT_FNT.ROM or incorrect file size.");
		return false;
	}

	fName=cpputil::MakeFullPathName(dirName,"FMT_F20.ROM");
	font20Rom=cpputil::ReadBinaryFile(fName);
	if(512*1024!=font20Rom.size())
	{
		Abort("Cannot read FMT_F20.ROM or incorrect file size.");
		return false;
	}

	fName=cpputil::MakeFullPathName(dirName,"FMT_DIC.ROM");
	dicRom=cpputil::ReadBinaryFile(fName);
	if(512*1024!=dicRom.size())
	{
		Abort("Cannot read FMT_DIC.ROM or incorrect file size.");
		return false;
	}

	fName=cpputil::MakeFullPathName(dirName,"MYTOWNS.ROM");
	serialRom=cpputil::ReadBinaryFile(fName);

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

