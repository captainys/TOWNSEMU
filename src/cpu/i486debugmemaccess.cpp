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
	this->isRedirector=true;
	this->debuggerPtr=&debugger;
	this->physAddrTop=physAddrTop;
	for(unsigned int i=0; i<Memory::MEMORY_ACCESS_SLOT_SIZE; ++i)
	{
		breakOnRead[i]=false;
		breakOnWrite[i]=false;
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
	}
}

/* virtual */ unsigned int i486DebugMemoryAccess::FetchByte(unsigned int physAddr) const
{
	if(true==breakOnRead[physAddr-physAddrTop])
	{
		std::string msg;
		msg="Memory Read BYTE PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="]";
		debuggerPtr->ExternalBreak(msg);
	}
	return redirectPtr->FetchByte(physAddr);
}
/* virtual */ unsigned int i486DebugMemoryAccess::FetchWord(unsigned int physAddr) const
{
	if(true==breakOnRead[physAddr-physAddrTop] ||
	   true==breakOnRead[physAddr+1-physAddrTop])
	{
		std::string msg;
		msg="Memory Read WORD PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="]";
		debuggerPtr->ExternalBreak(msg);
	}
	return redirectPtr->FetchWord(physAddr);
}
/* virtual */ unsigned int i486DebugMemoryAccess::FetchDword(unsigned int physAddr) const
{
	if(true==breakOnRead[physAddr-physAddrTop] ||
	   true==breakOnRead[physAddr+1-physAddrTop] ||
	   true==breakOnRead[physAddr+2-physAddrTop] ||
	   true==breakOnRead[physAddr+3-physAddrTop])
	{
		std::string msg;
		msg="Memory Read DWORD PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="]";
		debuggerPtr->ExternalBreak(msg);
	}
	return redirectPtr->FetchDword(physAddr);
}
/* virtual */ void i486DebugMemoryAccess::StoreByte(unsigned int physAddr,unsigned char data)
{
	if(true==breakOnWrite[physAddr-physAddrTop])
	{
		std::string msg;
		msg="Memory Write BYTE PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="]";
		debuggerPtr->ExternalBreak(msg);
	}
	redirectPtr->StoreByte(physAddr,data);
}
/* virtual */ void i486DebugMemoryAccess::StoreWord(unsigned int physAddr,unsigned int data)
{
	if(true==breakOnWrite[physAddr-physAddrTop] ||
	   true==breakOnWrite[physAddr+1-physAddrTop])
	{
		std::string msg;
		msg="Memory Write WORD PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="]";
		debuggerPtr->ExternalBreak(msg);
	}
	redirectPtr->StoreWord(physAddr,data);
}
/* virtual */ void i486DebugMemoryAccess::StoreDword(unsigned int physAddr,unsigned int data)
{
	if(true==breakOnWrite[physAddr-physAddrTop] ||
	   true==breakOnWrite[physAddr+1-physAddrTop] ||
	   true==breakOnWrite[physAddr+2-physAddrTop] ||
	   true==breakOnWrite[physAddr+3-physAddrTop])
	{
		std::string msg;
		msg="Memory Write DWORD PTR PHYS:[";
		msg+=cpputil::Uitox(physAddr);
		msg+="]";
		debuggerPtr->ExternalBreak(msg);
	}
	redirectPtr->StoreDword(physAddr,data);
}
