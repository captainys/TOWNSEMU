#ifndef MISCUTIL_IS_INCLUDED
#define MISCUTIL_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "i486.h"
#include "ramrom.h"

namespace miscutil
{
	std::vector <std::string> MakeMemDump(const i486DX &cpu,const Memory &mem,i486DX::FarPointer ptr,unsigned int length,bool shiftJIS);
};

/* } */
#endif
