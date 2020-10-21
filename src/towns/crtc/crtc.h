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
#include <cstdint>

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
  3 is for 24KHz modes. (VING titles use CLKSEL=3, but making it 31KHz by adjusting HST)

 [2] pp.149
  CLKSEL=0 BaseClock=28.6363MHz
  CLKSEL=1 BaseClock=24.5454MHz
  CLKSEL=2 BaseClock=25.175MHz
  CLKSEL=3 BaseClock=21.0525MHz
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

	enum
	{
		HIGHRES_REG_CTRL0   =0x000,   // Write 0 -> ?
		HIGHRES_REG_CTRL1   =0x004,   // Read bit0=HighResEnabled bit1=(Initial=1, WriteToBit1->0, HighResoDisabled->1)   Write bit1=1->bit0=0

		HIGHRES_REG_DISPPAGE=0x005,   // Prob  
		HIGHRES_REG_VSYNC1  =0x006,   // Prob
		HIGHRES_REG_HSYNC2  =0x100,
		HIGHRES_REG_HSYNC3  =0x101,
		HIGHRES_REG_VSYNC2  =0x102,
		HIGHRES_REG_VSYNC3  =0x103,
		HIGHRES_REG_XSTART  =0x104,
		HIGHRES_REG_XEND    =0x105,
		HIGHRES_REG_YSTART  =0x106,
		HIGHRES_REG_YEND    =0x107,

		HIGHRES_REG_P0_WID_H   =0x110,
		HIGHRES_REG_P0_WID_L   =0x111,
		HIGHRES_REG_P0_HEI_H   =0x112,
		HIGHRES_REG_P0_HEI_L   =0x113,
		HIGHRES_REG_P0_VRAM_WID=0x114,
		HIGHRES_REG_P0_VRAM_HEI=0x115,
		HIGHRES_REG_P0_ZOOM    =0x119,

		HIGHRES_REG_P1_WID_H   =0x120,
		HIGHRES_REG_P1_WID_L   =0x121,
		HIGHRES_REG_P1_HEI_H   =0x122,
		HIGHRES_REG_P1_HEI_L   =0x123,
		HIGHRES_REG_P1_VRAM_WID=0x124,
		HIGHRES_REG_P1_VRAM_HEI=0x125,
		HIGHRES_REG_P1_ZOOM    =0x129,

		HIGHRES_REG_PALSEL  =0x130,   // Read bit9=PaletteBusy,  Write (Prob) 0->Page0 16-color palette, 1->Page1 16-color palette, 2->256-color palette
		HIGHRES_REG_PALINDEX=0x132,   // Write Palette Index
		HIGHRES_REG_PALCOL  =0x133,   // Write (R<<8)|B then G
	};

	static const unsigned int CLKSELtoHz[4];
	unsigned int CLKSELtoFreq[4];

	class Layer
	{
	public:
		unsigned bitsPerPixel;
		unsigned int VRAMAddr;
		unsigned int VRAMOffset;
		unsigned int FMRVRAMOffset;
		unsigned int FMRGVRAMMask;
		Vec2i originOnMonitor;
		unsigned int VRAMHSkipBytes;
		Vec2i sizeOnMonitor;
		Vec2i VRAMCoverage1X;
		Vec2i zoom2x;
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

	enum
	{
		NUM_CRTC_REGISTERS=32,
		NUM_HIRES_CRTC_REGISTERS=512
	};

	class State
	{
	public:
		bool VSYNCIRQ=false;
		bool VSYNC=false;

		unsigned short crtcReg[NUM_CRTC_REGISTERS];
		unsigned int crtcAddrLatch;

		bool DPMD; // Digital-Palette Modify Flag
		unsigned int FMRPalette[8];

		unsigned char sifter[4];   // Is it really Sifter?  Isn't it Shifter? [2] pp.140
		unsigned int sifterAddrLatch;

		AnalogPalette palette;


		bool highResAvailable=false;
		bool highResCRTCEnabled=false;
		unsigned int highResCrtcReg[NUM_HIRES_CRTC_REGISTERS];
		unsigned int highResCrtcRegAddrLatch;
		bool highResCrtcReg4Bit1=true;
		unsigned int highResPaletteMode,highResPaletteLatch,highResPaletteBRorG;
		AnalogPalette highResCrtcPalette;



		unsigned int FMRGVRAMDisplayPlanes=0x0F;

		// FM-R VRAM Offset is not emulated correctly in the single-page mode.
		// The offset apparently is applied to the VRAM address between CRTC and VRAM,
		// and applied only if the VRAM address is between 00000H and 20000H
		// (first half of layer 0 in the 2-page mode).
		// However, in the single-page mode, in which CRTC reads VRAM staggered way,
		// the output will be staggered.
		// Using this bit in non-FMR mode (Screen Mode 4 X Screen Mode 1) is undefined,
		// and the behavior may be different in other FM Towns models.
		// No FM Towns application that is using this staggered effect has been confirmed.
		uint32_t FMRVRAMOffset;
		bool showPageFDA0[2];
		bool showPage0448[2];

		inline bool ShowPage(int page) const
		{
			return (showPageFDA0[page] && showPage0448[page]);
		}

		void Reset(void);
	};

	/* For CHASE HQ (VING).
	   CHASE HQ does extraordinary.  It uses 16-color (4bit-color) mode for background, but it updates
	   palette for every HSYNC and virtually making it 24-bit color.  In fact, until FM Towns II MX,
	   the maximum number of simultaneou colors is officially 32768 (15-bit) colors.  However, this
	   technique actually draws more colors than official maximum number of colors.  Genius!

	   This class only care when palette is updated by OUT DX,EAX.
	*/
	class ChaseHQPalette
	{
	public:
		enum
		{
			MAX_PALETTE_UPDATE_COUNT=512*16
		};
		unsigned int lastPaletteUpdateCount=0;
		unsigned int paletteUpdateCount=0;
		unsigned char paletteLog[MAX_PALETTE_UPDATE_COUNT*4];

		inline void NextFrame(void)
		{
			lastPaletteUpdateCount=paletteUpdateCount;
			paletteUpdateCount=0;
		}
		inline void AddCodeAndBlue(unsigned char code,unsigned char blue)
		{
			if(paletteUpdateCount<MAX_PALETTE_UPDATE_COUNT)
			{
				paletteLog[(paletteUpdateCount<<2)  ]=code;
				paletteLog[(paletteUpdateCount<<2)+1]=blue;
				++paletteUpdateCount;
			}
		}
		inline void SetRedAndGreen(unsigned char red,unsigned char green)
		{
			if(0<paletteUpdateCount && paletteUpdateCount<=MAX_PALETTE_UPDATE_COUNT)
			{
				auto idx=(paletteUpdateCount-1)<<2;
				paletteLog[idx+2]=red;
				paletteLog[idx+3]=green;
			}
		}
	};
	ChaseHQPalette chaseHQPalette;

	class FMTowns *townsPtr;
	class TownsSprite *spritePtr;
	State state;

	bool cached;   // At this time unused.
	ScreenModeCache cache;   // At this time unused.

	virtual const char *DeviceName(void) const{return "CRTC";}

	TownsCRTC(class FMTowns *ptr,class TownsSprite *spritePtr);

	void UpdateSpriteHardware(void);

	enum
	{
		// VSYNC_CYCLE is 1670000, but it is close enough to 0x1000000(16777216)
		VSYNC_CYCLE=         0x1000000,
		CRT_VERTICAL_DURATION=15360000, // Time CRTC spends for drawing.  VSYNC_CYCLE-CRT_VERTICAL_DURATION gives duration of VSYNC.
		// HSYNC_CYCLE should be 32000, but it is close enough to 0x8000(32768)
		HSYNC_CYCLE=            0x8000, // Not accurate.  Fixed at 31K
		CRT_HORIZONTAL_DURATION= 30000,
	};

	bool InVSYNC(const unsigned long long int townsTime) const;
	bool InHSYNC(const unsigned long long int townsTime) const;

	inline long long int NextVSYNCTime(long long int townsTime) const
	{
		long long int mod=townsTime%VSYNC_CYCLE;
		townsTime-=mod;
		townsTime+=VSYNC_CYCLE-(VSYNC_CYCLE-CRT_VERTICAL_DURATION);
		return townsTime;
	}
	inline long long int NextVSYNCEndTime(long long int townsTime) const
	{
		return NextVSYNCTime(townsTime)+(VSYNC_CYCLE-CRT_VERTICAL_DURATION);
	}
	inline void ProcessVSYNCIRQ(unsigned long long int townsTime)
	{
		unsigned long long int inCycle=townsTime%VSYNC_CYCLE;
		if(true!=state.VSYNC)
		{
			if(CRT_VERTICAL_DURATION<=inCycle)
			{
				state.VSYNC=true;
				/* ChaseHQ Resets the palette for sky-color inside VSYNC IRQ Handler.
				   Start recording here to get first 16 palettes for sky color, next 16 buildings, and the last road.
				*/
				chaseHQPalette.NextFrame();
				TurnOnVSYNCIRQ();
			}
		}
		else if(true==state.VSYNC)
		{
			if(inCycle<CRT_VERTICAL_DURATION)
			{
				state.VSYNC=false;
				TurnOffVSYNCIRQ();
			}
		}
	}
private:
	void TurnOffVSYNCIRQ(void);
	void TurnOnVSYNCIRQ(void);

public:
	/*! [2] pp.152
	*/
	bool InSinglePageMode(void) const;

	/*!
	    This function was initially for I/O FF82H, FM-R compatible mode display page and bit masks.
	    FF82H was supposed to be used only in the FM-R compatible mode, therefore EGB is not supposed
	    to have nay business with this I/O.
	    However, TBIOS EGB function AH=06 writes 27H.

	    One possibility is that I/O-write to FF82 should be disregarded if CRTC is not in the FM-R
	    compatible mode.  This function was initially made for that purpose.

	    However, another possibility is bit 6 of FF82 controls if VRAM offset should be controlled by
	    bit 4.  [2] tells that bit 6 must always be 1.  However, EGB is writing 27H.  Bit 6 is 0.

	    Since TBIOS is writing 27H to FF82H, bit 6 looks to have an unpublished functionality.

	    It returns true if the visible size on the monitor is 640x400, 2-layers, and 16-colors.
	*/
	bool IsInFMRCompatibleMode(void) const;

	unsigned int GetBaseClockFreq(void) const;
	unsigned int GetBaseClockScaler(void) const;


	/*! Returns 512K if not in High-Res mode.  Or 1024KB in High-Res mode.
	    (When High-Res mode is supported....)
	*/
	uint32_t GetEffectiveVRAMSize(void) const;

	/*! Returns scaling.  Between 2 to 8 in each axis.
	    It returns 2 times of actual zoom rate.
	    Since CRTC may scale a pixel up by 1.5, 2.5, 3.5.... times, 1X scale zoom rate cannot represent accurate zoom.
	*/
	Vec2i GetPageZoom2X(unsigned char page) const;

	/*! Returns the page display origin on the monitor in VGA (640x480) coordinate.
	*/
	Vec2i GetPageOriginOnMonitor(unsigned char page) const;
	/*! It is my guess.  The rectangle in which the image is drawn is defined only by
	    HDSx, VDSx, HDEx, and VDEx.  But, what about HAJx?  FM TOWNS Technical Guidebook [2]
	    fell short of explaining the meaning of HAJx.

	    From my observation, probably it is what happens.

	    CRTC starts scanning VRAM after HAJx*clocks after falling edge of HSYNC.
	    But, it really starts drawing at HDSx.  If this interpretation is correct
	    VRAM bytes for
	       (HDSx-HAJx)
	    pixels in 1x scale should be skipped for each line.  The number of bytes 
	    skipped should be:
	       (HDSx-HAJx)*bytesPerPixel/zoomX
	    This function returns (HDSx-HAJx).
	*/
	unsigned int GetVRAMHSkip1X(unsigned char page) const;
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


	void MEMIOWriteFMRVRAMDisplayMode(unsigned char data);	// [2] pp.158

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte twice
	virtual void IOWriteDword(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte 4 times
	virtual unsigned int IOReadByte(unsigned int ioport);

	/* If high-reso crtc is enabled, bit15=0 enables High Res CRTC and disables conventional CRTC, or vise-versa.
	*/
	void WriteCR0(unsigned int data);

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
		auto Hz=CLKSELtoHz[CLKSEL()];
		if(0<state.crtcReg[REG_HST])
		{
			return (Hz/state.crtcReg[REG_HST])/1000;
		}
		return 31; // Just make it 31KHz.  Not to crash.
	}

	std::vector <std::string> GetStatusText(void) const;
	std::vector <std::string> GetPageStatusText(int page) const;
	std::vector <std::string> GetPaletteText(void) const;
};


/* } */
#endif
