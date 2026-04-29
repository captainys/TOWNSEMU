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

void TownsPhysicalMemory::CleanUp(void)
{
	Memory::CleanUp();

	for(auto &t : memoryAccessType)
	{
		t=TOWNSMEM_LEGACY;
	}
}

void TownsPhysicalMemory::SetUpMemoryAccessType(int cpuType)
{
	const unsigned int pageLength=(1<<GRANURALITY_SHIFT);

	if(TOWNSCPU_80386SX==cpuType && 0xA00000<state.RAM.size())
	{
		state.RAM.resize(0xA00000);
	}

	for(unsigned addr=0; addr<TOWNSADDR_FMR_VRAM_BASE; addr+=pageLength)
	{
		memoryAccessType[addr>>GRANURALITY_SHIFT]=TOWNSMEM_MAINRAM;
	}
	// FMR_VRAM/CVRAM
	// FMR_DICROM
	// MAPPED_SYSROM
	for(unsigned addr=TOWNSADDR_SYSROM_MAP_END; addr<state.RAM.size(); addr+=pageLength)
	{
		memoryAccessType[addr>>GRANURALITY_SHIFT]=TOWNSMEM_MAINRAM;
	}

}

inline unsigned int TownsPhysicalMemory::TrueFetchByte(unsigned int physAddr) const
{
	switch(memoryAccessType[physAddr>>GRANURALITY_SHIFT])
	{
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchByte(physAddr);
	default:
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->FetchByte(physAddr);
	}
}

inline unsigned int TownsPhysicalMemory::TrueFetchByteDMA(unsigned int physAddr) const
{
	switch(memoryAccessType[physAddr>>GRANURALITY_SHIFT])
	{
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchByte(physAddr);
	default:
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->FetchByteDMA(physAddr);
	}
}

inline unsigned int TownsPhysicalMemory::TrueFetchWord(unsigned int physAddr) const
{
	switch(memoryAccessType[physAddr>>GRANURALITY_SHIFT])
	{
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchWord(physAddr);
	default:
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->FetchWord(physAddr);
	}
}

inline unsigned int TownsPhysicalMemory::TrueFetchDword(unsigned int physAddr) const
{
	switch(memoryAccessType[physAddr>>GRANURALITY_SHIFT])
	{
	case TOWNSMEM_MAINRAM:
		return MainRAMFetchDword(physAddr);
	default:
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->FetchDword(physAddr);
	}
}


inline void TownsPhysicalMemory::TrueStoreByte(unsigned int physAddr,unsigned char data)
{
	switch(memoryAccessType[physAddr>>GRANURALITY_SHIFT])
	{
	case TOWNSMEM_MAINRAM:
		MainRAMStoreByte(physAddr,data);
		break;
	default:
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		memAccess->StoreByte(physAddr,data);
		break;
	}
}

inline void TownsPhysicalMemory::TrueStoreByteDMA(unsigned int physAddr,unsigned char data)
{
	switch(memoryAccessType[physAddr>>GRANURALITY_SHIFT])
	{
	case TOWNSMEM_MAINRAM:
		MainRAMStoreByte(physAddr,data);
		break;
	default:
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		memAccess->StoreByteDMA(physAddr,data);
		break;
	}
}

inline void TownsPhysicalMemory::TrueStoreWord(unsigned int physAddr,unsigned int data)
{
	switch(memoryAccessType[physAddr>>GRANURALITY_SHIFT])
	{
	case TOWNSMEM_MAINRAM:
		MainRAMStoreWord(physAddr,data);
		break;
	default:
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		memAccess->StoreWord(physAddr,data);
		break;
	}
}

inline void TownsPhysicalMemory::TrueStoreDword(unsigned int physAddr,unsigned int data)
{
	switch(memoryAccessType[physAddr>>GRANURALITY_SHIFT])
	{
	case TOWNSMEM_MAINRAM:
		MainRAMStoreDword(physAddr,data);
		break;
	default:
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		memAccess->StoreDword(physAddr,data);
		break;
	}
}

inline MemoryAccess::ConstMemoryWindow TownsPhysicalMemory::TrueGetConstMemoryWindow(unsigned int physAddr) const
{
	switch(memoryAccessType[physAddr>>GRANURALITY_SHIFT])
	{
	case TOWNSMEM_MAINRAM:
		return MainRAMGetConstMemoryWindow(physAddr);
	default:
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->GetConstMemoryWindow(physAddr);
	}
}

inline MemoryAccess::MemoryWindow TownsPhysicalMemory::TrueGetMemoryWindow(unsigned int physAddr)
{
	switch(memoryAccessType[physAddr>>GRANURALITY_SHIFT])
	{
	case TOWNSMEM_MAINRAM:
		return MainRAMGetMemoryWindow(physAddr);
	default:
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->GetMemoryWindow(physAddr);
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

