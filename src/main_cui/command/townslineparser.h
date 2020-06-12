#ifndef TOWNSLINEPARSER_IS_INCLUDED
#define TOWNSLINEPARSER_IS_INCLUDED
/* { */

#include "lineparser.h"

class TownsLineParser : public LineParser
{
public:
	const class i486DX *cpuPtr;
	TownsLineParser(const class i486DX *);
	virtual long long int EvaluateRawNumber(const std::string &str) const;
};

class TownsLineParserHexadecimal : public LineParser
{
public:
	const class i486DX *cpuPtr;
	TownsLineParserHexadecimal(const class i486DX *);
	virtual long long int EvaluateRawNumber(const std::string &str) const;
};

/* } */
#endif
