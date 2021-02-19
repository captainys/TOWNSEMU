#include <iostream>
#include <string>
#include "cpputil.h"

bool Test(std::string ptn,std::string str,bool shouldBe)
{
	bool result=cpputil::WildCardCompare(ptn,str);
	std::cout << ptn << " " << str << " " << result << std::endl;
	if(result!=shouldBe)
	{
		std::cout << "Error!" << std::endl;
	}
	return result==shouldBe;
}

int main(void)
{
	// Should match
	if(true!=Test("*","abc",true) ||
	   true!=Test("a*","abc",true) ||
	   true!=Test("?bc","abc",true) ||
	   true!=Test("??makawa","Yamakawa",true) ||
	   true!=Test("Ya??kawa","Yamakawa",true) ||
	   true!=Test("Ya*w?","Yamakawa",true) ||
	   true!=Test("Ya*k*a","Yamakawa",true) ||
	   true!=Test("*Yam?k?wa","Yamakawa",true) ||

	// Should not match
	   true!=Test("?b","abc",false) ||
	   true!=Test("Ya?kawa","Yamakawa",false))
	{
		return 1;
	}
	return 0;
}
