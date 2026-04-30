/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include "memory.h"
#include "ramrom.h"
#include "physmem.h"
#include "towns.h"


////////////////////////////////////////////////////////////


#define DefaultFetchByteDMA(Prefix) \
inline unsigned int TownsPhysicalMemory::Prefix##FetchByteDMA(unsigned int physAddr) const \
{ \
	return Prefix##FetchByte(physAddr); \
}

#define DefaultFetchWord(Prefix) \
inline unsigned int TownsPhysicalMemory::Prefix##FetchWord(unsigned int physAddr) const \
{ \
	return Prefix##FetchByte(physAddr)|(Prefix##FetchByte(physAddr+1)<<8); \
}

#define DefaultFetchDword(Prefix) \
inline unsigned int TownsPhysicalMemory::Prefix##FetchDword(unsigned int physAddr) const \
{ \
	return Prefix##FetchByte(physAddr)| \
	      (Prefix##FetchByte(physAddr+1)<<8)| \
	      (Prefix##FetchByte(physAddr+2)<<16)| \
	      (Prefix##FetchByte(physAddr+3)<<24); \
}

#define DefaultStoreByteDMA(Prefix) \
inline void TownsPhysicalMemory::Prefix##StoreByteDMA(unsigned int physAddr,unsigned char data) \
{ \
	Prefix##StoreByte(physAddr,data); \
}

#define DefaultStoreWord(Prefix) \
inline void TownsPhysicalMemory::Prefix##StoreWord(unsigned int physAddr,unsigned int data) \
{ \
	Prefix##StoreByte(physAddr,data&255); \
	Prefix##StoreByte(physAddr+1,(data>>8)&255); \
}

#define DefaultStoreDword(Prefix) \
inline void TownsPhysicalMemory::Prefix##StoreDword(unsigned int physAddr,unsigned int data) \
{ \
	Prefix##StoreByte(physAddr,data&255); \
	Prefix##StoreByte(physAddr+1,(data>>8)&255); \
	Prefix##StoreByte(physAddr+2,(data>>16)&255); \
	Prefix##StoreByte(physAddr+3,(data>>24)&255); \
}

#define DefaultGetConstMemoryWindow(Prefix) \
MemoryAccess::ConstMemoryWindow TownsPhysicalMemory::Prefix##GetConstMemoryWindow(unsigned int physAddr) const \
{ \
	return EmptyConstMemoryWindow(); \
}

#define DefaultGetMemoryWindow(Prefix) \
MemoryAccess::MemoryWindow TownsPhysicalMemory::Prefix##GetMemoryWindow(unsigned int physAddr) \
{ \
	return EmptyMemoryWindow(); \
}


////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::MainRAMFetchByte(unsigned int physAddr) const
{
	return state.RAM[physAddr];
}

inline unsigned int TownsPhysicalMemory::MainRAMFetchWord(unsigned int physAddr) const
{
	auto *RAMPtr=state.RAM.data()+physAddr;
	return cpputil::GetWord(RAMPtr);
}

inline unsigned int TownsPhysicalMemory::MainRAMFetchDword(unsigned int physAddr) const
{
	auto *RAMPtr=state.RAM.data()+physAddr;
	return cpputil::GetDword(RAMPtr);
}

inline void TownsPhysicalMemory::MainRAMStoreByte(unsigned int physAddr,unsigned char data)
{
	state.RAM[physAddr]=data;
}

inline void TownsPhysicalMemory::MainRAMStoreWord(unsigned int physAddr,unsigned int data)
{
	auto *RAMPtr=state.RAM.data()+physAddr;
	cpputil::PutWord(RAMPtr,(unsigned short)data);
}

inline void TownsPhysicalMemory::MainRAMStoreDword(unsigned int physAddr,unsigned int data)
{
	auto *RAMPtr=state.RAM.data()+physAddr;
	cpputil::PutDword(RAMPtr,data);
}


inline MemoryAccess::ConstMemoryWindow TownsPhysicalMemory::MainRAMGetConstMemoryWindow(unsigned int physAddr) const
{
	MemoryAccess::ConstMemoryWindow memWin;
	memWin.ptr=state.RAM.data()+(physAddr&(~0xfff));
	return memWin;
}

inline MemoryAccess::MemoryWindow TownsPhysicalMemory::MainRAMGetMemoryWindow(unsigned int physAddr)
{
	MemoryAccess::MemoryWindow memWin;
	memWin.ptr=state.RAM.data()+(physAddr&(~0xfff));
	return memWin;
}

////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::NativeDICROMFetchByte(unsigned int physAddr) const
{
	return dicRom[physAddr&TOWNSADDR_NATIVE_DICROM_AND];
}

DefaultFetchWord(NativeDICROM)

DefaultFetchDword(NativeDICROM)

inline void TownsPhysicalMemory::NativeDICROMStoreByte(unsigned int physAddr,unsigned char data)
{
}

DefaultStoreWord(NativeDICROM)

DefaultStoreDword(NativeDICROM)

DefaultGetConstMemoryWindow(NativeDICROM)

DefaultGetMemoryWindow(NativeDICROM)

////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::NativeCMOSFetchByte(unsigned int physAddr) const
{
	return state.CMOSRAM[physAddr&TOWNSADDR_NATIVE_CMOSRAM_AND];
}

DefaultFetchWord(NativeCMOS)

DefaultFetchDword(NativeCMOS)

inline void TownsPhysicalMemory::NativeCMOSStoreByte(unsigned int physAddr,unsigned char data)
{
	state.CMOSRAM[physAddr&TOWNSADDR_NATIVE_CMOSRAM_AND]=data;
}

DefaultStoreWord(NativeCMOS)

DefaultStoreDword(NativeCMOS)

DefaultGetConstMemoryWindow(NativeCMOS)

DefaultGetMemoryWindow(NativeCMOS)

////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::MappedDICFetchByte(unsigned int physAddr) const
{
	if(TOWNSADDR_FMR_DICROM_BASE<=physAddr && physAddr<TOWNSADDR_FMR_DICROM_END) // Dic ROM
	{
		unsigned int offset=32768*state.DICROMBank+(physAddr-TOWNSADDR_FMR_DICROM_BASE);
		return dicRom[offset];
	}
	else if(TOWNSADDR_BACKUP_RAM_BASE<=physAddr && physAddr<TOWNSADDR_BACKUP_RAM_END) // 
	{
		return state.CMOSRAM[physAddr-TOWNSADDR_BACKUP_RAM_BASE];
	}
	return 0xFF;
}

DefaultFetchWord(MappedDIC)

DefaultFetchDword(MappedDIC)

inline void TownsPhysicalMemory::MappedDICStoreByte(unsigned int physAddr,unsigned char data)
{
	if(TOWNSADDR_BACKUP_RAM_BASE<=physAddr && physAddr<TOWNSADDR_BACKUP_RAM_END) // 
	{
		state.CMOSRAM[physAddr-TOWNSADDR_BACKUP_RAM_BASE]=data;
	}
}

DefaultStoreWord(MappedDIC)

DefaultStoreDword(MappedDIC)

DefaultGetConstMemoryWindow(MappedDIC)

DefaultGetMemoryWindow(MappedDIC)

////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::MappedSYSROMFetchByte(unsigned int physAddr) const
{
	unsigned int offset=physAddr-TOWNSADDR_SYSROM_MAP_BASE;
	auto ROMPtr=sysRom.data()+TOWNSADDR_SYSROM_MAP_OFFSET_DIFFERENCE+offset;
	if(offset<TOWNSADDR_SYSROM_MAP_SIZE)
	{
		return ROMPtr[0];
	}
	Abort("Out-of-bound access to Mapped SYSROM");
	return 0xFFFFFFFF;
}

inline unsigned int TownsPhysicalMemory::MappedSYSROMFetchWord(unsigned int physAddr) const
{
	unsigned int offset=physAddr-TOWNSADDR_SYSROM_MAP_BASE;
	auto ROMPtr=sysRom.data()+TOWNSADDR_SYSROM_MAP_OFFSET_DIFFERENCE+offset;
	if(offset<TOWNSADDR_SYSROM_MAP_SIZE-1)
	{
		return cpputil::GetWord(ROMPtr);
	}
	Abort("Cross-Border WORD access to Mapped SYSROM");
	return 0xFFFFFFFF;
}

inline unsigned int TownsPhysicalMemory::MappedSYSROMFetchDword(unsigned int physAddr) const
{
	unsigned int offset=physAddr-TOWNSADDR_SYSROM_MAP_BASE;
	auto ROMPtr=sysRom.data()+TOWNSADDR_SYSROM_MAP_OFFSET_DIFFERENCE+offset;
	if(offset<TOWNSADDR_SYSROM_MAP_SIZE-3)
	{
		return cpputil::GetDword(ROMPtr);;
	}
	Abort("Cross-Border DWORD access to Mapped SYSROM");
	return 0xFFFFFFFF;
}

inline void TownsPhysicalMemory::MappedSYSROMStoreByte(unsigned int physAddr,unsigned char data)
{
}

DefaultStoreWord(MappedSYSROM)

DefaultStoreDword(MappedSYSROM)

inline MemoryAccess::ConstMemoryWindow TownsPhysicalMemory::MappedSYSROMGetConstMemoryWindow(unsigned int physAddr) const
{
	MemoryAccess::ConstMemoryWindow memWin;
	const unsigned int offset=(physAddr&(~0xfff))-TOWNSADDR_SYSROM_MAP_BASE;
	memWin.ptr=sysRom.data()+TOWNSADDR_SYSROM_MAP_OFFSET_DIFFERENCE+offset;
	return memWin;
}

DefaultGetMemoryWindow(MappedSYSROM)

////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::SpriteRAMFetchByte(unsigned int physAddr) const
{
	return state.spriteRAM[physAddr&TOWNSADDR_SPRITERAM_AND];
}

inline unsigned int TownsPhysicalMemory::SpriteRAMFetchWord(unsigned int physAddr) const
{
	return cpputil::GetWord(state.spriteRAM+(physAddr&TOWNSADDR_SPRITERAM_AND));
}

inline unsigned int TownsPhysicalMemory::SpriteRAMFetchDword(unsigned int physAddr) const
{
	return cpputil::GetDword(state.spriteRAM+(physAddr&TOWNSADDR_SPRITERAM_AND));
}

inline void TownsPhysicalMemory::SpriteRAMStoreByte(unsigned int physAddr,unsigned char data)
{
	state.spriteRAM[physAddr&TOWNSADDR_SPRITERAM_AND]=data;
}

inline void TownsPhysicalMemory::SpriteRAMStoreWord(unsigned int physAddr,unsigned int data)
{
	cpputil::PutWord(state.spriteRAM+(physAddr&TOWNSADDR_SPRITERAM_AND),data);
}

inline void TownsPhysicalMemory::SpriteRAMStoreDword(unsigned int physAddr,unsigned int data)
{
	cpputil::PutDword(state.spriteRAM+(physAddr&TOWNSADDR_SPRITERAM_AND),data);
}

inline MemoryAccess::ConstMemoryWindow TownsPhysicalMemory::SpriteRAMGetConstMemoryWindow(unsigned int physAddr) const
{
	return EmptyConstMemoryWindow();
}

inline MemoryAccess::MemoryWindow TownsPhysicalMemory::SpriteRAMGetMemoryWindow(unsigned int physAddr)
{
	return EmptyMemoryWindow();
}

////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::OSROMFetchByte(unsigned int physAddr) const
{
	return dosRom[physAddr&TOWNSADDR_OSROM_AND];
}

inline unsigned int TownsPhysicalMemory::OSROMFetchWord(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_OSROM_AND;
	if(physAddr+1<dosRom.size())
	{
		auto *ROMPtr=dosRom.data()+physAddr;
		return ROMPtr[0]|(ROMPtr[1]<<8);
	}
	else
	{
		Abort("Cross-Border Access to OS ROM");
	}
	return 0xffff;
}

inline unsigned int TownsPhysicalMemory::OSROMFetchDword(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_OSROM_AND;
	if(physAddr+3<dosRom.size())
	{
		auto *ROMPtr=dosRom.data()+physAddr;
		return ROMPtr[0]|(ROMPtr[1]<<8)|(ROMPtr[2]<<16)|(ROMPtr[3]<<24);
	}
	else
	{
		Abort("Cross-Border Access to OS ROM");
	}
	return 0xffffffff;
}

inline void TownsPhysicalMemory::OSROMStoreByte(unsigned int physAddr,unsigned char data)
{
}

DefaultStoreWord(OSROM)

DefaultStoreDword(OSROM)

DefaultGetConstMemoryWindow(OSROM)

DefaultGetMemoryWindow(OSROM)

////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::FontROMFetchByte(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_FONT_AND;
	return fontRom[physAddr];
}

inline unsigned int TownsPhysicalMemory::FontROMFetchWord(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_FONT_AND;
	return cpputil::GetWord(fontRom.data()+physAddr);
}

inline unsigned int TownsPhysicalMemory::FontROMFetchDword(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_FONT_AND;
	return cpputil::GetDword(fontRom.data()+physAddr);
}

inline void TownsPhysicalMemory::FontROMStoreByte(unsigned int physAddr,unsigned char data)
{
}

DefaultStoreWord(FontROM)

DefaultStoreDword(FontROM)

DefaultGetMemoryWindow(FontROM)

DefaultGetConstMemoryWindow(FontROM)

////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::Font20ROMFetchByte(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_FONT20_AND;
	return font20Rom[physAddr];
}

inline unsigned int TownsPhysicalMemory::Font20ROMFetchWord(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_FONT20_AND;
	return cpputil::GetWord(font20Rom.data()+physAddr);
}

inline unsigned int TownsPhysicalMemory::Font20ROMFetchDword(unsigned int physAddr) const
{
	physAddr&=TOWNSADDR_FONT20_AND;
	return cpputil::GetDword(font20Rom.data()+physAddr);
}

inline void TownsPhysicalMemory::Font20ROMStoreByte(unsigned int physAddr,unsigned char data)
{
}

DefaultStoreWord(Font20ROM)

DefaultStoreDword(Font20ROM)

DefaultGetMemoryWindow(Font20ROM)

DefaultGetConstMemoryWindow(Font20ROM)

////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::NativeSYSROMFetchByte(unsigned int physAddr) const
{
	return sysRom[physAddr&TOWNSADDR_SYSROM_AND];
}

inline unsigned int TownsPhysicalMemory::NativeSYSROMFetchWord(unsigned int physAddr) const
{
	return cpputil::GetWord(sysRom.data()+(physAddr&TOWNSADDR_SYSROM_AND));
}

inline unsigned int TownsPhysicalMemory::NativeSYSROMFetchDword(unsigned int physAddr) const
{
	return cpputil::GetDword(sysRom.data()+(physAddr&TOWNSADDR_SYSROM_AND));
}

inline void TownsPhysicalMemory::NativeSYSROMStoreByte(unsigned int,unsigned char)
{
}

DefaultStoreWord(NativeSYSROM)

DefaultStoreDword(NativeSYSROM)

inline MemoryAccess::ConstMemoryWindow TownsPhysicalMemory::NativeSYSROMGetConstMemoryWindow(unsigned int physAddr) const
{
	MemoryAccess::ConstMemoryWindow memWin;
	memWin.ptr=sysRom.data()+((physAddr&(~0xfff)&TOWNSADDR_SYSROM_AND));
	return memWin;
}

DefaultGetMemoryWindow(NativeSYSROM)

////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::MartyEXROMFetchByte(unsigned int physAddr) const
{
	townsPtr->NotifyDiskRead();
	return martyRom[physAddr-TOWNSADDR_MARTY_ROM0_BASE];
}

inline unsigned int TownsPhysicalMemory::MartyEXROMFetchWord(unsigned int physAddr) const
{
	townsPtr->NotifyDiskRead();
	return cpputil::GetWord(martyRom.data()+(physAddr-TOWNSADDR_MARTY_ROM0_BASE));
}

inline unsigned int TownsPhysicalMemory::MartyEXROMFetchDword(unsigned int physAddr) const
{
	townsPtr->NotifyDiskRead();
	return cpputil::GetDword(martyRom.data()+(physAddr-TOWNSADDR_MARTY_ROM0_BASE));
}

inline void TownsPhysicalMemory::MartyEXROMStoreByte(unsigned int physAddr,unsigned char data)
{
}

DefaultStoreWord(MartyEXROM)

DefaultStoreDword(MartyEXROM)

inline MemoryAccess::ConstMemoryWindow TownsPhysicalMemory::MartyEXROMGetConstMemoryWindow(unsigned int physAddr) const
{
	MemoryAccess::ConstMemoryWindow memWin;
	memWin.ptr=martyRom.data()+(physAddr-TOWNSADDR_MARTY_ROM0_BASE);
	return memWin;
}

DefaultGetMemoryWindow(MartyEXROM)

////////////////////////////////////////////////////////////

void TownsPhysicalMemory::CleanUp(void)
{
	Memory::CleanUp();

	for(auto &t : memoryAccessType)
	{
		t=TOWNSMEM_LEGACY;
	}
}

void TownsPhysicalMemory::SetMemoryAccessTypeRange(uint32_t addrBegin,uint32_t addrEnd,uint8_t type)
{
	addrBegin>>=GRANURALITY_SHIFT;
	--addrEnd;
	addrEnd>>=GRANURALITY_SHIFT;
	for(unsigned addr=addrBegin; addr<=addrEnd; ++addr)
	{
		memoryAccessType[addr]&=ACCESSTYPE_DEBUG_FLAG;
		memoryAccessType[addr]|=type;
	}
}

void TownsPhysicalMemory::SetUpMemoryAccessType(int townsType,int cpuType)
{
	if(TOWNSCPU_80386SX==cpuType && 0xA00000<state.RAM.size())
	{
		state.RAM.resize(0xA00000);
	}

	SetMemoryAccessTypeRange(0,TOWNSADDR_FMR_VRAM_BASE,TOWNSMEM_MAINRAM);
	// FMR_VRAM/CVRAM
	// FMR_DICROM
	// MAPPED_SYSROM
	SetMemoryAccessTypeRange(TOWNSADDR_SYSROM_MAP_END,state.RAM.size(),TOWNSMEM_MAINRAM);

	// Unless CMOS is cleared, Towns OS will try to put R drive from 81000000H regardless of the CPU type.
	// To let 386SX run with the same CMOS setting, leave mapping of 81000000H.
	SetMemoryAccessTypeRange(TOWNSADDR_SPRITERAM_BASE,TOWNSADDR_SPRITERAM_END,TOWNSMEM_SPRITE_RAM);

	// Even when machine ID is 386SX, the CPU core is actually 486.  The reset instruction pointer needs to be at the end of 32-bit addre
	SetMemoryAccessTypeRange(TOWNSADDR_SYSROM_BASE,0xFFFFFFFF,TOWNSMEM_NATIVE_SYSROM);

	if(TOWNSCPU_80386SX!=cpuType)
	{
		SetMemoryAccessTypeRange(TOWNSADDR_OSROM_BASE,TOWNSADDR_OSROM_END,TOWNSMEM_OSROM);
		SetMemoryAccessTypeRange(TOWNSADDR_FONT_BASE,TOWNSADDR_FONT_END,TOWNSMEM_FONTROM);
		SetMemoryAccessTypeRange(TOWNSADDR_FONT20_BASE,TOWNSADDR_FONT20_END,TOWNSMEM_FONT20ROM);
		SetMemoryAccessTypeRange(TOWNSADDR_NATIVE_DICROM_BASE,TOWNSADDR_NATIVE_DICROM_END-1,TOWNSMEM_NATIVE_DICROM);
		SetMemoryAccessTypeRange(TOWNSADDR_WAVERAM_WINDOW_BASE,TOWNSADDR_WAVERAM_WINDOW_END-1,TOWNSMEM_WAVERAM);
		SetMemoryAccessTypeRange(TOWNSADDR_NATIVE_CMOSRAM_BASE,TOWNSADDR_NATIVE_CMOSRAM_END-1,TOWNSMEM_NATIVE_CMOS);
	}
	else
	{
		// Memory access for higher than 16MB address space is still neeeded in 80386SX mode since
		// high address may be given to DMA, and DMA will translate address to low address.
		// Instead of implementing the translation for DMA, an easier solution is to leave
		// high address memory access.
		// Also CPU core does not care 80386SX mode.  Therefore, the reset vector still needs to be
		// 0xFFFFFFF0, which means SYSROM still needs to be accessible from high address.
		SetMemoryAccessTypeRange(TOWNSADDR_386SX_SPRITERAM_BASE,TOWNSADDR_386SX_SPRITERAM_END,TOWNSMEM_SPRITE_RAM);

		if(TOWNSTYPE_MARTY==townsType)
		{
			SetMemoryAccessTypeRange(TOWNSADDR_MARTY_OSROM_BASE,TOWNSADDR_MARTY_OSROM_END,TOWNSMEM_OSROM);
			SetMemoryAccessTypeRange(TOWNSADDR_MARTY_ROM0_BASE,TOWNSADDR_MARTY_ROM3_END,TOWNSMEM_MARTY_EXROM);
		}
		else
		{
			SetMemoryAccessTypeRange(TOWNSADDR_386SX_OSROM_BASE,TOWNSADDR_386SX_OSROM_END,TOWNSMEM_OSROM);
		}

		SetMemoryAccessTypeRange(TOWNSADDR_386SX_FONT_BASE,TOWNSADDR_386SX_FONT_END,TOWNSMEM_FONTROM);
		SetMemoryAccessTypeRange(TOWNSADDR_386SX_SYSROM_BASE,TOWNSADDR_386SX_SYSROM_END,TOWNSMEM_NATIVE_SYSROM);
		SetMemoryAccessTypeRange(TOWNSADDR_386SX_NATIVE_CMOSRAM_BASE,TOWNSADDR_386SX_NATIVE_CMOSRAM_END-1,TOWNSMEM_NATIVE_CMOS);
		SetMemoryAccessTypeRange(TOWNSADDR_386SX_NATIVE_DICROM_BASE,TOWNSADDR_386SX_NATIVE_DICROM_END-1,TOWNSMEM_NATIVE_DICROM);
		SetMemoryAccessTypeRange(TOWNSADDR_386SX_WAVERAM_WINDOW_BASE,TOWNSADDR_386SX_WAVERAM_WINDOW_END-1,TOWNSMEM_WAVERAM);
	}
}





////////////////////////////////////////////////////////////

inline unsigned int TownsPhysicalMemory::TrueFetchByte(unsigned int physAddr) const
{
	unsigned char accessType=memoryAccessType[physAddr>>GRANURALITY_SHIFT];
REDO_WITH_DEBUG_FLAG_CLEAR:
	switch(accessType)
	{
	case TOWNSMEM_NONE:
		return 0xFF;
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchByte(physAddr);
	case TOWNSMEM_FMRVRAM:
		return FMRVRAMAccess.FetchByte(this,physAddr);
	case TOWNSMEM_MAPPED_DIC:
		return MappedDICFetchByte(physAddr);
	case TOWNSMEM_MAPPED_SYSROM:
		return MappedSYSROMFetchByte(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMFetchByte(physAddr);
	case TOWNSMEM_OSROM:
		return OSROMFetchByte(physAddr);
	case TOWNSMEM_NATIVE_SYSROM:
		return NativeSYSROMFetchByte(physAddr);
	case TOWNSMEM_FONTROM:
		return FontROMFetchByte(physAddr);
	case TOWNSMEM_FONT20ROM:
		return Font20ROMFetchByte(physAddr);
	case TOWNSMEM_NATIVE_CMOS:
		return NativeCMOSFetchByte(physAddr);
	case TOWNSMEM_NATIVE_DICROM:
		return NativeDICROMFetchByte(physAddr);
	case TOWNSMEM_MARTY_EXROM:
		return MartyEXROMFetchByte(physAddr);
	case TOWNSMEM_WAVERAM:
		return waveRAMAccess.FetchByte(physAddr);
	default:
		{
			auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
			return memAccess->FetchByte(physAddr);
		}
		break;

	case TOWNSMEM_NONE_DEBUG:
	case TOWNSMEM_LEGACY_DEBUG:
	case TOWNSMEM_MAINRAM_DEBUG:
	case TOWNSMEM_FMRVRAM_DEBUG:
	case TOWNSMEM_MAPPED_DIC_DEBUG:
	case TOWNSMEM_MAPPED_SYSROM_DEBUG:
	case TOWNSMEM_SPRITE_RAM_DEBUG:
	case TOWNSMEM_MARTY_EXROM_DEBUG:
	case TOWNSMEM_NATIVE_DICROM_DEBUG:
	case TOWNSMEM_NATIVE_CMOS_DEBUG:
	case TOWNSMEM_OLD_MEMCARD_DEBUG:
	case TOWNSMEM_JEIDA4_MEMCARD_DEBUG:
	case TOWNSMEM_OSROM_DEBUG:
	case TOWNSMEM_FONTROM_DEBUG:
	case TOWNSMEM_FONT20ROM_DEBUG:
	case TOWNSMEM_WAVERAM_DEBUG:
	case TOWNSMEM_NATIVE_SYSROM_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_WITH_MASK_DEBUG:
		debuggerLink[physAddr>>GRANURALITY_SHIFT]->FetchByte(physAddr);
		accessType&=~1;
		goto REDO_WITH_DEBUG_FLAG_CLEAR;
	}
}

inline unsigned int TownsPhysicalMemory::TrueFetchByteDMA(unsigned int physAddr) const
{
	unsigned char accessType=memoryAccessType[physAddr>>GRANURALITY_SHIFT];
REDO_WITH_DEBUG_FLAG_CLEAR:
	switch(accessType)
	{
	case TOWNSMEM_NONE:
		return 0xFF;
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchByte(physAddr);
	case TOWNSMEM_FMRVRAM:
		return FMRVRAMAccess.FetchByte(this,physAddr);
	case TOWNSMEM_MAPPED_DIC:
		return MappedDICFetchByte(physAddr);
	case TOWNSMEM_MAPPED_SYSROM:
		return MappedSYSROMFetchByte(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMFetchByte(physAddr);
	case TOWNSMEM_OSROM:
		return OSROMFetchByte(physAddr);
	case TOWNSMEM_NATIVE_SYSROM:
		return NativeSYSROMFetchByte(physAddr);
	case TOWNSMEM_FONTROM:
		return FontROMFetchByte(physAddr);
	case TOWNSMEM_FONT20ROM:
		return Font20ROMFetchByte(physAddr);
	case TOWNSMEM_NATIVE_CMOS:
		return NativeCMOSFetchByte(physAddr);
	case TOWNSMEM_NATIVE_DICROM:
		return NativeDICROMFetchByte(physAddr);
	case TOWNSMEM_MARTY_EXROM:
		return MartyEXROMFetchByte(physAddr);
	case TOWNSMEM_WAVERAM:
		return waveRAMAccess.FetchByte(physAddr);
	default:
		{
			auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
			return memAccess->FetchByteDMA(physAddr);
		}
		break;

	case TOWNSMEM_NONE_DEBUG:
	case TOWNSMEM_LEGACY_DEBUG:
	case TOWNSMEM_MAINRAM_DEBUG:
	case TOWNSMEM_FMRVRAM_DEBUG:
	case TOWNSMEM_MAPPED_DIC_DEBUG:
	case TOWNSMEM_MAPPED_SYSROM_DEBUG:
	case TOWNSMEM_SPRITE_RAM_DEBUG:
	case TOWNSMEM_MARTY_EXROM_DEBUG:
	case TOWNSMEM_NATIVE_DICROM_DEBUG:
	case TOWNSMEM_NATIVE_CMOS_DEBUG:
	case TOWNSMEM_OLD_MEMCARD_DEBUG:
	case TOWNSMEM_JEIDA4_MEMCARD_DEBUG:
	case TOWNSMEM_OSROM_DEBUG:
	case TOWNSMEM_FONTROM_DEBUG:
	case TOWNSMEM_FONT20ROM_DEBUG:
	case TOWNSMEM_WAVERAM_DEBUG:
	case TOWNSMEM_NATIVE_SYSROM_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_WITH_MASK_DEBUG:
		debuggerLink[physAddr>>GRANURALITY_SHIFT]->FetchByteDMA(physAddr);
		accessType&=~1;
		goto REDO_WITH_DEBUG_FLAG_CLEAR;
	}
}

inline unsigned int TownsPhysicalMemory::TrueFetchWord(unsigned int physAddr) const
{
	unsigned char accessType=memoryAccessType[physAddr>>GRANURALITY_SHIFT];
REDO_WITH_DEBUG_FLAG_CLEAR:
	switch(accessType)
	{
	case TOWNSMEM_NONE:
		return 0xFFFF;
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchWord(physAddr);
	case TOWNSMEM_FMRVRAM:
		return FMRVRAMAccess.FetchWord(this,physAddr);
	case TOWNSMEM_MAPPED_DIC:
		return MappedDICFetchWord(physAddr);
	case TOWNSMEM_MAPPED_SYSROM:
		return MappedSYSROMFetchWord(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMFetchWord(physAddr);
	case TOWNSMEM_OSROM:
		return OSROMFetchWord(physAddr);
	case TOWNSMEM_NATIVE_SYSROM:
		return NativeSYSROMFetchWord(physAddr);
	case TOWNSMEM_FONTROM:
		return FontROMFetchWord(physAddr);
	case TOWNSMEM_FONT20ROM:
		return Font20ROMFetchWord(physAddr);
	case TOWNSMEM_NATIVE_CMOS:
		return NativeCMOSFetchWord(physAddr);
	case TOWNSMEM_NATIVE_DICROM:
		return NativeDICROMFetchWord(physAddr);
	case TOWNSMEM_MARTY_EXROM:
		return MartyEXROMFetchWord(physAddr);
	case TOWNSMEM_WAVERAM:
		return waveRAMAccess.FetchWord(physAddr);
	default:
		{
			auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
			return memAccess->FetchWord(physAddr);
		}
		break;

	case TOWNSMEM_NONE_DEBUG:
	case TOWNSMEM_LEGACY_DEBUG:
	case TOWNSMEM_MAINRAM_DEBUG:
	case TOWNSMEM_FMRVRAM_DEBUG:
	case TOWNSMEM_MAPPED_DIC_DEBUG:
	case TOWNSMEM_MAPPED_SYSROM_DEBUG:
	case TOWNSMEM_SPRITE_RAM_DEBUG:
	case TOWNSMEM_MARTY_EXROM_DEBUG:
	case TOWNSMEM_NATIVE_DICROM_DEBUG:
	case TOWNSMEM_NATIVE_CMOS_DEBUG:
	case TOWNSMEM_OLD_MEMCARD_DEBUG:
	case TOWNSMEM_JEIDA4_MEMCARD_DEBUG:
	case TOWNSMEM_OSROM_DEBUG:
	case TOWNSMEM_FONTROM_DEBUG:
	case TOWNSMEM_FONT20ROM_DEBUG:
	case TOWNSMEM_WAVERAM_DEBUG:
	case TOWNSMEM_NATIVE_SYSROM_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_WITH_MASK_DEBUG:
		debuggerLink[physAddr>>GRANURALITY_SHIFT]->FetchWord(physAddr);
		accessType&=~1;
		goto REDO_WITH_DEBUG_FLAG_CLEAR;
	}
}

inline unsigned int TownsPhysicalMemory::TrueFetchDword(unsigned int physAddr) const
{
	unsigned char accessType=memoryAccessType[physAddr>>GRANURALITY_SHIFT];
REDO_WITH_DEBUG_FLAG_CLEAR:
	switch(accessType)
	{
	case TOWNSMEM_NONE:
		return 0xFFFFFFFF;
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchDword(physAddr);
	case TOWNSMEM_FMRVRAM:
		return FMRVRAMAccess.FetchDword(this,physAddr);
	case TOWNSMEM_MAPPED_DIC:
		return MappedDICFetchDword(physAddr);
	case TOWNSMEM_MAPPED_SYSROM:
		return MappedSYSROMFetchDword(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMFetchDword(physAddr);
	case TOWNSMEM_OSROM:
		return OSROMFetchDword(physAddr);
	case TOWNSMEM_NATIVE_SYSROM:
		return NativeSYSROMFetchDword(physAddr);
	case TOWNSMEM_FONTROM:
		return FontROMFetchDword(physAddr);
	case TOWNSMEM_FONT20ROM:
		return Font20ROMFetchDword(physAddr);
	case TOWNSMEM_NATIVE_CMOS:
		return NativeCMOSFetchDword(physAddr);
	case TOWNSMEM_NATIVE_DICROM:
		return NativeDICROMFetchDword(physAddr);
	case TOWNSMEM_MARTY_EXROM:
		return MartyEXROMFetchDword(physAddr);
	case TOWNSMEM_WAVERAM:
		return waveRAMAccess.FetchDword(physAddr);
	default:
		{
			auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
			return memAccess->FetchDword(physAddr);
		}
		break;

	case TOWNSMEM_NONE_DEBUG:
	case TOWNSMEM_LEGACY_DEBUG:
	case TOWNSMEM_MAINRAM_DEBUG:
	case TOWNSMEM_FMRVRAM_DEBUG:
	case TOWNSMEM_MAPPED_DIC_DEBUG:
	case TOWNSMEM_MAPPED_SYSROM_DEBUG:
	case TOWNSMEM_SPRITE_RAM_DEBUG:
	case TOWNSMEM_MARTY_EXROM_DEBUG:
	case TOWNSMEM_NATIVE_DICROM_DEBUG:
	case TOWNSMEM_NATIVE_CMOS_DEBUG:
	case TOWNSMEM_OLD_MEMCARD_DEBUG:
	case TOWNSMEM_JEIDA4_MEMCARD_DEBUG:
	case TOWNSMEM_OSROM_DEBUG:
	case TOWNSMEM_FONTROM_DEBUG:
	case TOWNSMEM_FONT20ROM_DEBUG:
	case TOWNSMEM_WAVERAM_DEBUG:
	case TOWNSMEM_NATIVE_SYSROM_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_WITH_MASK_DEBUG:
		debuggerLink[physAddr>>GRANURALITY_SHIFT]->FetchDword(physAddr);
		accessType&=~1;
		goto REDO_WITH_DEBUG_FLAG_CLEAR;
	}
}


inline void TownsPhysicalMemory::TrueStoreByte(unsigned int physAddr,unsigned char data)
{
	unsigned char accessType=memoryAccessType[physAddr>>GRANURALITY_SHIFT];
REDO_WITH_DEBUG_FLAG_CLEAR:
	switch(accessType)
	{
	case TOWNSMEM_NONE:
		break;
	case TOWNSMEM_MAINRAM:
		MainRAMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_FMRVRAM:
		FMRVRAMAccess.StoreByte(this,physAddr,data);
		break;
	case TOWNSMEM_MAPPED_DIC:
		MappedDICStoreByte(physAddr,data);
		break;
	case TOWNSMEM_MAPPED_SYSROM:
		MappedSYSROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_SPRITE_RAM:
		SpriteRAMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_OSROM:
		OSROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_SYSROM:
		NativeSYSROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_FONTROM:
		FontROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_FONT20ROM:
		Font20ROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_CMOS:
		NativeCMOSStoreByte(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_DICROM:
		NativeDICROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_MARTY_EXROM:
		MartyEXROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_WAVERAM:
		waveRAMAccess.StoreByte(physAddr,data);
		break;
	default:
		{
			auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
			memAccess->StoreByte(physAddr,data);
		}
		break;

	case TOWNSMEM_NONE_DEBUG:
	case TOWNSMEM_LEGACY_DEBUG:
	case TOWNSMEM_MAINRAM_DEBUG:
	case TOWNSMEM_FMRVRAM_DEBUG:
	case TOWNSMEM_MAPPED_DIC_DEBUG:
	case TOWNSMEM_MAPPED_SYSROM_DEBUG:
	case TOWNSMEM_SPRITE_RAM_DEBUG:
	case TOWNSMEM_MARTY_EXROM_DEBUG:
	case TOWNSMEM_NATIVE_DICROM_DEBUG:
	case TOWNSMEM_NATIVE_CMOS_DEBUG:
	case TOWNSMEM_OLD_MEMCARD_DEBUG:
	case TOWNSMEM_JEIDA4_MEMCARD_DEBUG:
	case TOWNSMEM_OSROM_DEBUG:
	case TOWNSMEM_FONTROM_DEBUG:
	case TOWNSMEM_FONT20ROM_DEBUG:
	case TOWNSMEM_WAVERAM_DEBUG:
	case TOWNSMEM_NATIVE_SYSROM_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_WITH_MASK_DEBUG:
		debuggerLink[physAddr>>GRANURALITY_SHIFT]->StoreByte(physAddr,data);
		accessType&=~1;
		goto REDO_WITH_DEBUG_FLAG_CLEAR;
	}
}

inline void TownsPhysicalMemory::TrueStoreByteDMA(unsigned int physAddr,unsigned char data)
{
	unsigned char accessType=memoryAccessType[physAddr>>GRANURALITY_SHIFT];
REDO_WITH_DEBUG_FLAG_CLEAR:
	switch(accessType)
	{
	case TOWNSMEM_NONE:
		break;
	case TOWNSMEM_MAINRAM:
		MainRAMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_FMRVRAM:
		FMRVRAMAccess.StoreByte(this,physAddr,data);
		break;
	case TOWNSMEM_MAPPED_DIC:
		MappedDICStoreByte(physAddr,data);
		break;
	case TOWNSMEM_MAPPED_SYSROM:
		MappedSYSROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_SPRITE_RAM:
		SpriteRAMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_OSROM:
		OSROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_SYSROM:
		NativeSYSROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_FONTROM:
		FontROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_FONT20ROM:
		Font20ROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_CMOS:
		NativeCMOSStoreByte(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_DICROM:
		NativeDICROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_MARTY_EXROM:
		MartyEXROMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_WAVERAM:
		waveRAMAccess.StoreByte(physAddr,data);
		break;
	default:
		{
			auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
			memAccess->StoreByteDMA(physAddr,data);
		}
		break;

	case TOWNSMEM_NONE_DEBUG:
	case TOWNSMEM_LEGACY_DEBUG:
	case TOWNSMEM_MAINRAM_DEBUG:
	case TOWNSMEM_FMRVRAM_DEBUG:
	case TOWNSMEM_MAPPED_DIC_DEBUG:
	case TOWNSMEM_MAPPED_SYSROM_DEBUG:
	case TOWNSMEM_SPRITE_RAM_DEBUG:
	case TOWNSMEM_MARTY_EXROM_DEBUG:
	case TOWNSMEM_NATIVE_DICROM_DEBUG:
	case TOWNSMEM_NATIVE_CMOS_DEBUG:
	case TOWNSMEM_OLD_MEMCARD_DEBUG:
	case TOWNSMEM_JEIDA4_MEMCARD_DEBUG:
	case TOWNSMEM_OSROM_DEBUG:
	case TOWNSMEM_FONTROM_DEBUG:
	case TOWNSMEM_FONT20ROM_DEBUG:
	case TOWNSMEM_WAVERAM_DEBUG:
	case TOWNSMEM_NATIVE_SYSROM_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_WITH_MASK_DEBUG:
		debuggerLink[physAddr>>GRANURALITY_SHIFT]->StoreByteDMA(physAddr,data);
		accessType&=~1;
		goto REDO_WITH_DEBUG_FLAG_CLEAR;
	}
}

inline void TownsPhysicalMemory::TrueStoreWord(unsigned int physAddr,unsigned int data)
{
	unsigned char accessType=memoryAccessType[physAddr>>GRANURALITY_SHIFT];
REDO_WITH_DEBUG_FLAG_CLEAR:
	switch(accessType)
	{
	case TOWNSMEM_NONE:
		break;
	case TOWNSMEM_MAINRAM:
		MainRAMStoreWord(physAddr,data);
		break;
	case TOWNSMEM_FMRVRAM:
		FMRVRAMAccess.StoreWord(this,physAddr,data);
		break;
	case TOWNSMEM_MAPPED_DIC:
		MappedDICStoreWord(physAddr,data);
		break;
	case TOWNSMEM_MAPPED_SYSROM:
		MappedSYSROMStoreWord(physAddr,data);
		break;
	case TOWNSMEM_SPRITE_RAM:
		SpriteRAMStoreWord(physAddr,data);
		break;
	case TOWNSMEM_OSROM:
		OSROMStoreWord(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_SYSROM:
		NativeSYSROMStoreWord(physAddr,data);
		break;
	case TOWNSMEM_FONTROM:
		FontROMStoreWord(physAddr,data);
		break;
	case TOWNSMEM_FONT20ROM:
		Font20ROMStoreWord(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_CMOS:
		NativeCMOSStoreWord(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_DICROM:
		NativeDICROMStoreWord(physAddr,data);
		break;
	case TOWNSMEM_MARTY_EXROM:
		MartyEXROMStoreWord(physAddr,data);
		break;
	case TOWNSMEM_WAVERAM:
		waveRAMAccess.StoreWord(physAddr,data);
		break;
	default:
		{
			auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
			memAccess->StoreWord(physAddr,data);
		}
		break;

	case TOWNSMEM_NONE_DEBUG:
	case TOWNSMEM_LEGACY_DEBUG:
	case TOWNSMEM_MAINRAM_DEBUG:
	case TOWNSMEM_FMRVRAM_DEBUG:
	case TOWNSMEM_MAPPED_DIC_DEBUG:
	case TOWNSMEM_MAPPED_SYSROM_DEBUG:
	case TOWNSMEM_SPRITE_RAM_DEBUG:
	case TOWNSMEM_MARTY_EXROM_DEBUG:
	case TOWNSMEM_NATIVE_DICROM_DEBUG:
	case TOWNSMEM_NATIVE_CMOS_DEBUG:
	case TOWNSMEM_OLD_MEMCARD_DEBUG:
	case TOWNSMEM_JEIDA4_MEMCARD_DEBUG:
	case TOWNSMEM_OSROM_DEBUG:
	case TOWNSMEM_FONTROM_DEBUG:
	case TOWNSMEM_FONT20ROM_DEBUG:
	case TOWNSMEM_WAVERAM_DEBUG:
	case TOWNSMEM_NATIVE_SYSROM_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_WITH_MASK_DEBUG:
		debuggerLink[physAddr>>GRANURALITY_SHIFT]->StoreWord(physAddr,data);
		accessType&=~1;
		goto REDO_WITH_DEBUG_FLAG_CLEAR;
	}
}

inline void TownsPhysicalMemory::TrueStoreDword(unsigned int physAddr,unsigned int data)
{
	unsigned char accessType=memoryAccessType[physAddr>>GRANURALITY_SHIFT];
REDO_WITH_DEBUG_FLAG_CLEAR:
	switch(accessType)
	{
	case TOWNSMEM_NONE:
		break;
	case TOWNSMEM_MAINRAM:
		MainRAMStoreDword(physAddr,data);
		break;
	case TOWNSMEM_FMRVRAM:
		FMRVRAMAccess.StoreDword(this,physAddr,data);
		break;
	case TOWNSMEM_MAPPED_DIC:
		MappedDICStoreDword(physAddr,data);
		break;
	case TOWNSMEM_MAPPED_SYSROM:
		MappedSYSROMStoreDword(physAddr,data);
		break;
	case TOWNSMEM_SPRITE_RAM:
		SpriteRAMStoreDword(physAddr,data);
		break;
	case TOWNSMEM_OSROM:
		OSROMStoreDword(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_SYSROM:
		NativeSYSROMStoreDword(physAddr,data);
		break;
	case TOWNSMEM_FONTROM:
		FontROMStoreDword(physAddr,data);
		break;
	case TOWNSMEM_FONT20ROM:
		Font20ROMStoreDword(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_CMOS:
		NativeCMOSStoreDword(physAddr,data);
		break;
	case TOWNSMEM_NATIVE_DICROM:
		NativeDICROMStoreDword(physAddr,data);
		break;
	case TOWNSMEM_MARTY_EXROM:
		MartyEXROMStoreDword(physAddr,data);
		break;
	case TOWNSMEM_WAVERAM:
		waveRAMAccess.StoreDword(physAddr,data);
		break;
	default:
		{
			auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
			memAccess->StoreDword(physAddr,data);
		}
		break;

	case TOWNSMEM_NONE_DEBUG:
	case TOWNSMEM_LEGACY_DEBUG:
	case TOWNSMEM_MAINRAM_DEBUG:
	case TOWNSMEM_FMRVRAM_DEBUG:
	case TOWNSMEM_MAPPED_DIC_DEBUG:
	case TOWNSMEM_MAPPED_SYSROM_DEBUG:
	case TOWNSMEM_SPRITE_RAM_DEBUG:
	case TOWNSMEM_MARTY_EXROM_DEBUG:
	case TOWNSMEM_NATIVE_DICROM_DEBUG:
	case TOWNSMEM_NATIVE_CMOS_DEBUG:
	case TOWNSMEM_OLD_MEMCARD_DEBUG:
	case TOWNSMEM_JEIDA4_MEMCARD_DEBUG:
	case TOWNSMEM_OSROM_DEBUG:
	case TOWNSMEM_FONTROM_DEBUG:
	case TOWNSMEM_FONT20ROM_DEBUG:
	case TOWNSMEM_WAVERAM_DEBUG:
	case TOWNSMEM_NATIVE_SYSROM_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_WITH_MASK_DEBUG:
		debuggerLink[physAddr>>GRANURALITY_SHIFT]->StoreDword(physAddr,data);
		accessType&=~1;
		goto REDO_WITH_DEBUG_FLAG_CLEAR;
	}
}

inline MemoryAccess::ConstMemoryWindow TownsPhysicalMemory::TrueGetConstMemoryWindow(unsigned int physAddr) const
{
	unsigned char accessType=memoryAccessType[physAddr>>GRANURALITY_SHIFT];
	switch(accessType)
	{
	case TOWNSMEM_NONE:
		return EmptyConstMemoryWindow();
	case TOWNSMEM_MAINRAM:
		return MainRAMGetConstMemoryWindow(physAddr);
	case TOWNSMEM_FMRVRAM:
		return EmptyConstMemoryWindow();
	case TOWNSMEM_MAPPED_DIC:
		return MappedDICGetConstMemoryWindow(physAddr);
	case TOWNSMEM_MAPPED_SYSROM:
		return MappedSYSROMGetConstMemoryWindow(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMGetConstMemoryWindow(physAddr);
	case TOWNSMEM_OSROM:
		return OSROMGetConstMemoryWindow(physAddr);
	case TOWNSMEM_NATIVE_SYSROM:
		return NativeSYSROMGetConstMemoryWindow(physAddr);
	case TOWNSMEM_FONTROM:
		return FontROMGetConstMemoryWindow(physAddr);
	case TOWNSMEM_FONT20ROM:
		return Font20ROMGetConstMemoryWindow(physAddr);
	case TOWNSMEM_NATIVE_CMOS:
		return NativeCMOSGetConstMemoryWindow(physAddr);
	case TOWNSMEM_NATIVE_DICROM:
		return NativeDICROMGetConstMemoryWindow(physAddr);
	case TOWNSMEM_MARTY_EXROM:
		return MartyEXROMGetConstMemoryWindow(physAddr);
	case TOWNSMEM_WAVERAM:
		return EmptyConstMemoryWindow();
	default:
		{
			auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
			return memAccess->GetConstMemoryWindow(physAddr);
		}
		break;

	case TOWNSMEM_NONE_DEBUG:
	case TOWNSMEM_LEGACY_DEBUG:
	case TOWNSMEM_MAINRAM_DEBUG:
	case TOWNSMEM_FMRVRAM_DEBUG:
	case TOWNSMEM_MAPPED_DIC_DEBUG:
	case TOWNSMEM_MAPPED_SYSROM_DEBUG:
	case TOWNSMEM_SPRITE_RAM_DEBUG:
	case TOWNSMEM_MARTY_EXROM_DEBUG:
	case TOWNSMEM_NATIVE_DICROM_DEBUG:
	case TOWNSMEM_NATIVE_CMOS_DEBUG:
	case TOWNSMEM_OLD_MEMCARD_DEBUG:
	case TOWNSMEM_JEIDA4_MEMCARD_DEBUG:
	case TOWNSMEM_OSROM_DEBUG:
	case TOWNSMEM_FONTROM_DEBUG:
	case TOWNSMEM_FONT20ROM_DEBUG:
	case TOWNSMEM_WAVERAM_DEBUG:
	case TOWNSMEM_NATIVE_SYSROM_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_WITH_MASK_DEBUG:
		return EmptyConstMemoryWindow();
	}
}

inline MemoryAccess::MemoryWindow TownsPhysicalMemory::TrueGetMemoryWindow(unsigned int physAddr)
{
	unsigned char accessType=memoryAccessType[physAddr>>GRANURALITY_SHIFT];
	switch(accessType)
	{
	case TOWNSMEM_NONE:
		return EmptyMemoryWindow();
	case TOWNSMEM_MAINRAM:
		return MainRAMGetMemoryWindow(physAddr);
	case TOWNSMEM_FMRVRAM:
		return EmptyMemoryWindow();
	case TOWNSMEM_MAPPED_DIC:
		return MappedDICGetMemoryWindow(physAddr);
	case TOWNSMEM_MAPPED_SYSROM:
		return MappedSYSROMGetMemoryWindow(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMGetMemoryWindow(physAddr);
	case TOWNSMEM_OSROM:
		return OSROMGetMemoryWindow(physAddr);
	case TOWNSMEM_NATIVE_SYSROM:
		return NativeSYSROMGetMemoryWindow(physAddr);
	case TOWNSMEM_FONTROM:
		return FontROMGetMemoryWindow(physAddr);
	case TOWNSMEM_FONT20ROM:
		return Font20ROMGetMemoryWindow(physAddr);
	case TOWNSMEM_NATIVE_CMOS:
		return NativeCMOSGetMemoryWindow(physAddr);
	case TOWNSMEM_NATIVE_DICROM:
		return NativeDICROMGetMemoryWindow(physAddr);
	case TOWNSMEM_MARTY_EXROM:
		return MartyEXROMGetMemoryWindow(physAddr);
	case TOWNSMEM_WAVERAM:
		return EmptyMemoryWindow();
	default:
		{
			auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
			return memAccess->GetMemoryWindow(physAddr);
		}
		break;

	case TOWNSMEM_NONE_DEBUG:
	case TOWNSMEM_LEGACY_DEBUG:
	case TOWNSMEM_MAINRAM_DEBUG:
	case TOWNSMEM_FMRVRAM_DEBUG:
	case TOWNSMEM_MAPPED_DIC_DEBUG:
	case TOWNSMEM_MAPPED_SYSROM_DEBUG:
	case TOWNSMEM_SPRITE_RAM_DEBUG:
	case TOWNSMEM_MARTY_EXROM_DEBUG:
	case TOWNSMEM_NATIVE_DICROM_DEBUG:
	case TOWNSMEM_NATIVE_CMOS_DEBUG:
	case TOWNSMEM_OLD_MEMCARD_DEBUG:
	case TOWNSMEM_JEIDA4_MEMCARD_DEBUG:
	case TOWNSMEM_OSROM_DEBUG:
	case TOWNSMEM_FONTROM_DEBUG:
	case TOWNSMEM_FONT20ROM_DEBUG:
	case TOWNSMEM_WAVERAM_DEBUG:
	case TOWNSMEM_NATIVE_SYSROM_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_DEBUG:
	case TOWNSMEM_VRAM_2PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_1PAGE_WITH_MASK_DEBUG:
	case TOWNSMEM_VRAM_HIGHRES_WITH_MASK_DEBUG:
		return EmptyMemoryWindow();
	}
}

////////////////////////////////////////////////////////////



Memory::Memory()
{
	memAccessPtr.resize(1<<(32-GRANURALITY_SHIFT));
	for(auto &ptr : memAccessPtr)
	{
		ptr=&nullAccess;
	}
}

void Memory::CleanUp(void)
{
	for(auto &ptr : memAccessPtr)
	{
		ptr=&nullAccess;
	}
}

void Memory::AddAccess(MemoryAccess *memAccess,unsigned int physAddrLow,unsigned int physAddrHigh)
{
	if(0!=(physAddrLow&((1<<GRANURALITY_SHIFT)-1)) || 0xfff!=(physAddrHigh&(1<<GRANURALITY_SHIFT)-1))
	{
		std::cout << __FUNCTION__ << std::endl;
		std::cout << "Error: Physical address must be integer multiple of 0x1000" << std::endl;
		std::cout << "       to integer multiple of 0x1000 minus 1." << std::endl;
		return;
	}
	auto low=physAddrLow>>GRANURALITY_SHIFT;
	auto high=physAddrHigh>>GRANURALITY_SHIFT;
	for(auto i=low; i<=high; ++i)
	{
		if(nullptr==memAccessPtr[i]->memAccessChain)
		{
			memAccessPtr[i]=memAccess;
		}
		else
		{
			auto ptr=memAccessPtr[i];
			while(nullptr!=ptr->memAccessChain->memAccessChain)
			{
				ptr=ptr->memAccessChain;
			}
			ptr->memAccessChain=memAccess;
		}
	}
}

Memory::DebuggerLink *Memory::GetDebuggerLink(uint32_t physAddr)
{
	if(0==debuggerLink.size())
	{
		return nullptr;
	}
	auto slot=physAddr>>GRANURALITY_SHIFT;
	return debuggerLink[slot];
}


void Memory::SetDebuggerLink(DebuggerLink *link,uint32_t addr)
{
	if(0==debuggerLink.size())
	{
		debuggerLink.resize(1<<(32-GRANURALITY_SHIFT));
		for(auto &l : debuggerLink)
		{
			l=nullptr;
		}
	}
	TownsPhysicalMemory *physMem=(TownsPhysicalMemory *)this;
	physMem->debuggerLink[addr>>GRANURALITY_SHIFT]=link;
	physMem->memoryAccessType[addr>>GRANURALITY_SHIFT]|=physMem->ACCESSTYPE_DEBUG_FLAG;
}

void Memory::ClearDebuggerLink(uint32_t addr)
{
}

unsigned int Memory::FetchByte(unsigned int physAddr) const
{
	const TownsPhysicalMemory *physMem=(const TownsPhysicalMemory *)this;
	return physMem->TrueFetchByte(physAddr);
}

unsigned int Memory::FetchByteDMA(unsigned int physAddr) const
{
	const TownsPhysicalMemory *physMem=(const TownsPhysicalMemory *)this;
	return physMem->TrueFetchByteDMA(physAddr);
}

unsigned int Memory::FetchWord(unsigned int physAddr) const
{
	const TownsPhysicalMemory *physMem=(const TownsPhysicalMemory *)this;
	return physMem->TrueFetchWord(physAddr);
}

unsigned int Memory::FetchDword(unsigned int physAddr) const
{
	const TownsPhysicalMemory *physMem=(const TownsPhysicalMemory *)this;
	return physMem->TrueFetchDword(physAddr);
}

void Memory::StoreByte(unsigned int physAddr,unsigned char data)
{
	TownsPhysicalMemory *physMem=(TownsPhysicalMemory *)this;
	physMem->TrueStoreByte(physAddr,data);
}

void Memory::StoreByteDMA(unsigned int physAddr,unsigned char data)
{
	TownsPhysicalMemory *physMem=(TownsPhysicalMemory *)this;
	physMem->TrueStoreByteDMA(physAddr,data);
}

void Memory::StoreWord(unsigned int physAddr,unsigned int data)
{
	TownsPhysicalMemory *physMem=(TownsPhysicalMemory *)this;
	physMem->TrueStoreWord(physAddr,data);
}

void Memory::StoreDword(unsigned int physAddr,unsigned int data)
{
	TownsPhysicalMemory *physMem=(TownsPhysicalMemory *)this;
	physMem->TrueStoreDword(physAddr,data);
}

MemoryAccess::ConstMemoryWindow Memory::GetConstMemoryWindow(unsigned int physAddr) const
{
	const TownsPhysicalMemory *physMem=(const TownsPhysicalMemory *)this;
	return physMem->TrueGetConstMemoryWindow(physAddr);
}
MemoryAccess::MemoryWindow Memory::GetMemoryWindow(unsigned int physAddr)
{
	TownsPhysicalMemory *physMem=(TownsPhysicalMemory *)this;
	return physMem->TrueGetMemoryWindow(physAddr);
}

