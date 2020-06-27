#include <iostream>
#include <string>

int main(void)
{
	for(;;)
	{
		std::string str;
		std::getline(std::cin,str);
		std::cout << str << std::endl;
		if("qqq"==str)
		{
			break;
		}
	}
	return 0;
}
