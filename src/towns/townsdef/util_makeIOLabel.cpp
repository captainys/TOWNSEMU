/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <string>
#include <vector>
#include <iostream>
#include <fstream>



int main(void)
{
	std::ifstream ifp("townsdef.h");
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



