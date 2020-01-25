#include "memaccess.h"



TownsMemAccess::TownsMemAccess()
{
	physMemPtr=nullptr;
}
void TownsMemAccess::SetPhysicalMemoryPointer(TownsPhysicalMemory *ptr)
{
	physMemPtr=ptr;
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
///* virtual */ unsigned int TownsMainRAMAccess::FetchWord(unsigned int physAddr) const
//{
//	auto &state=physMemPtr->state;
//	if(physAddr<state.RAM.size()-1)
//	{
//		return state.RAM[physAddr]|(state.RAM[physAddr+1]<<8);
//	}
//	return 0xffff;
//}
///* virtual */ unsigned int TownsMainRAMAccess::FetchDword(unsigned int physAddr) const
//{
//	auto &state=physMemPtr->state;
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
//	else if(physAddr<state.RAM.size())
//	{
//		return state.RAM[physAddr];
//	}
//	return 0xffffff;
//}
/* virtual */ void TownsMainRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &state=physMemPtr->state;
	if(physAddr<state.RAM.size())
	{
		state.RAM[physAddr]=data;
	}
}
///* virtual */ void TownsMainRAMAccess::StoreWord(unsigned int physAddr,unsigned int data)
//{
//	auto &state=physMemPtr->state;
//	if(physAddr<state.RAM.size()-1)
//	{
//		state.RAM[physAddr]=data&255;
//		state.RAM[physAddr+1]=(data>>8)&255;
//	}
//	else if(physAddr<state.RAM.size())
//	{
//		state.RAM[physAddr]=data&255;
//	}
//}
///* virtual */ void TownsMainRAMAccess::StoreDword(unsigned int physAddr,unsigned int data)
//{
//	auto &state=physMemPtr->state;
//	if(physAddr<state.RAM.size()-3)
//	{
//		state.RAM[physAddr]=data&255
//		state.RAM[physAddr+1]=(data>>8)&255;
//		state.RAM[physAddr+2]=(data>>16)&255;
//		state.RAM[physAddr+3]=(data>>24)&255;
//	}
//	else if(physAddr<state.RAM.size()-2)
//	{
//		state.RAM[physAddr]=data&255
//		state.RAM[physAddr+1]=(data>>8)&255;
//		state.RAM[physAddr+2]=(data>>16)&255;
//	}
//	else if(physAddr<state.RAM.size()-1)
//	{
//		state.RAM[physAddr]=data&255
//		state.RAM[physAddr+1]=(data>>8)&255;
//	}
//	else if(physAddr<state.RAM.size())
//	{
//		state.RAM[physAddr]=data&255
//	}
//}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsMainRAMorSysROMAccess::FetchByte(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	if(true==state.sysRomMapping && 0xF8000<=physAddr && physAddr<=0xFFFFF)
	{
		return physMemPtr->sysRom[physAddr-0xC0000];
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
//			return sysRom[physAddr-0xC0000]|(sysRom[physAddr-0xC0000+1]<<8);
//		}
//		else if(0xF8000<=physAddr && physAddr<=0xFFFFF)
//		{
//			return sysRom[physAddr-0xC0000];
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
//			return sysRom[physAddr-0xC0000]|(sysRom[physAddr-0xC0000+1]<<8)|(sysRom[physAddr-0xC0000+2]<<16)|(sysRom[physAddr-0xC0000+3]<<24);
//		}
//		else if(0xF8000<=physAddr && physAddr<=0xFFFFD)
//		{
//			return sysRom[physAddr-0xC0000]|(sysRom[physAddr-0xC0000+1]<<8)|(sysRom[physAddr-0xC0000+2]<<16);
//		}
//		else if(0xF8000<=physAddr && physAddr<=0xFFFFE)
//		{
//			return sysRom[physAddr-0xC0000]|(sysRom[physAddr-0xC0000+1]<<8);
//		}
//		else if(0xF8000<=physAddr && physAddr<=0xFFFFF)
//		{
//			return sysRom[physAddr-0xC0000];
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
	return 0xff;
}
/* virtual */ void TownsMainRAMorFMRVRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsDicROMandDicRAMAccess::FetchByte(unsigned int physAddr) const
{
	return 0xff;
}
/* virtual */ void TownsDicROMandDicRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
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
	return 0xff;
}
/* virtual */ void TownsSpriteRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
}


////////////////////////////////////////////////////////////


/* virtual */ unsigned int TownsOsROMAccess::FetchByte(unsigned int physAddr) const
{
	return 0xff;
}
/* virtual */ void TownsOsROMAccess::StoreByte(unsigned int physAddr,unsigned char data)
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
