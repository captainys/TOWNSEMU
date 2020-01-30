#include "fssimplewindow_connection.h"
#include "fssimplewindow.h"


/* virtual */ void FsSimpleWindowConnection::OpenWindow(void)
{
	FsOpenWindow(0,0,1024,768,1);
}
/* virtual */ void FsSimpleWindowConnection::DevicePolling(void)
{
	FsPollDevice();
}
/* virtual */ void FsSimpleWindowConnection::Render(const TownsRender::Image &img)
{
	glClear(GL_COLOR_BUFFER_BIT);
	int wid,hei;
	FsGetWindowSize(wid,hei);
	glRasterPos2i(0,hei-1);
	glDrawPixels(img.wid,img.hei,GL_RGBA,GL_UNSIGNED_BYTE,img.rgba);
	FsSwapBuffers();
}
