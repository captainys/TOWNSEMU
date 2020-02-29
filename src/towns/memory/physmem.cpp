#include "physmem.h"
#include "ramrom.h"
#include "cpputil.h"
#include "townsdef.h"
#include "i486.h"
#include "i486debug.h"



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
	DICROMBank=0;
	FMRVRAM=true; // [2] pp.91
	FMRVRAMMask=0x0F; // [2] pp.159
	FMRDisplayMode=0x77; // [2] pp.158
	FMRVRAMWriteOffset=0;
	TVRAMWrite=false;
	kanjiROMAccess.Reset();

	for(auto &c : RAM)
	{
		c=0;
	}
	for(auto &c : VRAM)
	{
		c=0;
	}
	for(auto &c : CVRAM)
	{
		c=0;
	}
	for(auto &c : spriteRAM)
	{
		c=0;
	}
	for(auto &c : waveRAM)
	{
		c=0;
	}
}


////////////////////////////////////////////////////////////


/* virtual */ void TownsPhysicalMemory::IOWriteByte(unsigned int ioport,unsigned int data)
{
	if(TOWNSIO_CMOS_BASE<=ioport && ioport<TOWNSIO_CMOS_END)
	{
		state.DICRAM[(ioport-TOWNSIO_CMOS_BASE)/2]=(unsigned char)(data&0xFF);
		return;
	}

	switch(ioport)
	{
	case TOWNSIO_FMR_VRAM_OR_MAINRAM: // 0x404
		state.FMRVRAM=((0x80&data)==0);
		break;
	case TOWNSIO_SYSROM_DICROM: // 0x480
		state.sysRomMapping=(0==(data&2));
		state.dicRom=(0!=(data&1));
		break;
	case TOWNSIO_DICROM_BANK://              0x484, // [2] pp.92
		state.DICROMBank=data&0x0F;
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
	if(TOWNSIO_CMOS_BASE<=ioport && ioport<TOWNSIO_CMOS_END)
	{
		return state.DICRAM[(ioport-TOWNSIO_CMOS_BASE)/2];
	}

	unsigned char data;
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
	case TOWNSIO_DICROM_BANK://              0x484, // [2] pp.92
		data=state.DICROMBank;
		break;
	case TOWNSIO_TVRAM_WRITE:
		data=(state.TVRAMWrite ? 0xff : 0x00);
		state.TVRAMWrite=false;
		break;
	case TOWNSIO_MEMSIZE:
		return (unsigned int)(state.RAM.size()/(1024*1024));
	case TOWNSIO_FMR_VRAMMASK: // 0xFF81
		return state.FMRVRAMMask;
	}
	return data;
}

TownsPhysicalMemory::TownsPhysicalMemory(class i486DX *cpuPtr,class Memory *memPtr)
{
	takeJISCodeLog=false;
	this->cpuPtr=cpuPtr;
	this->memPtr=memPtr;

	// Just took from my 2MX.
	unsigned char defSerialRom[SERIAL_ROM_LENGTH]=
	{
		0x04,0x65,0x54,0xA4,0x95,0x45,0x35,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0x0C,0x02,0x00,0x00,0x00,0x15,0xE0,0x00,0x00,
	};
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
	auto data=cpputil::ReadBinaryFile(fName);
	if(SERIAL_ROM_LENGTH<=data.size())
	{
		for(int i=0; i<SERIAL_ROM_LENGTH; ++i)
		{
			serialROM[i]=data[i];
		}
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

void TownsPhysicalMemory::SetCVRAMSize(long long int size)
{
	state.CVRAM.resize(size);
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

void TownsPhysicalMemory::SetDICRAMSize(long long int size)
{
	state.DICRAM.resize(size);
	for(auto &d : state.DICRAM)
	{
		d=0;
	}
}

std::vector <std::string> TownsPhysicalMemory::GetStatusText(void) const
{
	std::vector <std::string> text;
	std::string empty;

	text.push_back(empty);
	text.back()="C0000-CFFFF:";
	if(true==state.FMRVRAM)
	{
		text.back()+="FMR VRAM";
	}
	else
	{
		text.back()+="Main RAM";
	}

	text.push_back(empty);
	text.back()="D0000-EFFFF:";
	if(true==state.dicRom)
	{
		text.back()+="Dictionary RAM/ROM, User Font RAM";
	}
	else
	{
		text.back()+="Main RAM";
	}
	text.back()+="  Dictionary Bank:";
	text.back()+=cpputil::Uitox(state.DICROMBank);

	text.push_back(empty);
	text.back()="FF000-FFFFF:";
	if(true==state.sysRomMapping)
	{
		text.back()+="Boot ROM";
	}
	else
	{
		text.back()+="Main RAM";
	}

	return text;
}
