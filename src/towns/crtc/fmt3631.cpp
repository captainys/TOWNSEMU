/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */

#include <string.h>
#include "fmt3631.h"
#include "towns.h"
#include "townsdef.h"



FMT3631::FMT3631(class FMTownsCommon *ptr) : Device(ptr)
{
}

void FMT3631::PowerOn(void)
{
	Reset();
}

void FMT3631::Reset(void)
{
	state.sysconfig=0;
	state.interrupt=0;
	state.interrupt_en=0;
	state.status=0;
	memset(state.videoCtrl,0,sizeof(state.videoCtrl));
	memset(state.vramCtrl,0,sizeof(state.vramCtrl));
}

unsigned int FMT3631::IOReadByte(unsigned int ioport)
{
	if(true==state.enabled)
	{
		if(TOWNSIO_FMT_3631_PRESENCE_CHECK==ioport)
		{
			return 0x60;
			// Confirmed with a real FMT-3631.  It returns 0x60 only if TOWNS's on-board Video Out is connected to FMT-3631 RGB-in
			// with a straight cable with pin-7 left unconnected.
			// The cable should not convert pin assignments from TOWNS's D-Sub 15 pins to VGA.
			// The cable is a straight cable, pin-i to pin-i, except pin-7.
			// Connecting pin-7 risks directly connecting TOWNS's CSYNC to GND, which could fry something.
			// .... Wait, once it returnex 0x60 on I/O 0x1100, my FMT-3631 starts returning the same without the straight cable.
			// It could have changed the status of the relay by connecting the straight cable.  The reason is unknown.
		}
	}
	return 0xFF;
}

void FMT3631::IOWriteByte(unsigned int ioport,unsigned int data)
{
}


unsigned int FMT3631::FetchByte(unsigned int physAddr) const
{
	unsigned int data=0xFF;

	if(true==state.enabled)
	{
		auto relAddr=physAddr&TOWNSADDR_FMT3631_AND;
		if(vramBaseAddr<physAddr)
		{
		}
		else
		{
		}
	}

	if(true==monitor)
	{
		std::cout << "Power9000 BYTE read   " << cpputil::Uitox(physAddr) << " " << cpputil::Ubtox(data) << "\n";
	}

	return data;
}

unsigned int FMT3631::FetchDword(unsigned int physAddr) const
{
	unsigned int data=0xFFFFFFFF;

	if(true==state.enabled)
	{
		auto relAddr=physAddr&=TOWNSADDR_FMT3631_AND;
		if(vramBaseAddr<physAddr)
		{
		}
		else
		{
			switch(relAddr)
			{
			case SYSCONFIG: //0x00004,
				data=state.sysconfig;
				break;
			case INTERRUPT: //0x00008,
				data=state.interrupt;
				break;
			case INTERRUPT_EN: //0x0000C,
				data=state.interrupt_en;
				break;

			// Status
			case STATUS          : //0x80000,
				data=state.status;
				break;

			// Control and condition

			// Drawing Engine Registers
			// Pixel Processing 4.5

			// Video Control Registers 4.6
			case HRZC            : //0x00104,
			case HRZT            : //0x00108,
			case HRZSR           : //0x0010C,
			case HRZBR           : //0x00110,
			case HRZBT           : //0x00114,
			case PREHRZC         : //0x00118,
			case VRTC            : //0x0011C,
			case VRTT            : //0x00120,
			case VRSTR           : //0x00124,
			case VRTBR           : //0x00128,
			case VRTBF           : //0x0012C,
			case PREVRTC         : //0x00130,
			case SRADDR          : //0x00134,
			case SRTCTL          : //0x00138,
				data=state.videoCtrl[(relAddr-HRZC)/4];
				break;

			// VRAM Control Registers 4.7
			case MEM_CONFIG      : //0x00184,
			case RFPERIOD        : //0x00188,
			case RFCOUNT         : //0x0018C,
			case RLMAX           : //0x00190,
			case RLCUR           : //0x00194,
				data=state.vramCtrl[(relAddr-MEM_CONFIG)/4];
				break;
			}
		}
	}

	if(true==monitor)
	{
		std::cout << "Power9000 DWORD read  " << cpputil::Uitox(physAddr) << " " << cpputil::Uitox(data) << "\n";
	}

	return data;
}

void FMT3631::StoreByte(unsigned int physAddr,unsigned char data)
{
	if(true==monitor)
	{
		std::cout << "Power9000 BYTE Write   " << cpputil::Uitox(physAddr) << " " <<  cpputil::Ubtox(data) << "\n";
	}

	if(true==state.enabled)
	{
		auto relAddr=physAddr&TOWNSADDR_FMT3631_AND;
		if(vramBaseAddr<physAddr)
		{
		}
		else
		{
		}
	}
}

void FMT3631::StoreDword(unsigned int physAddr,unsigned int data)
{
	if(true==monitor)
	{
		std::cout << "Power9000 DWORD Write  " << cpputil::Uitox(physAddr) << " " << cpputil::Uitox(data) <<  "\n";
	}

	if(true==state.enabled)
	{
		auto relAddr=physAddr&TOWNSADDR_FMT3631_AND;
		if(vramBaseAddr<physAddr)
		{
		}
		else
		{
			switch(relAddr)
			{
			case SYSCONFIG: //0x00004,
				state.sysconfig=data;
				return;
			case INTERRUPT: //0x00008,
				state.interrupt=data;
				return;
			case INTERRUPT_EN: //0x0000C,
				state.interrupt_en=data;
				return;

			// Status
			case STATUS          : //0x80000,
				state.status=data;
				return;

			// Control and condition

			// Drawing Engine Registers
			// Pixel Processing 4.5

			// Video Control Registers 4.6
			case HRZC            : //0x00104,
			case HRZT            : //0x00108,
			case HRZSR           : //0x0010C,
			case HRZBR           : //0x00110,
			case HRZBT           : //0x00114,
			case PREHRZC         : //0x00118,
			case VRTC            : //0x0011C,
			case VRTT            : //0x00120,
			case VRSTR           : //0x00124,
			case VRTBR           : //0x00128,
			case VRTBF           : //0x0012C,
			case PREVRTC         : //0x00130,
			case SRADDR          : //0x00134,
			case SRTCTL          : //0x00138,
				state.videoCtrl[(relAddr-HRZC)/4]=data;
				return;

			// VRAM Control Registers 4.7
			case MEM_CONFIG      : //0x00184,
			case RFPERIOD        : //0x00188,
			case RFCOUNT         : //0x0018C,
			case RLMAX           : //0x00190,
			case RLCUR           : //0x00194,
				state.vramCtrl[(relAddr-MEM_CONFIG)/4]=data;
				return;
			}
		}
	}
}
