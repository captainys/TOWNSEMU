#include "i486.h"
#include "i486debug.h"



#include "memaccess.h"
#include "townsdef.h"
#include "cpputil.h"



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
	auto &state=physMemPtr->state;
	if(physAddr<state.RAM.size())
	{
		return state.RAM[physAddr];
	}
	return 0xff;
}
/* virtual */ unsigned int TownsMainRAMAccess::FetchWord(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	if(physAddr<state.RAM.size()-1)
	{
		return RAMPtr[0]|(RAMPtr[1]<<8);
	}
	return 0xffff;
}
/* virtual */ unsigned int TownsMainRAMAccess::FetchDword(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	if(physAddr<state.RAM.size()-3)
	{
		return RAMPtr[0]|(RAMPtr[1]<<8)|(RAMPtr[2]<<16)|(RAMPtr[3]<<24);
	}
	else if(physAddr<state.RAM.size()-2)
	{
		return RAMPtr[1]|(RAMPtr[1]<<8)|(RAMPtr[2]<<16);
	}
	else if(physAddr<state.RAM.size()-1)
	{
		return RAMPtr[2]|(RAMPtr[1]<<8);
	}
	else if(physAddr<state.RAM.size())
	{
		return RAMPtr[3];
	}
	return 0xffffff;
}
/* virtual */ void TownsMainRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &state=physMemPtr->state;
	if(physAddr<state.RAM.size())
	{
		state.RAM[physAddr]=data;
	}
}
/* virtual */ void TownsMainRAMAccess::StoreWord(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	if(physAddr<state.RAM.size()-1)
	{
		RAMPtr[0]=data&255;
		RAMPtr[1]=(data>>8)&255;
	}
	else if(physAddr<state.RAM.size())
	{
		RAMPtr[0]=data&255;
	}
}
/* virtual */ void TownsMainRAMAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	if(physAddr<state.RAM.size()-3)
	{
		RAMPtr[0]=data&255;
		RAMPtr[1]=(data>>8)&255;
		RAMPtr[2]=(data>>16)&255;
		RAMPtr[3]=(data>>24)&255;
	}
	else if(physAddr<state.RAM.size()-2)
	{
		RAMPtr[0]=data&255;
		RAMPtr[1]=(data>>8)&255;
		RAMPtr[2]=(data>>16)&255;
	}
	else if(physAddr<state.RAM.size()-1)
	{
		RAMPtr[0]=data&255;
		RAMPtr[1]=(data>>8)&255;
	}
	else if(physAddr<state.RAM.size())
	{
		RAMPtr[0]=data&255;
	}
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
		return ROMPtr[0]|(ROMPtr[1]<<8);
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
		return ROMPtr[0]|(ROMPtr[1]<<8)|(ROMPtr[2]<<16)|(ROMPtr[3]<<24);
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
	if(TOWNSADDR_FMR_VRAM_BASE<=physAddr && physAddr<=TOWNSADDR_FMR_CVRAM_END)
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
				break;

			case TOWNSMEMIO_FIRQ://               0x000CFF84, // [2] pp.22,pp.95 Always zero in FM TOWNS
				return 0;

			case TOWNSMEMIO_FMR_HSYNC_VSYNC://    0x000CFF86, // [2] pp.22,pp.160
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
			}
		}
		else if(TOWNSADDR_FMR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_VRAM_END) /// FMR VRAM Plane Access
		{
			const auto FMRAddr=physAddr-TOWNSADDR_FMR_VRAM_BASE;
			const auto VRAMAddr=(FMRAddr<<2)+physMemPtr->state.FMRVRAMWriteOffset;
			auto shift=(physMemPtr->state.FMRVRAMMask>>6)&3;
			unsigned char andPtnHigh=(0x10<<shift);
			unsigned char andPtnLow=(1<<shift);
			unsigned char orPtnHigh=0x80;
			unsigned char orPtnLow=0x40;
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
		else if(TOWNSADDR_FMR_VRAM_END<=physAddr && physAddr<TOWNSADDR_FMR_CVRAM_END) /// FMR I/OCVRAM Access
		{
			if(true==breakOnCVRAMRead &&
			   nullptr!=cpuPtr &&
			   nullptr!=cpuPtr->debuggerPtr)
			{
				cpuPtr->debuggerPtr->ExternalBreak("CVRAM Read "+cpputil::Uitox(physAddr));
			}
			return physMemPtr->state.spriteRAM[physAddr-TOWNSADDR_FMR_VRAM_END];
		}
		else if(TOWNSADDR_FMR_ANK8_BASE<=physAddr && physAddr<TOWNSADDR_FMR_ANK8_END)
		{
			return physMemPtr->fontRom[TOWNSADDR_FMR_ANK8_MAP+physAddr-TOWNSADDR_FMR_ANK8_BASE];
		}
		else if(TOWNSADDR_FMR_ANK16_BASE<=physAddr && physAddr<TOWNSADDR_FMR_ANK16_END)
		{
			return physMemPtr->fontRom[TOWNSADDR_FMR_ANK16_MAP+physAddr-TOWNSADDR_FMR_ANK16_BASE];
		}
	}
	return 0xff;
}
/* virtual */ unsigned int TownsFMRVRAMAccess::FetchWord(unsigned int physAddr) const
{
	if(TOWNSADDR_FMR_VRAM_BASE<=physAddr && physAddr<=TOWNSADDR_FMR_CVRAM_END)
	{
		return TownsMemAccess::FetchWord(physAddr);
	}
	return 0xffff;
}
/* virtual */ unsigned int TownsFMRVRAMAccess::FetchDword(unsigned int physAddr) const
{
	if(TOWNSADDR_FMR_VRAM_BASE<=physAddr && physAddr<=TOWNSADDR_FMR_CVRAM_END)
	{
		return TownsMemAccess::FetchDword(physAddr);
	}
	return 0xffffffff;
}
/* virtual */ void TownsFMRVRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	if(TOWNSADDR_FMR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_CVRAM_END)
	{
		if((TOWNS_MEMIO_1_LOW<=physAddr && physAddr<=TOWNS_MEMIO_1_HIGH) ||
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
				physMemPtr->state.FMRDisplayMode=data;
				break;
			case TOWNSMEMIO_FMR_GVRAMPAGESEL://   0x000CFF83, // [2] pp.22,pp.159
				physMemPtr->state.FMRVRAMWriteOffset=(0!=(data&0x10) ? 0x40000 : 0);
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
			}
		}
		else if(TOWNSADDR_FMR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_VRAM_END)
		{
			// Assume screen mode 1 and 2.
			//   Logical Resolution 640x819
			//   Visible Resolution 640x400 or 640x200
			// FMRVram 8pixels per byte, 640pixels=80bytes.
			// TownsVRAM 2pixels per byte, 640pixels=320bytes.
			// Just multiply 4 to get TownsVRAM address.
			const auto FMRAddr=physAddr-TOWNSADDR_FMR_VRAM_BASE;
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
		else if(TOWNSADDR_FMR_VRAM_END<=physAddr && physAddr<TOWNSADDR_FMR_CVRAM_END) // Except I/O.
		{
			if(true==breakOnCVRAMWrite &&
			   nullptr!=cpuPtr &&
			   nullptr!=cpuPtr->debuggerPtr)
			{
				cpuPtr->debuggerPtr->ExternalBreak("CVRAM Write "+cpputil::Uitox(physAddr));
			}
			physMemPtr->state.spriteRAM[physAddr-TOWNSADDR_FMR_VRAM_END]=data;
			physMemPtr->state.TVRAMWrite=true;
		}
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


/* virtual */ unsigned int TownsMappedDicROMandDicRAMAccess::FetchByte(unsigned int physAddr) const
{
	if(TOWNSADDR_FMR_DICROM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_DICROM_END) // Dic ROM
	{
		unsigned int offset=32768*physMemPtr->state.DICROMBank+(physAddr-TOWNSADDR_FMR_DICROM_BASE);
		return physMemPtr->dicRom[offset];
	}
	else if(TOWNSADDR_BACKUP_RAM_BASE<=physAddr && physAddr<TOWNSADDR_BACKUP_RAM_END) // 
	{
		return physMemPtr->state.DICRAM[physAddr-TOWNSADDR_BACKUP_RAM_BASE];
	}
	return 0xFF;
}
/* virtual */ void TownsMappedDicROMandDicRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	if(TOWNSADDR_BACKUP_RAM_BASE<=physAddr && physAddr<TOWNSADDR_BACKUP_RAM_END) // 
	{
		physMemPtr->state.DICRAM[physAddr-TOWNSADDR_BACKUP_RAM_BASE]=data;
	}
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsNativeDicROMandDicRAMAccess::FetchByte(unsigned int physAddr) const
{
	auto &physMem=*physMemPtr;
	if(0xC2080000<=physAddr && physAddr<=0xC20FFFFF)
	{
		return physMem.dicRom[physAddr-0xC2080000];
	}
	else if(0xC2140000<=physAddr && physAddr<=0xC2141FFF)
	{
		return physMem.state.DICRAM[physAddr-0xC2140000];
	}
	return 0xff;
}
/* virtual */ void TownsNativeDicROMandDicRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &physMem=*physMemPtr;
	if(0xC2140000<=physAddr && physAddr<=0xC2141FFF)
	{
		physMem.state.DICRAM[physAddr-0xC2140000]=data;
	}
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsVRAMAccess::FetchByte(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM_BASE];
	}
	if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE];
	}
	return 0xff;
}
/* virtual */ unsigned int TownsVRAMAccess::FetchWord(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END-1)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM_BASE]|(state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+1]<<8);
	}
	else if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM_BASE];
	}

	if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END-1)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE]|(state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+1]<<8);
	}
	else if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE];
	}

	return 0xffff;
}
/* virtual */ unsigned int TownsVRAMAccess::FetchDword(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END-3)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM_BASE]|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+1]<<8)|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+2]<<16)|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+3]<<24);
	}
	else if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END-2)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM_BASE]|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+1]<<8)|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+2]<<16);
	}
	else if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END-1)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM_BASE]|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+1]<<8);
	}
	else if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM_BASE];
	}


	if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END-3)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE]|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+1]<<8)|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+2]<<16)|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+3]<<24);
	}
	else if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END-2)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE]|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+1]<<8)|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+2]<<16);
	}
	else if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END-1)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE]|
		      (state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+1]<<8);
	}
	else if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END)
	{
		return state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE];
	}


	return 0xffffffff;
}
/* virtual */ void TownsVRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &state=physMemPtr->state;
	if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE]=data;
	}
	if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE]=data;
	}
}
/* virtual */ void TownsVRAMAccess::StoreWord(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END-1)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE  ]= data    &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+1]=(data>>8)&255;
	}
	else if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE]=data;
	}

	if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END-1)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE  ]= data    &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+1]=(data>>8)&255;
	}
	else if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE]=data;
	}
}
/* virtual */ void TownsVRAMAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END-3)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE  ]= data     &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+1]=(data>>8) &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+2]=(data>>16)&255;
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+3]=(data>>24)&255;
	}
	else if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END-2)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE  ]= data     &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+1]=(data>>8) &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+2]=(data>>16)&255;
	}
	else if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END-1)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE  ]= data    &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE+1]=(data>>8)&255;
	}
	else if(TOWNSADDR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_VRAM_END)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM_BASE]=data;
	}


	if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END-3)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE  ]= data     &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+1]=(data>>8) &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+2]=(data>>16)&255;
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+3]=(data>>24)&255;
	}
	else if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END-2)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE  ]= data     &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+1]=(data>>8) &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+2]=(data>>16)&255;
	}
	else if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END-1)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE  ]= data    &255;
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE+1]=(data>>8)&255;
	}
	else if(TOWNSADDR_VRAM2_BASE<=physAddr && physAddr<TOWNSADDR_VRAM2_END)
	{
		state.VRAM[physAddr-TOWNSADDR_VRAM2_BASE]=data;
	}
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsSpriteRAMAccess::FetchByte(unsigned int physAddr) const
{
	// 0x81000000,0x8101FFFF
	auto &state=physMemPtr->state;
	return state.spriteRAM[physAddr-0x81000000];
}
/* virtual */ void TownsSpriteRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &state=physMemPtr->state;
	state.spriteRAM[physAddr-0x81000000]=data;
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsOsROMAccess::FetchByte(unsigned int physAddr) const
{
	physAddr-=0xC2000000;
	if(physAddr<physMemPtr->dosRom.size())
	{
		return physMemPtr->dosRom[physAddr];
	}
	return 0xff;
}
/* virtual */ void TownsOsROMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsFontROMAccess::FetchByte(unsigned int physAddr) const
{
	physAddr-=0xC2100000;
	if(physAddr<physMemPtr->fontRom.size())
	{
		return physMemPtr->fontRom[physAddr];
	}
	return 0xff;
}
/* virtual */ void TownsFontROMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsWaveRAMAccess::FetchByte(unsigned int physAddr) const
{
	return 0xff;
}
/* virtual */ void TownsWaveRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsSysROMAccess::FetchByte(unsigned int physAddr) const
{
	if(0xFFFC0000<=physAddr)
	{
		return physMemPtr->sysRom[physAddr-0xFFFC0000];
	}
	return 0xff;
}
/* virtual */ void TownsSysROMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
}


////////////////////////////////////////////////////////////
