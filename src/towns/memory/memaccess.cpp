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
	if(physAddr<state.RAM.size()-1)
	{
		return state.RAM[physAddr]|(state.RAM[physAddr+1]<<8);
	}
	return 0xffff;
}
/* virtual */ unsigned int TownsMainRAMAccess::FetchDword(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	if(physAddr<state.RAM.size()-3)
	{
		return state.RAM[physAddr]|(state.RAM[physAddr+1]<<8)|(state.RAM[physAddr+2]<<16)|(state.RAM[physAddr+3]<<24);
	}
	else if(physAddr<state.RAM.size()-2)
	{
		return state.RAM[physAddr]|(state.RAM[physAddr+1]<<8)|(state.RAM[physAddr+2]<<16);
	}
	else if(physAddr<state.RAM.size()-1)
	{
		return state.RAM[physAddr]|(state.RAM[physAddr+1]<<8);
	}
	else if(physAddr<state.RAM.size())
	{
		return state.RAM[physAddr];
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
	if(physAddr<state.RAM.size()-1)
	{
		state.RAM[physAddr]=data&255;
		state.RAM[physAddr+1]=(data>>8)&255;
	}
	else if(physAddr<state.RAM.size())
	{
		state.RAM[physAddr]=data&255;
	}
}
/* virtual */ void TownsMainRAMAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	if(physAddr<state.RAM.size()-3)
	{
		state.RAM[physAddr]=data&255;
		state.RAM[physAddr+1]=(data>>8)&255;
		state.RAM[physAddr+2]=(data>>16)&255;
		state.RAM[physAddr+3]=(data>>24)&255;
	}
	else if(physAddr<state.RAM.size()-2)
	{
		state.RAM[physAddr]=data&255;
		state.RAM[physAddr+1]=(data>>8)&255;
		state.RAM[physAddr+2]=(data>>16)&255;
	}
	else if(physAddr<state.RAM.size()-1)
	{
		state.RAM[physAddr]=data&255;
		state.RAM[physAddr+1]=(data>>8)&255;
	}
	else if(physAddr<state.RAM.size())
	{
		state.RAM[physAddr]=data&255;
	}
}


////////////////////////////////////////////////////////////


TownsMainRAMorFMRVRAMAccess::TownsMainRAMorFMRVRAMAccess()
{
	breakOnFMRVRAMWrite=false;
	breakOnFMRVRAMRead=false;
	breakOnCVRAMWrite=false;
	breakOnCVRAMRead=false;
}

/* virtual */ unsigned int TownsMainRAMorSysROMAccess::FetchByte(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	if(true==state.sysRomMapping && 0xF8000<=physAddr && physAddr<=0xFFFFF)
	{
		return physMemPtr->sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE];
	}
	else if(physAddr<state.RAM.size())
	{
		return state.RAM[physAddr];
	}
	return 0xff;
}
///* virtual */ unsigned int TownsMainRAMorSysROMAccess::FetchWord(unsigned int physAddr) const
//{
//	auto &state=physMemPtr->state;
//	if(true==state.sysRomMapping)
//	{
//		if(0xF8000<=physAddr && physAddr<=0xFFFFE)
//		{
//			return sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE]|(sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE+1]<<8);
//		}
//		else if(0xF8000<=physAddr && physAddr<=0xFFFFF)
//		{
//			return sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE];
//		}
//	}
//	if(physAddr<state.RAM.size()-1)
//	{
//		return state.RAM[physAddr]|(state.RAM[physAddr+1]<<8);
//	}
//	else if(physAddr==state.RAM.size()-1)
//	{
//		return state.RAM[physAddr];
//	}
//	return 0xffff;
//}
///* virtual */ unsigned int TownsMainRAMorSysROMAccess::FetchDword(unsigned int physAddr) const
//{
//	auto &state=physMemPtr->state;
//	if(true==state.sysRomMapping)
//	{
//		if(0xF8000<=physAddr && physAddr<=0xFFFFC)
//		{
//			return sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE]|(sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE+1]<<8)|(sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE+2]<<16)|(sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE+3]<<24);
//		}
//		else if(0xF8000<=physAddr && physAddr<=0xFFFFD)
//		{
//			return sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE]|(sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE+1]<<8)|(sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE+2]<<16);
//		}
//		else if(0xF8000<=physAddr && physAddr<=0xFFFFE)
//		{
//			return sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE]|(sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE+1]<<8);
//		}
//		else if(0xF8000<=physAddr && physAddr<=0xFFFFF)
//		{
//			return sysRom[physAddr-TOWNSADDR_FMR_VRAM_BASE];
//		}
//	}
//	if(physAddr<state.RAM.size()-3)
//	{
//		return state.RAM[physAddr]|(state.RAM[physAddr+1]<<8)|(state.RAM[physAddr+2]<<16)|(state.RAM[physAddr+3]<<24);
//	}
//	else if(physAddr<state.RAM.size()-2)
//	{
//		return state.RAM[physAddr]|(state.RAM[physAddr+1]<<8)|(state.RAM[physAddr+2]<<16);
//	}
//	else if(physAddr<state.RAM.size()-1)
//	{
//		return state.RAM[physAddr]|(state.RAM[physAddr+1]<<8);
//	}
//	else if(physAddr==state.RAM.size()-1)
//	{
//		return state.RAM[physAddr];
//	}
//	return 0xffffffff;
//}
/* virtual */ void TownsMainRAMorSysROMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &state=physMemPtr->state;
	if(true==state.sysRomMapping && 0xF8000<=physAddr && physAddr<=0xFFFFF)
	{
		// ROM mode no writing
	}
	else if(physAddr<state.RAM.size())
	{
		state.RAM[physAddr]=data;
	}
}
///* virtual */ void TownsMainRAMorSysROMAccess::StoreWord(unsigned int physAddr,unsigned int data)
//{
//	auto &state=physMemPtr->state;
//	if(true==state.sysRomMapping && 0xF8000<=physAddr && physAddr<=0xFFFFF)
//	{
//		// ROM mode no writing
//	}
//	else if(physAddr<state.RAM.size()-1)
//	{
//		state.RAM[physAddr]=data&255;
//		state.RAM[physAddr+1]=(data>>8)&255;
//	}
//	else if(physAddr<state.RAM.size())
//	{
//		state.RAM[physAddr]=data;
//	}
//}
///* virtual */ void TownsMainRAMorSysROMAccess::StoreDword(unsigned int physAddr,unsigned int data)
//{
//	auto &state=physMemPtr->state;
//	if(true==state.sysRomMapping && 0xF8000<=physAddr && physAddr<=0xFFFFF)
//	{
//		// ROM mode no writing
//	}
//	else if(physAddr<state.RAM.size()-3)
//	{
//		state.RAM[physAddr]=data&255;
//		state.RAM[physAddr+1]=(data>>8)&255;
//		state.RAM[physAddr+2]=(data>>16)&255;
//		state.RAM[physAddr+3]=(data>>24)&255;
//	}
//	else if(physAddr<state.RAM.size()-2)
//	{
//		state.RAM[physAddr]=data&255;
//		state.RAM[physAddr+1]=(data>>8)&255;
//		state.RAM[physAddr+2]=(data>>16)&255;
//	}
//	else if(physAddr<state.RAM.size()-1)
//	{
//		state.RAM[physAddr]=data&255;
//		state.RAM[physAddr+1]=(data>>8)&255;
//	}
//	else if(physAddr<state.RAM.size())
//	{
//		state.RAM[physAddr]=data;
//	}
//}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsMainRAMorFMRVRAMAccess::FetchByte(unsigned int physAddr) const
{
	if(true==physMemPtr->state.FMRVRAM &&
	   TOWNSADDR_FMR_VRAM_BASE<=physAddr && physAddr<=TOWNSADDR_FMR_CVRAM_END)
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
	else
	{
		return physMemPtr->state.RAM[physAddr];
	}
	return 0xff;
}
/* virtual */ void TownsMainRAMorFMRVRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	if(true==physMemPtr->state.FMRVRAM &&
	   TOWNSADDR_FMR_VRAM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_CVRAM_END)
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
	else
	{
		physMemPtr->state.RAM[physAddr]=data;
	}
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsDicROMandDicRAMAccess::FetchByte(unsigned int physAddr) const
{
	auto &physMem=*physMemPtr;
	if(TOWNSADDR_FMR_DICROM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_RESERVED_END)
	{
		if(true==physMem.state.dicRom)
		{
			if(TOWNSADDR_FMR_DICROM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_DICROM_END) // Dic ROM
			{
				unsigned int offset=32768*physMem.state.DICROMBank+(physAddr-TOWNSADDR_FMR_DICROM_BASE);
				return physMem.dicRom[offset];
			}
			else if(TOWNSADDR_BACKUP_RAM_BASE<=physAddr && physAddr<TOWNSADDR_BACKUP_RAM_END) // 
			{
				return physMem.state.DICRAM[physAddr-TOWNSADDR_BACKUP_RAM_BASE];
			}
		}
		else
		{
			return physMemPtr->state.RAM[physAddr];
		}
	}
	else if(0xC2080000<=physAddr && physAddr<=0xC20FFFFF)
	{
		return physMem.dicRom[physAddr-0xC2080000];
	}
	else if(0xC2140000<=physAddr && physAddr<=0xC2141FFF)
	{
		return physMem.state.DICRAM[physAddr-0xC2140000];
	}
	return 0xff;
}
/* virtual */ void TownsDicROMandDicRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &physMem=*physMemPtr;
	if(TOWNSADDR_FMR_DICROM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_RESERVED_END)
	{
		if(true==physMem.state.dicRom)
		{
			if(TOWNSADDR_BACKUP_RAM_BASE<=physAddr && physAddr<TOWNSADDR_BACKUP_RAM_END) // 
			{
				physMem.state.DICRAM[physAddr-TOWNSADDR_BACKUP_RAM_BASE]=data;
			}
		}
		else
		{
			physMemPtr->state.RAM[physAddr]=data;
		}
	}
	else if(0xC2140000<=physAddr && physAddr<=0xC2141FFF)
	{
		physMem.state.DICRAM[physAddr-0xC2140000]=data;
	}
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsVRAMAccess::FetchByte(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	if(0x80000000<=physAddr && physAddr<0x80080000)
	{
		return state.VRAM[physAddr-0x80000000];
	}
	return 0xff;
}
/* virtual */ void TownsVRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &state=physMemPtr->state;
	if(0x80000000<=physAddr && physAddr<0x80080000)
	{
		state.VRAM[physAddr-0x80000000]=data;
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
