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

void TownsRender::Crop(unsigned int x0,unsigned int y0,unsigned int newWid,unsigned int newHei)
{
	std::vector <unsigned char> newRGBA;
	newRGBA.resize(newWid*newHei*4);

	memset(newRGBA.data(),0,newRGBA.size());

	unsigned char *dstRow=newRGBA.data();
	unsigned char *srcRow=rgba.data()+(wid*y0+x0)*4;
	unsigned int copySizePerRow=0;
	if(x0+newWid<=wid)
	{
		copySizePerRow=newWid*4;
	}
	else if(x0<wid)
	{
		copySizePerRow=(wid-x0)*4;
	}
	for(unsigned int y=0; y<newHei && y0+y<hei; ++y)
	{
		memcpy(dstRow,srcRow,copySizePerRow);
		dstRow+=newWid*4;
		srcRow+=wid*4;
	}

	std::swap(rgba,newRGBA);
	wid=newWid;
	hei=newHei;
}

void TownsRender::MakeOpaque(void)
{
	for(int i=0; i<wid*hei; ++i)
	{
		rgba[i*4+3]=255;
	}
}

void TownsRender::Prepare(const TownsCRTC &crtc)
{
	frequency=crtc.GetHorizontalFrequency();
	highResCRTC=crtc.state.highResCRTCEnabled;
	if(true==highResCRTC)
	{
		hardwareMouse=crtc.state.highResCrtcMouse;
	}
	crtcIsSinglePageMode=crtc.InSinglePageMode();
	if(true==crtcIsSinglePageMode)
	{
		crtcShowPage[0]=crtc.state.ShowPage(0);
		crtc.MakePageLayerInfo(crtcLayer[0],0);
	}
	else
	{
		crtcShowPage[0]=crtc.state.ShowPage(0);
		crtcShowPage[1]=crtc.state.ShowPage(1);
		crtc.MakePageLayerInfo(crtcLayer[0],0);
		crtc.MakePageLayerInfo(crtcLayer[1],1);
	}
	crtcPriorityPage=crtc.GetPriorityPage();
	crtcRenderSize=crtc.GetRenderSize();
}

void TownsRender::PrepareEntireVRAMLayer(const TownsCRTC &crtc,int layer)
{
	highResCRTC=crtc.state.highResCRTCEnabled;
	if(true==highResCRTC)
	{
		hardwareMouse=crtc.state.highResCrtcMouse;
	}
	crtcIsSinglePageMode=crtc.InSinglePageMode();
	if(true==crtcIsSinglePageMode)
	{
		crtcShowPage[0]=crtc.state.ShowPage(0);
		crtc.MakePageLayerInfo(crtcLayer[0],0);
	}
	else
	{
		crtcShowPage[0]=crtc.state.ShowPage(0);
		crtcShowPage[1]=crtc.state.ShowPage(1);
		crtc.MakePageLayerInfo(crtcLayer[0],0);
		crtc.MakePageLayerInfo(crtcLayer[1],1);
	}

	if(1==crtcIsSinglePageMode && 0!=layer)
	{
		crtcShowPage[0]=false;
		crtcShowPage[1]=false;
	}
	else if(0==layer || 1==layer)
	{
		crtcShowPage[layer]=true;
		crtcShowPage[1-layer]=false;
		crtcLayer[layer].VRAMOffset=0;
		crtcLayer[layer].originOnMonitor.Set(0,0);
		crtcLayer[layer].VRAMHSkipBytes=0;
		crtcLayer[layer].sizeOnMonitor[0]=crtcLayer[layer].bytesPerLine*8/crtcLayer[layer].bitsPerPixel;
		crtcLayer[layer].sizeOnMonitor[1]=(crtcLayer[layer].VScrollMask+1)/crtcLayer[layer].bytesPerLine;
		crtcLayer[layer].VRAMCoverage1X=crtcLayer[layer].sizeOnMonitor;
		crtcLayer[layer].zoom2x.Set(2,2);
	}
	else
	{
		crtcShowPage[0]=false;
		crtcShowPage[1]=false;
	}

	crtcPriorityPage=crtc.GetPriorityPage();
	crtcRenderSize=crtcLayer[layer].sizeOnMonitor;
}

void TownsRender::OerrideShowPage(bool layer0,bool layer1)
{
	crtcShowPage[0]=layer0;
	crtcShowPage[1]=layer1;
}

void TownsRender::BuildImage(const unsigned char VRAM[],const TownsCRTC::AnalogPalette &palette,const TownsCRTC::ChaseHQPalette &chaseHQPalette)
{
	SetResolution(crtcRenderSize.x(),crtcRenderSize.y());

	std::memset(rgba.data(),0,rgba.size());

	if(true==crtcIsSinglePageMode)
	{
		if(true==crtcShowPage[0])
		{
			if(true!=highResCRTC)
			{
				Render<VRAM1Trans>(0,crtcLayer[0],palette,chaseHQPalette,VRAM,false);
			}
			else
			{
				Render<VRAM0Trans>(0,crtcLayer[0],palette,chaseHQPalette,VRAM,false);
			}
		}
	}
	else
	{
		auto priorityPage=crtcPriorityPage;
		if(true==crtcShowPage[1-priorityPage])
		{
			Render<VRAM0Trans>(1-priorityPage,crtcLayer[1-priorityPage],palette,chaseHQPalette,VRAM,false);
		}
		if(true==crtcShowPage[priorityPage])
		{
			Render<VRAM0Trans>(priorityPage,  crtcLayer[priorityPage]  ,palette,chaseHQPalette,VRAM,true);
		}
	}

	if(true==scanLineEffectIn15KHz && 15==frequency)
	{
		auto dy=((scanLineCounter++)&2)>>1;
		for(int y=0; y+dy<hei; y+=2)
		{
			auto linePtr=rgba.data()+(y+dy)*wid*4;
			for(int x=0; x<wid; ++x)
			{
				linePtr[0]=(unsigned char)((unsigned int)linePtr[0]*7/8);
				linePtr[1]=(unsigned char)((unsigned int)linePtr[1]*7/8);
				linePtr[2]=(unsigned char)((unsigned int)linePtr[2]*7/8);
				linePtr+=4;
			}
		}
	}

	if(true==damperWireLine)
	{
		auto linePtr=rgba.data()+(hei*2/3)*wid*4;
		for(int x=0; x<wid; ++x)
		{
			linePtr[0]=(unsigned char)((unsigned int)linePtr[0]*7/8);
			linePtr[1]=(unsigned char)((unsigned int)linePtr[1]*7/8);
			linePtr[2]=(unsigned char)((unsigned int)linePtr[2]*7/8);
			linePtr+=4;
		}
	}

	if(true==highResCRTC && true==hardwareMouse.defined)
	{
		for(int y=0; y<64; ++y)
		{
			uint8_t *ANDPtn=hardwareMouse.ANDPtn+8*y;
			uint8_t *ORPtn=hardwareMouse.ORPtn+8*y;
			uint8_t bit=0x80;
			for(int x=0; x<64; ++x)
			{
				uint32_t xOnScrn=hardwareMouse.X+x-hardwareMouse.originX;
				uint32_t yOnScrn=hardwareMouse.Y+y-hardwareMouse.originY;
				if(xOnScrn<wid && yOnScrn<hei)
				{
					if(0==(*ANDPtn&bit))
					{
						auto pixelPtr=rgba.data()+4*(wid*yOnScrn+xOnScrn);
						if(0==(*ORPtn&bit))
						{
							pixelPtr[0]=0;
							pixelPtr[1]=0;
							pixelPtr[2]=0;
							pixelPtr[3]=255;
						}
						else
						{
							pixelPtr[0]=255;
							pixelPtr[1]=255;
							pixelPtr[2]=255;
							pixelPtr[3]=255;
						}
					}
					bit>>=1;
					if(0==bit)
					{
						++ANDPtn;
						++ORPtn;
						bit=0x80;
					}
				}
			}
		}
	}
}

void TownsRender::FlipUpsideDown(void)
{
	std::vector <unsigned char> flip;
	flip.resize(wid*4);

	auto upsideDown=rgba.data();
	auto rightSideUp=rgba.data()+(hei-1)*wid*4;
	for(unsigned int y=0; y<hei/2; ++y)
	{
		memcpy(flip.data(),upsideDown ,wid*4);
		memcpy(upsideDown ,rightSideUp,wid*4);
		memcpy(rightSideUp,flip.data(),wid*4);
		upsideDown+=wid*4;
		rightSideUp-=wid*4;
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
TownsRender::ImageCopy TownsRender::MoveImage(void)
{
	ImageCopy img;
	img.wid=this->wid;
	img.hei=this->hei;
	std::swap(img.rgba,this->rgba);
	this->wid=0;
	this->hei=0;
	this->rgba.clear();
	return img;
}

template <class OFFSETTRANS>
void TownsRender::Render(
    unsigned int page,
    const TownsCRTC::Layer &layer,
    const TownsCRTC::AnalogPalette &palette,
    const TownsCRTC::ChaseHQPalette &chaseHQPalette,
    const unsigned char VRAM[],
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
	case 24:
		Render24Bit<OFFSETTRANS>(layer,VRAM,transparent);
		break;
	}
}
template <class OFFSETTRANS>
void TownsRender::Render4Bit(
    const TownsCRTC::Layer &layer,
    const Vec3ub palette[16],
    const TownsCRTC::ChaseHQPalette &chaseHQPalette,
    const unsigned char VRAM[],
    bool transparent)
{
	const unsigned int VRAMAddr=layer.VRAMAddr;

/*	if(layer.zoom==Vec2i::Make(1,1))
	{
		for(int y=0; y<layer.sizeOnMonitor.y(); ++y)
		{
			const unsigned char *src=VRAM.data()+VRAMAddr+((layer.VRAMOffset+layer.FMRVRAMOffset+layer.bytesPerLine*y)&layer.VScrollMask);
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

	unsigned int pixelMask=(layer.FMRGVRAMMask&0x0F);
	pixelMask|=(pixelMask<<4);

	if(47!=chaseHQPalette.lastPaletteUpdateCount) // ChaseHQ updates palette 47 times between VSYNC
	{
		auto ZV0=layer.zoom2x.y()/2;
		auto ZV=ZV0;
		const int ZH[3] = { layer.zoom2x.x() / 2,(layer.zoom2x.x() + 1) / 2, (layer.zoom2x.x() / 2) + ((layer.zoom2x.x() + 1) / 2) };  // For x.5 times zoom rate.
		int bytesPerLineTimesVRAMy=layer.VRAMOffset+layer.FlipVRAMOffset;
		auto VRAMTop=VRAM+VRAMAddr+layer.VRAMHSkipBytes;

		// yStep should be 1 if transparent.
		// If transparnet==true, there is a possibility that memcpy overwrites background pixels.
		unsigned int yStep=(true!=transparent ? ZV : 1);
		auto bottomY=this->hei-yStep;
		if (true != transparent)
		{
			for (int y = 0; y < layer.sizeOnMonitor.y() && y + layer.originOnMonitor.y() <= bottomY; y += yStep)
			{
				const int Y = y + layer.originOnMonitor.y();
				const int X = layer.originOnMonitor.x();
				unsigned int VRAMAddr = (bytesPerLineTimesVRAMy & layer.VScrollMask);
				OFFSETTRANS::Trans(VRAMAddr);
				const unsigned char* src = VRAMTop + VRAMAddr;
				unsigned char* dstLine = rgba.data() + 4 * (Y * this->wid + X);
				auto dst = dstLine;
				for (int x = 0; x < layer.sizeOnMonitor.x() && x + layer.originOnMonitor.x() < this->wid; x += ZH[2])
				{
					unsigned char vrambyte = ((*src) & pixelMask);
					unsigned char pix = (vrambyte & 0x0f);
					for (int i = 0; i < ZH[0]; ++i)
					{
						dst[0] = palette[pix][0];
						dst[1] = palette[pix][1];
						dst[2] = palette[pix][2];
						dst[3] = 255;
						dst += 4;
					}
					pix = (vrambyte & 0xf0) >> 4;
					for (int i = 0; i < ZH[1]; ++i)
					{
						dst[0] = palette[pix][0];
						dst[1] = palette[pix][1];
						dst[2] = palette[pix][2];
						dst[3] = 255;
						dst += 4;
					}
					++src;
				}

				if (1 < yStep)
				{
					auto copyPtr = dstLine + (4 * this->wid);
					for (unsigned int zv = 1; zv < yStep; ++zv)
					{
						std::memcpy(copyPtr, dstLine, dst - dstLine);
						copyPtr += (4 * this->wid);
					}
					bytesPerLineTimesVRAMy += layer.bytesPerLine;
				}
				else
				{
					--ZV;
					if (0 == ZV)
					{
						ZV = ZV0;
						bytesPerLineTimesVRAMy += layer.bytesPerLine;
					}
				}
			}
		}
		else
		{
			for (int y = 0; y < layer.sizeOnMonitor.y() && y + layer.originOnMonitor.y() <= bottomY; y += yStep)
			{
				const int Y = y + layer.originOnMonitor.y();
				const int X = layer.originOnMonitor.x();
				unsigned int VRAMAddr = (bytesPerLineTimesVRAMy & layer.VScrollMask);
				OFFSETTRANS::Trans(VRAMAddr);
				const unsigned char* src = VRAMTop + VRAMAddr;
				unsigned char* dstLine = rgba.data() + 4 * (Y * this->wid + X);
				auto dst = dstLine;
				for (int x = 0; x < layer.sizeOnMonitor.x() && x + layer.originOnMonitor.x() < this->wid; x += ZH[2])
				{
					unsigned char vrambyte = ((*src) & pixelMask);
					unsigned char pix = (vrambyte & 0x0f);
					for (int i = 0; i < ZH[0]; ++i)
					{
						if (0 != pix)
						{
							dst[0] = palette[pix][0];
							dst[1] = palette[pix][1];
							dst[2] = palette[pix][2];
							dst[3] = 255;
						}
						dst += 4;
					}
					pix = (vrambyte & 0xf0) >> 4;
					for (int i = 0; i < ZH[1]; ++i)
					{
						if (0 != pix)
						{
							dst[0] = palette[pix][0];
							dst[1] = palette[pix][1];
							dst[2] = palette[pix][2];
							dst[3] = 255;
						}
						dst += 4;
					}
					++src;
				}

				if (1 < yStep)
				{
					auto copyPtr = dstLine + (4 * this->wid);
					for (unsigned int zv = 1; zv < yStep; ++zv)
					{
						std::memcpy(copyPtr, dstLine, dst - dstLine);
						copyPtr += (4 * this->wid);
					}
					bytesPerLineTimesVRAMy += layer.bytesPerLine;
				}
				else
				{
					--ZV;
					if (0 == ZV)
					{
						ZV = ZV0;
						bytesPerLineTimesVRAMy += layer.bytesPerLine;
					}
				}
			}
		}
	}
	else // For ChaseHQ special: If 16-color mode, and palette changed more than 40 times in one frame.
	{
		// Roughly 98:46:95
		// std::cout << "ChaseHQ Special" << " " << chaseHQPalette.lastPaletteUpdateCount << std::endl;

		Vec3ub paletteUpdate[16];
		for (int i = 0; i < 16; ++i)
		{
			paletteUpdate[i] = palette[i];
		}

		for (int i = 0; i < 16; ++i) // Sky
		{
			unsigned int code = chaseHQPalette.paletteLog[i << 2];
			paletteUpdate[code & 0x0F][0] = chaseHQPalette.paletteLog[(i << 2) + 2];   // BRG->RGB
			paletteUpdate[code & 0x0F][1] = chaseHQPalette.paletteLog[(i << 2) + 3];   // BRG->RGB
			paletteUpdate[code & 0x0F][2] = chaseHQPalette.paletteLog[(i << 2) + 1];   // BRG->RGB
		}

		auto ZV = layer.zoom2x.y() / 2;
		const int ZH[3] = { layer.zoom2x.x() / 2,(layer.zoom2x.x() + 1) / 2 ,  (layer.zoom2x.x() / 2) + ((layer.zoom2x.x() + 1) / 2) };  // For x.5 times zoom rate.
		int bytesPerLineTimesVRAMy = layer.VRAMOffset + layer.FlipVRAMOffset;
		auto VRAMTop = VRAM + VRAMAddr + layer.VRAMHSkipBytes;

		auto bottomY = this->hei - ZV;
		if (true != transparent)
		{
			for (int y = 0; y < layer.sizeOnMonitor.y() && y + layer.originOnMonitor.y() <= bottomY; y += ZV)
			{
				const int Y = y + layer.originOnMonitor.y();
				const int X = layer.originOnMonitor.x();

				if (196 == y)
				{
					for (int i = 16; i < 32; ++i) // Buildings
					{
						unsigned int code = chaseHQPalette.paletteLog[i << 2];
						paletteUpdate[code & 0x0F][0] = chaseHQPalette.paletteLog[(i << 2) + 2];   // BRG->RGB
						paletteUpdate[code & 0x0F][1] = chaseHQPalette.paletteLog[(i << 2) + 3];   // BRG->RGB
						paletteUpdate[code & 0x0F][2] = chaseHQPalette.paletteLog[(i << 2) + 1];   // BRG->RGB
					}
				}
				if (288 == y)
				{
					for (int i = 32; i < 47; ++i) // Road
					{
						unsigned int code = chaseHQPalette.paletteLog[i << 2];
						paletteUpdate[code & 0x0F][0] = chaseHQPalette.paletteLog[(i << 2) + 2];   // BRG->RGB
						paletteUpdate[code & 0x0F][1] = chaseHQPalette.paletteLog[(i << 2) + 3];   // BRG->RGB
						paletteUpdate[code & 0x0F][2] = chaseHQPalette.paletteLog[(i << 2) + 1];   // BRG->RGB
					}
				}

				unsigned int VRAMAddr = (bytesPerLineTimesVRAMy & layer.VScrollMask);
				OFFSETTRANS::Trans(VRAMAddr);
				const unsigned char* src = VRAMTop + VRAMAddr;
				unsigned char* dstLine = rgba.data() + 4 * (Y * this->wid + X);
				auto dst = dstLine;
				for (int x = 0; x < layer.sizeOnMonitor.x() && x + layer.originOnMonitor.x() < this->wid; x += ZH[2])
				{
					unsigned char vrambyte = ((*src) & pixelMask);
					unsigned char pix = (vrambyte & 0x0f);
					for (int i = 0; i < ZH[0]; ++i)
					{
						dst[0] = paletteUpdate[pix][0];
						dst[1] = paletteUpdate[pix][1];
						dst[2] = paletteUpdate[pix][2];
						dst[3] = 255;
						dst += 4;
					}
					pix = (vrambyte & 0xf0) >> 4;
					for (int i = 0; i < ZH[1]; ++i)
					{
						dst[0] = paletteUpdate[pix][0];
						dst[1] = paletteUpdate[pix][1];
						dst[2] = paletteUpdate[pix][2];
						dst[3] = 255;
						dst += 4;
					}
					++src;
				}

				auto copyPtr = dstLine + (4 * this->wid);
				for (unsigned int zv = 1; zv < ZV; ++zv)
				{
					std::memcpy(copyPtr, dstLine, dst - dstLine);
					copyPtr += (4 * this->wid);
				}

				bytesPerLineTimesVRAMy += layer.bytesPerLine;
			}
		}
		else
		{
			for (int y = 0; y < layer.sizeOnMonitor.y() && y + layer.originOnMonitor.y() <= bottomY; y += ZV)
			{
				const int Y = y + layer.originOnMonitor.y();
				const int X = layer.originOnMonitor.x();

				if (196 == y)
				{
					for (int i = 16; i < 32; ++i) // Buildings
					{
						unsigned int code = chaseHQPalette.paletteLog[i << 2];
						paletteUpdate[code & 0x0F][0] = chaseHQPalette.paletteLog[(i << 2) + 2];   // BRG->RGB
						paletteUpdate[code & 0x0F][1] = chaseHQPalette.paletteLog[(i << 2) + 3];   // BRG->RGB
						paletteUpdate[code & 0x0F][2] = chaseHQPalette.paletteLog[(i << 2) + 1];   // BRG->RGB
					}
				}
				if (288 == y)
				{
					for (int i = 32; i < 47; ++i) // Road
					{
						unsigned int code = chaseHQPalette.paletteLog[i << 2];
						paletteUpdate[code & 0x0F][0] = chaseHQPalette.paletteLog[(i << 2) + 2];   // BRG->RGB
						paletteUpdate[code & 0x0F][1] = chaseHQPalette.paletteLog[(i << 2) + 3];   // BRG->RGB
						paletteUpdate[code & 0x0F][2] = chaseHQPalette.paletteLog[(i << 2) + 1];   // BRG->RGB
					}
				}

				unsigned int VRAMAddr = (bytesPerLineTimesVRAMy & layer.VScrollMask);
				OFFSETTRANS::Trans(VRAMAddr);
				const unsigned char* src = VRAMTop + VRAMAddr;
				unsigned char* dstLine = rgba.data() + 4 * (Y * this->wid + X);
				auto dst = dstLine;
				for (int x = 0; x < layer.sizeOnMonitor.x() && x + layer.originOnMonitor.x() < this->wid; x += ZH[2])
				{
					unsigned char vrambyte = ((*src) & pixelMask);
					unsigned char pix = (vrambyte & 0x0f);
					for (int i = 0; i < ZH[0]; ++i)
					{
						if (0 != pix)
						{
							dst[0] = paletteUpdate[pix][0];
							dst[1] = paletteUpdate[pix][1];
							dst[2] = paletteUpdate[pix][2];
							dst[3] = 255;
						}
						dst += 4;
					}
					pix = (vrambyte & 0xf0) >> 4;
					for (int i = 0; i < ZH[1]; ++i)
					{
						if (0 != pix)
						{
							dst[0] = paletteUpdate[pix][0];
							dst[1] = paletteUpdate[pix][1];
							dst[2] = paletteUpdate[pix][2];
							dst[3] = 255;
						}
						dst += 4;
					}
					++src;
				}

				auto copyPtr = dstLine + (4 * this->wid);
				for (unsigned int zv = 1; zv < ZV; ++zv)
				{
					std::memcpy(copyPtr, dstLine, dst - dstLine);
					copyPtr += (4 * this->wid);
				}

				bytesPerLineTimesVRAMy += layer.bytesPerLine;
			}
		}
	}
}
template <class OFFSETTRANS>
void TownsRender::Render8Bit(const TownsCRTC::Layer &layer,const Vec3ub palette[256],const unsigned char VRAM[],bool transparent)
{
	unsigned int VRAMBase=layer.VRAMAddr;
	unsigned int VRAMOffsetVertical=(layer.VRAMOffset+layer.FlipVRAMOffset)&~layer.HScrollMask;
	unsigned int VRAMOffsetHorizontal=(layer.VRAMOffset+layer.FlipVRAMOffset)&layer.HScrollMask;
	const unsigned int VRAMHScrollMask=layer.HScrollMask;
	const unsigned int VRAMVScrollMask=layer.VScrollMask;
	unsigned int lineVRAMOffset=0;
	const int ZHsrc[2]={layer.zoom2x.x()/2,(layer.zoom2x.x()+1)/2};  // For x.5 times zoom rate.
	auto ZV=layer.zoom2x.y()/2;

	auto bottomY=this->hei-ZV;
	if (true != transparent)
	{
		for (int y = 0; y < layer.sizeOnMonitor.y() && y + layer.originOnMonitor.y() <= bottomY; y += ZV)
		{
			auto X = layer.originOnMonitor.x();
			auto Y = y + layer.originOnMonitor.y();
			unsigned char* dstLine = rgba.data() + 4 * (Y * this->wid + X);
			auto dst = dstLine;

			unsigned int inLineVRAMOffset = layer.VRAMHSkipBytes;
			int ZHswitch = 0;
			auto ZH = ZHsrc[ZHswitch];
			for (int x = 0; x < layer.sizeOnMonitor.x() && x + layer.originOnMonitor.x() < this->wid && inLineVRAMOffset < layer.bytesPerLine; x++)
			{
				unsigned int VRAMAddr = lineVRAMOffset + ((inLineVRAMOffset + VRAMOffsetHorizontal) & VRAMHScrollMask);
				VRAMAddr = VRAMBase + ((VRAMAddr + VRAMOffsetVertical) & VRAMVScrollMask);
				OFFSETTRANS::Trans(VRAMAddr);

				unsigned char col8 = VRAM[VRAMAddr];
				dst[0] = palette[col8][0];
				dst[1] = palette[col8][1];
				dst[2] = palette[col8][2];
				dst[3] = 255;
				dst += 4;
				if (0 == (--ZH))
				{
					ZHswitch = 1 - ZHswitch;
					ZH = ZHsrc[ZHswitch];
					++inLineVRAMOffset;
				}
			}

			auto copyPtr = dstLine + (4 * this->wid);
			for (unsigned int zv = 1; zv < ZV; ++zv)
			{
				std::memcpy(copyPtr, dstLine, dst - dstLine);
				copyPtr += (4 * this->wid);
			}

			lineVRAMOffset += layer.bytesPerLine;
		}
	}
	else
	{
		for (int y = 0; y < layer.sizeOnMonitor.y() && y + layer.originOnMonitor.y() <= bottomY; y += ZV)
		{
			auto X = layer.originOnMonitor.x();
			auto Y = y + layer.originOnMonitor.y();
			unsigned char* dstLine = rgba.data() + 4 * (Y * this->wid + X);
			auto dst = dstLine;

			unsigned int inLineVRAMOffset = layer.VRAMHSkipBytes;
			int ZHswitch = 0;
			auto ZH = ZHsrc[ZHswitch];
			for (int x = 0; x < layer.sizeOnMonitor.x() && x + layer.originOnMonitor.x() < this->wid && inLineVRAMOffset < layer.bytesPerLine; x++)
			{
				unsigned int VRAMAddr = lineVRAMOffset + ((inLineVRAMOffset + VRAMOffsetHorizontal) & VRAMHScrollMask);
				VRAMAddr = VRAMBase + ((VRAMAddr + VRAMOffsetVertical) & VRAMVScrollMask);
				OFFSETTRANS::Trans(VRAMAddr);

				unsigned char col8 = VRAM[VRAMAddr];
				if (0 != col8)
				{
					dst[0] = palette[col8][0];
					dst[1] = palette[col8][1];
					dst[2] = palette[col8][2];
					dst[3] = 255;
				}
				dst += 4;
				if (0 == (--ZH))
				{
					ZHswitch = 1 - ZHswitch;
					ZH = ZHsrc[ZHswitch];
					++inLineVRAMOffset;
				}
			}

			auto copyPtr = dstLine + (4 * this->wid);
			for (unsigned int zv = 1; zv < ZV; ++zv)
			{
				std::memcpy(copyPtr, dstLine, dst - dstLine);
				copyPtr += (4 * this->wid);
			}

			lineVRAMOffset += layer.bytesPerLine;
		}
	}
}
template <class OFFSETTRANS>
void TownsRender::Render16Bit(const TownsCRTC::Layer &layer,const unsigned char VRAM[],bool transparent)
{
	unsigned int VRAMBase=layer.VRAMAddr;
	unsigned int VRAMOffsetVertical=(layer.VRAMOffset+layer.FlipVRAMOffset)&~layer.HScrollMask;
	unsigned int VRAMOffsetHorizontal=(layer.VRAMOffset+layer.FlipVRAMOffset)&layer.HScrollMask;
	const unsigned int VRAMHScrollMask=layer.HScrollMask;
	const unsigned int VRAMVScrollMask=layer.VScrollMask;
	unsigned int lineVRAMOffset=0;
	const int ZHsrc[2]={layer.zoom2x.x()/2,(layer.zoom2x.x()+1)/2};  // For x.5 times zoom rate.
	auto ZV0=layer.zoom2x.y()/2;
	auto ZV=ZV0;

	// yStep should be 1 if transparent.
	// If transparnet==true, there is a possibility that memcpy overwrites background pixels.
	unsigned int yStep=(true!=transparent ? ZV0 : 1);
	auto bottomY=this->hei-yStep;
	auto X=layer.originOnMonitor.x();
	auto RenderOffsetX=X*2;
	if (true != transparent)
	{
		for (int y = 0; y < layer.sizeOnMonitor.y() && y + layer.originOnMonitor.y() <= bottomY; y += yStep)
		{
			auto Y = y + layer.originOnMonitor.y();
			unsigned char* dstLine = rgba.data() + 4 * (Y * this->wid + X);
			auto dst = dstLine;

			unsigned int inLineVRAMOffset = layer.VRAMHSkipBytes;
			int ZHswitch = 0;
			auto ZH = ZHsrc[ZHswitch];
			for (int x = 0; x < layer.sizeOnMonitor.x() && x + layer.originOnMonitor.x() < this->wid && inLineVRAMOffset < layer.bytesPerLine + RenderOffsetX; x++)
			{
				unsigned int VRAMAddr = lineVRAMOffset + ((inLineVRAMOffset + VRAMOffsetHorizontal) & VRAMHScrollMask);
				VRAMAddr = VRAMBase + ((VRAMAddr + VRAMOffsetVertical) & VRAMVScrollMask);
				OFFSETTRANS::Trans(VRAMAddr);

				unsigned short col16 = cpputil::GetWord(VRAM + VRAMAddr);
				dst[0] = ((col16 & 0b000001111100000) >> 5);
				dst[0] = (dst[0] << 3) | ((dst[0] >> 2) & 7);
				dst[1] = ((col16 & 0b111110000000000) >> 10);
				dst[1] = (dst[1] << 3) | ((dst[1] >> 2) & 7);
				dst[2] = (col16 & 0b000000000011111);
				dst[2] = (dst[2] << 3) | ((dst[2] >> 2) & 7);
				dst[3] = 255;
				dst += 4;
				if (0 == (--ZH))
				{
					ZHswitch = 1 - ZHswitch;
					ZH = ZHsrc[ZHswitch];
					inLineVRAMOffset += 2;
				}
			}

			if (1 < yStep)
			{
				auto copyPtr = dstLine + (4 * this->wid);
				for (unsigned int zv = 1; zv < yStep; ++zv)
				{
					std::memcpy(copyPtr, dstLine, dst - dstLine);
					copyPtr += (4 * this->wid);
				}
				lineVRAMOffset += layer.bytesPerLine;
			}
			else
			{
				--ZV;
				if (0 == ZV)
				{
					ZV = ZV0;
					lineVRAMOffset += layer.bytesPerLine;
				}
			}
		}
	}
	else
	{
		for (int y = 0; y < layer.sizeOnMonitor.y() && y + layer.originOnMonitor.y() <= bottomY; y += yStep)
		{
			auto Y = y + layer.originOnMonitor.y();
			unsigned char* dstLine = rgba.data() + 4 * (Y * this->wid + X);
			auto dst = dstLine;

			unsigned int inLineVRAMOffset = layer.VRAMHSkipBytes;
			int ZHswitch = 0;
			auto ZH = ZHsrc[ZHswitch];
			for (int x = 0; x < layer.sizeOnMonitor.x() && x + layer.originOnMonitor.x() < this->wid && inLineVRAMOffset < layer.bytesPerLine + RenderOffsetX; x++)
			{
				unsigned int VRAMAddr = lineVRAMOffset + ((inLineVRAMOffset + VRAMOffsetHorizontal) & VRAMHScrollMask);
				VRAMAddr = VRAMBase + ((VRAMAddr + VRAMOffsetVertical) & VRAMVScrollMask);
				OFFSETTRANS::Trans(VRAMAddr);

				unsigned short col16 = cpputil::GetWord(VRAM + VRAMAddr);
				if (0 == (col16 & 0x8000))
				{
					dst[0] = ((col16 & 0b000001111100000) >> 5);
					dst[0] = (dst[0] << 3) | ((dst[0] >> 2) & 7);
					dst[1] = ((col16 & 0b111110000000000) >> 10);
					dst[1] = (dst[1] << 3) | ((dst[1] >> 2) & 7);
					dst[2] = (col16 & 0b000000000011111);
					dst[2] = (dst[2] << 3) | ((dst[2] >> 2) & 7);
					dst[3] = 255;
				}
				dst += 4;
				if (0 == (--ZH))
				{
					ZHswitch = 1 - ZHswitch;
					ZH = ZHsrc[ZHswitch];
					inLineVRAMOffset += 2;
				}
			}

			if (1 < yStep)
			{
				auto copyPtr = dstLine + (4 * this->wid);
				for (unsigned int zv = 1; zv < yStep; ++zv)
				{
					std::memcpy(copyPtr, dstLine, dst - dstLine);
					copyPtr += (4 * this->wid);
				}
				lineVRAMOffset += layer.bytesPerLine;
			}
			else
			{
				--ZV;
				if (0 == ZV)
				{
					ZV = ZV0;
					lineVRAMOffset += layer.bytesPerLine;
				}
			}
		}
	}
}

template <class OFFSETTRANS>
void TownsRender::Render24Bit(const TownsCRTC::Layer &layer,const unsigned char VRAM[],bool transparent)
{
	unsigned int VRAMBase=layer.VRAMAddr;
	unsigned int VRAMOffsetVertical=(layer.VRAMOffset+layer.FlipVRAMOffset)&~layer.HScrollMask;
	unsigned int VRAMOffsetHorizontal=(layer.VRAMOffset+layer.FlipVRAMOffset)&layer.HScrollMask;
	const unsigned int VRAMHScrollMask=layer.HScrollMask+1;
	const unsigned int VRAMVScrollMask=layer.VScrollMask;
	unsigned int lineVRAMOffset=0;
	const int ZHsrc[2]={layer.zoom2x.x()/2,(layer.zoom2x.x()+1)/2};  // For x.5 times zoom rate.
	auto ZV0=layer.zoom2x.y()/2;
	auto ZV=ZV0;

	// yStep should be 1 if transparent.
	// If transparnet==true, there is a possibility that memcpy overwrites background pixels.
	unsigned int yStep=(true!=transparent ? ZV0 : 1);
	auto bottomY=this->hei-yStep;
	for(int y=0; y<layer.sizeOnMonitor.y() && y+layer.originOnMonitor.y()<=bottomY; y+=yStep)
	{
		auto X=  layer.originOnMonitor.x();
		auto Y=y+layer.originOnMonitor.y();
		unsigned char *dstLine=rgba.data()+4*(Y*this->wid+X);
		auto dst=dstLine;

		unsigned int inLineVRAMOffset=layer.VRAMHSkipBytes;
		int ZHswitch=0;
		auto ZH=ZHsrc[ZHswitch];
		for(int x=0; x<layer.sizeOnMonitor.x() && x+layer.originOnMonitor.x()<this->wid && inLineVRAMOffset<layer.bytesPerLine; x++)
		{
			unsigned int VRAMAddr=lineVRAMOffset+((inLineVRAMOffset+VRAMOffsetHorizontal)%VRAMHScrollMask);
			VRAMAddr=VRAMBase+((VRAMAddr+VRAMOffsetVertical)&VRAMVScrollMask);
			OFFSETTRANS::Trans(VRAMAddr);

			dst[0]=VRAM[VRAMAddr];
			dst[1]=VRAM[VRAMAddr+1];
			dst[2]=VRAM[VRAMAddr+2];
			dst[3]=255;

			dst+=4;
			if(0==(--ZH))
			{
				ZHswitch=1-ZHswitch;
				ZH=ZHsrc[ZHswitch];
				inLineVRAMOffset+=3;
			}
		}

		if(1<yStep)
		{
			auto copyPtr=dstLine+(4*this->wid);
			for(unsigned int zv=1; zv<yStep; ++zv)
			{
				std::memcpy(copyPtr,dstLine,dst-dstLine);
				copyPtr+=(4*this->wid);
			}
			lineVRAMOffset+=layer.bytesPerLine;
		}
		else
		{
			--ZV;
			if(0==ZV)
			{
				ZV=ZV0;
				lineVRAMOffset+=layer.bytesPerLine;
			}
		}
	}
}
