#include "i486.h"
#include "townslineparser.h"
#include "cpputil.h"

TownsLineParser::TownsLineParser(const class i486DX *cpuPtr)
{
	this->cpuPtr=cpuPtr;
}
/* virtual */ long long int TownsLineParser::EvaluateRawNumber(const std::string &str) const
{
	if('\''==str[0] && 0!=str[1] && '\''==str[2])
	{
		return int(str[1]);
	}

	std::string cap=str;
	cpputil::Capitalize(cap);
	auto reg=cpuPtr->StrToReg(cap);
	if(i486DX::REG_NULL!=reg)
	{
		return cpuPtr->GetRegisterValue(reg);
	}
	else
	{
		return cpputil::Atoi(str.c_str());
	}
}

TownsLineParserHexadecimal::TownsLineParserHexadecimal(const class i486DX *cpuPtr)
{
	this->cpuPtr=cpuPtr;
}
/* virtual */ long long int TownsLineParserHexadecimal::EvaluateRawNumber(const std::string &str) const
{
	if('\''==str[0] && 0!=str[1] && '\''==str[2])
	{
		return int(str[1]);
	}

	std::string cap=str;
	cpputil::Capitalize(cap);
	auto reg=cpuPtr->StrToReg(cap);
	if(i486DX::REG_NULL!=reg)
	{
		return cpuPtr->GetRegisterValue(reg);
	}
	else
	{
		return cpputil::Xtoi(str.c_str());
	}
}
