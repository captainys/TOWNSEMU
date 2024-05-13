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
#include "cpputil.h"

/*! MemoryAccess class is a base-class for actually memory-access implementation.
    MemoryAccess class pointers will be stored in 4KB slots of Memory class so that
    Memory class can direct access to the right memory-access object.

	Default behavior of Word and Dword accesses just call Byte access twice or 4 times respectively.
*/
class MemoryAccess
{
public:
	/*! For debugging memory read/write, a debug memory-access object may be inserted
	    between Memory class and a specific memory-access object.

	    memAccessChain is to tell Memory class that a memory-access object is inserted.

	    AddAccess function only changes the terminal memory-access object, if a memory-access
	    object is inserted.
	*/
	MemoryAccess *memAccessChain=nullptr;

	virtual unsigned int FetchByte(unsigned int physAddr) const=0;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data)=0;
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);

	/* Default behavior is just forward calls to FetchByte and StoreByte.
	*/
	virtual unsigned int FetchByteDMA(unsigned int physAddr) const;
	virtual void StoreByteDMA(unsigned int physAddr,unsigned char data);

	/*! Memory Access Pointer is for skipping segment and paging translation and directly accessing the memory.
	    Not all memory range can be accessible by the MemoryAccess::Pointer, memory-mapped I/O for example.
	    Therefore, the default behavior is returning an unaccessible pointer.

		Due to paging, length must not cross 4K boundary.
	*/
	class ConstPointer
	{
	public:
		unsigned int length;
		const unsigned char *ptr;

		/*! Reads a byte and move the pointer forward by one.
		    It does not check length.  Calling function is responsible for checking it.
		*/
		inline unsigned char FetchByte(void)
		{
			auto byteData=*ptr;
			++ptr;
			--length;
			return byteData;
		}
		/*! Reads two bytes.
		    It does not check length.  Calling function is responsible for checking it.
		*/
		inline void FetchTwoBytes(unsigned char buf[2])
		{
			cpputil::CopyWord(buf,ptr);
			ptr+=2;
			length-=2;
		}
		/*! Reads two bytes.
		    It does not check length.  Calling function is responsible for checking it.
		*/
		inline void FetchFourBytes(unsigned char buf[4])
		{
			cpputil::CopyDword(buf,ptr);
			ptr+=4;
			length-=4;
		}
		/*! Reads a byte.
		*/
		inline unsigned char PeekByte(void) const
		{
			return *ptr;
		}

		inline void Clear(void)
		{
			length=0;
			ptr=nullptr;
		}
	};

	enum
	{
		MEMORY_WINDOW_SIZE=4096
	};

	/*! Memory window is a 4K memory block that can be accessed by a pointer.
	    Not all memory area can be accessed through the memory window, memory-mapped I/O for example.
	    It should be 1-page length, which is 4KB.
	    And it should not cross 4KB border of the physical memory.

		The default behavior is just return a memory window with ptr=nullptr (inaccessible).
	*/
	class ConstMemoryWindow
	{
	public:
		/*! Linear address should be filled by the CPU.
		*/
		unsigned int linearBaseAddr=0;

		/*! Pointer to the memory window.
		    If the memory area cannot be accessed through a pointer, it is nullptr.
		*/
		const unsigned char *ptr=nullptr;

		inline void CleanUp(void)
		{
			linearBaseAddr=0;
			ptr=nullptr;
		}

		inline bool IsLinearAddressInRange(unsigned int addr) const
		{
		#ifdef YS_TWOS_COMPLEMENT
			addr-=linearBaseAddr;
			return addr<MEMORY_WINDOW_SIZE;
		#else
			return (linearBaseAddr<=addr && addr<linearBaseAddr+MEMORY_WINDOW_SIZE);
		#endif
		}

		/*! Returns a memory-access pointer from this window.
		    In this case, physical or linear doesn't matter.
		    Higher than 12th bit will be masked anyway.
		*/
		inline ConstPointer GetReadAccessPointer(unsigned int addr) const
		{
			ConstPointer ptr;
			if(nullptr!=this->ptr)
			{
				ptr.ptr=this->ptr+(addr&0xfff);
				ptr.length=MEMORY_WINDOW_SIZE-(addr&0xfff);
			}
			else
			{
				ptr.length=0;
				ptr.ptr=nullptr;
			}
			return ptr;
		}
	};

	/*! Memory window is a 4K memory block that can be accessed by a pointer.
	    Not all memory area can be accessed through the memory window, memory-mapped I/O for example.
	    It should be 1-page length, which is 4KB.
	    And it should not cross 4KB border of the physical memory.

	    ROM can be accessed from ConstMemoryWindow, but not from MemoryWindow.
	    RAM can be accessed from ConstMemoryWindow and from MemoryWindow.

		The default behavior is just return a memory window with ptr=nullptr (inaccessible).
	*/
	class MemoryWindow
	{
	public:
		/*! Linear address should be filled by the CPU.
		*/
		unsigned int linearBaseAddr=0;

		/*! Pointer to the memory window.
		    If the memory area cannot be accessed through a pointer, it is nullptr.
		*/
		unsigned char *ptr=nullptr;

		inline void CleanUp(void)
		{
			linearBaseAddr=0;
			ptr=nullptr;
		}

		inline bool IsLinearAddressInRange(unsigned int addr) const
		{
		#ifdef YS_TWOS_COMPLEMENT
			addr-=linearBaseAddr;
			return addr<MEMORY_WINDOW_SIZE;
		#else
			return (linearBaseAddr<=addr && addr<linearBaseAddr+MEMORY_WINDOW_SIZE);
		#endif
		}
	};

	virtual ConstMemoryWindow GetConstMemoryWindow(unsigned int physAddr) const;
	virtual MemoryWindow GetMemoryWindow(unsigned int physAddr);
};



class NullMemoryAccess : public MemoryAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

/*! Memory class organizes MemoryAccess objects.
    Fetch and store requests will be directed to memory-access objects based on the 
    pointers stored in the 4KB slots.
*/
class Memory
{
public:
	class MainRAMAccess : public MemoryAccess
	{
	public:
		unsigned int FetchByte(unsigned int physAddr) const override;
		unsigned int FetchWord(unsigned int physAddr) const override;
		unsigned int FetchDword(unsigned int physAddr) const override;
		void StoreByte(unsigned int physAddr,unsigned char data) override;
		void StoreWord(unsigned int physAddr,unsigned int data) override;
		void StoreDword(unsigned int physAddr,unsigned int data) override;

		ConstMemoryWindow GetConstMemoryWindow(unsigned int physAddr) const override;
		MemoryWindow GetMemoryWindow(unsigned int physAddr) override;

		class Memory *memPtr;
		MainRAMAccess(class Memory *memPtr);
	};

	MainRAMAccess mainRAMAccess;
	NullMemoryAccess nullAccess;

	class State
	{
	public:
		std::vector <unsigned char> RAM;
	};
	State state;

private:
	std::vector <MemoryAccess *> memAccessPtr;
	enum
	{
		GRANURALITY_SHIFT=12,  // 4KB slot.
	};

public:
	enum
	{
		MEMORY_ACCESS_SLOT_SIZE=(1<<GRANURALITY_SHIFT),
	};

	Memory();

	/*! Unassign all memory access objects.
	*/
	void CleanUp(void);

	/*! Register memory-access object for physical address physAddrLow to physAddrHigh.
	    Ex.  AddAccess(&low1MBAccess,0x00000000,0x000FFFFF);
	    If a memory-access object chain is assigned to the slot, it only replaces the
	    terminal memory-access object.
	*/
	void AddAccess(MemoryAccess *memAccess,unsigned int physAddrLow,unsigned int physAddrHigh);

	/*! Remove memory access.
	*/
	void RemoveAccess(unsigned int physAddrLow,unsigned int physAddrHigh);

	/*! Register memory-access object for the window that the physical address resides.
	    It disregards the memory-access object chain.  If the chain exists, entire
	    chain is replaced with the incoming memory-access object.
	*/
	void SetAccessObject(MemoryAccess *memAccess,unsigned int physAddr);

	/*! Returns memory access pointer for the given physical address.
	*/
	MemoryAccess *GetAccessObject(unsigned int physAddr);


	inline unsigned int FetchByte(unsigned int physAddr) const
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->FetchByte(physAddr);
	}

	inline unsigned int FetchWord(unsigned int physAddr) const
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->FetchWord(physAddr);
	}

	inline unsigned int FetchDword(unsigned int physAddr) const
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->FetchDword(physAddr);
	}

	inline MemoryAccess::ConstMemoryWindow GetConstMemoryWindow(unsigned int physAddr) const
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->GetConstMemoryWindow(physAddr);
	}
	inline MemoryAccess::MemoryWindow GetMemoryWindow(unsigned int physAddr)
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->GetMemoryWindow(physAddr);
	}

	inline void StoreByte(unsigned int physAddr,unsigned char data)
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		memAccess->StoreByte(physAddr,data);
	}

	inline unsigned int FetchByteDMA(unsigned int physAddr) const
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		return memAccess->FetchByteDMA(physAddr);
	}

	inline void StoreByteDMA(unsigned int physAddr,unsigned char data)
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		memAccess->StoreByteDMA(physAddr,data);
	}

	inline void StoreWord(unsigned int physAddr,unsigned int data)
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		memAccess->StoreWord(physAddr,data);
	}

	inline void StoreDword(unsigned int physAddr,unsigned int data)
	{
		auto memAccess=memAccessPtr[physAddr>>GRANURALITY_SHIFT];
		memAccess->StoreDword(physAddr,data);
	}
};


/* } */
#endif
