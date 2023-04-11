#ifndef TOWNSLINEPARSER_IS_INCLUDED
#define TOWNSLINEPARSER_IS_INCLUDED
/* { */

#include "lineparser.h"

class TownsLineParser : public LineParser
{
public:
	const class i486DXCommon *cpuPtr;
	TownsLineParser(const class i486DXCommon *);
	virtual long long int EvaluateRawNumber(const std::string &str) const;
};

class TownsLineParserHexadecimal : public LineParser
{
public:
	const class i486DXCommon *cpuPtr;
	TownsLineParserHexadecimal(const class i486DXCommon *);
	virtual long long int EvaluateRawNumber(const std::string &str) const;
};

/* } */
#endif
