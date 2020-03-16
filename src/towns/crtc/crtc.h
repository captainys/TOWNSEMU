/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef CRTC_IS_INCLUDED
#define CRTC_IS_INCLUDED
/* { */

#include <vector>

#include "device.h"
#include "cheapmath.h"



/* Reverse Engineering of the CRTC settings.
Probably,
Horizontal page size (in pixels) on the monitor is:
    HDSx-HDEx    in 24 and 31KHz modes, and
   (HDSx-HDEx)/2 in 15KHz mode.
Horizontal VRAM-coverage size (in pixels) that is mapped to the horizontal page size on the monitor is:
   (HDSx-HDEx)/(ZHx+1)

Vertical page size (in pixels) on the monitor is:
   (VDEx-VDSx)/2 if FOx==0, and
    VDEx-VDSx    if F0X!=0.
Vertical VRAM-coverage size (in pixels) that is mapped to the vertical page size on the monitor is:
   ((VDEx-VDSx)/2)/(ZVx+1) if FOx==0, and
   ( VDEx-VDSx   )/(ZVx+1) if F0x!=0.

CLKSEL
  0 and 1 are for 15KHz modes.
  2 is for 31KHz modes.
  3 is for 24KHz modes.
*/



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

	unsigned int CLKSELtoFreq[4];

	class Layer
	{
	public:
		unsigned bitsPerPixel;
		unsigned int VRAMAddr;
		unsigned int VRAMOffset;
		Vec2i originOnMonitor;
		Vec2i sizeOnMonitor;
		Vec2i VRAMCoverage1X;
		Vec2i zoom;
		unsigned int bytesPerLine;

		unsigned int HScrollMask,VScrollMask;
	};
	class ScreenModeCache
	{
	public:
		unsigned int numLayers;
		Layer layer[2];
		ScreenModeCache();
		void MakeFMRCompatible(void);
	};

	class AnalogPalette
	{
	public:
		unsigned int codeLatch;
		Vec3ub plt16[2][16];
		Vec3ub plt256[256];
		void Reset(void);

		void Set16(unsigned int page,unsigned int component,unsigned char v);
		void Set256(unsigned int component,unsigned char v);
		void SetRed(unsigned char v,unsigned int PLT); // PLT is (sifter[1]>>2)&3
		void SetGreen(unsigned char v,unsigned int PLT);
		void SetBlue(unsigned char v,unsigned int PLT);

		unsigned char Get16(unsigned int page,unsigned int component) const;
		unsigned char Get256(unsigned int component) const;
		unsigned char GetRed(unsigned int PLT) const;
		unsigned char GetGreen(unsigned int PLT) const;
		unsigned char GetBlue(unsigned int PLT) const;
	};

	class State
	{
	public:
		unsigned short crtcReg[32];
		unsigned int crtcAddrLatch;

		bool DPMD; // Digital-Palette Modify Flag
		unsigned int FMRPalette[8];

		unsigned char sifter[4];   // Is it really Sifter?  Isn't it Shifter? [2] pp.140
		unsigned int sifterAddrLatch;

		std::vector <unsigned int> mxVideoOutCtrl;
		unsigned int mxVideoOutCtrlAddrLatch;

		AnalogPalette palette;

		void Reset(void);
	};

	class FMTowns *townsPtr;
	State state;

	bool cached;   // At this time unused.
	ScreenModeCache cache;   // At this time unused.

	virtual const char *DeviceName(void) const{return "CRTC";}

	TownsCRTC(class FMTowns *ptr);

	bool InVSYNC(const unsigned long long int townsTime) const;
	bool InHSYNC(const unsigned long long int townsTime) const;

	/*! [2] pp.152
	*/
	bool InSinglePageMode(void) const;

	unsigned int GetBaseClockFreq(void) const;
	unsigned int GetBaseClockScaler(void) const;

	/*! Returns scaling.  Between 1 to 4 in each axis. 
	*/
	Vec2i GetPageZoom(unsigned char page) const;
	/*! Returns the page display origin on the monitor in VGA (640x480) coordinate.
	*/
	Vec2i GetPageOriginOnMonitor(unsigned char page) const;
	/*! Returns width and height of the page display size in VGA (640x480) coordinate.
	*/
	Vec2i GetPageSizeOnMonitor(unsigned char page) const;
	/*! Returns width and height in the VRAM in pixels that is mapped to the page size on the monitor.
	*/
	Vec2i GetPageVRAMCoverageSize1X(unsigned char page) const;
	/*! Returns number of bytes in VRAM per line.
	*/
	unsigned int GetPageBytesPerLine(unsigned char page) const;
	/*! Returns bits per pixel.  4, 8, or 16
	    [2] pp.147
	*/
	unsigned int GetPageBitsPerPixel(unsigned char page) const;
	/*! Get VRAM Address Offset
	    [2] pp.145
	*/
	unsigned int GetPageVRAMAddressOffset(unsigned char page) const;
	/*! Returns priority page 0 or 1.
	*/
	unsigned int GetPriorityPage(void) const;
	/*! Make Layer infor.
	*/
	void MakePageLayerInfo(Layer &layer,unsigned char page) const;


	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte twice
	virtual void IOWriteDword(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte 4 times
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void Reset(void);

	/*! Returns the render size.  At this time it always returns 640x480.
	    If I figure the high-res settings, it may return 1024x768.
	*/
	Vec2i GetRenderSize(void) const;

	inline unsigned int CLKSEL(void) const
	{
		return state.crtcReg[REG_CR1]&3;
	}
	inline unsigned int GetHorizontalFrequency(void) const
	{
		auto freq=CLKSELtoFreq[CLKSEL()];
		if(0<state.crtcReg[REG_HST])
		{
			return freq/state.crtcReg[REG_HST];
		}
		return 31; // Just make it 31KHz.  Not to crash.
	}

	std::vector <std::string> GetStatusText(void) const;
	std::vector <std::string> GetPageStatusText(int page) const;
};


/* } */
#endif
