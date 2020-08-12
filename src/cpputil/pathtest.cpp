#include <iostream>
#include "cpputil.h"



bool TestSimplify(const std::string src,const std::string expected)
{
	std::string path=src;
	cpputil::SimplifyPath(path);
	std::cout << src << " to " << path << std::endl;
	return (path==expected);
}


int main(void)
{
	if(true!=TestSimplify("src/towns/townsdef/townsdef.h","src/towns/townsdef/townsdef.h") || 
	   true!=TestSimplify("src/towns/townsdef/../towns.h","src/towns/towns.h") ||
	   true!=TestSimplify("src/towns/townsdef/../..","src") ||
	   true!=TestSimplify("/src/towns/townsdef/../..","/src") ||
	   true!=TestSimplify("/src/towns/townsdef/../../../..","") ||
	   true!=TestSimplify("/src/towns/townsdef/../../../../..","..")
	)
	{
		std::cout << "Error!" << std::endl;
		return 1;
	}
	return 0;
}
