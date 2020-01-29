#include <iostream>

#include "townscommandutil.h"
#include "cpputil.h"



i486DX::FarPointer cmdutil::MakeFarPointer(const std::string &str)
{
	i486DX::FarPointer ptr;
	ptr.Nullify();
	for(unsigned int i=0; i<str.size(); ++i)
	{
		if(':'==str[i])
		{
			ptr.OFFSET=cpputil::Xtoi(str.data()+i+1);
			ptr.SEG=cpputil::Xtoi(str.data());
			return ptr;
		}
	}

	ptr.SEG=i486DX::FarPointer::NO_SEG;
	ptr.OFFSET=cpputil::Xtoi(str.data());
	return ptr;
}

void cmdutil::PrintPrompt(void)
{
	std::cout << ">";
}
