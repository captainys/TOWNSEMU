#include <string>
#include <vector>
#include <iostream>
#include <fstream>



int main(int ac,char *av[])
{
	std::ifstream ifp(av[1]);
	std::ofstream ofp("townsmap.cpp");

	std::vector <std::string> text;

	text.clear();
	while(true!=ifp.eof())
	{
		std::string str;
		std::getline(ifp,str);

		while(0<str.size() && (str[0]==' ' || str[0]=='\t'))
		{
			str.erase(str.begin());
		}

		auto first=str;
		first.resize(8);
		if("TOWNSIO_"==first)
		{
			for(int i=0; i<str.size(); ++i)
			{
				if(str[i]=='=')
				{
					str.resize(i);
					break;
				}
			}

			auto macro=str;
			auto second=str;
			second.erase(0,8);

			std::string line;
			line.push_back('\t');
			line+="ioMap[";
			line+=macro;
			line+="]=";
			line.push_back('\"');
			line+=second;
			line.push_back('\"');
			line.push_back(';');

			text.push_back(line);
		}
	}

	ofp << "#include <map>" << std::endl;
	ofp << "#include <string>" << std::endl;
	ofp << "#include \"townsdef.h\"\n" << std::endl;
	ofp << "std::map <unsigned int,std::string> FMTownsIOMap(void)" << std::endl;
	ofp << "{" << std::endl;
	ofp << "\tstd::map <unsigned int,std::string> ioMap;" << std::endl;
	for(auto &line : text)
	{
		ofp << line << std::endl;
	}
	ofp << "\treturn ioMap;" << std::endl;
	ofp << "}" << std::endl;

	return 0;
}



