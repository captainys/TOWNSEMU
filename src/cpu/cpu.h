#ifndef CPU_IS_INCLUDED
#define CPU_IS_INCLUDED
/* { */

#include "device.h"

class CPU : public Device
{
public:
	static const unsigned int twoToN[32];
};

/* } */
#endif
