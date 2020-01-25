#ifndef MEMACCESS_IS_INCLUDED
#define MEMACCESS_IS_INCLUDED
/* { */

#include "physmem.h"

class TownsMemAccess : public MemoryAccess
{
public:
	TownsPhysicalMemory *physMemPtr;
	TownsMemAccess();
	void SetPhysicalMemoryPointer(TownsPhysicalMemory *ptr);
};


class TownsMainRAMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	//virtual unsigned int FetchWord(unsigned int physAddr) const;
	//virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	//virtual void StoreWord(unsigned int physAddr,unsigned int data);
	//virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

class TownsMainRAMorSysROMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	//virtual unsigned int FetchWord(unsigned int physAddr) const;
	//virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	//virtual void StoreWord(unsigned int physAddr,unsigned int data);
	//virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

class TownsMainRAMorFMRVRAMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
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
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
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


/* } */
#endif
