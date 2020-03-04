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

	// 0000001C  8A 9C 8D 40 23 01 00          						MOV     BL,[EBP+ECX*4+00012340H] 
	const unsigned char test32_17a[]={0x9C,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x8A,sizeof(test32_17a),test32_17a,towns,"MOV     BL,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 00000023  8A 8C 8D 40 23 01 00          						MOV     CL,[EBP+ECX*4+00012340H] 
	const unsigned char test32_17b[]={0x8C,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x8A,sizeof(test32_17b),test32_17b,towns,"MOV     CL,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 0000002A  8A 94 8D 40 23 01 00          						MOV     DL,[EBP+ECX*4+00012340H] 
	const unsigned char test32_17c[]={0x94,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x8A,sizeof(test32_17c),test32_17c,towns,"MOV     DL,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 00000031  8A A4 8D 40 23 01 00          						MOV     AH,[EBP+ECX*4+00012340H] 
	const unsigned char test32_17d[]={0xA4,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x8A,sizeof(test32_17d),test32_17d,towns,"MOV     AH,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 00000038  8A BC 8D 40 23 01 00          						MOV     BH,[EBP+ECX*4+00012340H] 
	const unsigned char test32_17e[]={0xBC,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x8A,sizeof(test32_17e),test32_17e,towns,"MOV     BH,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 0000003F  8A AC 8D 40 23 01 00          						MOV     CH,[EBP+ECX*4+00012340H] 
	const unsigned char test32_17f[]={0xAC,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x8A,sizeof(test32_17f),test32_17f,towns,"MOV     CH,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 00000046  8A B4 8D 40 23 01 00          						MOV     DH,[EBP+ECX*4+00012340H] 
	const unsigned char test32_17g[]={0xB4,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x8A,sizeof(test32_17g),test32_17g,towns,"MOV     DH,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}

	// 0000004D  88 9C 8D 40 23 01 00          						MOV     [EBP+ECX*4+00012340H],BL 
	const unsigned char test32_18a[]={0x9C,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x88,sizeof(test32_18a),test32_18a,towns,"MOV     [EBP+ECX*4+00012340H],BL"))
	{
		return 1;
	}
	// 00000054  88 8C 8D 40 23 01 00          						MOV     [EBP+ECX*4+00012340H],CL 
	const unsigned char test32_18b[]={0x8C,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x88,sizeof(test32_18b),test32_18b,towns,"MOV     [EBP+ECX*4+00012340H],CL"))
	{
		return 1;
	}
	// 0000005B  88 94 8D 40 23 01 00          						MOV     [EBP+ECX*4+00012340H],DL 
	const unsigned char test32_18c[]={0x94,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x88,sizeof(test32_18c),test32_18c,towns,"MOV     [EBP+ECX*4+00012340H],DL"))
	{
		return 1;
	}
	// 00000062  88 A4 8D 40 23 01 00          						MOV     [EBP+ECX*4+00012340H],AH 
	const unsigned char test32_18d[]={0xA4,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x88,sizeof(test32_18d),test32_18d,towns,"MOV     [EBP+ECX*4+00012340H],AH"))
	{
		return 1;
	}
	// 00000069  88 BC 8D 40 23 01 00          						MOV     [EBP+ECX*4+00012340H],BH 
	const unsigned char test32_18e[]={0xBC,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x88,sizeof(test32_18e),test32_18e,towns,"MOV     [EBP+ECX*4+00012340H],BH"))
	{
		return 1;
	}
	// 00000070  88 AC 8D 40 23 01 00          						MOV     [EBP+ECX*4+00012340H],CH 
	const unsigned char test32_18f[]={0xAC,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x88,sizeof(test32_18f),test32_18f,towns,"MOV     [EBP+ECX*4+00012340H],CH"))
	{
		return 1;
	}
	// 00000077  88 B4 8D 40 23 01 00          						MOV     [EBP+ECX*4+00012340H],DH 
	const unsigned char test32_18g[]={0xB4,0x8D,0x40,0x23,0x01,0x00,};
	if(true!=TestDisassembly(32,32,0x88,sizeof(test32_18g),test32_18g,towns,"MOV     [EBP+ECX*4+00012340H],DH"))
	{
		return 1;
	}



	// 0000007E  8B 94 8D 40 23 01 00             						MOV     EDX,[EBP+ECX*4+00012340H]
	const unsigned char test32_19a[]={0x94,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x8B,sizeof(test32_19a),test32_19a,towns,"MOV     EDX,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 00000085  8B 8C 8D 40 23 01 00             						MOV     ECX,[EBP+ECX*4+00012340H]
	const unsigned char test32_19b[]={0x8C,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x8B,sizeof(test32_19b),test32_19b,towns,"MOV     ECX,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 0000008C  8B 9C 8D 40 23 01 00             						MOV     EBX,[EBP+ECX*4+00012340H]
	const unsigned char test32_19c[]={0x9C,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x8B,sizeof(test32_19c),test32_19c,towns,"MOV     EBX,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 00000093  8B A4 8D 40 23 01 00             						MOV     ESP,[EBP+ECX*4+00012340H]
	const unsigned char test32_19d[]={0xA4,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x8B,sizeof(test32_19d),test32_19d,towns,"MOV     ESP,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 0000009A  8B AC 8D 40 23 01 00             						MOV     EBP,[EBP+ECX*4+00012340H]
	const unsigned char test32_19e[]={0xAC,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x8B,sizeof(test32_19e),test32_19e,towns,"MOV     EBP,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 000000A1  8B B4 8D 40 23 01 00             						MOV     ESI,[EBP+ECX*4+00012340H]
	const unsigned char test32_19f[]={0xB4,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x8B,sizeof(test32_19f),test32_19f,towns,"MOV     ESI,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 000000A8  8B BC 8D 40 23 01 00             						MOV     EDI,[EBP+ECX*4+00012340H]
	const unsigned char test32_19g[]={0xBC,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x8B,sizeof(test32_19g),test32_19g,towns,"MOV     EDI,[EBP+ECX*4+00012340H]"))
	{
		return 1;
	}
	// 000000AF  89 94 8D 40 23 01 00             						MOV     [EBP+ECX*4+00012340H],EDX
	const unsigned char test32_19h[]={0x94,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x89,sizeof(test32_19h),test32_19h,towns,"MOV     [EBP+ECX*4+00012340H],EDX"))
	{
		return 1;
	}
	// 000000B6  89 8C 8D 40 23 01 00             						MOV     [EBP+ECX*4+00012340H],ECX
	const unsigned char test32_19i[]={0x8C,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x89,sizeof(test32_19i),test32_19i,towns,"MOV     [EBP+ECX*4+00012340H],ECX"))
	{
		return 1;
	}
	// 000000BD  89 9C 8D 40 23 01 00             						MOV     [EBP+ECX*4+00012340H],EBX
	const unsigned char test32_19j[]={0x9C,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x89,sizeof(test32_19j),test32_19j,towns,"MOV     [EBP+ECX*4+00012340H],EBX"))
	{
		return 1;
	}
	// 000000C4  89 A4 8D 40 23 01 00             						MOV     [EBP+ECX*4+00012340H],ESP
	const unsigned char test32_19k[]={0xA4,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x89,sizeof(test32_19k),test32_19k,towns,"MOV     [EBP+ECX*4+00012340H],ESP"))
	{
		return 1;
	}
	// 000000CB  89 AC 8D 40 23 01 00             						MOV     [EBP+ECX*4+00012340H],EBP
	const unsigned char test32_19l[]={0xAC,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x89,sizeof(test32_19l),test32_19l,towns,"MOV     [EBP+ECX*4+00012340H],EBP"))
	{
		return 1;
	}
	// 000000D2  89 B4 8D 40 23 01 00             						MOV     [EBP+ECX*4+00012340H],ESI
	const unsigned char test32_19m[]={0xB4,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x89,sizeof(test32_19m),test32_19m,towns,"MOV     [EBP+ECX*4+00012340H],ESI"))
	{
		return 1;
	}
	// 000000D9  89 BC 8D 40 23 01 00             						MOV     [EBP+ECX*4+00012340H],EDI
	const unsigned char test32_19n[]={0xBC,0x8D,0x40,0x23,0x01,0x00};
	if(true!=TestDisassembly(32,32,0x89,sizeof(test32_19n),test32_19n,towns,"MOV     [EBP+ECX*4+00012340H],EDI"))
	{
		return 1;
	}

	const unsigned char test32_20a[]={0xC3};
	if(true!=TestDisassembly(32,32,0x0FBB,sizeof(test32_20a),test32_20a,towns,"BTC     EBX,EAX"))
	{
		return 1;
	}
	const unsigned char test32_20b[]={0x23,0x00};
	if(true!=TestDisassembly(32,32,0x0FBA,sizeof(test32_20b),test32_20b,towns,"BT      DWORD PTR [EBX],00H"))
	{
		return 1;
	}
	const unsigned char test32_20c[]={0x2B,0x01};
	if(true!=TestDisassembly(32,32,0x0FBA,sizeof(test32_20c),test32_20c,towns,"BTS     DWORD PTR [EBX],01H"))
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
	const unsigned char test16_17[]={0x06,0x7E,0x05,0xA4,};
	if(true!=TestDisassembly(16,16,0xC6,sizeof(test16_17),test16_17,towns,"MOV     BYTE PTR [057EH],A4H"))
	{
		return 1;
	}

/*
	const unsigned char test16_17[]={};
	if(true!=TestDisassembly(16,16,,sizeof(test16_17),test16_17,towns,""))
	{
		return 1;
	}
*/



	return 0;
}
