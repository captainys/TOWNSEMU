#ifndef RAMROM_IS_INCLUDED
#define RAMROM_IS_INCLUDED
/* { */

#include <vector>

#include "device.h"

class Memory : public Device
{
public:
	class State
	{
	public:
		bool sysRomMapping;
		std::vector <unsigned char> RAM;
		std::vector <unsigned char> VRAM;
		std::vector <unsigned char> spriteRAM;
		std::vector <unsigned char> waveRAM;
	};

	State state;
	std::vector <unsigned char> sysRom;

	virtual const char *DeviceName(void) const{return "MEMORY";}

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

	void Reset(void);
};


/* } */
#endif
