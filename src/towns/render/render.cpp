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
	BuildImageFMRMode(crtc,physMem); // Tentative
}
void TownsRender::BuildImageFMRMode(const TownsCRTC &crtc,const TownsPhysicalMemory &physMem)
{
	int priorityPage=0;
	unsigned int wid,hei;

	crtc.GetRenderSize(wid,hei);
	SetResolution(wid,hei);

	if(true!=crtc.cached)
	{
		// Cache crtc setting.
	}

	unsigned int page0Ptr=0;
	unsigned int page1Ptr=0x40000;
	Render(crtc.cache.layer[0],page0Ptr,physMem.state.VRAM,false);
	Render(crtc.cache.layer[1],page1Ptr,physMem.state.VRAM,true);
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

void TownsRender::Render(const TownsCRTC::Layer &layer,unsigned int VRAMAddr,const std::vector <unsigned char> &VRAM,bool transparent)
{
	switch(layer.bitsPerPixel)
	{
	case 4:
		Render4Bit(layer,VRAMAddr,VRAM,transparent);
		break;
	}
}

void TownsRender::Render4Bit(const TownsCRTC::Layer &layer,unsigned int VRAMAddr,const std::vector <unsigned char> &VRAM,bool transparent)
{
	unsigned char palette[16][4]=
	{
		{  0,  0,  0,255},
		{  0,  0,255,255},
		{255,  0,  0,255},
		{255,  0,255,255},
		{  0,255,  0,255},
		{  0,255,255,255},
		{255,255,  0,255},
		{255,255,255,255},
		{  0,  0,  0,255},
		{  0,  0,255,255},
		{255,  0,  0,255},
		{255,  0,255,255},
		{  0,255,  0,255},
		{  0,255,255,255},
		{255,255,  0,255},
		{255,255,255,255},
	};
	for(int y=0; y<layer.visibleHei; ++y)
	{
		const unsigned char *src=VRAM.data()+VRAMAddr+layer.bytesPerLine*y;
		unsigned char *dst=rgba.data()+4*y*this->wid;
		for(int x=0; x<layer.visibleWid; x+=2)
		{
			unsigned char vrambyte=*src;
			unsigned char pix=(vrambyte&0x0f);
			if(0!=pix ||true!=transparent)
			{
				dst[0]=palette[pix][0];
				dst[1]=palette[pix][1];
				dst[2]=palette[pix][2];
				dst[3]=palette[pix][3];
			}
			pix=(vrambyte&0xf0)>>4;
			if(0!=pix ||true!=transparent)
			{
				dst[4]=palette[pix][0];
				dst[5]=palette[pix][1];
				dst[6]=palette[pix][2];
				dst[7]=palette[pix][3];
			}
			++src;
			dst+=8;
		}
	}
}
