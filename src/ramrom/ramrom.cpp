#include "ramrom.h"
#include "cpputil.h"



bool Memory::LoadROMImages(const char dirName[])
{
	std::string fName;
	fName=cpputil::MakeFullPathName(dirName,"FMT_SYS.ROM");
	sysRom=cpputil::ReadBinaryFile(fName);
	if(0==sysRom.size())
	{
		Abort("Cannot read FMT_SYS.ROM");
		return false;
	}

	return true;
}
