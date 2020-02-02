#include <iostream>

#include "townsargv.h"
#include "cpputil.h"



TownsARGV::TownsARGV()
{
}
void TownsARGV::PrintHelp(void) const
{
	std::cout << "Usage:" << std::endl;
	std::cout << "-HELP,-H,-?" << std::endl;
	std::cout << "  Print Help." << std::endl;
	std::cout << "-FD0 filename" << std::endl;
	std::cout << "  Floppy disk image file name for Drive A." << std::endl;
	std::cout << "-FD1 filename" << std::endl;
	std::cout << "  Floppy disk image file name for Drive B." << std::endl;
}

bool TownsARGV::AnalyzeCommandParameter(int argc,char *argv[])
{
	for(int i=1; i<argc; ++i)
	{
		std::string ARG=cpputil::Capitalize(argv[i]);
		if("-HELP"==ARG || "-H"==ARG || "-?"==ARG)
		{
			PrintHelp();
		}
		else if("-FD0"==ARG && i+1<argc)
		{
			fdImgFName[0]=argv[i+1];
			++i;
		}
		else if("-FD1"==ARG && i+1<argc)
		{
			fdImgFName[1]=argv[i+1];
			++i;
		}
		else
		{
			if(1==i)
			{
				ROMPath=argv[i];
			}
			else
			{
				std::cout << "Undefined Option:" << argv[i] << std::endl;
			}
		}
	}
	return true;
}
