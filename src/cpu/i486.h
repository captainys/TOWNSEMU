#ifndef I486_IS_INCLUDED
#define I486_IS_INCLUDED
/* { */

#include "cpu.h"
#include "ramrom.h"
#include "inout.h"

class i486DX : public CPU
{
public:
	unsigned long long RunOneInstruction(void);
};

/* } */
#endif
