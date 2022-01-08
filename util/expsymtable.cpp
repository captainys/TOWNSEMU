#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>


const uint64_t symTabPointer=0x002E;


// Assuming little-endian.  Sorry for Motorola.
uint32_t GetDword(unsigned char *ptr)
{
	return *((uint32_t *)ptr);
}
uint16_t GetWord(unsigned char *ptr)
{
	return *((uint16_t *)ptr);
}



class ExpFile
{
public:
	std::vector <unsigned char> binary;
	uint32_t symTabOffset;

	bool Open(std::string fName);
};


bool ExpFile::Open(std::string fName)
{
	std::ifstream ifp;

	ifp.open(fName,std::ios::binary);
	ifp.seekg(0,std::ios::end);
	size_t len=ifp.tellg();
	printf("File Size=0x%08x\n",(uint32_t)len);

	binary.resize(len);
	ifp.seekg(0,std::ios::beg);
	ifp.read((char *)binary.data(),len);

	if(ifp.is_open())
	{
		symTabOffset=GetDword(binary.data()+symTabPointer);
		printf("Symbol Table Offset=%08x\n",symTabOffset);

		if(symTabOffset+4<=binary.size())
		{
			if('S'==binary[symTabOffset] &&
			   'Y'==binary[symTabOffset+1] &&
			   'M'==binary[symTabOffset+2])
			{
				std::cout << "Confirmed 'SYM' Keyword." << std::endl;
				std::cout << (char)binary[symTabOffset];
				std::cout << (char)binary[symTabOffset+1];
				std::cout << (char)binary[symTabOffset+2];
				std::cout << (char)binary[symTabOffset+3];
				std::cout << std::endl;
			}
			else
			{
				std::cout << "SYM not found." << std::endl;
				return false;
			}
		}
		return true;
	}
	return false;
}



int main(int ac,char *av[])
{
	if(ac<2)
	{
		std::cout << "Usage: expsymtable filename.exp" << std::endl;
		return 0;
	}
	ExpFile exp;
	if(true!=exp.Open(av[1]))
	{
		std::cout << "Failed to open " << av[1] << std::endl;
	}

	return 0;
}
