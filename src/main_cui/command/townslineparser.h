#ifndef TOWNSLINEPARSER_IS_INCLUDED
#define TOWNSLINEPARSER_IS_INCLUDED
/* { */

#include "lineparser.h"

class TownsLineParser : public LineParser
{
public:
	class i486DX *cpuPtr;
	TownsLineParser(class i486DX *);
	virtual long long int EvaluateRawNumber(const std::string &str) const;
};

/* } */
#endif
