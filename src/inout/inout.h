#ifndef INOUT_IS_INCLUDED
#define INOUT_IS_INCLUDED
/* { */

#include "device.h"

class InOut : public Device
{
public:
	virtual const char *DeviceName(void) const{return "IO";}
};

/* } */
#endif
