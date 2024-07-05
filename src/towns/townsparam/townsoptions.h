#ifndef TOWNSOPTION_H_IS_INCLUDED
#define TOWNSOPTION_H_IS_INCLUDED

#include <string>
#include <vector>
#include "townsdef.h"

class TownsOptions
{
public:
	bool showTips=true;
	std::string errorMessage;

	std::vector <std::string> Serialize(void) const;
	bool Deserialize(const std::vector <std::string> &data);
};


#endif
