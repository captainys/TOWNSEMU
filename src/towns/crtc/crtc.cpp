/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <sstream>
#include <algorithm>

#include "cpputil.h"
#include "crtc.h"
#include "sprite.h"
#include "towns.h"
#include "townsdef.h"



const unsigned int TownsCRTC::CLKSELtoHz[4]=
{
	28636300,   // 28636KHz
	24545400,   // 24545KHz
	25175000,   // 25175KHz
	21052500,   // 21052KHz
};

void TownsCRTC::AnalogPalette::Reset(void)
{
	codeLatch=0;
	for(int i=0; i<2; ++i)
	{
		plt16[i][ 0].Set(  0,  0,  0,255);
		plt16[i][ 1].Set(  0,  0,128,255);
		plt16[i][ 2].Set(128,  0,  0,255);
		plt16[i][ 3].Set(128,  0,128,255);
		plt16[i][ 4].Set(  0,128,  0,255);
		plt16[i][ 5].Set(  0,128,128,255);
		plt16[i][ 6].Set(128,128,  0,255);
		plt16[i][ 7].Set(128,128,128,255);
		plt16[i][ 8].Set(  0,  0,  0,255);
		plt16[i][ 9].Set(  0,  0,255,255);
		plt16[i][10].Set(255,  0,  0,255);
		plt16[i][11].Set(255,  0,255,255);
		plt16[i][12].Set(  0,255,  0,255);
		plt16[i][13].Set(  0,255,255,255);
		plt16[i][14].Set(255,255,  0,255);
		plt16[i][15].Set(255,255,255,255);
	}

	for(int i=0; i<256; ++i)
	{
		plt256[i].Set(255,255,255,255);
	}
}

void TownsCRTC::HardwareMouseCursor::Reset(void)
{
	defining=false;
	defined=false;
	ptnCount=0;
	unknownValueReg8=0; // Maybe color.
	X=0;
	Y=0;
	originX=0;
	originY=0;
	for(auto &c : ANDPtn)
	{
		c=0;
	}
	for(auto &c : ORPtn)
	{
		c=0;
	}

}

void TownsCRTC::AnalogPalette::Set16(unsigned int page,unsigned int component,unsigned char v)
{
	v=v&0xF0;
	v|=(v>>4);
	plt16[page][codeLatch&0x0F].v[component]=v;
}
void TownsCRTC::AnalogPalette::Set256(unsigned int component,unsigned char v)
{
	plt256[codeLatch].v[component]=v;
}
void TownsCRTC::AnalogPalette::SetRed(unsigned char v,unsigned int PLT)
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		Set16(0,0,v);
		break;
	case 2: // 16-color paletter Layer 1
		Set16(1,0,v);
		break;
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		Set256(0,v);
		break;
	}
}
void TownsCRTC::AnalogPalette::SetGreen(unsigned char v,unsigned int PLT)
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		Set16(0,1,v);
		break;
	case 2: // 16-color paletter Layer 1
		Set16(1,1,v);
		break;
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		Set256(1,v);
		break;
	}
}
void TownsCRTC::AnalogPalette::SetBlue(unsigned char v,unsigned int PLT)
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		Set16(0,2,v);
		break;
	case 2: // 16-color paletter Layer 1
		Set16(1,2,v);
		break;
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		Set256(2,v);
		break;
	}
}

unsigned char TownsCRTC::AnalogPalette::Get16(unsigned int page,unsigned int component) const
{
	return plt16[page][codeLatch&0x0F][component]&0xF0;
}
unsigned char TownsCRTC::AnalogPalette::Get256(unsigned int component) const
{
	return plt256[codeLatch][component];
}
unsigned char TownsCRTC::AnalogPalette::GetRed(unsigned int PLT) const
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		return Get16(0,0);
	case 2: // 16-color paletter Layer 1
		return Get16(1,0);
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		return Get256(0);
	}
	return 0;
}
unsigned char TownsCRTC::AnalogPalette::GetGreen(unsigned int PLT) const
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		return Get16(0,1);
	case 2: // 16-color paletter Layer 1
		return Get16(1,1);
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		return Get256(1);
	}
	return 0;
}
unsigned char TownsCRTC::AnalogPalette::GetBlue(unsigned int PLT) const
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		return Get16(0,2);
	case 2: // 16-color paletter Layer 1
		return Get16(1,2);
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		return Get256(2);
	}
	return 0;
}


////////////////////////////////////////////////////////////


void TownsCRTC::State::Reset(void)
{
	DPMD=false;
	for(auto &i : FMRPalette)
	{
		i=(unsigned int)(&i-FMRPalette);
	}

	unsigned int defCRTCReg[32]=
	{
		0x0040,0x0320,0x0000,0x0000,0x035F,0x0000,0x0010,0x0000,0x036F,0x009C,0x031C,0x009C,0x031C,0x0040,0x0360,0x0040,
		0x0360,0x0000,0x009C,0x0000,0x0050,0x0000,0x009C,0x0000,0x0050,0x004A,0x0001,0x0000,0x003F,0x0003,0x0000,0x0150,
	};
	for(int i=0; i<32; ++i)
	{
		crtcReg[i]=defCRTCReg[i];
	}
	crtcAddrLatch=0;

	unsigned char defSifter[4]={0x15,0x08,0,0};
	for(int i=0; i<4; ++i)
	{
		sifter[i]=defSifter[i];
	}
	sifterAddrLatch=0;

	palette.Reset();
	highResCrtcMouse.Reset();

	for(auto &d : highResCrtcReg)
	{
		d=0;
	}
	// highResAvailable=true;   Memo to self: Don't change it on Reset.
	highResCRTCEnabled=false;
	highResCrtcRegAddrLatch=0;
	highResCrtcPalette.Reset();

	FMRGVRAMDisplayPlanes=0x0F;
	FMRVRAMOffset=0;

	showPageFDA0[0]=true;
	showPageFDA0[1]=true;

	showPage0448[0]=true;
	showPage0448[1]=true;

	highResCrtcReg4Bit1=true;
}

void TownsCRTC::TurnOffVSYNCIRQ(void)
{
	state.VSYNCIRQ=false;
	townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_VSYNC,false);
}

void TownsCRTC::TurnOnVSYNCIRQ(void)
{
	state.VSYNCIRQ=true;
	townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_VSYNC,true);
}

TownsCRTC::ScreenModeCache::ScreenModeCache()
{
	MakeFMRCompatible();
}

void TownsCRTC::ScreenModeCache::MakeFMRCompatible(void)
{
	numLayers=2;

	layer[0].VRAMAddr=0;
	layer[0].VRAMHSkipBytes=0;
	layer[0].bitsPerPixel=4;
	layer[0].sizeOnMonitor=Vec2i::Make(640,400);
	layer[0].bytesPerLine=320;

	layer[1].VRAMAddr=0x40000;
	layer[1].VRAMHSkipBytes=0;
	layer[1].bitsPerPixel=4;
	layer[1].sizeOnMonitor=Vec2i::Make(640,400);
	layer[1].bytesPerLine=512;
}

////////////////////////////////////////////////////////////


TownsCRTC::TownsCRTC(class FMTownsCommon *ptr,TownsSprite *spritePtr) : Device(ptr)
{
	this->townsPtr=ptr;
	this->spritePtr=spritePtr;
	state.Reset();

	// Tentatively
	cached=true;
}

void TownsCRTC::UpdateSpriteHardware(void)
{
	if(true==InSinglePageMode() ||
	   16!=GetPageBitsPerPixel(1) ||
	   512!=GetPageBytesPerLine(1))
	{
		spritePtr->state.screenModeAcceptsSprite=false;
	}
	else
	{
		spritePtr->state.screenModeAcceptsSprite=true;
	}
}


// Let's say 60 frames per sec.
// 1 frame takes 16.7ms.
// Horizontal Scan frequency is say 31KHz.
// 1 line takes 0.032ms.
// 480 lines take 15.36ms.
// Then, VSYNC should be 1.34ms long.
// Will take screenmode into account eventually.
// Also should take HSYNC into account.
bool TownsCRTC::InVSYNC(const unsigned long long int townsTime) const
{
	unsigned int intoFrame=((unsigned long long)townsTime)%VSYNC_CYCLE;
	return  (CRT_VERTICAL_DURATION<intoFrame);
}
bool TownsCRTC::InHSYNC(const unsigned long long int townsTime) const
{
	unsigned int intoFrame=((unsigned long long)townsTime)%VSYNC_CYCLE;
	if(intoFrame<CRT_VERTICAL_DURATION)
	{
		auto intoLine=intoFrame%HSYNC_CYCLE;
		return (CRT_HORIZONTAL_DURATION<intoLine);
	}
	return false;
}
bool TownsCRTC::AvoidFirst1msOfVerticalPeriod(const unsigned long long int townsTime) const
{
	unsigned int intoFrame=((unsigned long long)townsTime)%VSYNC_CYCLE;
	return  1000000<intoFrame && intoFrame<CRT_VERTICAL_DURATION;
}

bool TownsCRTC::InSinglePageMode(void) const
{
	if(true!=state.highResCRTCEnabled)
	{
		return LowResCrtcIsInSinglePageMode();
	}
	else
	{
		return HighResCrtcIsInSinglePageMode();
	}
}

bool TownsCRTC::LowResCrtcIsInSinglePageMode(void) const
{
	return (0==(state.sifter[0]&0x10));
}

uint32_t TownsCRTC::GetEffectiveVRAMSize(void) const
{
	if(true==state.highResCRTCEnabled)
	{
		return TOWNS_VRAM_SIZE;
	}
	else
	{
		return TOWNS_MIDRES_VRAM_SIZE;
	}
}

bool TownsCRTC::IsInFMRCompatibleMode(void) const
{
	if(true==LowResCrtcIsInSinglePageMode())
	{
		return false;
	}

	auto pageSize=GetPageSizeOnMonitor(0);
	if(640!=pageSize.x() || 400!=pageSize.y())
	{
		return false;
	}
	pageSize=GetPageSizeOnMonitor(1);
	if(640!=pageSize.x() || 400!=pageSize.y())
	{
		return false;
	}

	if(4!=GetPageBitsPerPixel(0) || 4!=GetPageBitsPerPixel(1))
	{
		return false;
	}

	return true;
}

unsigned int TownsCRTC::GetBaseClockFreq(void) const
{
	auto CLKSEL=state.crtcReg[REG_CR1]&3;
	return CLKSELtoHz[CLKSEL];
}
unsigned int TownsCRTC::GetBaseClockScaler(void) const
{
	auto SCSEL=(state.crtcReg[REG_CR1]&0x0C)>>2;
	return (SCSEL+1)*2;
}
Vec2i TownsCRTC::GetPageZoom2X(unsigned char page) const
{
	if(true!=state.highResCRTCEnabled)
	{
		return GetLowResPageZoom2X(page);
	}
	else
	{
		return GetHighResPageZoom2X(page);
	}
}
Vec2i TownsCRTC::GetLowResPageZoom2X(unsigned char page) const
{
	Vec2i zoom;
	auto pageZoom=(state.crtcReg[REG_ZOOM]>>(8*page));
	zoom.x()=(( pageZoom    &15)+1);
	zoom.y()=(((pageZoom>>4)&15)+1);

	// I'm not sure if this logic is correct.  This doesn't cover screen mode 16.
	if(15==GetHorizontalFrequency())
	{
		if(true==LowResCrtcIsInSinglePageMode())
		{
			auto FO=state.crtcReg[REG_FO0+4*page];
			auto LO=state.crtcReg[REG_LO0+4*page];
			if(0==FO || FO==LO)  // FO==LO condition is to render GENOCIDE2 opening correctly.
			{
				zoom[1]*=4;
			}
			else
			{
				zoom[1]*=2;
			}
		}
		else
		{
			zoom[1]*=4;
		}
	}
	else if(3==CLKSEL() && 0x29D==state.crtcReg[REG_HST])
	{
		// VING games use this settings.  Apparently zoom-x needs to be interpreted as 4+(pageZoom&15).
		// Chase HQ        HST=029DH  ZOOM=1111H  Zoom2X=5
		// Viewpoint       HST=029DH  ZOOM=1111H  Zoom2X=5
		// Pu Li Ru La     HST=029DH  ZOOM=1111H  Zoom2X=5
		// Splatter House  HST=029DH  ZOOM=1111H  Zoom2X=5
		// Operation Wolf  N/A
		// New Zealand Story  N/A
		// Alshark Opening HST=029DH  ZOOM=0000H  Zoom2X=2
		// Freeware Collection 8 Oh!FM TOWNS Cover Picture Collection  HST=029DH  ZOOM=0000H  Zoom2X=2
		zoom[0]=2+3*(pageZoom&15); // Is it right?
		zoom[1]*=2;
	}
	else
	{
		zoom[0]*=2;
		zoom[1]*=2;
	}

	return zoom;
}
Vec2i TownsCRTC::GetPageOriginOnMonitor(unsigned char page) const
{
	if(true!=state.highResCRTCEnabled)
	{
		return GetLowResPageOriginOnMonitor(page);
	}
	else
	{
		return GetHighResPageOriginOnMonitor(page);
	}
}
Vec2i TownsCRTC::GetLowResPageOriginOnMonitor(unsigned char page) const
{
	int x0,y0,STD_H,STD_V;
	static const int reg[6]=
	{
		REG_HDS0,REG_HAJ0,REG_VDS0,
		REG_HDS1,REG_HAJ1,REG_VDS1,
	};
	/* Emerald Dragon uses HDS0<HAJ0.
	   If my interpretation is correct, CRTC won't start scanning VRAM until reaching HAJ0,
	   making the left-edge be at HAJ0, not HDS0.
	*/
	auto HDS=std::max(state.crtcReg[reg[page*3]],state.crtcReg[reg[page*3+1]]);
	auto VDS=state.crtcReg[reg[page*3+2]];
	switch(state.crtcReg[REG_HST])
	{
	case 779:
		STD_H=127;
		break;
	case 863:
		STD_H=156;
		break;
	case 895: // for Super Real Mahjang P2 & P3
		STD_H=160;
		break;
	case 1559: // 15KHz Non-interlaced mode
		STD_H=231;
		break;
	case 1819: // 15KHz Interlaced mode
		STD_H=297;
		break;
	default: // usually 669 or 799
		STD_H=138;
		break;
	}
	auto HDS_MIN=std::min(state.crtcReg[REG_HDS0],state.crtcReg[REG_HDS1]);
	if(HDS_MIN<STD_H)
	{
		STD_H=HDS_MIN;
	}
	switch(state.crtcReg[REG_VST])
	{
	case 523:
		STD_V=40;
		break;
	case 524:
		STD_V=42;
		break;
	case 879:
		STD_V=64;
		break;
	default: // usually 1049
		STD_V=70;
		break;
	}
	auto VDS_MIN=std::min(state.crtcReg[REG_VDS0],state.crtcReg[REG_VDS1]);
	if(VDS_MIN<STD_V)
	{
		STD_V=VDS_MIN;
	}
	x0=(HDS-STD_H);
	y0=(VDS-STD_V)>>1; // I'm not sure if I should divide by 2.  Will need experiments.

	// Probably >>
	if(15==GetHorizontalFrequency())
	{
		x0>>=1;
		y0<<=1;
	}
	// Probably <<

	if(x0<0)
	{
		x0=0;
	}
	if(y0<0)
	{
		y0=0;
	}
	return Vec2i::Make(x0,y0);
}
unsigned int TownsCRTC::GetVRAMHSkip1X(unsigned char page) const
{
	static const unsigned int regs[2][2]=
	{
		{REG_HAJ0,REG_HDS0},
		{REG_HAJ1,REG_HDS1},
	};
	int HAJ=state.crtcReg[regs[page][0]];
	int HDS=state.crtcReg[regs[page][1]];
	if(HAJ<HDS)
	{
		return (HDS-HAJ);
	}
	return 0;
}
Vec2i TownsCRTC::GetPageSizeOnMonitor(unsigned char page) const
{
	auto KHz=GetHorizontalFrequency();
	auto wid=state.crtcReg[REG_HDE0+page*2]-state.crtcReg[REG_HDS0+page*2];
	auto hei=state.crtcReg[REG_VDE0+page*2]-state.crtcReg[REG_VDS0+page*2];
	if(15==KHz)
	{
		wid/=2;
		hei*=2;
	}
	else if(3==CLKSEL() && 0x29D==state.crtcReg[REG_HST]) // VING Setting
	{
		// VING games use this settings.  Apparently zoom-x needs to be interpreted as 4+(pageZoom&15).
		// Chase HQ        HDS0=0082H  HDE0=00282H (Diff=512)  HST=029DH  ZOOM=1111H  Zoom2X=5  wid=640
		// Viewpoint       HDE0=008AH  HDE0=0028AH (Diff=512)  HST=029DH  ZOOM=1111H  Zoom2X=5  wid=640
		// Pu Li Ru La     HDE0=008AH  HDE0=0028AH (Diff=512)  HST=029DH  ZOOM=1111H  Zoom2X=5  wid=640
		// Splatter House  HDE0=008AH  HDE0=0028AH (Diff=512)  HST=029DH  ZOOM=1111H  Zoom2X=5  wid=640
		// Operation Wolf  N/A
		// New Zealand Story  N/A
		// Alshark Opening HDS0=0082H  HDE0=0282H  (Diff=512)  HST=029DH  ZOOM=0000H  Zoom2X=2  wid=512? 640?  Single-Page Mode
		// Freeware Collection 8 Oh!FM TOWNS Cover Picture Collection  
		//                 HDE0=0082H  HDE0=0282H  (Diff=512)  HST=029DH  ZOOM=0000H  Zoom2X=2  wid=512? 640?  Single-Page Mode

		// Kurunkurupa While Zooming
		//                 HDS0=0082H  HDE0=0282H  (Diff=512)  HST=029DH  ZOOM=1111H  Zoom2X=5  wid=640
		//                 HDS0=0082H  HDE0=0282H  (Diff=512)  HST=029DH  ZOOM=2222H  Zoom2X=8  wid=640
		//                 HDS0=0082H  HDE0=0282H  (Diff=512)  HST=029DH  ZOOM=3232H  Zoom2X=8  wid=640
		//                 HDS0=0082H  HDE0=0282H  (Diff=512)  HST=029DH  ZOOM=4242H  Zoom2X=8  wid=640
		//                 HDS0=0082H  HDE0=0282H  (Diff=512)  HST=029DH  ZOOM=4343H  Zoom2X=11 wid=640
		//                 HDS0=0082H  HDE0=0282H  (Diff=512)  HST=029DH  ZOOM=5353H  Zoom2X=11 wid=640

		// Looks like zoom2X=5 -> wid*5/4
		//            zoom2X=2 -> wid*4/4

		auto zoom=GetLowResPageZoom2X(page);
		if(5<=zoom.x())  // I have zero confidence in this condition.  It just takes care of known cases.
		{
			wid*=zoom.x();
			wid/=4;
		}
	}
	auto FO=state.crtcReg[REG_FO0+4*page];
	auto LO=state.crtcReg[REG_LO0+4*page];
	if(0==FO || FO==LO)  // FO==LO condition is to render GENOCIDE2 opening correctly.
	{
		hei/=2;
	}

	if(800<wid)
	{
		wid=800;
	}

	return Vec2i::Make(wid,hei);
}
Vec2i TownsCRTC::GetPageVRAMCoverageSize1X(unsigned char page) const
{
	auto wid=state.crtcReg[REG_HDE0+page*2]-state.crtcReg[REG_HDS0+page*2];
	auto hei=state.crtcReg[REG_VDE0+page*2]-state.crtcReg[REG_VDS0+page*2];
	auto FO=state.crtcReg[REG_FO0+4*page];
	auto LO=state.crtcReg[REG_LO0+4*page];
	if(0==FO || FO==LO)  // FO==LO condition is to render GENOCIDE2 opening correctly.
	{
		hei/=2;
	}
	return Vec2i::Make(wid,hei);
}
unsigned int TownsCRTC::GetPageBitsPerPixel(unsigned char page) const
{
	const unsigned int CL=(state.crtcReg[REG_CR0]>>(page*2))&3;
	if(true==LowResCrtcIsInSinglePageMode())
	{
		if(2==CL)
		{
			return 16;
		}
		else if(3==CL)
		{
			return 8;
		}
	}
	else
	{
		if(1==CL)
		{
			return 16;
		}
		else if(3==CL)
		{
			return 4;
		}
	}
	// std::cout << __FUNCTION__ << std::endl;
	// std::cout << "Unknown color setting." << std::endl;
	return 4; // What else can I do?
}
unsigned int TownsCRTC::GetPageVRAMAddressOffset(unsigned char page) const
{
	// [2] pp. 145
	auto FA0=state.crtcReg[REG_FA0+page*4];
	switch(GetPageBitsPerPixel(page))
	{
	case 4:
		return FA0*4;  // 8 pixels for 1 count.
	case 8:
		return FA0*8;  // 8 pixels for 1 count.
	case 16:
		return (LowResCrtcIsInSinglePageMode() ? FA0*8 : FA0*4); // 4 pixels or 2 pixels depending on the single-page or 2-page mode.
	}
	return 0;
}
unsigned int TownsCRTC::GetPriorityPage(void) const
{
	if(true!=state.highResCRTCEnabled)
	{
		return state.sifter[1]&1;
	}
	else
	{
		return state.highResCrtcReg[HIGHRES_REG_DISPPAGE]&1;
	}
}
unsigned int TownsCRTC::GetPageBytesPerLine(unsigned char page) const
{
	auto LOx=state.crtcReg[REG_LO0+page*4];
	auto numBytes=LOx*4;   // Why did I think it was (LOx-FOx)*4?
	if(true==LowResCrtcIsInSinglePageMode())
	{
		numBytes*=2;
	}
	return numBytes;
}

void TownsCRTC::MakePageLayerInfo(Layer &layer,unsigned char page) const
{
	if(true!=state.highResCRTCEnabled)
	{
		MakeLowResPageLayerInfo(layer,page);
	}
	else
	{
		MakeHighResPageLayerInfo(layer,page);
	}
}

const TownsCRTC::AnalogPalette &TownsCRTC::GetPalette(void) const
{
	if(true==state.highResCRTCEnabled)
	{
		return state.highResCrtcPalette;
	}
	else
	{
		return state.palette;
	}
}

void TownsCRTC::MakeLowResPageLayerInfo(Layer &layer,unsigned char page) const
{
	page&=1;
	layer.bitsPerPixel=GetPageBitsPerPixel(page);
	layer.highResRGBSwap=0; // Not used by the low-res screen modes.
	layer.originOnMonitor=GetLowResPageOriginOnMonitor(page);
	layer.sizeOnMonitor=GetPageSizeOnMonitor(page);
	layer.VRAMCoverage1X=GetPageVRAMCoverageSize1X(page);
	layer.zoom2x=GetLowResPageZoom2X(page);
	layer.VRAMAddr=0x40000*page;
	layer.VRAMOffset=GetPageVRAMAddressOffset(page);
	if (page == 0) {
		layer.FlipVRAMOffset = state.FMRVRAMOffset; // Can be applied only to layer 0 in two-layer mode, or in the single-page mode.  Either way page==0.
	} else {
		layer.FlipVRAMOffset = townsPtr->sprite.DisplayVRAMOffset();
	}
	layer.FMRGVRAMMask=(0==page ? state.FMRGVRAMDisplayPlanes : 0x0F); // GVRAM Planes works as a mask in 4-bit color mode for VRAM layer 0 only.
	layer.bytesPerLine=GetPageBytesPerLine(page);

	// VRAMSkipBytes looks to depend on raw zoom factor.
	auto rawZoomX=((state.crtcReg[REG_ZOOM]>>(8*page))&15)+1;
	layer.VRAMHSkipBytes=(((GetVRAMHSkip1X(page)/rawZoomX)*layer.bitsPerPixel)>>3);

	if(true==cpputil::Is2toN(layer.bytesPerLine))
	{
		layer.HScrollMask=layer.bytesPerLine-1;
	}
	else
	{
		layer.HScrollMask=0xFFFFFFFF;
	}
	if(true==LowResCrtcIsInSinglePageMode() && 0==page)
	{
		layer.VScrollMask=0x7FFFF;
	}
	else
	{
		layer.VScrollMask=0x3FFFF;
	}
}

void TownsCRTC::MEMIOWriteFMRVRAMDisplayMode(unsigned char data)
{
	auto disp=((data>>2)&8)|(data&7);
	auto page=((data>>4)&1);

	/* FM Towns 2F did not show any effect on disp bits.
	   Function of these bits are supposed to be turn on/off VRAM bit planes.
	   However, actual effect is unknown.
       -> Earlier test program was wrong.  These bits works as a mask in 4-bit color mode.
	*/
	state.FMRGVRAMDisplayPlanes=disp;

	/* Test on FM Towns 2F indicated that FM-R Page switching is done by an additional address offsetting between CRTC and VRAM.
	   The offset is applied only to 80000000H to 8004000H.  Since the single-page mode references the VRAM staggered,
	   image will be broken staggered way if this additional offset is applied in the single-page mode.
	   However, no FM Towns application that uses this staggered effect has been confirmed.

	   Also, [2] tells bit 6 needs to be 1 always.  However, TBIOS AH=06 writes 27H to this register.  Obviously bit 6 is off.
	   FM Towns 2F showed no different behavior if bit 6 is 0.
	*/
	state.FMRVRAMOffset=(0==(data&0x10) ? 0 : TOWNS_FMRMODE_VRAM_OFFSET);
}

/* virtual */ void TownsCRTC::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_ANALOGPALETTE_CODE://=  0xFD90,
		state.palette.codeLatch=data;
		break;
	case TOWNSIO_ANALOGPALETTE_BLUE://=  0xFD92,
		state.palette.SetBlue(data,(state.sifter[1]>>4)&3);
		break;
	case TOWNSIO_ANALOGPALETTE_RED://=   0xFD94,
		state.palette.SetRed(data,(state.sifter[1]>>4)&3);
		break;
	case TOWNSIO_ANALOGPALETTE_GREEN://= 0xFD96,
		state.palette.SetGreen(data,(state.sifter[1]>>4)&3);
		break;


	case TOWNSIO_FMR_DIGITALPALETTE0:// 0xFD98,
	case TOWNSIO_FMR_DIGITALPALETTE1:// 0xFD99,
	case TOWNSIO_FMR_DIGITALPALETTE2:// 0xFD9A,
	case TOWNSIO_FMR_DIGITALPALETTE3:// 0xFD9B,
	case TOWNSIO_FMR_DIGITALPALETTE4:// 0xFD9C,
	case TOWNSIO_FMR_DIGITALPALETTE5:// 0xFD9D,
	case TOWNSIO_FMR_DIGITALPALETTE6:// 0xFD9E,
	case TOWNSIO_FMR_DIGITALPALETTE7:// 0xFD9F,
		state.FMRPalette[ioport&7]=(data&0x0F);
		state.DPMD=true;
		break;


	case TOWNSIO_CRTC_ADDRESS://             0x440,
		state.crtcAddrLatch=data&0x1f;
		break;
	case TOWNSIO_CRTC_DATA_LOW://            0x442,
		state.crtcReg[state.crtcAddrLatch]&=0xff00;
		state.crtcReg[state.crtcAddrLatch]|=(data&0xff);
		if(REG_HST==state.crtcAddrLatch)
		{
			townsPtr->OnCRTC_HST_Write();
		}
		UpdateSpriteHardware();
		break;
	case TOWNSIO_CRTC_DATA_HIGH://           0x443,
		state.crtcReg[state.crtcAddrLatch]&=0x00ff;
		state.crtcReg[state.crtcAddrLatch]|=((data&0xff)<<8);
		if(REG_HST==state.crtcAddrLatch)
		{
			townsPtr->OnCRTC_HST_Write();
		}
		if(REG_CR0==state.crtcAddrLatch)
		{
			WriteCR0(data<<8);
		}
		UpdateSpriteHardware();
		break;


	case TOWNSIO_VIDEO_OUT_CTRL_ADDRESS://=   0x448,
		state.sifterAddrLatch=(data&3);
		break;
	case TOWNSIO_VIDEO_OUT_CTRL_DATA://=      0x44A,
		state.sifter[state.sifterAddrLatch]=data;
		if(0==state.sifterAddrLatch)
		{
			if(LowResCrtcIsInSinglePageMode())
			{
				state.showPage0448[0]=(0!=(data&0x08));
				state.showPage0448[1]=false;
			}
			else
			{
				state.showPage0448[0]=(0!=(data&0x01));
				state.showPage0448[1]=(0!=(data&0x04));
			}
		}
		UpdateSpriteHardware();
		break;

	case TOWNSIO_MX_HIRES://            0x470,
	case TOWNSIO_MX_VRAMSIZE://         0x471,
		break; // No write access;

	case TOWNSIO_MX_IMGOUT_ADDR_LOW://  0x472,
		state.highResCrtcRegAddrLatch=((state.highResCrtcRegAddrLatch&0xff00)|(data&0xff));
		break;
	case TOWNSIO_MX_IMGOUT_ADDR_HIGH:// 0x473,
		state.highResCrtcRegAddrLatch=((state.highResCrtcRegAddrLatch&0x00ff)|((data<<8)&0xff));
		break;
	case TOWNSIO_MX_IMGOUT_D0://   0x474,
		// std::cout << "MX-VIDOUTCONTROL8[" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << "H]=" << cpputil::Ubtox(data) << "H" << std::endl;
		if(true==monitorCRTC2)
		{
			std::ostringstream ss;
			ss << "Write to CRTC2 Reg(D0)=" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Ubtox(data);
			std::cout << ss.str() << std::endl;
			townsPtr->debugger.WriteLogFile(ss.str());
		}
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]&=0xFFFFFF00;
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]|=(data&0xFF);
		}
		switch(state.highResCrtcRegAddrLatch)
		{
		case HIGHRES_REG_CTRL0:
			state.highResCRTCEnabled=(0!=(data&1));
			break;
		case HIGHRES_REG_CTRL1:
			if(0!=(data&2))
			{
				state.highResCrtcReg4Bit0=false;
				state.highResCrtcReg4Bit1=false;
			}
			break;
		case HIGHRES_REG_PALINDEX:
			state.highResCrtcPalette.codeLatch=data;
			break;
		case HIGHRES_REG_PALCOL:
			switch(state.highResCrtcReg[HIGHRES_REG_PALSEL])
			{
			case 0:
				state.highResCrtcPalette.Set16(0,2,data);
				break;
			case 1:
				state.highResCrtcPalette.Set16(1,2,data);
				break;
			case 2:
				state.highResCrtcPalette.Set256(2,data);
				break;
			}
			break;
		case HIGHRES_REG_MOUSE_PATTERN:
			if(true==state.highResCrtcMouse.defining)
			{
				if(state.highResCrtcMouse.ptnCount<512)
				{
					state.highResCrtcMouse.ANDPtn[state.highResCrtcMouse.ptnCount]=data;
					++state.highResCrtcMouse.ptnCount;
				}
				else if(state.highResCrtcMouse.ptnCount<1024)
				{
					state.highResCrtcMouse.ORPtn[state.highResCrtcMouse.ptnCount-512]=data;
					++state.highResCrtcMouse.ptnCount;
				}
			}
			break;
		}
		break;
	case TOWNSIO_MX_IMGOUT_D1://   0x475,
		if(true==monitorCRTC2)
		{
			std::ostringstream ss;
			ss << "Write to CRTC2 Reg(D1)=" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Ubtox(data);
			std::cout << ss.str() << std::endl;
			townsPtr->debugger.WriteLogFile(ss.str());
		}
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			// std::cout << "MX-VIDOUTCONTROL8[" << cpputil::Ustox(state.highResCrtcRegAddrLatch+1) << "H]=" << cpputil::Ubtox(data) << "H" << std::endl;
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]&=0xFFFF00FF;
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]|=((data&0xFF)<<8);
			if(HIGHRES_REG_PALCOL==state.highResCrtcRegAddrLatch)
			{
				switch(state.highResCrtcReg[HIGHRES_REG_PALSEL])
				{
				case 0:
					state.highResCrtcPalette.Set16(0,0,data);
					break;
				case 1:
					state.highResCrtcPalette.Set16(1,0,data);
					break;
				case 2:
					state.highResCrtcPalette.Set256(0,data);
					break;
				}
			}
		}
		break;
	case TOWNSIO_MX_IMGOUT_D2://   0x476,
		if(true==monitorCRTC2)
		{
			std::ostringstream ss;
			ss << "Write to CRTC2 Reg(D2)=" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Ubtox(data);
			std::cout << ss.str() << std::endl;
			townsPtr->debugger.WriteLogFile(ss.str());
		}
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			// std::cout << "MX-VIDOUTCONTROL8[" << cpputil::Ustox(state.highResCrtcRegAddrLatch+2) << "H]=" << cpputil::Ubtox(data) << "H" << std::endl;
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]&=0xFF00FFFF;
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]|=((data&0xFF)<<16);
			if(HIGHRES_REG_PALCOL==state.highResCrtcRegAddrLatch)
			{
				switch(state.highResCrtcReg[HIGHRES_REG_PALSEL])
				{
				case 0:
					state.highResCrtcPalette.Set16(0,1,data);
					break;
				case 1:
					state.highResCrtcPalette.Set16(1,1,data);
					break;
				case 2:
					state.highResCrtcPalette.Set256(1,data);
					break;
				}
			}
		}
		break;
	case TOWNSIO_MX_IMGOUT_D3://   0x477,
		if(true==monitorCRTC2)
		{
			std::ostringstream ss;
			ss << "Write to CRTC2 Reg(D3)=" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Ubtox(data);
			std::cout << ss.str() << std::endl;
			townsPtr->debugger.WriteLogFile(ss.str());
		}
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			// std::cout << "MX-VIDOUTCONTROL8[" << cpputil::Ustox(state.highResCrtcRegAddrLatch+3) << "H]=" << cpputil::Ubtox(data) << "H" << std::endl;
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]&=0x00FFFFFF;
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]|=((data&0xFF)<<24);
		}
		break;
	case TOWNSIO_HSYNC_VSYNC:  // 0xFDA0 Also CRT Output COntrol
		if(LowResCrtcIsInSinglePageMode())
		{
			state.showPageFDA0[0]=(0!=((data>>2)&3));
			state.showPageFDA0[1]=state.showPageFDA0[0];
		}
		else
		{
			state.showPageFDA0[0]=(0!=((data>>2)&3));
			state.showPageFDA0[1]=(0!=( data    &3));
		}
		break;
	case TOWNSIO_FMR_HSYNC_VSYNC: // 0xFF86
		break;
	case TOWNSIO_WRITE_TO_CLEAR_VSYNCIRQ:// 0x5CA
		TurnOffVSYNCIRQ();
		break;
	}
}

/* virtual */ void TownsCRTC::IOWriteWord(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_CRTC_ADDRESS://             0x440,
		state.crtcAddrLatch=data&0x1f;
		break;
	case TOWNSIO_CRTC_DATA_LOW://            0x442,
		state.crtcReg[state.crtcAddrLatch]=(data&0xffff);
		if(REG_HST==state.crtcAddrLatch)
		{
			townsPtr->OnCRTC_HST_Write();
		}
		if(REG_CR0==state.crtcAddrLatch)
		{
			WriteCR0(data);
		}
		UpdateSpriteHardware();
		break;
	case TOWNSIO_CRTC_DATA_HIGH://           0x443,
		break;

	case TOWNSIO_VIDEO_OUT_CTRL_ADDRESS://=   0x448,
		state.sifterAddrLatch=(data&3);
		break;
	case TOWNSIO_VIDEO_OUT_CTRL_DATA://=      0x44A,
		state.sifter[state.sifterAddrLatch]=data;
		UpdateSpriteHardware();
		break;

	case TOWNSIO_MX_HIRES://            0x470,
	case TOWNSIO_MX_VRAMSIZE://         0x471,
		break; // No write access;

	case TOWNSIO_MX_IMGOUT_ADDR_LOW://  0x472,
		state.highResCrtcRegAddrLatch=data;
		break;
	case TOWNSIO_MX_IMGOUT_D0://   0x474,
		// std::cout << "MX-VIDOUTCONTROL16[" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << "H]=" << cpputil::Ustox(data) << "H" << std::endl;
		if(state.highResCrtcRegAddrLatch!=HIGHRES_REG_WD_MOUSEX && //0x200
		   state.highResCrtcRegAddrLatch!=HIGHRES_REG_WD_MOUSEY)   //0x201
		{
			if(true==monitorCRTC2)
			{
				std::ostringstream ss;
				ss << "Write to CRTC2 Reg(D0)=" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Ustox(data);
				std::cout << ss.str() << std::endl;
				townsPtr->debugger.WriteLogFile(ss.str());
			}
		}
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]&=0xFFFF0000;
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]|=(data&0xFFFF);
		}
		switch(state.highResCrtcRegAddrLatch)
		{
		case HIGHRES_REG_CTRL0:
			state.highResCRTCEnabled=(0!=(data&1));
			break;

		case HIGHRES_REG_CTRL1:
			if(0!=(data&2))
			{
				state.highResCrtcReg4Bit0=false;
				state.highResCrtcReg4Bit1=false;
			}
			break;
		case HIGHRES_REG_PALINDEX:
			state.highResCrtcPalette.codeLatch=data&0xFF;
			break;
		case HIGHRES_REG_PALCOL:
			switch(state.highResCrtcReg[HIGHRES_REG_PALSEL])
			{
			case 0:
				state.highResCrtcPalette.Set16(0,2,data&0xFF);          // Low Green
				state.highResCrtcPalette.Set16(0,0,(data>>8)&0xFF);     // High Red
				break;
			case 1:
				state.highResCrtcPalette.Set16(1,2,data&0xFF);          // Low Green
				state.highResCrtcPalette.Set16(1,0,(data>>8)&0xFF);     // High Red
				break;
			case 2:
				state.highResCrtcPalette.Set256(2,data&0xFF);          // Low Green
				state.highResCrtcPalette.Set256(0,(data>>8)&0xFF);     // High Red
				break;
			}
			break;
		case HIGHRES_REG_WD_MOUSEX: //0x200
			state.highResCrtcMouse.X=data;
			// Also update reported coord.  It is an ad-hoc solution to Windows 3.1 not sending WM_TIMER event to TGMOUSE in certain (unknown yet) conditions.
			townsPtr->var.mouseXReported=data;
			break;
		case HIGHRES_REG_WD_MOUSEY: //0x201
			state.highResCrtcMouse.Y=data;
			// Also update reported coord.  It is an ad-hoc solution to Windows 3.1 not sending WM_TIMER event to TGMOUSE in certain (unknown yet) conditions.
			townsPtr->var.mouseYReported=data;
			break;
		case HIGHRES_REG_WD_MOUSE_ORIGINX: //0x202
			state.highResCrtcMouse.originX=data;
			break;
		case HIGHRES_REG_WD_MOUSE_ORIGINY: //0x203
			state.highResCrtcMouse.originY=data;
			break;
		case HIGHRES_REG_WD_MOUSE_DEFINE: //0x206
			if(0==data)
			{
				state.highResCrtcMouse.defining=true;
				state.highResCrtcMouse.defined=false;
				state.highResCrtcMouse.ptnCount=0;
			}
			else
			{
				if(true==state.highResCrtcMouse.defining)
				{
					state.highResCrtcMouse.defined=true;
					state.highResCrtcMouse.defining=false;
				}
			}
			break;
		case HIGHRES_REG_WD_MOUSE_UNKNOWN8: //0x208,
			state.highResCrtcMouse.unknownValueReg8=data;
			break;
		case HIGHRES_REG_MOUSE_PATTERN:
			if(true==monitorCRTC2)
			{
				std::cout << "Word writing to the hardware-mouse pattern?" << std::endl;
			}
			break;
		}
		break;
	case TOWNSIO_MX_IMGOUT_D1://   0x475,
		// std::cout << "MX-VIDOUTCONTROL16[" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << "H]=" << cpputil::Ustox(data) << "H" << std::endl;
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]&=0xFF0000FF;
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]|=((data&0xFFFF)<<8);
			if(true==monitorCRTC2)
			{
				std::ostringstream ss;
				ss << "Write to CRTC2 Reg(D1)=" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Ustox(data);
				std::cout << ss.str() << std::endl;
				townsPtr->debugger.WriteLogFile(ss.str());
			}
		}
		break;
	case TOWNSIO_MX_IMGOUT_D2://   0x476,
		// std::cout << "MX-VIDOUTCONTROL16[" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << "H]=" << cpputil::Ustox(data) << "H" << std::endl;
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]&=0x0000FFFF;
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]|=((data&0xFFFF)<<16);
			if(true==monitorCRTC2)
			{
				std::ostringstream ss;
				ss << "Write to CRTC2 Reg(D2)=" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Ustox(data);
				std::cout << ss.str() << std::endl;
				townsPtr->debugger.WriteLogFile(ss.str());
			}
			if(HIGHRES_REG_PALCOL==state.highResCrtcRegAddrLatch)
			{
				switch(state.highResCrtcReg[HIGHRES_REG_PALSEL])
				{
				case 0:
					state.highResCrtcPalette.Set16(0,1,data&0xFF);
					break;
				case 1:
					state.highResCrtcPalette.Set16(1,1,data&0xFF);
					break;
				case 2:
					state.highResCrtcPalette.Set256(1,data&0xFF);
					break;
				}
			}
			state.highResCrtcPalette.codeLatch++;
			state.highResCrtcPalette.codeLatch&=0xFF;
		}
		break;
	case TOWNSIO_MX_IMGOUT_D3://   0x477,
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			if(true==monitorCRTC2)
			{
				std::ostringstream ss;
				ss << "Write to CRTC2 Reg(D3)=" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Ustox(data);
				std::cout << ss.str() << std::endl;
				townsPtr->debugger.WriteLogFile(ss.str());
			}
		}
		break;

	default:
		Device::IOWriteWord(ioport,data); // Let it write twice.
		break;
	}
}

/* virtual */ void TownsCRTC::IOWriteDword(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_MX_IMGOUT_D0://   0x474,
		// std::cout << "MX-VIDOUTCONTROL32[" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << "H]=" << cpputil::Uitox(data) << "H" << std::endl;
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			state.highResCrtcReg[state.highResCrtcRegAddrLatch]=data;
		}

		if(true==monitorCRTC2)
		{
			std::ostringstream ss;
			ss << "Write to CRTC2 Reg(D0)=" << cpputil::Uitox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Uitox(data);
			std::cout << ss.str() << std::endl;
			townsPtr->debugger.WriteLogFile(ss.str());
		}
		switch(state.highResCrtcRegAddrLatch)
		{
		case HIGHRES_REG_CTRL0://   =0x000,   // Write 0 -> ?   1->enable highRes CRTC
			state.highResCRTCEnabled=(0!=(data&1));
			break;

		case HIGHRES_REG_CTRL1://   =0x004,   // Read bit0=HighResEnabled bit1=(Initial=1, WriteToBit1->0, HighResoDisabled->1)   Write bit1=1->bit0=0
			if(0!=(data&2))
			{
				state.highResCrtcReg4Bit0=false;
				state.highResCrtcReg4Bit1=false;
			}
			break;

		case HIGHRES_REG_PALINDEX:
			state.highResCrtcPalette.codeLatch=data&0xFF;
			break;
		case HIGHRES_REG_PALCOL:
			switch(state.highResCrtcReg[HIGHRES_REG_PALSEL])
			{
			case 0:
				state.highResCrtcPalette.Set16(0,2,data&0xFF);       // B
				state.highResCrtcPalette.Set16(0,0,(data>>8)&0xFF);  // R
				state.highResCrtcPalette.Set16(0,1,(data>>16)&0xFF); // G
				break;
			case 1:
				state.highResCrtcPalette.Set16(1,2,data&0xFF);
				state.highResCrtcPalette.Set16(1,0,(data>>8)&0xFF);
				state.highResCrtcPalette.Set16(1,1,(data>>16)&0xFF);
				break;
			case 2:
				state.highResCrtcPalette.Set256(2,data&0xFF);
				state.highResCrtcPalette.Set256(0,(data>>8)&0xFF);
				state.highResCrtcPalette.Set256(1,(data>>16)&0xFF);
				break;
			}
			state.highResCrtcPalette.codeLatch++;
			state.highResCrtcPalette.codeLatch&=0xFF;
			break;
		case HIGHRES_REG_MOUSE_PATTERN:
			if(true==state.highResCrtcMouse.defining)
			{
				if(state.highResCrtcMouse.ptnCount<512)
				{
					state.highResCrtcMouse.ANDPtn[state.highResCrtcMouse.ptnCount]=data;
					++state.highResCrtcMouse.ptnCount;
				}
				else if(state.highResCrtcMouse.ptnCount<1024)
				{
					state.highResCrtcMouse.ORPtn[state.highResCrtcMouse.ptnCount-512]=data;
					++state.highResCrtcMouse.ptnCount;
				}
			}
			break;
		case HIGHRES_REG_WD_MOUSEX: //0x200
			state.highResCrtcMouse.X=data;
			// Also update reported coord.  It is an ad-hoc solution to Windows 3.1 not sending WM_TIMER event to TGMOUSE in certain (unknown yet) conditions.
			townsPtr->var.mouseXReported=data;
			break;
		case HIGHRES_REG_WD_MOUSEY: //0x201
			state.highResCrtcMouse.Y=data;
			// Also update reported coord.  It is an ad-hoc solution to Windows 3.1 not sending WM_TIMER event to TGMOUSE in certain (unknown yet) conditions.
			townsPtr->var.mouseYReported=data;
			break;
		case HIGHRES_REG_WD_MOUSE_ORIGINX: //0x202
			state.highResCrtcMouse.originX=data;
			break;
		case HIGHRES_REG_WD_MOUSE_ORIGINY: //0x203
			state.highResCrtcMouse.originY=data;
			break;
		case HIGHRES_REG_WD_MOUSE_DEFINE: //0x206
			if(0==data)
			{
				state.highResCrtcMouse.defining=true;
				state.highResCrtcMouse.defined=false;
				state.highResCrtcMouse.ptnCount=0;
			}
			else
			{
				if(true==state.highResCrtcMouse.defining)
				{
					state.highResCrtcMouse.defined=true;
					state.highResCrtcMouse.defining=false;
				}
			}
			break;
		case HIGHRES_REG_WD_MOUSE_UNKNOWN8: //0x208,
			state.highResCrtcMouse.unknownValueReg8=data;
			break;
		}
		break;
	case TOWNSIO_MX_IMGOUT_D1://   0x475,
		// std::cout << "MX-VIDOUTCONTROL16[" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << "H]=" << cpputil::Ustox(data) << "H" << std::endl;
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			if(true==monitorCRTC2)
			{
				std::ostringstream ss;
				ss << "Write to CRTC2 Reg(D1)=" << cpputil::Uitox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Uitox(data);
				std::cout << ss.str() << std::endl;
				townsPtr->debugger.WriteLogFile(ss.str());
			}
		}
		break;
	case TOWNSIO_MX_IMGOUT_D2://   0x476,
		// std::cout << "MX-VIDOUTCONTROL16[" << cpputil::Ustox(state.highResCrtcRegAddrLatch) << "H]=" << cpputil::Ustox(data) << "H" << std::endl;
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			if(true==monitorCRTC2)
			{
				std::ostringstream ss;
				ss << "Write to CRTC2 Reg(D2)=" << cpputil::Uitox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Uitox(data);
				std::cout << ss.str() << std::endl;
				townsPtr->debugger.WriteLogFile(ss.str());
			}
		}
		break;
	case TOWNSIO_MX_IMGOUT_D3://   0x477,
		if(state.highResCrtcRegAddrLatch<NUM_HIRES_CRTC_REGISTERS)
		{
			if(true==monitorCRTC2)
			{
				std::ostringstream ss;
				ss << "Write to CRTC2 Reg(D3)=" << cpputil::Uitox(state.highResCrtcRegAddrLatch) << " Value=" << cpputil::Uitox(data);
				std::cout << ss.str() << std::endl;
				townsPtr->debugger.WriteLogFile(ss.str());
			}
		}
		break;

	case TOWNSIO_ANALOGPALETTE_CODE://=  0xFD90,
		{
			const unsigned char code=data&0xFF;
			const unsigned char blue=(data>>16)&0xFF;
			state.palette.codeLatch=code;
			state.palette.SetBlue(blue,(state.sifter[1]>>4)&3);
			chaseHQPalette.AddCodeAndBlue(code,blue);
		}
		break;
	case TOWNSIO_ANALOGPALETTE_BLUE://=  0xFD92,
		state.palette.SetBlue(data&0xFF,(state.sifter[1]>>4)&3);
		state.palette.SetRed((data>>16)&0xFF,(state.sifter[1]>>4)&3);
		break;
	case TOWNSIO_ANALOGPALETTE_RED://=   0xFD94,
		{
			const unsigned char red=data&0xFF;
			const unsigned char green=(data>>16)&0xFF;
			state.palette.SetRed(red,(state.sifter[1]>>4)&3);
			state.palette.SetGreen(green,(state.sifter[1]>>4)&3);
			chaseHQPalette.SetRedAndGreen(red,green);
		}
		break;
	case TOWNSIO_ANALOGPALETTE_GREEN://= 0xFD96,
		state.palette.SetGreen(data&0xFF,(state.sifter[1]>>4)&3);
		break;

	default:
		// Analog-Palette Registers allow DWORD Access.
		// Towns MENU V2.1 writes to palette like:
		// 0110:000015C4 66BA94FD                  MOV     DX,FD94H
		// 0110:000015C8 EF                        OUT     DX,EAX
		// 0110:000015C9 8AC4                      MOV     AL,AH
		// 0110:000015CB B292                      MOV     DL,92H
		// 0110:000015CD EE                        OUT     DX,AL
		Device::IOWriteDword(ioport,data); // Let it write 4 times.
		break;
	}
}

/* virtual */ unsigned int TownsCRTC::IOReadByte(unsigned int ioport)
{
	unsigned char data=0xff;
	switch(ioport)
	{
	case TOWNSIO_ANALOGPALETTE_CODE://=  0xFD90,
		data=state.palette.codeLatch;
		break;
	case TOWNSIO_ANALOGPALETTE_BLUE://=  0xFD92,
		data=state.palette.GetBlue((state.sifter[1]>>4)&3);
		break;
	case TOWNSIO_ANALOGPALETTE_RED://=   0xFD94,
		data=state.palette.GetRed((state.sifter[1]>>4)&3);
		break;
	case TOWNSIO_ANALOGPALETTE_GREEN://= 0xFD96,
		data=state.palette.GetGreen((state.sifter[1]>>4)&3);
		break;


	case TOWNSIO_FMR_DIGITALPALETTE0:// 0xFD98,
	case TOWNSIO_FMR_DIGITALPALETTE1:// 0xFD99,
	case TOWNSIO_FMR_DIGITALPALETTE2:// 0xFD9A,
	case TOWNSIO_FMR_DIGITALPALETTE3:// 0xFD9B,
	case TOWNSIO_FMR_DIGITALPALETTE4:// 0xFD9C,
	case TOWNSIO_FMR_DIGITALPALETTE5:// 0xFD9D,
	case TOWNSIO_FMR_DIGITALPALETTE6:// 0xFD9E,
	case TOWNSIO_FMR_DIGITALPALETTE7:// 0xFD9F,
		return state.FMRPalette[ioport&7];
		break;


	case TOWNSIO_CRTC_ADDRESS://             0x440,
		break;
	case TOWNSIO_CRTC_DATA_LOW://            0x442,
		// It is supposed to be write-only, but 
		//   1 bit "START" and high-byte of FR can be read.  Why not make all readable then.
		data=state.crtcReg[state.crtcAddrLatch]&0xff;
		break;

	case TOWNSIO_VIDEO_OUT_CTRL_ADDRESS://=   0x448,  Supposed to be write-only
		data=state.sifterAddrLatch;
		break;
	case TOWNSIO_VIDEO_OUT_CTRL_DATA://=      0x44A,  Supposed to be write-only
		data=state.sifter[state.sifterAddrLatch];
		break;
	case TOWNSIO_DPMD_SPRITEBUSY_SPRITEPAGE: // 044CH  [2] pp.153
		data=(true==state.DPMD ? 0x80 : 0);
		data|=(true==spritePtr->SPD0() ? 2 : 0);
		data|=(spritePtr->PAGE() ? 1 : 0);
		state.DPMD=false;
		break;

	case TOWNSIO_CRTC_DATA_HIGH://           0x443,
		if(REG_FR==state.crtcAddrLatch)
		{
			const auto VSYNC=InVSYNC(townsPtr->state.townsTime);
			const auto HSYNC=InHSYNC(townsPtr->state.townsTime);
			const auto DSPTV0=!VSYNC;
			const auto DSPTV1=DSPTV0;
			const auto DSPTH0=!HSYNC;
			const auto DSPTH1=DSPTH0;
			const bool FIELD=false;   // What's FIELD?
			const bool VIN=false;
			data= (true==VIN ?    0x01 : 0)
			     |(true==HSYNC  ? 0x02 : 0)
			     |(true==VSYNC  ? 0x04 : 0)
			     |(true==FIELD  ? 0x08 : 0)
			     |(true==DSPTH0 ? 0x10 : 0)
			     |(true==DSPTH1 ? 0x20 : 0)
			     |(true==DSPTV0 ? 0x40 : 0)
			     |(true==DSPTV1 ? 0x80 : 0);
		}
		else
		{
			data=(state.crtcReg[state.crtcAddrLatch]>>8)&0xff;
		}
		break;

	case TOWNSIO_MX_HIRES://            0x470,
		data=(true==state.highResAvailable ? 0x7F : 0x80); // [2] pp. 831
		break;

	case TOWNSIO_MX_VRAMSIZE://         0x471,
		data=(TOWNSTYPE_2_MX<=townsPtr->townsType ? 0x01 : 0x00); // [2] pp. 831
		break;

	case TOWNSIO_MX_IMGOUT_ADDR_LOW://  0x472,
		data=(TOWNSTYPE_2_MX<=townsPtr->townsType ? (state.highResCrtcRegAddrLatch&255) : 0xff);
		break;

	case TOWNSIO_MX_IMGOUT_ADDR_HIGH:// 0x473,
		data=(TOWNSTYPE_2_MX<=townsPtr->townsType ? ((state.highResCrtcRegAddrLatch>>8)&255) : 0xff);
		break;

	case TOWNSIO_MX_IMGOUT_D0://   0x474,
		switch(state.highResCrtcRegAddrLatch)
		{
		case HIGHRES_REG_CTRL1:
			data=0;
			if(true==state.highResCrtcReg4Bit1)
			{
				data|=2;
			}
			if(true==state.highResCrtcReg4Bit0)
			{
				data|=1;
			}
			break;
		case HIGHRES_REG_VSYNC1:
			// Based on TBIOS disassembly.
			if(true==InVSYNC(townsPtr->state.townsTime))
			{
				data=2;
			}
			else
			{
				data=0;
			}
			break;
		case HIGHRES_REG_PALCOL:
			{
				switch(state.highResCrtcReg[HIGHRES_REG_PALSEL])
				{
				case 0:
					data=state.highResCrtcPalette.Get16(0,2);
					break;
				case 1:
					data=state.highResCrtcPalette.Get16(1,2);
					break;
				case 2:
					data=state.highResCrtcPalette.Get256(2);
					break;
				}
			}
			break;
		default:
			data=state.highResCrtcReg[state.highResCrtcRegAddrLatch];
			break;
		}
		break;
	case TOWNSIO_MX_IMGOUT_D1://   0x475,
		switch(state.highResCrtcRegAddrLatch)
		{
		case HIGHRES_REG_CTRL1:
			data=0;
			break;
		case HIGHRES_REG_PALCOL:
			{
				switch(state.highResCrtcReg[HIGHRES_REG_PALSEL])
				{
				case 0:
					data=state.highResCrtcPalette.Get16(0,0);
					break;
				case 1:
					data=state.highResCrtcPalette.Get16(1,0);
					break;
				case 2:
					data=state.highResCrtcPalette.Get256(0);
					break;
				}
			}
			break;
		default:
			data=((state.highResCrtcReg[state.highResCrtcRegAddrLatch]>>8)&0xFF);
			break;
		}
		break;
	case TOWNSIO_MX_IMGOUT_D2://   0x476,
		switch(state.highResCrtcRegAddrLatch)
		{
		case HIGHRES_REG_PALCOL:
			{
				switch(state.highResCrtcReg[HIGHRES_REG_PALSEL])
				{
				case 0:
					data=state.highResCrtcPalette.Get16(0,1);
					break;
				case 1:
					data=state.highResCrtcPalette.Get16(1,1);
					break;
				case 2:
					data=state.highResCrtcPalette.Get256(1);
					break;
				}
			}
			break;
		default:
			data=((state.highResCrtcReg[state.highResCrtcRegAddrLatch]>>16)&0xFF);
			break;
		}
		break;
	case TOWNSIO_MX_IMGOUT_D3://   0x477,
		data=((state.highResCrtcReg[state.highResCrtcRegAddrLatch]>>24)&0xFF);
		break;

	case TOWNSIO_HSYNC_VSYNC:
		data= (true==InVSYNC(townsPtr->state.townsTime) ? 1 : 0)
		     |(true==InHSYNC(townsPtr->state.townsTime) ? 2 : 0);
		break;
	case TOWNSIO_FMR_HSYNC_VSYNC: // 0xFF86
		data= (true==InVSYNC(townsPtr->state.townsTime) ? 0x04 : 0)
		     |(true==InHSYNC(townsPtr->state.townsTime) ? 0x80 : 0)
		     |0x10;
		break;
	}
	return data;
}

void TownsCRTC::WriteCR0(unsigned int data)
{
	if(true==state.highResAvailable)
	{
		state.highResCrtcReg4Bit1=true;
		if(0==(data&0x8000)) // Clear START bit, disable conventional CRTC, enable high-res CRTC
		{
			// Apparently High-Res CRTC is controlled only by HIGHRES_REG_CTRL0?
			// state.highResCRTCEnabled=true;
			state.highResCrtcReg4Bit0=true;
		}
		else
		{
			// Apparently High-Res CRTC is controlled only by HIGHRES_REG_CTRL0?
			// state.highResCRTCEnabled=false;
			state.highResCrtcReg4Bit0=false;
		}
	}
}

/* virtual */ void TownsCRTC::Reset(void)
{
	state.Reset();
}

Vec2i TownsCRTC::GetRenderSize(void) const
{
	const int minHei=402;  // Reading mackerel for 2 pixels.
	// What do I mean by 'read mackerel'?  Translate to Japanese and back to English.
	// Well, google translate didn't work.  Use something better.
	if(true!=state.highResCRTCEnabled)
	{
		const int maxHei=512;
		// Height still has errors.  Some 320x240 mode returns 320x880 size.  (Damn it!  I forgot which one.)

		// Raiden uses 256x256 (2X scale) mode.  But, if I make max height 512 pixels, it ends up showing
		// bottom two lines that are not supposed to be visible.  Do I have to make it 508 pixels (254 pixels if 2X) if sprite is ON?
		// I could not test it on my real FM TOWNS II MX because my LCD monitor cropped at Y=480 anyway in this frequency.

		if(LowResCrtcIsInSinglePageMode())
		{
			auto dim=GetPageSizeOnMonitor(0);
			dim[1]+=GetPageOriginOnMonitor(0).y();
			unsigned int hei=std::min(maxHei,std::max(dim.y(),minHei));
			return Vec2i::Make(std::max(640,dim.x()),hei);
		}
		else
		{
			auto dim0=GetPageSizeOnMonitor(0);
			auto dim1=GetPageSizeOnMonitor(1);

			// Unless I add origin y, Galaxy Force II is chopped off the bottom part.
			dim0[1]+=GetPageOriginOnMonitor(0).y();
			dim1[1]+=GetPageOriginOnMonitor(1).y();

			auto wid=std::max(dim0.x(),dim1.x());
			auto hei=std::max(dim0.y(),dim1.y());
			hei=std::min(maxHei,std::max(hei,minHei));
			return Vec2i::Make(std::max(640,wid),hei);
		}
	}
	else
	{
		auto dim=GetHighResDisplaySize();
		return Vec2i::Make(std::max(640,dim.x()),std::max(minHei,dim.y()));
	}
}

std::vector <std::string> TownsCRTC::GetStatusText(void) const
{
	std::vector <std::string> text;
	const char *const regLabel[]=
	{
		"HSW1",
		"HSW2",
		"----",
		"----",
		"HST ",
		"VST1",
		"VST2",
		"EET ",
		"VST ",
		"HDS0",
		"HDE0",
		"HDS1",
		"HDE1",
		"VDS0",
		"VDE0",
		"VDS1",
		"VDE1",
		"FA0 ",
		"HAJ0",
		"FO0 ",
		"LO0 ",
		"FA1 ",
		"HAJ1",
		"FO1 ",
		"LO1 ",
		"EHAJ",
		"EVAJ",
		"ZOOM",
		"CR0 ",
		"CR1 ",
		"FR  ",
		"CR2 ",
	};

	const unsigned int regTable[]=
	{
		REG_HSW1, //   0x00,
		REG_VST1, //   0x05,
		REG_HDS0, //   0x09,
		REG_HDE0, //   0x0A,
		REG_VDS0, //   0x0D,
		REG_VDE0, //   0x0E,
		REG_FA0, //    0x11,
		REG_HAJ0, //   0x12,
		REG_FO0, //    0x13,
		REG_LO0, //    0x14,


		REG_HSW2, //   0x01,
		REG_VST2, //   0x06,
		REG_HDS1, //   0x0B,
		REG_HDE1, //   0x0C,
		REG_VDS1, //   0x0F,
		REG_VDE1, //   0x10,
		REG_FA1, //    0x15,
		REG_HAJ1, //   0x16,
		REG_FO1, //    0x17,
		REG_LO1, //    0x18,

		REG_HST, //    0x04,
		REG_EET, //    0x07,
		REG_VST, //    0x08,
		REG_EHAJ, //   0x19,
		REG_EVAJ, //   0x1A,
		REG_ZOOM, //   0x1B,
		REG_CR0, //    0x1C,
		REG_CR1, //    0x1D,
		REG_CR2, //    0x1F,
		REG_FR, //     0x1E,

		REG_UNUSED1, //0x02,
		REG_UNUSED2, //0x03,
	};

	text.push_back("");
	text.back()=cpputil::Itoa(GetHorizontalFrequency());
	text.back()+="KHz";

	text.push_back("");
	text.back()="Registers:";
	for(int i=0; i<sizeof(regTable)/sizeof(regTable[0]); ++i)
	{
		if(0==i%10)
		{
			text.push_back("");
		}
		else
		{
			text.back()+=" ";
		}
		auto idx=regTable[i];
		text.back()+=regLabel[idx];
		text.back()+=":";
		text.back()+=cpputil::Ustox(state.crtcReg[idx]);
	}

	text.push_back("");
	text.back()="Sifters (Isn't it Shifter?):";
	for(int i=0; i<2; ++i)
	{
		text.back()+=cpputil::Ubtox(state.sifter[i]);
		text.back().push_back(' ');
	}
	text.back()+="PLT:";
	text.back()+=cpputil::Ubtox((state.sifter[1]>>4)&3);

	text.back()+="  Priority:";
	text.back().push_back('0'+GetPriorityPage());

	text.back()+="  CLKSEL:";
	text.back().push_back('0'+(state.crtcReg[REG_CR1]&3));

	text.back()+="  SCSEL:";
	text.back().push_back('0'+((state.crtcReg[REG_CR1]&0x0C)>>2));
	text.back()+="("+cpputil::Ubtox(GetBaseClockScaler())+"x)";

	text.back()+="  PM:";
	text.back()+=cpputil::Ustox(state.crtcReg[REG_CR2]&0x1ff);

	text.push_back("");
	text.back()="Address Latch: ";
	text.back()+=cpputil::Uitox(state.crtcAddrLatch)+"H";

	const unsigned int CL[2]=
	{
		(unsigned int)( state.crtcReg[REG_CR0]&3),
		(unsigned int)((state.crtcReg[REG_CR0]>>2)&3),
	};
	const unsigned int PMODE=((state.crtcReg[REG_CR0]>>4)&1);

	text.push_back("");
	text.back()="CL0:"+cpputil::Itoa(CL[0])+"  CL1:"+cpputil::Itoa(CL[1]);

	if(true==LowResCrtcIsInSinglePageMode())
	{
		text.push_back("");
		text.back()="Single-Page Mode.  ";

		Layer layer;
		MakeLowResPageLayerInfo(layer,0);

		auto pageStat0=GetPageStatusText(layer);
		text.insert(text.end(),pageStat0.begin(),pageStat0.end());
	}
	else
	{
		text.push_back("");
		text.back()="2-Page Mode.  ";

		Layer layer[2];
		MakeLowResPageLayerInfo(layer[0],0);
		MakeLowResPageLayerInfo(layer[1],1);

		text.push_back("Page 0");
		auto pageStat0=GetPageStatusText(layer[0]);
		text.insert(text.end(),pageStat0.begin(),pageStat0.end());

		text.push_back("Page 1");
		auto pageStat1=GetPageStatusText(layer[1]);
		text.insert(text.end(),pageStat1.begin(),pageStat1.end());
	}

	text.push_back("");
	text.back()+="VSYNC:";
	text.back()+=cpputil::BoolToChar(state.VSYNC);

	text.push_back("");
	text.back()+="Show Page (FDA0H):";
	text.back()+=cpputil::Itoa(state.showPageFDA0[0])+" "+cpputil::Itoa(state.showPageFDA0[1]);

	text.push_back("");
	text.back()+="Show Page (0448H):";
	text.back()+=cpputil::Itoa(state.sifter[0]&3)+" "+cpputil::Itoa((state.sifter[0]>>2)&3);

	text.push_back("");
	text.back()+="FMR VRAM Display Offset:"+cpputil::Uitox(state.FMRVRAMOffset);

	text.push_back("");
	text.back()+="FMR VRAM Display Planes:"+cpputil::Ubtox(state.FMRGVRAMDisplayPlanes);

	return text;
}

std::vector <std::string> TownsCRTC::GetPageStatusText(const Layer &layer) const
{
	std::vector <std::string> text;

	text.push_back("");
	text.back()+="Top-Left:("+cpputil::Itoa(layer.originOnMonitor.x())+","+cpputil::Itoa(layer.originOnMonitor.y())+")  ";
	text.back()+="Display Size:("+cpputil::Itoa(layer.sizeOnMonitor.x())+","+cpputil::Itoa(layer.sizeOnMonitor.y())+")";

	text.push_back("");
	text.back()+=cpputil::Itoa(layer.bitsPerPixel)+"-bit color";

	text.push_back("");
	text.back()+="VRAM Base="+cpputil::Uitox(layer.VRAMAddr);
	text.back()+="  Offset="+cpputil::Uitox(layer.VRAMOffset);

	text.push_back("");
	text.back()+="BytesPerLine="+cpputil::Uitox(layer.bytesPerLine);
	text.back()+="  SkipBytes="+cpputil::Uitox(layer.VRAMHSkipBytes);

	text.push_back("");
	text.back()+="Zoom=(";
	text.back()+=cpputil::Itoa(layer.zoom2x.x()/2);
	text.back()+=((layer.zoom2x.x()&1) ? ".5" : "");
	text.back()+=",";
	text.back()+=cpputil::Itoa(layer.zoom2x.y()/2);
	text.back()+=((layer.zoom2x.y()&1) ? ".5" : "");
	text.back()+=")";

	return text;
}
std::vector <std::string> TownsCRTC::GetRegisterDumpText(void) const
{
	std::vector <std::string> txt;
	for(auto reg : state.crtcReg)
	{
		txt.push_back(cpputil::Ustox(reg));
	}
	for(auto reg : state.sifter)
	{
		txt.push_back(cpputil::Ubtox(reg));
	}
	return txt;
}
std::vector <std::string> TownsCRTC::GetPaletteText(void) const
{
	return GetLowResPaletteText(state.palette);
}

std::vector <std::string> TownsCRTC::GetLowResPaletteText(const AnalogPalette &palette) const
{
	std::vector <std::string> text;

	text.push_back("");
	text.back()+="256-Color Palette";
	for(int i=0; i<256; i+=16)
	{
		text.push_back("");
		for(int j=0; j<16; ++j)
		{
			text.back()+=cpputil::Ubtox(palette.plt256[i+j][0]);
			text.back()+=cpputil::Ubtox(palette.plt256[i+j][1]);
			text.back()+=cpputil::Ubtox(palette.plt256[i+j][2]);
			text.back().push_back(' ');
		}
	}

	text.push_back("");
	text.back()+="16-Color Palette";
	for(int page=0; page<2; ++page)
	{
		text.push_back("");
		text.back()+="Page";
		text.back().push_back((char)('0'+page));
		text.back().push_back(':');
		for(int i=0; i<16; ++i)
		{
			text.back()+=cpputil::Ubtox(palette.plt16[page][i][0]);
			text.back()+=cpputil::Ubtox(palette.plt16[page][i][1]);
			text.back()+=cpputil::Ubtox(palette.plt16[page][i][2]);
			text.back().push_back(' ');
		}
	}

	return text;
}

void TownsCRTC::MakeHighResPageLayerInfo(Layer &layer,unsigned char page) const
{
	switch(state.highResCrtcReg[HIGHRES_REG_P0_PALETTE+0x10*page]&0xFFFF)
	{
	default:
	case 0x0F:
		layer.bitsPerPixel=4;
		break;
	case 0xFF:
		layer.bitsPerPixel=8;
		break;
	case 0x8000:
		layer.bitsPerPixel=16;
		break;
	case 0xFFFF:
		layer.bitsPerPixel=24;
		break;
	}
	layer.highResRGBSwap=state.highResCrtcReg[HIGHRES_REG_RGB_BGR_BRG_OR_ELSE]; // Hopefully meaningful only in the 24-bit color mode.  Figured 2025/07/02
	layer.VRAMAddr=0x80000*page;

	unsigned int dx=state.highResCrtcReg[HIGHRES_REG_P0_VRAM_OFFSET_X+0x10*page];
	unsigned int dy=state.highResCrtcReg[HIGHRES_REG_P0_VRAM_OFFSET_Y+0x10*page];
	unsigned int vramWidInPix=state.highResCrtcReg[HIGHRES_REG_P0_VRAM_WID+0x10*page];

	layer.VRAMOffset=(dy*vramWidInPix+dx)*layer.bitsPerPixel/8;
	layer.FlipVRAMOffset =0;        // Probably FM-R/Sprite page is not applicable to CRTC2.
	layer.FMRGVRAMMask=0x0F;        // Probably mask is not applicable to CRTC2.
	layer.originOnMonitor=GetHighResPageOriginOnMonitor(page);
	layer.VRAMHSkipBytes=0;
	layer.sizeOnMonitor=GetHighResDisplaySize();
	layer.VRAMCoverage1X=layer.sizeOnMonitor;
	layer.zoom2x=GetHighResPageZoom2X(page);
	layer.bytesPerLine=vramWidInPix*layer.bitsPerPixel/8;

	if(true==cpputil::Is2toN(layer.bytesPerLine))
	{
		layer.HScrollMask=layer.bytesPerLine-1;
	}
	else
	{
		layer.HScrollMask=0xFFFFFFFF;
	}
	if(true==HighResCrtcIsInSinglePageMode())
	{
		layer.VScrollMask=TOWNS_VRAM_SIZE-1;
	}
	else
	{
		layer.VScrollMask=TOWNS_VRAM_SIZE/2-1;
	}
}
bool TownsCRTC::HighResCrtcIsInSinglePageMode(void) const
{
	return (0==(state.highResCrtcReg[HIGHRES_REG_PGCTRL]&2));
}
Vec2i TownsCRTC::GetHighResPageOriginOnMonitor(unsigned char page) const
{
	return Vec2i::Make(0,0); // Tentative
}
Vec2i TownsCRTC::GetHighResPageZoom2X(unsigned char page) const
{
	auto zoomReg=state.highResCrtcReg[HIGHRES_REG_P0_ZOOM+0x10*page];
	return Vec2i::Make((1+(zoomReg&0xff))*2,(1+((zoomReg>>8)&0xff))*2);
}
Vec2i TownsCRTC::GetHighResDisplaySize(void) const
{
	return Vec2i::Make(state.highResCrtcReg[HIGHRES_REG_XEND]-state.highResCrtcReg[HIGHRES_REG_XSTART],state.highResCrtcReg[HIGHRES_REG_YEND]-state.highResCrtcReg[HIGHRES_REG_YSTART]);
}
std::vector <std::string> TownsCRTC::GetHighResStatusText(void) const
{
	std::vector <std::string> text;
	text.push_back(true==state.highResCRTCEnabled ? "HighRes CRTC Enabled" : "HighRes CRTC Disabled");

	text.push_back("Reg 4 ");
	text.back()+="bit0=";
	text.back().push_back('0'+state.highResCrtcReg4Bit0);
	text.back()+="  bit1=";
	text.back().push_back('0'+state.highResCrtcReg4Bit1);

	text.push_back(true==HighResCrtcIsInSinglePageMode() ? "Single-Layer Mode" : "Two-Layer Mode");

	Layer layer[2];
	MakeHighResPageLayerInfo(layer[0],0);
	MakeHighResPageLayerInfo(layer[1],1);

	auto page0Info=GetPageStatusText(layer[0]);
	auto page1Info=GetPageStatusText(layer[1]);

	text.push_back("Page 0");
	text.insert(text.end(),page0Info.begin(),page0Info.end());
	if(true!=HighResCrtcIsInSinglePageMode())
	{
		text.push_back("Page 1");
		text.insert(text.end(),page1Info.begin(),page1Info.end());
	}

	return text;
}
std::vector <std::string> TownsCRTC::GetHighResPaletteText(void) const
{
	return GetLowResPaletteText(state.highResCrtcPalette);
}


void TownsCRTC::AnalogPalette::Serialize(std::vector <unsigned char> &data) const
{
	PushUint32(data,codeLatch);
	for(int i=0; i<2; ++i)
	{
		for(int j=0; j<16; ++j)
		{
			uint32_t col;
			col=(plt16[i][j][2]<<16)|(plt16[i][j][1]<<8)|plt16[i][j][0];
			PushUint32(data,col);
		}
	}
	for(auto p : plt256)
	{
		uint32_t col;
		col=(p[2]<<16)|(p[1]<<8)|p[0];
		PushUint32(data,col);
	}
}
void TownsCRTC::AnalogPalette::Deserialize(const unsigned char *&data)
{
	codeLatch=ReadUint32(data);
	for(int i=0; i<2; ++i)
	{
		for(int j=0; j<16; ++j)
		{
			uint32_t col=ReadUint32(data);
			plt16[i][j][2]=(col>>16)&255;
			plt16[i][j][1]=(col>>8)&255;
			plt16[i][j][0]=col&255;
		}
	}
	for(auto &p : plt256)
	{
		uint32_t col=ReadUint32(data);
		p[2]=(col>>16)&255;
		p[1]=(col>>8)&255;
		p[0]=col&255;
	}
}

/* virtual */ uint32_t TownsCRTC::SerializeVersion(void) const
{
	// Version 1 Added High-Res CRTC Hardware Mouse Cursor.
	// Version 2 Added highResCrtcReg4Bit0.
	return 2;
}
/* virtual */ void TownsCRTC::SpecificSerialize(std::vector <unsigned char> &data,std::string) const
{
	PushBool(data,state.VSYNCIRQ);;
	PushBool(data,state.VSYNC);;

	for(auto r : state.crtcReg)
	{
		PushUint16(data,r);
	}
	PushUint32(data,state.crtcAddrLatch);

	PushBool(data,state.DPMD); // Digital-Palette Modify Flag
	for(auto p : state.FMRPalette)
	{
		PushUint32(data,p);
	}

	for(auto s : state.sifter)
	{
		PushUint16(data,s);
	}
	PushUint32(data,state.sifterAddrLatch);

	state.palette.Serialize(data);

	PushBool(data,state.highResAvailable);
	PushBool(data,state.highResCRTCEnabled);
	for(auto r : state.highResCrtcReg)
	{
		PushUint32(data,r);
	}
	PushUint32(data,state.highResCrtcRegAddrLatch);
	PushBool(data,state.highResCrtcReg4Bit0);
	PushBool(data,state.highResCrtcReg4Bit1);
	PushUint32(data,state.highResPaletteMode);
	PushUint32(data,state.highResPaletteLatch);

	state.highResCrtcPalette.Serialize(data);

	PushUint32(data,state.FMRGVRAMDisplayPlanes);

	PushUint32(data,state.FMRVRAMOffset);
	PushBool(data,state.showPageFDA0[0]);
	PushBool(data,state.showPageFDA0[1]);
	PushBool(data,state.showPage0448[0]);
	PushBool(data,state.showPage0448[1]);

	// Version 1
	PushBool(data,state.highResCrtcMouse.defining);
	PushBool(data,state.highResCrtcMouse.defined);
	PushUint32(data,state.highResCrtcMouse.ptnCount);
	PushUint32(data,state.highResCrtcMouse.unknownValueReg8);
	PushUint32(data,state.highResCrtcMouse.X);
	PushUint32(data,state.highResCrtcMouse.Y);
	PushUint32(data,state.highResCrtcMouse.originX);
	PushUint32(data,state.highResCrtcMouse.originY);
	PushUcharArray(data,512,state.highResCrtcMouse.ANDPtn);
	PushUcharArray(data,512,state.highResCrtcMouse.ORPtn);
}
/* virtual */ bool TownsCRTC::SpecificDeserialize(const unsigned char *&data,std::string,uint32_t version)
{
	state.VSYNCIRQ=ReadBool(data);;
	state.VSYNC=ReadBool(data);;

	for(auto &r : state.crtcReg)
	{
		r=ReadUint16(data);
	}
	state.crtcAddrLatch=ReadUint32(data);

	state.DPMD=ReadBool(data); // Digital-Palette Modify Flag
	for(auto &p : state.FMRPalette)
	{
		p=ReadUint32(data);
	}

	for(auto &s : state.sifter)
	{
		s=ReadUint16(data);
	}
	state.sifterAddrLatch=ReadUint32(data);

	state.palette.Deserialize(data);

	state.highResAvailable=ReadBool(data);
	state.highResCRTCEnabled=ReadBool(data);
	for(auto &r : state.highResCrtcReg)
	{
		r=ReadUint32(data);
	}
	state.highResCrtcRegAddrLatch=ReadUint32(data);
	if(2<=version)
	{
		state.highResCrtcReg4Bit0=ReadBool(data);
	}
	else
	{
		state.highResCrtcReg4Bit0=state.highResCRTCEnabled;
	}
	state.highResCrtcReg4Bit1=ReadBool(data);
	state.highResPaletteMode=ReadUint32(data);
	state.highResPaletteLatch=ReadUint32(data);

	state.highResCrtcPalette.Deserialize(data);

	state.FMRGVRAMDisplayPlanes=ReadUint32(data);

	state.FMRVRAMOffset=ReadUint32(data);
	state.showPageFDA0[0]=ReadBool(data);
	state.showPageFDA0[1]=ReadBool(data);
	state.showPage0448[0]=ReadBool(data);
	state.showPage0448[1]=ReadBool(data);

	if(1<=version)
	{
		// Version 1
		state.highResCrtcMouse.defining=ReadBool(data);
		state.highResCrtcMouse.defined=ReadBool(data);
		state.highResCrtcMouse.ptnCount=ReadUint32(data);
		state.highResCrtcMouse.unknownValueReg8=ReadUint32(data);
		state.highResCrtcMouse.X=ReadUint32(data);
		state.highResCrtcMouse.Y=ReadUint32(data);
		state.highResCrtcMouse.originX=ReadUint32(data);
		state.highResCrtcMouse.originY=ReadUint32(data);
		ReadUcharArray(data,512,state.highResCrtcMouse.ANDPtn);
		ReadUcharArray(data,512,state.highResCrtcMouse.ORPtn);
	}
	else
	{
		state.highResCrtcMouse.Reset();
	}

	return true;
}
