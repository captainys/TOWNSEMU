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
	};

	class State
	{
	public:
		bool sysRomMapping;
		bool dicRom;
		bool FMRVRAM;

		std::vector <unsigned char> RAM;
		std::vector <unsigned char> VRAM;
		std::vector <unsigned char> spriteRAM;
		std::vector <unsigned char> waveRAM;

		KanjiROMAccess kanjiROMAccess;

		void Reset(void);
	};

	State state;
	std::vector <unsigned char> sysRom;

	bool takeJISCodeLog;
	std::vector <unsigned char> JISCodeLog; // Log KanjiROM Read Access

	virtual const char *DeviceName(void) const{return "MEMORY";}



	TownsPhysicalMemory();

	bool LoadROMImages(const char dirName[]);

	/*! Sets the main RAM size.
	*/
	void SetMainRAMSize(long long int size);

	/*! Sets the VRAM size.
	*/
	void SetVRAMSize(long long int size);

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
