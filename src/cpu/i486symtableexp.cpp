/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <fstream>
#include <iostream>

#include "i486symtable.h"
#include "cpputil.h"



bool i486SymbolTable::ImportEXPSymbolTable(std::string fName)
{
	const uint64_t symTabPointerLocation=0x002E;
	const uint64_t symTabSizeLocation=0x0032;


	std::vector <unsigned char> binary;
	uint32_t symTabOffset,symTabSize;
	uint32_t symbolStartOffset;
	uint32_t symbolSize;



	// Read Binary
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
	}
	else
	{
		return false;
	}
	ifp.close();



	// Analyze
	if(binary.size()<2 || (binary[0]=='P' && binary[1]=='3'))
	{
		symTabOffset=cpputil::GetDword(binary.data()+symTabPointerLocation);
		symTabSize=cpputil::GetDword(binary.data()+symTabSizeLocation);
	}
	else if(binary.size()<2 || (binary[0]=='M' && binary[1]=='P'))
	{
		// Based on Oh! FM TOWNS February 1992 issue, pp.76
		uint32_t expSizeLow=cpputil::GetWord(binary.data()+2);
		uint32_t expSizeHigh=cpputil::GetWord(binary.data()+4);
		symTabOffset=expSizeLow+0x200*expSizeHigh;
		symTabSize=binary.size()-symTabOffset;
	}
	else
	{
		std::cout << "Unknown EXP format." << std::endl;
		return false;
	}

	printf("Symbol Table Offset=%08xh\n",symTabOffset);
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

	uint32_t SYMtoProgInfo=cpputil::GetWord(binary.data()+symTabOffset+4);
	uint32_t ProgInfoSize=cpputil::GetDword(binary.data()+symTabOffset+6);
	symbolSize=cpputil::GetDword(binary.data()+symTabOffset+10);

	printf("SYM1 to Program Info: %02xh bytes.\n",SYMtoProgInfo);
	printf("Program Info Size: %08xh bytes.\n",ProgInfoSize);
	printf("Symbols Size: %08xh bytes.\n",symbolSize);

	symbolStartOffset=symTabOffset+SYMtoProgInfo+ProgInfoSize;



	uint32_t offset=0;
	while(offset<symbolSize)
	{
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

		uint32_t addr=cpputil::GetDword(binary.data()+symbolStartOffset+offset);
		offset+=4;

		printf("%-32s ",label.c_str());

		printf(" %08x",addr);

		printf(" %02x %02x\n",binary[symbolStartOffset+offset],binary[symbolStartOffset+offset+1]);
		offset+=2;

		// Assume Code Segment is 000Ch
		i486DX::FarPointer ptr;
		ptr.SEG=0x000C;
		ptr.OFFSET=addr;
		SetImportedLabel(ptr,label);
	}

	return true;
}
