#include <iostream>

#include "cpputil.h"



int main(int ac,char *av[])
{
	std::cout << "CWD=" << cpputil::Getcwd() << std::endl;
	std::cout << cpputil::TrueName("D:\\subdir\\subdir") << std::endl;
	std::cout << cpputil::TrueName("..\\subdir") << std::endl;
	std::cout << cpputil::TrueName("..\\..\\subdir") << std::endl;
	std::cout << cpputil::TrueName("subdir/xyz") << std::endl;
	return 0;
}
