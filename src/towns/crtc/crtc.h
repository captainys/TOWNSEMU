#ifndef CRTC_IS_INCLUDED
#define CRTC_IS_INCLUDED
/* { */

#include <vector>

#include "device.h"



class TownsCRTC : public Device
{
public:
	enum
	{
		REG_HSW1=   0x00,
		REG_HSW2=   0x01,
		REG_UNUSED1=0x02,
		REG_UNUSED2=0x03,
		REG_HST=    0x04,
		REG_VST1=   0x05,
		REG_VST2=   0x06,
		REG_EET=    0x07,
		REG_VST=    0x08,
		REG_HDS0=   0x09,
		REG_HDE0=   0x0A,
		REG_HDS1=   0x0B,
		REG_HDE1=   0x0C,
		REG_VDS0=   0x0D,
		REG_VDE0=   0x0E,
		REG_VDS1=   0x0F,
		REG_VDE1=   0x10,
		REG_FA0=    0x11,
		REG_HAJ0=   0x12,
		REG_FO0=    0x13,
		REG_LO0=    0x14,
		REG_FA1=    0x15,
		REG_HAJ1=   0x16,
		REG_FO1=    0x17,
		REG_LO1=    0x18,
		REG_EHAJ=   0x19,
		REG_EVAJ=   0x1A,
		REG_ZOOM=   0x1B,
		REG_CR0=    0x1C,
		REG_CR1=    0x1D,
		REG_FR=     0x1E,
		REG_CR2=    0x1F,
	};

	class State
	{
	public:
		unsigned short crtcReg[32];
		unsigned int crtcAddrLatch;

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
