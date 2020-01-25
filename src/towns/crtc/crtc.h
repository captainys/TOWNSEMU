#ifndef CRTC_IS_INCLUDED
#define CRTC_IS_INCLUDED
/* { */

#include <vector>

#include "device.h"



class TownsCRTC : public Device
{
public:
	class State
	{
	public:
		std::vector <unsigned int> mxVideoOutCtrl;
		unsigned int mxVideoOutCtrlAddrLatch;

		void Reset(void);
	};

	State state;
	class FMTowns *townsPtr;

	virtual const char *DeviceName(void) const{return "CRTC";}

	TownsCRTC(class FMTowns *ptr);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte twice
	virtual void IOWriteDword(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte 4 times
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void Reset(void);
};


/* } */
#endif
