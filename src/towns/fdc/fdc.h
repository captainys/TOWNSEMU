#ifndef FDC_IS_INCLUDED
#define FDC_IS_INCLUDED
/* { */


#include "device.h"

class TownsFDC : public Device
{
public:
	class State
	{
	public:
		void Reset(void);
	};

	class FMTowns *townsPtr;

	State state;

	bool debugBreakOnCommandWrite;

	virtual const char *DeviceName(void) const{return "FDC";}

	TownsFDC(class FMTowns *townsPtr);
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void Reset(void);
};


/* } */
#endif
