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
	inst.operandSize=oplen;
	inst.opCode=opCode;
	inst.operandLen=oplen;
	for(unsigned int i=0; i<oplen; ++i)
	{
		inst.operand[i]=oplen;
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
	const unsigned char test16_1[]={0x8B,0xC2}; // [ECX+EAX*8]
	if(true!=TestDisassembly(16,16,0x8B,sizeof(test16_1),test16_1,towns,"MOV     AX,DX"))
	{
		return 1;
	}



	return 0;
}
