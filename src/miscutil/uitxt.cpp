#include <iostream>
#include <fstream>
#include "uitxt.h"

void UiText::clear(void)
{
	std::map <std::string,std::string> empty;
	std::swap(empty,textTable);
}
bool UiText::Load(std::string fileName)
{
	std::ifstream ifp(fileName);
	if(true==ifp.is_open())
	{
		// clear(); Let's not clear so that multiple uitxts can be used.

		bool firstLine=true;
		std::string line;
		while(true!=ifp.eof())
		{
			std::getline(ifp,line);
			if(""==line)
			{
				continue;
			}

			if(true==firstLine)
			{
				if("UITXT"!=line)
				{
					return false;
				}
				firstLine=false;
			}
			else
			{
				if('#'==line[0])
				{
					continue;
				}
				if('+'==line[0])
				{
					std::string tag,msg;
					int state=0;
					for(auto c : line)
					{
						switch(state)
						{
						case 0:
							// Skip '+'
							++state;
							break;
						case 1:
							if(' '==c || '\t'==c)
							{
								++state;
							}
							else
							{
								tag.push_back(c);
							}
							break;
						case 2:
							if(' '!=c && '\t'!=c)
							{
								msg.push_back(c);
								++state;
							}
							break;
						case 3:
							msg.push_back(c);
							break;
						}
					}


					for(size_t i=0; i+1<msg.size(); ++i)
					{
						if('\\'==msg[i])
						{
							if('n'==msg[i+1])
							{
								msg.erase(msg.begin()+i);
								msg[i]='\n';
							}
						}
					}


					// std::cout << '[' << tag << "]=" << msg << "\n";

					textTable[tag]=msg;
				}
			}
		}

		return true;
	}
	return false;
}
std::string UiText::operator()(std::string tag,std::string dflt) const
{
	auto found=textTable.find(tag);
	if(textTable.end()==found)
	{
		return dflt;
	}
	return found->second;
}
