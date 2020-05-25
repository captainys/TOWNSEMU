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



FsSimpleWindowConnection::FsSimpleWindowConnection()
{
	FSKEYtoTownsKEY=new unsigned int [FSKEY_NUM_KEYCODE];

	FSKEYtoTownsKEY[FSKEY_NULL]=        TOWNS_JISKEY_NULL;
	FSKEYtoTownsKEY[FSKEY_SPACE]=       TOWNS_JISKEY_SPACE;
	FSKEYtoTownsKEY[FSKEY_0]=           TOWNS_JISKEY_0;
	FSKEYtoTownsKEY[FSKEY_1]=           TOWNS_JISKEY_1;
	FSKEYtoTownsKEY[FSKEY_2]=           TOWNS_JISKEY_2;
	FSKEYtoTownsKEY[FSKEY_3]=           TOWNS_JISKEY_3;
	FSKEYtoTownsKEY[FSKEY_4]=           TOWNS_JISKEY_4;
	FSKEYtoTownsKEY[FSKEY_5]=           TOWNS_JISKEY_5;
	FSKEYtoTownsKEY[FSKEY_6]=           TOWNS_JISKEY_6;
	FSKEYtoTownsKEY[FSKEY_7]=           TOWNS_JISKEY_7;
	FSKEYtoTownsKEY[FSKEY_8]=           TOWNS_JISKEY_8;
	FSKEYtoTownsKEY[FSKEY_9]=           TOWNS_JISKEY_9;
	FSKEYtoTownsKEY[FSKEY_A]=           TOWNS_JISKEY_A;
	FSKEYtoTownsKEY[FSKEY_B]=           TOWNS_JISKEY_B;
	FSKEYtoTownsKEY[FSKEY_C]=           TOWNS_JISKEY_C;
	FSKEYtoTownsKEY[FSKEY_D]=           TOWNS_JISKEY_D;
	FSKEYtoTownsKEY[FSKEY_E]=           TOWNS_JISKEY_E;
	FSKEYtoTownsKEY[FSKEY_F]=           TOWNS_JISKEY_F;
	FSKEYtoTownsKEY[FSKEY_G]=           TOWNS_JISKEY_G;
	FSKEYtoTownsKEY[FSKEY_H]=           TOWNS_JISKEY_H;
	FSKEYtoTownsKEY[FSKEY_I]=           TOWNS_JISKEY_I;
	FSKEYtoTownsKEY[FSKEY_J]=           TOWNS_JISKEY_J;
	FSKEYtoTownsKEY[FSKEY_K]=           TOWNS_JISKEY_K;
	FSKEYtoTownsKEY[FSKEY_L]=           TOWNS_JISKEY_L;
	FSKEYtoTownsKEY[FSKEY_M]=           TOWNS_JISKEY_M;
	FSKEYtoTownsKEY[FSKEY_N]=           TOWNS_JISKEY_N;
	FSKEYtoTownsKEY[FSKEY_O]=           TOWNS_JISKEY_O;
	FSKEYtoTownsKEY[FSKEY_P]=           TOWNS_JISKEY_P;
	FSKEYtoTownsKEY[FSKEY_Q]=           TOWNS_JISKEY_Q;
	FSKEYtoTownsKEY[FSKEY_R]=           TOWNS_JISKEY_R;
	FSKEYtoTownsKEY[FSKEY_S]=           TOWNS_JISKEY_S;
	FSKEYtoTownsKEY[FSKEY_T]=           TOWNS_JISKEY_T;
	FSKEYtoTownsKEY[FSKEY_U]=           TOWNS_JISKEY_U;
	FSKEYtoTownsKEY[FSKEY_V]=           TOWNS_JISKEY_V;
	FSKEYtoTownsKEY[FSKEY_W]=           TOWNS_JISKEY_W;
	FSKEYtoTownsKEY[FSKEY_X]=           TOWNS_JISKEY_X;
	FSKEYtoTownsKEY[FSKEY_Y]=           TOWNS_JISKEY_Y;
	FSKEYtoTownsKEY[FSKEY_Z]=           TOWNS_JISKEY_Z;
	FSKEYtoTownsKEY[FSKEY_ESC]=         TOWNS_JISKEY_ESC;
	FSKEYtoTownsKEY[FSKEY_F1]=          TOWNS_JISKEY_PF01;
	FSKEYtoTownsKEY[FSKEY_F2]=          TOWNS_JISKEY_PF02;
	FSKEYtoTownsKEY[FSKEY_F3]=          TOWNS_JISKEY_PF03;
	FSKEYtoTownsKEY[FSKEY_F4]=          TOWNS_JISKEY_PF04;
	FSKEYtoTownsKEY[FSKEY_F5]=          TOWNS_JISKEY_PF05;
	FSKEYtoTownsKEY[FSKEY_F6]=          TOWNS_JISKEY_PF06;
	FSKEYtoTownsKEY[FSKEY_F7]=          TOWNS_JISKEY_PF07;
	FSKEYtoTownsKEY[FSKEY_F8]=          TOWNS_JISKEY_PF08;
	FSKEYtoTownsKEY[FSKEY_F9]=          TOWNS_JISKEY_PF09;
	FSKEYtoTownsKEY[FSKEY_F10]=         TOWNS_JISKEY_PF10;
	FSKEYtoTownsKEY[FSKEY_F11]=         TOWNS_JISKEY_PF11;
	FSKEYtoTownsKEY[FSKEY_F12]=         TOWNS_JISKEY_PF12;
	FSKEYtoTownsKEY[FSKEY_PRINTSCRN]=   TOWNS_JISKEY_NULL;
	FSKEYtoTownsKEY[FSKEY_CAPSLOCK]=    TOWNS_JISKEY_CAPS;
	FSKEYtoTownsKEY[FSKEY_SCROLLLOCK]=  TOWNS_JISKEY_NULL; // Can assign something later.
	FSKEYtoTownsKEY[FSKEY_PAUSEBREAK]=  TOWNS_JISKEY_BREAK;
	FSKEYtoTownsKEY[FSKEY_BS]=          TOWNS_JISKEY_BACKSPACE;
	FSKEYtoTownsKEY[FSKEY_TAB]=         TOWNS_JISKEY_TAB;
	FSKEYtoTownsKEY[FSKEY_ENTER]=       TOWNS_JISKEY_RETURN;
	FSKEYtoTownsKEY[FSKEY_SHIFT]=       TOWNS_JISKEY_SHIFT;
	FSKEYtoTownsKEY[FSKEY_CTRL]=        TOWNS_JISKEY_CTRL;
	FSKEYtoTownsKEY[FSKEY_ALT]=         TOWNS_JISKEY_NULL; // Can assign something later.
	FSKEYtoTownsKEY[FSKEY_INS]=         TOWNS_JISKEY_INSERT;
	FSKEYtoTownsKEY[FSKEY_DEL]=         TOWNS_JISKEY_DELETE;
	FSKEYtoTownsKEY[FSKEY_HOME]=        TOWNS_JISKEY_HOME;
	FSKEYtoTownsKEY[FSKEY_END]=         TOWNS_JISKEY_NULL; // Should be translated as SHIFT+DEL
	FSKEYtoTownsKEY[FSKEY_PAGEUP]=      TOWNS_JISKEY_PREV;
	FSKEYtoTownsKEY[FSKEY_PAGEDOWN]=    TOWNS_JISKEY_NEXT;
	FSKEYtoTownsKEY[FSKEY_UP]=          TOWNS_JISKEY_UP;
	FSKEYtoTownsKEY[FSKEY_DOWN]=        TOWNS_JISKEY_DOWN;
	FSKEYtoTownsKEY[FSKEY_LEFT]=        TOWNS_JISKEY_LEFT;
	FSKEYtoTownsKEY[FSKEY_RIGHT]=       TOWNS_JISKEY_RIGHT;
	FSKEYtoTownsKEY[FSKEY_NUMLOCK]=     TOWNS_JISKEY_NULL; // Can assign something later.
	FSKEYtoTownsKEY[FSKEY_TILDA]=       TOWNS_JISKEY_NULL;
	FSKEYtoTownsKEY[FSKEY_MINUS]=       TOWNS_JISKEY_MINUS;
	FSKEYtoTownsKEY[FSKEY_PLUS]=        TOWNS_JISKEY_NUM_PLUS;
	FSKEYtoTownsKEY[FSKEY_LBRACKET]=    TOWNS_JISKEY_LEFT_SQ_BRACKET;
	FSKEYtoTownsKEY[FSKEY_RBRACKET]=    TOWNS_JISKEY_RIGHT_SQ_BRACKET;
	FSKEYtoTownsKEY[FSKEY_BACKSLASH]=   TOWNS_JISKEY_BACKSLASH;
	FSKEYtoTownsKEY[FSKEY_SEMICOLON]=   TOWNS_JISKEY_SEMICOLON;
	FSKEYtoTownsKEY[FSKEY_SINGLEQUOTE]= TOWNS_JISKEY_AT;  // Should be reviewed.
	FSKEYtoTownsKEY[FSKEY_COMMA]=       TOWNS_JISKEY_COMMA;
	FSKEYtoTownsKEY[FSKEY_DOT]=         TOWNS_JISKEY_DOT;
	FSKEYtoTownsKEY[FSKEY_SLASH]=       TOWNS_JISKEY_SLASH;
	FSKEYtoTownsKEY[FSKEY_TEN0]=        TOWNS_JISKEY_NUM_0;
	FSKEYtoTownsKEY[FSKEY_TEN1]=        TOWNS_JISKEY_NUM_1;
	FSKEYtoTownsKEY[FSKEY_TEN2]=        TOWNS_JISKEY_NUM_2;
	FSKEYtoTownsKEY[FSKEY_TEN3]=        TOWNS_JISKEY_NUM_3;
	FSKEYtoTownsKEY[FSKEY_TEN4]=        TOWNS_JISKEY_NUM_4;
	FSKEYtoTownsKEY[FSKEY_TEN5]=        TOWNS_JISKEY_NUM_5;
	FSKEYtoTownsKEY[FSKEY_TEN6]=        TOWNS_JISKEY_NUM_6;
	FSKEYtoTownsKEY[FSKEY_TEN7]=        TOWNS_JISKEY_NUM_7;
	FSKEYtoTownsKEY[FSKEY_TEN8]=        TOWNS_JISKEY_NUM_8;
	FSKEYtoTownsKEY[FSKEY_TEN9]=        TOWNS_JISKEY_NUM_9;
	FSKEYtoTownsKEY[FSKEY_TENDOT]=      TOWNS_JISKEY_NUM_DOT;
	FSKEYtoTownsKEY[FSKEY_TENSLASH]=    TOWNS_JISKEY_NUM_SLASH;
	FSKEYtoTownsKEY[FSKEY_TENSTAR]=     TOWNS_JISKEY_NUM_STAR;
	FSKEYtoTownsKEY[FSKEY_TENMINUS]=    TOWNS_JISKEY_NUM_MINUS;
	FSKEYtoTownsKEY[FSKEY_TENPLUS]=     TOWNS_JISKEY_NUM_PLUS;
	FSKEYtoTownsKEY[FSKEY_TENENTER]=    TOWNS_JISKEY_NUM_RETURN;
	FSKEYtoTownsKEY[FSKEY_WHEELUP]=     TOWNS_JISKEY_UP;
	FSKEYtoTownsKEY[FSKEY_WHEELDOWN]=   TOWNS_JISKEY_DOWN;
	FSKEYtoTownsKEY[FSKEY_CONTEXT]=     TOWNS_JISKEY_NULL; // Can assign something later.

	// Japanese keyboard
	FSKEYtoTownsKEY[FSKEY_CONVERT]=     TOWNS_JISKEY_CONVERT;
	FSKEYtoTownsKEY[FSKEY_NONCONVERT]=  TOWNS_JISKEY_NO_CONVERT;
	FSKEYtoTownsKEY[FSKEY_KANA]=        TOWNS_JISKEY_KATAKANA;

	// The following key codes won't be returned by FsInkey()
	// These may return non zero for FsGetKeyState
	FSKEYtoTownsKEY[FSKEY_LEFT_CTRL]=   TOWNS_JISKEY_CTRL;
	FSKEYtoTownsKEY[FSKEY_RIGHT_CTRL]=  TOWNS_JISKEY_CTRL;
	FSKEYtoTownsKEY[FSKEY_LEFT_SHIFT]=  TOWNS_JISKEY_SHIFT;
	FSKEYtoTownsKEY[FSKEY_RIGHT_SHIFT]= TOWNS_JISKEY_SHIFT;
	FSKEYtoTownsKEY[FSKEY_LEFT_ALT]=    TOWNS_JISKEY_NULL;
	FSKEYtoTownsKEY[FSKEY_RIGHT_ALT]=   TOWNS_JISKEY_NULL;
}
FsSimpleWindowConnection::~FsSimpleWindowConnection()
{
	delete [] FSKEYtoTownsKEY;
}


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
			// CTRL+C, CTRL+S, CTRL+Q
			case FSKEY_C:
			case FSKEY_S:
			case FSKEY_Q:
				if(0!=FsGetKeyState(FSKEY_CTRL))
				{
					byteData[0]=TOWNS_KEYFLAG_CTRL;
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  FSKEYtoTownsKEY[c]);
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,FSKEYtoTownsKEY[c]);
				}
				break;
			case FSKEY_ENTER:
			case FSKEY_BS:
			case FSKEY_TAB:
			case FSKEY_ESC:
			case FSKEY_UP:
			case FSKEY_DOWN:
			case FSKEY_LEFT:
			case FSKEY_RIGHT:
			case FSKEY_HOME:
			case FSKEY_PAGEUP:
			case FSKEY_PAGEDOWN:
			case FSKEY_INS:
			case FSKEY_DEL:
			case FSKEY_F1:
			case FSKEY_F2:
			case FSKEY_F3:
			case FSKEY_F4:
			case FSKEY_F5:
			case FSKEY_F6:
			case FSKEY_F7:
			case FSKEY_F8:
			case FSKEY_F9:
			case FSKEY_F10:
			case FSKEY_F11:
			case FSKEY_F12:
				byteData[0]|=(0!=FsGetKeyState(FSKEY_CTRL) ? TOWNS_KEYFLAG_CTRL : 0);
				byteData[0]|=(0!=FsGetKeyState(FSKEY_SHIFT) ? TOWNS_KEYFLAG_SHIFT : 0);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  FSKEYtoTownsKEY[c]);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,FSKEYtoTownsKEY[c]);
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
		for(unsigned int portId=0; portId<TOWNS_NUM_GAMEPORTS; ++portId)
		{
			switch(gamePort[portId])
			{
			default:
				// Not implemented yet.
				break;
			case TOWNS_GAMEPORTEMU_KEYBOARD:
				{
					bool Abutton=(0!=FsGetKeyState(FSKEY_Z));
					bool Bbutton=(0!=FsGetKeyState(FSKEY_X));
					bool run=(0!=FsGetKeyState(FSKEY_A));
					bool pause=(0!=FsGetKeyState(FSKEY_S));
					bool left=(0!=FsGetKeyState(FSKEY_LEFT));
					bool right=(0!=FsGetKeyState(FSKEY_RIGHT));
					if(true==left && true==right)
					{
						right=false;
					}
					bool up=(0!=FsGetKeyState(FSKEY_UP));
					bool down=(0!=FsGetKeyState(FSKEY_DOWN));
					if(true==up && true==down)
					{
						down=false;
					}
					towns.SetGamePadState(portId,Abutton,Bbutton,left,right,up,down,run,pause);
				}
				break;
			}
		}

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

/* virtual */ void FsSimpleWindowConnection::PCMPlay(RF5C68 &pcm,unsigned int ch)
{
	auto wave=pcm.Make19KHzWave(ch);
	PCMChannel[ch].CreateFromSigned16bitStereo(RF5C68::FREQ,wave);
	soundPlayer.PlayOneShot(PCMChannel[ch]);
}
/* virtual */ void FsSimpleWindowConnection::PCMPlayStop(int ch)
{
	soundPlayer.Stop(PCMChannel[ch]);
}
/* virtual */ bool FsSimpleWindowConnection::PCMChannelPlaying(int ch)
{
	return YSTRUE==soundPlayer.IsPlaying(PCMChannel[ch]);
}
/* virtual */ double FsSimpleWindowConnection::PCMCurrentPosition(int ch)
{
	return soundPlayer.GetCurrentPosition(PCMChannel[ch]);
}

/* virtual */ void FsSimpleWindowConnection::FMPlay(unsigned int ch,std::vector <unsigned char> &wave)
{
	FMChannel[ch].CreateFromSigned16bitStereo(YM2612::WAVE_SAMPLING_RATE,wave);

	// std::string fName;
	// fName="tone";
	// fName+=cpputil::Itoa(ch);
	// fName+=".wav";
	// auto waveFile=FMChannel[ch].MakeWavByteData();
	// cpputil::WriteBinaryFile(fName,waveFile.size(),waveFile.data());

	soundPlayer.PlayOneShot(FMChannel[ch]);
}
/* virtual */ void FsSimpleWindowConnection::FMPlayStop(int ch)
{
}
/* virtual */ bool FsSimpleWindowConnection::FMChannelPlaying(int ch)
{
	return YSTRUE==soundPlayer.IsPlaying(FMChannel[ch]);
}

