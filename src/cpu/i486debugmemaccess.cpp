/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */

#include "i486debugmemaccess.h"
#include "cpputil.h"



i486DebugMemoryAccess::i486DebugMemoryAccess(i486Debugger &debugger,unsigned int physAddrTop)
{
	this->debuggerPtr=&debugger;
	this->physAddrTop=physAddrTop&(~(Memory::MEMORY_ACCESS_SLOT_SIZE-1));
	for(unsigned int i=0; i<Memory::MEMORY_ACCESS_SLOT_SIZE; ++i)
	{
		breakOnRead[i]=false;
		breakOnWrite[i]=false;
		breakOnWriteMin[i]=0;
		breakOnWriteMax[i]=0xFF;
	}
}
void i486DebugMemoryAccess::SetBreakOnRead(unsigned int physAddr)
{
	physAddr-=physAddrTop;
	if(0<=physAddr && physAddr<Memory::MEMORY_ACCESS_SLOT_SIZE)
	{
		breakOnRead[physAddr]=true;
	}
}
void i486DebugMemoryAccess::SetBreakOnWrite(unsigned int physAddr)
{
	physAddr-=physAddrTop;
	if(0<=physAddr && physAddr<Memory::MEMORY_ACCESS_SLOT_SIZE)
	{
		breakOnWrite[physAddr]=true;
		breakOnWriteMin[physAddr]=0;
		breakOnWriteMax[physAddr]=0xFF;
		auto found=breakOnWriteData.find(physAddr);
		if(breakOnWriteData.end()!=found)
		{
			breakOnWriteData.erase(found);
		}
	}
}
void i486DebugMemoryAccess::SetBreakOnWrite(unsigned int physAddr,unsigned char minValue,unsigned char maxValue)
{
	physAddr-=physAddrTop;
	if(0<=physAddr && physAddr<Memory::MEMORY_ACCESS_SLOT_SIZE)
	{
		breakOnWrite[physAddr]=true;
		breakOnWriteMin[physAddr]=minValue;
		breakOnWriteMax[physAddr]=maxValue;
		auto found=breakOnWriteData.find(physAddr);
		if(breakOnWriteData.end()!=found)
		{
			breakOnWriteData.erase(found);
		}
	}
}
void i486DebugMemoryAccess::SetBreakOnWrite(uint32_t physAddr,unsigned char data)
{
	physAddr-=physAddrTop;
	if(0<=physAddr && physAddr<Memory::MEMORY_ACCESS_SLOT_SIZE)
	{
		breakOnWrite[physAddr]=true;
		breakOnWriteData[physAddr]=data;
	}
}
void i486DebugMemoryAccess::ClearBreakOnRead(void)
{
	for(unsigned int i=0; i<Memory::MEMORY_ACCESS_SLOT_SIZE; ++i)
	{
		breakOnRead[i]=false;
	}
}
void i486DebugMemoryAccess::ClearBreakOnWrite(void)
{
	for(unsigned int i=0; i<Memory::MEMORY_ACCESS_SLOT_SIZE; ++i)
	{
		breakOnWrite[i]=false;
	}
	breakOnWriteData.clear();
}
void i486DebugMemoryAccess::ClearBreakOnReadWrite(void)
{
	for(unsigned int i=0; i<Memory::MEMORY_ACCESS_SLOT_SIZE; ++i)
	{
		breakOnRead[i]=false;
		breakOnWrite[i]=false;
	}
}
void i486DebugMemoryAccess::ClearBreakOnRead(unsigned int physAddr)
{
	physAddr-=physAddrTop;
	if(0<=physAddr && physAddr<Memory::MEMORY_ACCESS_SLOT_SIZE)
	{
		breakOnRead[physAddr]=false;
	}
}
void i486DebugMemoryAccess::ClearBreakOnWrite(unsigned int physAddr)
{
	physAddr-=physAddrTop;
	if(0<=physAddr && physAddr<Memory::MEMORY_ACCESS_SLOT_SIZE)
	{
		breakOnRead[physAddr]=false;
		auto found=breakOnWriteData.find(physAddr);
		if(breakOnWriteData.end()!=found)
		{
			breakOnWriteData.erase(found);
		}
	}
}

/* virtual */ unsigned int i486DebugMemoryAccess::FetchByte(unsigned int physAddr) const
{
	auto data=memAccessChain->FetchByte(physAddr);
	if(true==breakOnRead[physAddr-physAddrTop])
	{
		std::string msg;
		msg="Memory Read BYTE PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="] (";
		msg+=cpputil::Ubtox(data);
		msg+="H)";
		debuggerPtr->ExternalBreak(msg);
	}
	return data;
}
/* virtual */ unsigned int i486DebugMemoryAccess::FetchWord(unsigned int physAddr) const
{
	auto data=memAccessChain->FetchWord(physAddr);
	if(true==breakOnRead[physAddr-physAddrTop] ||
	   true==breakOnRead[physAddr+1-physAddrTop])
	{
		std::string msg;
		msg="Memory Read WORD PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="] (";
		msg+=cpputil::Ustox(data);
		msg+=")";
		debuggerPtr->ExternalBreak(msg);
	}
	return data;
}
/* virtual */ unsigned int i486DebugMemoryAccess::FetchDword(unsigned int physAddr) const
{
	auto data=memAccessChain->FetchDword(physAddr);
	if(true==breakOnRead[physAddr-physAddrTop] ||
	   true==breakOnRead[physAddr+1-physAddrTop] ||
	   true==breakOnRead[physAddr+2-physAddrTop] ||
	   true==breakOnRead[physAddr+3-physAddrTop])
	{
		std::string msg;
		msg="Memory Read DWORD PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="] (";
		msg+=cpputil::Uitox(data);
		msg+=")";
		debuggerPtr->ExternalBreak(msg);
	}
	return data;
}

/* virtual */ unsigned int i486DebugMemoryAccess::FetchByteDMA(unsigned int physAddr) const
{
	auto data=memAccessChain->FetchByte(physAddr);
	if(true==breakOnRead[physAddr-physAddrTop])
	{
		std::string msg;
		msg="Memory Read BYTE PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="] (";
		msg+=cpputil::Ubtox(data);
		msg+="H) **DMA**";
		debuggerPtr->ExternalBreak(msg);
	}
	return data;
}

inline bool i486DebugMemoryAccess::CheckBreakOnWriteCondition(uint32_t physAddr,unsigned int data) const
{
	auto offset=physAddr-physAddrTop;
	if(true==breakOnWrite[offset])
	{
		auto found=breakOnWriteData.find(offset);
		if(breakOnWriteData.end()!=found && found->second==data)
		{
			return true;
		}
		else if(breakOnWriteData.end()==found && breakOnWriteMin[offset]<=data && data<=breakOnWriteMax[offset])
		{
			return true;
		}
	}
	return false;
}

/* virtual */ void i486DebugMemoryAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	if(true==CheckBreakOnWriteCondition(physAddr,data))
	{
		std::string msg;
		msg="Memory Write BYTE PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="] (";
		msg+=cpputil::Ubtox(data);
		msg+=")";
		debuggerPtr->ExternalBreak(msg);
	}
	memAccessChain->StoreByte(physAddr,data);
}
/* virtual */ void i486DebugMemoryAccess::StoreWord(unsigned int physAddr,unsigned int data)
{
	if(true==CheckBreakOnWriteCondition(physAddr  ,data&0xFF) ||
	   true==CheckBreakOnWriteCondition(physAddr+1,(data>>8)&0xFF))
	{
		std::string msg;
		msg="Memory Write WORD PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="] (";
		msg+=cpputil::Ustox(data);
		msg+=")";
		debuggerPtr->ExternalBreak(msg);
	}
	memAccessChain->StoreWord(physAddr,data);
}
/* virtual */ void i486DebugMemoryAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	if(true==CheckBreakOnWriteCondition(physAddr  ,data&0xFF) ||
	   true==CheckBreakOnWriteCondition(physAddr+1,(data>>8)&0xFF) ||
	   true==CheckBreakOnWriteCondition(physAddr+2,(data>>16)&0xFF) ||
	   true==CheckBreakOnWriteCondition(physAddr+3,(data>>24)&0xFF))
	{
		std::string msg;
		msg="Memory Write DWORD PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="] (";
		msg+=cpputil::Uitox(data);
		msg+=")";
		debuggerPtr->ExternalBreak(msg);
	}
	memAccessChain->StoreDword(physAddr,data);
}
/* virtual */ void i486DebugMemoryAccess::StoreByteDMA(unsigned int physAddr,unsigned char data)
{
	if(true==CheckBreakOnWriteCondition(physAddr,data))
	{
		std::string msg;
		msg="Memory Write BYTE PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="] (";
		msg+=cpputil::Ubtox(data);
		msg+=") **DMA**";
		debuggerPtr->ExternalBreak(msg);
	}
	memAccessChain->StoreByte(physAddr,data);
}

/* static */ void i486DebugMemoryAccess::SetBreakOnMemRead(Memory &mem,i486Debugger &debugger,unsigned int physAddr)
{
	auto *curMemAccess=mem.GetAccessObject(physAddr);
	i486DebugMemoryAccess *debugMemAccess=dynamic_cast <i486DebugMemoryAccess *>(curMemAccess);
	if(nullptr==debugMemAccess)
	{
		debugMemAccess=new i486DebugMemoryAccess(debugger,physAddr);
		debugMemAccess->memAccessChain=curMemAccess;
	}
	debugMemAccess->SetBreakOnRead(physAddr);
	mem.SetAccessObject(debugMemAccess,physAddr);
}
/* static */ void i486DebugMemoryAccess::ClearBreakOnMemRead(Memory &mem,unsigned int physAddr)
{
	auto *curMemAccess=mem.GetAccessObject(physAddr);
	i486DebugMemoryAccess *debugMemAccess=dynamic_cast <i486DebugMemoryAccess *>(curMemAccess);
	if(nullptr!=debugMemAccess)
	{
		debugMemAccess->ClearBreakOnRead(physAddr);
	}
}
/* static */ void i486DebugMemoryAccess::ClearBreakOnMemRead(Memory &mem)
{
	for(unsigned long long physAddr=0; physAddr<0x100000000LL; physAddr+=Memory::MEMORY_ACCESS_SLOT_SIZE)
	{
		auto memAccessPtr=mem.GetAccessObject((unsigned int)physAddr);
		auto debugMemAccessPtr=dynamic_cast <i486DebugMemoryAccess *>(memAccessPtr);
		if(nullptr!=debugMemAccessPtr)
		{
			debugMemAccessPtr->ClearBreakOnRead();
		}
	}
}
/* static */ void i486DebugMemoryAccess::SetBreakOnMemWrite(Memory &mem,i486Debugger &debugger,unsigned int physAddr)
{
	auto *curMemAccess=mem.GetAccessObject(physAddr);
	i486DebugMemoryAccess *debugMemAccess=dynamic_cast <i486DebugMemoryAccess *>(curMemAccess);
	if(nullptr==debugMemAccess)
	{
		debugMemAccess=new i486DebugMemoryAccess(debugger,physAddr);
		debugMemAccess->memAccessChain=curMemAccess;
	}
	debugMemAccess->SetBreakOnWrite(physAddr);
	mem.SetAccessObject(debugMemAccess,physAddr);
}
/* static */ void i486DebugMemoryAccess::SetBreakOnMemWrite(Memory &mem,i486Debugger &debugger,unsigned int physAddr,unsigned char data)
{
	auto *curMemAccess=mem.GetAccessObject(physAddr);
	i486DebugMemoryAccess *debugMemAccess=dynamic_cast <i486DebugMemoryAccess *>(curMemAccess);
	if(nullptr==debugMemAccess)
	{
		debugMemAccess=new i486DebugMemoryAccess(debugger,physAddr);
		debugMemAccess->memAccessChain=curMemAccess;
	}
	debugMemAccess->SetBreakOnWrite(physAddr,data);
	mem.SetAccessObject(debugMemAccess,physAddr);
}
/* static */ void i486DebugMemoryAccess::SetBreakOnMemWrite(Memory &mem,i486Debugger &debugger,unsigned int physAddr,unsigned char minValue,unsigned char maxValue)
{
	auto *curMemAccess=mem.GetAccessObject(physAddr);
	i486DebugMemoryAccess *debugMemAccess=dynamic_cast <i486DebugMemoryAccess *>(curMemAccess);
	if(nullptr==debugMemAccess)
	{
		debugMemAccess=new i486DebugMemoryAccess(debugger,physAddr);
		debugMemAccess->memAccessChain=curMemAccess;
	}
	debugMemAccess->SetBreakOnWrite(physAddr,minValue,maxValue);
	mem.SetAccessObject(debugMemAccess,physAddr);
}
/* static */ void i486DebugMemoryAccess::ClearBreakOnMemWrite(Memory &mem,unsigned int physAddr)
{
	auto *curMemAccess=mem.GetAccessObject(physAddr);
	i486DebugMemoryAccess *debugMemAccess=dynamic_cast <i486DebugMemoryAccess *>(curMemAccess);
	if(nullptr!=debugMemAccess)
	{
		debugMemAccess->ClearBreakOnWrite(physAddr);
	}
}
/* static */ void i486DebugMemoryAccess::ClearBreakOnMemWrite(Memory &mem)
{
	for(unsigned long long physAddr=0; physAddr<0x100000000LL; physAddr+=Memory::MEMORY_ACCESS_SLOT_SIZE)
	{
		auto memAccessPtr=mem.GetAccessObject((unsigned int)physAddr);
		auto debugMemAccessPtr=dynamic_cast <i486DebugMemoryAccess *>(memAccessPtr);
		if(nullptr!=debugMemAccessPtr)
		{
			debugMemAccessPtr->ClearBreakOnWrite();
		}
	}
}
