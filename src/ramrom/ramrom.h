/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef RAMROM_IS_INCLUDED
#define RAMROM_IS_INCLUDED
/* { */



#include <vector>

/*! MemoryAccess class is a base-class for actually memory-access implementation.
    MemoryAccess class pointers will be stored in 4KB slots of Memory class so that
    Memory class can direct access to the right memory-access object.

	Default behavior of Word and Dword accesses just call Byte access twice or 4 times respectively.
*/
class MemoryAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const=0;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data)=0;
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};



/*! Memory class organizes MemoryAccess objects.
    Fetch and store requests will be directed to memory-access objects based on the 
    pointers stored in the 64KB slots.
*/
class Memory
{
private:
	std::vector <MemoryAccess *> memAccessPtr;
	enum
	{
		GRANURALITY_SHIFT=12,
	};

public:
	Memory();

	/*! Register memory-access object for physical address physAddrLow to physAddrHigh.
	    Ex.  AddAccess(&low1MBAccess,0x00000000,0x000FFFFF);
	*/
	void AddAccess(MemoryAccess *memAccess,unsigned int physAddrLow,unsigned int physAddrHigh);


	inline unsigned int FetchByte(unsigned int physAddr) const
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		if(nullptr!=memAccess)
		{
			return memAccess->FetchByte(physAddr);
		}
		return 0xff;
	}

	inline unsigned int FetchWord(unsigned int physAddr) const
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		if(nullptr!=memAccess)
		{
			return memAccess->FetchWord(physAddr);
		}
		return 0xffff;
	}

	inline unsigned int FetchDword(unsigned int physAddr) const
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		if(nullptr!=memAccess)
		{
			return memAccess->FetchDword(physAddr);
		}
		return 0xffffffff;
	}

	inline void StoreByte(unsigned int physAddr,unsigned char data)
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		if(nullptr!=memAccess)
		{
			memAccess->StoreByte(physAddr,data);
		}
	}

	inline void StoreWord(unsigned int physAddr,unsigned int data)
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		if(nullptr!=memAccess)
		{
			memAccess->StoreWord(physAddr,data);
		}
	}

	inline void StoreDword(unsigned int physAddr,unsigned int data)
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		if(nullptr!=memAccess)
		{
			memAccess->StoreDword(physAddr,data);
		}
	}
};


/* } */
#endif
