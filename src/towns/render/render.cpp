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

	if(true==crtc.InSinglePageMode())
	{
		TownsCRTC::Layer layer;
		crtc.MakePageLayerInfo(layer,0);
		Render(0,layer,crtc.state.palette,physMem.state.VRAM,false);
	}
	else
	{
		TownsCRTC::Layer layer[2];
		crtc.MakePageLayerInfo(layer[0],0);
		crtc.MakePageLayerInfo(layer[1],1);
		auto priorityPage=crtc.GetPriorityPage();
		Render(priorityPage,  layer[priorityPage]  ,crtc.state.palette,physMem.state.VRAM,false);
		Render(1-priorityPage,layer[1-priorityPage],crtc.state.palette,physMem.state.VRAM,true);
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
void TownsRender::Render(unsigned int page,const TownsCRTC::Layer &layer,const TownsCRTC::AnalogPalette &palette,const std::vector <unsigned char> &VRAM,bool transparent)
{
	switch(layer.bitsPerPixel)
	{
	case 4:
		Render4Bit(layer,palette.plt16[page&1],VRAM,transparent);
		break;
	case 8:
		Render8Bit(layer,palette.plt256,VRAM,transparent);
		break;
	case 16:
		Render16Bit(layer,VRAM,transparent);
		break;
	}
}
void TownsRender::Render4Bit(const TownsCRTC::Layer &layer,const Vec3ub palette[16],const std::vector <unsigned char> &VRAM,bool transparent)
{
	unsigned int VRAMAddr=layer.VRAMAddr;

	for(int y=0; y<layer.sizeOnMonitor.y(); ++y)
	{
		const unsigned char *src=VRAM.data()+VRAMAddr+layer.bytesPerLine*y;
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
void TownsRender::Render8Bit(const TownsCRTC::Layer &layer,const Vec3ub palette[256],const std::vector <unsigned char> &VRAM,bool transparent)
{
	unsigned int VRAMBase=layer.VRAMAddr; // Supposed to be zero.
	unsigned int VRAMOffsetVertical=layer.VRAMOffset&~layer.HScrollMask;
	unsigned int VRAMOffsetHorizontal=layer.VRAMOffset&layer.HScrollMask;
	const unsigned int VRAMHScrollMask=layer.HScrollMask;
	const unsigned int VRAMVScrollMask=layer.VScrollMask;
	unsigned int lineVRAMOffset=0;
	auto ZV=layer.zoom.y();
	for(int y=0; y<layer.sizeOnMonitor.y() && y+layer.originOnMonitor.y()<this->hei; ++y)
	{
		auto X=  layer.originOnMonitor.x();
		auto Y=y+layer.originOnMonitor.y();
		unsigned char *dst=rgba.data()+4*(Y*this->wid+X);

		unsigned int inLineVRAMOffset=0;
		auto ZH=layer.zoom.x();
		for(int x=0; x<layer.sizeOnMonitor.x() && x+layer.originOnMonitor.x()<this->wid && inLineVRAMOffset<layer.bytesPerLine; x++)
		{
			unsigned int VRAMAddr=lineVRAMOffset+((inLineVRAMOffset+VRAMOffsetHorizontal)&VRAMHScrollMask);
			VRAMAddr=VRAMBase+((VRAMAddr+VRAMOffsetVertical)&VRAMVScrollMask);

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
				ZH=layer.zoom.x();
				++inLineVRAMOffset;
			}
		}
		if(0==(--ZV))
		{
			ZV=layer.zoom.y();
			lineVRAMOffset+=layer.bytesPerLine;
		}
	}
}
void TownsRender::Render16Bit(const TownsCRTC::Layer &layer,const std::vector <unsigned char> &VRAM,bool transparent)
{
	unsigned int VRAMBase=layer.VRAMAddr;
	unsigned int VRAMOffsetVertical=layer.VRAMOffset&~layer.HScrollMask;
	unsigned int VRAMOffsetHorizontal=layer.VRAMOffset&layer.HScrollMask;
	const unsigned int VRAMHScrollMask=layer.HScrollMask;
	const unsigned int VRAMVScrollMask=layer.VScrollMask;
	unsigned int lineVRAMOffset=0;
	auto ZV=layer.zoom.y();
	for(int y=0; y<layer.sizeOnMonitor.y() && y+layer.originOnMonitor.y()<this->hei; ++y)
	{
		auto X=  layer.originOnMonitor.x();
		auto Y=y+layer.originOnMonitor.y();
		unsigned char *dst=rgba.data()+4*(Y*this->wid+X);

		unsigned int inLineVRAMOffset=0;
		auto ZH=layer.zoom.x();
		for(int x=0; x<layer.sizeOnMonitor.x() && x+layer.originOnMonitor.x()<this->wid && inLineVRAMOffset<layer.bytesPerLine; x++)
		{
			unsigned int VRAMAddr=lineVRAMOffset+((inLineVRAMOffset+VRAMOffsetHorizontal)&VRAMHScrollMask);
			VRAMAddr=VRAMBase+((VRAMAddr+VRAMOffsetVertical)&VRAMVScrollMask);

			unsigned short col16=VRAM[VRAMAddr]|(VRAM[VRAMAddr+1]<<8);
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
				ZH=layer.zoom.x();
				inLineVRAMOffset+=2;
			}
		}
		if(0==(--ZV))
		{
			ZV=layer.zoom.y();
			lineVRAMOffset+=layer.bytesPerLine;
		}
	}
}
