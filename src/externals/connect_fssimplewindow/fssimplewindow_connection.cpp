/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <stdio.h>

#include "fssimplewindow.h"
#include "fssimplewindow_connection.h"

// G** D*** Windows headers! >>
#ifdef REG_NONE
#undef REG_NONE
#endif
#ifdef OUT
#undef OUT
#endif
// *od *amn Windows headers! <<

#include "towns.h"



/* virtual */ void FsSimpleWindowConnection::Start(void)
{
	FsOpenWindow(0,0,640,480,1);
	FsSetWindowTitle("FM Towns Emulator - TSUGARU");
	soundPlayer.Start();
	cddaStartHSG=0;

}
/* virtual */ void FsSimpleWindowConnection::Stop(void)
{
	soundPlayer.End();
}
/* virtual */ void FsSimpleWindowConnection::DevicePolling(class FMTowns &towns)
{
	FsPollDevice();

	// For the time translation mode only.
	// if(true==keyTranslationMode)
	{
		unsigned int c;
		while(0!=(c=FsInkeyChar()))
		{
			if(0==FsGetKeyState(FSKEY_CTRL))
			{
				if(' '<=c)
				{
					unsigned char byteData[2]={0,0};
					if(0<TownsKeyboard::TranslateChar(byteData,c))
					{
						towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_PRESS_OR_RELEASE,byteData[1]);
						towns.keyboard.PushFifo(byteData[0],byteData[1]);
					}
				}
			}
		}
		while(0!=(c=FsInkey()))
		{
			unsigned char byteData[2]={0,0};
			switch(c)
			{
			default:
				byteData[1]=0;
				break;
			case FSKEY_ENTER:
				byteData[1]=TOWNS_JISKEY_RETURN;
				break;
			case FSKEY_BS:
				byteData[1]=TOWNS_JISKEY_BACKSPACE;
				break;
			case FSKEY_TAB:
				byteData[1]=TOWNS_JISKEY_TAB;
				break;
			case FSKEY_C:
				if(0!=FsGetKeyState(FSKEY_CTRL))
				{
					byteData[0]=TOWNS_KEYFLAG_CTRL;
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_C);
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_C);
				}
				break;
			case FSKEY_S:
				if(0!=FsGetKeyState(FSKEY_CTRL))
				{
					byteData[0]=TOWNS_KEYFLAG_CTRL;
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_S);
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_S);
				}
				break;
			case FSKEY_Q:
				if(0!=FsGetKeyState(FSKEY_CTRL))
				{
					byteData[0]=TOWNS_KEYFLAG_CTRL;
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_Q);
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_Q);
				}
				break;
			case FSKEY_ESC:
				byteData[0]|=(0!=FsGetKeyState(FSKEY_CTRL) ? TOWNS_KEYFLAG_CTRL : 0);
				byteData[0]|=(0!=FsGetKeyState(FSKEY_SHIFT) ? TOWNS_KEYFLAG_SHIFT : 0);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_ESC);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_ESC);
				break;
			case FSKEY_UP:
				byteData[0]=(0!=FsGetKeyState(FSKEY_CTRL) ? TOWNS_KEYFLAG_CTRL : 0);
				byteData[0]|=(0!=FsGetKeyState(FSKEY_SHIFT) ? TOWNS_KEYFLAG_SHIFT : 0);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_UP);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_UP);
				break;
			case FSKEY_DOWN:
				byteData[0]=(0!=FsGetKeyState(FSKEY_CTRL) ? TOWNS_KEYFLAG_CTRL : 0);
				byteData[0]|=(0!=FsGetKeyState(FSKEY_SHIFT) ? TOWNS_KEYFLAG_SHIFT : 0);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_DOWN);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_DOWN);
				break;
			case FSKEY_LEFT:
				byteData[0]=(0!=FsGetKeyState(FSKEY_CTRL) ? TOWNS_KEYFLAG_CTRL : 0);
				byteData[0]|=(0!=FsGetKeyState(FSKEY_SHIFT) ? TOWNS_KEYFLAG_SHIFT : 0);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_LEFT);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_LEFT);
				break;
			case FSKEY_RIGHT:
				byteData[0]=(0!=FsGetKeyState(FSKEY_CTRL) ? TOWNS_KEYFLAG_CTRL : 0);
				byteData[0]|=(0!=FsGetKeyState(FSKEY_SHIFT) ? TOWNS_KEYFLAG_SHIFT : 0);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_RIGHT);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_RIGHT);
				break;
			}
			if(0!=byteData[1])
			{
				byteData[0]=TOWNS_KEYFLAG_JIS_PRESS;
				towns.keyboard.PushFifo(byteData[0],byteData[1]);
				byteData[0]=TOWNS_KEYFLAG_JIS_RELEASE;
				towns.keyboard.PushFifo(byteData[0],byteData[1]);
			}
		}
	}

	if(towns.eventLog.mode!=TownsEventLog::MODE_PLAYBACK)
	{
		int lb,mb,rb,mx,my;
		FsGetMouseEvent(lb,mb,rb,mx,my);
		towns.SetMouseButtonState((0!=lb),(0!=rb));
		towns.ControlMouse(mx,my,towns.state.tbiosVersion);
	}
}
/* virtual */ void FsSimpleWindowConnection::Render(const TownsRender::Image &img)
{
	std::vector <unsigned char> flip;
	flip.resize(img.wid*img.hei*4);

	auto upsideDown=img.rgba;
	auto rightSideUp=flip.data()+(img.hei-1)*img.wid*4;
	for(unsigned int y=0; y<img.hei; ++y)
	{
		memcpy(rightSideUp,upsideDown,img.wid*4);
		upsideDown+=img.wid*4;
		rightSideUp-=img.wid*4;
	}

	glClear(GL_COLOR_BUFFER_BIT);
	int wid,hei;
	FsGetWindowSize(wid,hei);
	glViewport(0,0,wid,hei);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f,float(wid),float(hei),0.0f,-1,1);
	glRasterPos2i(0,hei-1);
	glDrawPixels(img.wid,img.hei,GL_RGBA,GL_UNSIGNED_BYTE,flip.data());
	FsSwapBuffers();
}
/* virtual */ void FsSimpleWindowConnection::CDDAPlay(const DiscImage &discImg,DiscImage::MinSecFrm from,DiscImage::MinSecFrm to)
{
	auto wave=discImg.GetWave(from,to);
	cddaChannel.CreateFromSigned16bitStereo(44100,wave);
	soundPlayer.PlayOneShot(cddaChannel);
	cddaStartHSG=from.ToHSG();
}
/* virtual */ void FsSimpleWindowConnection::CDDAStop(void)
{
	soundPlayer.Stop(cddaChannel);
}
/* virtual */ void FsSimpleWindowConnection::CDDAPause(void)
{
	soundPlayer.Pause(cddaChannel);
}
/* virtual */ void FsSimpleWindowConnection::CDDAResume(void)
{
	soundPlayer.Resume(cddaChannel);
}
/* virtual */ bool FsSimpleWindowConnection::CDDAIsPlaying(void)
{
	return (YSTRUE==soundPlayer.IsPlaying(cddaChannel));
}
/* virtual */ DiscImage::MinSecFrm FsSimpleWindowConnection::CDDACurrentPosition(void)
{
	double sec=soundPlayer.GetCurrentPosition(cddaChannel);
	unsigned long long secHSG=(unsigned long long)(sec*75.0);
	unsigned long long posInDisc=secHSG+cddaStartHSG;

	DiscImage::MinSecFrm msf;
	msf.FromHSG(posInDisc);
	return msf;
}

/* virtual */ void FsSimpleWindowConnection::PCMPlay(const RF5C68 &pcm,unsigned int ch)
{
printf("%s %d %d\n",__FUNCTION__,__LINE__,ch);
	auto wave=pcm.Make19KHzWave(ch);
printf("%s %d\n",__FUNCTION__,__LINE__);
	PCMChannel[ch].CreateFromSigned16bitStereo(44100,wave);
	soundPlayer.PlayOneShot(PCMChannel[ch]);
}
/* virtual */ void FsSimpleWindowConnection::PCMPlayStop(int ch)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	soundPlayer.Stop(PCMChannel[ch]);
}
/* virtual */ double FsSimpleWindowConnection::PCMCurrentPosition(int ch)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	return soundPlayer.GetCurrentPosition(PCMChannel[ch]);
}
