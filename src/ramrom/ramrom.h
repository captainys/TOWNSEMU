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
		return 0xffffffff;
		}
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
