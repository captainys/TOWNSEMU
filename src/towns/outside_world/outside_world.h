#ifndef OUTSIDE_WORLD_IS_INCLUDED
#define OUTSIDE_WORLD_IS_INCLUDED
/* { */

#include "render.h"

class Outside_World
{
public:
	virtual void OpenWindow(void)=0;
	virtual void DevicePolling(void)=0;
	virtual void Render(const TownsRender::Image &img)=0;
};


/* } */
#endif
