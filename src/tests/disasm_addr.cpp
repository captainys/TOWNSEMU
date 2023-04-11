/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <string>

#include "towns.h"
#include "cpputil.h"
#include "i486symtable.h"


bool TestAddressingDisassembly(
    unsigned addressSize,
    unsigned dataSize,
    unsigned oplen,const unsigned char operand[],FMTownsCommon &towns,const std::string &correctDisasm)
{
	i486DX::Operand oper;
	i486SymbolTable symTable;
	auto numBytes=oper.Decode(addressSize,dataSize,operand);
	auto disasm=oper.Disassemble(0,0,symTable);

	if(numBytes!=oplen)
	{
		std::cout << "Inconsistent operand length!" << std::endl;
		std::cout << "Decoded:" << numBytes << std::endl;
		std::cout << "Correct:" << oplen << std::endl;
		goto ERREND;
	}

	// auto disasm=towns.cpu.DisassembleAddressing(addressSize,dataSize,operand);
	std::cout << "Disassembled as: " << disasm << std::endl;
	if(disasm!=correctDisasm)
	{
		std::cout << "Wrong disassembly!" << std::endl;
		goto ERREND;
	}
	return true;

ERREND:
	std::cout << "Correct Disassembly: " << correctDisasm << std::endl;
	std::cout << "Operands: ";
	for(unsigned i=0; i<oplen; ++i)
	{
		std::cout << cpputil::Ubtox(operand[i]) << " ";
	}
	std::cout << std::endl;
	return false;
}

int main(int ac,char *av[])
{
	static FMTownsCommon towns;

	towns.Reset();


	// 32 bit addressing
	const unsigned char test1[]={0x04,0xC1}; // [ECX+EAX*8]
	if(true!=TestAddressingDisassembly(32,32,sizeof(test1),test1,towns,"[ECX+EAX*8]"))
	{
		return 1;
	}

	const unsigned char test2[]={0x04,0x41}; // [ECX+EAX*2]
	if(true!=TestAddressingDisassembly(32,32,sizeof(test2),test2,towns,"[ECX+EAX*2]"))
	{
		return 1;
	}

	const unsigned char test3[]={0x8E,0x4D,0x38,0x00,0x00};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test3),test3,towns,"[ESI+0000384DH]"))
	{
		return 1;
	}

	const unsigned char test4[]={0x15,0xEC,0x19,0x02,0x00};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test4),test4,towns,"[000219ECH]"))
	{
		return 1;
	}

	const unsigned char test5[]={0x85,0x34,0x10,0x00,0x00};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test5),test5,towns,"[EBP+00001034H]"))
	{
		return 1;
	}

	const unsigned char test6[]={0x3C,0x5C};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test6),test6,towns,"[ESP+EBX*2]"))
	{
		return 1;
	}

	const unsigned char test7[]={0x04,0x7D,0xFF,0x3A,0x00,0x00};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test7),test7,towns,"[EDI*2+00003AFFH]"))
	{
		return 1;
	}

	const unsigned char test8[]={0xB4,0x4D,0x00,0x00,0x32,0x51};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test8),test8,towns,"[EBP+ECX*2+51320000H]"))
	{
		return 1;
	}

	const unsigned char test32_eax[]={0xC0};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test32_eax),test32_eax,towns,"EAX"))
	{
		return 1;
	}

	const unsigned char test32_ecx[]={0xC9};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test32_ecx),test32_ecx,towns,"ECX"))
	{
		return 1;
	}

	const unsigned char test32_esi[]={0xF6};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test32_esi),test32_esi,towns,"ESI"))
	{
		return 1;
	}

	const unsigned char test32_edi[]={0xFF};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test32_edi),test32_edi,towns,"EDI"))
	{
		return 1;
	}



	// 16 bit addressing
	const unsigned char test16_1[]={0xA0,0xFF,0x06};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_1),test16_1,towns,"[BX+SI+06FFH]"))
	{
		return 1;
	}

	const unsigned char test16_2[]={0x00};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_2),test16_2,towns,"[BX+SI]"))
	{
		return 1;
	}

	const unsigned char test16_3[]={0x02};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_3),test16_3,towns,"[BP+SI]"))
	{
		return 1;
	}

	const unsigned char test16_4[]={0x47,0x06};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_4),test16_4,towns,"[BX+06H]"))
	{
		return 1;
	}

	const unsigned char test16_5[]={0x94,0x97,0x26};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_5),test16_5,towns,"[SI+2697H]"))
	{
		return 1;
	}

	const unsigned char test16_ax[]={0xC0};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_ax),test16_ax,towns,"AX"))
	{
		return 1;
	}

	const unsigned char test16_cx[]={0xC9};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_cx),test16_cx,towns,"CX"))
	{
		return 1;
	}

	const unsigned char test16_si[]={0xF6};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_si),test16_si,towns,"SI"))
	{
		return 1;
	}

	const unsigned char test16_di[]={0xFF};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_di),test16_di,towns,"DI"))
	{
		return 1;
	}

	const unsigned char test16_7[]={0x82,0x2a,0x65};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_7),test16_7,towns,"[BP+SI+652AH]"))
	{
		return 1;
	}

	const unsigned char test16_8[]={0x8a,0x2a,0xe8};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_8),test16_8,towns,"[BP+SI-17D6H]"))
	{
		return 1;
	}


	return 0;
}
