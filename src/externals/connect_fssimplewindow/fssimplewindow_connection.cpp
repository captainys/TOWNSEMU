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

	int lb,mb,rb,mx,my;
	FsGetMouseEvent(lb,mb,rb,mx,my);
	for(auto &p : towns.gameport.state.ports)
	{
		if(p.device==TownsGamePort::MOUSE)
		{
			p.button[0]=(0!=lb);
			p.button[1]=(0!=rb);

			towns.ControlMouse(mx,my,towns.state.tbiosVersion);
			/* if(FsGetKeyState(FSKEY_UP))
			{
				p.mouseMotion.y()=4;
			}
			else if(FsGetKeyState(FSKEY_DOWN))
			{
				p.mouseMotion.y()=-4;
			}
			if(FsGetKeyState(FSKEY_LEFT))
			{
				p.mouseMotion.x()=4;
			}
			else if(FsGetKeyState(FSKEY_RIGHT))
			{
				p.mouseMotion.x()=-4;
			} */
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
