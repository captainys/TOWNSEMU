#include "fssimplewindow_connection.h"
#include "fssimplewindow.h"
#include <stdio.h>


/* virtual */ void FsSimpleWindowConnection::OpenWindow(void)
{
	FsOpenWindow(0,0,640,480,1);
}
/* virtual */ void FsSimpleWindowConnection::DevicePolling(void)
{
	FsPollDevice();
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
