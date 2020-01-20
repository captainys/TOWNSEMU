#include <iostream>
#include <string>

#include "towns.h"
#include "cpputil.h"



bool TestDisassembly(
    unsigned addressSize,
    unsigned dataSize,
    unsigned int opCode,
    unsigned oplen,const unsigned char operand[],FMTowns &towns,const std::string &correctDisasm)
{
	i486DX::Instruction inst;

	inst.Clear();
	inst.addressSize=addressSize;
	inst.operandSize=dataSize;
	inst.opCode=opCode;
	inst.operandLen=oplen;
	for(unsigned int i=0; i<oplen; ++i)
	{
		inst.operand[i]=operand[i];
	}

	i486DX::SegmentRegister seg;
	seg.value=0;
	seg.baseLinearAddr=0;

	auto disasm=inst.Disassemble(seg,0);

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
	FMTowns towns;

	towns.Reset();


	// 32 bit addressing
	const unsigned char test32_1[]={0xC2};
	if(true!=TestDisassembly(32,32,0x8B,sizeof(test32_1),test32_1,towns,"MOV     EAX,EDX"))
	{
		return 1;
	}

	const unsigned char test32_2[]={0xD0};
	if(true!=TestDisassembly(32,32,0x8B,sizeof(test32_2),test32_2,towns,"MOV     EDX,EAX"))
	{
		return 1;
	}

	const unsigned char test32_3[]={0x84,0x96,0xFF,0x1F,0x00,0x00};
	if(true!=TestDisassembly(32,32,0x8A,sizeof(test32_3),test32_3,towns,"MOV     AL,[ESI+EDX*4+00001FFFH]"))
	{
		return 1;
	}

	const unsigned char test32_4[]={0x84,0x95,0xFF,0x2F,0x00,0x00};
	if(true!=TestDisassembly(32,32,0x8A,sizeof(test32_4),test32_4,towns,"MOV     AL,[EBP+EDX*4+00002FFFH]"))
	{
		return 1;
	}

	const unsigned char test32_5[]={0x78,0x56,0x34,0x12};
	if(true!=TestDisassembly(32,32,0xB9,sizeof(test32_5),test32_5,towns,"MOV     ECX,12345678H"))
	{
		return 1;
	}

	const unsigned char test32_6[]={0xAB,0xAB,0xAB,0xAB};
	if(true!=TestDisassembly(32,32,0xBC,sizeof(test32_6),test32_6,towns,"MOV     ESP,ABABABABH"))
	{
		return 1;
	}

	const unsigned char test32_7[]={0x0D,0x67,0x45,0x23,0x01};
	if(true!=TestDisassembly(32,32,0xFE,sizeof(test32_7),test32_7,towns,"DEC     BYTE PTR [01234567H]"))
	{
		return 1;
	}

	const unsigned char test32_8[]={0x0D,0x67,0x45,0x23,0x01};
	if(true!=TestDisassembly(32,32,0xFF,sizeof(test32_8),test32_8,towns,"DEC     DWORD PTR [01234567H]"))
	{
		return 1;
	}

	const unsigned char test32_9[]={0x8C,0x70,0x67,0x45,0x23,0x01};
	if(true!=TestDisassembly(32,32,0xFE,sizeof(test32_9),test32_9,towns,"DEC     BYTE PTR [EAX+ESI*2+01234567H]"))
	{
		return 1;
	}



	// 16 bit addressing
	const unsigned char test16_1[]={0xC2};
	if(true!=TestDisassembly(16,16,0x8B,sizeof(test16_1),test16_1,towns,"MOV     AX,DX"))
	{
		return 1;
	}

	const unsigned char test16_2[]={0xD0};
	if(true!=TestDisassembly(16,16,0x8B,sizeof(test16_2),test16_2,towns,"MOV     DX,AX"))
	{
		return 1;
	}

	const unsigned char test16_3[]={0x80,0xFF,0x1F};
	if(true!=TestDisassembly(16,16,0x8A,sizeof(test16_3),test16_3,towns,"MOV     AL,[BX+SI+1FFFH]"))
	{
		return 1;
	}

	const unsigned char test16_4[]={0x83,0xFF,0x2F};
	if(true!=TestDisassembly(16,16,0x8A,sizeof(test16_4),test16_4,towns,"MOV     AL,[BP+DI+2FFFH]"))
	{
		return 1;
	}

	const unsigned char test16_5[]={0x78,0x56};
	if(true!=TestDisassembly(16,16,0xB9,sizeof(test16_5),test16_5,towns,"MOV     CX,5678H"))
	{
		return 1;
	}

	const unsigned char test16_6[]={0x76,0x98};
	if(true!=TestDisassembly(16,16,0xBC,sizeof(test16_6),test16_6,towns,"MOV     SP,9876H"))
	{
		return 1;
	}

	return 0;
}
