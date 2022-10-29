/* ////////////////////////////////////////////////////////////

File Name: fsmacosxwrappercpp.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "fssimplewindow.h"

extern "C" void FsOpenWindowC(int x0,int y0,int wid,int hei,int useDoubleBuffer,int useMultiSampleBuffer,int fullScreen,const char windowName[]);
extern "C" void FsResizeWindowC(int wid,int hei);
extern "C" void FsGetWindowSizeC(int *wid,int *hei);
extern "C" void FsGetWindowPositionC(int *wid,int *hei);
extern "C" void FsMaximizeWindowC(void);
extern "C" void FsUnmaximizeWindowC(void);
extern "C" void FsMakeFullScreenC(void);
extern "C" void FsPollDeviceC(void);
extern "C" void FsPushOnPaintEventC(void);
extern "C" void FsOnInitializeOpenGLC(void);
extern "C" void FsSleepC(int ms);
extern "C" int FsPassedTimeC(void);
extern "C" long long int FsSubSecondTimerC(void);
extern "C" void FsMouseC(int *lb,int *mb,int *rb,int *mx,int *my);
extern "C" void FsSetMousePositionC(int mx,int my);
extern "C" int FsGetMouseEventC(int *lb,int *mb,int *rb,int *mx,int *my);
extern "C" void FsSwapBufferC(void);
extern "C" int FsInkeyC(void);
extern "C" int FsInkeyCharC(void);
extern "C" int FsKeyStateC(int fsKeyCode);
extern "C" void FsChangeToProgramDirC(void);
extern "C" int FsCheckExposureC(void);
extern "C" void FsPushKeyC(int fsKeyCode);
extern "C" void FsEnableIMEC(void);
extern "C" void FsDisableIMEC(void);


static int doubleBuffer=1;
static int windowOpen=0;

void FsOpenWindow(const FsOpenWindowOption &opt)
{
	if(0!=windowOpen)
	{
		printf("Error! Window is already open.\n");
		exit(1);
	}

	const int useDoubleBuffer=(int)opt.useDoubleBuffer;
	const int useMultiSampleBuffer=(int)opt.useMultiSampleBuffer;
	const int fullScreen=(FsOpenWindowOption::FULLSCREEN==opt.sizeOpt ? 1 : 0);

	if(NULL==opt.windowTitle)
	{
		FsOpenWindowC(opt.x0,opt.y0,opt.wid,opt.hei,useDoubleBuffer,useMultiSampleBuffer,fullScreen,"Main Window");
	}
	else
	{
		FsOpenWindowC(opt.x0,opt.y0,opt.wid,opt.hei,useDoubleBuffer,useMultiSampleBuffer,fullScreen,opt.windowTitle);
	}

	FsPassedTimeC();  // Resetting the timer.
	doubleBuffer=useDoubleBuffer;
	windowOpen=1;

	if(NULL!=fsAfterWindowCreationCallBack)
	{
		(*fsAfterWindowCreationCallBack)(fsAfterWindowCreationCallBackParam);
	}
}

void FsResizeWindow(int wid,int hei)
{
	FsResizeWindowC(wid,hei);
}

int FsCheckWindowOpen(void)
{
	return windowOpen;
}

void FsPushOnPaintEvent(void)
{
	FsPushOnPaintEventC();
}

void FsCloseWindow(void)
{
}

void FsGetWindowSize(int &wid,int &hei)
{
	FsGetWindowSizeC(&wid,&hei);
}

void FsGetWindowPosition(int &x0,int &y0)
{
	FsGetWindowPositionC(&x0,&y0);
}

void FsSetWindowTitle(const char windowTitle[])
{
	printf("Sorry. %s not supported on this platform yet\n",__FUNCTION__);
}

void FsMaximizeWindow(void)
{
	FsMaximizeWindowC();
}
void FsUnmaximizeWindow(void)
{
	FsUnmaximizeWindowC();
}
void FsMakeFullScreen(void)
{
	FsMakeFullScreenC();
}

void FsPollDevice(void)
{
	FsPollDeviceC();
}

void FsSleep(int ms)
{
	FsSleepC(ms);
}

long long int FsPassedTime(void)
{
	static long long int lastTick;
	long long int tick;

	static int first=1;
	if(1==first)
	{
		lastTick=FsSubSecondTimer();
		first=0;
	}

	tick=FsSubSecondTimer();
	long long passed=tick-lastTick;
	lastTick=tick;

	return passed;
}

long long int FsSubSecondTimer(void)
{
	return FsSubSecondTimerC();
}

void FsSetMousePosition(int mx,int my)
{
	FsSetMousePositionC(mx,my);
}

void FsGetMouseState(int &lb,int &mb,int &rb,int &mx,int &my)
{
	FsMouseC(&lb,&mb,&rb,&mx,&my);
}

int FsGetMouseEvent(int &lb,int &mb,int &rb,int &mx,int &my)
{
	return FsGetMouseEventC(&lb,&mb,&rb,&mx,&my);
}

void FsSwapBuffers(void)
{
	if(0==doubleBuffer)
	{
		printf("Error! FsSwapBufferes called in a single-buffered application.\n");
		exit(1);
	}

	FsSwapBufferC();
}

int FsInkey(void)
{
	return FsInkeyC();
}

int FsInkeyChar(void)
{
	return FsInkeyCharC();
}

void FsPushKey(int fskeyCode)
{
	FsPushKeyC(fskeyCode);
}

int FsGetKeyState(int fsKeyCode)
{
	return FsKeyStateC(fsKeyCode);
}

int FsCheckWindowExposure(void)
{
	return FsCheckExposureC();
}

void FsChangeToProgramDir(void)
{
	FsChangeToProgramDirC();
}

extern "C" void FsOnPaintCallBackCpp(void)
{
	if(NULL!=fsOnPaintCallback)
	{
		(*fsOnPaintCallback)(fsOnPaintCallbackParam);
	}
}

extern "C" void FsOnInitializeOpenGLC(void)
{
	if(NULL!=fsOpenGLInitializationCallBack)
	{
		(*fsOpenGLInitializationCallBack)(fsOpenGLInitializationCallBackParam);
	}
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
	FsEnableIMEC();
	return 1;
}

void FsDisableIME(void)
{
	FsDisableIMEC();
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

void FsShowMouseCursor(int showFlag)
{
}
int FsIsMouseCursorVisible(void)
{
	return 1;
}
