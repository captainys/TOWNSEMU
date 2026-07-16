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
#include "cheapmath.h"

const uint32_t FMT3631::defPalette[256]=
{
0x000000,0x800000,0x008000,0x808000,0x000080,0x800080,0x008080,0xC0C0C0,0xC0DCC0,0xA6CAF0,0x5F3F3F,0x7F3F3F,0x9F3F3F,0xBF3F3F,0xDF3F3F,0xFF3F3F,
0x3F5F3F,0x5F5F3F,0x7F5F3F,0x9F5F3F,0xBF5F3F,0xDF5F3F,0xFF5F3F,0x3F7F3F,0x5F7F3F,0x7F7F3F,0x9F7F3F,0xBF7F3F,0xDF7F3F,0xFF7F3F,0x3F9F3F,0x5F9F3F,
0x7F9F3F,0x9F9F3F,0xBF9F3F,0xDF9F3F,0xFF9F3F,0x3FBF3F,0x5FBF3F,0x7FBF3F,0x9FBF3F,0xBFBF3F,0xDFBF3F,0xFFBF3F,0x3FDF3F,0x5FDF3F,0x7FDF3F,0x9FDF3F,
0xBFDF3F,0xDFDF3F,0xFFDF3F,0x3FFF3F,0x5FFF3F,0x7FFF3F,0x9FFF3F,0xBFFF3F,0xDFFF3F,0xFFFF3F,0x3F3F5F,0x5F3F5F,0x7F3F5F,0x9F3F5F,0xBF3F5F,0xDF3F5F,
0xFF3F5F,0x3F5F5F,0x5F5F5F,0x7F5F5F,0x9F5F5F,0xBF5F5F,0xDF5F5F,0xFF5F5F,0x3F7F5F,0x5F7F5F,0x7F7F5F,0x9F7F5F,0xBF7F5F,0xDF7F5F,0xFF7F5F,0x3F9F5F,
0x5F9F5F,0x7F9F5F,0x9F9F5F,0xBF9F5F,0xDF9F5F,0xFF9F5F,0x3FBF5F,0x5FBF5F,0x7FBF5F,0x9FBF5F,0xBFBF5F,0xDFBF5F,0xFFBF5F,0x3FDF5F,0x5FDF5F,0x7FDF5F,
0x9FDF5F,0xBFDF5F,0xDFDF5F,0xFFDF5F,0x3FFF5F,0x5FFF5F,0x7FFF5F,0x9FFF5F,0xBFFF5F,0xDFFF5F,0xFFFF5F,0x3F3F7F,0x5F3F7F,0x7F3F7F,0x9F3F7F,0xBF3F7F,
0xDF3F7F,0xFF3F7F,0x3F5F7F,0x5F5F7F,0x7F5F7F,0x9F5F7F,0xBF5F7F,0xDF5F7F,0xFF5F7F,0x3F7F7F,0x5F7F7F,0x7F7F7F,0x9F7F7F,0xBF7F7F,0xDF7F7F,0xFF7F7F,
0x3F9F7F,0x5F9F7F,0x7F9F7F,0x9F9F7F,0xBF9F7F,0xDF9F7F,0xFF9F7F,0x3FBF7F,0x5FBF7F,0x7FBF7F,0x9FBF7F,0xBFBF7F,0xDFBF7F,0xFFBF7F,0x3FDF7F,0x5FDF7F,
0x7FDF7F,0x9FDF7F,0xBFDF7F,0xDFDF7F,0xFFDF7F,0x3FFF7F,0x5FFF7F,0x7FFF7F,0x9FFF7F,0xBFFF7F,0xDFFF7F,0xFFFF7F,0x3F3F9F,0x5F3F9F,0x7F3F9F,0x9F3F9F,
0xBF3F9F,0xDF3F9F,0xFF3F9F,0x3F5F9F,0x5F5F9F,0x7F5F9F,0x9F5F9F,0xBF5F9F,0xDF5F9F,0xFF5F9F,0x3F7F9F,0x5F7F9F,0x7F7F9F,0x9F7F9F,0xBF7F9F,0xDF7F9F,
0xFF7F9F,0x3F9F9F,0x5F9F9F,0x7F9F9F,0x9F9F9F,0xBF9F9F,0xDF9F9F,0xFF9F9F,0x3FBF9F,0x5FBF9F,0x7FBF9F,0x9FBF9F,0xBFBF9F,0xDFBF9F,0xFFBF9F,0x3FDF9F,
0x5FDF9F,0x7FDF9F,0x9FDF9F,0xBFDF9F,0xDFDF9F,0xFFDF9F,0x3FFF9F,0x5FFF9F,0x7FFF9F,0x9FFF9F,0xBFFF9F,0xDFFF9F,0xFFFF9F,0x3F3FBF,0x5F3FBF,0x7F3FBF,
0x9F3FBF,0xBF3FBF,0xDF3FBF,0xFF3FBF,0x3F5FBF,0x5F5FBF,0x7F5FBF,0x9F5FBF,0xBF5FBF,0xDF5FBF,0xFF5FBF,0x3F7FBF,0x5F7FBF,0x7F7FBF,0x9F7FBF,0xBF7FBF,
0xDF7FBF,0xFF7FBF,0x3F9FBF,0x5F9FBF,0x7F9FBF,0x9F9FBF,0xBF9FBF,0xDF9FBF,0xFF9FBF,0x3FBFBF,0x5FBFBF,0x7FBFBF,0x9FBFBF,0xBFBFBF,0xDFBFBF,0xFFBFBF,
0x3FDFBF,0x5FDFBF,0x7FDFBF,0x9FDFBF,0xBFDFBF,0xDFDFBF,0xFFFBF0,0xA0A0A4,0x808080,0xFF0000,0x00FF00,0xFFFF00,0x0000FF,0xFF00FF,0x00FFFF,0xFFFFFF,
};

FMT3631::FMT3631(class FMTownsCommon *ptr) : Device(ptr)
{
	state.vram.clear();
	mutableThis=this;
}

void FMT3631::Disable(void)
{
	state.enabled=false;
	state.isFMT3632=false;
	state.vram.clear();
}

void FMT3631::EnableAsFMT3631(void)
{
	state.enabled=true;
	state.isFMT3632=false;
	state.vram.resize(VRAM_SIZE_3631);
	state.BaseAddr=TOWNSADDR_FMT3631_BASE;
	state.RAMDACBaseAddr=TOWNSADDR_FMT3631_BASE;
	state.ControlBaseAddr=TOWNSADDR_FMT3631_BASE+CONTROL_BEGIN_3631;
	state.VRAMBaseAddr=TOWNSADDR_FMT3631_BASE+VRAM_BEGIN_3631;
	state.VRAMEndAddr=TOWNSADDR_FMT3631_BASE+VRAM_END_3631;
	state.hwCursor.originX=32;
	state.hwCursor.originY=32;
}

void FMT3631::EnableAsFMT3632(void)
{
	state.enabled=true;
	state.isFMT3632=true;
	state.vram.resize(VRAM_SIZE_3632);
	state.BaseAddr=TOWNSADDR_FMT3632_BASE;
	state.RAMDACBaseAddr=TOWNSADDR_FMT3632_BASE;
	state.ControlBaseAddr=TOWNSADDR_FMT3632_BASE+CONTROL_BEGIN_3632;
	state.VRAMBaseAddr=TOWNSADDR_FMT3632_BASE+VRAM_BEGIN_3632;
	state.VRAMEndAddr=TOWNSADDR_FMT3632_BASE+VRAM_END_3632;
	state.hwCursor.originX=0;
	state.hwCursor.originY=0;
}

void FMT3631::PowerOn(void)
{
	Reset();
}

void FMT3631::Reset(void)
{
	memset(state.btCommandReg,0,sizeof(state.btCommandReg));

	state.masterSwitch=0;
	state.sysconfig=0;
	state.interrupt=0;
	state.interrupt_en=0;
	state.status=0;
	state.alt_read_bank=0;
	state.alt_write_bank=0;
	memset(state.videoCtrl,0,sizeof(state.videoCtrl));
	memset(state.vramCtrl,0,sizeof(state.vramCtrl));
	memset(state.drawingAttrib,0,sizeof(state.drawingAttrib));
	memset(state.vram.data(),0,state.vram.size());
	memset(state.pattern,255,sizeof(state.pattern));

	state.nLoadedCoord=0;
	state.lastLoadedCoord=0;
	state.nextLoadIndex=0;
	memset(state.coord,0,sizeof(state.coord));
	for(auto &i : state.metaCoordType)
	{
		i=LOAD_COORD_PRIMTYPE_NOT_LOADED;
	}

	memset(state.ctlCond,0,sizeof(state.ctlCond));

	state.bitsPerPixel=8;
	state.highColor565=false;
	state.pixelLeftUp=Vec2i::Make(0,0);
	state.pixelCurrent=Vec2i::Make(0,0);
	state.pixelWid=0;
	state.pixelYIncrement=1;

	state.plt.Reset();
	for(int i=0; i<256; ++i)
	{
		auto r=(defPalette[i]>>16)&255;
		auto g=(defPalette[i]>>8)&255;
		auto b=(defPalette[i])&255;
		state.plt.plt256[i].Set(r,g,b,255);
	}

	state.hwCursor.Reset();
	state.hwCursor.defined=true; // I don't know what triggers to make it visible.
	if(true!=state.isFMT3632)
	{
		state.hwCursor.originX=32;
		state.hwCursor.originY=32;
	}
	else
	{
		state.hwCursor.originX=0;
		state.hwCursor.originY=0;
	}
	state.hwCursor.wid=32;
	state.hwCursor.twoColorCursor=false;
	state.hwCursor.twoColor[0]=0xff;
	state.hwCursor.twoColor[1]=0xff;
	state.hwCursor.twoColor[2]=0xff;
	state.hwCursor.twoColor[3]=0;
	state.hwCursor.twoColor[4]=0;
	state.hwCursor.twoColor[5]=0;
	state.hwCursorTwoColorReadPos=0;

	state.fmt3632RegSel=0;
	memset(state.fmt3632Regs,0,sizeof(state.fmt3632Regs));
}

int FMT3631::U16toS16(uint32_t in)
{
	int x=in;
	if(0!=(in&0x8000))
	{
		x&=0x7FFF;
		x-=0x8000;
	}
	return x;
}
int FMT3631::U32toS32(uint32_t in)
{
	int x=in;
	if(0!=(in&0x80000000))
	{
		x&=0x7FFFFFFF;
		x-=0x80000000;
	}
	return x;
}
uint32_t FMT3631::S32toU32(int in)
{
	if(0<=in)
	{
		return in;
	}
	else
	{
		in+=0x40000000;
		in+=0x40000000;
		uint32_t data=in;
		data+=0x40000000;
		data+=0x40000000;
		return data;
	}
}
uint32_t FMT3631::ByteSwap32(uint32_t in)
{
	uint32_t out=0;
	out=in>>24;
	out|=((in>>8)&0xFF00);
	out|=((in<<8)&0xFF0000);
	out|=((in<<24)&0xFF000000);
	return out;
}

bool FMT3631::IsEnabled(void) const
{
	// According to p9000init.c of Linux XFree86 source, writing
	// 1e5, 1e4, or 1c4 enables Power 9000 video output.
	// 1e5 for 2MB VRAM (FMT-3631 is this case)    0001 1110 0101
	// 1e4 for 1MB VRAM.                           0001 1110 0100
	// 1c4 is unknown, but I don't care.           0001 1100 0100

	// Windows driver sets bit 4 of 0x46000000 to enable, and clears to disable FMT-3631.
	// Probably this bit works as a master switch of FMT-3631.  Maybe this controls the relay on the board.

	auto srtctl=*GetControlWordPtr(SRTCTL);
	return true==state.enabled /*&& 0x1c4==(srtctl&0x1c4)*/ && 0!=(MS_ENABLE&state.masterSwitch);
}

unsigned int FMT3631::Height(void) const
{
	auto br=*GetControlWordPtr(VRTBR);
	auto bf=*GetControlWordPtr(VRTBF);
	return bf-br;
}

unsigned int FMT3631::Width(void) const
{
	return BytesPerLine()*8/BitsPerPixel();
}

unsigned int FMT3631::BitsPerPixel(void) const
{
	return state.bitsPerPixel;
}

unsigned int FMT3631::BytesPerLine(void) const
{
	auto br=*GetControlWordPtr(HRZBR);
	auto bf=*GetControlWordPtr(HRZBF);
	if(true!=state.isFMT3632)
	{
		return (bf-br)*4;
	}
	else
	{
		return (bf-br)*8;
	}
}

unsigned int FMT3631::BytesPerPixel(void) const
{
	return state.bitsPerPixel/8;
}

void FMT3631::MakePageLayerInfo(Layer &layer) const
{
	layer.bitsPerPixel=BitsPerPixel();
	layer.highColor565=state.highColor565;
	layer.highColorGRB=false;

	layer.highResRGBSwap=0b100100;
	// Apparently GBR or BRG
	// GBR:  G=0  B=1  R=2
	// BGR:  B=0  G=1  R=2
	// RGBSwap=RRGGBB     RRGGBB
	//         2 1 0  or  2 0 1
	//         100100 or  100001

	layer.VRAMAddr=0;

	layer.VRAMOffset=0;
	layer.FlipVRAMOffset =0;        // FM-R/Sprite page is not applicable to Power 9000.
	layer.FMRGVRAMMask=0x0F;        // Hopefully there's no mask in Power 9000.
	layer.originOnMonitor=Vec2i::Make(0,0);
	layer.VRAMHSkipBytes=0;
	layer.sizeOnMonitor=Vec2i::Make(Width(),Height());
	layer.VRAMCoverage1X=layer.sizeOnMonitor;
	layer.zoom2x=Vec2i::Make(2,2);
	layer.bytesPerLine=BytesPerLine();

	layer.HScrollMask=0xFFFFFFFF;
	layer.VScrollMask=state.vram.size()-1;
}
const FMT3631::AnalogPalette &FMT3631::GetPalette(void) const
{
	return state.plt;
}

unsigned int FMT3631::IOReadByte(unsigned int ioport)
{
	if(true==state.enabled)
	{
		if(TOWNSIO_FMT_3631_PRESENCE_CHECK==ioport) // 0x1100
		{
			if(true!=state.isFMT3632)
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
			else
			{
				// Windows 95 FMT-3632 driver expects 0x80.
				return 0x80|(state.masterSwitch&MS_ENABLE);
			}
		}
		else if(TOWNSIO_FMT_3632_1==ioport) // 0x1101
		{
			// b0 apparently indicates 2MB or 4MB.  b0==1 probably indicates that 4MB.
			if(true==state.isFMT3632)
			{
				return 3;
			}
		}
		else if(TOWNSIO_FMT_3632_2==ioport) // 0x9100
		{
			return state.fmt3632RegSel; // Maybe it should return reg sel.
		}
		else if(TOWNSIO_FMT_3632_3==ioport) // 0x9104
		{
			return state.fmt3632Regs[state.fmt3632RegSel&255];
		}
	}
	return 0xFF;
}

void FMT3631::IOWriteByte(unsigned int ioport,unsigned int data)
{
	if(true==state.enabled)
	{
		if(TOWNSIO_FMT_3631_PRESENCE_CHECK==ioport) // 0x1100
		{
			if(true==state.isFMT3632)
			{
				state.masterSwitch=data; // FMT-3632 apparently uses I/O 1100h for enabling/disabling.
			}
		}
		else if(TOWNSIO_FMT_3632_2==ioport)
		{
			state.fmt3632RegSel=data;
		}
		else if(TOWNSIO_FMT_3632_3==ioport) // 0x9104
		{
			state.fmt3632Regs[state.fmt3632RegSel&255]=data;
		}
	}
}

bool FMT3631::IsReadableParameter(uint32_t &data,uint32_t physAddr) const
{
	if((0xFFF07&physAddr)==DEVICE_COORD)
	{
		auto rel=(0!=(physAddr&LOAD_COORD_ABS_REL_MASK));
		if(true==rel)
		{
			return false; // Window-Relative is write only.
		}

		auto idx=(physAddr>>6)&3;
		auto coord=state.coord[idx];

		auto x_y_or_xy=physAddr&LOAD_COORD_X_Y_XY_MASK;
		if(LOAD_COORD_XY==x_y_or_xy)
		{
			uint32_t x=S32toU32(coord.x());
			uint32_t y=S32toU32(coord.y());
			data=(x<<16)|(y&0xFFFF);
			return true;
		}
		else if(LOAD_COORD_X==x_y_or_xy)
		{
			data=S32toU32(coord.x());
			return true;
		}
		else if(LOAD_COORD_Y==x_y_or_xy)
		{
			data=S32toU32(coord.y());
			return true;
		}

		return true;
	}
	if(P_W_MIN==physAddr-state.ControlBaseAddr) //0x80194, // Read Only
	{
		data=*GetControlWordPtr(WINDOW_MIN);
		return true;
	}
	if(P_W_MAX==physAddr-state.ControlBaseAddr)
	{
		data=*GetControlWordPtr(WINDOW_MAX);
		return true;
	}
	if(VRTC==physAddr-state.ControlBaseAddr)
	{
		auto h=Height();
		if(0<h)
		{
			// 60fps
			auto *towns=(FMTownsCommon *)vmPtr;
			const uint64_t nsPerFrame=1000000000/60;
			const uint64_t nsIntoFrame=towns->state.townsTime%nsPerFrame;

			// I don't know exactly how long VSYNC takes, but let's say 5% of the frame.
			const uint64_t nsPerLine=(nsPerFrame*95/100)/h;

			const uint64_t linesIntoFrame=nsIntoFrame/nsPerLine;
			data=(uint32_t)linesIntoFrame;
			return true;
		}
	}
	if(HRZC==physAddr-state.ControlBaseAddr)
	{
		auto h=Height();
		auto w=Width();
		if(0<h && 0<w)
		{
			// 60fps
			auto *towns=(FMTownsCommon *)vmPtr;
			const uint64_t nsPerFrame=1000000000/60;
			const uint64_t nsIntoFrame=towns->state.townsTime%nsPerFrame;

			// I don't know exactly how long VSYNC takes, but let's say 5% of the frame.
			const uint64_t nsPerLine=(nsPerFrame*95/100)/h;
			const uint64_t nsIntoLine=nsIntoFrame%nsPerLine;

			const uint64_t pixelIntoLine=w*nsIntoLine/nsPerLine;
			data=(uint32_t)pixelIntoLine;

			return true;
		}
	}
	if(POWER_UP_CONFIG==physAddr-state.ControlBaseAddr)
	{
		if(true==state.isFMT3632)
		{
			// This is used for selecting a possible-bpp table, and a jump table.
			// Only 0,1,2,4, or 8 are possible.
			//             0     1     2           4
			// DS:00000328 4C 0B 18 0C AC 0C 00 00 4C 0B 00 00 00 00 00 00|L       L
			//             8
			// DS:00000338 2C 0D 00 00 00 00 00 00 00 00 00 00 00 00 00 00|,

			// 0: Thinking it's Power 9000.  Cannot select 24-bit color (Screen corrupts).
			// 1: ?
			// 2: ?
			// 4: Crash
			// 8: Works.  But, bytesPerLine becomes (HRZBF-HRZBR)*8.  Maybe it is what it should be.
			data=(8<<12);
		}
		else
		{
			data=0;
		}
		return true;
	}
	return false;
}


template <class returnType,class stateType>
inline returnType FMT3631::GetControlWordPtrTemplate(uint32_t physAddr,stateType &state)
{
	if(true!=state.isFMT3632)
	{
		switch(physAddr-state.BaseAddr)
		{
		case BT_COMMAND_REG_1:
			return &state.btCommandReg[1];
		case BT_COMMAND_REG_2:
			return &state.btCommandReg[2];
		case BT_COMMAND_REG_3:
			return &state.btCommandReg[3];

		// Somehow SYSCONFIG, INTERRUPT, INTERRUPT_EN are mapped to 4600000x,
		// while all other registers are mapped to 461xxxxx.
		case MASTERSWITCH: // 0x0
			return &state.masterSwitch;
		case SYSCONFIG: //0x00004,
			return &state.sysconfig;
		case INTERRUPT: //0x00008,
			return &state.interrupt;
		case INTERRUPT_EN: //0x0000C,
			return &state.interrupt_en;
		}
	}

	auto relAddr=physAddr&COMMAND_MASK;

	if(FGCOLOR_BYTESWAP==relAddr ||
	   BGCOLOR_BYTESWAP==relAddr ||
	   COLOR2_BYTESWAP==relAddr ||
	   COLOR3_BYTESWAP==relAddr)
	{
		// Supposed to be:
		// P9100 manual p. 25 Figure 42.
		// b18  H  Half-word swap
		// b17  B  Byte within half-word swap
		// b16  b  Bit within byte swap
		// eg.
		// 0x70000 Bit-Reverse
		// 0x60000 Byte-Reverse
		// But, if I implemented it, it was messed up.
		relAddr&=~0x70000;
	}

	switch(relAddr)
	{
	case SYSCONFIG: //0x00004,
		return &state.sysconfig;
	case INTERRUPT: //0x00008,
		return &state.interrupt;
	case INTERRUPT_EN: //0x0000C,
		return &state.interrupt_en;

	case ALT_READ_BANK: // 0x00010,
		return &state.alt_read_bank;
	case ALT_WRITE_BANK: // 0x00014,
		return &state.alt_write_bank;


	// Status
	case STATUS          : //0x80000,
		return &state.status;

	// Control and condition
	case OOR             : //0x80184,
	//               0x80188, // Not used
	case CINDEX          : //0x8018C,
	case WINDOW_OFFSET_XY: //0x80190,
	case P_W_MIN         : //0x80194, // Read Only
	case P_W_MAX         : //0x80198,
	//               0x8019C, // Not used
	case YCLIP           : //0x801A0,
	case XCLIP           : //0x801A4,
	case XEDGE_LT        : //0x801A8,
	case XEDGE_GT        : //0x801AC,
	case YEDGE_LT        : //0x801B0,
	case YEDGE_GT        : //0x801B4,
		return &state.ctlCond[(relAddr-CTL_COND_BEGIN)/4];

	// Drawing Engine Registers
	// Pixel Processing 4.5
	case FGCOLOR:
	case BGCOLOR:
	case PLANE_MASK      ://0x80208,
	case DRAWING_MODE    ://0x8020C,
	case PATTERN_X0      ://0x80210,
	case PATTERN_Y0      ://0x80214,
	case RASTER          ://0x80218,
	case PIXEL8          ://0x8021C,
	case WINDOW_MIN      ://0x80220,
	case WINDOW_MAX      ://0x80224,
		return &state.drawingAttrib[(relAddr-FGCOLOR)/4];

	case PATTERN0:
	case PATTERN1:
	case PATTERN2:
	case PATTERN3:
	case PATTERN4:
	case PATTERN5:
	case PATTERN6:
	case PATTERN7:
		return &state.pattern[(relAddr-PATTERN0)/4];

	// Video Control Registers 4.6
	case HRZC            : //0x00104,
	case HRZT            : //0x00108,
	case HRZSR           : //0x0010C,
	case HRZBR           : //0x00110,
	case HRZBF           : //0x00114,
	case PREHRZC         : //0x00118,
	case VRTC            : //0x0011C,
	case VRTT            : //0x00120,
	case VRTSR           : //0x00124,
	case VRTBR           : //0x00128,
	case VRTBF           : //0x0012C,
	case PREVRTC         : //0x00130,
	case SRADDR          : //0x00134,
	case SRTCTL          : //0x00138,
		return &state.videoCtrl[(relAddr-HRZC)/4];

	// VRAM Control Registers 4.7
	case MEM_CONFIG      : //0x00184,
	case RFPERIOD        : //0x00188,
	case RFCOUNT         : //0x0018C,
	case RLMAX           : //0x00190,
	case RLCUR           : //0x00194,
		return &state.vramCtrl[(relAddr-MEM_CONFIG)/4];

	case COLOR2:          // 0x80238, 010 0010 0011 1000 P9100 only
	case COLOR3:          // 0x8023C, 010 0010 0011 1100 P9100 only
		return &state.color2_3[(relAddr-COLOR2)/4];

	case BYTE_WIN_MIN:    // 0x802A0, 010 0010 1010 0000 P9100 only
	case BYTE_WIN_MAX:    // 0x802A4, 010 0010 1010 0100 P9100 only
		return &state.byteWinMinMax[(relAddr-BYTE_WIN_MIN)/4];
	}
	return nullptr;
}

const uint32_t *FMT3631::GetControlWordPtr(unsigned int physAddr) const
{
	return GetControlWordPtrTemplate<const uint32_t *,const State>(physAddr,state);
}

uint32_t *FMT3631::GetControlWordPtr(unsigned int physAddr)
{
	return GetControlWordPtrTemplate<uint32_t *,State>(physAddr,state);
}

Vec2i FMT3631::GetWindowOffset(void) const
{
	uint32_t offset=*GetControlWordPtr(WINDOW_OFFSET_XY);
	Vec2i v;
	v.x()=U16toS16(offset>>16);
	v.y()=U16toS16(offset&0xffff);
	return v;
}

Vec2i FMT3631::GetWindowMin(void) const
{
	uint32_t dw=*GetControlWordPtr(WINDOW_MIN);
	Vec2i v;
	v.x()=U16toS16(dw>>16);
	v.y()=U16toS16(dw&0xffff);
	return v;
}

Vec2i FMT3631::GetWindowMax(void) const
{
	uint32_t dw=*GetControlWordPtr(WINDOW_MAX);
	Vec2i v;
	v.x()=U16toS16(dw>>16);
	v.y()=U16toS16(dw&0xffff);
	return v;
}

int FMT3631::DeviceCoordOrLoadCoord(Vec2i &coordToLoad,Vec2i absRef,Vec2i relRef,uint32_t physAddr,uint32_t data)
{
	auto rel=(0!=(physAddr&LOAD_COORD_ABS_REL_MASK));

	auto x_y_or_xy=physAddr&LOAD_COORD_X_Y_XY_MASK;
	if(LOAD_COORD_XY==x_y_or_xy)
	{
		int x=U16toS16(data>>16);
		int y=U16toS16(data&0xFFFF);
		coordToLoad.Set(x,y);
		if(true==rel)
		{
			coordToLoad.x()+=relRef.x();
			coordToLoad.y()+=relRef.y();
		}
		else
		{
			coordToLoad.x()+=absRef.x();
            coordToLoad.y()+=absRef.y();
		}
		if(true==monitorCtrl)
		{
			std::cout << "Coord " << coordToLoad.x() << " " << coordToLoad.y() << "\n";
		}
		return 1;
	}
	else if(LOAD_COORD_X==x_y_or_xy)
	{
		int coord=U32toS32(data);
		coordToLoad.x()=coord;
		if(true==rel)
		{
			coordToLoad.x()+=relRef.x();
		}
		else
		{
			coordToLoad.x()+=absRef.x();
		}
		if(true==monitorCtrl)
		{
			std::cout << "Coord X " << coordToLoad.x() << "\n";
		}
		return 0;
	}
	else if(LOAD_COORD_Y==x_y_or_xy)
	{
		int coord=U32toS32(data);
		coordToLoad.y()=coord;
		if(true==rel)
		{
			coordToLoad.y()+=relRef.y();
		}
		else
		{
            coordToLoad.y()+=absRef.y();
		}
		if(true==monitorCtrl)
		{
			std::cout << "Coord Y " << coordToLoad.y() << "\n";
		}
		return 1;
	}
	return 0;
}

void FMT3631::ClearLoadedFlags(void)
{
	state.nLoadedCoord=0;
	for(auto &i : state.metaCoordType)
	{
		i=LOAD_COORD_PRIMTYPE_NOT_LOADED;
	}
}

void FMT3631::DeviceCoord(uint32_t physAddr,uint32_t data)
{
	auto absRef=Vec2i::Make(0,0);
	auto relRef=GetWindowOffset();
	auto idx=(physAddr>>6)&3;
	auto &coordToLoad=state.coord[idx];
	DeviceCoordOrLoadCoord(coordToLoad,absRef,relRef,physAddr,data);
	state.metaCoordType[idx]=LOAD_COORD_PRIMTYPE_NOT_TYPED;

	if(0==idx) // It also updates the pixel width.
	{
		state.pixelLeftUp.x()=coordToLoad.x();
		state.pixelWid=state.coord[2].x()-state.coord[0].x();
	}
	else if(1==idx) // It also updates the current pixel position.
	{
		state.pixelCurrent=coordToLoad;
	}
	else if(2==idx) // It also updates the pixel width.
	{
		state.pixelWid=state.coord[2].x()-state.coord[0].x();
	}

	if(true==monitorCtrl)
	{
		std::cout << "DEVICE_COORD " << idx << "\n";
	}
}

void FMT3631::LoadCoord(uint32_t physAddr,uint32_t data)
{
	auto absRef=GetWindowOffset();
	auto relRef=state.coord[state.lastLoadedCoord];
	auto &coordToLoad=state.coord[state.nextLoadIndex];

	if(state.nextLoadIndex<COORD_MAX)
	{
		state.lastLoadedCoord=state.nextLoadIndex;
		state.metaCoordType[state.nextLoadIndex]=physAddr&LOAD_COORD_PRIMTYPE_MASK;
		if(0!=DeviceCoordOrLoadCoord(coordToLoad,absRef,relRef,physAddr,data))
		{
			++state.nextLoadIndex;
			state.nLoadedCoord=state.nextLoadIndex;
			for(auto i=state.nLoadedCoord; i<COORD_MAX; ++i)
			{
				state.metaCoordType[i]=LOAD_COORD_PRIMTYPE_NOT_LOADED;
			}

			switch(physAddr&LOAD_COORD_PRIMTYPE_MASK)
			{
			case LOAD_COORD_PRIMTYPE_LINE:
			case LOAD_COORD_PRIMTYPE_RECT:
				if(2<=state.nextLoadIndex)
				{
					state.nextLoadIndex=0;
				}
				break;
			case LOAD_COORD_PRIMTYPE_TRI:
				if(3<=state.nextLoadIndex)
				{
					state.nextLoadIndex=0;
				}
				break;
			default:
				if(4<=state.nextLoadIndex)
				{
					state.nextLoadIndex=0;
				}
				break;
			}
		}
	}
}

void FMT3631::DrawPoint(void)
{
	std::cout << "DrawPoint not supported yet.\n";
}
void FMT3631::DrawLine(Vec2i p0,Vec2i p1)
{
	auto bytesPerLine=BytesPerLine();
	auto bitsPerPixel=BitsPerPixel();
	auto bytesPerPixel=BytesPerPixel();

	Vec2i clip[2]=
	{
		GetWindowMin(),
		GetWindowMax(),
	};

	if(true==monitorCtrl)
	{
		std::cout << "Line\n";
	}

	int dx=p1.x()-p0.x();
	int dy=p1.y()-p0.y();
	int vx=1,vy=1;
	int x0=p0.x(),y0=p0.y(),x1=p1.x(),y1=p1.y();

	if((x0<clip[0].x() && x1<clip[0].x()) || 
	   (y0<clip[0].y() && y1<clip[0].y()) ||
	   (clip[1].x()<x0 && clip[1].x()<x1) ||
	   (clip[1].y()<y0 && clip[1].y()<y1))
	{
		return;
	}

	if(dx<0)
	{
		dx=-dx;
		vx=-1;
	}
	if(dy<0)
	{
		dy=-dy;
		vy=-1;
	}

	uint8_t *lineTop=state.vram.data()+bytesPerLine*y0+x0;
	auto fgColor=*GetControlWordPtr(FGCOLOR);
	auto bgColor=*GetControlWordPtr(BGCOLOR);
	uint32_t raster=*GetControlWordPtr(RASTER);
	bool usePattern=(0!=(raster&RASTER_USEPATTERN));
	uint32_t patternBit=0x80000000;

	auto pset=[=](uint8_t *ptr,bool usePattern,uint32_t pattern,uint32_t patternBit)
	{
		switch(raster&0xFFFF)
		{
		default:
			if(true==breakOnUnsupported)
			{
				auto *towns=(FMTownsCommon *)vmPtr;
				towns->debugger.ExternalBreak("Unsupported Raster type for Rect "+cpputil::Itoa(bitsPerPixel)+" bpp ("+cpputil::Uitox(raster)+")");
			}
			break;
		case 0xff00: // Copy
			*ptr=fgColor;
			break;
		case 0x5555: // Not dst
			*ptr=~*ptr;
			break;
		case 0x55aa: // Xor
			*ptr^=fgColor;
			break;
		case 0xFC30: // Pattern=0->BG,  1->FG.  If use_pattern==false, always FG (probably)
			if(true!=usePattern || 0!=(patternBit&pattern))
			{
				*ptr=fgColor;
			}
			else
			{
				*ptr=bgColor;
			}
			break;
		}
	};

	if(0==dx)
	{
		auto *ptr=lineTop;
		int vPtr=bytesPerLine;
		vPtr*=vy;

		unsigned int patternBit=(0x80000000>>(x0%31));

		bool last=false;
		for(auto y=y0; true!=last; y+=vy)
		{
			if(y==y1)
			{
				last=true;
			}

			if(y<clip[0].y() || clip[1].y()<y)
			{
				continue;
			}

			pset(ptr,usePattern,state.pattern[y%PATTERN_LEN],patternBit);
			ptr+=vPtr;
		}
	}
	else if(0==dy)
	{
		auto *ptr=lineTop;
		int vPtr=vx;

		bool last=false;
		for(auto x=x0; true!=last; x+=vx)
		{
			if(x==x1)
			{
				last=true;
			}

			if(x<clip[0].x() || clip[1].x()<x)
			{
				continue;
			}

			uint32_t patternBit=(0x80000000>>(x%31));
			pset(ptr,usePattern,state.pattern[y0%PATTERN_LEN],patternBit);
			ptr+=vPtr;
		}
	}
	else
	{
		auto *ptr=lineTop;

		int balance=0;
		int x=x0,y=y0;
		int vyPtr=bytesPerLine;
		vyPtr*=vy;
		int vxPtr=vx;

		bool last=false;
		while(true!=last)
		{
			if(x==x1 && y==y1)
			{
				last=true;
			}

			uint32_t patternBit=(0x80000000>>(x%31));
			pset(ptr,usePattern,state.pattern[y%PATTERN_LEN],patternBit);

			if(0==balance)
			{
				y+=vy;
				ptr+=vyPtr;
				balance-=dx;

				x+=vx;
				ptr+=vxPtr;
				balance+=dy;
			}
			else if(0<balance)
			{
				y+=vy;
				ptr+=vyPtr;
				balance-=dx;
			}
			else
			{
				x+=vx;
				ptr+=vxPtr;
				balance+=dy;
			}
		}
	}
}

void FMT3631::DrawTri(void)
{
	std::cout << "DrawTri not supported yet.\n";
}

void FMT3631::DrawQuad(void)
{
	std::cout << "DrawQuad not supported yet.\n";
}
void FMT3631::DrawRect(Vec2i p0,Vec2i p1)
{
	auto bytesPerLine=BytesPerLine();
	auto bitsPerPixel=BitsPerPixel();
	auto bytesPerPixel=BytesPerPixel();

	int x0=p0.x();
	int y0=p0.y();
	int x1=p1.x();
	int y1=p1.y();

	if(x1<x0)
	{
		std::swap(x0,x1);
	}
	if(y1<y0)
	{
		std::swap(y0,y1);
	}

	Vec2i clip[2]=
	{
		GetWindowMin(),
		GetWindowMax(),
	};

	if(x1<clip[0].x() || clip[1].x()<x0 ||
	   y1<clip[0].y() || clip[1].y()<y0)
	{
		return;
	}

	if(true==monitorCtrl)
	{
		std::cout << "Rect\n";
	}
	auto fgColor=*GetControlWordPtr(FGCOLOR);
	auto bgColor=*GetControlWordPtr(BGCOLOR);
	uint32_t raster=*GetControlWordPtr(RASTER);

	if(true!=state.isFMT3632)
	{
		// FMT-3631
		uint8_t *lineTop=state.vram.data()+bytesPerLine*y0+x0;
		bool usePattern=(0!=(raster&RASTER_USEPATTERN));
		uint32_t patternBit=0x80000000;
		for(auto y=y0; y<=y1; ++y)
		{
			if(clip[1].y()<y)
			{
				break;
			}

			uint32_t pattern=state.pattern[y%PATTERN_LEN];

			// Shockingly, Power 9000's Quad command fills the bytes within the region with the same byte.
			// If it is 24-bit color mode, it can only draw gray scale.
			// Linux Power 9000 driver draws a quad twice with a pattern and logic ops to fill component by component.
			// Windows driver apparently did not push it that much.
			auto ptr=lineTop;

			if(y<clip[0].y())
			{
				goto NEXTY;
			}

			for(auto x=x0; x<=x1; ++x)
			{
				if(clip[1].x()<x)
				{
					break;
				}
				if(clip[0].x()<=x)
				{
					// Apparently X coordinate is multiplied by bytes-per-pixel by the software.
					switch(raster&0xFFFF)
					{
					// FG    1111111100000000
					// BG    1111000011110000
					// SRC   1100110011001100
					// DST   1010101010101010
					default:
						if(true==breakOnUnsupported)
						{
							auto *towns=(FMTownsCommon *)vmPtr;
							towns->debugger.ExternalBreak("Unsupported Raster type for Rect "+cpputil::Itoa(bitsPerPixel)+" bpp ("+cpputil::Uitox(raster)+")");
						}
						break;
					case 0xF0F0: // Used by Windows 3.1
					    // Same as IGM_B_MASK of Linux P9000 driver, then bgColor?
						*ptr=bgColor;
						break;
					case 0xff00: // Copy
						*ptr=fgColor;
						break;
					case 0x5555: // Not dst
						*ptr=~*ptr;
						break;
					case 0x55aa: // Xor
						*ptr^=fgColor;
						break;
					case 0x569a: // Supposed to be Pixel1 XOR
						// FG    1111111100000000
						// BG    1111000011110000
						// SRC   1100110011001100
						// DST   1010101010101010
						// ----------------------
						// 569A  0101011010011010
						// How come this becomes XOR?  
						*ptr^=fgColor;
						break;
					case 0xFC30: // Pattern=0->BG,  1->FG.  If use_pattern==false, always FG (probably)
						if(true!=usePattern || 0!=(patternBit&pattern))
						{
							*ptr=fgColor;
						}
						else
						{
							*ptr=bgColor;
						}
						break;
					}
				}
				++ptr;
				patternBit>>=1;
				if(0==patternBit)
				{
					patternBit=0x80000000;
				}
			}
		NEXTY:
			lineTop+=bytesPerLine;
		}
	}
	else
	{
		// FMT-3632
		uint8_t *lineTop=state.vram.data()+bytesPerLine*y0+bytesPerPixel*x0;
		bool usePattern=(0!=(raster&RASTER_P9100_PATTERN_ENABLE));
		uint32_t patternBit=0x80000000;
		uint8_t color[2][4];
		cpputil::PutDword(color[0],fgColor);
		cpputil::PutDword(color[1],bgColor);

		if(0==(raster&RASTER_OVERSIZED)) // Well, I guess I need to care.
		{
			if(x0<x1)
			{
				--x1;
			}
			if(y0<y1)
			{
				--y1;
			}
		}

		raster&=~RASTER_OVERSIZED;
		raster&=0xFF;
		for(auto y=y0; y<=y1; ++y)
		{
			if(clip[1].y()<y)
			{
				break;
			}

			uint32_t pattern=state.pattern[y%PATTERN_LEN];

			// Shockingly, Power 9000's Quad command fills the bytes within the region with the same byte.
			// If it is 24-bit color mode, it can only draw gray scale.
			// Linux Power 9000 driver draws a quad twice with a pattern and logic ops to fill component by component.
			// Windows driver apparently did not push it that much.
			auto ptr=lineTop;

			if(clip[0].y()<=y)
			{
				uint32_t colorPtr=0;
				for(auto x=x0; x<=x1; ++x)
				{
					if(clip[1].x()<x)
					{
						break;
					}
					if(clip[0].x()<=x)
					{
						switch(raster)
						{
						default:
							if(true==breakOnUnsupported)
							{
								auto *towns=(FMTownsCommon *)vmPtr;
								towns->debugger.ExternalBreak("Unsupported Raster type for Rect "+cpputil::Itoa(bitsPerPixel)+" bpp ("+cpputil::Uitox(raster)+")");
							}
							break;
						case 0:
							memset(ptr,0,bytesPerPixel);
							break;
						case 0xf0: // Copy
							memcpy(ptr,color[0]+colorPtr,bytesPerPixel);
							break;
						case 0x55: // Probably XOR
							// b7  0   Pattern* Source* Destination
							// b6  1   Pattern* Source*~Destination
							// b5  0   Pattern*~Source* Destination
							// b4  1   Pattern*~Source*~Destination
							// b3  0  ~Pattern* Source* Destination
							// b2  1  ~Pattern* Source*~Destination
							// b1  0  ~Pattern*~Source* Destination
							// b0  1  ~Pattern*~Source*~Destination
							for(int i=0; i<bytesPerPixel; ++i)
							{
								ptr[i]^=color[0][colorPtr+i];
							}
							break;
						case 0x5a: // Probably NOT  Low 4-bits do not make sense though.
							// b7  0   Pattern* Source* Destination
							// b6  1   Pattern* Source*~Destination
							// b5  0   Pattern*~Source* Destination
							// b4  1   Pattern*~Source*~Destination
							// b3  1  ~Pattern* Source* Destination
							// b2  0  ~Pattern* Source*~Destination
							// b1  1  ~Pattern*~Source* Destination
							// b0  0  ~Pattern*~Source*~Destination
							for(int i=0; i<bytesPerPixel; ++i)
							{
								ptr[i]=~ptr[i];
							}
							break;
						}
					}
					if(bytesPerPixel<3)
					{
						colorPtr=(colorPtr+bytesPerPixel)&3;
					}
					ptr+=bytesPerPixel;
					patternBit>>=1;
					if(0==patternBit)
					{
						patternBit=0x80000000;
					}
				}
			}
			lineTop+=bytesPerLine;
		}
	}
}

void FMT3631::CmdNextPixels(uint32_t physAddr,uint32_t data)
{
	state.pixelLeftUp=state.coord[2];
	state.pixelCurrent=state.coord[2];
	state.pixelWid=data;

	if(true==monitorCtrl)
	{
		std::cout << "Next Pixels " << state.pixelLeftUp.x() << " " << state.pixelLeftUp.y() << " " << state.pixelWid << "\n";
	}
}

class Pixel1CopyTransparentP9000
{
public:
	static inline void DoLogicOp(uint8_t &dst,bool fg,uint32_t fgColor,uint32_t bgColor)
	{
		if(true==fg)
		{
			dst=fgColor;
		}
	}
};
class Pixel1CopyOpaqueP9000
{
public:
	static inline void DoLogicOp(uint8_t &dst,bool fg,uint32_t fgColor,uint32_t bgColor)
	{
		if(true==fg)
		{
			dst=fgColor;
		}
		else
		{
			dst=bgColor;
		}
	}
};

class Pixel1SrcANDDstOpaqueP9000
{
public:
	static inline void DoLogicOp(uint8_t &dst,bool fg,uint32_t fgColor,uint32_t bgColor)
	{
		if(true==fg)
		{
			dst&=fgColor;
		}
		else
		{
			dst&=bgColor;
		}
	}
};

class Pixel1NotSrcOrDstP9000
{
public:
	static inline void DoLogicOp(uint8_t &dst,bool fg,uint32_t fgColor,uint32_t bgColor)
	{
		if(true==fg)
		{
			dst=((~fgColor)|dst);
		}
		else
		{
			dst=((~bgColor)|dst);
		}
	}
};

template <class LogicOp>
void FMT3631::CmdPixel1LoopP9000(uint32_t physAddr,uint32_t data,bool byteSwap,bool bitSwap)
{
	uint32_t count=1+((physAddr>>2)&31);

	if(true==bitSwap && true==byteSwap)
	{
		auto dataRev=data;
		data=0;
		uint32_t tstBit=0x80000000,orBit=1;
		while(0!=tstBit)
		{
			if(dataRev&tstBit)
			{
				data|=orBit;
			}
			tstBit>>=1;
			orBit<<=1;
		}
	}
	else if(true==byteSwap)
	{
		data=ByteSwap32(data);
	}

	state.pixelYIncrement=state.coord[3].y();

	auto fgColor=*GetControlWordPtr(FGCOLOR);
	auto bgColor=*GetControlWordPtr(BGCOLOR);
	auto bytesPerLine=BytesPerLine();

	auto winMin=GetWindowMin();
	auto winMax=GetWindowMax();

	uint8_t *lineTop=state.vram.data()+bytesPerLine*state.pixelCurrent.y();
	while(0<count)
	{
		bool fg=data&0x80000000;
		data<<=1;

		if(state.pixelCurrent.IsInsideWindow(winMin,winMax))
		{
			LogicOp::DoLogicOp(lineTop[state.pixelCurrent.x()],fg,fgColor,bgColor);
		}

		++state.pixelCurrent.x();
		if(state.pixelLeftUp.x()+state.pixelWid<=state.pixelCurrent.x())
		{
			state.pixelCurrent.x()=state.pixelLeftUp.x();
			state.pixelCurrent.y()+=state.pixelYIncrement;
			if(0<state.pixelYIncrement)
			{
				lineTop+=bytesPerLine;
			}
			else
			{
				lineTop-=bytesPerLine;
			}
		}

		--count;
	}
}

class Pixel1CopyTransparentP9100BG
{
public:
	static inline void DoLogicOp(uint8_t *pixelPtr,int bytesPerPixel,bool fg,uint8_t color[2][4],unsigned int colorPtr);
};
void Pixel1CopyTransparentP9100BG::DoLogicOp(uint8_t *pixelPtr,int bytesPerPixel,bool fg,uint8_t color[2][4],unsigned int colorPtr)
{
	if(true==fg)
	{
		memcpy(pixelPtr,color[1]+colorPtr,bytesPerPixel);
	}
}

class Pixel1CopyOpaqueP9100BG
{
public:
	static inline void DoLogicOp(uint8_t *pixelPtr,int bytesPerPixel,bool fg,uint8_t color[2][4],unsigned int colorPtr);
};
void Pixel1CopyOpaqueP9100BG::DoLogicOp(uint8_t *pixelPtr,int bytesPerPixel,bool fg,uint8_t color[2][4],unsigned int colorPtr)
{
	if(true==fg)
	{
		memcpy(pixelPtr,color[1]+colorPtr,bytesPerPixel);
	}
	else
	{
		memcpy(pixelPtr,color[0]+colorPtr,bytesPerPixel);
	}
}

template <class LogicOp>
void FMT3631::CmdPixel1LoopP9100(uint32_t physAddr,uint32_t data,bool byteSwap,bool bitSwap)
{
	uint32_t count=1+((physAddr>>2)&31);

	if(true==bitSwap && true==byteSwap)
	{
		auto dataRev=data;
		data=0;
		uint32_t tstBit=0x80000000,orBit=1;
		while(0!=tstBit)
		{
			if(dataRev&tstBit)
			{
				data|=orBit;
			}
			tstBit>>=1;
			orBit<<=1;
		}
	}
	else if(true==byteSwap)
	{
		data=ByteSwap32(data);
	}

	state.pixelYIncrement=state.coord[3].y();

	auto fgColor=*GetControlWordPtr(FGCOLOR);
	auto bgColor=*GetControlWordPtr(BGCOLOR);
	auto bytesPerPixel=BytesPerPixel();
	auto bytesPerLine=BytesPerLine();

	uint8_t color[2][4];
	cpputil::PutDword(color[0],fgColor);
	cpputil::PutDword(color[1],bgColor);

	auto winMin=GetWindowMin();
	auto winMax=GetWindowMax();

	uint8_t *lineTop=state.vram.data()+bytesPerLine*state.pixelCurrent.y();
	int colorPtr=0;

	while(0<count)
	{
		bool fg=data&0x80000000;
		data<<=1;

		if(state.pixelCurrent.IsInsideWindow(winMin,winMax))
		{
			uint8_t *destPixel=lineTop+state.pixelCurrent.x()*bytesPerPixel;
			LogicOp::DoLogicOp(destPixel,bytesPerPixel,fg,color,colorPtr);
		}

		++state.pixelCurrent.x();
		if(bytesPerPixel<3)
		{
			colorPtr=(colorPtr+bytesPerPixel)&3;
		}
		if(state.pixelLeftUp.x()+state.pixelWid<=state.pixelCurrent.x())
		{
			state.pixelCurrent.x()=state.pixelLeftUp.x();
			state.pixelCurrent.y()+=state.pixelYIncrement;
			if(0<state.pixelYIncrement)
			{
				lineTop+=bytesPerLine;
			}
			else
			{
				lineTop-=bytesPerLine;
			}
		}

		--count;
	}
}

void FMT3631::CmdPixel1(uint32_t physAddr,uint32_t data,bool byteSwap,bool bitSwap)
{
	if(true==monitorCtrl)
	{
		std::cout << "Pixel1 " << state.pixelCurrent.x() << " " << state.pixelCurrent.y() << " " << state.pixelYIncrement << "\n";
	}

	uint16_t raster=*GetControlWordPtr(RASTER);
	raster&=~RASTER_OVERSIZED; // Doesn't matter for Pixel1
	if(true!=state.isFMT3632)
	{
		switch(raster)
		{
		default:
			if(true==breakOnUnsupported)
			{
				auto *towns=(FMTownsCommon *)vmPtr;
				towns->debugger.ExternalBreak("Unsupported Raster type for Pixel1 ("+cpputil::Uitox(raster)+")");
			}
			std::cout << "Unsupported Raster type for Pixel1 ("+cpputil::Uitox(raster)+")\n";
			CmdPixel1LoopP9000<Pixel1CopyTransparentP9000>(physAddr,data,byteSwap,bitSwap);
			break;

		case 0xa820: // Used by Windows 3.1   Supposed to be SRC and DST Opaque.
			CmdPixel1LoopP9000<Pixel1SrcANDDstOpaqueP9000>(physAddr,data,byteSwap,bitSwap);
			break;

		case 0xABEF: // Used by Windows 3.1   0xABEF=~0x5410=~(src AND NOT dst)=NOT src OR dst
			CmdPixel1LoopP9000<Pixel1NotSrcOrDstP9000>(physAddr,data,byteSwap,bitSwap);
			break;

		case 0xee22:
			CmdPixel1LoopP9000<Pixel1CopyTransparentP9000>(physAddr,data,byteSwap,bitSwap);
			break;

		case 0xfc30:
			CmdPixel1LoopP9000<Pixel1CopyOpaqueP9000>(physAddr,data,byteSwap,bitSwap);
			break;
		}
	}
	else
	{
		switch(raster)
		{
		default:
			if(true==breakOnUnsupported)
			{
				auto *towns=(FMTownsCommon *)vmPtr;
				towns->debugger.ExternalBreak("Unsupported Raster type for Pixel1 ("+cpputil::Uitox(raster)+")");
			}
			std::cout << "Unsupported Raster type for Pixel1 ("+cpputil::Uitox(raster)+")";
			CmdPixel1LoopP9100<Pixel1CopyTransparentP9100BG>(physAddr,data,byteSwap,bitSwap);
			break;

		case 0xCC:
			// Transparent flag is off, but 0xCC is used for drawing letters on the title bar.
			// Also looks to be using the bgColor instead of fgCOlor.
			CmdPixel1LoopP9100<Pixel1CopyOpaqueP9100BG>(physAddr,data,byteSwap,bitSwap);
			break;
		case 0x80CC: // Transparent flag.
			CmdPixel1LoopP9100<Pixel1CopyTransparentP9100BG>(physAddr,data,byteSwap,bitSwap);
			break;
		}
	}
}

void FMT3631::CmdPixel8(uint32_t physAddr,uint32_t data,bool byteSwap,bool bitSwap)
{
	if(true==breakOnUnsupported)
	{
		auto *towns=(FMTownsCommon *)vmPtr;
		towns->debugger.ExternalBreak("Pixel8 not supported yet.\n");
	}
}


uint32_t FMT3631::CmdQuad(uint32_t physAddr) // Apparently, it is executed by Fetch.
{
	if((state.coord[1].x()==state.coord[0].x() &&
	    state.coord[1].y()==state.coord[2].y() &&
	    state.coord[3].x()==state.coord[2].x() &&
	    state.coord[3].y()==state.coord[0].y()) ||
	   (state.coord[1].y()==state.coord[0].y() &&
	    state.coord[1].x()==state.coord[2].x() &&
	    state.coord[3].y()==state.coord[2].y() &&
	    state.coord[3].x()==state.coord[0].x()))
	{
		DrawRect(state.coord[0],state.coord[2]);
		state.nextLoadIndex=0;
		return 0;
	}

	if(2==state.nLoadedCoord &&
	   LOAD_COORD_PRIMTYPE_LINE==state.metaCoordType[0] &&
	   LOAD_COORD_PRIMTYPE_LINE==state.metaCoordType[1])
	{
		// Is this command for line as well?  Linux Power 9000 driver seems to be assuming so.
		DrawLine(state.coord[0],state.coord[1]);
		state.nextLoadIndex=0;
		return 0;
	}

	if(2==state.nLoadedCoord &&
	   LOAD_COORD_PRIMTYPE_RECT==state.metaCoordType[0] &&
	   LOAD_COORD_PRIMTYPE_RECT==state.metaCoordType[1])
	{
		DrawRect(state.coord[0],state.coord[1]);
		state.nextLoadIndex=0;
		return 0;
	}

	std::cout << "General quad not implemented yet.  " << state.nLoadedCoord << " loaded coords\n";
	std::cout << "(" << state.coord[0].x() << "," << state.coord[0].y() << ") " << cpputil::Ustox(state.metaCoordType[0]) << "\n";
	std::cout << "(" << state.coord[1].x() << "," << state.coord[1].y() << ") " << cpputil::Ustox(state.metaCoordType[1]) << "\n";
	std::cout << "(" << state.coord[2].x() << "," << state.coord[2].y() << ") " << cpputil::Ustox(state.metaCoordType[2]) << "\n";
	std::cout << "(" << state.coord[3].x() << "," << state.coord[3].y() << ") " << cpputil::Ustox(state.metaCoordType[3]) << "\n";
	if(true==breakOnUnsupported)
	{
		auto *towns=(FMTownsCommon *)vmPtr;
		towns->debugger.ExternalBreak("General quad not implemented yet.\n");
	}
	state.nextLoadIndex=0;
	return 0;
}

uint32_t FMT3631::CmdBlit(uint32_t physAddr)
{
	auto srcP0=state.coord[0];
	auto srcP1=state.coord[1];

	auto dstP0=state.coord[2];
	auto dstP1=state.coord[3];

	if(srcP0.x()>srcP1.x())
	{
		std::swap(srcP0.x(),srcP1.x());
	}
	if(srcP0.y()>srcP1.y())
	{
		std::swap(srcP0.y(),srcP1.y());
	}
	if(dstP0.x()>dstP1.x())
	{
		std::swap(dstP0.x(),dstP1.x());
	}
	if(dstP0.y()>dstP1.y())
	{
		std::swap(dstP0.y(),dstP1.y());
	}

	if(true==monitorCtrl)
	{
		std::cout << "Blit\n";
	}

	if(srcP1.x()-srcP0.x()!=dstP1.x()-dstP0.x() ||
	   srcP1.y()-srcP0.y()!=dstP1.y()-dstP0.y())
	{
		return 0;  // Apparently need to indicate blit_software.
	}


	if(dstP0.y()<srcP0.y())
	{
		auto srcPtr=state.vram.data()+srcP0.y()*BytesPerLine()+srcP0.x();
		auto dstPtr=state.vram.data()+dstP0.y()*BytesPerLine()+dstP0.x();
		auto wid=(1+srcP1.x()-srcP0.x());
		for(int y=srcP0.y(); y<=srcP1.y(); ++y)
		{
			memcpy(dstPtr,srcPtr,wid);
			srcPtr+=BytesPerLine();
			dstPtr+=BytesPerLine();
		}
	}
	else if(dstP0.y()>srcP0.y())
	{
		auto srcPtr=state.vram.data()+srcP1.y()*BytesPerLine()+srcP0.x();
		auto dstPtr=state.vram.data()+dstP1.y()*BytesPerLine()+dstP0.x();
		auto wid=(1+srcP1.x()-srcP0.x());
		for(int y=srcP1.y(); y>=srcP0.y(); --y)
		{
			memcpy(dstPtr,srcPtr,wid);
			srcPtr-=BytesPerLine();
			dstPtr-=BytesPerLine();
		}
	}
	else if(dstP0.x()<srcP0.x())
	{
		auto srcPtr=state.vram.data()+srcP0.y()*BytesPerLine()+srcP0.x();
		auto dstPtr=state.vram.data()+dstP0.y()*BytesPerLine()+dstP0.x();
		auto wid=(1+srcP1.x()-srcP0.x());
		for(int y=srcP0.y(); y<=srcP1.y(); ++y)
		{
			memcpy(dstPtr,srcPtr,wid);
			srcPtr+=BytesPerLine();
			dstPtr+=BytesPerLine();
		}
	}
	else
	{
		auto srcPtr=state.vram.data()+srcP0.y()*BytesPerLine()+srcP0.x();
		auto dstPtr=state.vram.data()+dstP0.y()*BytesPerLine()+dstP0.x();
		auto wid=(1+srcP1.x()-srcP0.x());
		for(int y=srcP0.y(); y<=srcP1.y(); ++y)
		{
			for(int x=wid-1; 0<=x; --x)
			{
				dstPtr[x]=srcPtr[x];
			}
			srcPtr+=BytesPerLine();
			dstPtr+=BytesPerLine();
		}
	}

	state.nextLoadIndex=0;
	return 0;
}

bool FMT3631::IsCommand(uint32_t physAddr,uint32_t data)
{
	const auto masked=(COMMAND_MASK&physAddr);

	if(true!=state.isFMT3632)
	{
		if(masked==BT_COMMAND_REG_1)
		{
			state.btCommandReg[1]=data;
			if(0==(data&(BT_CR1_BP16|BT_CR1_BP8)))
			{
				if(true==monitorCtrl)
				{
					std::cout << "32 bits per pixel.\n";
				}
				state.bitsPerPixel=32;
			}
			else if(0!=(data&BT_CR1_BP8))
			{
				if(true==monitorCtrl)
				{
					std::cout << "8 bits per pixel.\n";
				}
				state.bitsPerPixel=8;
			}
			else if(0!=(data&BT_CR1_BP16))
			{
				if(true==monitorCtrl)
				{
					std::cout << "16 bits per pixel.\n";
				}
				state.bitsPerPixel=16;
				state.highColor565=(0!=(data&BT_CR1_565RGB));
			}
		}
		else if(masked==BT_COMMAND_REG_2)
		{
			state.btCommandReg[2]=data;
			state.hwCursor.defined=(0!=(data&BT_CR2_CURSOR_ENABLE));
			state.hwCursor.twoColorCursor=(0!=(data&BT_CR2_2COLOR_CURSOR));
		}
		else if(masked==BT_COMMAND_REG_3)
		{
			state.btCommandReg[3]=data;
			if(0!=(BT_CR3_64SQ_CURSOR&data))
			{
				state.hwCursor.wid=64;
				state.hwCursor.originX=64;
				state.hwCursor.originY=64;
			}
			else
			{
				state.hwCursor.wid=32;
				state.hwCursor.originX=32;
				state.hwCursor.originY=32;
			}
		}
		else if(masked==BT_WRITE_ADDR)
		{
			if(BT_CURS_OR_PTN==data)
			{
				state.hwCursor.ptnCount=0;
			}
			if(BT_CURS_AND_PTN==data)
			{
				state.hwCursor.ptnCount=512;
			}
			state.writingPalette=data&255;
			state.writingPaletteRGBCount=0;
			return true;
		}
		else if(masked==BT_CURS_RAM_DATA)
		{
			if(state.hwCursor.ptnCount<512)
			{
				state.hwCursor.ORPtn[state.hwCursor.ptnCount]=data;
			}
			else if(state.hwCursor.ptnCount<1024)
			{
				state.hwCursor.ANDPtn[state.hwCursor.ptnCount-512]=data;
			}
			++state.hwCursor.ptnCount;
			state.hwCursor.ptnCount&=0x3FF;
			return true;
		}
		else if(masked==BT_CURS_WR_ADDR) //  0x000090
		{
			if(1==data)
			{
				state.hwCursorTwoColorReadPos=0;
			}
			return true;
		}
		else if(masked==BT_CURS_DATA) //    0x000094
		{
			if(state.hwCursorTwoColorReadPos<6)
			{
				state.hwCursor.twoColor[state.hwCursorTwoColorReadPos]=data;
				++state.hwCursorTwoColorReadPos;
			}
			return true;
		}
		else if(masked==BT_RAMDAC_DATA)
		{
			if(state.writingPaletteRGBCount<3)
			{
				state.writingPaletteRGB[state.writingPaletteRGBCount]=data;
				++state.writingPaletteRGBCount;
				if(3==state.writingPaletteRGBCount)
				{
					state.plt.plt256[state.writingPalette].Set(
					    state.writingPaletteRGB[0],
					    state.writingPaletteRGB[1],
					    state.writingPaletteRGB[2],
					    255);
				}
			}
		}
		else if(masked==BT_CURS_X_LOW)
		{
			state.hwCursorXY_LowByte[0]=data;
			return true;
		}
		else if(masked==BT_CURS_Y_LOW)
		{
			state.hwCursorXY_LowByte[1]=data;
			return true;
		}
		else if(masked==BT_CURS_X_HIGH)
		{
			state.hwCursor.X=(state.hwCursorXY_LowByte[0]&255)|(data<<8);
			return true;
		}
		else if(masked==BT_CURS_Y_HIGH)
		{
			state.hwCursor.Y=(state.hwCursorXY_LowByte[1]&255)|(data<<8);
			return true;
		}
	}
	else
	{
		if(masked==P9100_CURSOR_REGSEL) // 210
		{
			state.p9100CursorRegSel=(data>>16)&0xFF;
			return true;
		}
		else if(masked==P9100_CURSOR_DATA) // 218
		{
			if(P9100_CURSOR_REG_ON_OFF==state.p9100CursorRegSel)
			{
				state.hwCursor.defined=(0!=(data&0x00020000));
				state.hwCursor.twoColorCursor=0;
			}
			else if(P9100_CURSOR_REG_POSITION==state.p9100CursorRegSel)
			{
				switch(state.p9100CursorDataCount)
				{
				case 0:
					state.hwCursorXY_LowByte[0]=(data>>16)&0xFF;
					break;
				case 1:
					state.hwCursor.X=((data>>8)&0xFF00)|state.hwCursorXY_LowByte[0];
					break;
				case 2:
					state.hwCursorXY_LowByte[1]=(data>>16)&0xFF;
					break;
				case 3:
					state.hwCursor.Y=((data>>8)&0xFF00)|state.hwCursorXY_LowByte[1];
					break;
				}
				++state.p9100CursorDataCount;
			}
			else if(P9100_CURSOR_REG_PATTERN==state.p9100CursorRegSel)
			{
				auto ptn=((data>>16)&0xFF);
				if(state.hwCursor.ptnCount<512)
				{
					state.hwCursor.ORPtn[state.hwCursor.ptnCount]=ptn;
				}
				else if(state.hwCursor.ptnCount<1024)
				{
					state.hwCursor.ANDPtn[state.hwCursor.ptnCount-512]=ptn;
				}
				++state.hwCursor.ptnCount;
				state.hwCursor.ptnCount&=0x3FF;
			}
			return true;
		}
		else if(masked==P9100_CURSOR_IS_ARRAYDATA) // 21C
		{
			if(0!=(data&0x010000)) // Next data is an array data.
			{
			}
			else // Array data done.
			{
				if(P9100_CURSOR_REG_PATTERN==state.p9100CursorRegSel)
				{
					// Unpack pattern.
					uint8_t packed[sizeof(state.hwCursor.ORPtn)+sizeof(state.hwCursor.ANDPtn)];
					memcpy(packed,state.hwCursor.ORPtn,sizeof(state.hwCursor.ORPtn));
					memcpy(packed+sizeof(state.hwCursor.ORPtn),state.hwCursor.ANDPtn,sizeof(state.hwCursor.ANDPtn));
					for(int i=0; i<sizeof(packed); i+=2)
					{
						uint8_t ANDPtn=0,ORPtn=0;
						uint16_t pk=(packed[i]<<8)|packed[i+1];
						for(int j=0; j<8; ++j)
						{
							ANDPtn<<=1;
							ORPtn<<=1;
							if(pk&0x8000)
							{
								ANDPtn|=1;
							}
							if(pk&0x4000)
							{
								ORPtn|=1;
							}
							pk<<=2;
						}
						state.hwCursor.ORPtn[i/2]=ORPtn;
						state.hwCursor.ANDPtn[i/2]=ANDPtn;
					}
				}
			}
			state.p9100CursorDataCount=0; // Array or not, reset the data counter.
			state.hwCursor.ptnCount=0;  // Also reset the pattern count.
		}
		else if(P9100_PALETTE_COLOR_ADDR==masked) // 0x00200
		{
			state.writingPalette=(data>>16)&255;
			state.writingPaletteRGBCount=0;
		}
		else if(P9100_PALETTE_COLOR_DATA==masked) // 0x00204
		{
			if(state.writingPaletteRGBCount<3)
			{
				state.writingPaletteRGB[state.writingPaletteRGBCount]=(data>>16)&255;
				++state.writingPaletteRGBCount;
				if(3==state.writingPaletteRGBCount)
				{
					state.plt.plt256[state.writingPalette].Set(
					    state.writingPaletteRGB[0],
					    state.writingPaletteRGB[1],
					    state.writingPaletteRGB[2],
					    255);
				}
			}
		}
	}

	if((0xFFE07&physAddr)==LOAD_COORD)
	{
		LoadCoord(physAddr,data);
		return true;
	}
	else if((0xFFF07&physAddr)==DEVICE_COORD)
	{
		DeviceCoord(physAddr,data);
		return true;
	}
	else if((0xFFFFF&physAddr)==NEXT_PIXELS_CMD)
	{
		CmdNextPixels(physAddr,data);
		return true;
	}
	else if((0xFFF80&physAddr)==PIXEL1_CMD)
	{
		CmdPixel1(physAddr,data,false,false);
		return true;
	}
	else if((0xFFF80&physAddr)==PIXEL1_BYTE_SWAP_CMD)
	{
		CmdPixel1(physAddr,data,true,false);
		return true;
	}
	else if((0xFFF80&physAddr)==PIXEL1_BIT_REVERSE_CMD)
	{
		CmdPixel1(physAddr,data,true,true);
		return true;
	}
	else if(masked==PIXEL8_CMD)
	{
		CmdPixel8(physAddr,data,false,false);
	}
	else if(masked==PIXEL8_BYTE_SWAP_CMD)
	{
		CmdPixel8(physAddr,data,true,true);
	}
	else if(masked==PIXEL8_BIT_REVERSE_CMD)
	{
		CmdPixel8(physAddr,data,true,false);
	}
	else if(masked==QUAD_CMD)
	{
		CmdQuad(physAddr);
		return true;
	}
	else if(masked==BLIT_CMD)
	{
		CmdBlit(physAddr);
		return true;
	}
	return false;
}

void FMT3631::SysConfigToBpp3632(void)
{
	auto bpp=(state.sysconfig>>26)&7;
	if(2==bpp)
	{
		state.bitsPerPixel=8;
	}
	else if(3==bpp)
	{
		state.bitsPerPixel=16;
		state.highColor565=true;
	}
	else if(7==bpp)
	{
		state.bitsPerPixel=24;
	}
	else if(5==bpp)
	{
		state.bitsPerPixel=32;
	}
}

void FMT3631::SetControlByte(uint32_t physAddr,uint8_t data)
{
	if(true==IsCommand(physAddr,data))
	{
		return;
	}
	auto ptr=GetControlWordPtr(physAddr);
	if(nullptr!=ptr)
	{
		*ptr&=0xFFFFFF00;
		*ptr|=data;
	}
}

void FMT3631::SetControlWord(uint32_t physAddr,uint16_t data)
{
	if(true==IsCommand(physAddr,data))
	{
		return;
	}
	auto ptr=GetControlWordPtr(physAddr);
	if(nullptr!=ptr)
	{
		*ptr&=0xFFFF0000;
		*ptr|=data;
		if(true==state.isFMT3632)
		{
			SysConfigToBpp3632();
		}
	}
}

void FMT3631::SetControlDword(uint32_t physAddr,uint32_t data)
{
	if(true==IsCommand(physAddr,data))
	{
		return;
	}

	auto ptr=GetControlWordPtr(physAddr);
	if(nullptr!=ptr)
	{
		*ptr=data;
		if(true==state.isFMT3632)
		{
			SysConfigToBpp3632();
		}
	}
}


unsigned int FMT3631::FetchByte(unsigned int physAddr) const
{
	unsigned int data=0xFF;
	bool monitor=false;
	if(true==state.enabled)
	{
		if(state.VRAMBaseAddr<=physAddr && physAddr<state.VRAMEndAddr)
		{
			monitor=monitorVRAM;
			data=state.vram[physAddr-state.VRAMBaseAddr];
		}
		else
		{
			auto addr=Translate3631to3632(physAddr);

			monitor=monitorCtrl;
			if((COMMAND_MASK&addr)==QUAD_CMD)
			{
				data=mutableThis->CmdQuad(addr);
			}
			else if((COMMAND_MASK&addr)==BLIT_CMD)
			{
				data=mutableThis->CmdBlit(addr);
			}
			else if(true==IsReadableParameter(data,addr))
			{
			}
			else
			{
				uint32_t hanging=addr&3; // FMT-3632 apparently allow access to any bytes of the register.
				addr&=~3;
				auto *ptr=GetControlWordPtr(addr);
				if(nullptr!=ptr)
				{
					data>>=(hanging*8);
					data=*ptr;
				}
			}
		}
	}
	if(true==monitor)
	{
		std::cout << "Power9x00 BYTE read   " << cpputil::Uitox(physAddr) << " " << cpputil::Ubtox(data) << "\n";
	}

	return data;
}

unsigned int FMT3631::FetchWord(unsigned int physAddr) const
{
	unsigned int data=0xFFFF;
	bool monitor=false;
	if(true==state.enabled)
	{
		if(state.VRAMBaseAddr<=physAddr && physAddr+1<state.VRAMEndAddr)
		{
			monitor=monitorVRAM;
			data=cpputil::GetWord(state.vram.data()+physAddr-state.VRAMBaseAddr);
		}
		else
		{
			auto addr=Translate3631to3632(physAddr);

			monitor=monitorCtrl;
			if((COMMAND_MASK&addr)==QUAD_CMD)
			{
				data=mutableThis->CmdQuad(addr);
			}
			else if((COMMAND_MASK&addr)==BLIT_CMD)
			{
				data=mutableThis->CmdBlit(addr);
			}
			else if(true==IsReadableParameter(data,addr))
			{
			}
			else
			{
				uint32_t hanging=addr&3; // FMT-3632 apparently allow access to any bytes of the register.
				addr&=~3;
				auto *ptr=GetControlWordPtr(addr);
				if(nullptr!=ptr)
				{
					data>>=(hanging*8);
					data=*ptr;
				}
			}
		}
	}
	if(true==monitor)
	{
		std::cout << "Power9x00 WORD read   " << cpputil::Uitox(physAddr) << " " << cpputil::Ustox(data) << "\n";
	}
	return data;
}

unsigned int FMT3631::FetchDword(unsigned int physAddr) const
{
	unsigned int data=0xFFFFFFFF;
	bool monitor=false;
	if(true==state.enabled)
	{
		if(state.VRAMBaseAddr<=physAddr && physAddr+3<state.VRAMEndAddr)
		{
			monitor=monitorVRAM;
			data=cpputil::GetDword(state.vram.data()+physAddr-state.VRAMBaseAddr);
		}
		else
		{
			auto addr=Translate3631to3632(physAddr);

			monitor=monitorCtrl;
			if((COMMAND_MASK&addr)==QUAD_CMD)
			{
				data=mutableThis->CmdQuad(addr);
			}
			else if((COMMAND_MASK&addr)==BLIT_CMD)
			{
				data=mutableThis->CmdBlit(addr);
			}
			else if(true==IsReadableParameter(data,addr))
			{
			}
			else
			{
				uint32_t hanging=addr&3; // FMT-3632 apparently allow access to any bytes of the register.
				addr&=~3;
				auto *ptr=GetControlWordPtr(addr);
				if(nullptr!=ptr)
				{
					data>>=(hanging*8);
					data=*ptr;
				}
			}
		}
	}
	if(true==monitor)
	{
		std::cout << "Power9x00 DWORD read  " << cpputil::Uitox(physAddr) << " " << cpputil::Uitox(data) << "\n";
	}
	return data;
}

void FMT3631::StoreByte(unsigned int physAddr,unsigned char data)
{
	bool monitor=false;
	if(true==state.enabled)
	{
		if(state.VRAMBaseAddr<=physAddr && physAddr<state.VRAMEndAddr)
		{
			state.vram[physAddr-state.VRAMBaseAddr]=data;
			monitor=monitorVRAM;
		}
		else
		{
			auto addr=Translate3631to3632(physAddr);
			SetControlByte(addr,data);
			monitor=monitorCtrl;
		}
	}
	if(true==monitor)
	{
		std::cout << "Power9x00 BYTE Write   " << cpputil::Uitox(physAddr) << " " <<  cpputil::Ubtox(data) << "\n";
	}
}

void FMT3631::StoreWord(unsigned int physAddr,unsigned int data)
{
	bool monitor=false;
	if(true==state.enabled)
	{
		if(state.VRAMBaseAddr<=physAddr && physAddr+1<state.VRAMEndAddr)
		{
			cpputil::PutWord(state.vram.data()+physAddr-state.VRAMBaseAddr,data);
			monitor=monitorVRAM;
		}
		else
		{
			auto addr=Translate3631to3632(physAddr);
			SetControlWord(addr,data);
			monitor=monitorCtrl;
		}
	}
	if(true==monitor)
	{
		std::cout << "Power9x00 WORD Write   " << cpputil::Uitox(physAddr) << " " <<  cpputil::Ustox(data) << "\n";
	}
}

void FMT3631::StoreDword(unsigned int physAddr,unsigned int data)
{
	bool monitor=false;
	if(true==state.enabled)
	{
		if(state.VRAMBaseAddr<=physAddr && physAddr+3<state.VRAMEndAddr)
		{
			cpputil::PutDword(state.vram.data()+physAddr-state.VRAMBaseAddr,data);
			monitor=monitorVRAM;
		}
		else
		{
			auto addr=Translate3631to3632(physAddr);
			SetControlDword(addr,data);
			monitor=monitorCtrl;
		}
	}
	if(true==monitor)
	{
		std::cout << "Power9x00 DWORD Write  " << cpputil::Uitox(physAddr) << " " << cpputil::Uitox(data) <<  "\n";
	}
}

std::vector <std::string> FMT3631::GetStatusText(void) const
{
	std::vector <std::string> text;

	if(true==state.enabled)
	{
		text.push_back(true==state.isFMT3632 ? "FMT-3632" : "FMT-3631");
		text.back()+=(" is enabled and ");
		text.back()+=std::string(true==IsEnabled() ? "active" : "inactive");

		text.push_back(cpputil::Uitoa(state.bitsPerPixel)+" bits per pixel");
		if(16==state.bitsPerPixel)
		{
			text.back()+=("  "+std::string(true==state.highColor565 ? "565" : "555"));
		}
		text.push_back("META-Coord Loaded: "+cpputil::Uitoa(state.nLoadedCoord));
		for(int i=0; i<COORD_MAX; ++i)
		{
			auto c=state.coord[i];
			text.push_back("Coord["+cpputil::Uitoa(i)+"]=("+cpputil::Uitoa(c.x())+","+cpputil::Uitoa(c.y())+")");
		}
		for(int i=0; i<4; ++i)
		{
			text.push_back("BtCmdReg["+cpputil::Uitoa(i)+"]=0x"+cpputil::Uitox(state.btCommandReg[i]));
		}
		text.push_back("PixelOpLUP=("
		               +cpputil::Uitoa(state.pixelLeftUp.x())
		               +","
		               +cpputil::Uitoa(state.pixelLeftUp.y())
		               +")"
		               +" PixelOpWidth="
		               +cpputil::Uitoa(state.pixelWid));

		text.push_back("SYSCONFIG        =0x"+cpputil::Uitox(state.sysconfig));
		text.push_back("INTERRUPT        =0x"+cpputil::Uitox(state.interrupt));
		text.push_back("INTERRUPT_EN     =0x"+cpputil::Uitox(state.interrupt_en));
		text.push_back("STATUS           =0x"+cpputil::Uitox(state.status));

		text.push_back("OOR              =0x"+cpputil::Uitox(*GetControlWordPtr(OOR)));
		text.push_back("CINDEX           =0x"+cpputil::Uitox(*GetControlWordPtr(CINDEX)));
		text.push_back("WINDOW_OFFSET_XY =0x"+cpputil::Uitox(*GetControlWordPtr(WINDOW_OFFSET_XY)));
		text.push_back("P_W_MIN          =0x"+cpputil::Uitox(*GetControlWordPtr(P_W_MIN)));
		text.push_back("P_W_MAX          =0x"+cpputil::Uitox(*GetControlWordPtr(P_W_MAX)));
		text.push_back("YCLIP            =0x"+cpputil::Uitox(*GetControlWordPtr(YCLIP)));
		text.push_back("XCLIP            =0x"+cpputil::Uitox(*GetControlWordPtr(XCLIP)));
		text.push_back("XEDGE_LT         =0x"+cpputil::Uitox(*GetControlWordPtr(XEDGE_LT)));
		text.push_back("XEDGE_GT         =0x"+cpputil::Uitox(*GetControlWordPtr(XEDGE_GT)));
		text.push_back("YEDGE_LT         =0x"+cpputil::Uitox(*GetControlWordPtr(YEDGE_LT)));
		text.push_back("YEDGE_GT         =0x"+cpputil::Uitox(*GetControlWordPtr(YEDGE_GT)));

		text.push_back("FGCOLOR          =0x"+cpputil::Uitox(*GetControlWordPtr(FGCOLOR)));
		text.push_back("BGCOLOR          =0x"+cpputil::Uitox(*GetControlWordPtr(BGCOLOR)));
		text.push_back("PLANE_MASK       =0x"+cpputil::Uitox(*GetControlWordPtr(PLANE_MASK)));
		text.push_back("DRAWING_MODE     =0x"+cpputil::Uitox(*GetControlWordPtr(DRAWING_MODE)));
		text.push_back("PATTERN_X0       =0x"+cpputil::Uitox(*GetControlWordPtr(PATTERN_X0)));
		text.push_back("PATTERN_Y0       =0x"+cpputil::Uitox(*GetControlWordPtr(PATTERN_Y0)));
		text.push_back("RASTER           =0x"+cpputil::Uitox(*GetControlWordPtr(RASTER)));
		text.push_back("PIXEL8           =0x"+cpputil::Uitox(*GetControlWordPtr(PIXEL8)));
		text.push_back("WINDOW_MIN       =0x"+cpputil::Uitox(*GetControlWordPtr(WINDOW_MIN)));
		text.push_back("WINDOW_MAX       =0x"+cpputil::Uitox(*GetControlWordPtr(WINDOW_MAX)));

		text.push_back("HRZC             =0x"+cpputil::Uitox(*GetControlWordPtr(HRZC)));
		text.push_back("HRZT             =0x"+cpputil::Uitox(*GetControlWordPtr(HRZT)));
		text.push_back("HRZSR            =0x"+cpputil::Uitox(*GetControlWordPtr(HRZSR)));
		text.push_back("HRZBR            =0x"+cpputil::Uitox(*GetControlWordPtr(HRZBR)));
		text.push_back("HRZBF            =0x"+cpputil::Uitox(*GetControlWordPtr(HRZBF)));
		text.push_back("PREHRZC          =0x"+cpputil::Uitox(*GetControlWordPtr(PREHRZC)));
		text.push_back("VRTC             =0x"+cpputil::Uitox(*GetControlWordPtr(VRTC)));
		text.push_back("VRTT             =0x"+cpputil::Uitox(*GetControlWordPtr(VRTT)));
		text.push_back("VRTSR            =0x"+cpputil::Uitox(*GetControlWordPtr(VRTSR)));
		text.push_back("VRTBR            =0x"+cpputil::Uitox(*GetControlWordPtr(VRTBR)));
		text.push_back("VRTBF            =0x"+cpputil::Uitox(*GetControlWordPtr(VRTBF)));
		text.push_back("PREVRTC          =0x"+cpputil::Uitox(*GetControlWordPtr(PREVRTC)));
		text.push_back("SRADDR           =0x"+cpputil::Uitox(*GetControlWordPtr(SRADDR)));
		text.push_back("SRTCTL           =0x"+cpputil::Uitox(*GetControlWordPtr(SRTCTL)));

		text.push_back("MEM_CONFIG       =0x"+cpputil::Uitox(*GetControlWordPtr(MEM_CONFIG)));
		text.push_back("RFPERIOD         =0x"+cpputil::Uitox(*GetControlWordPtr(RFPERIOD)));
		text.push_back("RFCOUNT          =0x"+cpputil::Uitox(*GetControlWordPtr(RFCOUNT)));
		text.push_back("RLMAX            =0x"+cpputil::Uitox(*GetControlWordPtr(RLMAX)));
		text.push_back("RLCUR            =0x"+cpputil::Uitox(*GetControlWordPtr(RLCUR)));
	}
	else
	{
		text.push_back("FMT-3631/3632 is disabled.");
	}
	return text;
}


/*! Version used for serialization.
*/
uint32_t FMT3631::SerializeVersion(void) const
{
	// Version 2:
	//   VRAM size may be 2MB or 4MB.
	//   isFMT3632, RAMDACBaseAddr,ControlBaseAddr,VRAMBaseADdr.
	return 2;
};
/*! Device-specific Serialization.
*/
void FMT3631::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	PushBool(data,state.enabled);
	if(true!=state.enabled)
	{
		return;
	}

	PushBool(data,state.isFMT3632); // Version 2
	PushUint32(data,state.BaseAddr); // Version 2
	PushUint32(data,state.RAMDACBaseAddr); // Version 2
	PushUint32(data,state.ControlBaseAddr); // Version 2
	PushUint32(data,state.VRAMBaseAddr); // Version 2
	PushUint32(data,state.VRAMEndAddr); // Version 2
	PushUint32(data,state.alt_read_bank); // Version 2
	PushUint32(data,state.alt_write_bank); // Version 2
	PushUint16(data,state.fmt3632RegSel); // Version 2
	PushUcharArray(data,FMT3632REG_LEN,state.fmt3632Regs); // Version 2
	PushUint32(data,state.color2_3[0]); // Version 2
	PushUint32(data,state.color2_3[1]); // Version 2
	PushUint32(data,state.byteWinMinMax[0]); // Version 2
	PushUint32(data,state.byteWinMinMax[1]); // Version 2
	PushUint32(data,state.p9100CursorRegSel); // Version 2
	PushUint32(data,state.p9100CursorDataCount); // Version 2


	PushInt32(data,state.nLoadedCoord);
	PushInt32(data,state.lastLoadedCoord);
	PushInt32(data,state.nextLoadIndex);
	for(auto c : state.coord)
	{
		PushInt32(data,c.x());
		PushInt32(data,c.y());
	}
	for(auto i : state.metaCoordType)
	{
		PushUint32(data,i);
	}
	for(auto i : state.btCommandReg)
	{
		PushUint32(data,i);
	}
	state.plt.Serialize(data);

	PushBool(data,state.hwCursor.defining);
	PushBool(data,state.hwCursor.defined);
	PushUint32(data,state.hwCursor.ptnCount);
	PushUint32(data,state.hwCursor.unknownValueReg8);
	PushUint32(data,state.hwCursor.X);
	PushUint32(data,state.hwCursor.Y);
	PushUint32(data,state.hwCursor.originX);
	PushUint32(data,state.hwCursor.originY);
	PushUint32(data,state.hwCursor.wid);
	PushUcharArray(data,512,state.hwCursor.ANDPtn);
	PushUcharArray(data,512,state.hwCursor.ORPtn);
	PushBool(data,state.hwCursor.twoColorCursor);
	PushUcharArray(data,6,state.hwCursor.twoColor);
	PushUint16(data,state.hwCursorTwoColorReadPos);

	PushUint32(data,state.hwCursorXY_LowByte[0]);
	PushUint32(data,state.hwCursorXY_LowByte[1]);
	PushUint32(data,state.writingPalette);
	PushUint32(data,state.writingPaletteRGBCount);
	PushUint32(data,state.writingPaletteRGB[0]);
	PushUint32(data,state.writingPaletteRGB[1]);
	PushUint32(data,state.writingPaletteRGB[2]);

	PushInt32(data,state.pixelLeftUp.x());
	PushInt32(data,state.pixelLeftUp.y());
	PushInt32(data,state.pixelCurrent.x());
	PushInt32(data,state.pixelCurrent.y());
	PushInt32(data,state.pixelWid);
	PushInt32(data,state.pixelYIncrement);

	PushInt32(data,state.bitsPerPixel);
	PushBool(data,state.highColor565);
	PushUint32(data,state.masterSwitch);
	PushUint32(data,state.sysconfig);
	PushUint32(data,state.interrupt);
	PushUint32(data,state.interrupt_en);
	PushUint32(data,state.status);

	for(auto i : state.drawingAttrib)
	{
		PushUint32(data,i);
	}
	for(auto i : state.videoCtrl)
	{
		PushUint32(data,i);
	}
	for(auto i : state.vramCtrl)
	{
		PushUint32(data,i);
	}
	for(auto i : state.ctlCond)
	{
		PushUint32(data,i);
	}
	for(auto i : state.pattern)
	{
		PushUint32(data,i);
	}

	PushUcharArray(data,state.vram);
}
/*! Device-specific De-serialization.
*/
bool FMT3631::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	state.enabled=ReadBool(data);
	if(true!=state.enabled)
	{
		return true;
	}

	if(2<=version)
	{
		state.isFMT3632=ReadBool(data);
		state.BaseAddr=ReadUint32(data);
		state.RAMDACBaseAddr=ReadUint32(data);
		state.ControlBaseAddr=ReadUint32(data);
		state.VRAMBaseAddr=ReadUint32(data);
		state.VRAMEndAddr=ReadUint32(data);
		state.alt_read_bank=ReadUint32(data);
		state.alt_write_bank=ReadUint32(data);
		state.fmt3632RegSel=ReadUint16(data);
		ReadUcharArray(data,FMT3632REG_LEN,state.fmt3632Regs);
		state.color2_3[0]=ReadUint32(data);
		state.color2_3[1]=ReadUint32(data);
		state.byteWinMinMax[0]=ReadUint32(data);
		state.byteWinMinMax[1]=ReadUint32(data);
		state.p9100CursorRegSel=ReadUint32(data);
		state.p9100CursorDataCount=ReadUint32(data);
	}

	state.nLoadedCoord=ReadInt32(data);
	state.lastLoadedCoord=ReadInt32(data);
	state.nextLoadIndex=ReadInt32(data);
	for(auto &c : state.coord)
	{
		c.x()=ReadInt32(data);
		c.y()=ReadInt32(data);
	}
	for(auto &i : state.metaCoordType)
	{
		i=ReadUint32(data);
	}
	for(auto &i : state.btCommandReg)
	{
		i=ReadUint32(data);
	}
	state.plt.Deserialize(data);

	state.hwCursor.defining=ReadBool(data);
	state.hwCursor.defined=ReadBool(data);
	state.hwCursor.ptnCount=ReadUint32(data);
	state.hwCursor.unknownValueReg8=ReadUint32(data);
	state.hwCursor.X=ReadUint32(data);
	state.hwCursor.Y=ReadUint32(data);
	state.hwCursor.originX=ReadUint32(data);
	state.hwCursor.originY=ReadUint32(data);
	state.hwCursor.wid=ReadUint32(data);
	ReadUcharArray(data,512,state.hwCursor.ANDPtn);
	ReadUcharArray(data,512,state.hwCursor.ORPtn);
	state.hwCursor.twoColorCursor=ReadBool(data);
	ReadUcharArray(data,6,state.hwCursor.twoColor);
	state.hwCursorTwoColorReadPos=ReadUint16(data);

	state.hwCursorXY_LowByte[0]=ReadUint32(data);
	state.hwCursorXY_LowByte[1]=ReadUint32(data);
	state.writingPalette=ReadUint32(data);
	state.writingPaletteRGBCount=ReadUint32(data);
	state.writingPaletteRGB[0]=ReadUint32(data);
	state.writingPaletteRGB[1]=ReadUint32(data);
	state.writingPaletteRGB[2]=ReadUint32(data);

	state.pixelLeftUp.x()=ReadInt32(data);
	state.pixelLeftUp.y()=ReadInt32(data);
	state.pixelCurrent.x()=ReadInt32(data);
	state.pixelCurrent.y()=ReadInt32(data);
	state.pixelWid=ReadInt32(data);
	state.pixelYIncrement=ReadInt32(data);

	state.bitsPerPixel=ReadInt32(data);
	state.highColor565=ReadBool(data);
	state.masterSwitch=ReadUint32(data);
	state.sysconfig=ReadUint32(data);
	state.interrupt=ReadUint32(data);
	state.interrupt_en=ReadUint32(data);
	state.status=ReadUint32(data);

	if(2<=version)
	{
		for(auto &i : state.drawingAttrib)
		{
			i=ReadUint32(data);
		}
		for(auto &i : state.videoCtrl)
		{
			i=ReadUint32(data);
		}
		for(auto &i : state.vramCtrl)
		{
			i=ReadUint32(data);
		}
	}
	else
	{
		for(int i=0; i<10; ++i)
		{
			state.drawingAttrib[i]=ReadUint32(data);
		}
		for(int i=0; i<14; ++i)
		{
			state.videoCtrl[i]=ReadUint32(data);
		}
		for(int i=0; i<4; ++i)
		{
			state.vramCtrl[i]=ReadUint32(data);
		}
	}
	for(auto &i : state.ctlCond)
	{
		i=ReadUint32(data);
	}
	for(auto &i : state.pattern)
	{
		i=ReadUint32(data);
	}

	if(2<=version)
	{
		state.vram=ReadUcharArray(data);
	}
	else
	{
		state.vram.resize(VRAM_SIZE_3631);
		ReadUcharArray(data,VRAM_SIZE_3631,state.vram.data());
	}

	return true;
};
