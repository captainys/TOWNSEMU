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
	i486DX cpu(nullptr);
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

	i486DX::Operand op1,op2;
	i486SymbolTable emptySymTable;
	std::map <unsigned int,std::string> emptyIOTable;
	auto inst=cpu.FetchInstruction(op1,op2,cpu.state.CS(),cpu.state.EIP,mem,cpu.state.CS().operandSize,cpu.state.CS().addressSize);
	auto disasm=inst.Disassemble(op1,op2,cpu.state.CS(),cpu.state.EIP,emptySymTable,emptyIOTable);

	std::cout << "Disassembled as: [" << disasm << "]" << std::endl;
	if(disasm!=correctDisasm)
	{
		std::cout << "Wrong disassembly!" << std::endl;
		goto ERREND;
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
		"D5 07","ADX     07H",   // Questionable
		"D4 07","AMX     07H",   // Questionable

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


