#undef YS_LITTLE_ENDIAN

#include <iostream>
#include "cpputil.h"



int main(void)
{
	auto u16=cpputil::MakeUnsignedWord(0x11,0x22);
	if(u16!=0x2211)
	{
		std::cout << "U16" << std::endl;
		return 1;
	}

	auto u32=cpputil::MakeUnsignedDword(0x11,0x22,0x33,0x44);
	if(u32!=0x44332211)
	{
		std::cout << "U32" << std::endl;
		return 1;
	}

	std::cout << "Pass" << std::endl;

	return 0;
}
