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



/* virtual */ void FsSimpleWindowConnection::OpenWindow(void)
{
	FsOpenWindow(0,0,640,480,1);
	FsSetWindowTitle("FM Towns Emulator - TSUGARU");
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
					unsigned char byteData[2];
					if(0<TownsKeyboard::TranslateChar(byteData,c))
					{
						towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_THUMBSHIFT_PRESS_OR_RELEASE,byteData[1]);
						towns.keyboard.PushFifo(byteData[0],byteData[1]);
					}
				}
			}
		}
		while(0!=(c=FsInkey()))
		{
			unsigned char byteData[2];
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
