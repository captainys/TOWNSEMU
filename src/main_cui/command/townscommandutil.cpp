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

			auto segPart=str;
			segPart.resize(i);
			cpputil::Capitalize(segPart);
			if("CS"==segPart)
			{
				ptr.SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_CS;
			}
			else if("SS"==segPart)
			{
				ptr.SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_SS;
			}
			else if("DS"==segPart)
			{
				ptr.SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_DS;
			}
			else if("ES"==segPart)
			{
				ptr.SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_ES;
			}
			else if("FS"==segPart)
			{
				ptr.SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_FS;
			}
			else if("GS"==segPart)
			{
				ptr.SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_GS;
			}
			else if("PHYS"==segPart || "P"==segPart)
			{
				ptr.SEG=i486DX::FarPointer::PHYS_ADDR;
			}
			else if("LINE"==segPart || "L"==segPart)
			{
				ptr.SEG=i486DX::FarPointer::LINEAR_ADDR;
			}
			else
			{
				ptr.SEG=cpputil::Xtoi(str.data());
			}
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
