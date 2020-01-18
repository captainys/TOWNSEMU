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
		// bool sysRomMapping;
	};

	std::vector <unsigned char> sysRom;

	virtual const char *DeviceName(void) const{return "MEMORY";}

	bool LoadROMImages(const char dirName[]);
};


/* } */
#endif
