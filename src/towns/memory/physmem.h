#ifndef PHYSMEM_IS_INCLUDED
#define PHYSMEM_IS_INCLUDED
/* { */

#include <vector>

#include "device.h"
#include "ramrom.h"

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
		bool sysRomMapping;
		bool dicRom;
		bool FMRVRAM;
		unsigned int FMRVRAMMask;
		unsigned int FMRDisplayMode;
		unsigned int FMRVRAMWriteOffset;

		std::vector <unsigned char> RAM;
		std::vector <unsigned char> VRAM;
		std::vector <unsigned char> CVRAM;
		std::vector <unsigned char> spriteRAM;
		std::vector <unsigned char> waveRAM;

		KanjiROMAccess kanjiROMAccess;

		void Reset(void);
	};

	State state;
	std::vector <unsigned char> sysRom,dosRom,fontRom,font20Rom,dicRom,serialRom;

	bool takeJISCodeLog;
	std::vector <unsigned char> JISCodeLog; // Log KanjiROM Read Access

	class i486DX *cpuPtr;

	virtual const char *DeviceName(void) const{return "MEMORY";}



	TownsPhysicalMemory(class i486DX *cpuPtr);

	bool LoadROMImages(const char dirName[]);

	/*! Sets the main RAM size.
	*/
	void SetMainRAMSize(long long int size);

	/*! Sets the VRAM size.
	*/
	void SetVRAMSize(long long int size);

	/*! Sets the Character VRAM size.
	*/
	void SetCVRAMSize(long long int size);

	/*! Sets the SpriteRAM size.
	*/
	void SetSpriteRAMSize(long long int size);

	/*! Sets the WaveRAM size.
	*/
	void SetWaveRAMSize(long long int size);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);
	void Reset(void);
};


/* } */
#endif
