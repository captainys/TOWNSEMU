/* ////////////////////////////////////////////////////////////

File Name: fssimplewindowcommon.cpp
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
#include <string.h>
#include <chrono>

#include "fssimplewindow.h"


FsOpenWindowOption::FsOpenWindowOption()
{
	x0=16;
	y0=16;
	wid=800;
	hei=600;

	windowTitle="Main Window";

	useDoubleBuffer=false;
	useMultiSampleBuffer=false;

	sizeOpt=NORMAL_WINDOW;
}

void FsOpenWindow(int x0,int y0,int wid,int hei,int useDoubleBuffer)
{
	FsOpenWindow(x0,y0,wid,hei,useDoubleBuffer,NULL);
}

void FsOpenWindow(int x0,int y0,int wid,int hei,int useDoubleBuffer,const char windowName[])
{
	FsOpenWindowOption opt;
	opt.x0=x0;
	opt.y0=y0;
	opt.wid=wid;
	opt.hei=hei;
	opt.windowTitle=windowName;
	opt.useDoubleBuffer=(bool)useDoubleBuffer;
	FsOpenWindow(opt);
}

void FsClearEventQueue(void)
{
	auto intervalFunc=fsIntervalCallBack;
	auto intervalParam=fsIntervalCallBackParameter;
	FsRegisterIntervalCallBack(NULL,NULL);

	for(;;)
	{
		int checkAgain=0;

		FsPollDevice();

		int lb,mb,rb,mx,my;
		while(FSMOUSEEVENT_NONE!=FsGetMouseEvent(lb,mb,rb,mx,my) ||
		      FSKEY_NULL!=FsInkey() ||
		      0!=FsInkeyChar() ||
		      0!=FsCheckWindowExposure())
		{
			checkAgain=1;
		}

		if(0!=lb || 0!=rb || 0!=mb)
		{
			checkAgain=1;
		}

		if(0==checkAgain)
		{
			break;
		}

		FsSleep(50);
	}

	// Win32 has a bug that VK_KANJI is falsely detected as held down.
	// To avoid infinite loop, Windows requires timeout.
	// Probably Microsoft won't address this bug forever.
	auto t0=std::chrono::system_clock::now();
	for(;;)
	{
		FsPollDevice();
		if(0==FsCheckKeyHeldDown())
		{
			break;
		}

	#ifdef WIN32
		auto dt=std::chrono::system_clock::now()-t0;
		if(500<std::chrono::duration_cast<std::chrono::milliseconds>(dt).count())
		{
			break;
		}
	#endif

		FsSleep(50);
	}

	FsRegisterIntervalCallBack(intervalFunc,intervalParam);
}

int FsCheckKeyHeldDown(void)
{
	int keyCode;
	for(keyCode=FSKEY_NULL+1; keyCode<FSKEY_NUM_KEYCODE; keyCode++)
	{
		if(0!=FsGetKeyState(keyCode))
		{
			return 1;
		}
	}
	return 0;
}


static const char *const keyCodeToStr[]=
{
	"NULL",
	"SPACE",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"ESC",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"PRINTSCRN",
	"CAPSLOCK",
	"SCROLLLOCK",
	"PAUSEBREAK",
	"BS",
	"TAB",
	"ENTER",
	"SHIFT",
	"CTRL",
	"ALT",
	"INS",
	"DEL",
	"HOME",
	"END",
	"PAGEUP",
	"PAGEDOWN",
	"UP",
	"DOWN",
	"LEFT",
	"RIGHT",
	"NUMLOCK",
	"TILDA",
	"MINUS",
	"PLUS",
	"LBRACKET",
	"RBRACKET",
	"BACKSLASH",
	"SEMICOLON",
	"SINGLEQUOTE",
	"COMMA",
	"DOT",
	"SLASH",
	"TEN0",
	"TEN1",
	"TEN2",
	"TEN3",
	"TEN4",
	"TEN5",
	"TEN6",
	"TEN7",
	"TEN8",
	"TEN9",
	"TENDOT",
	"TENSLASH",
	"TENSTAR",
	"TENMINUS",
	"TENPLUS",
	"TENENTER",
	"WHEELUP",
	"WHEELDOWN",
	"CONTEXT",

	// Japanese keyboard
	"CONVERT",
	"NONCONVERT",
	"KANA",       // Japanese JIS Keyboard Only => Win32 VK_KANA
	"RO",         // Japanese JIS Keyboard Only => Win32 VK_OEM_102
	"ZENKAKU",    // Japanese JIS Keyboard Only => Full Pitch/Half Pitch

	// The following key codes won't be returned by FsInkey()
	// These may return non zero for FsGetKeyState
	"LEFT_CTRL",
	"RIGHT_CTRL",
	"LEFT_SHIFT",
	"RIGHT_SHIFT",
	"LEFT_ALT",
	"RIGHT_ALT",
};

const char *FsKeyCodeToString(int keyCode)
{
	if(0<=keyCode && keyCode<FSKEY_NUM_KEYCODE)
	{
		return keyCodeToStr[keyCode];
	}
	return "(Undefined keycode)";
}

int FsStringToKeyCode(const char str[])
{
	if(nullptr==str)
	{
		return FSKEY_NULL;
	}

	char upper[256];
	for(int i=0; i<255 && 0!=str[i]; ++i)
	{
		upper[i  ]=str[i];
		upper[i+1]=0;
		if('a'<=upper[i] && upper[i]<='z')
		{
			upper[i]=upper[i]+'A'-'a';
		}
	}
	if(0==strcmp("COLON",upper))
	{
		return FSKEY_COLON;
	}
	for(int i=0; i<FSKEY_NUM_KEYCODE; ++i)
	{
		if(0==strcmp(upper,keyCodeToStr[i]))
		{
			return i;
		}
	}
	return FSKEY_NULL;
}


void *fsCloseWindowCallBackParam=NULL;
bool (*fsCloseWindowCallBack)(void *param)=NULL;

void *fsOpenGLContextCreationCallBackParam=NULL;
bool (*fsOpenGLContextCreationCallBack)(void *param)=NULL;

void *fsAfterWindowCreationCallBackParam=NULL;
void (*fsAfterWindowCreationCallBack)(void *param)=NULL;

void *fsOpenGLInitializationCallBackParam=NULL;
bool (*fsOpenGLInitializationCallBack)(void *param)=NULL;

void *fsSwapBuffersHookParam=NULL;
bool (*fsSwapBuffersHook)(void *param)=NULL;

void *fsOnPaintCallbackParam=NULL;
void (*fsOnPaintCallback)(void *param)=NULL;

void *fsOnResizeCallBackParam=NULL;
void (*fsOnResizeCallBack)(void *param,int wid,int hei)=NULL;

void (*fsIntervalCallBack)(void *)=NULL;
void *fsIntervalCallBackParameter=NULL;

void (*fsPollDeviceHook)(void *)=NULL;
void *fsPollDeviceHookParam=NULL;

void FsRegisterCloseWindowCallBack(bool (*callback)(void *),void *param)
{
	fsCloseWindowCallBack=callback;
	fsCloseWindowCallBackParam=param;
}

void FsRegisterBeforeOpenGLContextCreationCallBack(bool (*callback)(void *),void *param)
{
	fsOpenGLContextCreationCallBack=callback;
	fsOpenGLContextCreationCallBackParam=param;
}

void FsRegisterAfterWindowCreationCallBack(void (*callback)(void *),void *param)
{
	fsAfterWindowCreationCallBackParam=param;
	fsAfterWindowCreationCallBack=callback;
}

void FsRegisterOpenGLInitializationCallBack(bool (*callback)(void *),void *param)
{
	fsOpenGLInitializationCallBack=callback;
	fsOpenGLInitializationCallBackParam=param;
}

void FsRegisterSwapBuffersCallBack(bool (*callback)(void *),void *param)
{
	fsSwapBuffersHookParam=param;
	fsSwapBuffersHook=callback;
}

void FsRegisterOnPaintCallBack(void (*callback)(void *),void *param)
{
	fsOnPaintCallbackParam=param;
	fsOnPaintCallback=callback;
}

void FsRegisterWindowResizeCallBack(void (*callback)(void *,int wid,int hei),void *param)
{
	fsOnResizeCallBackParam=param;
	fsOnResizeCallBack=callback;
}

void FsRegisterIntervalCallBack(void (*callback)(void *),void *param)
{
	fsIntervalCallBack=callback;
	fsIntervalCallBackParameter=param;
}

void FsRegisterPollDeviceCallBack(void (*callback)(void *),void *param)
{
	fsPollDeviceHook=callback;
	fsPollDeviceHookParam=param;
}

