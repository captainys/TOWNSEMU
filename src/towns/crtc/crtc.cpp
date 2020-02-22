#include <iostream>
#include <algorithm>

#include "cpputil.h"
#include "crtc.h"
#include "towns.h"
#include "townsdef.h"



void TownsCRTC::State::Reset(void)
{
	for(auto &d : crtcReg)
	{
		d=0;
	}
	crtcAddrLatch=0;

	for(auto &d : sifter)
	{
		d=0;
	}
	sifterAddrLatch=0;

	for(auto &d : mxVideoOutCtrl)
	{
		d=0;
	}
	mxVideoOutCtrlAddrLatch=0;
}

TownsCRTC::ScreenModeCache::ScreenModeCache()
{
	MakeFMRCompatible();
}

void TownsCRTC::ScreenModeCache::MakeFMRCompatible(void)
{
	numLayers=2;

	layer[0].mode=1;
	layer[0].bitsPerPixel=4;
	layer[0].virtualWid=640;
	layer[0].virtualHei=400;
	layer[0].visibleWid=640;
	layer[0].visibleHei=400;
	layer[0].bytesPerLine=320;

	layer[1].mode=4;
	layer[1].bitsPerPixel=4;
	layer[1].virtualWid=1024;
	layer[1].virtualHei=512;
	layer[1].visibleWid=640;
	layer[1].visibleHei=400;
	layer[1].bytesPerLine=512;
}

////////////////////////////////////////////////////////////


TownsCRTC::TownsCRTC(class FMTowns *ptr)
{
	townsPtr=ptr;
	state.mxVideoOutCtrl.resize(0x10000);
	state.Reset();

	// Tentatively
	cached=true;
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
	auto intoFrame=townsTime%16700000;
	return  (15360000<intoFrame);
}
bool TownsCRTC::InHSYNC(const unsigned long long int townsTime) const
{
	auto intoFrame=townsTime%16700000;
	if(intoFrame<1536000)
	{
		auto intoLine=intoFrame%32000;
		return (30000<intoLine);
	}
	return false;
}

bool TownsCRTC::InSinglePageMode(void) const
{
	return (0==(state.sifter[0]&0x10));
}

unsigned int TownsCRTC::GetBaseClockFreq(void) const
{
	auto CLKSEL=state.crtcReg[REG_CR1]&3;
	static const unsigned int freqTable[4]=
	{
		28636300,
		24545400,
		25175000,
		21052500,
	};
	return freqTable[CLKSEL];
}
unsigned int TownsCRTC::GetBaseClockScaler(void) const
{
	auto SCSEL=state.crtcReg[REG_CR1]&0x0C;
	return (SCSEL>>1)+2;
}
unsigned int TownsCRTC::GetPageZoomV(unsigned char page) const
{
	auto reg=state.crtcReg[REG_ZOOM];
	reg>>=(4+(page<<3));
	return (reg&0x0F)+1;
}
unsigned int TownsCRTC::GetPageZoomH(unsigned char page) const
{
	auto reg=state.crtcReg[REG_ZOOM];
	reg>>=(page<<3);
	return (reg&0x0F)+1;
}
Vec2i TownsCRTC::GetTopLeftCorner(unsigned char page) const
{
	page;
	return Vec2i::Origin();
}
Vec2i TownsCRTC::GetDisplaySize(unsigned char page) const
{
	auto wid= state.crtcReg[REG_HDE0+page*2]-state.crtcReg[REG_HDS0+page*2];
	auto hei=(state.crtcReg[REG_VDE0+page*2]-state.crtcReg[REG_VDS0+page*2])>>1;
	return Vec2i::Make(wid,hei);
}
unsigned int TownsCRTC::GetPageBitsPerPixel(unsigned char page) const
{
	const unsigned int CL=(state.crtcReg[REG_CR0]>>(page*2))&3;
	if(true==InSinglePageMode())
	{
		if(2==CL)
		{
			return 16;
		}
		else if(3==CL)
		{
			return 256;
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
	std::cout << __FUNCTION__ << std::endl;
	std::cout << "Unknown color setting." << std::endl;
	return 4; // What else can I do?
}
unsigned int TownsCRTC::GetBytesPerLine(unsigned char page) const
{
	auto FOx=state.crtcReg[REG_FO0+page*4];
	auto LOx=state.crtcReg[REG_LO0+page*4];
	auto numBytes=(LOx-FOx)*4;
	if(true==InSinglePageMode())
	{
		numBytes*=2;
	}
	return numBytes;
}


/* virtual */ void TownsCRTC::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_CRTC_ADDRESS://             0x440,
		state.crtcAddrLatch=data&0x1f;
		break;
	case TOWNSIO_CRTC_DATA_LOW://            0x442,
		state.crtcReg[state.crtcAddrLatch]&=0xff00;
		state.crtcReg[state.crtcAddrLatch]|=(data&0xff);
		break;
	case TOWNSIO_CRTC_DATA_HIGH://           0x443,
		state.crtcReg[state.crtcAddrLatch]&=0x00ff;
		state.crtcReg[state.crtcAddrLatch]|=((data&0xff)<<8);
		break;


	case TOWNSIO_VIDEO_OUT_CTRL_ADDRESS://=   0x448,
		state.sifterAddrLatch=(data&3);
		break;
	case TOWNSIO_VIDEO_OUT_CTRL_DATA://=      0x44A,
		state.sifter[state.sifterAddrLatch]=data;
		break;


	case TOWNSIO_MX_HIRES://            0x470,
	case TOWNSIO_MX_VRAMSIZE://         0x471,
		break; // No write access;

	case TOWNSIO_MX_IMGOUT_ADDR_LOW://  0x472,
		state.mxVideoOutCtrlAddrLatch=((state.mxVideoOutCtrlAddrLatch&0xff00)|(data&0xff));
		break;
	case TOWNSIO_MX_IMGOUT_ADDR_HIGH:// 0x473,
		state.mxVideoOutCtrlAddrLatch=((state.mxVideoOutCtrlAddrLatch&0x00ff)|((data<<8)&0xff));
		break;
	case TOWNSIO_MX_IMGOUT_ADDR_D0://   0x474,
		std::cout << "MX-VIDOUTCONTROL8[" << cpputil::Ustox(state.mxVideoOutCtrlAddrLatch) << "H]=" << cpputil::Ubtox(data) << "H" << std::endl;
		state.mxVideoOutCtrl[state.mxVideoOutCtrlAddrLatch]=data;
		break;
	case TOWNSIO_MX_IMGOUT_ADDR_D1://   0x475,
		if(state.mxVideoOutCtrlAddrLatch+1<state.mxVideoOutCtrl.size())
		{
			std::cout << "MX-VIDOUTCONTROL8[" << cpputil::Ustox(state.mxVideoOutCtrlAddrLatch+1) << "H]=" << cpputil::Ubtox(data) << "H" << std::endl;
			state.mxVideoOutCtrl[state.mxVideoOutCtrlAddrLatch+1]=data;
		}
		break;
	case TOWNSIO_MX_IMGOUT_ADDR_D2://   0x476,
		if(state.mxVideoOutCtrlAddrLatch+2<state.mxVideoOutCtrl.size())
		{
			std::cout << "MX-VIDOUTCONTROL8[" << cpputil::Ustox(state.mxVideoOutCtrlAddrLatch+2) << "H]=" << cpputil::Ubtox(data) << "H" << std::endl;
			state.mxVideoOutCtrl[state.mxVideoOutCtrlAddrLatch+2]=data;
		}
		break;
	case TOWNSIO_MX_IMGOUT_ADDR_D3://   0x477,
		if(state.mxVideoOutCtrlAddrLatch+3<state.mxVideoOutCtrl.size())
		{
			std::cout << "MX-VIDOUTCONTROL8[" << cpputil::Ustox(state.mxVideoOutCtrlAddrLatch+3) << "H]=" << cpputil::Ubtox(data) << "H" << std::endl;
			state.mxVideoOutCtrl[state.mxVideoOutCtrlAddrLatch+3]=data;
		}
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
		state.crtcReg[state.crtcAddrLatch]=(data&0xfff);
		break;
	case TOWNSIO_CRTC_DATA_HIGH://           0x443,
		break;

	case TOWNSIO_VIDEO_OUT_CTRL_ADDRESS://=   0x448,
		state.sifterAddrLatch=(data&3);
		break;
	case TOWNSIO_VIDEO_OUT_CTRL_DATA://=      0x44A,
		state.sifter[state.sifterAddrLatch]=data;
		break;

	case TOWNSIO_MX_HIRES://            0x470,
	case TOWNSIO_MX_VRAMSIZE://         0x471,
		break; // No write access;

	case TOWNSIO_MX_IMGOUT_ADDR_LOW://  0x472,
		state.mxVideoOutCtrlAddrLatch=(data&0xffff);
		break;
	case TOWNSIO_MX_IMGOUT_ADDR_D0://   0x474,
		std::cout << "MX-VIDOUTCONTROL16[" << cpputil::Ustox(state.mxVideoOutCtrlAddrLatch) << "H]=" << cpputil::Ustox(data) << "H" << std::endl;
		state.mxVideoOutCtrl[state.mxVideoOutCtrlAddrLatch]=data;
		state.mxVideoOutCtrl[state.mxVideoOutCtrlAddrLatch+1]=(data>>8)&255;
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
	case TOWNSIO_MX_IMGOUT_ADDR_D0://   0x474,
		std::cout << "MX-VIDOUTCONTROL32[" << cpputil::Ustox(state.mxVideoOutCtrlAddrLatch) << "H]=" << cpputil::Uitox(data) << "H" << std::endl;
		state.mxVideoOutCtrl[state.mxVideoOutCtrlAddrLatch]=data;
		state.mxVideoOutCtrl[state.mxVideoOutCtrlAddrLatch+1]=(data>>8)&255;
		state.mxVideoOutCtrl[state.mxVideoOutCtrlAddrLatch+2]=(data>>16)&255;
		state.mxVideoOutCtrl[state.mxVideoOutCtrlAddrLatch+3]=(data>>24)&255;
		break;
	default:
		Device::IOWriteWord(ioport,data); // Let it write 4 times.
		break;
	}
}

/* virtual */ unsigned int TownsCRTC::IOReadByte(unsigned int ioport)
{
	unsigned char data=0xff;
	switch(ioport)
	{
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
		data=(TOWNSTYPE_2_MX<=townsPtr->townsType ? 0x7F : 0x80); // [2] pp. 831
		break;

	case TOWNSIO_MX_VRAMSIZE://         0x471,
		data=(TOWNSTYPE_2_MX<=townsPtr->townsType ? 0x01 : 0x00); // [2] pp. 831
		break;

	case TOWNSIO_MX_IMGOUT_ADDR_LOW://  0x472,
		data=(TOWNSTYPE_2_MX<=townsPtr->townsType ? (state.mxVideoOutCtrlAddrLatch&255) : 0xff);
		break;

	case TOWNSIO_MX_IMGOUT_ADDR_HIGH:// 0x473,
		data=(TOWNSTYPE_2_MX<=townsPtr->townsType ? ((state.mxVideoOutCtrlAddrLatch>>8)&255) : 0xff);
		break;

	case TOWNSIO_MX_IMGOUT_ADDR_D0://   0x474,
		switch(state.mxVideoOutCtrlAddrLatch)
		{
		case 0x0004:
			data=0;
			break;
		}
		break;
	case TOWNSIO_MX_IMGOUT_ADDR_D1://   0x475,
		break;
	case TOWNSIO_MX_IMGOUT_ADDR_D2://   0x476,
		break;
	case TOWNSIO_MX_IMGOUT_ADDR_D3://   0x477,
		break;

	case TOWNSIO_HSYNC_VSYNC:
		data= (true==InVSYNC(townsPtr->state.townsTime) ? 1 : 0)
		     |(true==InHSYNC(townsPtr->state.townsTime) ? 2 : 0);
		break;
	}
	return data;
}

/* virtual */ void TownsCRTC::Reset(void)
{
	state.Reset();
}

void TownsCRTC::GetRenderSize(unsigned int &wid,unsigned int &hei) const
{
	if(1==cache.numLayers)
	{
		wid=cache.layer[0].visibleWid;
		hei=cache.layer[0].visibleHei;
	}
	else
	{
		wid=std::max(cache.layer[0].visibleWid,cache.layer[1].visibleWid);
		hei=std::max(cache.layer[0].visibleHei,cache.layer[1].visibleHei);
	}
}

std::vector <std::string> TownsCRTC::GetStatusText(void) const
{
	std::vector <std::string> text;
	std::string empty;

	text.push_back(empty);
	text.back()="Registers:";
	for(int i=0; i<sizeof(state.crtcReg)/sizeof(state.crtcReg[0]); ++i)
	{
		if(0==i%16)
		{
			text.push_back(empty);
			text.back()+="REG";
			text.back()+=cpputil::Ubtox(i);
			text.back()+=":";
		}
		text.back()+=" ";
		text.back()+=cpputil::Ustox(state.crtcReg[i]);
	}

	text.push_back(empty);
	text.back()="Address Latch: ";
	text.back()+=cpputil::Uitox(state.crtcAddrLatch)+"H";

	const unsigned int CL[2]=
	{
		(unsigned int)( state.crtcReg[REG_CR0]&3),
		(unsigned int)((state.crtcReg[REG_CR0]>>2)&3),
	};

	text.push_back(empty);
	text.back()="CL0:"+cpputil::Itoa(CL[0])+"  CL1:"+cpputil::Itoa(CL[1]);

	if(true==InSinglePageMode())
	{
		text.push_back(empty);
		text.back()="Single-Page Mode.  ";

		auto pageStat0=GetPageStatusText(0);
		text.insert(text.end(),pageStat0.begin(),pageStat0.end());
	}
	else
	{
		text.push_back(empty);
		text.back()="2-Page Mode.  ";

		auto pageStat0=GetPageStatusText(0);
		text.insert(text.end(),pageStat0.begin(),pageStat0.end());

		auto pageStat1=GetPageStatusText(1);
		text.insert(text.end(),pageStat1.begin(),pageStat1.end());
	}

	

	return text;
}

std::vector <std::string> TownsCRTC::GetPageStatusText(int page) const
{
	std::vector <std::string> text;
	std::string empty;

	text.push_back(empty);
	text.back()="Page "+cpputil::Itoa(page);

	text.push_back(empty);
	auto dim=GetDisplaySize(0);
	text.back()+="Display Size:("+cpputil::Itoa(dim.x())+","+cpputil::Itoa(dim.y())+")";

	text.push_back(empty);
	text.back()+=cpputil::Itoa(1<<GetPageBitsPerPixel(page))+" colors";

	return text;
}
