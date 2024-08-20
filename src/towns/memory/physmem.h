/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef PHYSMEM_IS_INCLUDED
#define PHYSMEM_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include <cstdint>

#ifdef REG_NONE  // F**king Windows headers!
#undef REG_NONE
#endif

#include "cpputil.h"
#include "i486.h"
#include "i486debug.h"

#include "device.h"
#include "ramrom.h"
#include "townsdef.h"
#include "memcard.h"



class TownsMemAccess : public MemoryAccess
{
public:
	class TownsPhysicalMemory *physMemPtr;
	class i486DXCommon *cpuPtr;
	TownsMemAccess();
	void SetPhysicalMemoryPointer(TownsPhysicalMemory *ptr);
	void SetCPUPointer(class i486DXCommon *cpuPtr);
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

	virtual ConstMemoryWindow GetConstMemoryWindow(unsigned int physAddr) const;
	virtual MemoryWindow GetMemoryWindow(unsigned int physAddr);
};

class TownsMappedSysROMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);

	virtual ConstMemoryWindow GetConstMemoryWindow(unsigned int physAddr) const;
};

class TownsFMRVRAMAccess : public TownsMemAccess
{
public:
	class TownsCRTC *crtcPtr;
	class FMTownsCommon *townsPtr; // Need townsTime for getting HSYNC.

	bool breakOnFMRVRAMWrite,breakOnFMRVRAMRead;
	bool breakOnCVRAMWrite,breakOnCVRAMRead;
	TownsFMRVRAMAccess();
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

class TownsMappedDICROMandCMOSRAMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

class TownsNativeDICROMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

class TownsNativeCMOSRAMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

template <const uint32_t DISPLACEMENT>
class TownsVRAMAccessTemplate : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

template <const uint32_t DISPLACEMENT>
class TownsVRAMAccessWithMaskTemplate : public TownsVRAMAccessTemplate <DISPLACEMENT>
{
public:
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

class TownsSinglePageVRAMAddressTransform
{
public:
	static inline unsigned int SinglePageOffsetToLinearOffset(unsigned int offset)
	{
		return ((offset&4)<<16)|((offset&0x7fff8)>>1)|(offset&3);
	};
};

class TownsSinglePageHighResVRAMAddressTransform
{
public:
	static inline unsigned int SinglePageOffsetToLinearOffset(unsigned int offset)
	{
		return (offset&0x80000)|((offset&4)<<16)|((offset&0x7fff8)>>1)|(offset&3);
	};
};

template <const uint32_t DISPLACEMENT,class TRANSFORM>
class TownsSinglePageVRAMAccessTemplate : public TownsMemAccess, public TRANSFORM
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

template <const uint32_t DISPLACEMENT,class TRANSFORM>
class TownsSinglePageVRAMAccessWithMaskTemplate : public TownsSinglePageVRAMAccessTemplate <DISPLACEMENT,TRANSFORM>
{
public:
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

class TownsSpriteRAMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual void StoreWord(unsigned int physAddr,unsigned int data);
	virtual void StoreDword(unsigned int physAddr,unsigned int data);
};

class TownsOldMemCardAccess : public TownsMemAccess
{
public:
	class FMTownsCommon *townsPtr; // Need to notify disk read upon BOOTKEY ICM
	TownsOldMemCardAccess(class FMTownsCommon *townsPtr)
	{
		this->townsPtr=townsPtr;
	}

	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

class TownsJEIDA4MemCardAccess : public TownsMemAccess
{
public:
	class FMTownsCommon *townsPtr; // Need to notify disk read upon BOOTKEY ICM
	TownsJEIDA4MemCardAccess(class FMTownsCommon *townsPtr)
	{
		this->townsPtr=townsPtr;
	}

	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

class TownsOsROMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual unsigned int FetchWord(unsigned int physAddr) const;
	virtual unsigned int FetchDword(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

class TownsFontROMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
};

class TownsFont20ROMAccess : public TownsMemAccess
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

	class FMTownsCommon *townsPtr=nullptr;
	class RF5C68 *pcmPtr=nullptr;
	class VGMRecorder *vgmRecorderPtr=nullptr;
	TownsWaveRAMAccess(class FMTownsCommon *townsPtr,class RF5C68 *pcmPtr,class VGMRecorder *vgmRecPtr);
};

class TownsSysROMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);

	virtual ConstMemoryWindow GetConstMemoryWindow(unsigned int physAddr) const;
};

class TownsMartyEXROMAccess : public TownsMemAccess
{
public:
	virtual unsigned int FetchByte(unsigned int physAddr) const;
	virtual void StoreByte(unsigned int physAddr,unsigned char data);
	virtual ConstMemoryWindow GetConstMemoryWindow(unsigned int physAddr) const;
};


template <class MemAccessClass>
class TownsMemAccessDebug : public MemAccessClass
{
public:
	bool breakOnWrite=false;
	bool breakOnRead=false;

	virtual unsigned int FetchByte(unsigned int physAddr) const
	{
		if(true==breakOnRead && nullptr!=this->cpuPtr->debuggerPtr)
		{
			this->cpuPtr->debuggerPtr->ExternalBreak("Memory Read");
		}
		return MemAccessClass::FetchByte(physAddr);
	}
	virtual unsigned int FetchWord(unsigned int physAddr) const
	{
		if(true==breakOnRead && nullptr!=this->cpuPtr->debuggerPtr)
		{
			this->cpuPtr->debuggerPtr->ExternalBreak("Memory Read");
		}
		return MemAccessClass::FetchWord(physAddr);
	}
	virtual unsigned int FetchDword(unsigned int physAddr) const
	{
		if(true==breakOnRead && nullptr!=this->cpuPtr->debuggerPtr)
		{
			this->cpuPtr->debuggerPtr->ExternalBreak("Memory Read");
		}
		return MemAccessClass::FetchDword(physAddr);
	}
	virtual void StoreByte(unsigned int physAddr,unsigned char data)
	{
		if(true==breakOnWrite && nullptr!=this->cpuPtr->debuggerPtr)
		{
			this->cpuPtr->debuggerPtr->ExternalBreak("Memory Write");
		}
		MemAccessClass::StoreByte(physAddr,data);
	}
	virtual void StoreWord(unsigned int physAddr,unsigned int data)
	{
		if(true==breakOnWrite && nullptr!=this->cpuPtr->debuggerPtr)
		{
			this->cpuPtr->debuggerPtr->ExternalBreak("Memory Write");
		}
		MemAccessClass::StoreWord(physAddr,data);
	}
	virtual void StoreDword(unsigned int physAddr,unsigned int data)
	{
		if(true==breakOnWrite && nullptr!=this->cpuPtr->debuggerPtr)
		{
			this->cpuPtr->debuggerPtr->ExternalBreak("Memory Write");
		}
		MemAccessClass::StoreDword(physAddr,data);
	}
};



class TownsPhysicalMemory : public Device
{
public:
	class KanjiROMAccess
	{
	public:
		unsigned char JISCodeHigh; // 000CFF94 Big Endian?
		unsigned char JISCodeLow;  // 000CFF95
		int row;

		void Reset();
		inline unsigned int JISCode(){return JISCodeLow|(JISCodeHigh<<8);}
		inline unsigned int FontROMCode(void)
		{
			if(JISCodeHigh<0x28)
			{
				// 32x8 Blocks
				unsigned int BLK=(JISCodeLow-0x20)>>5;
				unsigned int x=JISCodeLow&0x1F;
				unsigned int y=JISCodeHigh&7;
				if(BLK==1)
				{
					BLK=2;
				}
				else if(BLK==2)
				{
					BLK=1;
				}
				return BLK*32*8+y*32+x;
			}
			else
			{
				// 32x16 Blocks;
				unsigned int BlkX=(JISCodeLow-0x20)>>5;
				unsigned int BlkY=(JISCodeHigh-0x30)>>4;
				unsigned int BLK=BlkY*3+BlkX;
				unsigned int x=JISCodeLow&0x1F;
				unsigned int y=JISCodeHigh&0x0F;
				return 0x400+BLK*32*16+y*32+x;
			}
		}
	};

	class State
	{
	public:
		// Memo for myself:  When implementing state load, make sure to refresh mapping 
		//                   according to sysRomMapping, dicRom, and FMRVRAM flags.
		bool sysRomMapping;  // Whenever changing this flag, synchronously change memory access mapping.
		bool dicRom;
		unsigned int DICROMBank;
		bool FMRVRAM;
		bool TVRAMWrite;
		bool ANKFont;
		unsigned int FMRVRAMMask;
		unsigned int FMRVRAMWriteOffset;

		unsigned int nativeVRAMMaskRegisterLatch=0;
		unsigned char nativeVRAMMask[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; // Repeat twice for faster access.

		std::vector <unsigned char> RAM;
		unsigned char VRAM[TOWNS_VRAM_SIZE];
		std::vector <unsigned char> CVRAM;
		unsigned char spriteRAM[TOWNS_SPRITERAM_SIZE];
		std::vector <unsigned char> notUsed;
		unsigned char CMOSRAM[TOWNS_CMOS_SIZE];

		// PCMCIA memory card.
		ICMemoryCard memCard;
		unsigned int memCardBank=0;
		bool memCardREG=false; // [2] pp.795

		KanjiROMAccess kanjiROMAccess;

		void Reset(void);
	};

	class FMTownsCommon *townsPtr;
	State state;
	std::vector <unsigned char> sysRom,dosRom,fontRom,font20Rom,dicRom;
	std::vector <unsigned char> martyRom;
	enum
	{
		SERIAL_ROM_LENGTH=32
	};
	unsigned char serialROM[SERIAL_ROM_LENGTH];

	// bool preventCMOSInitToSingleDriveMode=true;

	bool takeJISCodeLog;
	std::vector <unsigned char> JISCodeLog; // Log KanjiROM Read Access

	class Memory *memPtr;

	TownsMainRAMAccess mainRAMAccess;
	TownsMappedSysROMAccess mappedSysROMAccess;
	TownsFMRVRAMAccess FMRVRAMAccess;
	TownsMappedDICROMandCMOSRAMAccess mappedDicROMandDicRAMAccess;
	TownsNativeDICROMAccess nativeDicROMAccess;
	TownsNativeCMOSRAMAccess nativeCMOSRAMAccess;
	TownsFontROMAccess fontROMAccess;


	TownsVRAMAccessTemplate           <0> VRAMAccess0;
	TownsSinglePageVRAMAccessTemplate <0,TownsSinglePageVRAMAddressTransform> VRAMAccess1;
	TownsVRAMAccessTemplate           <0> VRAMAccessHighRes0;
	TownsVRAMAccessTemplate           <0x80000> VRAMAccessHighRes1;
	TownsSinglePageVRAMAccessTemplate <0,TownsSinglePageHighResVRAMAddressTransform> VRAMAccessHighRes2;

	TownsVRAMAccessWithMaskTemplate           <0> VRAMAccessWithMask0;
	TownsSinglePageVRAMAccessWithMaskTemplate <0,TownsSinglePageVRAMAddressTransform> VRAMAccessWithMask1;
	TownsVRAMAccessWithMaskTemplate           <0> VRAMAccessWithMaskHighRes0;
	TownsVRAMAccessWithMaskTemplate           <0x80000> VRAMAccessWithMaskHighRes1;
	TownsSinglePageVRAMAccessWithMaskTemplate <0,TownsSinglePageHighResVRAMAddressTransform> VRAMAccessWithMaskHighRes2;

	TownsMemAccessDebug <TownsVRAMAccessTemplate           <0> > VRAMAccess0Debug;
	TownsMemAccessDebug <TownsSinglePageVRAMAccessTemplate <0,TownsSinglePageVRAMAddressTransform> > VRAMAccess1Debug;
	TownsMemAccessDebug <TownsVRAMAccessTemplate           <0> > VRAMAccessHighRes0Debug;
	TownsMemAccessDebug <TownsVRAMAccessTemplate           <0x80000> > VRAMAccessHighRes1Debug;
	TownsMemAccessDebug <TownsSinglePageVRAMAccessTemplate <0,TownsSinglePageHighResVRAMAddressTransform> > VRAMAccessHighRes2Debug;


	TownsSpriteRAMAccess spriteRAMAccess;
	TownsOldMemCardAccess oldMemCardAccess;
	TownsJEIDA4MemCardAccess JEIDA4MemCardAccess;
	TownsOsROMAccess osROMAccess;
	TownsFont20ROMAccess font20ROMAccess;
	TownsWaveRAMAccess waveRAMAccess;
	TownsSysROMAccess sysROMAccess;
	TownsMartyEXROMAccess martyROMAccess;


	virtual const char *DeviceName(void) const{return "MEMORY";}



	TownsPhysicalMemory(class FMTownsCommon *townsPtr,class Memory *memPtr,class RF5C68 *pcmPtr);

	bool LoadROMImages(const char dirName[]);

	/*! Set CMOS image.
	*/
	void SetCMOS(const std::vector <unsigned char> &cmos);

	/*! Sets the main RAM size.
	    Don't call this function directly.  Call FMTownsCommon::SetMainRAMSize, which will call this function from inside.
	*/
	void SetMainRAMSize(long long int size);

	/*! Sets the VRAM size.
	*/
	void SetVRAMSize(long long int size);

	/*! Returns VRAM size.
	*/
	constexpr uint32_t GetVRAMSize(void) const
	{
		return TOWNS_VRAM_SIZE;
	}

	/*! Sets the Character VRAM size.
	*/
	void SetCVRAMSize(long long int size);

	/*! Sets the SpriteRAM size.
	*/
	void SetSpriteRAMSize(long long int size);

	/*! Returns Sprite RAM size.
	*/
	constexpr uint32_t GetSpriteRAMSize(void) const
	{
		return TOWNS_SPRITERAM_SIZE;
	}

	/*! Sets the WaveRAM size.  Used to be.  Now RF5C68 owns the wave RAM.
	*/
	void SetDummySize(long long int size);

	/*! Set up memory access.  Protected-mode memory are mapped differently depending on 386SX or other.
	    To allow 486 core to start, SYSROM is always mapped to the end of 32-bit address space.
	*/
	void SetUpMemoryAccess(unsigned int townsType,unsigned int cpuType);

	/*!
	*/
	void SetUpVRAMAccess(unsigned int cpuType,bool breakOnRead,bool breakOnWrite);

	/*! 
	*/
	void UpdateSysROMDicROMMappingFlag(bool sysRomMapping,bool dicRomMapping);
	void ResetSysROMDicROMMappingFlag(bool sysRomMapping,bool dicRomMapping);


	/*!
	*/
	void UpdateFMRVRAMMappingFlag(bool FMRVRAMMapping);
	void ResetFMRVRAMMappingFlag(bool FMRVRAMMapping);

	/*! Enable or disable Native VRAM Mask depending on the mask.
	    It resets VRAM access objects.
	    Called from IOWriteByte when native VRAM mask changes from/to 0xFFFFFFFF.
	    VRAM Access Mask is a rarely used feature, and always keeping it enabled poses a serious performance hit.
	    VRAM access object must only consider when the mask is not 0xFFFFFFFF.
	    Solution is making two sets of VRAM-access objects one without mask and one with mask, and switching only when necessary.
	*/
	void EnableOrDisableNativeVRAMMask(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport, unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);
	virtual unsigned int IOReadWord(unsigned int ioport);
	void Reset(void);



	class MemoryFilter
	{
	public:
		std::vector <bool> RAMFilter;
		std::vector <unsigned char> prevRAM;
		std::vector <bool> spriteRAMFilter;
		std::vector <unsigned char> prevSpriteRAM;
		unsigned int unit=1; // Unit number of bytes
	};
	MemoryFilter memFilter;

	/*!
	*/
	void BeginMemFilter(unsigned int unit);
	unsigned int ApplyMemFilter(uint32_t currentValue);
	unsigned int ApplyMemFilterDecrease(void);
	unsigned int ApplyMemFilterIncrease(void);
	unsigned int ApplyMemFilterDifferent(void);
	unsigned int ApplyMemFilterEqual(void);
	void PrintMemFilter(void);

	template <class MemFilterHelper> class MemFilterTemplate;



	std::vector <std::string> GetStatusText(void) const;


	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};


template <const uint32_t DISPLACEMENT>
unsigned int TownsVRAMAccessTemplate <DISPLACEMENT>::FetchByte(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	return state.VRAM[((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND)];
}
template <const uint32_t DISPLACEMENT>
unsigned int TownsVRAMAccessTemplate <DISPLACEMENT>::FetchWord(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	return cpputil::GetWord(state.VRAM+((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND));
}
template <const uint32_t DISPLACEMENT>
unsigned int TownsVRAMAccessTemplate <DISPLACEMENT>::FetchDword(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	return cpputil::GetDword(state.VRAM+((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND));
}
template <const uint32_t DISPLACEMENT>
void TownsVRAMAccessTemplate <DISPLACEMENT>::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &state=physMemPtr->state;
	state.VRAM[((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND)]=data;
}
template <const uint32_t DISPLACEMENT>
void TownsVRAMAccessTemplate <DISPLACEMENT>::StoreWord(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	cpputil::PutWord(state.VRAM+((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND),(unsigned short)data);
}
template <const uint32_t DISPLACEMENT>
void TownsVRAMAccessTemplate <DISPLACEMENT>::StoreDword(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	cpputil::PutDword(state.VRAM+((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND),data);
}



template <const uint32_t DISPLACEMENT>
void TownsVRAMAccessWithMaskTemplate<DISPLACEMENT>::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &state=this->physMemPtr->state;
	unsigned char mask=state.nativeVRAMMask[physAddr&3];
	unsigned char nega=~mask;
	state.VRAM[((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND)]&=nega;
	state.VRAM[((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND)]|=(data&mask);
}
template <const uint32_t DISPLACEMENT>
void TownsVRAMAccessWithMaskTemplate<DISPLACEMENT>::StoreWord(unsigned int physAddr,unsigned int data)
{
	auto &state=this->physMemPtr->state;
	unsigned short mask=cpputil::GetWord(state.nativeVRAMMask+(physAddr&3));
	unsigned short nega=~mask;
	unsigned short vram=cpputil::GetWord(state.VRAM+((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND));
	cpputil::PutWord(state.VRAM+((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND),(unsigned short)((vram&nega)|(data&mask)));
}
template <const uint32_t DISPLACEMENT>
void TownsVRAMAccessWithMaskTemplate<DISPLACEMENT>::StoreDword(unsigned int physAddr,unsigned int data)
{
	auto &state=this->physMemPtr->state;
	unsigned int mask=cpputil::GetDword(state.nativeVRAMMask+(physAddr&3));
	unsigned int nega=~mask;
	unsigned int vram=cpputil::GetDword(state.VRAM+((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND));
	cpputil::PutDword(state.VRAM+((physAddr+DISPLACEMENT)&TOWNSADDR_VRAM_AND),(vram&nega)|(data&mask));
}

////////////////////////////////////////////////////////////

template <const unsigned int DISPLACEMENT,class TRANSFORM>
unsigned int TownsSinglePageVRAMAccessTemplate <DISPLACEMENT,TRANSFORM>::FetchByte(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	auto offset=this->SinglePageOffsetToLinearOffset(physAddr&TOWNSADDR_VRAM_AND);
	return state.VRAM[offset];
}
template <const unsigned int DISPLACEMENT,class TRANSFORM>
unsigned int TownsSinglePageVRAMAccessTemplate <DISPLACEMENT,TRANSFORM>::FetchWord(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	auto offset=(physAddr&TOWNSADDR_VRAM_AND);
	if(0==(offset&1))
	{
		offset=this->SinglePageOffsetToLinearOffset(offset);
		return cpputil::GetWord(state.VRAM+offset);
	}
	return cpputil::MakeUnsignedWord(
	    state.VRAM[this->SinglePageOffsetToLinearOffset(offset)],
	    state.VRAM[this->SinglePageOffsetToLinearOffset(offset+1)]);
}
template <const unsigned int DISPLACEMENT,class TRANSFORM>
unsigned int TownsSinglePageVRAMAccessTemplate <DISPLACEMENT,TRANSFORM>::FetchDword(unsigned int physAddr) const
{
	auto &state=physMemPtr->state;
	auto offset=(physAddr&TOWNSADDR_VRAM_AND);
	if(0==(offset&3))
	{
		offset=this->SinglePageOffsetToLinearOffset(offset);
		return cpputil::GetDword(state.VRAM+offset);
	}
	return cpputil::MakeUnsignedDword(
	    state.VRAM[this->SinglePageOffsetToLinearOffset(offset)],
	    state.VRAM[this->SinglePageOffsetToLinearOffset(offset+1)],
	    state.VRAM[this->SinglePageOffsetToLinearOffset(offset+2)],
	    state.VRAM[this->SinglePageOffsetToLinearOffset(offset+3)]);
}
template <const unsigned int DISPLACEMENT,class TRANSFORM>
void TownsSinglePageVRAMAccessTemplate <DISPLACEMENT,TRANSFORM>::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &state=physMemPtr->state;
	auto offset=this->SinglePageOffsetToLinearOffset(physAddr&TOWNSADDR_VRAM_AND);
	state.VRAM[offset]=data;
}
template <const unsigned int DISPLACEMENT,class TRANSFORM>
void TownsSinglePageVRAMAccessTemplate <DISPLACEMENT,TRANSFORM>::StoreWord(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	auto offset=(physAddr&TOWNSADDR_VRAM_AND);
	if(0==(offset&1))
	{
		offset=this->SinglePageOffsetToLinearOffset(offset);
		cpputil::PutWord(state.VRAM+offset,(unsigned short)data);
	}
	else
	{
	#ifdef YS_LITTLE_ENDIAN
		auto *dataPtr=(uint8_t *)&data;
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset)]  =dataPtr[0];
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+1)]=dataPtr[1];
	#else
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset)]  =( data    &255);
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+1)]=((data>>8)&255);
	#endif
	}
}
template <const unsigned int DISPLACEMENT,class TRANSFORM>
void TownsSinglePageVRAMAccessTemplate <DISPLACEMENT,TRANSFORM>::StoreDword(unsigned int physAddr,unsigned int data)
{
	auto &state=physMemPtr->state;
	auto offset=(physAddr&TOWNSADDR_VRAM_AND);
	if(0==(offset&3))
	{
		offset=this->SinglePageOffsetToLinearOffset(offset);
		cpputil::PutDword(state.VRAM+offset,data);
	}
	else
	{
	#ifdef YS_LITTLE_ENDIAN
		auto *dataPtr=(uint8_t *)&data;
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset)]  =dataPtr[0];
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+1)]=dataPtr[1];
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+2)]=dataPtr[2];
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+3)]=dataPtr[3];
	#else
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset)]  =( data     &255);
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+1)]=((data>> 8)&255);
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+2)]=((data>>16)&255);
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+3)]=((data>>24)&255);
	#endif
	}
}



template <const unsigned int DISPLACEMENT,class TRANSFORM>
void TownsSinglePageVRAMAccessWithMaskTemplate<DISPLACEMENT,TRANSFORM>::StoreByte(unsigned int physAddr,unsigned char data)
{
	auto &state=this->physMemPtr->state;
	auto offset=(physAddr&TOWNSADDR_VRAM_AND);
	offset=this->SinglePageOffsetToLinearOffset(offset);

	unsigned char mask=state.nativeVRAMMask[physAddr&3];
	unsigned char nega=~mask;
	state.VRAM[offset]&=nega;
	state.VRAM[offset]|=(data&mask);
}
template <const unsigned int DISPLACEMENT,class TRANSFORM>
void TownsSinglePageVRAMAccessWithMaskTemplate<DISPLACEMENT,TRANSFORM>::StoreWord(unsigned int physAddr,unsigned int data)
{
	auto &state=this->physMemPtr->state;
	auto offset=(physAddr&TOWNSADDR_VRAM_AND);
	if(0==(offset&1))
	{
		offset=this->SinglePageOffsetToLinearOffset(offset);
		unsigned short mask=cpputil::GetWord(state.nativeVRAMMask+(physAddr&3));
		unsigned short nega=~mask;
		unsigned short vram=cpputil::GetWord(state.VRAM+offset);
		cpputil::PutWord(state.VRAM+offset,(unsigned short)((vram&nega)|(data&mask)));
	}
	else
	{
		data&=cpputil::GetWord(state.nativeVRAMMask+(physAddr&3));
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset)  ]&=  ~state.nativeVRAMMask[(physAddr&3)  ];
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset)  ]|=(( state.nativeVRAMMask[(physAddr&3)  ])&(data&255));
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+1)]&=  ~state.nativeVRAMMask[(physAddr&3)+1];
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+1)]|=(( state.nativeVRAMMask[(physAddr&3)+1])&((data>>8)&255));
	}
}
template <const unsigned int DISPLACEMENT,class TRANSFORM>
void TownsSinglePageVRAMAccessWithMaskTemplate<DISPLACEMENT,TRANSFORM>::StoreDword(unsigned int physAddr,unsigned int data)
{
	auto &state=this->physMemPtr->state;
	auto offset=(physAddr&TOWNSADDR_VRAM_AND);
	if(0==(offset&3))
	{
		offset=this->SinglePageOffsetToLinearOffset(offset);
		unsigned int mask=cpputil::GetDword(state.nativeVRAMMask+(physAddr&3));
		unsigned int nega=~mask;
		unsigned int vram=cpputil::GetDword(state.VRAM+offset);
		cpputil::PutDword(state.VRAM+offset,(vram&nega)|(data&mask));
	}
	else
	{
		data&=cpputil::GetDword(state.nativeVRAMMask+(physAddr&3));
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset)  ]&=  ~state.nativeVRAMMask[(physAddr&3)  ];
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset)  ]|=(( state.nativeVRAMMask[(physAddr&3)  ])& (data     &255));
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+1)]&=  ~state.nativeVRAMMask[(physAddr&3)+1];
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+1)]|=(( state.nativeVRAMMask[(physAddr&3)+1])&((data>> 8)&255));
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+2)]&=  ~state.nativeVRAMMask[(physAddr&3)+2];
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+2)]|=(( state.nativeVRAMMask[(physAddr&3)+2])&((data>>16)&255));
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+3)]&=  ~state.nativeVRAMMask[(physAddr&3)+3];
		state.VRAM[this->SinglePageOffsetToLinearOffset(offset+3)]|=(( state.nativeVRAMMask[(physAddr&3)+3])&((data>>24)&255));
	}
}

/* } */
#endif
