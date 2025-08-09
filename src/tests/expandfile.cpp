#include <stdlib.h>
#include <iostream>
#include <string>
#include <map>
#include "cpputil.h"

void Verify(std::string exp,std::string shouldBe)
{
	if(exp!=shouldBe)
	{
		std::cout << "Error\n";
		std::cout << "Expanded: " << exp << "\n";
		std::cout << "ShouldBe: " << shouldBe << "\n";
		exit(1);
	}
}

int main(void)
{
	std::map <std::string,std::string> dict;
	dict["progdir"]="c:/users/soji/tsugaru";

	std::string exp;
	exp=cpputil::ExpandFileName("${progdir}/roms",dict);
	std::cout << exp << "\n";
	Verify(exp,"c:/users/soji/tsugaru/roms");


	exp=cpputil::ExpandFileName("${nothing}/roms",dict);
	std::cout << exp << "\n";
	Verify(exp,"${nothing}/roms");


	exp=cpputil::ExpandFileName("$dollar/roms",dict);
	std::cout << exp << "\n";
	Verify(exp,"$dollar/roms");


	exp=cpputil::ExpandFileName("$$dollar/roms",dict);
	std::cout << exp << "\n";
	Verify(exp,"$$dollar/roms");


	exp=cpputil::ExpandFileName("${open/roms",dict);
	std::cout << exp << "\n";
	Verify(exp,"${open/roms");


	return 0;
}
