/* ////////////////////////////////////////////////////////////

File Name: fsuwpwrapper.cpp
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



#include "../fssimplewindow.h"
#include <stdio.h>
#include <agile.h>


using namespace Windows::ApplicationModel::Core;


extern void FsUWPReportWindowSize(int wid,int hei);
extern void FsUWPMakeReportWindowSize(void);



class FsMouseEventLog
{
public:
	int eventType,lb,mb,rb,mx,my;
};

#define NKEYBUF 256
static int keyBuffer[NKEYBUF];
static int nKeyBufUsed=0;
static int charBuffer[NKEYBUF];
static int nCharBufUsed=0;

// Mouse Emulation
#define NEVTBUF 256
static int nMosBufUsed=0;
static FsMouseEventLog mosBuffer[NEVTBUF];
static FsMouseEventLog lastKnownMos;
static FsVec2i touchCache[NEVTBUF];

static int exposure=0;


void FsPushKey(int fskey)
{
	if(nKeyBufUsed<NKEYBUF)
	{
		keyBuffer[nKeyBufUsed++]=fskey;
	}
}

void FsOpenWindow(const FsOpenWindowOption &)
{
}

void FsCloseWindow(void)
{
}

void FsMaximizeWindow(void)
{
}
void FsUnmaximizeWindow(void)
{
}
void FsMakeFullScreen(void)
{
}

static int winWid=0,winHei=0;

void FsUWPReportWindowSize(int wid,int hei)
{
	winWid=wid;
	winHei=hei;
}

void FsUWPReportMouseEvent(int evt,int lb,int mb,int rb,int mx,int my)
{
	if(nMosBufUsed<NEVTBUF)
	{
		mosBuffer[nMosBufUsed].eventType=evt;
		mosBuffer[nMosBufUsed].lb=lb;
		mosBuffer[nMosBufUsed].mb=mb;
		mosBuffer[nMosBufUsed].rb=rb;
		mosBuffer[nMosBufUsed].mx=mx;
		mosBuffer[nMosBufUsed].my=my;

		lastKnownMos=mosBuffer[nMosBufUsed];

		nMosBufUsed++;
	}
}

void FsUWPReportCharIn(int unicode)
{
	if(nCharBufUsed<NKEYBUF)
	{
		charBuffer[nCharBufUsed]=unicode;
		++nCharBufUsed;
	}
}

void FsGetWindowSize(int &wid,int &hei)
{
	FsUWPMakeReportWindowSize();
	wid=winWid;
	hei=winHei;
}

void FsGetWindowPosition(int &x0,int &y0)
{
	x0=0;
	y0=0;
}

void FsSetWindowTitle(const char [])
{
}

void FsPushOnPaintEvent(void)
{
}

void FsPollDevice(void)
{
}

void FsSleep(int ms)
{
}

long long int FsSubSecondTimer(void)
{
	return 0;
}

long long int FsPassedTime(void)
{
	return 10;
}

void FsGetMouseState(int &lb,int &mb,int &rb,int &mx,int &my)
{
	lb=lastKnownMos.lb;
	mb=lastKnownMos.mb;
	rb=lastKnownMos.rb;
	mx=lastKnownMos.mx;
	my=lastKnownMos.my;
}

int FsGetMouseEvent(int &lb,int &mb,int &rb,int &mx,int &my)
{
	if(0<nMosBufUsed)
	{
		const int eventType=mosBuffer[0].eventType;
		lb=mosBuffer[0].lb;
		mb=mosBuffer[0].mb;
		rb=mosBuffer[0].rb;
		mx=mosBuffer[0].mx;
		my=mosBuffer[0].my;

		for(int i=0; i<nMosBufUsed; ++i)
		{
			mosBuffer[i]=mosBuffer[i+1];
		}
		--nMosBufUsed;
		return eventType;
	}
	else 
	{
		FsGetMouseState(lb,mb,rb,mx,my);
		return FSMOUSEEVENT_NONE;
	}
}

void FsSwapBuffers(void)
{
}

int FsInkey(void)
{
	if(nKeyBufUsed>0)
	{
		int i,keyCode;
		keyCode=keyBuffer[0];
		nKeyBufUsed--;
		for(i=0; i<nKeyBufUsed; i++)
		{
			keyBuffer[i]=keyBuffer[i+1];
		}
		return keyCode;
	}
	return 0;
}

int FsInkeyChar(void)
{
	if(nCharBufUsed>0)
	{
		int i,asciiCode;
		asciiCode=charBuffer[0];
		nCharBufUsed--;
		for(i=0; i<nCharBufUsed; i++)
		{
			charBuffer[i]=charBuffer[i+1];
		}
		return asciiCode;
	}
	return 0;
}

int FsGetKeyState(int fsKeyCode)
{
	return 0;
}

int FsCheckWindowExposure(void)
{
	auto e=exposure;
	exposure=0;
	return e;
}

void FsChangeToProgramDir(void)
{
	auto package=Windows::ApplicationModel::Package::Current;
	auto instDir=package->InstalledLocation->Path->Data();

	printf("%s==%ls\n",__FUNCTION__,(const wchar_t *)instDir);
	_wchdir(instDir);
	_wchdir(L"Assets");
}

int FsGetNumCurrentTouch(void)
{
	return 0;
}

const FsVec2i *FsGetCurrentTouch(void)
{
	return nullptr;
}

////////////////////////////////////////////////////////////

int FsEnableIME(void)
{
	return 0;
}

void FsDisableIME(void)
{
}

////////////////////////////////////////////////////////////


int FsIsNativeTextInputAvailable(void)
{
	return 0;
}

int FsOpenNativeTextInput(int x1,int y1,int wid,int hei)
{
	return 0;
}

void FsCloseNativeTextInput(void)
{
}

void FsSetNativeTextInputText(const wchar_t [])
{
}

int FsGetNativeTextInputTextLength(void)
{
	return 0;
}

void FsGetNativeTextInputText(wchar_t str[],int bufLen)
{
	if(0<bufLen)
	{
		str[0]=0;
	}
}

int FsGetNativeTextInputEvent(void)
{
	return FSNATIVETEXTEVENT_NONE;
}
