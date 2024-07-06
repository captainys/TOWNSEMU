#include "townsoptions.h"
#include "cpputil.h"

std::vector <std::string> TownsOptions::Serialize(void) const
{
	std::vector <std::string> data;

	{
		data.push_back("SHOWTIPS ");
		data.back()+=std::string(cpputil::BoolToStr(showTips));
	}

	return data;
}
bool TownsOptions::Deserialize(const std::vector <std::string> &data)
{
	bool res=true;
	errorMessage="";

	for(auto str : data)
	{
		auto argv=cpputil::Parser(str.c_str());
		if(0<argv.size())
		{
			if("SHOWTIPS"==argv[0])
			{
				if(2<=argv.size())
				{
					showTips=cpputil::StrToBool(argv[1]);
				}
			}
			else
			{
				errorMessage="Unknown keyword "+argv[0];
				res=false;
			}
		}
	}

	return res;
}
