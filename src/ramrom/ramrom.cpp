#include <iostream>
#include "ramrom.h"



/* virtual */ unsigned int MemoryAccess::FetchWord(unsigned int physAddr) const
{
	return FetchByte(physAddr)|(FetchByte(physAddr+1)<<8);
}
/* virtual */ unsigned int MemoryAccess::FetchDword(unsigned int physAddr) const
{
	return FetchByte(physAddr)|(FetchByte(physAddr+1)<<8)|(FetchByte(physAddr+2)<<16)|(FetchByte(physAddr+3)<<24);
}
/* virtual */ void MemoryAccess::StoreWord(unsigned int physAddr,unsigned int data)
{
	StoreByte(physAddr,data&255);
	StoreByte(physAddr+1,(data>>8)&255);
}
/* virtual */ void MemoryAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	StoreByte(physAddr,data&255);
	StoreByte(physAddr+1,(data>>8)&255);
	StoreByte(physAddr+2,(data>>16)&255);
	StoreByte(physAddr+3,(data>>24)&255);
}


////////////////////////////////////////////////////////////


Memory::Memory()
{
	memAccessPtr.resize(1<<(32-GRANURALITY_SHIFT));
	for(auto ptr : memAccessPtr)
	{
		ptr=nullptr;
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
		memAccessPtr[i]=memAccess;
	}
}
