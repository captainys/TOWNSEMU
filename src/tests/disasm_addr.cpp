#include <iostream>
#include <string>

#include "towns.h"
#include "cpputil.h"


bool TestAddressingDisassembly(
    unsigned addressSize,
    unsigned dataSize,
    unsigned oplen,const unsigned char operand[],FMTowns &towns,const std::string &correctDisasm)
{
	auto disasm=towns.cpu.DisassembleAddressing(addressSize,dataSize,operand);
	std::cout << "Disassembled as: " << disasm << std::endl;
	if(disasm!=correctDisasm)
	{
		std::cout << "Wrong disassembly!" << std::endl;
		std::cout << "Correct: " << correctDisasm << std::endl;
		std::cout << "Operands: ";
		for(unsigned i=0; i<oplen; ++i)
		{
			std::cout << cpputil::Ubtox(operand[i]) << " ";
		}
		std::cout << std::endl;
		return false;
	}
	return true;
}

int main(int ac,char *av[])
{
	FMTowns towns;

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
	if(true!=TestAddressingDisassembly(32,32,sizeof(test3),test3,towns,"[ESI+00000384DH]"))
	{
		return 1;
	}

	const unsigned char test4[]={0x15,0xEC,0x19,0x02,0x00};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test4),test4,towns,"[0000219ECH]"))
	{
		return 1;
	}

	const unsigned char test5[]={0x85,0x34,0x10,0x00,0x00,0x10};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test5),test5,towns,"[EBP+000001034H]"))
	{
		return 1;
	}

	const unsigned char test6[]={0x3C,0x5C};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test6),test6,towns,"[ESP+EBX*2]"))
	{
		return 1;
	}

	const unsigned char test7[]={0x04,0x7D,0xFF,0x3A,0x00,0x00};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test7),test7,towns,"[EDI*2+000003AFFH]"))
	{
		return 1;
	}

	const unsigned char test8[]={0xB4,0x4D,0x00,0x00,0x32,0x51};
	if(true!=TestAddressingDisassembly(32,32,sizeof(test8),test8,towns,"[EBP+ECX*2+051320000H]"))
	{
		return 1;
	}



	// 16 bit addressing
	const unsigned char test16_1[]={0xA0,0xFF,0x06};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_1),test16_1,towns,"[BX+SI+006FFH]"))
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
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_4),test16_4,towns,"[BX+006H]"))
	{
		return 1;
	}

	const unsigned char test16_5[]={0x94,0x97,0x26};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_5),test16_5,towns,"[SI+02697H]"))
	{
		return 1;
	}

	const unsigned char test16_6[]={0xC0};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_6),test16_6,towns,"AX"))
	{
		return 1;
	}

	const unsigned char test16_7[]={0x82,0x2a,0x65};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_7),test16_7,towns,"[BP+SI+0652AH]"))
	{
		return 1;
	}

	const unsigned char test16_8[]={0x8a,0x2a,0xe8};
	if(true!=TestAddressingDisassembly(16,16,sizeof(test16_8),test16_8,towns,"[BP+SI-017D6H]"))
	{
		return 1;
	}


	return 0;
}
