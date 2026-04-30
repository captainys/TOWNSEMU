#include <iostream>
#include "memory.h"
#include "ramrom.h"
#include "physmem.h"



////////////////////////////////////////////////////////////


#define DefaultFetchByteDMA \
inline unsigned int FetchByteDMA(const TownsPhysicalMemory &physMem,unsigned int physAddr) const \
{ \
	return FetchByte(physMem,physAddr); \
}

#define DefaultFetchWord \
inline unsigned int FetchWord(const TownsPhysicalMemory &physMem,unsigned int physAddr) const \
{ \
	return FetchByte(physMem,physAddr)|(FetchByte(physMem,physAddr+1)<<8); \
}

#define DefaultFetchDword \
inline unsigned int FetchDword(const TownsPhysicalMemory &physMem,unsigned int physAddr) const \
{ \
	return FetchByte(physMem,physAddr)| \
	      (FetchByte(physMem,physAddr+1)<<8)| \
	      (FetchByte(physMem,physAddr+2)<<16)| \
	      (FetchByte(physMem,physAddr+3)<<24); \
}

#define DefaultStoreByteDMA \
inline void StoreByteDMA(TownsPhysicalMemory &physMem,unsigned int physAddr,unsigned char data) \
{ \
	StoreByte(physMem,physAddr,data); \
}

#define DefaultStoreWord \
inline void StoreWord(TownsPhysicalMemory &physMem,unsigned int physAddr,unsigned int data) \
{ \
	StoreByte(physMem,physAddr,data&255); \
	StoreByte(physMem,physAddr+1,(data>>8)&255); \
}

#define DefaultStoreDword \
inline void StoreDword(TownsPhysicalMemory &physMem,unsigned int physAddr,unsigned int data) \
{ \
	StoreByte(physMem,physAddr,data&255); \
	StoreByte(physMem,physAddr+1,(data>>8)&255); \
	StoreByte(physMem,physAddr+2,(data>>16)&255); \
	StoreByte(physMem,physAddr+3,(data>>24)&255); \
}

#define DefaultGetConstMemoryWindow \
MemoryAccess::ConstMemoryWindow GetConstMemoryWindow(unsigned int physAddr) const \
{ \
	MemoryAccess::ConstMemoryWindow window; \
	window.ptr=nullptr; \
	return window; \
}

#define DefaultGetMemoryWindow \
MemoryAccess::MemoryWindow GetMemoryWindow(unsigned int physAddr) \
{ \
	MemoryAccess::MemoryWindow window; \
	window.ptr=nullptr; \
	return window; \
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
	const unsigned int pageLength=(1<<GRANURALITY_SHIFT);
	for(unsigned addr=addrBegin; addr<addrEnd; addr+=pageLength)
	{
		memoryAccessType[addr>>GRANURALITY_SHIFT]&=ACCESSTYPE_DEBUG_FLAG;
		memoryAccessType[addr>>GRANURALITY_SHIFT]|=type;
	}
}

void TownsPhysicalMemory::SetUpMemoryAccessType(int cpuType)
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

	if(TOWNSCPU_80386SX!=cpuType)
	{
	}
	else
	{
		SetMemoryAccessTypeRange(TOWNSADDR_386SX_SPRITERAM_BASE,TOWNSADDR_386SX_SPRITERAM_END,TOWNSMEM_SPRITE_RAM);
	}
}

inline unsigned int TownsPhysicalMemory::TrueFetchByte(unsigned int physAddr) const
{
	unsigned char accessType=memoryAccessType[physAddr>>GRANURALITY_SHIFT];
REDO_WITH_DEBUG_FLAG_CLEAR:
	switch(accessType)
	{
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchByte(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMFetchByte(physAddr);
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
	case TOWNSMEM_MARTY_OSROM_DEBUG:
	case TOWNSMEM_MARTY_ROM0_DEBUG:
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
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchByte(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMFetchByte(physAddr);
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
	case TOWNSMEM_MARTY_OSROM_DEBUG:
	case TOWNSMEM_MARTY_ROM0_DEBUG:
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
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchWord(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMFetchWord(physAddr);
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
	case TOWNSMEM_MARTY_OSROM_DEBUG:
	case TOWNSMEM_MARTY_ROM0_DEBUG:
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
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchDword(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMFetchDword(physAddr);
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
	case TOWNSMEM_MARTY_OSROM_DEBUG:
	case TOWNSMEM_MARTY_ROM0_DEBUG:
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
	case TOWNSMEM_MAINRAM:
		MainRAMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_SPRITE_RAM:
		SpriteRAMStoreByte(physAddr,data);
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
	case TOWNSMEM_MARTY_OSROM_DEBUG:
	case TOWNSMEM_MARTY_ROM0_DEBUG:
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
	case TOWNSMEM_MAINRAM:
		MainRAMStoreByte(physAddr,data);
		break;
	case TOWNSMEM_SPRITE_RAM:
		SpriteRAMStoreByte(physAddr,data);
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
	case TOWNSMEM_MARTY_OSROM_DEBUG:
	case TOWNSMEM_MARTY_ROM0_DEBUG:
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
	case TOWNSMEM_MAINRAM:
		MainRAMStoreWord(physAddr,data);
		break;
	case TOWNSMEM_SPRITE_RAM:
		SpriteRAMStoreWord(physAddr,data);
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
	case TOWNSMEM_MARTY_OSROM_DEBUG:
	case TOWNSMEM_MARTY_ROM0_DEBUG:
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
	case TOWNSMEM_MAINRAM:
		MainRAMStoreDword(physAddr,data);
		break;
	case TOWNSMEM_SPRITE_RAM:
		SpriteRAMStoreDword(physAddr,data);
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
	case TOWNSMEM_MARTY_OSROM_DEBUG:
	case TOWNSMEM_MARTY_ROM0_DEBUG:
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
	case TOWNSMEM_MAINRAM:
		return MainRAMGetConstMemoryWindow(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMGetConstMemoryWindow(physAddr);
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
	case TOWNSMEM_MARTY_OSROM_DEBUG:
	case TOWNSMEM_MARTY_ROM0_DEBUG:
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
	case TOWNSMEM_MAINRAM:
		return MainRAMGetMemoryWindow(physAddr);
	case TOWNSMEM_SPRITE_RAM:
		return SpriteRAMGetMemoryWindow(physAddr);
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
	case TOWNSMEM_MARTY_OSROM_DEBUG:
	case TOWNSMEM_MARTY_ROM0_DEBUG:
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
void Memory::RemoveAccess(unsigned int physAddrLow,unsigned int physAddrHigh)
{
	auto low=physAddrLow>>GRANURALITY_SHIFT;
	auto high=physAddrHigh>>GRANURALITY_SHIFT;
	for(auto i=low; i<=high; ++i)
	{
		if(nullptr==memAccessPtr[i]->memAccessChain)
		{
			memAccessPtr[i]=&nullAccess;
		}
		else
		{
			auto ptr=memAccessPtr[i];
			while(nullptr!=ptr->memAccessChain->memAccessChain)
			{
				ptr=ptr->memAccessChain;
			}
			ptr->memAccessChain=&nullAccess;
		}
	}
}
void Memory::SetAccessObject(MemoryAccess *memAccess,unsigned int physAddr)
{
	auto slot=physAddr>>GRANURALITY_SHIFT;
	memAccessPtr[slot]=memAccess;
}
MemoryAccess *Memory::GetAccessObject(unsigned int physAddr)
{
	auto slot=physAddr>>GRANURALITY_SHIFT;
	return memAccessPtr[slot];
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

