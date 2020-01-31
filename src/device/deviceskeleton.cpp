#include "device.h"
//#include "townsdef.h"
#include "cpputil.h"


/*
add_library(townsnewdevice newdevice.h newdevice.cpp)
target_link_libraries(townsnewdevice device cpputil towns townsdef)
target_include_directories(townsnewdevice PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
*/


class DeviceSkeleton : public Device
{
private:
	class FMTowns *townsPtr;
public:
	virtual const char *DeviceName(void) const=0; // You must implement it.

	class State
	{
	public:
		void PowerOn(void);
		void Reset(void);
	};

	State state;

	DeviceSkeleton(class FMTowns *townsPtr)
	{
		this->townsPtr=townsPtr;
	}

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);
};

/* virtual */ void DeviceSkeleton::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void DeviceSkeleton::Reset(void)
{
	state.Reset();
}
/* virtual */ void DeviceSkeleton::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		break;
	}
}
/* virtual */ unsigned int DeviceSkeleton::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		break;
	}
	return 0xff;
}

