/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <map>
#include <string>

#include "cpputil.h"
#include "i486symtable.h"



class MemoryForDisassemble : public MemoryAccess
{
public:
	unsigned char ram[256];
	virtual unsigned int FetchByte(unsigned int physAddr) const
	{
		return ram[physAddr-0x1000];
	}
	virtual void StoreByte(unsigned int physAddr,unsigned char data)
	{
	}
};

bool TestDisassembly(unsigned int operandSize,unsigned int addressSize,long long int instLen,const unsigned char instByte[],const std::string &correctDisasm)
{
	static i486DX cpu(nullptr);
	cpu.Reset();

	Memory mem;
	MemoryForDisassemble memAccess;

	mem.AddAccess(&memAccess,0,0xffffff);

	cpu.state.CS().value=0;
	cpu.state.CS().baseLinearAddr=0;
	cpu.state.CS().operandSize=operandSize;
	cpu.state.CS().addressSize=addressSize;
	cpu.state.CS().limit=0x1000000;

	cpu.state.CS().value=0;
	cpu.state.SS().baseLinearAddr=0;
	cpu.state.SS().operandSize=operandSize;
	cpu.state.SS().addressSize=addressSize;
	cpu.state.SS().limit=0x1000000;

	cpu.state.CS().value=0;
	cpu.state.DS().baseLinearAddr=0;
	cpu.state.DS().operandSize=operandSize;
	cpu.state.DS().addressSize=addressSize;
	cpu.state.DS().limit=0x1000000;

	cpu.state.EIP=0x1000;

	cpu.SetCR(0,i486DX::CR0_PROTECTION_ENABLE);

	for(unsigned int i=0; i<instLen; ++i)
	{
		memAccess.ram[i]=instByte[i];
	}

	i486DX::Instruction inst;
	i486DX::Operand op1,op2;
	i486SymbolTable emptySymTable;
	MemoryAccess::ConstMemoryWindow memWin;
	std::map <unsigned int,std::string> emptyIOTable;

	{
		cpu.DebugFetchInstruction(memWin,inst,op1,op2,cpu.state.CS(),cpu.state.EIP,mem,cpu.state.CS().operandSize,cpu.state.CS().addressSize);
		auto disasm=inst.Disassemble(op1,op2,cpu.state.CS(),cpu.state.EIP,emptySymTable,emptyIOTable);
		std::cout << "i486DX::DebugFetchInstruction()" << std::endl;
		std::cout << "Disassembled as: [" << disasm << "]" << std::endl;
		if(disasm!=correctDisasm)
		{
			std::cout << "Wrong disassembly!" << std::endl;
			goto ERREND;
		}
	}
	{
		cpu.FetchInstruction(memWin,inst,op1,op2,cpu.state.CS(),cpu.state.EIP,mem,cpu.state.CS().operandSize,cpu.state.CS().addressSize);
		auto disasm=inst.Disassemble(op1,op2,cpu.state.CS(),cpu.state.EIP,emptySymTable,emptyIOTable);
		std::cout << "i486DX::FetchInstruction()" << std::endl;
		std::cout << "Disassembled as: [" << disasm << "]" << std::endl;
		if(disasm!=correctDisasm)
		{
			std::cout << "Wrong disassembly!" << std::endl;
			goto ERREND;
		}
	}

	return true;

ERREND:
	std::cout << "Correct Disassembly: [" << correctDisasm << "]" << std::endl;
	std::cout << "Instruction: ";
	for(unsigned i=0; i<instLen; ++i)
	{
		std::cout << cpputil::Ubtox(instByte[i]) << " ";
	}
	std::cout << std::endl;
	return false;
}

bool TestDisassembly(unsigned int operandSize,unsigned int addressSize,const char instByteStr[],const std::string &correctDisasm)
{
	auto argv=cpputil::Parser(instByteStr);
	std::vector <unsigned char> instByte;
	for(auto arg : argv)
	{
		for(long long int i=arg.size()-2; 0<=i; i-=2)
		{
			char hex[2]={arg[i],arg[i+1]};
			instByte.push_back(cpputil::Xtoi(hex));
		}
	}
	return TestDisassembly(operandSize,addressSize,instByte.size(),instByte.data(),correctDisasm);
}

int main(int ac,char *av[])
{
	const unsigned char MOV_AL_MEM_32[]={0x8A,0x84,0x96,0xFF,0x1F,0x00,0x00};
	if(true!=TestDisassembly(32,32,sizeof(MOV_AL_MEM_32),MOV_AL_MEM_32,"MOV     AL,[ESI+EDX*4+00001FFFH]"))
	{
		return 1;
	}


	const char *const test32_32[]=
	{
		"E5 77","IN      EAX,77H",
		"8B C2","MOV     EAX,EDX",
		"8B D0","MOV     EDX,EAX",
		"8A 84 96 FF 1F 00 00","MOV     AL,[ESI+EDX*4+00001FFFH]",
		"8A 84 95 FF 2F 00 00","MOV     AL,[EBP+EDX*4+00002FFFH]",
		"B8 78 56 34 12","MOV     EAX,12345678H",
		"B9 78 56 34 12","MOV     ECX,12345678H",
		"BA 78 56 34 12","MOV     EDX,12345678H",
		"BB 78 56 34 12","MOV     EBX,12345678H",
		"BC AB AB AB AB","MOV     ESP,ABABABABH",
		"BD AB AB AB AB","MOV     EBP,ABABABABH",
		"BE AB AB AB AB","MOV     ESI,ABABABABH",
		"BF AB AB AB AB","MOV     EDI,ABABABABH",
		"FE 0D 67 45 23 01","DEC     BYTE PTR [01234567H]",
		"FF 0D 67 45 23 01","DEC     DWORD PTR [01234567H]",
		"FE 8C 70 67 45 23 01","DEC     BYTE PTR [EAX+ESI*2+01234567H]",
		"34 68","XOR     AL,68H",
		"35 67 45 23 01","XOR     EAX,01234567H",
		"80 F3 77","XOR     BL,77H",
		"80 E3 77","AND     BL,77H",
		"80 B4 97 67 45 23 01 86","XOR     BYTE PTR [EDI+EDX*4+01234567H],86H",
		"81 F3 67 45 23 01","XOR     EBX,01234567H",
		"81 E3 67 45 23 01","AND     EBX,01234567H",
		"81 B4 46 67 45 23 01 EF CD AB 89","XOR     DWORD PTR [ESI+EAX*2+01234567H],89ABCDEFH",
		"83 F1 80","XOR     ECX,-80H",
		"83 B5 77 77 77 77 80","XOR     DWORD PTR [EBP+77777777H],-80H",
		"83 F1 11","XOR     ECX,11H",
		"83 B4 24 77 77 77 77 11","XOR     DWORD PTR [ESP+77777777H],11H",
		"32 DF","XOR     BL,BH",
		"32 E9","XOR     CH,CL",
		"33 D9","XOR     EBX,ECX",
		"33 CB","XOR     ECX,EBX",
		"32 9F 11 11 11 11","XOR     BL,[EDI+11111111H]",
		"33 96 11 11 11 11","XOR     EDX,[ESI+11111111H]",
		"8A 9C 8D 40 23 01 00,","MOV     BL,[EBP+ECX*4+00012340H]",
		"8A 8C 8D 40 23 01 00,","MOV     CL,[EBP+ECX*4+00012340H]",
		"8A 94 8D 40 23 01 00,","MOV     DL,[EBP+ECX*4+00012340H]",
		"8A A4 8D 40 23 01 00,","MOV     AH,[EBP+ECX*4+00012340H]",
		"8A BC 8D 40 23 01 00,","MOV     BH,[EBP+ECX*4+00012340H]",
		"8A AC 8D 40 23 01 00,","MOV     CH,[EBP+ECX*4+00012340H]",
		"8A B4 8D 40 23 01 00,","MOV     DH,[EBP+ECX*4+00012340H]",
		"88 9C 8D 40 23 01 00,","MOV     [EBP+ECX*4+00012340H],BL",
		"88 8C 8D 40 23 01 00,","MOV     [EBP+ECX*4+00012340H],CL",
		"88 94 8D 40 23 01 00,","MOV     [EBP+ECX*4+00012340H],DL",
		"88 A4 8D 40 23 01 00,","MOV     [EBP+ECX*4+00012340H],AH",
		"88 BC 8D 40 23 01 00,","MOV     [EBP+ECX*4+00012340H],BH",
		"88 AC 8D 40 23 01 00,","MOV     [EBP+ECX*4+00012340H],CH",
		"88 B4 8D 40 23 01 00,","MOV     [EBP+ECX*4+00012340H],DH",
		"8B 94 8D 40 23 01 00","MOV     EDX,[EBP+ECX*4+00012340H]",
		"8B 8C 8D 40 23 01 00","MOV     ECX,[EBP+ECX*4+00012340H]",
		"8B 9C 8D 40 23 01 00","MOV     EBX,[EBP+ECX*4+00012340H]",
		"8B A4 8D 40 23 01 00","MOV     ESP,[EBP+ECX*4+00012340H]",
		"8B AC 8D 40 23 01 00","MOV     EBP,[EBP+ECX*4+00012340H]",
		"8B B4 8D 40 23 01 00","MOV     ESI,[EBP+ECX*4+00012340H]",
		"8B BC 8D 40 23 01 00","MOV     EDI,[EBP+ECX*4+00012340H]",
		"89 94 8D 40 23 01 00","MOV     [EBP+ECX*4+00012340H],EDX",
		"89 8C 8D 40 23 01 00","MOV     [EBP+ECX*4+00012340H],ECX",
		"89 9C 8D 40 23 01 00","MOV     [EBP+ECX*4+00012340H],EBX",
		"89 A4 8D 40 23 01 00","MOV     [EBP+ECX*4+00012340H],ESP",
		"89 AC 8D 40 23 01 00","MOV     [EBP+ECX*4+00012340H],EBP",
		"89 B4 8D 40 23 01 00","MOV     [EBP+ECX*4+00012340H],ESI",
		"89 BC 8D 40 23 01 00","MOV     [EBP+ECX*4+00012340H],EDI",
		"0F BB C3","BTC     EBX,EAX",
		"0F BA 23 00","BT      DWORD PTR [EBX],00H",
		"0F BA 2B 01","BTS     DWORD PTR [EBX],01H",

		"C0 C5 05","ROL     CH,05H",
		"C0 C3 03","ROL     BL,03H", 
		"C1 FB 07","SAR     EBX,07H",
		"C1 D2 0B","RCL     EDX,0BH",

		"D0 CC","ROR     AH,1",
		"D0 EE","SHR     DH,1",
		"D2 C8","ROR     AL,CL",
		"D2 ED","SHR     CH,CL",

		"D5 0A","AAD",
		"D4 0A","AAM",
		"D5 07","AAD     07H",   // Questionable
		"D4 07","AAM     07H",   // Questionable

		"F6 C6 0C",      "TEST    DH,0CH",
		"66 F7 C1 000E", "TEST    CX,000EH",
		"F7 C3 00000008","TEST    EBX,00000008H",

		"B0 10","MOV     AL,10H",
		"B7 20","MOV     BH,20H",
		"B1 33","MOV     CL,33H",
		"B2 44","MOV     DL,44H",

		"C6 06 55",      "MOV     BYTE PTR [ESI],55H",
		"C7 07 00001234","MOV     DWORD PTR [EDI],00001234H",

		"B8 12345678",   "MOV     EAX,12345678H",
		"66 BB 5678",    "MOV     BX,5678H",

		"E4 10",   "IN      AL,10H",
		"66 E5 12","IN      AX,12H",
		"E5 77",   "IN      EAX,77H",

		"CD 21",   "INT     21H",

		"66 15 0064", "ADC     AX,0064H",
		"1D 00008F00","SBB     EAX,00008F00H",
		"66 A9 007F", "TEST    AX,007FH",
		"35 00001234","XOR     EAX,00001234H",

		"14 70     ","ADC     AL,70H",
		"04 71     ","ADD     AL,71H",
		"24 72     ","AND     AL,72H",
		"3C 73     ","CMP     AL,73H",
		"0C 74     ","OR      AL,74H",
		"1C 75     ","SBB     AL,75H",
		"2C 76     ","SUB     AL,76H",
		"A8 77     ","TEST    AL,77H",
		"34 78     ","XOR     AL,78H",

		"66 15 0170","ADC     AX,0170H",
		"66 05 0171","ADD     AX,0171H",
		"66 25 0172","AND     AX,0172H",
		"66 3D 0173","CMP     AX,0173H",
		"66 0D 0174","OR      AX,0174H",
		"66 1D 0175","SBB     AX,0175H",
		"66 2D 0176","SUB     AX,0176H",
		"66 A9 0177","TEST    AX,0177H",
		"66 35 0178","XOR     AX,0178H",

		"15 00000170","ADC     EAX,00000170H",
		"05 00000171","ADD     EAX,00000171H",
		"25 00000172","AND     EAX,00000172H",
		"3D 00000173","CMP     EAX,00000173H",
		"0D 00000174","OR      EAX,00000174H",
		"1D 00000175","SBB     EAX,00000175H",
		"2D 00000176","SUB     EAX,00000176H",
		"A9 00000177","TEST    EAX,00000177H",
		"35 00000178","XOR     EAX,00000178H",

		"C2 0164       ","RET     0164H",
		"CA 0164       ","RETF    0164H",
		"68 00000164   ","PUSH    DWORD PTR 00000164H",
		"6A 64         ","PUSH    DWORD PTR 64H",
		"E6 10         ","OUT     10H,AL",
		"66 E7 12      ","OUT     12H,AX",
		"E7 14         ","OUT     14H,EAX",
		"C6 06 77      ","MOV     BYTE PTR [ESI],77H",
		"66 C7 06 7777 ","MOV     WORD PTR [ESI],7777H",
		"C7 06 77777777","MOV     DWORD PTR [ESI],77777777H",
		"80 07 64      ","ADD     BYTE PTR [EDI],64H",
		"66 81 CF 0084 ","OR      DI,0084H",
		"66 81 D6 8801 ","ADC     SI,8801H",
		"66 83 DA F0   ","SBB     DX,-10H",
		"83 E1 E0      ","AND     ECX,-20H",
		"80 EB 10      ","SUB     BL,10H",
		"80 F7 20      ","XOR     BH,20H",
		"80 3F 98      ","CMP     BYTE PTR [EDI],98H",

		"6A 68"      ,"PUSH    DWORD PTR 68H",
		"66 6A 68"   ,"PUSH    WORD PTR 68H",
		"68 00006809","PUSH    DWORD PTR 00006809H",
		"66 68 6809" ,"PUSH    WORD PTR 6809H",

		"26 A0 01234567",   "MOV     AL,ES:[01234567H]",
		"66 64 A1 01234567","MOV     AX,FS:[01234567H]",
		"65 A1 01234567",   "MOV     EAX,GS:[01234567H]",
		                               
		"26 A2 01234567",   "MOV     ES:[01234567H],AL",
		"66 64 A3 01234567","MOV     FS:[01234567H],AX",
		"65 A3 01234567",   "MOV     GS:[01234567H],EAX",

		"37", "AAA",
		"27", "DAA",
		"2F", "DAS",

		"13 C3",             "ADC     EAX,EBX",
		"03 C3",             "ADD     EAX,EBX",
		"23 C3",             "AND     EAX,EBX",
		"3B C3",             "CMP     EAX,EBX",
		"0B C3",             "OR      EAX,EBX",
		"1B C3",             "SBB     EAX,EBX",
		"2B C3",             "SUB     EAX,EBX",
		"33 C3",             "XOR     EAX,EBX",

		"66 13 DA",          "ADC     BX,DX",
		"66 03 DA",          "ADD     BX,DX",
		"66 23 DA",          "AND     BX,DX",
		"66 3B DA",          "CMP     BX,DX",
		"66 0B DA",          "OR      BX,DX",
		"66 1B DA",          "SBB     BX,DX",
		"66 2B DA",          "SUB     BX,DX",
		"66 33 DA",          "XOR     BX,DX",

		"12 DE",             "ADC     BL,DH",
		"02 DE",             "ADD     BL,DH",
		"22 DE",             "AND     BL,DH",
		"3A DE",             "CMP     BL,DH",
		"0A DE",             "OR      BL,DH",
		"1A DE",             "SBB     BL,DH",
		"2A DE",             "SUB     BL,DH",
		"32 DE",             "XOR     BL,DH",

		"13 44 7D 04",       "ADC     EAX,[EBP+EDI*2+04H]",
		"03 5C 7D 04",       "ADD     EBX,[EBP+EDI*2+04H]",
		"23 4C 7D 04",       "AND     ECX,[EBP+EDI*2+04H]",
		"3B 54 7D 04",       "CMP     EDX,[EBP+EDI*2+04H]",
		"0B 74 7D 04",       "OR      ESI,[EBP+EDI*2+04H]",
		"1B 7C 7D 04",       "SBB     EDI,[EBP+EDI*2+04H]",
		"2B 6C 7D 04",       "SUB     EBP,[EBP+EDI*2+04H]",
		"33 64 7D 04",       "XOR     ESP,[EBP+EDI*2+04H]",

		"66 13 7C 7D 04",    "ADC     DI,[EBP+EDI*2+04H]",
		"66 03 7C 7D 04",    "ADD     DI,[EBP+EDI*2+04H]",
		"66 23 7C 7D 04",    "AND     DI,[EBP+EDI*2+04H]",
		"66 3B 7C 7D 04",    "CMP     DI,[EBP+EDI*2+04H]",
		"66 0B 7C 7D 04",    "OR      DI,[EBP+EDI*2+04H]",
		"66 1B 7C 7D 04",    "SBB     DI,[EBP+EDI*2+04H]",
		"66 2B 7C 7D 04",    "SUB     DI,[EBP+EDI*2+04H]",
		"66 33 7C 7D 04",    "XOR     DI,[EBP+EDI*2+04H]",

		"12 44 7D 04",       "ADC     AL,[EBP+EDI*2+04H]",
		"02 7C 7D 04",       "ADD     BH,[EBP+EDI*2+04H]",
		"22 4C 7D 04",       "AND     CL,[EBP+EDI*2+04H]",
		"3A 74 7D 04",       "CMP     DH,[EBP+EDI*2+04H]",
		"0A 44 7D 04",       "OR      AL,[EBP+EDI*2+04H]",
		"1A 7C 7D 04",       "SBB     BH,[EBP+EDI*2+04H]",
		"2A 4C 7D 04",       "SUB     CL,[EBP+EDI*2+04H]",
		"32 74 7D 04",       "XOR     DH,[EBP+EDI*2+04H]",

		"11 84 7D 12345678",      "ADC     [EBP+EDI*2+12345678H],EAX",
		"01 9C 7D 12345678",      "ADD     [EBP+EDI*2+12345678H],EBX",
		"21 8C 7D 12345678",      "AND     [EBP+EDI*2+12345678H],ECX",
		"39 94 7D 12345678",      "CMP     [EBP+EDI*2+12345678H],EDX",
		"09 B4 7D 12345678",      "OR      [EBP+EDI*2+12345678H],ESI",
		"19 BC 7D 12345678",      "SBB     [EBP+EDI*2+12345678H],EDI",
		"29 AC 7D 12345678",      "SUB     [EBP+EDI*2+12345678H],EBP",
		"31 A4 7D 12345678",      "XOR     [EBP+EDI*2+12345678H],ESP",
		"66 11 BC 7D 12345678",   "ADC     [EBP+EDI*2+12345678H],DI", 
		"66 01 BC 7D 12345678",   "ADD     [EBP+EDI*2+12345678H],DI", 
		"66 21 BC 7D 12345678",   "AND     [EBP+EDI*2+12345678H],DI", 
		"66 39 BC 7D 12345678",   "CMP     [EBP+EDI*2+12345678H],DI", 
		"66 09 BC 7D 12345678",   "OR      [EBP+EDI*2+12345678H],DI", 
		"66 19 BC 7D 12345678",   "SBB     [EBP+EDI*2+12345678H],DI", 
		"66 29 BC 7D 12345678",   "SUB     [EBP+EDI*2+12345678H],DI", 
		"66 31 BC 7D 12345678",   "XOR     [EBP+EDI*2+12345678H],DI", 
		"10 84 7D 12345678",      "ADC     [EBP+EDI*2+12345678H],AL", 
		"00 BC 7D 12345678",      "ADD     [EBP+EDI*2+12345678H],BH", 
		"20 8C 7D 12345678",      "AND     [EBP+EDI*2+12345678H],CL", 
		"38 B4 7D 12345678",      "CMP     [EBP+EDI*2+12345678H],DH", 
		"08 84 7D 12345678",      "OR      [EBP+EDI*2+12345678H],AL", 
		"18 BC 7D 12345678",      "SBB     [EBP+EDI*2+12345678H],BH", 
		"28 8C 7D 12345678",      "SUB     [EBP+EDI*2+12345678H],CL", 
		"30 B4 7D 12345678",      "XOR     [EBP+EDI*2+12345678H],DH", 
		"86 B4 7D 12345678",      "XCHG    [EBP+EDI*2+12345678H],DH", 

		"86 3C 07",               "XCHG    [EDI+EAX],BH",
		"86 14 1E",               "XCHG    [ESI+EBX],DL",
		"86 6C 05 00",            "XCHG    [EBP+EAX],CH",
		"0F BE 0C 07",            "MOVSX   ECX,BYTE PTR [EDI+EAX]",
		"0F B6 14 16",            "MOVZX   EDX,BYTE PTR [ESI+EDX]",
		"66 0F BE 0C 07",         "MOVSX   CX,BYTE PTR [EDI+EAX]",
		"66 0F B6 14 16",         "MOVZX   DX,BYTE PTR [ESI+EDX]",
		"0F BC 1C 06",            "BSF     EBX,[ESI+EAX]",
		"0F BD 14 1F",            "BSR     EDX,[EDI+EBX]",
		"0F 02 04 06",            "LAR     EAX,[ESI+EAX]",
		"0F AF 45 00",            "IMUL    EAX,[EBP]",
		"8D 3C 07",               "LEA     EDI,[EDI+EAX]",
		"C5 31",                  "LDS     ESI,[ECX]",
		"C5 39",                  "LDS     EDI,[ECX]",
		"0F B4 39",               "LFS     EDI,[ECX]",
		"0F B5 39",               "LGS     EDI,[ECX]",
		"0F B2 39",               "LSS     EDI,[ECX]",

		"0F A3 0C 06",            "BT      [ESI+EAX],ECX",
		"0F BB 0C 06",            "BTC     [ESI+EAX],ECX",
		"0F AB 0C 06",            "BTS     [ESI+EAX],ECX",
		"0F B3 0C 06",            "BTR     [ESI+EAX],ECX",
		"0F A5 D8",               "SHLD    EAX,EBX,CL",
		"0F AD D8",               "SHRD    EAX,EBX,CL",

		"23 C0",             "AND     EAX,EAX",
		"66 23 C0",          "AND     AX,AX",
		"22 E4",             "AND     AH,AH",
		"22 C0",             "AND     AL,AL",
	};

	for(int i=0; i<sizeof(test32_32)/sizeof(test32_32[0]); i+=2)
	{
		if(true!=TestDisassembly(32,32,test32_32[i],test32_32[i+1]))
		{
			return 1;
		}
	}



	const char *const test16_16[]=
	{
		"B0 77","MOV     AL,77H",
		"B1 77","MOV     CL,77H",
		"B2 77","MOV     DL,77H",
		"B3 77","MOV     BL,77H",
		"B4 77","MOV     AH,77H",
		"B5 77","MOV     CH,77H",
		"B6 77","MOV     DH,77H",
		"B7 77","MOV     BH,77H",
		"C6 06 7E 05 A4","MOV     BYTE PTR [057EH],A4H",
		"E4 77","IN      AL,77H",
		"E5 77","IN      AX,77H",

		"8B C2",      "MOV     AX,DX",
		"8B D0",      "MOV     DX,AX",
		"8A 80 1FFF", "MOV     AL,[BX+SI+1FFFH]",
		"8A 83 2FFF", "MOV     AL,[BP+DI+2FFFH]",
		"B9 5678",    "MOV     CX,5678H",
		"BC 9876",    "MOV     SP,9876H",
		"26 A0 4567", "MOV     AL,ES:[4567H]",
		"2E A1 4567", "MOV     AX,CS:[4567H]",
		"26 A2 4567", "MOV     ES:[4567H],AL",
		"2E A3 4567", "MOV     CS:[4567H],AX",
		"37", "AAA",
		"27", "DAA",
		"2F", "DAS",
	};
	for(int i=0; i<sizeof(test16_16)/sizeof(test16_16[0]); i+=2)
	{
		if(true!=TestDisassembly(16,16,test16_16[i],test16_16[i+1]))
		{
			return 1;
		}
	}





	return 0;
}


