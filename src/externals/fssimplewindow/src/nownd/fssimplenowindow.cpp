/* ////////////////////////////////////////////////////////////

File Name: fssimplenowindow.cpp
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
#include <time.h>

#ifdef _WIN32
	#include <io.h>
	#include <conio.h>
#elif defined(__APPLE__)
	#include <sys/ioctl.h>
	#include <sgtty.h>
	#include <sys/file.h>
	#include <sys/types.h>
	#include <sys/poll.h>
	#include <sys/stat.h>
	#include <dirent.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <signal.h>
	#include <time.h>
#else
	#include <sys/ioctl.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/poll.h>
	#include <sys/stat.h>
	#include <dirent.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <termio.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
#endif

#include "fssimplewindow.h"



static const int FS_NUM_VK=65536;
static int mapVKtoFSKEY[FS_NUM_VK];
static int mapFSKEYtoVK[FSKEY_NUM_KEYCODE];

#ifdef __APPLE__
static struct sgttyb OriginalConsoleSetting;
#elif !defined(_WIN32)
struct termio OriginalConsoleSetting;
#endif



////////////////////////////////////////////////////////////

static void FsAddKeyMapping(int fskey,int vk)
{
	mapVKtoFSKEY[vk]=fskey;
	mapFSKEYtoVK[fskey]=vk;
}

static void FsCreateKeyMapping(void)
{
	int i;
	for(i=0; i<FS_NUM_VK; i++)
	{
		mapVKtoFSKEY[i]=0;
	}
	for(i=0; i<FSKEY_NUM_KEYCODE; i++)
	{
		mapFSKEYtoVK[i]=0;
	}

	FsAddKeyMapping(FSKEY_SPACE,               0x20);
	FsAddKeyMapping(FSKEY_0,                   '0');
	FsAddKeyMapping(FSKEY_1,                   '1');
	FsAddKeyMapping(FSKEY_2,                   '2');
	FsAddKeyMapping(FSKEY_3,                   '3');
	FsAddKeyMapping(FSKEY_4,                   '4');
	FsAddKeyMapping(FSKEY_5,                   '5');
	FsAddKeyMapping(FSKEY_6,                   '6');
	FsAddKeyMapping(FSKEY_7,                   '7');
	FsAddKeyMapping(FSKEY_8,                   '8');
	FsAddKeyMapping(FSKEY_9,                   '9');

	FsAddKeyMapping(FSKEY_A,                   'a');
	FsAddKeyMapping(FSKEY_B,                   'b');
	FsAddKeyMapping(FSKEY_C,                   'c');
	FsAddKeyMapping(FSKEY_D,                   'd');
	FsAddKeyMapping(FSKEY_E,                   'e');
	FsAddKeyMapping(FSKEY_F,                   'f');
	FsAddKeyMapping(FSKEY_G,                   'g');
	FsAddKeyMapping(FSKEY_H,                   'h');
	FsAddKeyMapping(FSKEY_I,                   'i');
	FsAddKeyMapping(FSKEY_J,                   'j');
	FsAddKeyMapping(FSKEY_K,                   'k');
	FsAddKeyMapping(FSKEY_L,                   'l');
	FsAddKeyMapping(FSKEY_M,                   'm');
	FsAddKeyMapping(FSKEY_N,                   'n');
	FsAddKeyMapping(FSKEY_O,                   'o');
	FsAddKeyMapping(FSKEY_P,                   'p');
	FsAddKeyMapping(FSKEY_Q,                   'q');
	FsAddKeyMapping(FSKEY_R,                   'r');
	FsAddKeyMapping(FSKEY_S,                   's');
	FsAddKeyMapping(FSKEY_T,                   't');
	FsAddKeyMapping(FSKEY_U,                   'u');
	FsAddKeyMapping(FSKEY_V,                   'v');
	FsAddKeyMapping(FSKEY_W,                   'w');
	FsAddKeyMapping(FSKEY_X,                   'x');
	FsAddKeyMapping(FSKEY_Y,                   'y');
	FsAddKeyMapping(FSKEY_Z,                   'z');

	FsAddKeyMapping(FSKEY_A,                   'A');
	FsAddKeyMapping(FSKEY_B,                   'B');
	FsAddKeyMapping(FSKEY_C,                   'C');
	FsAddKeyMapping(FSKEY_D,                   'D');
	FsAddKeyMapping(FSKEY_E,                   'E');
	FsAddKeyMapping(FSKEY_F,                   'F');
	FsAddKeyMapping(FSKEY_G,                   'G');
	FsAddKeyMapping(FSKEY_H,                   'H');
	FsAddKeyMapping(FSKEY_I,                   'I');
	FsAddKeyMapping(FSKEY_J,                   'J');
	FsAddKeyMapping(FSKEY_K,                   'K');
	FsAddKeyMapping(FSKEY_L,                   'L');
	FsAddKeyMapping(FSKEY_M,                   'M');
	FsAddKeyMapping(FSKEY_N,                   'N');
	FsAddKeyMapping(FSKEY_O,                   'O');
	FsAddKeyMapping(FSKEY_P,                   'P');
	FsAddKeyMapping(FSKEY_Q,                   'Q');
	FsAddKeyMapping(FSKEY_R,                   'R');
	FsAddKeyMapping(FSKEY_S,                   'S');
	FsAddKeyMapping(FSKEY_T,                   'T');
	FsAddKeyMapping(FSKEY_U,                   'U');
	FsAddKeyMapping(FSKEY_V,                   'V');
	FsAddKeyMapping(FSKEY_W,                   'W');
	FsAddKeyMapping(FSKEY_X,                   'X');
	FsAddKeyMapping(FSKEY_Y,                   'Y');
	FsAddKeyMapping(FSKEY_Z,                   'Z');

	FsAddKeyMapping(FSKEY_ESC,                 0x1b);
	FsAddKeyMapping(FSKEY_TILDA,               0x0060);
	FsAddKeyMapping(FSKEY_BS,                  0x0008);
	FsAddKeyMapping(FSKEY_TAB,                 0x0009);

	FsAddKeyMapping(FSKEY_MINUS,               '-');
	FsAddKeyMapping(FSKEY_PLUS,                '=');
	FsAddKeyMapping(FSKEY_LBRACKET,            '[');
	FsAddKeyMapping(FSKEY_RBRACKET,            ']');
	FsAddKeyMapping(FSKEY_BACKSLASH,           '\\');
	FsAddKeyMapping(FSKEY_SEMICOLON,           ';');
	FsAddKeyMapping(FSKEY_SINGLEQUOTE,         '\'');
#ifdef _WIN32
	FsAddKeyMapping(FSKEY_ENTER,               0x000d);
#else
	FsAddKeyMapping(FSKEY_ENTER,               0x000a);
#endif
	FsAddKeyMapping(FSKEY_COMMA,               ',');
	FsAddKeyMapping(FSKEY_DOT,                 '.');
	FsAddKeyMapping(FSKEY_SLASH,               '/');

	// FsAddKeyMapping(FSKEY_F1,                  VK_F1);
	// FsAddKeyMapping(FSKEY_F2,                  VK_F2);
	// FsAddKeyMapping(FSKEY_F3,                  VK_F3);
	// FsAddKeyMapping(FSKEY_F4,                  VK_F4);
	// FsAddKeyMapping(FSKEY_F5,                  VK_F5);
	// FsAddKeyMapping(FSKEY_F6,                  VK_F6);
	// FsAddKeyMapping(FSKEY_F7,                  VK_F7);
	// FsAddKeyMapping(FSKEY_F8,                  VK_F8);
	// FsAddKeyMapping(FSKEY_F9,                  VK_F9);
	// FsAddKeyMapping(FSKEY_F10,                 VK_F10);
	FsAddKeyMapping(FSKEY_F11,                 0xe085);
	FsAddKeyMapping(FSKEY_F12,                 0xe086);
	// FsAddKeyMapping(FSKEY_PRINTSCRN,           0 /* Unassignable */);
	// FsAddKeyMapping(FSKEY_SCROLLLOCK,          VK_SCROLL);
	// FsAddKeyMapping(FSKEY_PAUSEBREAK,          VK_PAUSE);
	// FsAddKeyMapping(FSKEY_CAPSLOCK,            VK_CAPITAL);
	// FsAddKeyMapping(FSKEY_SHIFT,               VK_SHIFT);
	// FsAddKeyMapping(FSKEY_CTRL,                VK_CONTROL);
	// FsAddKeyMapping(FSKEY_ALT,                 0 /* Unassignable */);
	FsAddKeyMapping(FSKEY_INS,                 0xe052);
	FsAddKeyMapping(FSKEY_DEL,                 0xe053);
	FsAddKeyMapping(FSKEY_HOME,                0xe047);
	FsAddKeyMapping(FSKEY_END,                 0xe04f);
	FsAddKeyMapping(FSKEY_PAGEUP,              0xe049);
	FsAddKeyMapping(FSKEY_PAGEDOWN,            0xe051);
	FsAddKeyMapping(FSKEY_UP,                  0xe048);
	FsAddKeyMapping(FSKEY_DOWN,                0xe050);
	FsAddKeyMapping(FSKEY_LEFT,                0xe04b);
	FsAddKeyMapping(FSKEY_RIGHT,               0xe04d);
	// FsAddKeyMapping(FSKEY_NUMLOCK,             VK_NUMLOCK);
	// FsAddKeyMapping(FSKEY_TEN0,                VK_NUMPAD0);
	// FsAddKeyMapping(FSKEY_TEN1,                VK_NUMPAD1);
	// FsAddKeyMapping(FSKEY_TEN2,                VK_NUMPAD2);
	// FsAddKeyMapping(FSKEY_TEN3,                VK_NUMPAD3);
	// FsAddKeyMapping(FSKEY_TEN4,                VK_NUMPAD4);
	// FsAddKeyMapping(FSKEY_TEN5,                VK_NUMPAD5);
	// FsAddKeyMapping(FSKEY_TEN6,                VK_NUMPAD6);
	// FsAddKeyMapping(FSKEY_TEN7,                VK_NUMPAD7);
	// FsAddKeyMapping(FSKEY_TEN8,                VK_NUMPAD8);
	// FsAddKeyMapping(FSKEY_TEN9,                VK_NUMPAD9);
	// FsAddKeyMapping(FSKEY_TENDOT,              VK_DECIMAL);
	// FsAddKeyMapping(FSKEY_TENSLASH,            VK_DIVIDE);
	// FsAddKeyMapping(FSKEY_TENSTAR,             VK_MULTIPLY);
	// FsAddKeyMapping(FSKEY_TENMINUS,            VK_SUBTRACT);
	// FsAddKeyMapping(FSKEY_TENPLUS,             VK_ADD);
	// FsAddKeyMapping(FSKEY_TENENTER,            0 /* Unassignable */);
}

////////////////////////////////////////////////////////////

static void Restore(int)
{
#ifdef _WIN32
#elif defined(__APPLE__)
	ioctl(fileno(stdin),TIOCSETP,&OriginalConsoleSetting);
#else
	ioctl(0,TCSETA,&OriginalConsoleSetting);
#endif
	printf("%s %d\n",__FUNCTION__,__LINE__);
	exit(0);
}

static void FsSetUpInkeyConsole(void)
{
#ifdef _WIN32
#elif defined(__APPLE__)
	signal(SIGQUIT,Restore);
	signal(SIGINT,Restore);
	// signal(SIGWINCH,);  // Window size change?

	struct sgttyb inkey;
	ioctl(fileno(stdin),TIOCGETP,&OriginalConsoleSetting);
	inkey=OriginalConsoleSetting;
	inkey.sg_flags|=CBREAK;
	inkey.sg_flags&=~ECHO;
	ioctl(fileno(stdin),TIOCSETP,&inkey);
#else
	struct termio inkey;
	ioctl(0,TCGETA,&OriginalConsoleSetting);
	inkey=OriginalConsoleSetting;
	inkey.c_lflag&=~ECHO;
	inkey.c_lflag&=~ICANON;
	inkey.c_cc[VMIN]=0;   /* Zero wait */
	inkey.c_cc[VTIME]=0;  /* Wait 0 second */
	ioctl(0,TCSETA,&inkey);
#endif
}

////////////////////////////////////////////////////////////

class FsMouseEventLog
{
public:
	int eventType;
	int lb,mb,rb;
	int mx,my;
	unsigned int shift,ctrl;
};


#define NKEYBUF 256
static int keyBuffer[NKEYBUF];
static int nKeyBufUsed=0;
static int charBuffer[NKEYBUF];
static int nCharBufUsed=0;
static int nMosBufUsed=0;
static FsMouseEventLog mosBuffer[NKEYBUF];
static int exposure=0;



void FsOpenWindow(const FsOpenWindowOption &)
{
	FsSetUpInkeyConsole();
	FsCreateKeyMapping();

	if(NULL!=fsAfterWindowCreationCallBack)
	{
		(*fsAfterWindowCreationCallBack)(fsAfterWindowCreationCallBackParam);
	}
}

void FsCloseWindow(void)
{
	Restore(0);
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

void FsGetWindowSize(int &wid,int &hei)
{
	wid=16;
	hei=16;
}

void FsGetWindowPosition(int &x0,int &y0)
{
	x0=0;
	y0=0;
}

void FsSetWindowTitle(const char windowTitle[])
{
	printf("%s\n",windowTitle);
}

void FsPollDevice(void)
{
	static bool busy=false;
	if(true!=busy)
	{
	#ifdef _WIN32
		if(kbhit()!=0)
		{
			int ky;
			ky=getch();

			FsPushChar(ky);

			if(ky==224)
			{
				if(kbhit()!=0)
				{
					ky=ky*256+getch();
				}
			}
			FsPushKey(mapVKtoFSKEY[ky]);
		}
	#elif defined(__APPLE__)
		fd_set fdSet;
		FD_ZERO(&fdSet);
		FD_SET(fileno(stdin),&fdSet);

		timeval wait;
		wait.tv_sec=0;
		wait.tv_usec=0;
		if(1<=select(fileno(stdin)+1,&fdSet,NULL,NULL,&wait))
		{
			const int nBufferSize=256;
			char buffer[nBufferSize];
			int nBytesReceived;
			nBytesReceived=read(fileno(stdin),(char *)buffer,nBufferSize);
			for(int i=0; i<nBytesReceived; ++i)
			{
				printf("%d ",buffer[i]);
			}
			printf("\n");

			for(int i=0; i<nBytesReceived; ++i)
			{
				int ky=buffer[i];
				if(ky>0)
				{
					FsPushChar(ky);

					if(ky==0x1b)
					{
						int ky2=0,ky3=0;
						ky2=(i+1<nBytesReceived ? buffer[i+1] : 0);
						ky3=(i+2<nBytesReceived ? buffer[i+2] : 0);
						if(ky2>0 && ky3>0)
						{
							if(ky2==91 && ky3==65)
							{
								FsPushKey(FSKEY_UP);
							}
							if(ky2==91 && ky3==66)
							{
								FsPushKey(FSKEY_DOWN);
							}
							if(ky2==91 && ky3==68)
							{
								FsPushKey(FSKEY_LEFT);
							}
							if(ky2==91 && ky3==67)
							{
								FsPushKey(FSKEY_RIGHT);
							}
							i+=2;
							continue;
						}
					}

					FsPushKey(mapVKtoFSKEY[ky]);
				}
			}
		}
	#else
		int ky;
		ky=getchar();
		if(ky>0)
		{
			FsPushChar(ky);

			if(ky==0x1b)
			{
				int ky2,ky3;
				if((ky2=getchar())>0 && (ky3=getchar())>0)
				{
					if(ky2==91 && ky3==65)
					{
						FsPushKey(FSKEY_UP);
					}
					if(ky2==91 && ky3==66)
					{
						FsPushKey(FSKEY_DOWN);
					}
					if(ky2==91 && ky3==68)
					{
						FsPushKey(FSKEY_LEFT);
					}
					if(ky2==91 && ky3==67)
					{
						FsPushKey(FSKEY_RIGHT);
					}
					return;
				}
			}

			FsPushKey(mapVKtoFSKEY[ky]);
		}
	#endif

		busy=true;
		if(NULL!=fsPollDeviceHook)
		{
			(*fsPollDeviceHook)(fsPollDeviceHookParam);
		}
		busy=false;
	}
}

void FsPushOnPaintEvent(void)
{
}

void FsSleep(int ms)
{
#ifdef WIN32
	Sleep(ms);
#else
	fd_set set;
	struct timeval wait;
	wait.tv_sec=ms/1000;
	wait.tv_usec=(ms%1000)*1000;
	FD_ZERO(&set);
	select(0,&set,NULL,NULL,&wait);
#endif
}

long long int FsPassedTime(void)
{
	return 10;
}

long long int FsSubSecondTimer(void)
{
	return time(NULL)*1000;
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

void FsPushKey(int fskey)
{
	if(nKeyBufUsed<NKEYBUF)
	{
		keyBuffer[nKeyBufUsed++]=fskey;
	}
}

void FsPushChar(int c)
{
	if(nCharBufUsed<NKEYBUF)
	{
		charBuffer[nCharBufUsed++]=c;
	}
}

int FsGetKeyState(int fsKeyCode)
{
	return 0;
}

int FsCheckWindowExposure(void)
{
	const int ret=exposure;
	exposure=0;
	return ret;
}

void FsGetMouseState(int &lb,int &mb,int &rb,int &mx,int &my)
{
	lb=0;
	mb=0;
	rb=0;
	mx=0;
	my=0;
}

int FsGetMouseEvent(int &lb,int &mb,int &rb,int &mx,int &my)
{
	lb=0;
	mb=0;
	rb=0;
	mx=0;
	my=0;
	return FSMOUSEEVENT_NONE;
}

void FsSwapBuffers(void)
{
}

void FsChangeToProgramDir(void)
{
}

int FsGetNumCurrentTouch(void)
{
	return 0;
}

const FsVec2i *FsGetCurrentTouch(void)
{
	return nullptr;
}


// The following two dummy functions are for loop-holing fsguilib to fssimplewindow for
// managing clipboard.  There's got to be a better way, but I cannot find so far.
void FsX11GetClipBoardString(long long int &,char *&)
{
}
void FsX11SetClipBoardString(long long int,const char [])
{
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

void FsShowMouseCursor(int showFlag)
{
}
int FsIsMouseCursorVisible(void)
{
	return 1;
}
