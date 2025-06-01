/*
Based on the discussion with a student of CMU Computer Club, it may be a good idea to make
opCodeRenumberTable and opCodeNeedOperandTable shorter because the tables will be more likely
to be kept in the CPU cache.  Internal representations of the two-byte opcodes need to be converted
from 0xF?? to 0x1??.  This program does exactly it.
*/


#include <stdio.h>
#include <string.h>
#include <fstream>
#include <string>
#include <iostream>

int main(void)
{
	std::ifstream ifp("opcode.txt");
	std::ofstream ofp("1ff.txt");
	std::string str;
	if(true==ifp.is_open())
	{
		while(true!=ifp.eof())
		{
			std::getline(ifp,str);

			for(int i=0; i+1<str.size(); ++i)
			{
				if('0'==str[i] && 'x'==str[i+1])
				{
					unsigned int code=0;
					size_t firstNonZero=0;
					for(int j=i+2; j<str.size(); ++j)
					{
						if('0'!=str[j] && 0==firstNonZero)
						{
							firstNonZero=j;
						}

						if('0'<=str[j] && str[j]<='9')
						{
							code<<=4;
							code+=(str[j]-'0');
						}
						else if('A'<=str[j] && str[j]<='F')
						{
							code<<=4;
							code+=(str[j]+10-'A');
						}
						else if('a'<=str[j] && str[j]<='f')
						{
							code<<=4;
							code+=(str[j]+10-'a');
						}
						else
						{
							break;
						}
					}

					if(0x100<=code)
					{
						if(str[firstNonZero]=='F' || str[firstNonZero]=='f')
						{
							str[firstNonZero]='1';
							std::cout << "Convert: " << str << "\n";
						}
						else
						{
							std::cout << "Error!  Not 0xF?? >> " << str << "\n";
							std::cout << str[firstNonZero] << "\n";
						}
					}
				}
			}

			ofp << str << "\n";
		}
	}
	return 0;
}
