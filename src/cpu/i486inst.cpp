#include <iostream>

#include "cpputil.h"
#include "i486.h"
#include "i486inst.h"



bool i486DX::OpCodeNeedsOneMoreByte(unsigned int firstByte) const
{
	switch(firstByte)
	{
	case I486_OPCODE_NEED_SECOND_BYTE:
		return true;
	}
	return false;
}


i486DX::Instruction i486DX::FetchInstruction(const SegmentRegister seg,unsigned int offset,const Memory &mem) const
{
	Instruction inst;
	inst.Clear();
	if(true==IsInRealMode())
	{
		inst.operandSize=16;
		inst.addressSize=16;
	}
	else
	{
		// Default operandSize and addressSize depends on the D flag of the segment descriptor.
		Abort("Protected mode not supported yet.");
	}

	// Question: Do prefixes need to be in the specific order INST_PREFIX->ADDRSIZE_OVERRIDE->OPSIZE_OVERRIDE->SEG_OVERRIDE?

	unsigned int lastByte=FetchByte(seg,offset+inst.numBytes++,mem);
	for(;;) // While looking at prefixes.
	{
		switch(lastByte)
		{
		case INST_PREFIX_REP: // REP/REPE/REPZ
		case INST_PREFIX_REPNE:
		case INST_PREFIX_LOCK:
			inst.instPrefix=lastByte;
			break;

		case SEG_OVERRIDE_CS:
		case SEG_OVERRIDE_SS:
		case SEG_OVERRIDE_DS:
		case SEG_OVERRIDE_ES:
		case SEG_OVERRIDE_FS:
		case SEG_OVERRIDE_GS:
			inst.segOverride=lastByte;
			break;

		case OPSIZE_OVERRIDE:
			inst.operandSize^=48;
			break;
		case ADDRSIZE_OVERRIDE:
			inst.addressSize^=48;
			break;
		default:
			goto PREFIX_DONE;
		}
		lastByte=FetchByte(seg,offset+inst.numBytes++,mem);
	}
PREFIX_DONE:
	inst.opCode=lastByte;
	if(true==OpCodeNeedsOneMoreByte(inst.opCode))
	{
		lastByte=FetchByte(seg,offset+inst.numBytes++,mem);
		inst.opCode<<=8;
		inst.opCode|=lastByte;
	}

	FetchOperand(inst,seg,offset+inst.numBytes,mem);

	return inst;
}

unsigned int i486DX::FetchOperand8(Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const
{
	auto byte=FetchByte(seg,offset++,mem);
	inst.operand[inst.operandLen++]=byte;
	++inst.numBytes;
	return 1;
}
unsigned int i486DX::FetchOperand16(Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const
{
	unsigned int byte[2];
	byte[0]=FetchByte(seg,offset++,mem);
	byte[1]=FetchByte(seg,offset++,mem);

	inst.operand[inst.operandLen++]=byte[0];
	inst.operand[inst.operandLen++]=byte[1];
	inst.numBytes+=2;
	return byte[0]+(byte[1]<<8);
}
unsigned int i486DX::FetchOperand32(Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const
{
	unsigned int byte[4];
	byte[0]=FetchByte(seg,offset++,mem);
	byte[1]=FetchByte(seg,offset++,mem);
	byte[2]=FetchByte(seg,offset++,mem);
	byte[3]=FetchByte(seg,offset++,mem);
	inst.operand[inst.operandLen++]=byte[0];
	inst.operand[inst.operandLen++]=byte[1];
	inst.operand[inst.operandLen++]=byte[2];
	inst.operand[inst.operandLen++]=byte[3];
	inst.numBytes+=4;
	return byte[0]+(byte[1]<<8)+(byte[2]<<16)+(byte[3]<<24);
}

void i486DX::FetchOperandRM(Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const
{
	auto MODR_M=FetchOperand8(inst,seg,offset,mem);
	auto MOD=(MODR_M>>6)&3;
	auto R_M=(MODR_M)&7;

	// [1] Table 26-1, 26-2, 26-3, pp. 26-4,26-5,26-6
	if(16==inst.addressSize)
	{
		if(0b00==MOD && 0b110==R_M) // disp16
		{
			FetchOperand16(inst,seg,offset,mem);
		}
		else if(0b01==MOD)
		{
			FetchOperand8(inst,seg,offset,mem);
		}
		else if(0b10==MOD)
		{
			FetchOperand16(inst,seg,offset,mem);
		}
	}
	else // if(32==inst.addressSize)
	{
		if(0b00==MOD)
		{
			if(0b100==R_M) // SIB
			{
				auto SIB=FetchOperand8(inst,seg,offset,mem);
				auto BASE=(SIB&7);
				// Special case MOD=0b00 && BASE==5 [1] Table 26-4 pp.26-7
				// No base, [disp32+scaled_index]
				FetchOperand32(inst,seg,offset,mem);
			}
			else if(0b101==R_M) // disp32
			{
				FetchOperand32(inst,seg,offset,mem);
			}
		}
		else if(0b01==MOD)
		{
			if(0b100==R_M) // SIB+disp8
			{
				FetchOperand8(inst,seg,offset,mem);
			}
			FetchOperand8(inst,seg,offset,mem);
		}
		else if(0b10==MOD)
		{
			if(0b100==R_M) // SIB+disp32
			{
				FetchOperand8(inst,seg,offset,mem);
			}
			FetchOperand32(inst,seg,offset,mem);
		}
	}
}

void i486DX::FetchOperand(Instruction &inst,SegmentRegister seg,int offset,const Memory &mem) const
{
	switch(inst.opCode)
	{
	case I486_OPCODE_CLD:
	case I486_OPCODE_CLI:
		break;
	case I486_OPCODE_JMP_FAR:
		switch(inst.operandSize)
		{
		case 16:
			FetchOperand16(inst,seg,offset,mem);
			offset+=2;
			break;
		case 32:
			FetchOperand32(inst,seg,offset,mem);
			offset+=4;
			break;
		}
		FetchOperand16(inst,seg,offset,mem);
		break;

	case I486_OPCODE_MOV_FROM_R8: //      0x88,
		// Example:  88 4c ff        MOV CL,[SI-1]     In Real Mode
		// Example:  88 10           MOV DL,[BX+SI]    In Real Mode
		// Example:  88 36 21 00     MOV DH,[021H]     In Real Mode
		// Example:  67 88 26 61 10  MOV [1061],AH     In Protected Mode -> disp16 may become disp32, and vise-versa

		// Example:  8D 04 C1        LEA EAX,[ECX+EAX*8] In Protected Mode
		// Example:  8D 04 41        LEA EAX,[ECX+EAX*2] In Protected Mode
	case I486_OPCODE_MOV_FROM_R: //       0x89, // 16/32 depends on OPSIZE_OVERRIDE
		// Example:  89 26 3e 00     MOV [003EH],SP
	case I486_OPCODE_MOV_TO_R8: //        0x8A,
		// Example:  8a 0e 16 00     MOV CL,[0016H]
	case I486_OPCODE_MOV_TO_R: //         0x8B, // 16/32 depends on OPSIZE_OVERRIDE
		FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_MOV_FROM_SEG: //     0x8C,
		// Example:  8c c6           MOV SI,ES
		// Sreg: ES=0, CS=1, SS=2, DS=3, FD=4, GS=5 (OPCODE part of MODR_M)  [1] pp.26-10
	case I486_OPCODE_MOV_TO_SEG: //       0x8E,
		FetchOperand8(inst,seg,offset,mem);
		break;

	case I486_OPCODE_MOV_M_TO_AL: //      0xA0,
	case I486_OPCODE_MOV_M_TO_EAX: //     0xA1, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_M_FROM_AL: //    0xA2,
	case I486_OPCODE_MOV_M_FROM_EAX: //   0xA3, // 16/32 depends on OPSIZE_OVERRIDE
		FetchOperandRM(inst,seg,offset,mem);
		break;

	case I486_OPCODE_MOV_I8_TO_AL: //     0xB0,
	case I486_OPCODE_MOV_I8_TO_CL: //     0xB1,
	case I486_OPCODE_MOV_I8_TO_DL: //     0xB2,
	case I486_OPCODE_MOV_I8_TO_BL: //     0xB3,
	case I486_OPCODE_MOV_I8_TO_AH: //     0xB4,
	case I486_OPCODE_MOV_I8_TO_CH: //     0xB5,
	case I486_OPCODE_MOV_I8_TO_DH: //     0xB6,
	case I486_OPCODE_MOV_I8_TO_BH: //     0xB7,
	case I486_OPCODE_MOV_I8_TO_RM8: //    0xC6,
		FetchOperand8(inst,seg,offset,mem);
		break;
	case I486_OPCODE_MOV_I_TO_EAX: //   0xB8, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ECX: //   0xB9, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EDX: //   0xBA, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EBX: //   0xBB, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ESP: //   0xBC, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EBP: //   0xBD, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ESI: //   0xBE, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EDI: //   0xBF, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_RM: //      0xC7, // 16/32 depends on OPSIZE_OVERRIDE
		if(16==inst.operandSize)
		{
			FetchOperand16(inst,seg,offset,mem);
		}
		else // if(32==inst.operandSize)
		{
			FetchOperand32(inst,seg,offset,mem);
		}
		break;


	default:
		// Undefined operand, or probably not implemented yet.
		break;
	}
}

std::string i486DX::Instruction::Disassemble(SegmentRegister reg,unsigned int offset) const
{
	std::string disasm;

	switch(opCode)
	{
	case I486_OPCODE_CLD:
		disasm="CLD";
		break;
	case I486_OPCODE_CLI:
		disasm="CLI";
		break;
	case I486_OPCODE_JMP_FAR:
		disasm="JMP";
		cpputil::ExtendString(disasm,8);
		{
			unsigned int seg,offset;
			switch(operandSize)
			{
			case 16:
				offset=cpputil::GetWord(operand);
				seg=cpputil::GetWord(operand+2);
				disasm+=cpputil::Ustox(seg);
				disasm+=":";
				disasm+=cpputil::Uitox(offset);
				break;
			case 32:
				offset=cpputil::GetDword(operand);
				seg=cpputil::GetWord(operand+4);
				disasm+=cpputil::Ustox(seg);
				disasm+=":";
				disasm+=cpputil::Ustox(offset);
				break;
			}
		}
		break;

/*	case I486_OPCODE_MOV_FROM_R8: //      0x88,
		disasm="MOV";
		cpputil::ExtendString(disasm,8);
		{
		}
		break;
	case I486_OPCODE_MOV_FROM_R: //       0x89, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_TO_R8: //        0x8A,
		break;
	case I486_OPCODE_MOV_TO_R: //         0x8B, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_FROM_SEG: //     0x8C,
		break;
	case I486_OPCODE_MOV_TO_SEG: //       0x8E,
		break;
	case I486_OPCODE_MOV_M_TO_AL: //      0xA0,
		break;
	case I486_OPCODE_MOV_M_TO_EAX: //     0xA1, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_M_FROM_AL: //    0xA2,
		break;
	case I486_OPCODE_MOV_M_FROM_EAX: //   0xA3, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_I8_TO_AL: //     0xB0,
		break;
	case I486_OPCODE_MOV_I8_TO_CL: //     0xB1,
		break;
	case I486_OPCODE_MOV_I8_TO_DL: //     0xB2,
		break;
	case I486_OPCODE_MOV_I8_TO_BL: //     0xB3,
		break;
	case I486_OPCODE_MOV_I8_TO_AH: //     0xB4,
		break;
	case I486_OPCODE_MOV_I8_TO_CH: //     0xB5,
		break;
	case I486_OPCODE_MOV_I8_TO_DH: //     0xB6,
		break;
	case I486_OPCODE_MOV_I8_TO_BH: //     0xB7,
		break;
	case I486_OPCODE_MOV_I_TO_EAX: //     0xB8, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_I_TO_ECX: //     0xB9, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_I_TO_EDX: //     0xBA, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_I_TO_EBX: //     0xBB, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_I_TO_ESP: //     0xBC, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_I_TO_EBP: //     0xBD, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_I_TO_ESI: //     0xBE, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_I_TO_EDI: //     0xBF, // 16/32 depends on OPSIZE_OVERRIDE
		break;
	case I486_OPCODE_MOV_I8_TO_RM8: //    0xC6,
		break;
	case I486_OPCODE_MOV_I_TO_RM: //      0xC7, // 16/32 depends on OPSIZE_OVERRIDE
		break;
*/
	}

	return disasm;
}

std::string i486DX::DisassembleAddressing(int addressSize,int dataSize,const unsigned char operand[])
{
	std::string disasm;

	auto MODR_M=operand[0];
	auto MOD=((MODR_M>>6)&3);
	auto REG_OPCODE=((MODR_M>>3)&7);
	auto R_M=(MODR_M&7);
	if(16==addressSize)
	{
		if(0b00==MOD && 0b110==R_M)
		{
			disasm="[";
			auto sword=cpputil::GetSignedWord(operand+1);
			disasm+=cpputil::Stox(sword);
			disasm+="H]";
		}
		else if(0b00==MOD || 0b01==MOD || 0b10==MOD)
		{
			switch(R_M)
			{
			case 0:
				disasm="[BX+SI";
				break;
			case 1:
				disasm="[BX+DI";
				break;
			case 2:
				disasm="[BP+SI";
				break;
			case 3:
				disasm="[BP+DI";
				break;
			case 4:
				disasm="[SI";
				break;
			case 5:
				disasm="[DI";
				break;
			case 6:
				disasm="[BP";
				break;
			case 7:
				disasm="[BX";
				break;
			}
			if(0b01==MOD)
			{
				auto sbyte=cpputil::GetSignedByte(operand[1]);
				if(0<=sbyte)
				{
					disasm.push_back('+');
				}
				disasm+=cpputil::Btox(sbyte);
				disasm.push_back('H');
			}
			else if(0b10==MOD)
			{
				auto sword=cpputil::GetSignedWord(operand+1);
				if(0<=sword)
				{
					disasm.push_back('+');
				}
				disasm+=cpputil::Stox(sword);
				disasm.push_back('H');
			}
			disasm.push_back(']');
		}
		else
		{
			if(8==dataSize)
			{
				disasm=Reg8[R_M];
			}
			else if(16==dataSize)
			{
				disasm=Reg16[R_M];
			}
			else if(32==dataSize)
			{
				disasm=Reg32[R_M];
			}
		}
	}
	else // if(32==addressSize)
	{
		disasm.push_back('[');
		if(0b00==MOD && 0b101==R_M)
		{
			auto sdword=cpputil::GetSignedDword(operand+1);
			disasm+=cpputil::Itox(sdword);
			disasm.push_back('H');
		}
		else if(0b00==MOD || 0b01==MOD || 0b10==MOD)
		{
			if(0b100==R_M) // Depends on SIB
			{
				auto SIB=operand[1];
				auto SS=((SIB>>6)&3);
				auto INDEX=((SIB>>3)&7);
				auto BASE=(SIB&7);

				if(5!=BASE)
				{
					disasm+=Reg32[BASE];
				}
				else
				{
					if(0b00==MOD) // disp32[index]
					{
						// No base
					}
					else if(0b01==MOD || 0b10==MOD) // disp[EBP][index]
					{
						disasm+="EBP";
					}
				}
				if(1!=disasm.size())
				{
					disasm.push_back('+');
				}
				if(0b100!=INDEX)
				{
					disasm+=Reg32[INDEX];
					switch(SS)
					{
					case 1:
						disasm+="*2";
						break;
					case 2:
						disasm+="*4";
						break;
					case 3:
						disasm+="*8";
						break;
					}
				}

				if((0==MOD && 5==BASE) || 0b10==MOD)
				{
					auto sdword=cpputil::GetSignedDword(operand+2);
					if(0<=sdword)
					{
						disasm.push_back('+');
					}
					disasm+=cpputil::Itox(sdword);
					disasm.push_back('H');
				}
				else if(0b01==MOD)
				{
					auto sbyte=cpputil::GetSignedByte(operand[2]);
					if(0<=sbyte)
					{
						disasm.push_back('+');
					}
					disasm+=cpputil::Btox(sbyte);
					disasm.push_back('H');
				}
			}
			else
			{
				disasm+=Reg32[R_M];
				if(0b01==MOD)
				{
					auto sbyte=cpputil::GetSignedByte(operand[1]);
					if(0<=sbyte)
					{
						disasm.push_back('+');
					}
					disasm+=cpputil::Btox(sbyte);
					disasm.push_back('H');
				}
				else if(0b10==MOD)
				{
					auto sdword=cpputil::GetSignedDword(operand+1);
					if(0<=sdword)
					{
						disasm.push_back('+');
					}
					disasm+=cpputil::Itox(sdword);
					disasm.push_back('H');
				}
			}
		}
		else if(0b11==MOD)
		{
			if(8==dataSize)
			{
				disasm=Reg8[R_M];
			}
			else if(16==dataSize)
			{
				disasm=Reg16[R_M];
			}
			else if(32==dataSize)
			{
				disasm=Reg32[R_M];
			}
		}
		disasm.push_back(']');
	}

	return disasm;
}

unsigned int i486DX::RunOneInstruction(Memory &mem,InOut &io)
{
	auto inst=FetchInstruction(state.CS,state.EIP,mem);

	bool EIPChanged=false;
	unsigned int clocksPassed=0;

	switch(inst.opCode)
	{
	case I486_OPCODE_CLD:
		state.EFLAGS&=(~EFLAGS_DIRECTION);
		clocksPassed=2;
		break;
	case I486_OPCODE_CLI:
		state.EFLAGS&=(~EFLAGS_INT_ENABLE);
		clocksPassed=2;
		break;
	case I486_OPCODE_JMP_FAR:
		{
			unsigned int seg,offset;
			switch(inst.operandSize)
			{
			case 16:
				offset=cpputil::GetWord(inst.operand);
				seg=cpputil::GetWord(inst.operand+2);
				if(true==IsInRealMode())
				{
					clocksPassed=17;
				}
				else
				{
					clocksPassed=19;
				}
				break;
			case 32:
				offset=cpputil::GetDword(inst.operand);
				seg=cpputil::GetWord(inst.operand+4);
				if(true==IsInRealMode())
				{
					clocksPassed=13;
				}
				else
				{
					clocksPassed=18;
				}
				break;
			}
			LoadSegmentRegister(state.CS,seg,mem);
			state.EIP=offset;
			EIPChanged=true;
		}
		break;
	default:
		Abort("Undefined instruction or simply not supported yet.");
		break;
	}

	if(true!=EIPChanged)
	{
		state.EIP+=inst.numBytes;
	}

	return clocksPassed;
}
