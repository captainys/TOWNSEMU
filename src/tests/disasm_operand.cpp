#include <iostream>
#include <string>

#include "towns.h"
#include "cpputil.h"
#include "i486symtable.h"



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

	i486SymbolTable symTable;
	auto disasm=inst.Disassemble(seg,0,symTable);

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
	if(true!=TestDisassembly(32,32,0xB8,sizeof(test32_5),test32_5,towns,"MOV     EAX,12345678H"))
	{
		return 1;
	}
	if(true!=TestDisassembly(32,32,0xB9,sizeof(test32_5),test32_5,towns,"MOV     ECX,12345678H"))
	{
		return 1;
	}
	if(true!=TestDisassembly(32,32,0xBA,sizeof(test32_5),test32_5,towns,"MOV     EDX,12345678H"))
	{
		return 1;
	}
	if(true!=TestDisassembly(32,32,0xBB,sizeof(test32_5),test32_5,towns,"MOV     EBX,12345678H"))
	{
		return 1;
	}

	const unsigned char test32_6[]={0xAB,0xAB,0xAB,0xAB};
	if(true!=TestDisassembly(32,32,0xBC,sizeof(test32_6),test32_6,towns,"MOV     ESP,ABABABABH"))
	{
		return 1;
	}
	if(true!=TestDisassembly(32,32,0xBD,sizeof(test32_6),test32_6,towns,"MOV     EBP,ABABABABH"))
	{
		return 1;
	}
	if(true!=TestDisassembly(32,32,0xBE,sizeof(test32_6),test32_6,towns,"MOV     ESI,ABABABABH"))
	{
		return 1;
	}
	if(true!=TestDisassembly(32,32,0xBF,sizeof(test32_6),test32_6,towns,"MOV     EDI,ABABABABH"))
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

	const unsigned char test32_10[]={0x68};
	if(true!=TestDisassembly(32,32,0x34,sizeof(test32_10),test32_10,towns,"XOR     AL,68H"))
	{
		return 1;
	}
	const unsigned char test32_11[]={0x67,0x45,0x23,0x01};
	if(true!=TestDisassembly(32,32,0x35,sizeof(test32_11),test32_11,towns,"XOR     EAX,01234567H"))
	{
		return 1;
	}
	const unsigned char test32_12[]={0xF3,0x77};
	if(true!=TestDisassembly(32,32,0x80,sizeof(test32_12),test32_12,towns,"XOR     BL,77H"))
	{
		return 1;
	}
	const unsigned char test32_12and[]={0xE3,0x77};
	if(true!=TestDisassembly(32,32,0x80,sizeof(test32_12and),test32_12and,towns,"AND     BL,77H"))
	{
		return 1;
	}
	const unsigned char test32_13[]={0xB4,0x97,0x67,0x45,0x23,0x01,0x86};
	if(true!=TestDisassembly(32,32,0x80,sizeof(test32_13),test32_13,towns,"XOR     BYTE PTR [EDI+EDX*4+01234567H],86H"))
	{
		return 1;
	}
	const unsigned char test32_15[]={0xF3,0x67,0x45,0x23,0x01};
	if(true!=TestDisassembly(32,32,0x81,sizeof(test32_15),test32_15,towns,"XOR     EBX,01234567H"))
	{
		return 1;
	}
	const unsigned char test32_15and[]={0xE3,0x67,0x45,0x23,0x01};
	if(true!=TestDisassembly(32,32,0x81,sizeof(test32_15and),test32_15and,towns,"AND     EBX,01234567H"))
	{
		return 1;
	}
	const unsigned char test32_16[]={0xB4,0x46,0x67,0x45,0x23,0x01,0xEF,0xCD,0xAB,0x89};
	if(true!=TestDisassembly(32,32,0x81,sizeof(test32_16),test32_16,towns,"XOR     DWORD PTR [ESI+EAX*2+01234567H],89ABCDEFH"))
	{
		return 1;
	}
	const unsigned char test32_17[]={0xF1,0x80};
	if(true!=TestDisassembly(32,32,0x83,sizeof(test32_17),test32_17,towns,"XOR     ECX,FFFFFF80H"))
	{
		return 1;
	}
	const unsigned char test32_18[]={0xB5,0x77,0x77,0x77,0x77,0x80};
	if(true!=TestDisassembly(32,32,0x83,sizeof(test32_18),test32_18,towns,"XOR     DWORD PTR [EBP+77777777H],FFFFFF80H"))
	{
		return 1;
	}
	const unsigned char test32_19[]={0xF1,0x11};
	if(true!=TestDisassembly(32,32,0x83,sizeof(test32_19),test32_19,towns,"XOR     ECX,00000011H"))
	{
		return 1;
	}
	const unsigned char test32_20[]={0xB4,0x24,0x77,0x77,0x77,0x77,0x11};
	if(true!=TestDisassembly(32,32,0x83,sizeof(test32_20),test32_20,towns,"XOR     DWORD PTR [ESP+77777777H],00000011H"))
	{
		return 1;
	}
	const unsigned char test32_21[]={0xDF};
	if(true!=TestDisassembly(32,32,0x32,sizeof(test32_21),test32_21,towns,"XOR     BL,BH"))
	{
		return 1;
	}
	const unsigned char test32_22[]={0xE9};
	if(true!=TestDisassembly(32,32,0x32,sizeof(test32_22),test32_22,towns,"XOR     CH,CL"))
	{
		return 1;
	}
	const unsigned char test32_23[]={0xD9};
	if(true!=TestDisassembly(32,32,0x33,sizeof(test32_23),test32_23,towns,"XOR     EBX,ECX"))
	{
		return 1;
	}
	const unsigned char test32_24[]={0xCB};
	if(true!=TestDisassembly(32,32,0x33,sizeof(test32_24),test32_24,towns,"XOR     ECX,EBX"))
	{
		return 1;
	}
	const unsigned char test32_25[]={0x9F,0x11,0x11,0x11,0x11};
	if(true!=TestDisassembly(32,32,0x32,sizeof(test32_25),test32_25,towns,"XOR     BL,[EDI+11111111H]"))
	{
		return 1;
	}
	const unsigned char test32_26[]={0x96,0x11,0x11,0x11,0x11};
	if(true!=TestDisassembly(32,32,0x33,sizeof(test32_26),test32_26,towns,"XOR     EDX,[ESI+11111111H]"))
	{
		return 1;
	}

/*
	const unsigned char test32_14[]={};
	if(true!=TestDisassembly(32,32,,sizeof(test32_14),test32_14,towns,""))
	{
		return 1;
	}

*/


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

	const unsigned char test16_7[]={0x77};
	if(true!=TestDisassembly(16,16,0xB0,sizeof(test16_7),test16_7,towns,"MOV     AL,77H"))
	{
		return 1;
	}
	if(true!=TestDisassembly(16,16,0xB1,sizeof(test16_7),test16_7,towns,"MOV     CL,77H"))
	{
		return 1;
	}
	if(true!=TestDisassembly(16,16,0xB2,sizeof(test16_7),test16_7,towns,"MOV     DL,77H"))
	{
		return 1;
	}
	if(true!=TestDisassembly(16,16,0xB3,sizeof(test16_7),test16_7,towns,"MOV     BL,77H"))
	{
		return 1;
	}
	if(true!=TestDisassembly(16,16,0xB4,sizeof(test16_7),test16_7,towns,"MOV     AH,77H"))
	{
		return 1;
	}
	if(true!=TestDisassembly(16,16,0xB5,sizeof(test16_7),test16_7,towns,"MOV     CH,77H"))
	{
		return 1;
	}
	if(true!=TestDisassembly(16,16,0xB6,sizeof(test16_7),test16_7,towns,"MOV     DH,77H"))
	{
		return 1;
	}
	if(true!=TestDisassembly(16,16,0xB7,sizeof(test16_7),test16_7,towns,"MOV     BH,77H"))
	{
		return 1;
	}

/*
	const unsigned char test32_14[]={};
	if(true!=TestDisassembly(32,32,,sizeof(test32_14),test32_14,towns,""))
	{
		return 1;
	}

*/



	return 0;
}
