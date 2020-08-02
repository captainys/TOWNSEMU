/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <cstring>
#include "cpputil.h"
#include "render.h"



TownsRender::TownsRender()
{
	wid=0;
	hei=0;
}
void TownsRender::Create(int wid,int hei)
{
	this->wid=wid;
	this->hei=hei;
	rgba.resize(4*wid*hei);
}
void TownsRender::BuildImage(const TownsCRTC &crtc,const TownsPhysicalMemory &physMem)
{
	auto renderSize=crtc.GetRenderSize();
	SetResolution(renderSize.x(),renderSize.y());

	if(true!=crtc.cached)
	{
		// Cache crtc setting.
	}

	std::memset(rgba.data(),0,rgba.size());

	if(true==crtc.InSinglePageMode())
	{
		if(true==crtc.state.ShowPage(0))
		{
			TownsCRTC::Layer layer;
			crtc.MakePageLayerInfo(layer,0);
			Render<VRAM1Trans>(0,layer,crtc.state.palette,crtc.chaseHQPalette,physMem.state.VRAM,false);
		}
	}
	else
	{
		TownsCRTC::Layer layer[2];
		crtc.MakePageLayerInfo(layer[0],0);
		crtc.MakePageLayerInfo(layer[1],1);
		auto priorityPage=crtc.GetPriorityPage();
		if(true==crtc.state.ShowPage(1-priorityPage))
		{
			Render<VRAM0Trans>(1-priorityPage,layer[1-priorityPage],crtc.state.palette,crtc.chaseHQPalette,physMem.state.VRAM,false);
		}
		if(true==crtc.state.ShowPage(priorityPage))
		{
			Render<VRAM0Trans>(priorityPage,  layer[priorityPage]  ,crtc.state.palette,crtc.chaseHQPalette,physMem.state.VRAM,true);
		}
	}
}
void TownsRender::SetResolution(int wid,int hei)
{
	if(wid!=this->wid || hei!=this->hei)
	{
		Create(wid,hei);
	}
}
TownsRender::Image TownsRender::GetImage(void) const
{
	Image img;
	img.wid=wid;
	img.hei=hei;
	img.rgba=this->rgba.data();
	return img;
}

template <class OFFSETTRANS>
void TownsRender::Render(
    unsigned int page,
    const TownsCRTC::Layer &layer,
    const TownsCRTC::AnalogPalette &palette,
    const TownsCRTC::ChaseHQPalette &chaseHQPalette,
    const std::vector <unsigned char> &VRAM,
    bool transparent)
{
	switch(layer.bitsPerPixel)
	{
	case 4:
		Render4Bit<OFFSETTRANS>(layer,palette.plt16[page&1],chaseHQPalette,VRAM,transparent);
		break;
	case 8:
		Render8Bit<OFFSETTRANS>(layer,palette.plt256,VRAM,transparent);
		break;
	case 16:
		Render16Bit<OFFSETTRANS>(layer,VRAM,transparent);
		break;
	}
}
template <class OFFSETTRANS>
void TownsRender::Render4Bit(
    const TownsCRTC::Layer &layer,
    const Vec3ub palette[16],
    const TownsCRTC::ChaseHQPalette &chaseHQPalette,
    const std::vector <unsigned char> &VRAM,
    bool transparent)
{
	const unsigned int VRAMAddr=layer.VRAMAddr;

/*	if(layer.zoom==Vec2i::Make(1,1))
	{
		for(int y=0; y<layer.sizeOnMonitor.y(); ++y)
		{
			const unsigned char *src=VRAM.data()+VRAMAddr+((layer.VRAMOffset+layer.bytesPerLine*y)&layer.VScrollMask);
			unsigned char *dst=rgba.data()+4*y*this->wid;
			for(int x=0; x<layer.sizeOnMonitor.x(); x+=2)
			{
				unsigned char vrambyte=*src;
				unsigned char pix=(vrambyte&0x0f);
				if(0!=pix ||true!=transparent)
				{
					dst[0]=palette[pix][0];
					dst[1]=palette[pix][1];
					dst[2]=palette[pix][2];
					dst[3]=255;
				}
				pix=(vrambyte&0xf0)>>4;
				if(0!=pix ||true!=transparent)
				{
					dst[4]=palette[pix][0];
					dst[5]=palette[pix][1];
					dst[6]=palette[pix][2];
					dst[7]=255;
				}
				++src;
				dst+=8;
			}
		}
	}
	else */

	if(47!=chaseHQPalette.lastPaletteUpdateCount) // ChaseHQ updates palette 47 times between VSYNC
	{
		auto ZV=layer.zoom2x.y()/2;
		const auto ZH=layer.zoom2x.x()/2;
		int bytesPerLineTimesVRAMy=layer.VRAMOffset;
		auto VRAMTop=VRAM.data()+VRAMAddr+layer.VRAMHSkipBytes;

		auto bottomY=this->hei-ZV;
		for(int y=0; y<layer.sizeOnMonitor.y() && y+layer.originOnMonitor.y()<=bottomY; y+=ZV)
		{
			const int Y=y+layer.originOnMonitor.y();
			const int X=  layer.originOnMonitor.x();
			unsigned int VRAMAddr=(bytesPerLineTimesVRAMy&layer.VScrollMask);
			OFFSETTRANS::Trans(VRAMAddr);
			const unsigned char *src=VRAMTop+VRAMAddr;
			unsigned char *dstLine=rgba.data()+4*(Y*this->wid+X);
			auto dst=dstLine;
			for(int x=0; x<layer.sizeOnMonitor.x() && x+layer.originOnMonitor.x()<this->wid; x+=2*ZH)
			{
				unsigned char vrambyte=*src;
				unsigned char pix=(vrambyte&0x0f);
				for(int i=0; i<ZH; ++i)
				{
					if(0!=pix ||true!=transparent)
					{
						dst[0]=palette[pix][0];
						dst[1]=palette[pix][1];
						dst[2]=palette[pix][2];
						dst[3]=255;
					}
					dst+=4;
				}
				pix=(vrambyte&0xf0)>>4;
				for(int i=0; i<ZH; ++i)
				{
					if(0!=pix ||true!=transparent)
					{
						dst[0]=palette[pix][0];
						dst[1]=palette[pix][1];
						dst[2]=palette[pix][2];
						dst[3]=255;
					}
					dst+=4;
				}
				++src;
			}

			auto copyPtr=dstLine+(4*this->wid);
			for(unsigned int zv=1; zv<ZV; ++zv)
			{
				std::memcpy(copyPtr,dstLine,dst-dstLine);
				copyPtr+=(4*this->wid);
			}

			bytesPerLineTimesVRAMy+=layer.bytesPerLine;
		}
	}
	else // For ChaseHQ special: If 16-color mode, and palette changed more than 40 times in one frame.
	{
		// Roughly 98:46:95
		// std::cout << "ChaseHQ Special" << " " << chaseHQPalette.lastPaletteUpdateCount << std::endl;

		Vec3ub paletteUpdate[16];
		for(int i=0; i<16; ++i)
		{
			paletteUpdate[i]=palette[i];
		}

		for(int i=0; i<16; ++i) // Sky
		{
			unsigned int code=chaseHQPalette.paletteLog[i<<2];
			paletteUpdate[code&0x0F][0]=chaseHQPalette.paletteLog[(i<<2)+2];   // BRG->RGB
			paletteUpdate[code&0x0F][1]=chaseHQPalette.paletteLog[(i<<2)+3];   // BRG->RGB
			paletteUpdate[code&0x0F][2]=chaseHQPalette.paletteLog[(i<<2)+1];   // BRG->RGB
		}

		auto ZV=layer.zoom2x.y()/2;
		const auto ZH=layer.zoom2x.x()/2;
		int bytesPerLineTimesVRAMy=layer.VRAMOffset;
		auto VRAMTop=VRAM.data()+VRAMAddr+layer.VRAMHSkipBytes;

		auto bottomY=this->hei-ZV;
		for(int y=0; y<layer.sizeOnMonitor.y() && y+layer.originOnMonitor.y()<=bottomY; y+=ZV)
		{
			const int Y=y+layer.originOnMonitor.y();
			const int X=  layer.originOnMonitor.x();

			if(196==y)
			{
				for(int i=16; i<32; ++i) // Buildings
				{
					unsigned int code=chaseHQPalette.paletteLog[i<<2];
					paletteUpdate[code&0x0F][0]=chaseHQPalette.paletteLog[(i<<2)+2];   // BRG->RGB
					paletteUpdate[code&0x0F][1]=chaseHQPalette.paletteLog[(i<<2)+3];   // BRG->RGB
					paletteUpdate[code&0x0F][2]=chaseHQPalette.paletteLog[(i<<2)+1];   // BRG->RGB
				}
			}
			if(288==y)
			{
				for(int i=32; i<47; ++i) // Road
				{
					unsigned int code=chaseHQPalette.paletteLog[i<<2];
					paletteUpdate[code&0x0F][0]=chaseHQPalette.paletteLog[(i<<2)+2];   // BRG->RGB
					paletteUpdate[code&0x0F][1]=chaseHQPalette.paletteLog[(i<<2)+3];   // BRG->RGB
					paletteUpdate[code&0x0F][2]=chaseHQPalette.paletteLog[(i<<2)+1];   // BRG->RGB
				}
			}

			unsigned int VRAMAddr=(bytesPerLineTimesVRAMy&layer.VScrollMask);
			OFFSETTRANS::Trans(VRAMAddr);
			const unsigned char *src=VRAMTop+VRAMAddr;
			unsigned char *dstLine=rgba.data()+4*(Y*this->wid+X);
			auto dst=dstLine;
			for(int x=0; x<layer.sizeOnMonitor.x() && x+layer.originOnMonitor.x()<this->wid; x+=2*ZH)
			{
				unsigned char vrambyte=*src;
				unsigned char pix=(vrambyte&0x0f);
				for(int i=0; i<ZH; ++i)
				{
					if(0!=pix ||true!=transparent)
					{
						dst[0]=paletteUpdate[pix][0];
						dst[1]=paletteUpdate[pix][1];
						dst[2]=paletteUpdate[pix][2];
						dst[3]=255;
					}
					dst+=4;
				}
				pix=(vrambyte&0xf0)>>4;
				for(int i=0; i<ZH; ++i)
				{
					if(0!=pix ||true!=transparent)
					{
						dst[0]=paletteUpdate[pix][0];
						dst[1]=paletteUpdate[pix][1];
						dst[2]=paletteUpdate[pix][2];
						dst[3]=255;
					}
					dst+=4;
				}
				++src;
			}

			auto copyPtr=dstLine+(4*this->wid);
			for(unsigned int zv=1; zv<ZV; ++zv)
			{
				std::memcpy(copyPtr,dstLine,dst-dstLine);
				copyPtr+=(4*this->wid);
			}

			bytesPerLineTimesVRAMy+=layer.bytesPerLine;
		}
	}
}
template <class OFFSETTRANS>
void TownsRender::Render8Bit(const TownsCRTC::Layer &layer,const Vec3ub palette[256],const std::vector <unsigned char> &VRAM,bool transparent)
{
	unsigned int VRAMBase=layer.VRAMAddr+layer.VRAMHSkipBytes;
	unsigned int VRAMOffsetVertical=layer.VRAMOffset&~layer.HScrollMask;
	unsigned int VRAMOffsetHorizontal=layer.VRAMOffset&layer.HScrollMask;
	const unsigned int VRAMHScrollMask=layer.HScrollMask;
	const unsigned int VRAMVScrollMask=layer.VScrollMask;
	unsigned int lineVRAMOffset=0;
	auto ZV=layer.zoom2x.y()/2;

	auto bottomY=this->hei-ZV;
	for(int y=0; y<layer.sizeOnMonitor.y() && y+layer.originOnMonitor.y()<=bottomY; y+=ZV)
	{
		auto X=  layer.originOnMonitor.x();
		auto Y=y+layer.originOnMonitor.y();
		unsigned char *dstLine=rgba.data()+4*(Y*this->wid+X);
		auto dst=dstLine;

		unsigned int inLineVRAMOffset=0;
		auto ZH=layer.zoom2x.x()/2;
		for(int x=0; x<layer.sizeOnMonitor.x() && x+layer.originOnMonitor.x()<this->wid && inLineVRAMOffset<layer.bytesPerLine; x++)
		{
			unsigned int VRAMAddr=lineVRAMOffset+((inLineVRAMOffset+VRAMOffsetHorizontal)&VRAMHScrollMask);
			VRAMAddr=VRAMBase+((VRAMAddr+VRAMOffsetVertical)&VRAMVScrollMask);
			OFFSETTRANS::Trans(VRAMAddr);

			unsigned char col8=VRAM[VRAMAddr];
			if(true!=transparent || 0!=col8)
			{
				dst[0]=palette[col8][0];
				dst[1]=palette[col8][1];
				dst[2]=palette[col8][2];
				dst[3]=255;
			}
			dst+=4;
			if(0==(--ZH))
			{
				ZH=layer.zoom2x.x()/2;
				++inLineVRAMOffset;
			}
		}

		auto copyPtr=dstLine+(4*this->wid);
		for(unsigned int zv=1; zv<ZV; ++zv)
		{
			std::memcpy(copyPtr,dstLine,dst-dstLine);
			copyPtr+=(4*this->wid);
		}

		lineVRAMOffset+=layer.bytesPerLine;
	}
}
template <class OFFSETTRANS>
void TownsRender::Render16Bit(const TownsCRTC::Layer &layer,const std::vector <unsigned char> &VRAM,bool transparent)
{
	unsigned int VRAMBase=layer.VRAMAddr+layer.VRAMHSkipBytes;
	unsigned int VRAMOffsetVertical=layer.VRAMOffset&~layer.HScrollMask;
	unsigned int VRAMOffsetHorizontal=layer.VRAMOffset&layer.HScrollMask;
	const unsigned int VRAMHScrollMask=layer.HScrollMask;
	const unsigned int VRAMVScrollMask=layer.VScrollMask;
	unsigned int lineVRAMOffset=0;
	auto ZV=layer.zoom2x.y()/2;

	auto bottomY=this->hei-ZV;
	for(int y=0; y<layer.sizeOnMonitor.y() && y+layer.originOnMonitor.y()<=bottomY; y+=ZV)
	{
		auto X=  layer.originOnMonitor.x();
		auto Y=y+layer.originOnMonitor.y();
		unsigned char *dstLine=rgba.data()+4*(Y*this->wid+X);
		auto dst=dstLine;

		unsigned int inLineVRAMOffset=0;
		auto ZH=layer.zoom2x.x()/2;
		for(int x=0; x<layer.sizeOnMonitor.x() && x+layer.originOnMonitor.x()<this->wid && inLineVRAMOffset<layer.bytesPerLine; x++)
		{
			unsigned int VRAMAddr=lineVRAMOffset+((inLineVRAMOffset+VRAMOffsetHorizontal)&VRAMHScrollMask);
			VRAMAddr=VRAMBase+((VRAMAddr+VRAMOffsetVertical)&VRAMVScrollMask);
			OFFSETTRANS::Trans(VRAMAddr);

			unsigned short col16=cpputil::GetWord(VRAM.data()+VRAMAddr);
			if(true!=transparent || 0==(col16&0x8000))
			{
				dst[0]=((col16&0b000001111100000)>>5);
				dst[0]=(dst[0]<<3)|((dst[0]>>2)&7);
				dst[1]=((col16&0b111110000000000)>>10);
				dst[1]=(dst[1]<<3)|((dst[1]>>2)&7);
				dst[2]=(col16&0b000000000011111);
				dst[2]=(dst[2]<<3)|((dst[2]>>2)&7);
				dst[3]=255;
			}
			dst+=4;
			if(0==(--ZH))
			{
				ZH=layer.zoom2x.x()/2;
				inLineVRAMOffset+=2;
			}
		}

		auto copyPtr=dstLine+(4*this->wid);
		for(unsigned int zv=1; zv<ZV; ++zv)
		{
			std::memcpy(copyPtr,dstLine,dst-dstLine);
			copyPtr+=(4*this->wid);
		}

		lineVRAMOffset+=layer.bytesPerLine;
	}
}
