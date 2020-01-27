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

	State state;

	virtual const char *DeviceName(void) const{return "FDC";}
	TownsFDC();
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void Reset(void);
};


/* } */
#endif
