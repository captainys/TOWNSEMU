#ifndef FSSIMPLEWINDOW_CONNECTION_IS_INCLUDED
#define FSSIMPLEWINDOW_CONNECTION_IS_INCLUDED
/* { */

#include "outside_world.h"

class FsSimpleWindowConnection : public Outside_World
{
public:
	virtual void OpenWindow(void);
	virtual void DevicePolling(class FMTowns &towns);
	virtual void Render(const TownsRender::Image &img);
};

/* } */
#endif
