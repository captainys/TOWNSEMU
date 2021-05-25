/* ////////////////////////////////////////////////////////////

File Name: fsguimain.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include <fslazywindow.h>
#include <ysgl.h>

// Comment out FSGUI_USE_SYSTEM_FONT to remove depencency on YsSystemFont library.
#define FSGUI_USE_SYSTEM_FONT

// Comment out FSGUI_USE_MODERN_UI to use primitive graphics, which may be faster.
#define FSGUI_USE_MODERN_UI



#ifdef FSGUI_USE_SYSTEM_FONT
#include <ysfontrenderer.h>
#endif

#include "fsguiapp.h"



class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
public:
	static FsLazyWindowApplication *currentApplication;
	YsSystemFontRenderer *sysFont;

	FsLazyWindowApplication();
	~FsLazyWindowApplication();

	virtual void BeforeEverything(int ac,char *av[]);
	virtual void GetOpenWindowOption(FsOpenWindowOption &opt) const;
	virtual void Initialize(int ac,char *av[]);
	virtual void Interval(void);
	virtual void BeforeTerminate(void);
	virtual void Draw(void);
	virtual bool NeedRedraw(void) const;
	virtual bool MustTerminate(void) const;
	virtual long long int GetMinimumSleepPerInterval(void) const;
};

FsLazyWindowApplication::FsLazyWindowApplication()
{
	sysFont=nullptr;
}
FsLazyWindowApplication::~FsLazyWindowApplication()
{
	if(nullptr!=sysFont)
	{
		delete sysFont;
		sysFont=nullptr;
	}
}

/* virtual */ void FsLazyWindowApplication::BeforeEverything(int,char *[])
{
}

/* virtual */ void FsLazyWindowApplication::GetOpenWindowOption(FsOpenWindowOption &opt) const
{
	opt.x0=0;
	opt.y0=0;
	opt.wid=1024;
	opt.hei=768;
}

/* virtual */ void FsLazyWindowApplication::Initialize(int ac,char *av[])
{
#ifdef FSGUI_USE_SYSTEM_FONT
	sysFont=new YsSystemFontRenderer;
	FsGuiObject::defUnicodeRenderer=sysFont;
#endif

#ifdef FSGUI_USE_MODERN_UI
	FsGuiObject::scheme=FsGuiObject::MODERN;
	FsGuiObject::defRoundRadius=8.0;
	FsGuiObject::defHScrollBar=20;
	FsGuiObject::defHAnnotation=14;
	FsGuiObject::defVSpaceUnit=12;

	FsGuiObject::defDialogBgCol.SetDoubleRGB(0.75,0.75,0.75);

	FsGuiObject::defTabBgCol.SetDoubleRGB(0.82,0.82,0.82);
	FsGuiObject::defTabClosedFgCol.SetDoubleRGB(0.8,0.8,0.8);
	FsGuiObject::defTabClosedBgCol.SetDoubleRGB(0.2,0.2,0.2);

	FsGuiObject::defBgCol.SetDoubleRGB(0.85,0.85,0.85);
	FsGuiObject::defFgCol.SetDoubleRGB(0.0,0.0,0.0);
	FsGuiObject::defActiveBgCol.SetDoubleRGB(0.3,0.3,0.7);
	FsGuiObject::defActiveFgCol.SetDoubleRGB(1.0,1.0,1.0);
	FsGuiObject::defFrameCol.SetDoubleRGB(0.0,0.0,0.0);

	FsGuiObject::defListFgCol.SetDoubleRGB(0.0,0.0,0.0);
	FsGuiObject::defListBgCol.SetDoubleRGB(0.8,0.8,0.8);
	FsGuiObject::defListActiveFgCol.SetDoubleRGB(1.0,1.0,1.0);
	FsGuiObject::defListActiveBgCol.SetDoubleRGB(0.3,0.3,0.7);
#endif

	FsGuiObject::defUnicodeRenderer->RequestDefaultFontWithPixelHeight(16);
	FsGuiObject::defAsciiRenderer->RequestDefaultFontWithPixelHeight(16);

	YsCoordSysModel=YSOPENGL;

	// This should be the first call of FsGuiMainCanvas::GetMainCanvas(),
	// in which the application is created.
	FsGuiMainCanvas::GetMainCanvas()->Initialize(ac,av);
}

/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	FsGuiMainCanvas::GetMainCanvas()->OnInterval();
}

/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	YsGLSLDeleteSharedRenderer();
	FsGuiMainCanvas::DeleteMainCanvas();
}

/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	FsGuiMainCanvas::GetMainCanvas()->Draw();
}

/* virtual */ bool FsLazyWindowApplication::NeedRedraw(void) const
{
	return (bool)FsGuiMainCanvas::GetMainCanvas()->NeedRedraw();
}

/* virtual */ bool FsLazyWindowApplication::MustTerminate(void) const
{
	return (bool)FsGuiMainCanvas::GetMainCanvas()->appMustTerminate;
}



FsLazyWindowApplication *FsLazyWindowApplication::currentApplication=nullptr;

/* static */ FsLazyWindowApplicationBase *FsLazyWindowApplicationBase::GetApplication(void)
{
	if(nullptr==FsLazyWindowApplication::currentApplication)
	{
		FsLazyWindowApplication::currentApplication=new FsLazyWindowApplication;
	}
	return FsLazyWindowApplication::currentApplication;
}

/* virtual */ long long int FsLazyWindowApplication::GetMinimumSleepPerInterval(void) const
{
	return 60;
}
