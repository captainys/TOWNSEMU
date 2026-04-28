#include <iostream>
#include "memory.h"
#include "ramrom.h"



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
	auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
	return memAccess->FetchByte(physAddr);
}

unsigned int Memory::FetchWord(unsigned int physAddr) const
{
	auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
	return memAccess->FetchWord(physAddr);
}

unsigned int Memory::FetchDword(unsigned int physAddr) const
{
	auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
	return memAccess->FetchDword(physAddr);
}

MemoryAccess::ConstMemoryWindow Memory::GetConstMemoryWindow(unsigned int physAddr) const
{
	auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
	return memAccess->GetConstMemoryWindow(physAddr);
}
MemoryAccess::MemoryWindow Memory::GetMemoryWindow(unsigned int physAddr)
{
	auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
	return memAccess->GetMemoryWindow(physAddr);
}

void Memory::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
	memAccess->StoreByte(physAddr,data);
}

unsigned int Memory::FetchByteDMA(unsigned int physAddr) const
{
	auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
	return memAccess->FetchByteDMA(physAddr);
}

void Memory::StoreByteDMA(unsigned int physAddr,unsigned char data)
{
	auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
	memAccess->StoreByteDMA(physAddr,data);
}

void Memory::StoreWord(unsigned int physAddr,unsigned int data)
{
	auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
	memAccess->StoreWord(physAddr,data);
}

void Memory::StoreDword(unsigned int physAddr,unsigned int data)
{
	auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
	memAccess->StoreDword(physAddr,data);
}
