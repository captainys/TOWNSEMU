/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
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
/* virtual */ MemoryAccess::ConstMemoryWindow MemoryAccess::GetConstMemoryWindow(unsigned int physAddr) const
{
	ConstMemoryWindow window;
	window.ptr=nullptr;
	return window;
}
/* virtual */ MemoryAccess::MemoryWindow MemoryAccess::GetMemoryWindow(unsigned int physAddr)
{
	MemoryWindow window;
	window.ptr=nullptr;
	return window;
}

/* virtual */ unsigned int MemoryAccess::FetchByteDMA(unsigned int physAddr) const
{
	return FetchByte(physAddr);
}
/* virtual */ void MemoryAccess::StoreByteDMA(unsigned int physAddr,unsigned char data)
{
	StoreByte(physAddr,data);
}

////////////////////////////////////////////////////////////


/* virtual */ unsigned int NullMemoryAccess::FetchByte(unsigned int) const
{
	return 0xff;
}
/* virtual */ unsigned int NullMemoryAccess::FetchWord(unsigned int) const
{
	return 0xffff;
}
/* virtual */ unsigned int NullMemoryAccess::FetchDword(unsigned int) const
{
	return 0xffffffff;
}
/* virtual */ void NullMemoryAccess::StoreByte(unsigned int,unsigned char)
{
}
/* virtual */ void NullMemoryAccess::StoreWord(unsigned int,unsigned int)
{
}
/* virtual */ void NullMemoryAccess::StoreDword(unsigned int,unsigned int)
{
}


////////////////////////////////////////////////////////////

/* MainRAMAccess class still may be used from Memory when the debugger links to the
   memory-access object for break-on-read/write.
   However, it may be skipped and short-cutted by FetchByte, FetchWord, and FetchDword.
*/
Memory::MainRAMAccess::MainRAMAccess(class Memory *memPtr)
{
	this->memPtr=memPtr;
}

unsigned int Memory::MainRAMAccess::FetchByte(unsigned int physAddr) const
{
	return memPtr->state.RAM[physAddr];
}
unsigned int Memory::MainRAMAccess::FetchWord(unsigned int physAddr) const
{
	auto &state=memPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	return cpputil::GetWord(RAMPtr);
}
unsigned int Memory::MainRAMAccess::FetchDword(unsigned int physAddr) const
{
	auto &state=memPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	return cpputil::GetDword(RAMPtr);
}
void Memory::MainRAMAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	memPtr->state.RAM[physAddr]=data;
}
void Memory::MainRAMAccess::StoreWord(unsigned int physAddr,unsigned int data)
{
	auto &state=memPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	cpputil::PutWord(RAMPtr,(unsigned short)data);
}
void Memory::MainRAMAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	auto &state=memPtr->state;
	auto *RAMPtr=state.RAM.data()+physAddr;
	cpputil::PutDword(RAMPtr,data);
}
MemoryAccess::ConstMemoryWindow Memory::MainRAMAccess::GetConstMemoryWindow(unsigned int physAddr) const
{
	MemoryAccess::ConstMemoryWindow memWin;
	memWin.ptr=memPtr->state.RAM.data()+(physAddr&(~0xfff));
	return memWin;
}
MemoryAccess::MemoryWindow Memory::MainRAMAccess::GetMemoryWindow(unsigned int physAddr)
{
	MemoryAccess::MemoryWindow memWin;
	memWin.ptr=memPtr->state.RAM.data()+(physAddr&(~0xfff));
	return memWin;
}


////////////////////////////////////////////////////////////


Memory::Memory() : mainRAMAccess(this)
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
		memAccessPtr[i]=&nullAccess;
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
