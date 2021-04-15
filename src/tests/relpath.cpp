#include <iostream>

#include "cpputil.h"



void TestRelativePath(std::string fName,std::string relativeToThisDir)
{
	auto relPath=cpputil::MakeRelativePath(fName,relativeToThisDir);
	auto fulPath=cpputil::MakeFullPathName(relativeToThisDir,relPath);
	cpputil::SimplifyPath(fulPath);

	std::cout << "Input=" << fName << std::endl;
	std::cout << "RelPath=" << relPath << std::endl;
	std::cout << "Recover=" << fulPath << std::endl;

	if(fulPath!=fName)
	{
		printf("Cannot recover the original file name\n");
		exit(1);
	}
}

int main(void)
{
	TestRelativePath("C:/users/soji/disks/test.bin","C:/users/soji");

	TestRelativePath("C:/users/soji/disks/test.bin","C:/users/someone");

	TestRelativePath("C:/users/soji/disks/test.bin","C:/users/someone/disks");

	TestRelativePath("/users/soji/disks/test.bin","/users/someone/disks");

	return 0;
}
