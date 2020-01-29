#ifndef TOWNSCOMMANDUTIL_IS_INCLUDED
#define TOWNSCOMMANDUTIL_IS_INCLUDED
/* { */

#include <string>

#include "i486.h"

namespace cmdutil
{
	/*! Make a far pointer from a string.
	    If no segment is given, SEG member of i486DX::FarPointer will be set to
	    i486DX::FarPointer::NO_SEG.
	*/
	i486DX::FarPointer MakeFarPointer(const std::string &str);

	/*! Print prompt.
	*/
	void PrintPrompt(void);
};

/* } */
#endif
