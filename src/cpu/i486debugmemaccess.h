/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */

#ifndef I486DEBUGMEMACCESS_IS_INCLUDED
#define I486DEBUGMEMACCESS_IS_INCLUDED
/* { */

#include <map>

#include "ramrom.h"
#include "i486.h"
#include "i486debug.h"

class i486DebugMemoryAccess : public MemoryAccess
{
public:
	i486Debugger *debuggerPtr;
	unsigned int physAddrTop;
	using MemoryAccess::memAccessChain;

	bool breakOnRead[Memory::MEMORY_ACCESS_SLOT_SIZE];
	bool breakOnWrite[Memory::MEMORY_ACCESS_SLOT_SIZE];
	unsigned char breakOnWriteMin[Memory::MEMORY_ACCESS_SLOT_SIZE];
	unsigned char breakOnWriteMax[Memory::MEMORY_ACCESS_SLOT_SIZE];
	std::map <uint32_t,unsigned char> breakOnWriteData;

	i486DebugMemoryAccess(i486Debugger &debugger,unsigned int physAddrTop);
	void SetBreakOnRead(unsigned int physAddr);
	void SetBreakOnWrite(unsigned int physAddr);
	void SetBreakOnWrite(unsigned int physAddr,unsigned char minValue,unsigned char maxValue);
	void SetBreakOnWrite(uint32_t physAddr,unsigned char data);
	void ClearBreakOnRead(void);
	void ClearBreakOnWrite(void);
	void ClearBreakOnReadWrite(void);
	void ClearBreakOnRead(unsigned int physAddr);
	void ClearBreakOnWrite(unsigned int physAddr);

	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);

	inline bool CheckBreakOnWriteCondition(uint32_t physAddr,unsigned int data) const;

	static void SetBreakOnMemRead(Memory &mem,i486Debugger &debugger,unsigned int physAddr);
	static void ClearBreakOnMemRead(Memory &mem,unsigned int physAddr);
	static void ClearBreakOnMemRead(Memory &mem);
	static void SetBreakOnMemWrite(Memory &mem,i486Debugger &debugger,unsigned int physAddr);
	static void SetBreakOnMemWrite(Memory &mem,i486Debugger &debugger,unsigned int physAddr,unsigned char data);
	static void ClearBreakOnMemWrite(Memory &mem,unsigned int physAddr);
	static void ClearBreakOnMemWrite(Memory &mem);
};

/* } */
#endif
