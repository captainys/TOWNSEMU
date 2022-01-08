#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


const uint64_t symTabPointerLocation=0x002E;
const uint64_t symTabSizeLocation=0x0032;


// Assuming little-endian.  Sorry for Motorola.
uint32_t GetDword(const unsigned char *ptr)
{
	return *((uint32_t *)ptr);
}
uint16_t GetWord(const unsigned char *ptr)
{
	return *((uint16_t *)ptr);
}



class ExpFile
{
public:
	std::vector <unsigned char> binary;
	uint32_t symTabOffset,symTabSize;
	uint32_t symbolStartOffset;
	uint32_t symbolSize;

	bool Open(std::string fName);
	bool Analyze(void);
	void Print(uint32_t n) const;
};


bool ExpFile::Open(std::string fName)
{
	std::ifstream ifp;

	ifp.open(fName,std::ios::binary);
	if(ifp.is_open())
	{
		ifp.seekg(0,std::ios::end);
		size_t len=ifp.tellg();
		printf("File Size=0x%08x\n",(uint32_t)len);

		binary.resize(len);
		ifp.seekg(0,std::ios::beg);
		ifp.read((char *)binary.data(),len);

		return true;
	}
	return false;
}

bool ExpFile::Analyze(void)
{
	if(binary.size()<2 || binary[0]!='P' || binary[1]!='3')
	{
		std::cout << "Only P3 format can have a symbol table." << std::endl;
		return false;
	}

	symTabOffset=GetDword(binary.data()+symTabPointerLocation);
	printf("Symbol Table Offset=%08xh\n",symTabOffset);

	symTabSize=GetDword(binary.data()+symTabSizeLocation);
	printf("Symbol Table Size=%08xh\n",symTabSize);

	if(symTabOffset+4+16<=binary.size())
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

			std::cout << "Subsequent 16 bytes:" << std::endl;
			for(int i=0; i<16 && symTabOffset+4+i<binary.size(); ++i)
			{
				printf("%02x ",binary[symTabOffset+4+i]);
			}
			std::cout << std::endl;
		}
		else
		{
			std::cout << "SYM not found." << std::endl;
			return false;
		}
	}

	uint32_t SYMtoProgInfo=GetWord(binary.data()+symTabOffset+4);
	uint32_t ProgInfoSize=GetDword(binary.data()+symTabOffset+6);
	symbolSize=GetDword(binary.data()+symTabOffset+10);

	printf("SYM1 to Program Info: %02xh bytes.\n",SYMtoProgInfo);
	printf("Program Info Size: %08xh bytes.\n",ProgInfoSize);
	printf("Symbols Size: %08xh bytes.\n",symbolSize);

	symbolStartOffset=symTabOffset+SYMtoProgInfo+ProgInfoSize;

	return true;
}

void ExpFile::Print(uint32_t num) const
{
	uint32_t offset=0;
	while(offset<symbolSize && 0<num)
	{
		printf("%08x ",offset);

		std::string label;
		auto n=binary[symbolStartOffset+offset];
		++offset;
		if(0==n)
		{
			continue;
		}
		while(0!=n)
		{
			label.push_back(binary[symbolStartOffset+offset]);
			++offset;
			--n;
		}

		uint32_t addr=GetDword(binary.data()+symbolStartOffset+offset);
		offset+=4;

		printf("%-32s ",label.c_str());

		printf(" %08x",addr);

		printf(" %02x %02x\n",binary[symbolStartOffset+offset],binary[symbolStartOffset+offset+1]);
		offset+=2;

		--num;
	}
}

int main(int ac,char *av[])
{
	if(ac<2)
	{
		std::cout << "Usage: expsymtable filename.exp" << std::endl;
		std::cout << "       expsymtable filenam.exp num" << std::endl;
		std::cout << "  If you specify num, it will print up to first num symbols." << std::endl;
		return 0;
	}

	int n=0x7fffffff;
	if(3<=ac)
	{
		n=atoi(av[2]);
	}

	ExpFile exp;
	if(true!=exp.Open(av[1]))
	{
		std::cout << "Failed to open " << av[1] << std::endl;
	}
	if(true!=exp.Analyze())
	{
		std::cout << "Failed to find symbol table." << std::endl;
	}

	exp.Print(n);

	return 0;
}
