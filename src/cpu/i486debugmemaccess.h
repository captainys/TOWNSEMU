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

#include <stdint.h>
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

	uint8_t onRead[Memory::MEMORY_ACCESS_SLOT_SIZE];
	uint8_t onWrite[Memory::MEMORY_ACCESS_SLOT_SIZE];
	unsigned char onWriteMin[Memory::MEMORY_ACCESS_SLOT_SIZE];
	unsigned char onWriteMax[Memory::MEMORY_ACCESS_SLOT_SIZE];

	i486DebugMemoryAccess(i486Debugger &debugger,unsigned int physAddrTop);
	void SetBreakOnRead(unsigned int physAddr,bool monitorOnly);
	void SetBreakOnWrite(unsigned int physAddr,bool monitorOnly);
	void SetBreakOnWrite(unsigned int physAddr,unsigned char minValue,unsigned char maxValue,bool monitorOnly);
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

	virtual unsigned int FetchByteDMA(unsigned int physAddr) const override;
	virtual void StoreByteDMA(unsigned int physAddr,unsigned char data) override;

	inline bool CheckBreakOnWriteCondition(uint32_t physAddr,unsigned int data) const;

	static void SetBreakOnMemRead(Memory &mem,i486Debugger &debugger,unsigned int physAddr,bool monitorOnly);
	static void ClearBreakOnMemRead(Memory &mem,unsigned int physAddr);
	static void ClearBreakOnMemRead(Memory &mem);
	static void SetBreakOnMemWrite(Memory &mem,i486Debugger &debugger,unsigned int physAddr,bool monitorOnly);
	static void SetBreakOnMemWrite(Memory &mem,i486Debugger &debugger,unsigned int physAddr,unsigned char minValue,unsigned char maxValue,bool monitorOnly);
	static void ClearBreakOnMemWrite(Memory &mem,unsigned int physAddr);
	static void ClearBreakOnMemWrite(Memory &mem);
};

/* } */
#endif
