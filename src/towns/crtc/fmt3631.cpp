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



FMT3631::FMT3631(class FMTownsCommon *ptr) : Device(ptr)
{
	state.vram.resize(VRAM_SIZE);
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
	memset(state.drawingAttrib,0,sizeof(state.drawingAttrib));
	memset(state.vram.data(),0,VRAM_SIZE);

	state.nLoadedCoord=0;
	state.lastLoadedCoord=0;
	memset(state.coord,0,sizeof(state.coord));

	memset(state.ctlCond,0,sizeof(state.ctlCond));

	state.pixelLeftUp=Vec2i::Make(0,0);
	state.pixelCurrent=Vec2i::Make(0,0);
	state.pixelWid=0;
	state.pixelYIncrement=1;

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

bool FMT3631::IsEnabled(void) const
{
	// According to p9000init.c of Linux XFree86 source, writing
	// 1e5, 1e4, or 1c4 enables Power 9000 video output.
	// 1e5 for 2MB VRAM (FMT-3631 is this case)    0001 1110 0101
	// 1e4 for 1MB VRAM.                           0001 1110 0100
	// 1c4 is unknown, but I don't care.           0001 1100 0100
	auto srtctl=*GetControlWordPtr(SRTCTL);
	return (true==state.enabled) && (0x1c4==(srtctl&0x1c4));
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

void FMT3631::MakePageLayerInfo(Layer &layer) const
{
	layer.bitsPerPixel=BitsPerPixel();
	layer.highResRGBSwap=0; // RGB or BRG or BGR.  Yet to figure what register controls it.
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
	static AnalogPalette plt;
	plt.Reset();
	for(int i=0; i<256; ++i)
	{
		int b=(i&3)*255/3;
		int r=((i>>2)&3)*255/3;
		int g=((i>>4)&7)*255/7;
		plt.plt256[i].Set(r,g,b,255);
	}
	return plt;
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


template <class returnType,class stateType>
inline returnType FMT3631::GetControlWordPtrTemplate(uint32_t physAddr,stateType &state)
{
	auto relAddr=(physAddr&TOWNSADDR_FMT3631_AND);
	switch(relAddr)
	{
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

unsigned int FMT3631::FetchByte(unsigned int physAddr) const
{
	unsigned int data=0xFF;
	bool monitor=false;

	if(true==state.enabled)
	{
		if(vramBaseAddr<physAddr)
		{
			data=state.vram[physAddr-TOWNSADDR_FMT3631_VRAM];
			monitor=monitorVRAM;
		}
		else
		{
			auto *ptr=GetControlWordPtr(physAddr);
			if(nullptr!=ptr)
			{
				data=*ptr;
			}
			monitor=monitorCtrl;
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
	bool monitor=false;

	if(true==state.enabled)
	{
		if(vramBaseAddr<physAddr)
		{
			data=cpputil::GetDword(state.vram.data()+physAddr-TOWNSADDR_FMT3631_VRAM);
			monitor=monitorVRAM;
		}
		else
		{
			auto *ptr=GetControlWordPtr(physAddr);
			if(nullptr!=ptr)
			{
				data=*ptr;
			}
			monitor=monitorCtrl;
		}
	}

	if(true==monitor)
	{
		std::cout << "Power9000 DWORD read  " << cpputil::Uitox(physAddr) << " " << cpputil::Uitox(data) << "\n";
	}

	return data;
}

Vec2i FMT3631::GetWindowOffset(void) const
{
	uint32_t offset=*GetControlWordPtr(WINDOW_OFFSET_XY);
	Vec2i v;
	v.x()=U16toS16(offset>>16);
	v.y()=U16toS16(offset&0xffff);
	return v;
}

void FMT3631::DeviceCoord(uint32_t physAddr,uint32_t data)
{
	std::cout << "DEVICE_COORD to be implemented.\n";
}

void FMT3631::LoadCoord(uint32_t physAddr,uint32_t data)
{
	auto absRef=GetWindowOffset();
	auto relRef=state.coord[state.lastLoadedCoord];
	auto &coordToLoad=state.coord[state.nLoadedCoord];

	if(state.nLoadedCoord<COORD_MAX)
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
		state.lastLoadedCoord=state.nLoadedCoord;
		++state.nLoadedCoord;
	}

	auto primType=physAddr&LOAD_COORD_PRIMTYPE_MASK;
	switch(primType)
	{
	case LOAD_COORD_PRIMTYPE_POINT: // 0,
		{
			DrawPoint();
			state.nLoadedCoord=0;
		}
		break;
	case LOAD_COORD_PRIMTYPE_LINE: // 0x40,
		if(2<=state.nLoadedCoord)
		{
			DrawLine();
			state.nLoadedCoord=0;
		}
		break;
	case LOAD_COORD_PRIMTYPE_TRI: // 0x80,
		if(3<=state.nLoadedCoord)
		{
			DrawTri();
			state.nLoadedCoord=0;
		}
		break;
	case LOAD_COORD_PRIMTYPE_QUAD: // 0xC0,
		if(4<=state.nLoadedCoord)
		{
			DrawQuad();
			state.nLoadedCoord=0;
		}
		break;
	case LOAD_COORD_PRIMTYPE_RECT: // 0x100,
		if(2<=state.nLoadedCoord)
		{
			DrawRect();
			state.nLoadedCoord=0;
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
void FMT3631::DrawRect(void)
{
	auto bytesPerLine=BytesPerLine();
	auto bitsPerPixel=BitsPerPixel();

	int x0=state.coord[0].x();
	int y0=state.coord[0].y();
	int x1=state.coord[1].x();
	int y1=state.coord[1].y();

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
		Vec2i::Make(Width()-1,Height()-1),
	};

	if(x1<clip[0].x() || clip[1].x()<x0 ||
	   y1<clip[0].y() || clip[1].y()<y0)
	{
		return;
	}

	if(8!=bitsPerPixel)
	{
		std::cout << "DrawRect for this bpp not supported yet.\n";
	}
	std::cout << "Rect\n";
	uint8_t *lineTop=state.vram.data()+bytesPerLine*y0+(bitsPerPixel*x0/8);
	auto fgColor=*GetControlWordPtr(FGCOLOR);
	for(auto y=y0; y<=y1; ++y)
	{
		auto ptr=lineTop;
		for(auto x=x0; x<=x1; ++x)
		{
			if(8==bitsPerPixel)
			{
				*(ptr++)=fgColor;
			}
		}
		lineTop+=bytesPerLine;
	}
}

void FMT3631::SetControlByte(uint32_t physAddr,uint8_t data)
{
	if((LOAD_COORD&physAddr)==LOAD_COORD)
	{
		LoadCoord(physAddr,data);
		return;
	}
	if((DEVICE_COORD&physAddr)==DEVICE_COORD)
	{
		DeviceCoord(physAddr,data);
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
	if((LOAD_COORD&physAddr)==LOAD_COORD)
	{
		LoadCoord(physAddr,data);
		return;
	}
	if((DEVICE_COORD&physAddr)==DEVICE_COORD)
	{
		DeviceCoord(physAddr,data);
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
	if((LOAD_COORD&physAddr)==LOAD_COORD)
	{
		LoadCoord(physAddr,data);
		return;
	}
	if((DEVICE_COORD&physAddr)==DEVICE_COORD)
	{
		DeviceCoord(physAddr,data);
		return;
	}

	auto ptr=GetControlWordPtr(physAddr);
	if(nullptr!=ptr)
	{
		*ptr=data;
	}
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
