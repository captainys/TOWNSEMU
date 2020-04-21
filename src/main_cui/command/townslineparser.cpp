#include "i486.h"
#include "townslineparser.h"
#include "cpputil.h"

TownsLineParser::TownsLineParser(class i486DX *cpuPtr)
{
	this->cpuPtr=cpuPtr;
}
/* virtual */ long long int TownsLineParser::EvaluateRawNumber(const std::string &str) const
{
	auto reg=cpuPtr->StrToReg(str);
	if(i486DX::REG_NULL!=reg)
	{
		return cpuPtr->GetRegisterValue(reg);
	}
	else
	{
		return cpputil::Atoi(str.c_str());
	}
}

