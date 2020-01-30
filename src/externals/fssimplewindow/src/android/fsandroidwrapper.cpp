#include "../fssimplewindow.h"
#include "fssimplewindowconnector.h"
#include <string>
#include <chrono>

#include <unistd.h>


static std::string filesDir,cacheDir;

extern "C" const char *FsSimpleWindowConnector_GetFilesDir(void)
{
	return filesDir.c_str();
}

extern "C" const char *FsSimpleWindowConnector_GetCacheDir(void)
{
	return cacheDir.c_str();
}

/*! Called from ysgles2view.cpp */
extern "C" void FsSimpleWindowConnector_NotifyFilesDirAndCacheDir(const char filesDirIn[],const char cacheDirIn[])
{
	filesDir=filesDirIn;
	cacheDir=cacheDirIn;
}




// Connector functions >>

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
static int nTouch=0;
static FsVec2i touchCache[NEVTBUF];

static int exposure=0;

static int winWid=0,winHei=0;


void FsSimpleWindowConnector_BeforeBigBang(void)
{
}

void FsSimpleWindowConnector_Initialize(void)
{
}

void FsSimpleWindowConnector_NotifyWindowSize(int wid,int hei)
{
	winWid=wid;
	winHei=hei;
}

void FsSimpleWindowConnector_NotifyMouseEvent(int evt,int lb,int mb,int rb,int mx,int my)
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

void FsSimpleWindowConnector_NotifyCharIn(int unicode)
{
	if(nCharBufUsed<NKEYBUF)
	{
		charBuffer[nCharBufUsed]=unicode;
		++nCharBufUsed;
	}
}

void FsSimpleWindowConnector_NotifyTouchState(int n,const int xy[])
{
	if(NEVTBUF<n)
	{
		n=NEVTBUF;
	}
	nTouch=n;
	for(int i=0; i<n; ++i)
	{
		touchCache[i].v[0]=xy[i*2];
		touchCache[i].v[1]=xy[i*2+1];
	}
}

// Connector functions <<



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

void FsGetWindowSize(int &wid,int &hei)
{
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

static bool firstTime=true;
static std::chrono::time_point<std::chrono::system_clock> timeOrigin;
static std::chrono::time_point<std::chrono::system_clock> prevTime;

long long int FsSubSecondTimer(void)
{
	auto now=std::chrono::system_clock::now();
	if(true==firstTime)
	{
		firstTime=false;
		timeOrigin=now;
		prevTime=now;
	}
	return std::chrono::duration_cast <std::chrono::milliseconds> (now-timeOrigin).count();
}

long long int FsPassedTime(void)
{
	auto now=std::chrono::system_clock::now();
	if(true==firstTime)
	{
		firstTime=false;
		timeOrigin=now;
		prevTime=now;
	}
	long long int passed=std::chrono::duration_cast <std::chrono::milliseconds> (now-prevTime).count();
	prevTime=now;
	if(0==passed)
	{
		passed=1;
	}
	return passed;
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
	chdir(filesDir.c_str());
}

int FsGetNumCurrentTouch(void)
{
	return nTouch;
}

const FsVec2i *FsGetCurrentTouch(void)
{
	return touchCache;
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
