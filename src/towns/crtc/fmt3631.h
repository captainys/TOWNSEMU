/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */

#ifndef FMT3631_IS_INCLUDED
#define FMT3631_IS_INCLUDED
/* { */

#include "device.h"
#include "ramrom.h"


// Device Coord Registers are supposed to be:
// 31        15 14            8 7                      0
//  | prefix   | 0 1 1 0 0 0 0 | R R | Rel | Y X | 0 0 0|
//           |        |        |              |         |
// Translated into:
//  0x83xxx
// However, p9000regs.h translates to 0x81xxx.

// The Status Register is supposed to be:
// 31       15 14                       2   1  0
//  | prefix  | 0 1 0 0 0 0 0 0 0 0 0 0 0 | 0 0|
//  |       |        |       |       |         |
// Translated into:
//   0x82000
// However, p9000regs.h translates to 0x80000.

// Should ignore bit 13?

// FMT-3631 Weitek Power 9000 Windows Accelerator Card.
class FMT3631 : public Device, public MemoryAccess
{
public:
	// Based on Linux p9000regs.h and Weitek Power 9100 Graphics Controller Preliminary Nov93
	enum
	{
		// System Control Registers
		SYSCONFIG       =0x00004,
		INTERRUPT       =0x00008,
		INTERRUPT_EN    =0x0000C,

		// Parameter Engine Registers 4.4
		// Device Coordinate
		DEVICE_COORD    =0x81000, // See 4.4.1 for low 8 bits.
		// Status
		STATUS          =0x80000,
		// Control and condition

		// Drawing Engine Registers
		// Pixel Processing 4.5

		// Video Control Registers 4.6
		// Horizontal
		HRZC            =0x00104,
		HRZT            =0x00108,
		HRZSR           =0x0010C,
		HRZBR           =0x00110,
		HRZBT           =0x00114,
		PREHRZC         =0x00118,
		// Vertical
		VRTC            =0x0011C,
		VRTT            =0x00120,
		VRSTR           =0x00124,
		VRTBR           =0x00128,
		VRTBF           =0x0012C,
		PREVRTC         =0x00130,
		// Repaint
		SRADDR          =0x00134,
		SRTCTL          =0x00138,
		//SRTCTL2         = P9100?
		//QSFCOUNTER      = P9100?
		VIDCTRL_LAST    =0x00138,

		// VRAM Control Registers 4.7
		MEM_CONFIG      =0x00184,
		RFPERIOD        =0x00188,
		RFCOUNT         =0x0018C,
		RLMAX           =0x00190,
		RLCUR           =0x00194,
		VRAMCTRL_LAST   =0x00194,
	};

	class State
	{
	public:
		bool enabled=false;

		uint32_t sysconfig=0,interrupt=0,interrupt_en=0;
		uint32_t status=0x40000000;
		uint32_t videoCtrl[(VIDCTRL_LAST-HRZC)/4];
		uint32_t vramCtrl[(VRAMCTRL_LAST-MEM_CONFIG)/4];
	};
	State state;

	const char *DeviceName(void) const override {return "FMT3631";}

	const uint32_t baseAddr=0x46000000;
	const uint32_t vramBaseAddr=46200000;

	FMT3631(class FMTownsCommon *ptr);

	void PowerOn(void) override;
	void Reset(void) override;

	unsigned int IOReadByte(unsigned int ioport) override;
	void IOWriteByte(unsigned int ioport,unsigned int data) override;


	unsigned int FetchByte(unsigned int physAddr) const override;
	unsigned int FetchDword(unsigned int physAddr) const override;
	void StoreByte(unsigned int physAddr,unsigned char data) override;
	void StoreDword(unsigned int physAddr,unsigned int data) override;
};

/* } */
#endif
