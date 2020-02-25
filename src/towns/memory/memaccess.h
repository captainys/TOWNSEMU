#ifndef MEMACCESS_IS_INCLUDED
#define MEMACCESS_IS_INCLUDED
/* { */

#include "physmem.h"
#include "i486.h"
#include "i486debug.h"

class TownsMemAccess : public MemoryAccess
{
public:
	TownsPhysicalMemory *physMemPtr;
	class i486DX *cpuPtr;
	TownsMemAccess();
	void SetPhysicalMemoryPointer(TownsPhysicalMemory *ptr);
	void SetCPUPointer(class i486DX *cpuPtr);
};


class TownsMainRAMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

class TownsMainRAMorSysROMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

class TownsMainRAMorFMRVRAMAccess : public TownsMemAccess
{
public:
	bool breakOnFMRVRAMWrite,breakOnFMRVRAMRead;
	bool breakOnCVRAMWrite,breakOnCVRAMRead;
	TownsMainRAMorFMRVRAMAccess();
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

class TownsDicROMandDicRAMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

class TownsVRAMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

class TownsSpriteRAMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

class TownsOsROMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

class TownsFontROMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

class TownsWaveRAMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

class TownsSysROMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};


template <class MemAccessClass>
class TownsMemAccessDebug : public MemAccessClass
{
public:
	bool breakOnWrite=false;
	bool breakOnRead=false;

	virtual unsigned int FetchByte(unsigned int physAddr) const
	{
		if(true==breakOnRead && nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("Memory Read");
		}
		return MemAccessClass::FetchByte(physAddr);
	}
	virtual unsigned int FetchWord(unsigned int physAddr) const
	{
		if(true==breakOnRead && nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("Memory Read");
		}
		return MemAccessClass::FetchWord(physAddr);
	}
	virtual unsigned int FetchDword(unsigned int physAddr) const
	{
		if(true==breakOnRead && nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("Memory Read");
		}
		return MemAccessClass::FetchDword(physAddr);
	}
	virtual void StoreByte(unsigned int physAddr,unsigned char data)
	{
		if(true==breakOnWrite && nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("Memory Write");
		}
		MemAccessClass::StoreByte(physAddr,data);
	}
	virtual void StoreWord(unsigned int physAddr,unsigned int data)
	{
		if(true==breakOnWrite && nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("Memory Write");
		}
		MemAccessClass::StoreWord(physAddr,data);
	}
	virtual void StoreDword(unsigned int physAddr,unsigned int data)
	{
		if(true==breakOnWrite && nullptr!=cpuPtr->debuggerPtr)
		{
			cpuPtr->debuggerPtr->ExternalBreak("Memory Write");
		}
		MemAccessClass::StoreDword(physAddr,data);
	}
};


/* } */
#endif
