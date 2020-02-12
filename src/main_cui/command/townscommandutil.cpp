#include <iostream>

#include "townscommandutil.h"
#include "cpputil.h"



i486DX::FarPointer cmdutil::MakeFarPointer(const std::string &str)
{
	i486DX::FarPointer ptr;
	ptr.MakeFromString(str);
	return ptr;
}

void cmdutil::PrintPrompt(void)
{
	std::cout << ">";
}
