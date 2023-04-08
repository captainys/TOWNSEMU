/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>

#include "physmem.h"
#include "ramrom.h"
#include "cpputil.h"
#include "towns.h"
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
	FMRVRAMWriteOffset=0;
	TVRAMWrite=false;
	ANKFont=false;
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
	for(auto &c : notUsed)
	{
		c=0;
	}
}


////////////////////////////////////////////////////////////


/* virtual */ void TownsPhysicalMemory::IOWriteByte(unsigned int ioport,unsigned int data)
{
	if(TOWNSIO_CMOS_BASE<=ioport && ioport<TOWNSIO_CMOS_END)
	{
		/* I'll try again.
		if(true==preventCMOSInitToSingleDriveMode)
		{
			if(0x328C==ioport)
			{
				std::cout << "Blocking Single Drive Mode " << cpputil::Ubtox(data) << "->" << "00H" << std::endl;
				data=0;
			}
			if(0x33CE==ioport)
			{
				std::cout << "Blocking Single Drive Mode " << cpputil::Ubtox(data) << "->" << "FAH" << std::endl;
				data=0xFA;
			}
		} */
		state.CMOSRAM[(ioport-TOWNSIO_CMOS_BASE)/2]=(unsigned char)(data&0xFF);
		return;
	}

	switch(ioport)
	{
	case TOWNSIO_FMR_VRAM_OR_MAINRAM: // 0x404
		UpdateFMRVRAMMappingFlag((0x80&data)==0);
		break;
	case TOWNSIO_SYSROM_DICROM: // 0x480
		UpdateSysROMDicROMMappingFlag(0==(data&2),0!=(data&1));
		break;
	case TOWNSIO_DICROM_BANK://              0x484, // [2] pp.92
		state.DICROMBank=data&0x0F;
		break;
	case TOWNSIO_FMR_VRAMMASK: // 0xFF81
		state.FMRVRAMMask=data;
		break;
	case TOWNSIO_FMR_VRAMDISPLAYMODE: // 0xFF82
		FMRVRAMAccess.crtcPtr->MEMIOWriteFMRVRAMDisplayMode(data);
		break;
	case TOWNSIO_FMR_VRAMPAGESEL:
		state.FMRVRAMWriteOffset=(0!=(data&0x10) ? TOWNS_FMRMODE_VRAM_OFFSET : 0);
		break;

	case TOWNSIO_VRAMACCESSCTRL_ADDR: //      0x458, // [2] pp.17,pp.112
		state.nativeVRAMMaskRegisterLatch=data&1; // [2] pp.112 suggests lower 2 bits are effectve, but actually just 1.
		break;
	case TOWNSIO_VRAMACCESSCTRL_DATA_LOW: //  0x45A, // [2] pp.17,pp.112
		{
			auto prevMask=cpputil::GetDword(state.nativeVRAMMask);
			state.nativeVRAMMask[(state.nativeVRAMMaskRegisterLatch<<1)  ]=data&255;
			state.nativeVRAMMask[(state.nativeVRAMMaskRegisterLatch<<1)+4]=data&255;
			auto newMask=cpputil::GetDword(state.nativeVRAMMask);
			if(prevMask!=newMask && (0xffffffff==prevMask || 0xffffffff==newMask))
			{
				EnableOrDisableNativeVRAMMask();
			}
		}
		break;
	case TOWNSIO_VRAMACCESSCTRL_DATA_HIGH: // 0x45B, // [2] pp.17,pp.112
		{
			auto prevMask=cpputil::GetDword(state.nativeVRAMMask);
			state.nativeVRAMMask[(state.nativeVRAMMaskRegisterLatch<<1)+1]=data&255;
			state.nativeVRAMMask[(state.nativeVRAMMaskRegisterLatch<<1)+5]=data&255;
			auto newMask=cpputil::GetDword(state.nativeVRAMMask);
			if(prevMask!=newMask && (0xffffffff==prevMask || 0xffffffff==newMask))
			{
				EnableOrDisableNativeVRAMMask();
			}
		}
		break;

	case TOWNSIO_KANJI_JISCODE_HIGH://  0xFF94,
		state.kanjiROMAccess.JISCodeHigh=data&0x7F;
		break;
	case TOWNSIO_KANJI_JISCODE_LOW://   0xFF95,
		state.kanjiROMAccess.JISCodeLow=data;
		state.kanjiROMAccess.row=0;
		break;
	case TOWNSIO_KANJI_PTN_HIGH://      0xFF96,
		// Write access enabled? [2] pp.95
		break;
	case TOWNSIO_KANJI_PTN_LOW://       0xFF97,
		// Write access enabled? [2] pp.95
		break;
	case TOWNSIO_KVRAM_OR_ANKFONT://    0xFF99,
		state.ANKFont=(0!=(data&1));
		break;

	case TOWNSIO_MEMCARD_STATUS: //           0x48A, // [2] pp.93
		break;
	case TOWNSIO_MEMCARD_BANK: //             0x490, // [2] pp.794
		if(TOWNSCPU_80386SX!=townsPtr->GetCPUType())
		{
			state.memCardBank=((data>>4)&3);
		}
		else
		{
			state.memCardBank=(data&0x3F);
		}
		break;
	case TOWNSIO_MEMCARD_ATTRIB: //           0x491, // [2] pp.795
		state.memCardREG=(0!=(data&1));
		break;
	}
}
/* virtual */ unsigned int TownsPhysicalMemory::IOReadByte(unsigned int ioport)
{
	if(TOWNSIO_CMOS_BASE<=ioport && ioport<TOWNSIO_CMOS_END)
	{
		return state.CMOSRAM[(ioport-TOWNSIO_CMOS_BASE)/2];
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
	case TOWNSIO_VRAMACCESSCTRL_ADDR: //      0x458, // [2] pp.17,pp.112
		return state.nativeVRAMMaskRegisterLatch;
	case TOWNSIO_VRAMACCESSCTRL_DATA_LOW: //  0x45A, // [2] pp.17,pp.112
		return state.nativeVRAMMask[(state.nativeVRAMMaskRegisterLatch<<1)  ];
	case TOWNSIO_VRAMACCESSCTRL_DATA_HIGH: // 0x45B, // [2] pp.17,pp.112
		return state.nativeVRAMMask[(state.nativeVRAMMaskRegisterLatch<<1)+1];

	case TOWNSIO_KANJI_JISCODE_HIGH://  0xFF94,
		break;
	case TOWNSIO_KANJI_JISCODE_LOW://   0xFF95,
		break;
	case TOWNSIO_KANJI_PTN_HIGH://      0xFF96,
		{
			auto ROMCode=(state.kanjiROMAccess.FontROMCode())&8191;
			return fontRom[32*ROMCode+state.kanjiROMAccess.row*2];
		}
		break;
	case TOWNSIO_KANJI_PTN_LOW://       0xFF97,
		if(true==takeJISCodeLog && 0==state.kanjiROMAccess.row)
		{
			JISCodeLog.push_back(state.kanjiROMAccess.JISCodeHigh);
			JISCodeLog.push_back(state.kanjiROMAccess.JISCodeLow);
		}
		{
			auto ROMCode=state.kanjiROMAccess.FontROMCode()&8191;
			auto byteData=fontRom[32*ROMCode+state.kanjiROMAccess.row*2+1];
			state.kanjiROMAccess.row=(state.kanjiROMAccess.row+1)&0x0F;
			return byteData;
		}
		break;
	case TOWNSIO_KVRAM_OR_ANKFONT://    0xFF99,
		break;

	case TOWNSIO_MEMCARD_STATUS: //           0x48A, // [2] pp.93
		data=(true==state.memCard.changed ? 0x80 : 0);
		state.memCard.changed=false;
		// data|=0x20 if low battery
		// data|=0x10 if battery needs to be changed
		data|=(0==state.memCard.data.size() ? 6 : 0);
		data|=(state.memCard.writeProtected ? 1 : 0);
		break;
	case TOWNSIO_MEMCARD_BANK: //             0x490, // [2] pp.794
		if(TOWNSCPU_80386SX!=townsPtr->GetCPUType())
		{
			data=((state.memCardBank&3)<<4);
		}
		else
		{
			data=state.memCardBank;
		}
		break;
	case TOWNSIO_MEMCARD_ATTRIB: //           0x491, // [2] pp.795
		data=(TOWNS_MEMCARD_TYPE_JEIDA4==state.memCard.memCardType ? 0 : 0x80);
		data|=(state.memCardREG ? 1 : 0);
		break;
	}
	return data;
}

TownsPhysicalMemory::TownsPhysicalMemory(class FMTowns *townsPtr,class i486DX *cpuPtr,class Memory *memPtr,class RF5C68 *pcmPtr) : 
	Device(townsPtr),
	waveRAMAccess(pcmPtr),
	oldMemCardAccess(townsPtr),
	JEIDA4MemCardAccess(townsPtr)
{
	this->townsPtr=townsPtr;

	takeJISCodeLog=false;
	this->cpuPtr=cpuPtr;
	this->memPtr=memPtr;

	for(auto &b : state.CMOSRAM)
	{
		b=0;
	}

	// Just took from my 2MX.
	unsigned char defSerialROM[SERIAL_ROM_LENGTH]=
	{
		0x04,0x65,0x54,0xA4,0x95,0x45,0x35,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0x0C,0x02,0x00,0x00,0x00,0x15,0xE0,0x00,0x00,
	};
	for(unsigned int i=0; i<SERIAL_ROM_LENGTH; ++i)
	{
		serialROM[i]=defSerialROM[i];
	}
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
		std::cout << "Cannot read FMT_F20.ROM or incorrect file size." << std::endl;
		std::cout << "Filling with all 0FFHs." << std::endl;
		font20Rom.resize(512*1024);
		for(auto &b : font20Rom)
		{
			b=0xff;
		}
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

	fName=cpputil::MakeFullPathName(dirName,"MAR_EX0.ROM");
	auto mar0=cpputil::ReadBinaryFile(fName);
	fName=cpputil::MakeFullPathName(dirName,"MAR_EX1.ROM");
	auto mar1=cpputil::ReadBinaryFile(fName);
	fName=cpputil::MakeFullPathName(dirName,"MAR_EX2.ROM");
	auto mar2=cpputil::ReadBinaryFile(fName);
	fName=cpputil::MakeFullPathName(dirName,"MAR_EX3.ROM");
	auto mar3=cpputil::ReadBinaryFile(fName);

	if(512*1024==mar0.size() &&
	   512*1024==mar1.size() &&
	   512*1024==mar2.size() &&
	   512*1024==mar3.size())
	{
		martyRom.resize(2048*1024);
		size_t ptr=0;
		for(auto d : mar0)
		{
			martyRom[ptr++]=d;
		}
		for(auto d : mar1)
		{
			martyRom[ptr++]=d;
		}
		for(auto d : mar2)
		{
			martyRom[ptr++]=d;
		}
		for(auto d : mar3)
		{
			martyRom[ptr++]=d;
		}
	}
	else
	{
		martyRom.clear();
	}

	return true;
}

void TownsPhysicalMemory::SetCMOS(const std::vector <unsigned char> &cmos)
{
	for(unsigned int i=0; i<TOWNS_CMOS_SIZE && i<cmos.size(); ++i)
	{
		state.CMOSRAM[i]=cmos[i];
	}
}

void TownsPhysicalMemory::SetMainRAMSize(long long int size)
{
	size&=0xFFF00000;
	if(size<0x100000) // Minimum 1MB.
	{
		size=0x100000;
	}
	state.RAM.resize(size);
}

void TownsPhysicalMemory::SetVRAMSize(long long int size)
{
	if(TOWNS_VRAM_SIZE!=size)
	{
		std::cout << "VRAM size is now fixed." << std::endl;
		exit(1);
	}
}

void TownsPhysicalMemory::SetCVRAMSize(long long int size)
{
	state.CVRAM.resize(size);
}

void TownsPhysicalMemory::SetSpriteRAMSize(long long int size)
{
	if(TOWNS_SPRITERAM_SIZE!=size)
	{
		std::cout << "Sprite RAM size is fixed." << std::endl;
		exit(1);
	}
}

void TownsPhysicalMemory::SetDummySize(long long int size)
{
	state.notUsed.resize(size);
}

/* virtual */ void TownsPhysicalMemory::Reset(void)
{
	state.Reset();
	ResetSysROMDicROMMappingFlag(state.sysRomMapping,state.dicRom);
	ResetFMRVRAMMappingFlag(state.FMRVRAM);

}

void TownsPhysicalMemory::SetUpMemoryAccess(unsigned int townsType,unsigned int cpuType)
{
	auto &mem=*memPtr;
	auto &cpu=*cpuPtr;

	mem.CleanUp();

	mainRAMAccess.SetPhysicalMemoryPointer(this);
	mainRAMAccess.SetCPUPointer(&cpu);
	mem.AddAccess(&mainRAMAccess,0x00000000,0x000FFFFF);

	FMRVRAMAccess.SetPhysicalMemoryPointer(this);
	FMRVRAMAccess.SetCPUPointer(&cpu);
	ResetFMRVRAMMappingFlag(true);  // This will set up memory access for 0xC0000 to 0xCFFFF

	mappedDicROMandDicRAMAccess.SetPhysicalMemoryPointer(this);
	mappedDicROMandDicRAMAccess.SetCPUPointer(&cpu);

	nativeDicROMAccess.SetPhysicalMemoryPointer(this);
	nativeDicROMAccess.SetCPUPointer(&cpu);
	nativeCMOSRAMAccess.SetPhysicalMemoryPointer(this);
	nativeCMOSRAMAccess.SetCPUPointer(&cpu);

	mappedSysROMAccess.SetPhysicalMemoryPointer(this);
	mappedSysROMAccess.SetCPUPointer(&cpu);
	ResetSysROMDicROMMappingFlag(true,false);   // This will set up memory access for 0xF8000 to 0xFFFFF and 0xD0000 to 0xDFFFF

	if(0x00100000<state.RAM.size())
	{
		mem.AddAccess(&mainRAMAccess,0x00100000,(unsigned int)state.RAM.size()-1);
	}

	VRAMAccess0.SetPhysicalMemoryPointer(this);
	VRAMAccess0.SetCPUPointer(&cpu);
	VRAMAccess1.SetPhysicalMemoryPointer(this);
	VRAMAccess1.SetCPUPointer(&cpu);
	VRAMAccessHighRes0.SetPhysicalMemoryPointer(this);
	VRAMAccessHighRes0.SetCPUPointer(&cpu);
	VRAMAccessHighRes1.SetPhysicalMemoryPointer(this);
	VRAMAccessHighRes1.SetCPUPointer(&cpu);
	VRAMAccessHighRes2.SetPhysicalMemoryPointer(this);
	VRAMAccessHighRes2.SetCPUPointer(&cpu);

	VRAMAccessWithMask0.SetPhysicalMemoryPointer(this);
	VRAMAccessWithMask0.SetCPUPointer(&cpu);
	VRAMAccessWithMask1.SetPhysicalMemoryPointer(this);
	VRAMAccessWithMask1.SetCPUPointer(&cpu);
	VRAMAccessWithMaskHighRes0.SetPhysicalMemoryPointer(this);
	VRAMAccessWithMaskHighRes0.SetCPUPointer(&cpu);
	VRAMAccessWithMaskHighRes1.SetPhysicalMemoryPointer(this);
	VRAMAccessWithMaskHighRes1.SetCPUPointer(&cpu);
	VRAMAccessWithMaskHighRes2.SetPhysicalMemoryPointer(this);
	VRAMAccessWithMaskHighRes2.SetCPUPointer(&cpu);

	VRAMAccess0Debug.SetPhysicalMemoryPointer(this);
	VRAMAccess0Debug.SetCPUPointer(&cpu);
	VRAMAccess1Debug.SetPhysicalMemoryPointer(this);
	VRAMAccess1Debug.SetCPUPointer(&cpu);
	VRAMAccessHighRes0Debug.SetPhysicalMemoryPointer(this);
	VRAMAccessHighRes0Debug.SetCPUPointer(&cpu);
	VRAMAccessHighRes1Debug.SetPhysicalMemoryPointer(this);
	VRAMAccessHighRes1Debug.SetCPUPointer(&cpu);
	VRAMAccessHighRes2Debug.SetPhysicalMemoryPointer(this);
	VRAMAccessHighRes2Debug.SetCPUPointer(&cpu);
	SetUpVRAMAccess(cpuType,false,false);

	spriteRAMAccess.SetPhysicalMemoryPointer(this);
	spriteRAMAccess.SetCPUPointer(&cpu);

	oldMemCardAccess.SetPhysicalMemoryPointer(this);
	oldMemCardAccess.SetCPUPointer(&cpu);

	JEIDA4MemCardAccess.SetPhysicalMemoryPointer(this);
	JEIDA4MemCardAccess.SetCPUPointer(&cpu);

	osROMAccess.SetPhysicalMemoryPointer(this);
	osROMAccess.SetCPUPointer(&cpu);

	fontROMAccess.SetPhysicalMemoryPointer(this);
	fontROMAccess.SetCPUPointer(&cpu);

	font20ROMAccess.SetPhysicalMemoryPointer(this);
	font20ROMAccess.SetCPUPointer(&cpu);

	waveRAMAccess.SetPhysicalMemoryPointer(this);
	waveRAMAccess.SetCPUPointer(&cpu);

	sysROMAccess.SetPhysicalMemoryPointer(this);
	sysROMAccess.SetCPUPointer(&cpu);

	martyROMAccess.SetPhysicalMemoryPointer(this);
	martyROMAccess.SetCPUPointer(&cpu);

	mem.AddAccess(&nativeDicROMAccess,TOWNSADDR_NATIVE_DICROM_BASE,TOWNSADDR_NATIVE_DICROM_END-1);
	mem.AddAccess(&nativeCMOSRAMAccess,TOWNSADDR_NATIVE_CMOSRAM_BASE,TOWNSADDR_NATIVE_CMOSRAM_END-1);
	mem.AddAccess(&spriteRAMAccess,TOWNSADDR_SPRITERAM_BASE,TOWNSADDR_SPRITERAM_END-1);
	mem.AddAccess(&oldMemCardAccess,TOWNSADDR_MEMCARD_OLD_BASE,TOWNSADDR_MEMCARD_OLD_END-1);
	mem.AddAccess(&JEIDA4MemCardAccess,TOWNSADDR_MEMCARD_JEIDA4_BASE,TOWNSADDR_MEMCARD_JEIDA4_END-1);
	mem.AddAccess(&osROMAccess,TOWNSADDR_OSROM_BASE,TOWNSADDR_OSROM_END-1);
	mem.AddAccess(&fontROMAccess,TOWNSADDR_FONT_BASE,TOWNSADDR_FONT_END-1);
	mem.AddAccess(&font20ROMAccess,TOWNSADDR_FONT20_BASE,TOWNSADDR_FONT20_END-1);
	mem.AddAccess(&waveRAMAccess,TOWNSADDR_WAVERAM_WINDOW_BASE,TOWNSADDR_WAVERAM_WINDOW_END-1);
	mem.AddAccess(&sysROMAccess,TOWNSADDR_SYSROM_BASE,0xFFFFFFFF);

	if(TOWNSTYPE_MARTY==townsType)
	{
		mem.AddAccess(&martyROMAccess,TOWNSADDR_MARTY_ROM0_BASE,TOWNSADDR_MARTY_ROM3_END-1);
	}

	if(TOWNSCPU_80386SX==cpuType)
	{
		// Memory access for higher than 16MB address space is still neeeded in 80386SX mode since
		// high address may be given to DMA, and DMA will translate address to low address.
		// Instead of implementing the translation for DMA, an easier solution is to leave
		// high address memory access.
		// Also CPU core does not care 80386SX mode.  Therefore, the reset vector still needs to be
		// 0xFFFFFFF0, which means SYSROM still needs to be accessible from high address.
		mem.AddAccess(&nativeDicROMAccess,TOWNSADDR_386SX_NATIVE_DICROM_BASE,TOWNSADDR_386SX_NATIVE_DICROM_END-1);
		mem.AddAccess(&nativeCMOSRAMAccess,TOWNSADDR_386SX_NATIVE_CMOSRAM_BASE,TOWNSADDR_386SX_NATIVE_CMOSRAM_END-1);
		mem.AddAccess(&spriteRAMAccess,TOWNSADDR_386SX_SPRITERAM_BASE,TOWNSADDR_386SX_SPRITERAM_END-1);
		mem.AddAccess(&oldMemCardAccess,TOWNSADDR_386SX_MEMCARD_BASE,TOWNSADDR_386SX_MEMCARD_END-1);
		mem.AddAccess(&osROMAccess,TOWNSADDR_386SX_OSROM_BASE,TOWNSADDR_386SX_OSROM_END-1);
		mem.AddAccess(&fontROMAccess,TOWNSADDR_386SX_FONT_BASE,TOWNSADDR_386SX_FONT_END-1);
		mem.AddAccess(&waveRAMAccess,TOWNSADDR_386SX_WAVERAM_WINDOW_BASE,TOWNSADDR_386SX_WAVERAM_WINDOW_END-1);
		mem.AddAccess(&sysROMAccess,TOWNSADDR_386SX_SYSROM_BASE,TOWNSADDR_386SX_SYSROM_END-1);
	}
}

void TownsPhysicalMemory::SetUpVRAMAccess(unsigned int cpuType,bool breakOnRead,bool breakOnWrite)
{
	auto &mem=*memPtr;
	if(true!=breakOnRead && true!=breakOnWrite)
	{
		mem.AddAccess(&VRAMAccess0,TOWNSADDR_VRAM0_BASE,TOWNSADDR_VRAM0_END-1);
		mem.AddAccess(&VRAMAccess1,TOWNSADDR_VRAM1_BASE,TOWNSADDR_VRAM1_END-1);
		mem.AddAccess(&VRAMAccessHighRes0,TOWNSADDR_VRAM_HIGHRES0_BASE,TOWNSADDR_VRAM_HIGHRES0_END-1); // For IIMX High Resolution Access.
		mem.AddAccess(&VRAMAccessHighRes1,TOWNSADDR_VRAM_HIGHRES1_BASE,TOWNSADDR_VRAM_HIGHRES1_END-1); // For IIMX High Resolution Access.
		mem.AddAccess(&VRAMAccessHighRes2,TOWNSADDR_VRAM_HIGHRES2_BASE,TOWNSADDR_VRAM_HIGHRES2_END-1); // For IIMX High Resolution Access.
		if(TOWNSCPU_80386SX==cpuType)
		{
			mem.AddAccess(&VRAMAccess0,TOWNSADDR_386SX_VRAM0_BASE,TOWNSADDR_386SX_VRAM0_END-1);
			mem.AddAccess(&VRAMAccess1,TOWNSADDR_386SX_VRAM1_BASE,TOWNSADDR_386SX_VRAM1_END-1);
		}
	}
	else
	{
		VRAMAccess0Debug.breakOnRead=breakOnRead;
		VRAMAccess0Debug.breakOnWrite=breakOnWrite;
		VRAMAccess1Debug.breakOnRead=breakOnRead;
		VRAMAccess1Debug.breakOnWrite=breakOnWrite;
		VRAMAccessHighRes0Debug.breakOnRead=breakOnRead;
		VRAMAccessHighRes0Debug.breakOnWrite=breakOnWrite;
		VRAMAccessHighRes1Debug.breakOnRead=breakOnRead;
		VRAMAccessHighRes1Debug.breakOnWrite=breakOnWrite;
		VRAMAccessHighRes2Debug.breakOnRead=breakOnRead;
		VRAMAccessHighRes2Debug.breakOnWrite=breakOnWrite;
		mem.AddAccess(&VRAMAccess0Debug,TOWNSADDR_VRAM0_BASE,TOWNSADDR_VRAM0_END-1);
		mem.AddAccess(&VRAMAccess1Debug,TOWNSADDR_VRAM1_BASE,TOWNSADDR_VRAM1_END-1);
		mem.AddAccess(&VRAMAccessHighRes0Debug,TOWNSADDR_VRAM_HIGHRES0_BASE,TOWNSADDR_VRAM_HIGHRES0_END-1); // For IIMX High Resolution Access.
		mem.AddAccess(&VRAMAccessHighRes1Debug,TOWNSADDR_VRAM_HIGHRES1_BASE,TOWNSADDR_VRAM_HIGHRES1_END-1); // For IIMX High Resolution Access.
		mem.AddAccess(&VRAMAccessHighRes2Debug,TOWNSADDR_VRAM_HIGHRES2_BASE,TOWNSADDR_VRAM_HIGHRES2_END-1); // For IIMX High Resolution Access.
		if(TOWNSCPU_80386SX==cpuType)
		{
			mem.AddAccess(&VRAMAccess0Debug,TOWNSADDR_386SX_VRAM0_BASE,TOWNSADDR_386SX_VRAM0_END-1);
			mem.AddAccess(&VRAMAccess1Debug,TOWNSADDR_386SX_VRAM1_BASE,TOWNSADDR_386SX_VRAM1_END-1);
		}
	}
}

void TownsPhysicalMemory::UpdateSysROMDicROMMappingFlag(bool sysRomMapping, bool dicRomMapping)
{
	// The interpretation of 0480H is very difficult.
	// On startup system rom does:
	//	MOV	DX,0480H
	//	IN	AL,DX
	//	OR	AL,2
	//	OUT DX,AL
	// To disable system-ROM mapping (active low) before RAM test.  However, since it keeps bit0, if I interpret bit 0
	// as controlling Dictionary ROM and CMOS RAM mapping to D0000H to DFFFFH, it doesn't clear Dictionary ROM and CMOS RAM
	// mapping, and then destroys CMOS RAM during the memory test.
	//
	//  404h    480h    480h
	//  Bit7    Bit1    Bit0        d0000   d8000   e0000   f8000
	//    0       0       0          N/A     N/A     N/A    SysROM
	//    0       0       1         DicROM   CMOS    N/A    SysROM
	//    0       1       0          N/A     N/A     N/A     RAM
	//    0       1       1         DicROM   CMOS    N/A     RAM
	//    1       0       0          RAM     RAM     RAM    SysROM
	//    1       0       1          RAM     RAM     RAM    SysROM
	//    1       1       0          RAM     RAM     RAM     RAM
	//    1       1       1          RAM     RAM     RAM     RAM
	//  !FMRVRAM
	//          !sysRomMapping
	//                  dicRom

	if(state.sysRomMapping!=sysRomMapping)
	{
		state.sysRomMapping = sysRomMapping;

		if (sysRomMapping)
		{
			memPtr->AddAccess(&mappedSysROMAccess, TOWNSADDR_SYSROM_MAP_BASE, TOWNSADDR_SYSROM_MAP_END - 1);
		}
		else
		{
			memPtr->AddAccess(&mainRAMAccess, TOWNSADDR_SYSROM_MAP_BASE, TOWNSADDR_SYSROM_MAP_END - 1);
		}
	}

	if(state.dicRom!=dicRomMapping)
	{
		state.dicRom = dicRomMapping;

		if (state.FMRVRAM)
		{
			if (dicRomMapping)
			{
				memPtr->AddAccess(&mappedDicROMandDicRAMAccess, TOWNSADDR_FMR_DICROM_BASE, TOWNSADDR_BACKUP_RAM_END - 1);
			}
			else
			{
				memPtr->RemoveAccess(TOWNSADDR_FMR_DICROM_BASE, TOWNSADDR_BACKUP_RAM_END - 1);
			}
		}
	}
}

void TownsPhysicalMemory::ResetSysROMDicROMMappingFlag(bool sysRomMapping,bool dicRomMapping)
{
	UpdateSysROMDicROMMappingFlag(true!=sysRomMapping,true!=dicRomMapping);
	UpdateSysROMDicROMMappingFlag(sysRomMapping,dicRomMapping);
}

void TownsPhysicalMemory::UpdateFMRVRAMMappingFlag(bool FMRVRAMMapping)
{
	if(state.FMRVRAM!=FMRVRAMMapping)
	{
		state.FMRVRAM = FMRVRAMMapping;

		if (FMRVRAMMapping)
		{
			memPtr->AddAccess(&FMRVRAMAccess, TOWNSADDR_FMR_VRAM_BASE, TOWNSADDR_FMR_VRAM_CVRAM_FONT_END - 1);
			if (state.dicRom) {
				memPtr->AddAccess(&mappedDicROMandDicRAMAccess, TOWNSADDR_FMR_DICROM_BASE, TOWNSADDR_BACKUP_RAM_END - 1);
				memPtr->RemoveAccess(TOWNSADDR_FMR_RESERVED_BASE, TOWNSADDR_FMR_RESERVED_END - 1);
			}
			else
			{
				memPtr->RemoveAccess(TOWNSADDR_FMR_DICROM_BASE, TOWNSADDR_FMR_RESERVED_END - 1);
			}
		}
		else
		{
			memPtr->AddAccess(&mainRAMAccess, TOWNSADDR_FMR_VRAM_BASE, TOWNSADDR_FMR_RESERVED_END - 1);
		}
	}
}

void TownsPhysicalMemory::ResetFMRVRAMMappingFlag(bool FMRVRAMMapping)
{
	UpdateFMRVRAMMappingFlag(true!=FMRVRAMMapping);
	UpdateFMRVRAMMappingFlag(FMRVRAMMapping);
}

void TownsPhysicalMemory::EnableOrDisableNativeVRAMMask(void)
{
	auto &mem=*memPtr;
	if(0xffffffff==cpputil::GetDword(state.nativeVRAMMask))
	{
		mem.AddAccess(&VRAMAccess0,TOWNSADDR_VRAM0_BASE,TOWNSADDR_VRAM0_END-1);
		mem.AddAccess(&VRAMAccess1,TOWNSADDR_VRAM1_BASE,TOWNSADDR_VRAM1_END-1);
		mem.AddAccess(&VRAMAccessHighRes0,TOWNSADDR_VRAM_HIGHRES0_BASE,TOWNSADDR_VRAM_HIGHRES0_END-1); // For IIMX High Resolution Access.
		mem.AddAccess(&VRAMAccessHighRes1,TOWNSADDR_VRAM_HIGHRES1_BASE,TOWNSADDR_VRAM_HIGHRES1_END-1); // For IIMX High Resolution Access.
		mem.AddAccess(&VRAMAccessHighRes2,TOWNSADDR_VRAM_HIGHRES2_BASE,TOWNSADDR_VRAM_HIGHRES2_END-1); // For IIMX High Resolution Access.
	}
	else
	{
		mem.AddAccess(&VRAMAccessWithMask0,TOWNSADDR_VRAM0_BASE,TOWNSADDR_VRAM0_END-1);
		mem.AddAccess(&VRAMAccessWithMask1,TOWNSADDR_VRAM1_BASE,TOWNSADDR_VRAM1_END-1);
		mem.AddAccess(&VRAMAccessWithMaskHighRes0,TOWNSADDR_VRAM_HIGHRES0_BASE,TOWNSADDR_VRAM_HIGHRES0_END-1); // For IIMX High Resolution Access.
		mem.AddAccess(&VRAMAccessWithMaskHighRes1,TOWNSADDR_VRAM_HIGHRES1_BASE,TOWNSADDR_VRAM_HIGHRES1_END-1); // For IIMX High Resolution Access.
		mem.AddAccess(&VRAMAccessWithMaskHighRes2,TOWNSADDR_VRAM_HIGHRES2_BASE,TOWNSADDR_VRAM_HIGHRES2_END-1); // For IIMX High Resolution Access.
	}
}

void TownsPhysicalMemory::BeginMemFilter(void)
{
	memFilter.RAMFilter.resize(state.RAM.size());
	memFilter.prevRAM.resize(state.RAM.size());
	memFilter.spriteRAMFilter.resize(GetSpriteRAMSize());
	memFilter.prevSpriteRAM.resize(GetSpriteRAMSize());
	for(uint32_t i=0; i<state.RAM.size(); ++i)
	{
		memFilter.RAMFilter[i]=true;
		memFilter.prevRAM[i]=state.RAM[i];
	}
	for(uint32_t i=0; i<GetSpriteRAMSize(); ++i)
	{
		memFilter.spriteRAMFilter[i]=true;
		memFilter.prevSpriteRAM[i]=state.spriteRAM[i];
	}
}
unsigned int TownsPhysicalMemory::ApplyMemFilter(uint8_t currentValue)
{
	unsigned int N=0;
	for(uint32_t i=0; i<state.RAM.size(); ++i)
	{
		if(state.RAM[i]!=currentValue)
		{
			memFilter.RAMFilter[i]=false;
		}
		if(true==memFilter.RAMFilter[i])
		{
			++N;
		}
		memFilter.prevRAM[i]=state.RAM[i];
	}
	for(uint32_t i=0; i<GetSpriteRAMSize(); ++i)
	{
		if(state.spriteRAM[i]!=currentValue)
		{
			memFilter.spriteRAMFilter[i]=false;
		}
		if(true==memFilter.spriteRAMFilter[i])
		{
			++N;
		}
		memFilter.prevSpriteRAM[i]=state.spriteRAM[i];
	}
	return N;
}
unsigned int TownsPhysicalMemory::ApplyMemFilterDecrease(void)
{
	unsigned int N=0;
	for(uint32_t i=0; i<state.RAM.size(); ++i)
	{
		if(state.RAM[i]>=memFilter.prevRAM[i])
		{
			memFilter.RAMFilter[i]=false;
		}
		if(true==memFilter.RAMFilter[i])
		{
			++N;
		}
		memFilter.prevRAM[i]=state.RAM[i];
	}
	for(uint32_t i=0; i<GetSpriteRAMSize(); ++i)
	{
		if(state.spriteRAM[i]>=memFilter.prevSpriteRAM[i])
		{
			memFilter.spriteRAMFilter[i]=false;
		}
		if(true==memFilter.spriteRAMFilter[i])
		{
			++N;
		}
		memFilter.prevSpriteRAM[i]=state.spriteRAM[i];
	}
	return N;
}
unsigned int TownsPhysicalMemory::ApplyMemFilterIncrease(void)
{
	unsigned int N=0;
	for(uint32_t i=0; i<state.RAM.size(); ++i)
	{
		if(state.RAM[i]<=memFilter.prevRAM[i])
		{
			memFilter.RAMFilter[i]=false;
		}
		if(true==memFilter.RAMFilter[i])
		{
			++N;
		}
		memFilter.prevRAM[i]=state.RAM[i];
	}
	for(uint32_t i=0; i<GetSpriteRAMSize(); ++i)
	{
		if(state.spriteRAM[i]<=memFilter.prevSpriteRAM[i])
		{
			memFilter.spriteRAMFilter[i]=false;
		}
		if(true==memFilter.spriteRAMFilter[i])
		{
			++N;
		}
		memFilter.prevSpriteRAM[i]=state.spriteRAM[i];
	}
	return N;
}
unsigned int TownsPhysicalMemory::ApplyMemFilterDifferent(void)
{
	unsigned int N=0;
	for(uint32_t i=0; i<state.RAM.size(); ++i)
	{
		if(state.RAM[i]==memFilter.prevRAM[i])
		{
			memFilter.RAMFilter[i]=false;
		}
		if(true==memFilter.RAMFilter[i])
		{
			++N;
		}
		memFilter.prevRAM[i]=state.RAM[i];
	}
	for(uint32_t i=0; i<GetSpriteRAMSize(); ++i)
	{
		if(state.spriteRAM[i]==memFilter.prevSpriteRAM[i])
		{
			memFilter.spriteRAMFilter[i]=false;
		}
		if(true==memFilter.spriteRAMFilter[i])
		{
			++N;
		}
		memFilter.prevSpriteRAM[i]=state.spriteRAM[i];
	}
	return N;
}
unsigned int TownsPhysicalMemory::ApplyMemFilterEqual(void)
{
	unsigned int N=0;
	for(uint32_t i=0; i<state.RAM.size(); ++i)
	{
		if(state.RAM[i]!=memFilter.prevRAM[i])
		{
			memFilter.RAMFilter[i]=false;
		}
		if(true==memFilter.RAMFilter[i])
		{
			++N;
		}
		memFilter.prevRAM[i]=state.RAM[i];
	}
	for(uint32_t i=0; i<GetSpriteRAMSize(); ++i)
	{
		if(state.spriteRAM[i]!=memFilter.prevSpriteRAM[i])
		{
			memFilter.spriteRAMFilter[i]=false;
		}
		if(true==memFilter.spriteRAMFilter[i])
		{
			++N;
		}
		memFilter.prevSpriteRAM[i]=state.spriteRAM[i];
	}
	return N;
}
void TownsPhysicalMemory::PrintMemFilter(void)
{
	for(unsigned int i=0; i<memFilter.RAMFilter.size(); ++i)
	{
		if(true==memFilter.RAMFilter[i])
		{
			std::cout << "PHYS:" << cpputil::Uitox(i) << std::endl;
		}
	}
	for(unsigned int i=0; i<memFilter.spriteRAMFilter.size(); ++i)
	{
		if(true==memFilter.spriteRAMFilter[i])
		{
			std::cout << "PHYS:" << cpputil::Uitox(i+TOWNSADDR_SPRITERAM_BASE) << std::endl;
		}
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
	text.back()+="  FMR VRAM Mask(000CFF81H)="+cpputil::Ubtox(state.FMRVRAMMask);
	text.back()+="  FMR VRAM Read/Write Offset="+cpputil::Uitox(state.FMRVRAMWriteOffset);

	text.push_back(empty);
	text.back()="D0000-EFFFF:";
	if (state.FMRVRAM)
	{
		if (state.dicRom)
		{
			text.back() += "Dictionary RAM/ROM, User Font RAM";
		}
		else
		{
			text.back() += "N/A";
		}
	}
	else
	{
		text.back() += "Main RAM";
	}
	text.back()+="  Dictionary Bank:";
	text.back()+=cpputil::Uitox(state.DICROMBank);

	text.push_back(empty);
	text.back() = "F8000-FFFFF:";
	if(true==state.sysRomMapping)
	{
		text.back()+="Boot ROM";
	}
	else
	{
		text.back()+="Main RAM";
	}

	text.push_back("");
	text.back()="Native VRAM Mask:";
	text.back()+=cpputil::Uitox(cpputil::GetDword(state.nativeVRAMMask));

	text.push_back("");
	text.back()="IC Memory Card Type:";
	text.back()+=TownsMemCardTypeToStr(state.memCard.memCardType);

	return text;
}


/* virtual */ uint32_t TownsPhysicalMemory::SerializeVersion(void) const
{
	return 1;
}
/* virtual */ void TownsPhysicalMemory::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	std::string stateDir,stateName;
	cpputil::SeparatePathFile(stateDir,stateName,stateFName);

	PushBool(data,state.sysRomMapping);  // Whenever changing this flag, synchronously change memory access mapping.
	PushBool(data,state.dicRom);
	PushUint32(data,state.DICROMBank);
	PushBool(data,state.FMRVRAM);
	PushBool(data,state.TVRAMWrite);
	PushBool(data,state.ANKFont);
	PushUint32(data,state.FMRVRAMMask);
	PushUint32(data,state.FMRVRAMWriteOffset);

	PushUint32(data,state.nativeVRAMMaskRegisterLatch);
	for(auto nvMsk : state.nativeVRAMMask)
	{
		PushUint16(data, nvMsk);
	}

	std::vector <uint8_t> VRAM,spriteRAM; // For absolute backward compatibility, save as vector.
	VRAM.resize(GetVRAMSize());
	memcpy(VRAM.data(),state.VRAM,GetVRAMSize());
	spriteRAM.resize(GetSpriteRAMSize());
	memcpy(spriteRAM.data(),state.spriteRAM,GetSpriteRAMSize());

	PushUcharArray(data,state.RAM);
	PushUcharArray(data,VRAM);
	PushUcharArray(data,state.CVRAM);
	PushUcharArray(data,spriteRAM);
	PushUcharArray(data,state.notUsed);
	PushUcharArray(data,TOWNS_CMOS_SIZE,state.CMOSRAM);

	// PCMCIA memory card.
	PushUint32(data,state.memCard.memCardType);
	PushString(data,state.memCard.fName);
	PushString(data,cpputil::MakeRelativePath(state.memCard.fName,stateDir));
	PushUcharArray(data,state.memCard.data);
	PushBool(data, state.memCard.modified);
	PushBool(data, state.memCard.changed);
	PushBool(data, state.memCard.writeProtected);
	PushUint64(data,state.memCard.lastModified);


	PushUint32(data, state.memCardBank);
	PushBool(data, state.memCardREG); // [2] pp.795

	PushUint16(data,state.kanjiROMAccess.JISCodeHigh); // 000CFF94 Big Endian?
	PushUint16(data,state.kanjiROMAccess.JISCodeLow);  // 000CFF95
	PushUint32(data,state.kanjiROMAccess.row);

	// System ROM and DOS rom needs to be saved.
	// If the system started with YSDOS, it cannot switch back to the original MSDOS, vise-versa.
	PushUcharArray(data,sysRom);
	PushUcharArray(data,dosRom);
}
/* virtual */ bool TownsPhysicalMemory::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	std::string stateDir,stateName;
	cpputil::SeparatePathFile(stateDir,stateName,stateFName);

	auto prevRAMsize=state.RAM.size();



	state.sysRomMapping=ReadBool(data);  // Whenever changing this flag, synchronously change memory access mapping.
	state.dicRom=ReadBool(data);
	state.DICROMBank=ReadUint32(data);
	state.FMRVRAM=ReadBool(data);
	state.TVRAMWrite=ReadBool(data);
	state.ANKFont=ReadBool(data);
	state.FMRVRAMMask=ReadUint32(data);
	state.FMRVRAMWriteOffset=ReadUint32(data);

	state.nativeVRAMMaskRegisterLatch=ReadUint32(data);
	for(auto &nvMsk : state.nativeVRAMMask)
	{
		nvMsk=ReadUint16(data);
	}

	state.RAM=ReadUcharArray(data);
	auto VRAM=ReadUcharArray(data);
	state.CVRAM=ReadUcharArray(data);
	auto spriteRAM=ReadUcharArray(data);
	state.notUsed=ReadUcharArray(data);
	ReadUcharArray(data,TOWNS_CMOS_SIZE,state.CMOSRAM);

	memcpy(state.VRAM,     VRAM.data(),     std::min<uint32_t>(GetVRAMSize(),VRAM.size()));
	memcpy(state.spriteRAM,spriteRAM.data(),std::min<uint32_t>(GetSpriteRAMSize(),spriteRAM.size()));


	// PCMCIA memory card.
	state.memCard.memCardType=ReadUint32(data);

	{
		state.memCard.fName=""; // Tentative

		std::string fName=ReadString(data);
		std::string relPath="";
		if(1<=version)
		{
			relPath=ReadString(data);
		}

		// See disk-image search rule in townsstate.cpp
		bool fileExists=false;

		// (1) Try using the filename stored in the state file as is.
		if(true!=fileExists)
		{
			fileExists=cpputil::FileExists(fName);
			if(true==fileExists)
			{
				state.memCard.fName=fName;
			}
		}

		// (2) Try state path+relative path
		auto stateRel=cpputil::MakeFullPathName(stateDir,relPath);
		if(true!=fileExists)
		{
			fileExists=cpputil::FileExists(stateRel);
			if(true==fileExists)
			{
				state.memCard.fName=stateRel;
			}
		}

		// (3) Try image search path+file name
		// No search paths for IC memory card.

		// (4) Try state path+file name
		if(true!=fileExists)
		{
			std::string imgDir,imgName;
			cpputil::SeparatePathFile(imgDir,imgName,fName);
			auto ful=cpputil::MakeFullPathName(stateDir,imgName);
			fileExists=cpputil::FileExists(ful);
			if(true==fileExists)
			{
				state.memCard.fName=ful;
			}
		}
	}


	state.memCard.data=ReadUcharArray(data);
	state.memCard.modified=ReadBool(data);
	state.memCard.changed=ReadBool(data);
	state.memCard.writeProtected=ReadBool(data);
	state.memCard.lastModified=ReadUint64(data);


	state.memCardBank=ReadUint32(data);
	state.memCardREG=ReadBool(data); // [2] pp.795

	state.kanjiROMAccess.JISCodeHigh=ReadUint16(data); // 000CFF94 Big Endian?
	state.kanjiROMAccess.JISCodeLow=ReadUint16(data);  // 000CFF95
	state.kanjiROMAccess.row=ReadUint32(data);

	// System ROM and DOS rom needs to be saved.
	// If the system started with YSDOS, it cannot switch back to the original MSDOS, vise-versa.
	sysRom=ReadUcharArray(data);
	dosRom=ReadUcharArray(data);



	// Reset mappings
	ResetSysROMDicROMMappingFlag(state.sysRomMapping,state.dicRom);
	ResetFMRVRAMMappingFlag(state.FMRVRAM);
	EnableOrDisableNativeVRAMMask();
	if(prevRAMsize<state.RAM.size())
	{
		memPtr->AddAccess(&mainRAMAccess,prevRAMsize,(unsigned int)state.RAM.size()-1);
	}
	else if(state.RAM.size()<prevRAMsize)
	{
		memPtr->AddAccess(&memPtr->nullAccess,(unsigned int)state.RAM.size(),prevRAMsize-1);
	}

	return true;
}
