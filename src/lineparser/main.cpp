#include "lineparser.h"

#include <iostream>

bool Validate(LineParser &parser,const char str[],long long int value)
{
	if(true!=parser.Analyze(str))
	{
		parser.Print();
		std::cout << "Parse Error!" << std::endl;
		return false;
	}
	if(parser.Evaluate()!=value)
	{
		parser.Print();
		std::cout << "Value Error!" << " Evaluated:" << parser.Evaluate() << " Correct:" << value << std::endl;
		return false;
	}
	std::cout << "Pass: " << str << "=" << value << std::endl;
	return true;
}

int main(void)
{
	LineParser parser;
	if(true!=Validate(parser,"255&~15",240))
	{
		return 1;
	}
	if(true!=Validate(parser,"65535&127",127))
	{
		return 1;
	}
	if(true!=Validate(parser,"1+2+3",6))
	{
		return 1;
	}
	if(true!=Validate(parser,"16*16+8*32",512))
	{
		return 1;
	}
	if(true!=Validate(parser,"123%100",23))
	{
		return 1;
	}
	if(true!=Validate(parser,"123*(-10+110)",12300))
	{
		return 1;
	}
	if(true!=Validate(parser,"(-10+110)*123",12300))
	{
		return 1;
	}
	if(true!=Validate(parser,"100*(1+3*(33))",10000))
	{
		return 1;
	}
	if(true!=Validate(parser,"(1+3*(33))*100",10000))
	{
		return 1;
	}
	if(true!=Validate(parser,"(1+3*(33))",100))
	{
		return 1;
	}
	if(true!=Validate(parser,"(((12345)))",12345))
	{
		return 1;
	}
	return 0;
}

