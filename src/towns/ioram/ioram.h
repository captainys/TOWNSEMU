#ifndef IORAM_IS_INCLUDED
#define IORAM_IS_INCLUDED
/* { */

#include <vector>

#include "device.h"

/*! Unpublished specification?
    FM Towns IO Port 3000H to 3FFFH(?) just seem to work as a RAM.
*/
class IORam : public Device
{
public:
	class State
	{
	public:
		std::vector <unsigned int> RAM;
	};

	enum
	{
		NUM_PORTS=0x1000,
		PORT_TOP=0x3000
	};

	State state;

	virtual const char *DeviceName(void) const{return "IO_RAM";}

	IORam();

	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data);
	virtual void IOWriteDword(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);
	virtual unsigned int IOReadWord(unsigned int ioport);
	virtual unsigned int IOReadDword(unsigned int ioport);
};

/* } */
#endif
