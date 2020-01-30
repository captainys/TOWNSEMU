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

	int priorityPage=0;
	SetResolution(640,400);
	unsigned int page0Ptr=0;
	unsigned int page1Ptr=0x40000;
	for(unsigned int ctr=0; ctr<640*400; ctr+=2)
	{
		unsigned char vrambyte[2]=
		{
			physMem.state.VRAM[page0Ptr++],
			physMem.state.VRAM[page1Ptr++]
		};
		unsigned char pix4bit[2]=
		{
			(unsigned char)((vrambyte[0]&0xf0)>>4),
			(unsigned char)((vrambyte[1]&0xf0)>>4),
		};
		unsigned char pix=(pix4bit[priorityPage]!=0 ? pix4bit[priorityPage] : pix4bit[1-priorityPage]);
		rgba[ctr*4  ]=palette[pix][0];
		rgba[ctr*4+1]=palette[pix][1];
		rgba[ctr*4+2]=palette[pix][2];
		rgba[ctr*4+3]=palette[pix][3];

		pix4bit[0]=(vrambyte[0]&0x0f);
		pix4bit[1]=(vrambyte[1]&0x0f);
		pix=(pix4bit[priorityPage]!=0 ? pix4bit[priorityPage] : pix4bit[1-priorityPage]);
		rgba[ctr*4+4]=palette[pix][0];
		rgba[ctr*4+5]=palette[pix][1];
		rgba[ctr*4+6]=palette[pix][2];
		rgba[ctr*4+7]=palette[pix][3];
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
