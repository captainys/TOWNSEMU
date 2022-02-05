#include <iostream>
#include "filesys.h"



int main(int ac,char *av[])
{
	FileSys fsys;
	auto context=fsys.CreateFindContext();

	fsys.hostPath=av[1];

	unsigned int checked=0;
	bool first=true;
	for(;;)
	{
		FileSys::DirectoryEntry ent;
		if(true==first)
		{
			ent=fsys.FindFirst(av[2],context);
			first=false;
		}
		else
		{
			ent=fsys.FindNext(context);
		}

		if(true==ent.endOfDir)
		{
			break;
		}
		std::cout << ent.fName << std::endl;
		if(ent.fName=="CMakeLists.txt")
		{
			checked|=1;
		}
		if(ent.fName=="main.cpp")
		{
			checked|=2;
		}
		if(ent.fName=="filesys_null.cpp")
		{
			checked|=4;
		}
		if(ent.fName=="filesys_win.cpp" || ent.fName=="filesys_unix.cpp" || ent.fName=="filesys_mac.cpp")
		{
			checked|=8;
		}
		if(ent.fName=="filesys.h")
		{
			checked|=16;
		}
	}

	fsys.DeleteFindContext(context);

	if(31==checked)
	{
		std::cout << "Detected all files." << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Missed some files." << std::endl;
		return 1;
	}
}
