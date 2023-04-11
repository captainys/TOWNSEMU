/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "device.h"
//#include "townsdef.h"
#include "cpputil.h"


/*
add_library(townsnewdevice newdevice.h newdevice.cpp)
target_link_libraries(townsnewdevice device cpputil towns townsdef)
target_include_directories(townsnewdevice PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
*/

/*
	allDevices.push_back(newDevice);
*/


class DeviceSkeleton : public Device
{
private:
	class FMTownsCommon *townsPtr;
public:
	virtual const char *DeviceName(void) const=0; // You must implement it.

	class State
	{
	public:
		void PowerOn(void);
		void Reset(void);
	};

	State state;

	DeviceSkeleton(class FMTownsCommon *townsPtr);

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);
};

#include "towns.h"

DeviceSkeleton::DeviceSkeleton(class FMTownsCommon *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
}
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

