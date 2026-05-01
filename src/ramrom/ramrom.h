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
		unsigned char FetchByte(void)
		{
			auto byteData=*ptr;
			++ptr;
			--length;
			return byteData;
		}
		/*! Reads two bytes.
		    It does not check length.  Calling function is responsible for checking it.
		*/
		void FetchTwoBytes(unsigned char buf[2])
		{
			cpputil::CopyWord(buf,ptr);
			ptr+=2;
			length-=2;
		}
		/*! Reads two bytes.
		    It does not check length.  Calling function is responsible for checking it.
		*/
		void FetchFourBytes(unsigned char buf[4])
		{
			cpputil::CopyDword(buf,ptr);
			ptr+=4;
			length-=4;
		}
		/*! Reads a byte.
		*/
		unsigned char PeekByte(void) const
		{
			return *ptr;
		}

		void Clear(void)
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
	private:
		/*! Linear address should be filled by the CPU by calling UpdateLinearBaseAddress.
		*/
		unsigned int _linearBaseAddr=~0;

	public:
		/*! Pointer to the memory window.
		    If the memory area cannot be accessed through a pointer, it is nullptr.
		*/
		const unsigned char *ptr=nullptr;

		void CleanUp(void)
		{
			_linearBaseAddr=~0;
			ptr=nullptr;
		}

		void UpdateLinearBaseAddress(unsigned int addr)
		{
			if(nullptr!=ptr)
			{
				_linearBaseAddr=addr&~(MEMORY_WINDOW_SIZE-1);
			}
			else
			{
				_linearBaseAddr=~0;
			}
		}

		bool IsLinearAddressInRange(unsigned int addr) const
		{
			return (addr&~(MEMORY_WINDOW_SIZE-1))==_linearBaseAddr;
		}

		/*! Returns a memory-access pointer from this window.
		    In this case, physical or linear doesn't matter.
		    Higher than 12th bit will be masked anyway.
		*/
		ConstPointer GetReadAccessPointer(unsigned int addr) const
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
	private:
		/*! Linear address should be filled by the CPU.
		*/
		unsigned int linearBaseAddr=~0;

	public:

		/*! Pointer to the memory window.
		    If the memory area cannot be accessed through a pointer, it is nullptr.
		*/
		unsigned char *ptr=nullptr;

		void CleanUp(void)
		{
			linearBaseAddr=~0;
			ptr=nullptr;
		}

		void UpdateLinearBaseAddress(unsigned int addr)
		{
			if(nullptr!=ptr)
			{
				linearBaseAddr=addr&~(MEMORY_WINDOW_SIZE-1);
			}
			else
			{
				linearBaseAddr=~0;
			}
		}

		bool IsLinearAddressInRange(unsigned int addr) const
		{
			return (addr&~(MEMORY_WINDOW_SIZE-1))==linearBaseAddr;
		}
	};
};



/*! Memory class needs to be implemented for each VM.
    Sub class it, and do like:

	unsigned int Memory::FetchByte(unsigned int physAddr) const
	{
		TownsMemory *townsMem=(TownsMemory *)this;
		return townsMem->FetchByte(physAddr);
	}

	if TownsMemory is the true identity of the Memory.
*/
class Memory
{
public:
	class DebuggerLink
	{
	public:
		virtual void FetchByte(uint32_t physAddr) const=0;
		virtual void FetchByteDMA(uint32_t physAddr) const=0;
		virtual void FetchWord(uint32_t physAddr) const=0;
		virtual void FetchDword(uint32_t physAddr) const=0;
		virtual void StoreByte(uint32_t physAddr,unsigned char data)=0;
		virtual void StoreByteDMA(uint32_t physAddr,unsigned char data)=0;
		virtual void StoreWord(uint32_t physAddr,uint32_t data)=0;
		virtual void StoreDword(uint32_t physAddr,uint32_t data)=0;
	};

protected:
	std::vector <DebuggerLink *> debuggerLink;
	enum
	{
		GRANURALITY_SHIFT=12,  // 4KB slot.
	};

public:
	enum
	{
		MEMORY_ACCESS_SLOT_SIZE=(1<<GRANURALITY_SHIFT),
		MEMORY_ACCESS_SLOT_COUNT=(1<<(32-GRANURALITY_SHIFT)),
	};

	MemoryAccess::ConstMemoryWindow EmptyConstMemoryWindow(void) const
	{
		MemoryAccess::ConstMemoryWindow window;
		window.ptr=nullptr;
		return window;
	}
	MemoryAccess::MemoryWindow EmptyMemoryWindow(void) const
	{
		MemoryAccess::MemoryWindow window;
		window.ptr=nullptr;
		return window;
	}



	// Below functions are implemented for specific VM.

	Memory();

	/*! Unassign all memory access objects.
	*/
	void CleanUp(void);


	DebuggerLink *GetDebuggerLink(uint32_t physAddr);

	void SetDebuggerLink(DebuggerLink *debuggerLink,uint32_t addr);
	void ClearDebuggerLink(uint32_t addr);

	unsigned int FetchByte(unsigned int physAddr) const;
	unsigned int FetchByteDMA(unsigned int physAddr) const;
	unsigned int FetchWord(unsigned int physAddr) const;
	unsigned int FetchDword(unsigned int physAddr) const;

	void StoreByte(unsigned int physAddr,unsigned char data);
	void StoreByteDMA(unsigned int physAddr,unsigned char data);
	void StoreWord(unsigned int physAddr,unsigned int data);
	void StoreDword(unsigned int physAddr,unsigned int data);

	MemoryAccess::ConstMemoryWindow GetConstMemoryWindow(unsigned int physAddr) const;
	MemoryAccess::MemoryWindow GetMemoryWindow(unsigned int physAddr);

};


/* } */
#endif
