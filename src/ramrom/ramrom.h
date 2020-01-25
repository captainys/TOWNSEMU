#ifndef RAMROM_IS_INCLUDED
#define RAMROM_IS_INCLUDED
/* { */



class MemoryAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const=0;
	virtual unsigned int FetchWord(unsigned int physAddr) const=0;
	virtual unsigned int FetchDword(unsigned int physAddr) const=0;
	virtual void StoreByte(unsigned int physAddr,unsigned char data)=0;
	virtual void StoreWord(unsigned int physAddr,unsigned char data)=0;
	virtual void StoreDword(unsigned int physAddr,unsigned char data)=0;
};

class Memory
{
private:
	std::vector <MemoryAccess *> memAccessPtr;

public:
	Memory();

	/*! Register memory-access object for physical address physAddrLow to physAddrHigh.
	    Ex.  AddAccess(&low1MBAccess,0x00000000,0x000FFFFF);
	*/
	void AddAccess(MemoryAccess *memAccess,unsigned int physAddrLow,unsigned int physAddrHigh);


	inline unsigned int FetchByte(unsigned int physAddr) const
	{
		auto memAccess=memAccessPtr[physAddr>>16];
		if(nullptr!=memAccess)
		{
			return memAccess->FetchByte(physAddr);
		}
		return 0xff;
	}

	inline unsigned int FetchWord(unsigned int physAddr) const
	{
		auto memAccess=memAccessPtr[physAddr>>16];
		if(nullptr!=memAccess)
		{
			return memAccess->FetchWord(physAddr);
		}
		return 0xffff;
	}

	inline unsigned int FetchDword(unsigned int physAddr) const
	{
		auto memAccess=memAccessPtr[physAddr>>16];
		if(nullptr!=memAccess)
		{
			return memAccess->FetchDword(physAddr);
		return 0xffffffff;
		}
	}

	inline void StoreByte(unsigned int physAddr,unsigned char data)
	{
		auto memAccess=memAccessPtr[physAddr>>16];
		if(nullptr!=memAccess)
		{
			memAccess->StoreByte(physAddr,data);
		}
	}

	inline void StoreWord(unsigned int physAddr,unsigned char data)
	{
		auto memAccess=memAccessPtr[physAddr>>16];
		if(nullptr!=memAccess)
		{
			memAccess->StoreWord(physAddr,data);
		}
	}

	inline void StoreDword(unsigned int physAddr,unsigned char data)
	{
		auto memAccess=memAccessPtr[physAddr>>16];
		if(nullptr!=memAccess)
		{
			memAccess->StoreDword(physAddr,data);
		}
	}
};


/* } */
#endif
