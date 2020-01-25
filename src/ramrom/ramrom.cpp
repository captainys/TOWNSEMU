#include <iostream>
#include "ramrom.h"



Memory::Memory()
{
	memAccessPtr.resize(0x10000);
	for(auto ptr : memAccessPtr)
	{
		ptr=nullptr;
	}
}

void Memory::AddAccess(MemoryAccess *memAccess,unsigned int physAddrLow,unsigned int physAddrHigh)
{
	if(0!=(physAddrLow&0xffff) || 0xffff!=(physAddrHigh&0xffff))
	{
		std::cout << __FUNCTION__ << std::endl;
		std::cout << "Error: Physical address must be integer multiple of 0x10000" << std::endl;
		std::cout <<         to integer multiple of 0x10000 minus 1." << std::endl;
		return;
	}
	auto low=physAddrLow<<16;
	auto high=physAddrHigh<<16;
	for(int i=low; i<=high; ++i)
	{
		memAccessPtr[i]=memAccess;
	}
}
