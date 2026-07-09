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
	state.vram.resize(VRAM_SIZE);
	mutableThis=this;
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
	memset(state.videoCtrl,0,sizeof(state.videoCtrl));
	memset(state.vramCtrl,0,sizeof(state.vramCtrl));
	memset(state.drawingAttrib,0,sizeof(state.drawingAttrib));
	memset(state.vram.data(),0,VRAM_SIZE);
	memset(state.pattern,255,sizeof(state.pattern));

	state.nLoadedCoord=0;
	state.lastLoadedCoord=0;
	memset(state.coord,0,sizeof(state.coord));

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
	state.hwCursor.originX=32;
	state.hwCursor.originY=32;
	state.hwCursor.wid=32;
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
	return true==state.enabled && 0x1c4==(srtctl&0x1c4) && 0!=(MS_ENABLE&state.masterSwitch);
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
	return (bf-br)*4;
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
	layer.VScrollMask=VRAM_SIZE-1;
}
const FMT3631::AnalogPalette &FMT3631::GetPalette(void) const
{
	return state.plt;
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

bool FMT3631::IsReadableParameter(uint32_t &data,uint32_t physAddr) const
{
	if((0x1FFF07&physAddr)==DEVICE_COORD)
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
	return false;
}


template <class returnType,class stateType>
inline returnType FMT3631::GetControlWordPtrTemplate(uint32_t physAddr,stateType &state)
{
	auto relAddr=(physAddr&TOWNSADDR_FMT3631_AND);
	switch(relAddr)
	{
	case BT_COMMAND_REG_1:
		return &state.btCommandReg[1];
	case BT_COMMAND_REG_3:
		return &state.btCommandReg[3];

	case MASTERSWITCH: // 0x0
		return &state.masterSwitch;
	case SYSCONFIG: //0x00004,
		return &state.sysconfig;
	case INTERRUPT: //0x00008,
		return &state.interrupt;
	case INTERRUPT_EN: //0x0000C,
		return &state.interrupt_en;

	// Status
	case STATUS          : //0x80000,
		return &state.status;

	// Control and condition
	case OOR             : //0x180184,
	//               0x180188, // Not used
	case CINDEX          : //0x18018C,
	case WINDOW_OFFSET_XY: //0x180190,
	case P_W_MIN         : //0x180194, // Read Only
	case P_W_MAX         : //0x180198,
	//               0x18019C, // Not used
	case YCLIP           : //0x1801A0,
	case XCLIP           : //0x1801A4,
	case XEDGE_LT        : //0x1801A8,
	case XEDGE_GT        : //0x1801AC,
	case YEDGE_LT        : //0x1801B0,
	case YEDGE_GT        : //0x1801B4,
		return &state.ctlCond[(relAddr-CTL_COND_BEGIN)/4];

	// Drawing Engine Registers
	// Pixel Processing 4.5
	case FGCOLOR:
	case BGCOLOR:
	case PLANE_MASK      ://0x180208,
	case DRAWING_MODE    ://0x18020C,
	case PATTERN_X0      ://0x180210,
	case PATTERN_Y0      ://0x180214,
	case RASTER          ://0x180218,
	case PIXEL8          ://0x18021C,
	case WINDOW_MIN      ://0x180220,
	case WINDOW_MAX      ://0x180224,
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

void FMT3631::DeviceCoordOrLoadCoord(Vec2i &coordToLoad,Vec2i absRef,Vec2i relRef,uint32_t physAddr,uint32_t data)
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
	}
}

void FMT3631::DeviceCoord(uint32_t physAddr,uint32_t data)
{
	auto absRef=Vec2i::Make(0,0);
	auto relRef=GetWindowOffset();
	auto idx=(physAddr>>6)&3;
	auto &coordToLoad=state.coord[idx];
	DeviceCoordOrLoadCoord(coordToLoad,absRef,relRef,physAddr,data);

	if(true==monitorCtrl)
	{
		std::cout << "DEVICE_COORD " << idx << "\n";
	}
}

void FMT3631::LoadCoord(uint32_t physAddr,uint32_t data)
{
	auto absRef=GetWindowOffset();
	auto relRef=state.coord[state.lastLoadedCoord];
	auto &coordToLoad=state.coord[state.nLoadedCoord];

	if(state.nLoadedCoord<COORD_MAX)
	{
		DeviceCoordOrLoadCoord(coordToLoad,absRef,relRef,physAddr,data);
		state.lastLoadedCoord=state.nLoadedCoord;
		++state.nLoadedCoord;
	}

	auto primType=physAddr&LOAD_COORD_PRIMTYPE_MASK;
	switch(primType)
	{
	case LOAD_COORD_PRIMTYPE_POINT: // 0,
		break;
	case LOAD_COORD_PRIMTYPE_LINE: // 0x40,
		break;
	case LOAD_COORD_PRIMTYPE_TRI: // 0x80,
		if(3==state.nLoadedCoord)
		{
			state.coord[3]=state.coord[2];
			state.nLoadedCoord=4;
		}
		break;
	case LOAD_COORD_PRIMTYPE_QUAD: // 0xC0,
		break;
	case LOAD_COORD_PRIMTYPE_RECT: // 0x100,
		if(2==state.nLoadedCoord)
		{
			state.coord[2]=state.coord[1];
			state.coord[1].x()=state.coord[0].x();
			state.coord[1].y()=state.coord[2].y();
			state.coord[3].x()=state.coord[2].x();
			state.coord[3].y()=state.coord[0].y();
			state.nLoadedCoord=4;
		}
		break;
	}
}

void FMT3631::DrawPoint(void)
{
	std::cout << "DrawPoint not supported yet.\n";
}
void FMT3631::DrawLine(void)
{
	std::cout << "DrawLine not supported yet.\n";
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
		Vec2i::Make(0,0),
		Vec2i::Make(Width()*bytesPerPixel-1,Height()-1),
	};

	if(x1<clip[0].x() || clip[1].x()<x0 ||
	   y1<clip[0].y() || clip[1].y()<y0)
	{
		return;
	}

	if(8!=bitsPerPixel && 32!=bitsPerPixel)
	{
		std::cout << "DrawRect for this bpp not supported yet.\n";
	}
	std::cout << "Rect\n";
	uint8_t *lineTop=state.vram.data()+bytesPerLine*y0+x0;
	auto fgColor=*GetControlWordPtr(FGCOLOR);
	auto bgColor=*GetControlWordPtr(BGCOLOR);
	uint32_t raster=*GetControlWordPtr(RASTER);
	bool usePattern=(0!=(raster&RASTER_USEPATTERN));
	uint32_t patternBit=0x80000000;
	for(auto y=y0; y<=y1; ++y)
	{
		uint32_t pattern=state.pattern[y%PATTERN_LEN];

		// Shockingly, Power 9000's Quad command fills the bytes within the region with the same byte.
		// If it is 24-bit color mode, it can only draw gray scale.
		// Linux Power 9000 driver draws a quad twice with a pattern and logic ops to fill component by component.
		// Windows driver apparently did not push it that much.
		auto ptr=lineTop;
		for(auto x=x0; x<=x1; ++x)
		{
			// Apparently X coordinate is multiplied by bytes-per-pixel by the software.
			switch(raster&0xFFFF)
			{
			default:
				if(true==breakOnUnsupported)
				{
					auto *towns=(FMTownsCommon *)vmPtr;
					towns->debugger.ExternalBreak("Unsupported Raster type for Rect "+cpputil::Itoa(bitsPerPixel)+" bpp");
				}
				break;
			case 0xff00: // Copy
				*ptr=fgColor;
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
			++ptr;
			patternBit>>=1;
			if(0==patternBit)
			{
				patternBit=0x80000000;
			}
		}
		lineTop+=bytesPerLine;
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

class Pixel1CopyTransparent
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
class Pixel1CopyOpaque
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

template <class LogicOp>
void FMT3631::CmdPixel1Loop(uint32_t physAddr,uint32_t data,bool doSwap)
{
	uint32_t count=1+((physAddr>>2)&31);

	if(true==doSwap)
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
			if(8==BitsPerPixel())
			{
				LogicOp::DoLogicOp(lineTop[state.pixelCurrent.x()],fg,fgColor,bgColor);
			}
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

void FMT3631::CmdPixel1(uint32_t physAddr,uint32_t data,bool doSwap)
{
	if(true==monitorCtrl)
	{
		std::cout << "Pixel1 " << state.pixelCurrent.x() << " " << state.pixelCurrent.y() << " " << state.pixelYIncrement << "\n";
	}

	uint16_t raster=*GetControlWordPtr(RASTER);
	switch(raster)
	{
	default:
		if(true==breakOnUnsupported)
		{
			auto *towns=(FMTownsCommon *)vmPtr;
			towns->debugger.ExternalBreak("Unsupported Raster type for Pixel1\n");
		}
		CmdPixel1Loop<Pixel1CopyTransparent>(physAddr,data,doSwap);
		break;

	case 0xee22:
		CmdPixel1Loop<Pixel1CopyTransparent>(physAddr,data,doSwap);
		break;

	case 0xfc30:
		CmdPixel1Loop<Pixel1CopyOpaque>(physAddr,data,doSwap);
		break;
	}
}

void FMT3631::CmdPixel8(uint32_t physAddr,uint32_t data,bool doSwap)
{
	if(true==breakOnUnsupported)
	{
		auto *towns=(FMTownsCommon *)vmPtr;
		towns->debugger.ExternalBreak("Pixel8 not supported yet.\n");
	}
}


uint32_t FMT3631::CmdQuad(uint32_t physAddr) // Apparently, it is executed by Fetch.
{
	if(state.coord[1].x()==state.coord[0].x() &&
	   state.coord[1].y()==state.coord[2].y() &&
	   state.coord[3].x()==state.coord[2].x() &&
	   state.coord[3].y()==state.coord[0].y())
	{
		DrawRect(state.coord[0],state.coord[2]);
		state.nLoadedCoord=0;
		return 0;
	}
	std::cout << "General quad not implemented yet.\n";
	if(true==breakOnUnsupported)
	{
		auto *towns=(FMTownsCommon *)vmPtr;
		towns->debugger.ExternalBreak("General quad not implemented yet.\n");
	}
	state.nLoadedCoord=0;
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

	state.nLoadedCoord=0;
	return 0;
}

bool FMT3631::IsCommand(uint32_t physAddr,uint32_t data)
{
	const auto masked=(COMMAND_MASK&physAddr);

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
	if(masked==BT_COMMAND_REG_3)
	{
		state.btCommandReg[3]=data;
		if(0!=(BT_CR3_64SQ_CURSOR&data))
		{
			state.hwCursor.wid=64;
		}
		else
		{
			state.hwCursor.wid=32;
		}
	}
	if(masked==BT_WRITE_ADDR)
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
	if(masked==BT_CURS_RAM_DATA)
	{
		if(state.hwCursor.ptnCount<512)
		{
			state.hwCursor.ORPtn[state.hwCursor.ptnCount]=data;
			++state.hwCursor.ptnCount;
		}
		else if(state.hwCursor.ptnCount<1024)
		{
			state.hwCursor.ANDPtn[state.hwCursor.ptnCount-512]=data;
			++state.hwCursor.ptnCount;
		}
		return true;
	}
	if(masked==BT_RAMDAC_DATA)
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
	if(masked==BT_CURS_X_LOW)
	{
		state.hwCursorXY_LowByte[0]=data;
		return true;
	}
	if(masked==BT_CURS_Y_LOW)
	{
		state.hwCursorXY_LowByte[1]=data;
		return true;
	}
	if(masked==BT_CURS_X_HIGH)
	{
		state.hwCursor.X=(state.hwCursorXY_LowByte[0]&255)|(data<<8);
		return true;
	}
	if(masked==BT_CURS_Y_HIGH)
	{
		state.hwCursor.Y=(state.hwCursorXY_LowByte[1]&255)|(data<<8);
		return true;
	}


	if((0x1FFE07&physAddr)==LOAD_COORD)
	{
		LoadCoord(physAddr,data);
		return true;
	}
	if((0x1FFF07&physAddr)==DEVICE_COORD)
	{
		DeviceCoord(physAddr,data);
		return true;
	}
	if((0x1FFFFF&physAddr)==NEXT_PIXELS_CMD)
	{
		CmdNextPixels(physAddr,data);
		return true;
	}
	if((0x1FFF80&physAddr)==PIXEL1_CMD)
	{
		CmdPixel1(physAddr,data,false);
		return true;
	}
	if((0x1FFF80&physAddr)==PIXEL1_SWAP_CMD)
	{
		CmdPixel1(physAddr,data,true);
		return true;
	}
	if(masked==PIXEL8_CMD)
	{
		CmdPixel8(physAddr,data,false);
	}
	if(masked==PIXEL8_SWAP_CMD)
	{
		CmdPixel8(physAddr,data,true);
	}
	if(masked==QUAD_CMD)
	{
		CmdQuad(physAddr);
		return true;
	}
	if(masked==BLIT_CMD)
	{
		CmdBlit(physAddr);
		return true;
	}
	return false;
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
	}
}


unsigned int FMT3631::FetchByte(unsigned int physAddr) const
{
	unsigned int data=0xFF;
	bool monitor=false;

	if(true==state.enabled)
	{
		if(vramBaseAddr<physAddr)
		{
			monitor=monitorVRAM;
			data=state.vram[physAddr-TOWNSADDR_FMT3631_VRAM];
		}
		else
		{
			monitor=monitorCtrl;
			if((COMMAND_MASK&physAddr)==QUAD_CMD)
			{
				data=mutableThis->CmdQuad(physAddr);
			}
			else if((COMMAND_MASK&physAddr)==BLIT_CMD)
			{
				data=mutableThis->CmdBlit(physAddr);
			}
			else if(true==IsReadableParameter(data,physAddr))
			{
			}
			else
			{
				auto *ptr=GetControlWordPtr(physAddr);
				if(nullptr!=ptr)
				{
					data=*ptr;
				}
			}
		}
	}

	if(true==monitor)
	{
		std::cout << "Power9000 BYTE read   " << cpputil::Uitox(physAddr) << " " << cpputil::Ubtox(data) << "\n";
	}

	return data;
}

unsigned int FMT3631::FetchWord(unsigned int physAddr) const
{
	unsigned int data=0xFFFF;
	bool monitor=false;

	if(true==state.enabled)
	{
		if(vramBaseAddr<physAddr)
		{
			monitor=monitorVRAM;
			data=cpputil::GetWord(state.vram.data()+physAddr-TOWNSADDR_FMT3631_VRAM);
		}
		else
		{
			monitor=monitorCtrl;
			if((COMMAND_MASK&physAddr)==QUAD_CMD)
			{
				data=mutableThis->CmdQuad(physAddr);
			}
			else if((COMMAND_MASK&physAddr)==BLIT_CMD)
			{
				data=mutableThis->CmdBlit(physAddr);
			}
			else if(true==IsReadableParameter(data,physAddr))
			{
			}
			else
			{
				auto *ptr=GetControlWordPtr(physAddr);
				if(nullptr!=ptr)
				{
					data=*ptr;
				}
			}
		}
	}

	if(true==monitor)
	{
		std::cout << "Power9000 WORD read   " << cpputil::Uitox(physAddr) << " " << cpputil::Ustox(data) << "\n";
	}

	return data;
}

unsigned int FMT3631::FetchDword(unsigned int physAddr) const
{
	unsigned int data=0xFFFFFFFF;
	bool monitor=false;

	if(true==state.enabled)
	{
		if(vramBaseAddr<physAddr)
		{
			monitor=monitorVRAM;
			data=cpputil::GetDword(state.vram.data()+physAddr-TOWNSADDR_FMT3631_VRAM);
		}
		else
		{
			monitor=monitorCtrl;
			if((COMMAND_MASK&physAddr)==QUAD_CMD)
			{
				data=mutableThis->CmdQuad(physAddr);
			}
			else if((COMMAND_MASK&physAddr)==BLIT_CMD)
			{
				data=mutableThis->CmdBlit(physAddr);
			}
			else if(true==IsReadableParameter(data,physAddr))
			{
			}
			else
			{
				auto *ptr=GetControlWordPtr(physAddr);
				if(nullptr!=ptr)
				{
					data=*ptr;
				}
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
	bool monitor=false;
	if(true==state.enabled)
	{
		if(vramBaseAddr<physAddr)
		{
			state.vram[physAddr-TOWNSADDR_FMT3631_VRAM]=data;
			monitor=monitorVRAM;
		}
		else
		{
			SetControlByte(physAddr,data);
			monitor=monitorCtrl;
		}
	}
	if(true==monitor)
	{
		std::cout << "Power9000 BYTE Write   " << cpputil::Uitox(physAddr) << " " <<  cpputil::Ubtox(data) << "\n";
	}
}

void FMT3631::StoreWord(unsigned int physAddr,unsigned int data)
{
	bool monitor=false;
	if(true==state.enabled)
	{
		if(vramBaseAddr<physAddr)
		{
			cpputil::PutWord(state.vram.data()+physAddr-TOWNSADDR_FMT3631_VRAM,data);
			monitor=monitorVRAM;
		}
		else
		{
			SetControlWord(physAddr,data);
			monitor=monitorCtrl;
		}
	}
	if(true==monitor)
	{
		std::cout << "Power9000 WORD Write   " << cpputil::Uitox(physAddr) << " " <<  cpputil::Ustox(data) << "\n";
	}
}

void FMT3631::StoreDword(unsigned int physAddr,unsigned int data)
{
	bool monitor=false;
	if(true==state.enabled)
	{
		if(vramBaseAddr<physAddr)
		{
			cpputil::PutDword(state.vram.data()+physAddr-TOWNSADDR_FMT3631_VRAM,data);
			monitor=monitorVRAM;
		}
		else
		{
			SetControlDword(physAddr,data);
			monitor=monitorCtrl;
		}
	}
	if(true==monitor)
	{
		std::cout << "Power9000 DWORD Write  " << cpputil::Uitox(physAddr) << " " << cpputil::Uitox(data) <<  "\n";
	}
}
