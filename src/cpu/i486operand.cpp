#include <iostream>

#include "cpputil.h"
#include "i486.h"
#include "i486inst.h"



i486DX::Operand::Operand(int addressSize,int dataSize,const unsigned char operand[])
{
	Decode(addressSize,dataSize,operand);
}

void i486DX::Operand::Clear(void)
{
	operandType=OPER_UNDEFINED;
	indexScaling=1;
}

unsigned int i486DX::Operand::Decode(int addressSize,int dataSize,const unsigned char operand[])
{
	auto MODR_M=operand[0];
	auto MOD=((MODR_M>>6)&3);
	auto REG_OPCODE=((MODR_M>>3)&7);
	auto R_M=(MODR_M&7);
	unsigned int numBytes=0;

	Clear();

	if(16==addressSize)
	{
		if(0b00==MOD && 0b110==R_M)
		{
			operandType=OPER_ADDR;
			baseReg=REG_NONE;
			indexReg=REG_NONE;
			indexScaling=1;
			offset=cpputil::GetSignedWord(operand+1);
			offsetBits=16;
			numBytes=3;
		}
		else if(0b00==MOD || 0b01==MOD || 0b10==MOD)
		{
			static const unsigned int R_M_to_BaseIndex[8][2]=
			{
				{REG_BX,REG_SI},
				{REG_BX,REG_DI},
				{REG_BP,REG_SI},
				{REG_BP,REG_DI},
				{REG_SI,REG_NONE},
				{REG_DI,REG_NONE},
				{REG_BP,REG_NONE},
				{REG_BX,REG_NONE},
			};

			operandType=OPER_ADDR;
			baseReg=REG_NONE;
			indexReg=REG_NONE;
			indexScaling=1;
			offset=0;  // Tentative
			offsetBits=16;
			numBytes=1;

			baseReg=R_M_to_BaseIndex[R_M][0];
			indexReg=R_M_to_BaseIndex[R_M][1];

			if(0b01==MOD)
			{
				offsetBits=8;
				offset=cpputil::GetSignedByte(operand[1]);
				++numBytes;
			}
			else if(0b10==MOD)
			{
				offsetBits=16;
				offset=cpputil::GetSignedWord(operand+1);
				numBytes+=2;
			}
		}
		else
		{
			operandType=OPER_REG;
			if(8==dataSize)
			{
				reg=REG_8BIT_REG_BASE+R_M;
			}
			else if(16==dataSize)
			{
				reg=REG_16BIT_REG_BASE+R_M;
			}
			else if(32==dataSize)
			{
				reg=REG_32BIT_REG_BASE+R_M;
			}
			numBytes=1;
		}
	}
	else // if(32==addressSize)
	{
		if(0b00==MOD && 0b101==R_M)
		{
			operandType=OPER_ADDR;
			baseReg=REG_NONE;
			indexReg=REG_NONE;
			indexScaling=1;
			offset=cpputil::GetSignedDword(operand+1);
			offsetBits=32;
			numBytes=5;
		}
		else if(0b00==MOD || 0b01==MOD || 0b10==MOD)
		{
			operandType=OPER_ADDR;
			baseReg=REG_NONE;
			indexReg=REG_NONE;
			indexScaling=1;
			offset=0;
			numBytes=1;
			if(0b100==R_M) // Depends on SIB
			{
				auto SIB=operand[1];
				auto SS=((SIB>>6)&3);
				auto INDEX=((SIB>>3)&7);
				auto BASE=(SIB&7);
				++numBytes;

				if(5!=BASE)
				{
					baseReg=REG_32BIT_REG_BASE+BASE;
				}
				else
				{
					if(0b00==MOD) // disp32[index]
					{
						// No base
					}
					else if(0b01==MOD || 0b10==MOD) // disp[EBP][index]
					{
						baseReg=REG_EBP;
					}
				}
				if(0b100!=INDEX)
				{
					indexReg=REG_32BIT_REG_BASE+INDEX;
					indexScaling=twoToN[SS];
				}

				if((0==MOD && 5==BASE) || 0b10==MOD)
				{
					offsetBits=32;
					offset=cpputil::GetSignedDword(operand+2);
					numBytes+=4;
				}
				else if(0b01==MOD)
				{
					offsetBits=8;
					offset=cpputil::GetSignedByte(operand[2]);
					++numBytes;
				}
			}
			else
			{
				baseReg=REG_32BIT_REG_BASE+R_M;
				if(0b01==MOD) // 8-bit offset
				{
					offsetBits=8;
					offset=cpputil::GetSignedByte(operand[1]);
					++numBytes;
				}
				else if(0b10==MOD) // 32-bit offset
				{
					offsetBits=32;
					offset=cpputil::GetSignedDword(operand+1);
					numBytes+=4;
				}
			}
		}
		else if(0b11==MOD)
		{
			operandType=OPER_REG;
			if(8==dataSize)
			{
				reg=REG_8BIT_REG_BASE+R_M;
			}
			else if(16==dataSize)
			{
				reg=REG_16BIT_REG_BASE+R_M;
			}
			else if(32==dataSize)
			{
				reg=REG_32BIT_REG_BASE+R_M;
			}
			numBytes=1;
		}
	}

	return numBytes;
}
void i486DX::Operand::DecodeMODR_MForRegister(int dataSize,unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	MakeByRegisterNumber(dataSize,REG_OPCODE);
}
void i486DX::Operand::DecodeMODR_MForSegmentRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_REG;
	reg=REG_SEGMENT_REG_BASE+REG_OPCODE;
}
void i486DX::Operand::DecodeMODR_MForCRRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_REG;
	reg=REG_CR_REG_BASE+REG_OPCODE;
}
void i486DX::Operand::DecodeMODR_MForDRRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_REG;
	reg=REG_DR_REG_BASE+REG_OPCODE;
}
void i486DX::Operand::DecodeMODR_MForTestRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_REG;
	reg=REG_TEST_REG_BASE+REG_OPCODE;
}
void i486DX::Operand::MakeByRegisterNumber(int dataSize,int regNum)
{
	operandType=OPER_REG;
	switch(dataSize)
	{
	case 8:
		reg=REG_8BIT_REG_BASE+regNum;
		break;
	case 16:
		reg=REG_16BIT_REG_BASE+regNum;
		break;
	default:
		reg=REG_32BIT_REG_BASE+regNum;
		break;
	}
}
void i486DX::Operand::MakeImm8(const Instruction &inst)
{
	operandType=OPER_IMM8;
	this->imm=inst.GetUimm8();
}
void i486DX::Operand::MakeImm16(const Instruction &inst)
{
	operandType=OPER_IMM16;
	this->imm=inst.GetUimm16();
}
void i486DX::Operand::MakeImm32(const Instruction &inst)
{
	operandType=OPER_IMM32;
	this->imm=inst.GetUimm32();
}
void i486DX::Operand::MakeImm8or16or32(const Instruction &inst,unsigned int operandSize)
{
	switch(operandSize)
	{
	case 8:
		MakeImm8(inst);
		break;
	case 16:
		MakeImm16(inst);
		break;
	case 32:
		MakeImm32(inst);
		break;
	}
}

void i486DX::Operand::MakeSimpleAddressOffset(const Instruction &inst)
{
	switch(inst.addressSize)
	{
	default:
	case 32:
		operandType=OPER_ADDR;
		baseReg=REG_NONE;
		indexReg=REG_NONE;
		indexScaling=1;
		offset=cpputil::GetSignedDword(inst.operand+inst.operandLen-4);
		offsetBits=32;
		break;
	case 16:
		operandType=OPER_ADDR;
		baseReg=REG_NONE;
		indexReg=REG_NONE;
		indexScaling=1;
		offset=cpputil::GetSignedWord(inst.operand+inst.operandLen-2);
		offsetBits=16;
		break;
	}
}

bool i486DX::Operand::SignExtendImm(int newOperaType)
{
	switch(operandType)
	{
	case OPER_IMM8:
		imm&=255;
		if(0!=(imm&0x80))
		{
			imm-=256;
		}
		break;
	case OPER_IMM16:
		imm&=65535;
		if(0!=(imm&0x8000))
		{
			imm-=65536;
		}
		break;
	default:
		std::cout << "!!!! ERROR !!!!" << std::endl;
		std::cout << "Cannot Sign-Extend a non IMM8 and non IMM16." << std::endl;
		return false;
	}

	operandType=newOperaType;
	return true;
}

unsigned int i486DX::Operand::DecodeFarAddr(int addressSize,int operandSize,const unsigned char operand[])
{
	operandType=OPER_FARADDR;
	switch(operandSize)
	{
	case 16:
		offset=cpputil::GetWord(operand);
		offsetBits=16;
		seg=cpputil::GetWord(operand+2);
		return 4;
	case 32:
	default:
		offset=cpputil::GetDword(operand);
		offsetBits=32;
		seg=cpputil::GetWord(operand+4);
		return 6;
	}
}

std::string i486DX::Operand::Disassemble(void) const
{
	switch(operandType)
	{
	case OPER_ADDR:
		return DisassembleAsAddr();
	case OPER_FARADDR:
		return DisassembleAsFarAddr();
	case OPER_REG:
		return DisassembleAsReg();
	case OPER_IMM8:
		return DisassembleAsImm(8);
	case OPER_IMM16:
		return DisassembleAsImm(16);
	case OPER_IMM32:
		return DisassembleAsImm(32);
	}
	return "(UndefinedOperandType?)";
}

std::string i486DX::Operand::DisassembleAsAddr(void) const
{
	bool empty=true;

	std::string disasm;
	disasm.push_back('[');

	if(REG_NONE!=baseReg)
	{
		disasm+=RegToStr[baseReg];
		empty=false;
	}

	if(REG_NONE!=indexReg)
	{
		if(true!=empty)
		{
			disasm.push_back('+');
		}
		disasm+=RegToStr[indexReg];
		if(1!=indexScaling)
		{
			disasm.push_back('*');
			disasm.push_back('0'+indexScaling);
		}
		empty=false;
	}

	if(0!=offset)
	{
		if(true!=empty && 0<=offset)
		{
			disasm.push_back('+');
		}
		switch(offsetBits)
		{
		case 8:
			disasm+=cpputil::Btox(offset);
			break;
		case 16:
			disasm+=cpputil::Stox(offset);
			break;
		default:
			disasm+=cpputil::Itox(offset);
			break;
		}
		disasm.push_back('H');
	}
	else if(REG_NONE==baseReg && REG_NONE==indexReg)
	{
		switch(offsetBits)
		{
		case 8:
			disasm+=cpputil::Ubtox(offset);
			break;
		case 16:
			disasm+=cpputil::Ustox(offset);
			break;
		default:
			disasm+=cpputil::Uitox(offset);
			break;
		}
		disasm.push_back('H');
	}

	disasm.push_back(']');
	return disasm;
}
std::string i486DX::Operand::DisassembleAsFarAddr(void) const
{
	// It doesn't add [] because may be used by JMP.
	std::string disasm;
	switch(offsetBits)
	{
	case 16:
		disasm=cpputil::Ustox(seg);
		disasm.push_back(':');
		disasm+=cpputil::Ustox(offset);
		break;
	case 32:
	default:
		disasm=cpputil::Ustox(seg);
		disasm.push_back(':');
		disasm+=cpputil::Uitox(offset);
		break;
	}
	return disasm;
}
std::string i486DX::Operand::DisassembleAsReg(void) const
{
	return RegToStr[reg];
}
std::string i486DX::Operand::DisassembleAsImm(int immSize) const
{
	switch(immSize)
	{
	case 8:
		return cpputil::Ubtox(imm)+"H";
	case 16:
		return cpputil::Ustox(imm)+"H";
	default:
		return cpputil::Uitox(imm)+"H";
	}
}
/* static */ void i486DX::Operand::GetSizeQualifierToDisassembly(
			std::string &op1Qual,std::string &op2Qual,const Operand &op1,const Operand &op2)
{
	// If one of the two operands is IMM and the other is an address.
	if((op1.operandType==OPER_IMM8 ||
	    op1.operandType==OPER_IMM16 ||
	    op1.operandType==OPER_IMM32) &&
	    op2.operandType==OPER_ADDR)
	{
		switch(op1.operandType)
		{
		case OPER_IMM8:
			op2Qual="BYTE PTR ";
			break;
		case OPER_IMM16:
			op2Qual="WORD PTR ";
			break;
		case OPER_IMM32:
			op2Qual="DWORD PTR ";
			break;
		}
	}
	else if((op2.operandType==OPER_IMM8 ||
	    op2.operandType==OPER_IMM16 ||
	    op2.operandType==OPER_IMM32) &&
	    op1.operandType==OPER_ADDR)
	{
		switch(op2.operandType)
		{
		case OPER_IMM8:
			op1Qual="BYTE PTR ";
			break;
		case OPER_IMM16:
			op1Qual="WORD PTR ";
			break;
		case OPER_IMM32:
			op1Qual="DWORD PTR ";
			break;
		}
	}
}

/* static */ std::string i486DX::Operand::GetSizeQualifierToDisassembly(const Operand &op,int operandSize)
{
	if(op.operandType==OPER_ADDR)
	{
		switch(operandSize)
		{
		case 8:
			return "BYTE PTR ";
		case 16:
			return "WORD PTR ";
		case 32:
			return "DWORD PTR ";
		case 48:
			return "FWORD PTR ";
		}
	}
	return "";
}

/* static */ std::string i486DX::Operand::GetSegmentQualifierToDisassembly(int segOverride,const Operand &op)
{
	if(0!=segOverride && OPER_ADDR==op.operandType)
	{
		switch(segOverride)
		{
		case SEG_OVERRIDE_CS: //  0x2E,
			return "CS:";
		case SEG_OVERRIDE_SS: //  0x36,
			return "SS:";
		case SEG_OVERRIDE_DS: //  0x3E,
			return "DS:";
		case SEG_OVERRIDE_ES: //  0x26,
			return "ES:";
		case SEG_OVERRIDE_FS: //  0x64,
			return "FS:";
		case SEG_OVERRIDE_GS: //  0x65,
			return "GS:";
		}
	}
	return "";
}

unsigned int i486DX::Operand::GetSize(void) const
{
	switch(operandType)
	{
	case OPER_ADDR:
		return 0;
	case OPER_FARADDR:
		return 0;
	case OPER_REG:
		return i486DX::GetRegisterSize(reg);
	case OPER_IMM8:
		return 1;
	case OPER_IMM16:
		return 2;
	case OPER_IMM32:
		return 4;
	}
	return 0;
}
