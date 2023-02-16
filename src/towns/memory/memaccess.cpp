/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "i486.h"
#include "i486debug.h"



#include "cpputil.h"
#include "memaccess.h"
#include "townsdef.h"
#include "crtc.h"
#include "towns.h"


TownsMemAccess::TownsMemAccess()
{
	physMemPtr=nullptr;
	cpuPtr=nullptr;
}
void TownsMemAccess::SetPhysicalMemoryPointer(TownsPhysicalMemory *ptr)
{
	physMemPtr=ptr;
}
void TownsMemAccess::SetCPUPointer(class i486DX *cpuPtr)
{
	this->cpuPtr=cpuPtr;
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsMainRAMAccess::FetchByte(unsigned int physAddr) const
{
	return physMemPtr->state.RAM[physAddr];
}
/* virtual */ unsigned int TownsMainRAMAccess::FetchWord(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	return cpputil::GetWord(RAMPtr);
}
/* virtual */ unsigned int TownsMainRAMAccess::FetchDword(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	return cpputil::GetDword(RAMPtr);
}
/* virtual */ void TownsMainRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	physMemPtr->state.RAM[physAddr]=data;
}
/* virtual */ void TownsMainRAMAccess::StoreWord(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	cpputil::PutWord(RAMPtr,(unsigned short)data);
}
/* virtual */ void TownsMainRAMAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	cpputil::PutDword(RAMPtr,data);
}
/* virtual */ MemoryAccess::ConstMemoryWindow TownsMainRAMAccess::GetConstMemoryWindow(unsigned int physAddr) const
{
	MemoryAccess::ConstMemoryWindow memWin;
	memWin.ptr=physMemPtr->state.RAM.data()+(physAddr&(~0xfff));
	return memWin;
}
/* virtual */ MemoryAccess::MemoryWindow TownsMainRAMAccess::GetMemoryWindow(unsigned int physAddr)
{
	MemoryAccess::MemoryWindow memWin;
	memWin.ptr=physMemPtr->state.RAM.data()+(physAddr&(~0xfff));
	return memWin;
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsMappedSysROMAccess::FetchByte(unsigned int physAddr) const
{
	unsigned int offset=physAddr-TOWNSADDR_SYSROM_MAP_BASE;
	auto ROMPtr=physMemPtr->sysRom.data()+TOWNSADDR_SYSROM_MAP_OFFSET_DIFFERENCE+offset;
	if(offset<TOWNSADDR_SYSROM_MAP_SIZE)
	{
		return ROMPtr[0];
	}
	cpuPtr->Abort("Out-of-bound access to Mapped SYSROM");
	return 0xFFFFFFFF;
}
/* virtual */ unsigned int TownsMappedSysROMAccess::FetchWord(unsigned int physAddr) const
{
	unsigned int offset=physAddr-TOWNSADDR_SYSROM_MAP_BASE;
	auto ROMPtr=physMemPtr->sysRom.data()+TOWNSADDR_SYSROM_MAP_OFFSET_DIFFERENCE+offset;
	if(offset<TOWNSADDR_SYSROM_MAP_SIZE-1)
	{
		return cpputil::GetWord(ROMPtr);
	}
	cpuPtr->Abort("Cross-Border WORD access to Mapped SYSROM");
	return 0xFFFFFFFF;
}
/* virtual */ unsigned int TownsMappedSysROMAccess::FetchDword(unsigned int physAddr) const
{
	unsigned int offset=physAddr-TOWNSADDR_SYSROM_MAP_BASE;
	auto ROMPtr=physMemPtr->sysRom.data()+TOWNSADDR_SYSROM_MAP_OFFSET_DIFFERENCE+offset;
	if(offset<TOWNSADDR_SYSROM_MAP_SIZE-3)
	{
		return cpputil::GetDword(ROMPtr);;
	}
	cpuPtr->Abort("Cross-Border DWORD access to Mapped SYSROM");
	return 0xFFFFFFFF;
}
/* virtual */ void TownsMappedSysROMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	// ROM mode no writing
}
/* virtual */ void TownsMappedSysROMAccess::StoreWord(unsigned int physAddr,unsigned int data)
{
	// ROM mode no writing
}
/* virtual */ void TownsMappedSysROMAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	// ROM mode no writing
}

/* virtual */ MemoryAccess::ConstMemoryWindow TownsMappedSysROMAccess::GetConstMemoryWindow(unsigned int physAddr) const
{
	MemoryAccess::ConstMemoryWindow memWin;
	const unsigned int offset=(physAddr&(~0xfff))-TOWNSADDR_SYSROM_MAP_BASE;
	memWin.ptr=physMemPtr->sysRom.data()+TOWNSADDR_SYSROM_MAP_OFFSET_DIFFERENCE+offset;
	return memWin;
}

////////////////////////////////////////////////////////////


TownsFMRVRAMAccess::TownsFMRVRAMAccess()
{
	breakOnFMRVRAMWrite=false;
	breakOnFMRVRAMRead=false;
	breakOnCVRAMWrite=false;
	breakOnCVRAMRead=false;
}

/* virtual */ unsigned int TownsFMRVRAMAccess::FetchByte(unsigned int physAddr) const
{
	if((TOWNS_MEMIO_1_LOW<=physAddr && physAddr<=TOWNS_MEMIO_1_HIGH) ||
	   (TOWNS_MEMIO_2_LOW<=physAddr && physAddr<=TOWNS_MEMIO_2_HIGH))
	{
		switch(physAddr)
		{
		case TOWNSMEMIO_MIX://                0x000CFF80, // Called Dummy [2] pp.22, pp.158
			break;
		case TOWNSMEMIO_FMR_GVRAMMASK://      0x000CFF81, // [2] pp.22,pp.159
			return physMemPtr->state.FMRVRAMMask;
		case TOWNSMEMIO_FMR_GVRAMDISPMODE://  0x000CFF82, // [2] pp.22,pp.158
			break;
		case TOWNSMEMIO_FMR_GVRAMPAGESEL://   0x000CFF83, // [2] pp.22,pp.159
			if(0==physMemPtr->state.FMRVRAMWriteOffset)
			{
				return 0;
			}
			else
			{
				return 0x10;
			}
			break;

		case TOWNSMEMIO_FIRQ://               0x000CFF84, // [2] pp.22,pp.95 Always zero in FM TOWNS
			return 0;

		case TOWNSMEMIO_FMR_HSYNC_VSYNC://    0x000CFF86, // [2] pp.22,pp.160
			{
				bool VSYNC=crtcPtr->InVSYNC(townsPtr->state.townsTime);
				bool HSYNC=crtcPtr->InHSYNC(townsPtr->state.townsTime);
				unsigned char data=0x10;  // Bit4 is always 1.
				data|=(VSYNC ? 4 : 0);
				data|=(HSYNC ? 0x80 : 0);
				return data;
			}
			break;

		case TOWNSMEMIO_KANJI_JISCODE_HIGH:// 0x000CFF94,
			return 0x80;  // 
		case TOWNSMEMIO_KANJI_JISCODE_LOW://  0x000CFF95,
			break;
		case TOWNSMEMIO_KANJI_PTN_HIGH://     0x000CFF96,
			{
				auto ROMCode=(physMemPtr->state.kanjiROMAccess.FontROMCode())&8191;
				return physMemPtr->fontRom[32*ROMCode+physMemPtr->state.kanjiROMAccess.row*2];
			}
			break;
		case TOWNSMEMIO_KANJI_PTN_LOW ://     0x000CFF97,
			if(true==physMemPtr->takeJISCodeLog && 0==physMemPtr->state.kanjiROMAccess.row)
			{
				physMemPtr->JISCodeLog.push_back(physMemPtr->state.kanjiROMAccess.JISCodeHigh);
				physMemPtr->JISCodeLog.push_back(physMemPtr->state.kanjiROMAccess.JISCodeLow);
			}
			{
				auto ROMCode=physMemPtr->state.kanjiROMAccess.FontROMCode()&8191;
				auto byteData=physMemPtr->fontRom[32*ROMCode+physMemPtr->state.kanjiROMAccess.row*2+1];
				physMemPtr->state.kanjiROMAccess.row=(physMemPtr->state.kanjiROMAccess.row+1)&0x0F;
				return byteData;
			}
			break;
		case TOWNSMEMIO_BUZZER_CONTROL://     0x000CFF98
			townsPtr->timer.ControlBuzzerByMemoryIO(true);
			break;
		}
	}
	else if(TOWNSADDR_FMR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_VRAM_END) /// FMR VRAM Plane Access
	{
		const auto FMRAddr=physAddr-TOWNSADDR_FMR_VRAM_BASE;
		const auto VRAMAddr=(FMRAddr<<2)+physMemPtr->state.FMRVRAMWriteOffset;
		auto shift=(physMemPtr->state.FMRVRAMMask>>6)&3;
		unsigned char andPtnHigh=(0x10<<shift);
		unsigned char andPtnLow=(1<<shift);
		unsigned char orPtnHigh=0x40;
		unsigned char orPtnLow=0x80;
		unsigned char data=0;
		for(int i=0; i<4; ++i)
		{
			if(0!=(physMemPtr->state.VRAM[VRAMAddr+i]&andPtnHigh))
			{
				data|=orPtnHigh;
			}
			if(0!=(physMemPtr->state.VRAM[VRAMAddr+i]&andPtnLow))
			{
				data|=orPtnLow;
			}
			orPtnHigh>>=2;
			orPtnLow>>=2;
		}
		if(true==breakOnFMRVRAMRead &&
		   nullptr!=cpuPtr &&
		   nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("FMRVRAM Read "+cpputil::Uitox(physAddr));
		}
		return data;
	}
	else if(TOWNSADDR_FMR_CVRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_CVRAM_END) /// FMR I/OCVRAM Access
	{
		if(true==breakOnCVRAMRead &&
		   nullptr!=cpuPtr &&
		   nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("CVRAM Read "+cpputil::Uitox(physAddr));
		}
		return physMemPtr->state.spriteRAM[physAddr-TOWNSADDR_FMR_VRAM_END];
	}
	else
	{
		if(true==physMemPtr->state.ANKFont)
		{
			if(TOWNSADDR_FMR_ANK8_BASE<=physAddr && physAddr<TOWNSADDR_FMR_ANK8_END)
			{
				return physMemPtr->fontRom[TOWNSADDR_FMR_ANK8_MAP+physAddr-TOWNSADDR_FMR_ANK8_BASE];
			}
			else if(TOWNSADDR_FMR_ANK16_BASE<=physAddr && physAddr<TOWNSADDR_FMR_ANK16_END)
			{
				return physMemPtr->fontRom[TOWNSADDR_FMR_ANK16_MAP+physAddr-TOWNSADDR_FMR_ANK16_BASE];
			}
		}
		else if(TOWNSADDR_FMR_CVRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_KVRAM_END)
		{
			return physMemPtr->state.spriteRAM[physAddr-TOWNSADDR_FMR_CVRAM_BASE];
		}
	}
	return 0xff;
}
/* virtual */ unsigned int TownsFMRVRAMAccess::FetchWord(unsigned int physAddr) const
{
	return TownsMemAccess::FetchWord(physAddr);
}
/* virtual */ unsigned int TownsFMRVRAMAccess::FetchDword(unsigned int physAddr) const
{
	return TownsMemAccess::FetchDword(physAddr);
}
/* virtual */ void TownsFMRVRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	const auto FMRAddr=physAddr-TOWNSADDR_FMR_VRAM_BASE;
	if(FMRAddr<TOWNSADDR_FMR_VRAM_END-TOWNSADDR_FMR_VRAM_BASE)
	{
		// Assume screen mode 1 and 2.
		//   Logical Resolution 640x819
		//   Visible Resolution 640x400 or 640x200
		// FMRVram 8pixels per byte, 640pixels=80bytes.
		// TownsVRAM 2pixels per byte, 640pixels=320bytes.
		// Just multiply 4 to get TownsVRAM address.
		const auto VRAMAddr=(FMRAddr<<2)+physMemPtr->state.FMRVRAMWriteOffset;

		unsigned char maskLow=(physMemPtr->state.FMRVRAMMask&0x0F);
		auto maskHigh=(maskLow<<4);
		auto andPtn=~(maskLow|maskHigh);

		unsigned char bitTestHigh=0x40;
		unsigned char bitTestLow=0x80;
		for(int i=0; i<4; ++i)
		{
			physMemPtr->state.VRAM[VRAMAddr+i]&=andPtn;
			if(0!=(data&bitTestHigh))
			{
				physMemPtr->state.VRAM[VRAMAddr+i]|=maskHigh;
			}
			if(0!=(data&bitTestLow))
			{
				physMemPtr->state.VRAM[VRAMAddr+i]|=maskLow;
			}
			bitTestHigh>>=2;
			bitTestLow>>=2;
		}
		if(true==breakOnFMRVRAMWrite &&
		   nullptr!=cpuPtr &&
		   nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("FMRVRAM Write "+cpputil::Uitox(physAddr));
		}
	}
	else if((TOWNS_MEMIO_1_LOW<=physAddr && physAddr<=TOWNS_MEMIO_1_HIGH) ||
	        (TOWNS_MEMIO_2_LOW<=physAddr && physAddr<=TOWNS_MEMIO_2_HIGH))
	{
		switch(physAddr)
		{
		case TOWNSMEMIO_MIX://                0x000CFF80, // Called Dummy [2] pp.22, pp.158
			break;
		case TOWNSMEMIO_FMR_GVRAMMASK://      0x000CFF81, // [2] pp.22,pp.159
			physMemPtr->state.FMRVRAMMask=data;
			break;
		case TOWNSMEMIO_FMR_GVRAMDISPMODE://  0x000CFF82, // [2] pp.22,pp.158
			crtcPtr->MEMIOWriteFMRVRAMDisplayMode(data);
			break;
		case TOWNSMEMIO_FMR_GVRAMPAGESEL://   0x000CFF83, // [2] pp.22,pp.159
			// Looks like I was interpreting the definition of FM-R Graphics VRAM 'PAGE' wrong.
			// I thought PAGEs mapps to LAYERs, but after reading Artane.'s FM TOWNS emulator source,
			// FM-R Graphics VRAM PAGE0 maps to upper half or VRAM LAYER0, and PAGE1 lower half.
			// It makes sense because [2] pp. 155 Section 4.9.2 tells that FMR50 compatible mode
			// uses screen layer0 for graphics and layer1 for text.  So, 'page' and 'layer' are
			// different.
			// Credit to Artane.!  Thanks!
			physMemPtr->state.FMRVRAMWriteOffset=(0!=(data&0x10) ? TOWNS_FMRMODE_VRAM_OFFSET : 0);
			break;

		case TOWNSMEMIO_FIRQ://               0x000CFF84, // [2] pp.22,pp.95 Always zero in FM TOWNS
			break; // No write access

		case TOWNSMEMIO_FMR_HSYNC_VSYNC://    0x000CFF86, // [2] pp.22,pp.160
			break;

		case TOWNSMEMIO_KANJI_JISCODE_HIGH:// 0x000CFF94,
			physMemPtr->state.kanjiROMAccess.JISCodeHigh=data;
			break;
		case TOWNSMEMIO_KANJI_JISCODE_LOW://  0x000CFF95,
			physMemPtr->state.kanjiROMAccess.JISCodeLow=data;
			physMemPtr->state.kanjiROMAccess.row=0;
			break;
		case TOWNSMEMIO_KANJI_PTN_HIGH://     0x000CFF96,
			// Write access enabled? [2] pp.95
			break;
		case TOWNSMEMIO_KANJI_PTN_LOW://      0x000CFF97,
			// Write access enabled? [2] pp.95
			break;
		case TOWNSMEMIO_BUZZER_CONTROL://     0x000CFF98
			townsPtr->timer.ControlBuzzerByMemoryIO(false);
			break;
		case TOWNSMEMIO_KVRAM_OR_ANKFONT: //  0x000CFF99
			physMemPtr->state.ANKFont=(0!=(data&1));
			break;
		}
	}
	else if(TOWNSADDR_FMR_CVRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_KVRAM_END) // Except I/O.
	{
		if(true==breakOnCVRAMWrite &&
		   nullptr!=cpuPtr &&
		   nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("CVRAM Write "+cpputil::Uitox(physAddr));
		}
		physMemPtr->state.spriteRAM[physAddr-TOWNSADDR_FMR_CVRAM_BASE]=data;
		physMemPtr->state.TVRAMWrite=true;
	}
}
/* virtual */ void TownsFMRVRAMAccess::StoreWord(unsigned int physAddr,unsigned int data)
{
	TownsMemAccess::StoreWord(physAddr,data);
}
/* virtual */ void TownsFMRVRAMAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	TownsMemAccess::StoreDword(physAddr,data);
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsMappedDICROMandCMOSRAMAccess::FetchByte(unsigned int physAddr) const
{
	if(TOWNSADDR_FMR_DICROM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_DICROM_END) // Dic ROM
	{
		unsigned int offset=32768*physMemPtr->state.DICROMBank+(physAddr-TOWNSADDR_FMR_DICROM_BASE);
		return physMemPtr->dicRom[offset];
	}
	else if(TOWNSADDR_BACKUP_RAM_BASE<=physAddr && physAddr<TOWNSADDR_BACKUP_RAM_END) // 
	{
		return physMemPtr->state.CMOSRAM[physAddr-TOWNSADDR_BACKUP_RAM_BASE];
	}
	return 0xFF;
}
/* virtual */ void TownsMappedDICROMandCMOSRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	if(TOWNSADDR_BACKUP_RAM_BASE<=physAddr && physAddr<TOWNSADDR_BACKUP_RAM_END) // 
	{
		physMemPtr->state.CMOSRAM[physAddr-TOWNSADDR_BACKUP_RAM_BASE]=data;
	}
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsNativeDICROMAccess::FetchByte(unsigned int physAddr) const
{
	auto &physMem=*physMemPtr;
	return physMem.dicRom[physAddr&TOWNSADDR_NATIVE_DICROM_AND];
}
/* virtual */ void TownsNativeDICROMAccess::StoreByte(unsigned int,unsigned char)
{
	// It's a ROM.
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsNativeCMOSRAMAccess::FetchByte(unsigned int physAddr) const
{
	auto &physMem=*physMemPtr;
	return physMem.state.CMOSRAM[physAddr&TOWNSADDR_NATIVE_CMOSRAM_AND];
}
/* virtual */ void TownsNativeCMOSRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &physMem=*physMemPtr;
	physMem.state.CMOSRAM[physAddr&TOWNSADDR_NATIVE_CMOSRAM_AND]=data;
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsSpriteRAMAccess::FetchByte(unsigned int physAddr) const
{
	// 0x81000000,0x8101FFFF
	auto &state=physMemPtr->state;
	return state.spriteRAM[physAddr&TOWNSADDR_SPRITERAM_AND];
}
/* virtual */ unsigned int TownsSpriteRAMAccess::FetchWord(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	return cpputil::GetWord(state.spriteRAM+(physAddr&TOWNSADDR_SPRITERAM_AND));
}
/* virtual */ unsigned int TownsSpriteRAMAccess::FetchDword(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	return cpputil::GetDword(state.spriteRAM+(physAddr&TOWNSADDR_SPRITERAM_AND));
}
/* virtual */ void TownsSpriteRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &state=physMemPtr->state;
	state.spriteRAM[physAddr&TOWNSADDR_SPRITERAM_AND]=data;
}
/* virtual */ void TownsSpriteRAMAccess::StoreWord(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	cpputil::PutWord(state.spriteRAM+(physAddr&TOWNSADDR_SPRITERAM_AND),data);
}
/* virtual */ void TownsSpriteRAMAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	cpputil::PutDword(state.spriteRAM+(physAddr&TOWNSADDR_SPRITERAM_AND),data);
}

////////////////////////////////////////////////////////////

/* virtual */ unsigned int TownsOldMemCardAccess::FetchByte(unsigned int physAddr) const
{
	// DISKBIOS (INT 93H AX=??50H) reads C0000000H for CIS information.
	// Just returning 0xFF (CIS termination) makes TICMFMT.EXE happy.
	if(true==physMemPtr->state.memCardREG)
	{
		return 0xFF;
	}

	auto &memCard=physMemPtr->state.memCard;

	// Looks like first 16MB of JEIDA4 memory card is also accessible from C0000000h
	unsigned int memCardAddr=physAddr;
	if(0xC0000000<=physAddr)
	{
		memCardAddr&=TOWNSADDR_MEMCARD_AND;
	}
	else
	{
		memCardAddr&=TOWNSADDR_386SX_MEMCARD_AND;
	}
	if(memCardAddr<memCard.data.size())
	{
		townsPtr->NotifyDiskRead();
		return memCard.data[memCardAddr];
	}
	return 0xFF;
}
/* virtual */ void TownsOldMemCardAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	if(true!=physMemPtr->state.memCardREG)
	{
		townsPtr->NotifyDiskRead();

		// Looks like first 16MB of JEIDA4 memory card is also accessible from C0000000h
		auto &memCard=physMemPtr->state.memCard;
		if(true!=memCard.writeProtected)
		{
			unsigned int memCardAddr=physAddr;
			if(0xC0000000<=physAddr)
			{
				memCardAddr&=TOWNSADDR_MEMCARD_AND;
			}
			else
			{
				memCardAddr&=TOWNSADDR_386SX_MEMCARD_AND;
			}
			if(memCardAddr<memCard.data.size())
			{
				memCard.data[memCardAddr]=data;
				memCard.modified=true;
			}
		}
	}
}

////////////////////////////////////////////////////////////

/* virtual */ unsigned int TownsJEIDA4MemCardAccess::FetchByte(unsigned int physAddr) const
{
	if(true!=physMemPtr->state.memCardREG)
	{
		auto &memCard=physMemPtr->state.memCard;
		if(TOWNS_MEMCARD_TYPE_JEIDA4==memCard.memCardType)
		{
			// I should return attribute information if REG==true.  But, I don't know what exactly it is.
			unsigned int memCardAddr=physAddr;
			if(0xC0000000<=physAddr)
			{
				memCardAddr&=TOWNSADDR_MEMCARD_AND;
				memCardAddr+=0x400000*physMemPtr->state.memCardBank;
			}
			else
			{
				memCardAddr&=TOWNSADDR_386SX_MEMCARD_AND;
				memCardAddr+=0x100000*physMemPtr->state.memCardBank;
			}
			if(memCardAddr<memCard.data.size())
			{
				return memCard.data[memCardAddr];
			}
		}
	}
	return 0xFF;
}
/* virtual */ void TownsJEIDA4MemCardAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	if(true!=physMemPtr->state.memCardREG)
	{
		auto &memCard=physMemPtr->state.memCard;
		if(TOWNS_MEMCARD_TYPE_JEIDA4==memCard.memCardType && true!=memCard.writeProtected)
		{
			unsigned int memCardAddr=physAddr;
			if(0xC0000000<=physAddr)
			{
				memCardAddr&=TOWNSADDR_MEMCARD_AND;
				memCardAddr+=0x400000*physMemPtr->state.memCardBank;
			}
			else
			{
				memCardAddr&=TOWNSADDR_386SX_MEMCARD_AND;
				memCardAddr+=0x100000*physMemPtr->state.memCardBank;
			}
			if(memCardAddr<memCard.data.size())
			{
				memCard.data[memCardAddr]=data;
				memCard.modified=true;
			}
		}
	}
}

////////////////////////////////////////////////////////////

/* virtual */ unsigned int TownsOsROMAccess::FetchByte(unsigned int physAddr) const
{
	return physMemPtr->dosRom[physAddr&TOWNSADDR_OSROM_AND];
}
/* virtual */ unsigned int TownsOsROMAccess::FetchWord(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_OSROM_AND;
	if(physAddr+1<physMemPtr->dosRom.size())
	{
		auto *ROMPtr=physMemPtr->dosRom.data()+physAddr;
		return ROMPtr[0]|(ROMPtr[1]<<8);
	}
	else
	{
		cpuPtr->Abort("Cross-Border Access to OS ROM");
	}
	return 0xffff;
}
/* virtual */ unsigned int TownsOsROMAccess::FetchDword(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_OSROM_AND;
	if(physAddr+3<physMemPtr->dosRom.size())
	{
		auto *ROMPtr=physMemPtr->dosRom.data()+physAddr;
		return ROMPtr[0]|(ROMPtr[1]<<8)|(ROMPtr[2]<<16)|(ROMPtr[3]<<24);
	}
	else
	{
		cpuPtr->Abort("Cross-Border Access to OS ROM");
	}
	return 0xffffffff;
}
/* virtual */ void TownsOsROMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsFontROMAccess::FetchByte(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_FONT_AND;
	return physMemPtr->fontRom[physAddr];
}
/* virtual */ void TownsFontROMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
}

////////////////////////////////////////////////////////////

/* virtual */ unsigned int TownsFont20ROMAccess::FetchByte(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_FONT20_AND;
	return physMemPtr->font20Rom[physAddr];
}
/* virtual */ void TownsFont20ROMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
}

////////////////////////////////////////////////////////////

#include "rf5c68.h"

/* virtual */ unsigned int TownsWaveRAMAccess::FetchByte(unsigned int physAddr) const
{
	return pcmPtr->ReadWaveRAM(physAddr&TOWNSADDR_WAVERAM_WINDOW_AND);
}
/* virtual */ void TownsWaveRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	pcmPtr->WriteWaveRAM(physAddr&TOWNSADDR_WAVERAM_WINDOW_AND,data);
}
TownsWaveRAMAccess::TownsWaveRAMAccess(class RF5C68 *pcmPtr)
{
	this->pcmPtr=pcmPtr;
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsSysROMAccess::FetchByte(unsigned int physAddr) const
{
	return physMemPtr->sysRom[physAddr&TOWNSADDR_SYSROM_AND];
}
/* virtual */ void TownsSysROMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
}
/* virtual */ MemoryAccess::ConstMemoryWindow TownsSysROMAccess::GetConstMemoryWindow(unsigned int physAddr) const
{
	MemoryAccess::ConstMemoryWindow memWin;
	memWin.ptr=physMemPtr->sysRom.data()+((physAddr&(~0xfff)&TOWNSADDR_SYSROM_AND));
	return memWin;
}

////////////////////////////////////////////////////////////

/* virtual */ unsigned int TownsMartyEXROMAccess::FetchByte(unsigned int physAddr) const
{
	return physMemPtr->martyRom[physAddr-TOWNSADDR_MARTY_ROM0_BASE];
}
/* virtual */ void TownsMartyEXROMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
}
/* virtual */ MemoryAccess::ConstMemoryWindow TownsMartyEXROMAccess::GetConstMemoryWindow(unsigned int physAddr) const
{
	MemoryAccess::ConstMemoryWindow memWin;
	memWin.ptr=physMemPtr->martyRom.data()+(physAddr-TOWNSADDR_MARTY_ROM0_BASE);
	return memWin;
}
