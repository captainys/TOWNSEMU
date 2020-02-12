#include <iostream>

#include "cpputil.h"
#include "i486.h"
#include "i486inst.h"
#include "i486debug.h"
#include "i486symtable.h"



bool i486DX::OpCodeNeedsOneMoreByte(unsigned int firstByte) const
{
	switch(firstByte)
	{
	case I486_OPCODE_NEED_SECOND_BYTE:
	case I486_OPCODE_NEED_SECOND_BYTE_AAD://_=0xD5,
	case I486_OPCODE_NEED_SECOND_BYTE_AAM://_=0xD4,
		return true;
	}
	return false;
}


i486DX::Instruction i486DX::FetchInstruction(const SegmentRegister &CS,unsigned int offset,const Memory &mem,unsigned int defOperSize,unsigned int defAddrSize) const
{
	Instruction inst;
	inst.Clear();
	inst.operandSize=defOperSize;
	inst.addressSize=defAddrSize;

	// Question: Do prefixes need to be in the specific order INST_PREFIX->ADDRSIZE_OVERRIDE->OPSIZE_OVERRIDE->SEG_OVERRIDE?

	unsigned int lastByte=FetchByte(defAddrSize,CS,offset+inst.numBytes++,mem);
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

		case FPU_FWAIT:
			inst.fwait=lastByte;
			break;

		default:
			goto PREFIX_DONE;
		}
		lastByte=FetchByte(inst.addressSize,CS,offset+inst.numBytes++,mem);
	}
PREFIX_DONE:
	inst.opCode=lastByte;
	if(true==OpCodeNeedsOneMoreByte(inst.opCode))
	{
		lastByte=FetchByte(inst.addressSize,CS,offset+inst.numBytes++,mem);
		inst.opCode|=(lastByte<<8);
	}

	FetchOperand(inst,CS,offset+inst.numBytes,mem);

	return inst;
}

unsigned int i486DX::FetchOperand8(Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	auto byte=FetchByte(inst.addressSize,seg,offset++,mem);
	inst.operand[inst.operandLen++]=byte;
	++inst.numBytes;
	return 1;
}
unsigned int i486DX::PeekOperand8(unsigned int &operand,const Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	operand=FetchByte(inst.addressSize,seg,offset,mem);
	return 1;
}
unsigned int i486DX::FetchOperand16(Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	unsigned int byte[2];
	byte[0]=FetchByte(inst.addressSize,seg,offset++,mem);
	byte[1]=FetchByte(inst.addressSize,seg,offset++,mem);

	inst.operand[inst.operandLen++]=byte[0];
	inst.operand[inst.operandLen++]=byte[1];
	inst.numBytes+=2;
	return 2;
}
unsigned int i486DX::FetchOperand32(Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	unsigned int byte[4];
	byte[0]=FetchByte(inst.addressSize,seg,offset++,mem);
	byte[1]=FetchByte(inst.addressSize,seg,offset++,mem);
	byte[2]=FetchByte(inst.addressSize,seg,offset++,mem);
	byte[3]=FetchByte(inst.addressSize,seg,offset++,mem);
	inst.operand[inst.operandLen++]=byte[0];
	inst.operand[inst.operandLen++]=byte[1];
	inst.operand[inst.operandLen++]=byte[2];
	inst.operand[inst.operandLen++]=byte[3];
	inst.numBytes+=4;
	return 4;
}

unsigned int i486DX::FetchOperand16or32(Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	if(16==inst.operandSize)
	{
		return FetchOperand16(inst,seg,offset,mem);
	}
	else // if(32==inst.operandSize)
	{
		return FetchOperand32(inst,seg,offset,mem);
	}
}

unsigned int i486DX::FetchOperandRM(Instruction &inst,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	offset+=FetchOperand8(inst,seg,offset,mem);

	unsigned int numBytesFetched=1;
	auto MODR_M=inst.operand[inst.operandLen-1];
	auto MOD=(MODR_M>>6)&3;
	auto R_M=(MODR_M)&7;

	// [1] Table 26-1, 26-2, 26-3, pp. 26-4,26-5,26-6
	if(16==inst.addressSize)
	{
		if(0b00==MOD && 0b110==R_M) // disp16
		{
			numBytesFetched+=FetchOperand16(inst,seg,offset,mem);
		}
		else if(0b01==MOD)
		{
			numBytesFetched+=FetchOperand8(inst,seg,offset,mem);
		}
		else if(0b10==MOD)
		{
			numBytesFetched+=FetchOperand16(inst,seg,offset,mem);
		}
	}
	else // if(32==inst.addressSize)
	{
		if(0b00==MOD)
		{
			if(0b100==R_M) // SIB
			{
				FetchOperand8(inst,seg,offset,mem);
				++numBytesFetched;
				++offset;

				auto SIB=inst.operand[inst.operandLen-1];
				auto BASE=(SIB&7);
				// Special case MOD=0b00 && BASE==5 [1] Table 26-4 pp.26-7
				// No base, [disp32+scaled_index]
				if(5==BASE)
				{
					numBytesFetched+=FetchOperand32(inst,seg,offset,mem);
				}
			}
			else if(0b101==R_M) // disp32
			{
				numBytesFetched+=FetchOperand32(inst,seg,offset,mem);
			}
		}
		else if(0b01==MOD)
		{
			if(0b100==R_M) // SIB+disp8
			{
				FetchOperand8(inst,seg,offset,mem);
				++numBytesFetched;
				++offset;
			}
			FetchOperand8(inst,seg,offset,mem);
			++numBytesFetched;
		}
		else if(0b10==MOD)
		{
			if(0b100==R_M) // SIB+disp32
			{
				FetchOperand8(inst,seg,offset,mem);
				++numBytesFetched;
				++offset;
			}
			numBytesFetched+=FetchOperand32(inst,seg,offset,mem);
		}
	}

	return numBytesFetched;
}

std::string i486DX::Instruction::SegmentOverrideString(int segOverridePrefix)
{
	switch(segOverridePrefix)
	{
	case SEG_OVERRIDE_CS://  0x2E,
		return "CS:";
	case SEG_OVERRIDE_SS://  0x36,
		return "SS:";
	case SEG_OVERRIDE_DS://  0x3E,
		return "DS:";
	case SEG_OVERRIDE_ES://  0x26,
		return "ES:";
	case SEG_OVERRIDE_FS://  0x64,
		return "FS:";
	case SEG_OVERRIDE_GS://  0x65,
		return "GS:";
	}
	return "";
}

void i486DX::FetchOperand(Instruction &inst,const SegmentRegister &seg,int offset,const Memory &mem) const
{
	switch(inst.opCode)
	{
	case I486_OPCODE_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8://0xC0,// ::ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8:// 0xC1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		offset+=FetchOperandRM(inst,seg,offset,mem);
		FetchOperand8(inst,seg,offset,mem);
		break;
	case I486_OPCODE_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1://0xD0, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1://0xD1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL://0xD2,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL://0xD3, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		offset+=FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF6
		offset+=FetchOperandRM(inst,seg,offset,mem);
		if(0==inst.GetREG()) // TEST RM8,I8
		{
			FetchOperand8(inst,seg,offset,mem);
		}
		inst.operandSize=8;
		break;
	case I486_OPCODE_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF7,
		offset+=FetchOperandRM(inst,seg,offset,mem);
		if(0==inst.GetREG()) // TEST RM8,I8
		{
			FetchOperand16or32(inst,seg,offset,mem);
		}
		break;


	case I486_OPCODE_ARPL://       0x63,
		FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_BTS_RM_R://   0xAB0F,
		FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_CALL_REL://   0xE8,
	case I486_OPCODE_JMP_REL://          0xE9,   // cw or cd
		FetchOperand16or32(inst,seg,offset,mem);
		break;
	case I486_OPCODE_CALL_FAR://   0x9A,
	case I486_OPCODE_JMP_FAR:
		offset+=FetchOperand16or32(inst,seg,offset,mem);
		FetchOperand16(inst,seg,offset,mem);
		break;


	case I486_OPCODE_CBW_CWDE://        0x98,
	case I486_OPCODE_CWD_CDQ://         0x99,
	case I486_OPCODE_CLC:
	case I486_OPCODE_CLD:
	case I486_OPCODE_CLI:
	case I486_OPCODE_CMC://        0xF5,
		break;


	case I486_OPCODE_CMPSB://           0xA6,
		inst.operandSize=8;
		break;
	case I486_OPCODE_CMPS://            0xA7,
		break;


	case I486_OPCODE_DEC_EAX:
	case I486_OPCODE_DEC_ECX:
	case I486_OPCODE_DEC_EDX:
	case I486_OPCODE_DEC_EBX:
	case I486_OPCODE_DEC_ESP:
	case I486_OPCODE_DEC_EBP:
	case I486_OPCODE_DEC_ESI:
	case I486_OPCODE_DEC_EDI:
		break;


	case I486_OPCODE_ENTER://      0xC8,
		offset+=FetchOperand16(inst,seg,offset,mem);
		FetchOperand8(inst,seg,offset,mem);
		break;


	case I486_OPCODE_FWAIT://      0x9B,
		break;
	case I486_OPCODE_FPU_D9_FNSTCW_M16_FNSTENV_F2XM1_FXAM_FXCH_FXTRACT_FYL2X_FYL2XP1_FABS_:// 0xD9,
		{
			unsigned int MODR_M;
			PeekOperand8(MODR_M,inst,seg,offset,mem);
			if(0xF0<=MODR_M && MODR_M<=0xFF)
			{
				FetchOperand8(inst,seg,offset,mem);
			}
			else
			{
				FetchOperandRM(inst,seg,offset,mem);
			}
		}
		break;
	case I486_OPCODE_FPU_DB_FNINIT_FRSTOR://     0xDB, 
		{
			unsigned int MODR_M;
			PeekOperand8(MODR_M,inst,seg,offset,mem);
			if(0xE3==MODR_M) // FNINIT
			{
				FetchOperand8(inst,seg,offset,mem);
			}
			else
			{
				FetchOperand8(inst,seg,offset,mem);
				switch(Instruction::GetREG(MODR_M))
				{
				}
			}
		}
		break;
	case I486_OPCODE_FPU_DF_FNSTSW_AX://  0xDF,
		{
			unsigned int MODR_M;
			PeekOperand8(MODR_M,inst,seg,offset,mem);
			if(0xE0==MODR_M) // FNSTSW
			{
				FetchOperand8(inst,seg,offset,mem);
			}
			else
			{
				FetchOperand8(inst,seg,offset,mem); // Tentative.
				switch(Instruction::GetREG(MODR_M))
				{
				}
			}
		}
		break;


	case I486_OPCODE_IN_AL_I8://=        0xE4,
	case I486_OPCODE_IN_A_I8://=         0xE5,
		FetchOperand8(inst,seg,offset,mem);
		break;
	case I486_OPCODE_IN_AL_DX://=        0xEC,
	case I486_OPCODE_IN_A_DX://=         0xED,
		break;


	case I486_OPCODE_IMUL_R_RM_I8://0x6B,
		offset+=FetchOperandRM(inst,seg,offset,mem);
		FetchOperand8(inst,seg,offset,mem);
		break;
	case I486_OPCODE_IMUL_R_RM_IMM://0x69,
		offset+=FetchOperandRM(inst,seg,offset,mem);
		if(16==inst.operandSize)
		{
			FetchOperand16(inst,seg,offset,mem);
		}
		else
		{
			FetchOperand32(inst,seg,offset,mem);
		}
		break;
	case I486_OPCODE_IMUL_R_RM://       0xAF0F,
		FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_LEAVE://            0xC9,
		break;


	case I486_OPCODE_HLT://        0xF4,
		break;


	case I486_OPCODE_INC_DEC_R_M8:
		FetchOperandRM(inst,seg,offset,mem);
		break;
	case I486_OPCODE_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH:
		FetchOperandRM(inst,seg,offset,mem);
		break;
	case I486_OPCODE_INC_EAX://    0x40, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ECX://    0x41, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EDX://    0x42, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EBX://    0x43, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ESP://    0x44, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EBP://    0x45, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ESI://    0x46, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EDI://    0x47, // 16/32 depends on OPSIZE_OVERRIDE
		break;


	case I486_OPCODE_IRET://   0xCF,
		break;


	case I486_OPCODE_INT3://       0xCC,
		break;
	case I486_OPCODE_INT://        0xCD,
	case I486_OPCODE_INTO://       0xCE,
		FetchOperand8(inst,seg,offset,mem);
		break;


	case I486_OPCODE_JMP_REL8://         0xEB,   // cb
	case I486_OPCODE_JO_REL8:   // 0x70,
	case I486_OPCODE_JNO_REL8:  // 0x71,
	case I486_OPCODE_JB_REL8:   // 0x72,
	case I486_OPCODE_JAE_REL8:  // 0x73,
	case I486_OPCODE_JE_REL8:   // 0x74,
	case I486_OPCODE_JECXZ_REL8:// 0xE3,  // Depending on the operand size
	case I486_OPCODE_JNE_REL8:  // 0x75,
	case I486_OPCODE_JBE_REL8:  // 0x76,
	case I486_OPCODE_JA_REL8:   // 0x77,
	case I486_OPCODE_JS_REL8:   // 0x78,
	case I486_OPCODE_JNS_REL8:  // 0x79,
	case I486_OPCODE_JP_REL8:   // 0x7A,
	case I486_OPCODE_JNP_REL8:  // 0x7B,
	case I486_OPCODE_JL_REL8:   // 0x7C,
	case I486_OPCODE_JGE_REL8:  // 0x7D,
	case I486_OPCODE_JLE_REL8:  // 0x7E,
	case I486_OPCODE_JG_REL8:   // 0x7F,
		FetchOperand8(inst,seg,offset,mem);
		break;


	case I486_OPCODE_JA_REL://    0x870F,
	case I486_OPCODE_JAE_REL://   0x830F,
	case I486_OPCODE_JB_REL://    0x820F,
	case I486_OPCODE_JBE_REL://   0x860F,
	// case I486_OPCODE_JC_REL://    0x820F, Same as JB_REL
	case I486_OPCODE_JE_REL://    0x840F,
	// case I486_OPCODE_JZ_REL://    0x840F, Same as JZ_REL
	case I486_OPCODE_JG_REL://    0x8F0F,
	case I486_OPCODE_JGE_REL://   0x8D0F,
	case I486_OPCODE_JL_REL://    0x8C0F,
	case I486_OPCODE_JLE_REL://   0x8E0F,
	// case I486_OPCODE_JNA_REL://   0x860F, Same as JBE_REL
	// case I486_OPCODE_JNAE_REL://  0x820F, Same as JB_REL
	// case I486_OPCODE_JNB_REL://   0x830F, Same as JAE_REL
	// case I486_OPCODE_JNBE_REL://  0x870F, Same as JA_REL
	// case I486_OPCODE_JNC_REL://   0x830F, Same as JAE_REL
	case I486_OPCODE_JNE_REL://   0x850F,
	// case I486_OPCODE_JNG_REL://   0x8E0F, Same as JLE_REL
	// case I486_OPCODE_JNGE_REL://  0x8C0F, Same as JL_REL
	// case I486_OPCODE_JNL_REL://   0x8D0F, Same as JGE_REL
	// case I486_OPCODE_JNLE_REL://  0x8F0F, Same as JG_REL
	case I486_OPCODE_JNO_REL://   0x810F,
	case I486_OPCODE_JNP_REL://   0x8B0F,
	case I486_OPCODE_JNS_REL://   0x890F,
	// case I486_OPCODE_JNZ_REL://   0x850F, Same as JNE_REL
	case I486_OPCODE_JO_REL://    0x800F,
	case I486_OPCODE_JP_REL://    0x8A0F,
	// case I486_OPCODE_JPE_REL://   0x8A0F, Same as JP_REL
	// case I486_OPCODE_JPO_REL://   0x8B0F, Same as JNP_REL
	case I486_OPCODE_JS_REL://    0x880F,
		FetchOperand16or32(inst,seg,offset,mem);
		break;


	case I486_OPCODE_BINARYOP_RM8_FROM_I8:
	case I486_OPCODE_BINARYOP_RM8_FROM_I8_ALIAS:
		offset+=FetchOperandRM(inst,seg,offset,mem);
		FetchOperand8(inst,seg,offset,mem);
		break;
	case I486_OPCODE_BINARYOP_R_FROM_I:
		offset+=FetchOperandRM(inst,seg,offset,mem);
		FetchOperand16or32(inst,seg,offset,mem);
		break;
	case I486_OPCODE_BINARYOP_RM_FROM_SXI8:
		offset+=FetchOperandRM(inst,seg,offset,mem);
		FetchOperand8(inst,seg,offset,mem);
		break;


	case I486_OPCODE_LGDT_LIDT_SGDT_SIDT:
		FetchOperandRM(inst,seg,offset,mem);
		if(4==inst.GetREG() || 6==inst.GetREG())
		{
			inst.operandSize=16;
		}
		break;


	case I486_OPCODE_LAHF://=             0x9F,
		break;


	case I486_OPCODE_LEA://=              0x8D,
		FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_LDS://              0xC5,
	case I486_OPCODE_LSS://              0xB20F,
	case I486_OPCODE_LES://              0xC4,
	case I486_OPCODE_LFS://              0xB40F,
	case I486_OPCODE_LGS://              0xB50F,
		FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_LODSB://            0xAC,
	case I486_OPCODE_LODS://             0xAD,
		break;


	case I486_OPCODE_LOOP://             0xE2,
	case I486_OPCODE_LOOPE://            0xE1,
	case I486_OPCODE_LOOPNE://           0xE0,
		FetchOperand8(inst,seg,offset,mem);
		break;


	case I486_OPCODE_LSL://              0x030F,
		FetchOperandRM(inst,seg,offset,mem);
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
		FetchOperandRM(inst,seg,offset,mem);
		break;

	case I486_OPCODE_MOV_M_TO_AL: //      0xA0, // 16/32 depends on ADDRESSSIZE_OVERRIDE
	case I486_OPCODE_MOV_M_TO_EAX: //     0xA1, // 16/32 depends on ADDRESSSIZE_OVERRIDE
	case I486_OPCODE_MOV_M_FROM_AL: //    0xA2, // 16/32 depends on ADDRESSSIZE_OVERRIDE
	case I486_OPCODE_MOV_M_FROM_EAX: //   0xA3, // 16/32 depends on ADDRESSSIZE_OVERRIDE
		switch(inst.addressSize)
		{
		default:
		case 32:
			FetchOperand32(inst,seg,offset,mem);
			break;
		case 16:
			FetchOperand16(inst,seg,offset,mem);
			break;
		}
		break;

	case I486_OPCODE_MOV_I8_TO_AL: //     0xB0,
	case I486_OPCODE_MOV_I8_TO_CL: //     0xB1,
	case I486_OPCODE_MOV_I8_TO_DL: //     0xB2,
	case I486_OPCODE_MOV_I8_TO_BL: //     0xB3,
	case I486_OPCODE_MOV_I8_TO_AH: //     0xB4,
	case I486_OPCODE_MOV_I8_TO_CH: //     0xB5,
	case I486_OPCODE_MOV_I8_TO_DH: //     0xB6,
	case I486_OPCODE_MOV_I8_TO_BH: //     0xB7,
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
		FetchOperand16or32(inst,seg,offset,mem);
		break;
	case I486_OPCODE_MOV_I8_TO_RM8: //    0xC6,
		offset+=FetchOperandRM(inst,seg,offset,mem);
		FetchOperand8(inst,seg,offset,mem);
		break;
	case I486_OPCODE_MOV_I_TO_RM: //      0xC7, // 16/32 depends on OPSIZE_OVERRIDE
		offset+=FetchOperandRM(inst,seg,offset,mem);
		FetchOperand16or32(inst,seg,offset,mem);
		break;


	case I486_OPCODE_MOV_TO_CR://        0x220F,
	case I486_OPCODE_MOV_FROM_CR://      0x200F,
	case I486_OPCODE_MOV_FROM_DR://      0x210F,
	case I486_OPCODE_MOV_TO_DR://        0x230F,
	case I486_OPCODE_MOV_FROM_TR://      0x240F,
	case I486_OPCODE_MOV_TO_TR://        0x260F,
		inst.operandSize=32; // [1] pp.26-213 32bit operands are always used with these instructions, 
		                     //      regardless of the opreand-size attribute.
		FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_MOVSB://            0xA4,
	case I486_OPCODE_MOVS://             0xA5,
		break;


	case I486_OPCODE_MOVSX_R_RM8://=      0xBE0F,
	case I486_OPCODE_MOVSX_R32_RM16://=   0xBF0F,
	case I486_OPCODE_MOVZX_R_RM8://=      0xB60F,
	case I486_OPCODE_MOVZX_R32_RM16://=   0xB70F,
		FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_NOP://              0x90,
		break;


	case I486_OPCODE_OUT_I8_AL: //        0xE6,
	case I486_OPCODE_OUT_I8_A: //         0xE7,
		FetchOperand8(inst,seg,offset,mem);
		break;
	case I486_OPCODE_OUT_DX_AL: //        0xEE,
	case I486_OPCODE_OUT_DX_A: //         0xEF,
		break;


	case I486_OPCODE_OUTSB://            0x6E,
	case I486_OPCODE_OUTS://             0x6F,
		break;


	case I486_OPCODE_PUSHA://            0x60,
	case I486_OPCODE_PUSHF://            0x9C,
		break;


	case I486_OPCODE_PUSH_EAX://         0x50,
	case I486_OPCODE_PUSH_ECX://         0x51,
	case I486_OPCODE_PUSH_EDX://         0x52,
	case I486_OPCODE_PUSH_EBX://         0x53,
	case I486_OPCODE_PUSH_ESP://         0x54,
	case I486_OPCODE_PUSH_EBP://         0x55,
	case I486_OPCODE_PUSH_ESI://         0x56,
	case I486_OPCODE_PUSH_EDI://         0x57,
		break;
	case I486_OPCODE_PUSH_I8://          0x6A,
		FetchOperand8(inst,seg,offset,mem);
		break;
	case I486_OPCODE_PUSH_I://           0x68,
		FetchOperand16or32(inst,seg,offset,mem);
		break;
	case I486_OPCODE_PUSH_CS://          0x0E,
	case I486_OPCODE_PUSH_SS://          0x16,
	case I486_OPCODE_PUSH_DS://          0x1E,
	case I486_OPCODE_PUSH_ES://          0x06,
	case I486_OPCODE_PUSH_FS://          0xA00F,
	case I486_OPCODE_PUSH_GS://          0xA80F,
		break;


	case I486_OPCODE_POP_M://            0x8F,
		FetchOperandRM(inst,seg,offset,mem);
		break;
	case I486_OPCODE_POP_EAX://          0x58,
	case I486_OPCODE_POP_ECX://          0x59,
	case I486_OPCODE_POP_EDX://          0x5A,
	case I486_OPCODE_POP_EBX://          0x5B,
	case I486_OPCODE_POP_ESP://          0x5C,
	case I486_OPCODE_POP_EBP://          0x5D,
	case I486_OPCODE_POP_ESI://          0x5E,
	case I486_OPCODE_POP_EDI://          0x5F,
	case I486_OPCODE_POP_SS://           0x17,
	case I486_OPCODE_POP_DS://           0x1F,
	case I486_OPCODE_POP_ES://           0x07,
	case I486_OPCODE_POP_FS://           0xA10F,
	case I486_OPCODE_POP_GS://           0xA90F,

	case I486_OPCODE_POPA://             0x61,
	case I486_OPCODE_POPF://             0x9D,
		break;


	case I486_OPCODE_RET://              0xC3,
	case I486_OPCODE_RETF://             0xCB,
		break;
	case I486_OPCODE_RET_I16://          0xC2,
	case I486_OPCODE_RETF_I16://         0xCA,
		FetchOperand16(inst,seg,offset,mem);
		break;


	case I486_OPCODE_SAHF://=             0x9E,
		break;


	case I486_OPCODE_SHLD_RM_I8://       0xA40F,
	case I486_OPCODE_SHRD_RM_I8://       0xAC0F,
		offset+=FetchOperandRM(inst,seg,offset,mem);
		FetchOperand8(inst,seg,offset,mem);
		break;
	case I486_OPCODE_SHLD_RM_CL://       0xA50F,
	case I486_OPCODE_SHRD_RM_CL://       0xAD0F,
		FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_SCASB://            0xAE,
	case I486_OPCODE_SCAS://             0xAF,
		break;


	case I486_OPCODE_SETA://             0x970F,
	case I486_OPCODE_SETAE://            0x930F,
	case I486_OPCODE_SETB://             0x920F,
	case I486_OPCODE_SETBE://            0x960F,
	// I486_OPCODE_SETC://             0x920F,
	case I486_OPCODE_SETE://             0x940F,
	case I486_OPCODE_SETG://             0x9F0F,
	case I486_OPCODE_SETGE://            0x9D0F,
	case I486_OPCODE_SETL://             0x9C0F,
	case I486_OPCODE_SETLE://            0x9E0F,
	//I486_OPCODE_SETNA://            0x960F,
	//I486_OPCODE_SETNAE://           0x920F,
	//I486_OPCODE_SETNB://            0x930F,
	//I486_OPCODE_SETNBE://           0x970F,
	//I486_OPCODE_SETNC://            0x930F,
	case I486_OPCODE_SETNE://            0x950F,
	//I486_OPCODE_SETNG://            0x9E0F,
	//I486_OPCODE_SETNGE://           0x9C0F,
	//I486_OPCODE_SETNL://            0x9D0F,
	//I486_OPCODE_SETNLE://           0x9F0F,
	case I486_OPCODE_SETNO://            0x910F,
	case I486_OPCODE_SETNP://            0x9B0F,
	case I486_OPCODE_SETNS://            0x990F,
	// I486_OPCODE_SETNZ://            0x950F,
	case I486_OPCODE_SETO://             0x900F,
	case I486_OPCODE_SETP://             0x9A0F,
	//I486_OPCODE_SETPE://            0x9A0F,
	//I486_OPCODE_SETPO://            0x9B0F,
	case I486_OPCODE_SETS://             0x980F,
	// I486_OPCODE_SETZ://             0x940F,
		FetchOperandRM(inst,seg,offset,mem);
		inst.operandSize=8;
		break;


	case I486_OPCODE_SLDT_STR_LLDT_LTR_VERR_VERW://             0x000F,
		FetchOperandRM(inst,seg,offset,mem);
		inst.operandSize=16;
		break;


	case I486_OPCODE_STC://              0xF9,
	case I486_OPCODE_STD://              0xFD,
	case I486_OPCODE_STI://              0xFB,
		break;


	case I486_OPCODE_STOSB://            0xAA,
	case I486_OPCODE_STOS://             0xAB,
		break;


	case  I486_OPCODE_ADC_AL_FROM_I8://  0x14,
	case  I486_OPCODE_ADD_AL_FROM_I8://  0x04,
	case  I486_OPCODE_AND_AL_FROM_I8://  0x24,
	case  I486_OPCODE_CMP_AL_FROM_I8://  0x3C,
	case   I486_OPCODE_OR_AL_FROM_I8://  0x0C,
	case  I486_OPCODE_SBB_AL_FROM_I8://  0x1C,
	case  I486_OPCODE_SUB_AL_FROM_I8://  0x2C,
	case I486_OPCODE_TEST_AL_FROM_I8://  0xA8,
	case  I486_OPCODE_XOR_AL_FROM_I8:
		FetchOperand8(inst,seg,offset,mem);
		break;
	case  I486_OPCODE_ADC_A_FROM_I://    0x15,
	case  I486_OPCODE_ADD_A_FROM_I://    0x05,
	case  I486_OPCODE_AND_A_FROM_I://    0x25,
	case  I486_OPCODE_CMP_A_FROM_I://    0x3D,
	case   I486_OPCODE_OR_A_FROM_I://    0x0D,
	case  I486_OPCODE_SBB_A_FROM_I://    0x1D,
	case  I486_OPCODE_SUB_A_FROM_I://    0x2D,
	case I486_OPCODE_TEST_A_FROM_I://    0xA9,
	case  I486_OPCODE_XOR_A_FROM_I:
		FetchOperand16or32(inst,seg,offset,mem);
		break;
	case  I486_OPCODE_ADC_RM8_FROM_R8:// 0x10,
	case  I486_OPCODE_ADD_RM8_FROM_R8:// 0x00,
	case  I486_OPCODE_AND_RM8_FROM_R8:// 0x20,
	case  I486_OPCODE_CMP_RM8_FROM_R8:// 0x38,
	case   I486_OPCODE_OR_RM8_FROM_R8:// 0x08,
	case  I486_OPCODE_SBB_RM8_FROM_R8:// 0x18,
	case  I486_OPCODE_SUB_RM8_FROM_R8:// 0x28,
	case I486_OPCODE_TEST_RM8_FROM_R8:// 0x84,
	case  I486_OPCODE_XOR_RM8_FROM_R8:

	case  I486_OPCODE_ADC_RM_FROM_R://   0x11,
	case  I486_OPCODE_ADD_RM_FROM_R://   0x01,
	case  I486_OPCODE_AND_RM_FROM_R://   0x21,
	case  I486_OPCODE_CMP_RM_FROM_R://   0x39,
	case   I486_OPCODE_OR_RM_FROM_R://   0x09,
	case  I486_OPCODE_SBB_RM_FROM_R://   0x19,
	case  I486_OPCODE_SUB_RM_FROM_R://   0x29,
	case I486_OPCODE_TEST_RM_FROM_R://   0x85,
	case  I486_OPCODE_XOR_RM_FROM_R:

	case I486_OPCODE_ADC_R8_FROM_RM8:// 0x12,
	case I486_OPCODE_ADD_R8_FROM_RM8:// 0x02,
	case I486_OPCODE_AND_R8_FROM_RM8:// 0x22,
	case I486_OPCODE_CMP_R8_FROM_RM8:// 0x3A,
	case  I486_OPCODE_OR_R8_FROM_RM8:// 0x0A,
	case I486_OPCODE_SBB_R8_FROM_RM8:// 0x1A,
	case I486_OPCODE_SUB_R8_FROM_RM8:// 0x2A,
	case I486_OPCODE_XOR_R8_FROM_RM8:

	case I486_OPCODE_ADC_R_FROM_RM://   0x13,
	case I486_OPCODE_ADD_R_FROM_RM://   0x03,
	case I486_OPCODE_AND_R_FROM_RM://   0x23,
	case I486_OPCODE_CMP_R_FROM_RM://   0x3B,
	case  I486_OPCODE_OR_R_FROM_RM://   0x0B,
	case I486_OPCODE_SBB_R_FROM_RM://   0x1B,
	case I486_OPCODE_SUB_R_FROM_RM://   0x2B,
	case I486_OPCODE_XOR_R_FROM_RM:
		FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_XCHG_EAX_ECX://     0x91,
	case I486_OPCODE_XCHG_EAX_EDX://     0x92,
	case I486_OPCODE_XCHG_EAX_EBX://     0x93,
	case I486_OPCODE_XCHG_EAX_ESP://     0x94,
	case I486_OPCODE_XCHG_EAX_EBP://     0x95,
	case I486_OPCODE_XCHG_EAX_ESI://     0x96,
	case I486_OPCODE_XCHG_EAX_EDI://     0x97,
		// No operand
		break;
	case I486_OPCODE_XCHG_RM8_R8://      0x86,
	case I486_OPCODE_XCHG_RM_R://        0x87,
		FetchOperandRM(inst,seg,offset,mem);
		break;


	case I486_OPCODE_XLAT://             0xD7,
		break;


	default:
		// Undefined operand, or probably not implemented yet.
		break;
	}
}

void i486DX::Instruction::DecodeOperand(int addressSize,int operandSize,Operand &op1,Operand &op2) const
{
	op1.Clear();
	op2.Clear();

	switch(opCode)
	{
	case I486_OPCODE_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8://=0xC0,// ::ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		op1.Decode(addressSize,8,operand);
		op2.MakeImm8(*this);
		break;
	case I486_OPCODE_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8:// =0xC1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		op1.Decode(addressSize,operandSize,operand);
		op2.MakeImm8(*this);
		break;
	case I486_OPCODE_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1://=0xD0, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL://0xD2,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		op1.Decode(addressSize,8,operand);
		break;
	case I486_OPCODE_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1://=0xD1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL://0xD3, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		op1.Decode(addressSize,operandSize,operand);
		break;


	case I486_OPCODE_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF6
	case I486_OPCODE_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF7,
		op1.Decode(addressSize,operandSize,operand);
		if(0==GetREG()) // TEST RM,I
		{
			op2.MakeImm8or16or32(*this,operandSize);
		}
		break;


	case I486_OPCODE_ARPL://       0x63,
		operandSize=16;
		op1.Decode(addressSize,operandSize,operand);
		op2.DecodeMODR_MForRegister(operandSize,operand[0]);
		break;


	case I486_OPCODE_BTS_RM_R://   0xAB0F,
		op1.Decode(addressSize,operandSize,operand);
		op2.DecodeMODR_MForRegister(operandSize,operand[0]);
		break;


	case I486_OPCODE_CALL_REL://   0xE8,
	case I486_OPCODE_JMP_REL://          0xE9,   // cw or cd
		op1.MakeImm8or16or32(*this,operandSize);
		break;
	case I486_OPCODE_CALL_FAR://   0x9A,
	case I486_OPCODE_JMP_FAR:
		op1.DecodeFarAddr(addressSize,operandSize,operand);
		break;


	case I486_OPCODE_CBW_CWDE://        0x98,
	case I486_OPCODE_CWD_CDQ://         0x99,
	case I486_OPCODE_CLC:
	case I486_OPCODE_CLD:
	case I486_OPCODE_CLI:
	case I486_OPCODE_CMC://        0xF5,
		break;


	case I486_OPCODE_CMPSB://           0xA6,
	case I486_OPCODE_CMPS://            0xA7,
		break;


	case I486_OPCODE_ENTER://      0xC8,
		break;


	case I486_OPCODE_FWAIT://      0x9B,
		break;
	case I486_OPCODE_FPU_D9_FNSTCW_M16_FNSTENV_F2XM1_FXAM_FXCH_FXTRACT_FYL2X_FYL2XP1_FABS_:// 0xD9,
		if(0xF0<=operand[0] && operand[0]<=0xFF)
		{
			// Do nothing
		}
		else
		{
			op1.Decode(addressSize,operandSize,operand);
		}
		break;
	case I486_OPCODE_FPU_DB_FNINIT_FRSTOR://     0xDB, 
		break;
	case I486_OPCODE_FPU_DF_FNSTSW_AX://  0xDF,
		break;


	case I486_OPCODE_DEC_EAX:
	case I486_OPCODE_DEC_ECX:
	case I486_OPCODE_DEC_EDX:
	case I486_OPCODE_DEC_EBX:
	case I486_OPCODE_DEC_ESP:
	case I486_OPCODE_DEC_EBP:
	case I486_OPCODE_DEC_ESI:
	case I486_OPCODE_DEC_EDI:
		op1.MakeByRegisterNumber(operandSize,opCode&7);
		break;


	case I486_OPCODE_IN_AL_I8://=        0xE4,
	case I486_OPCODE_IN_A_I8://=         0xE5,
		op1.MakeImm8(*this);
		break;
	case I486_OPCODE_IN_AL_DX://=        0xEC,
	case I486_OPCODE_IN_A_DX://=         0xED,
		break;


	case I486_OPCODE_IMUL_R_RM_I8://0x6B,
	case I486_OPCODE_IMUL_R_RM_IMM://0x69,
	case I486_OPCODE_IMUL_R_RM://       0xAF0F,
		op1.DecodeMODR_MForRegister(operandSize,operand[0]);
		op2.Decode(addressSize,operandSize,operand);
		break;


	case I486_OPCODE_LEAVE://            0xC9,
		break;


	case I486_OPCODE_HLT://        0xF4,
		break;


	case I486_OPCODE_INC_DEC_R_M8:
		op1.Decode(addressSize,8,operand);
		break;
	case I486_OPCODE_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH:
		op1.Decode(addressSize,operandSize,operand);
		break;
	case I486_OPCODE_INC_EAX://    0x40, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ECX://    0x41, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EDX://    0x42, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EBX://    0x43, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ESP://    0x44, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EBP://    0x45, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ESI://    0x46, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EDI://    0x47, // 16/32 depends on OPSIZE_OVERRIDE
		op1.MakeByRegisterNumber(operandSize,opCode&7);
		break;


	case I486_OPCODE_INT3://       0xCC,
		break;
	case I486_OPCODE_INT://        0xCD,
	case I486_OPCODE_INTO://       0xCE,
		op1.MakeImm8(*this);
		break;


	case I486_OPCODE_IRET://   0xCF,
		break;


	case I486_OPCODE_JMP_REL8://         0xEB,   // cb
	case I486_OPCODE_JO_REL8:   // 0x70,
	case I486_OPCODE_JNO_REL8:  // 0x71,
	case I486_OPCODE_JB_REL8:   // 0x72,
	case I486_OPCODE_JAE_REL8:  // 0x73,
	case I486_OPCODE_JE_REL8:   // 0x74,
	case I486_OPCODE_JECXZ_REL8:// 0xE3,  // Depending on the operand size
	case I486_OPCODE_JNE_REL8:  // 0x75,
	case I486_OPCODE_JBE_REL8:  // 0x76,
	case I486_OPCODE_JA_REL8:   // 0x77,
	case I486_OPCODE_JS_REL8:   // 0x78,
	case I486_OPCODE_JNS_REL8:  // 0x79,
	case I486_OPCODE_JP_REL8:   // 0x7A,
	case I486_OPCODE_JNP_REL8:  // 0x7B,
	case I486_OPCODE_JL_REL8:   // 0x7C,
	case I486_OPCODE_JGE_REL8:  // 0x7D,
	case I486_OPCODE_JLE_REL8:  // 0x7E,
	case I486_OPCODE_JG_REL8:   // 0x7F,
		break;


	case I486_OPCODE_LGDT_LIDT_SGDT_SIDT:
		op1.Decode(addressSize,operandSize,operand);
		break;


	case I486_OPCODE_BINARYOP_RM8_FROM_I8: //  0x80, // AND(REG=4), OR(REG=1), or XOR(REG=6) depends on the REG field of MODR/M
	case I486_OPCODE_BINARYOP_RM8_FROM_I8_ALIAS:
		op1.Decode(addressSize,8,operand);
		op2.MakeImm8(*this);
		break;
	case I486_OPCODE_BINARYOP_R_FROM_I://     0x81,
		op1.Decode(addressSize,operandSize,operand);
		op2.MakeImm8or16or32(*this,operandSize);
		break;
	case I486_OPCODE_BINARYOP_RM_FROM_SXI8:// 0x83,
		op1.Decode(addressSize,operandSize,operand);
		op2.MakeImm8(*this);
		if(16==operandSize)
		{
			op2.SignExtendImm(OPER_IMM16);
		}
		else
		{
			op2.SignExtendImm(OPER_IMM32);
		}
		break;


	case I486_OPCODE_LAHF://=             0x9F,
		break;


	case I486_OPCODE_LEA://=              0x8D,
		op1.DecodeMODR_MForRegister(operandSize,operand[0]);
		op2.Decode(addressSize,operandSize,operand);
		break;


	case I486_OPCODE_LDS://              0xC5,
	case I486_OPCODE_LSS://              0xB20F,
	case I486_OPCODE_LES://              0xC4,
	case I486_OPCODE_LFS://              0xB40F,
	case I486_OPCODE_LGS://              0xB50F,
		op1.DecodeMODR_MForRegister(operandSize,operand[0]);
		op2.Decode(addressSize,operandSize,operand);
		break;


	case I486_OPCODE_LODSB://            0xAC,
	case I486_OPCODE_LODS://             0xAD,
		break;


	case I486_OPCODE_LOOP://             0xE2,
	case I486_OPCODE_LOOPE://            0xE1,
	case I486_OPCODE_LOOPNE://           0xE0,
		break;


	case I486_OPCODE_LSL://              0x030F,
		op1.DecodeMODR_MForRegister(operandSize,operand[0]);
		op2.Decode(addressSize,operandSize,operand);
		break;


	case I486_OPCODE_MOV_FROM_R8: //      0x88,
		op1.Decode(addressSize,8,operand);
		op2.DecodeMODR_MForRegister(8,operand[0]);
		break;
	case I486_OPCODE_MOV_FROM_R: //       0x89, // 16/32 depends on OPSIZE_OVERRIDE
		op1.Decode(addressSize,operandSize,operand);
		op2.DecodeMODR_MForRegister(operandSize,operand[0]);
		break;
	case I486_OPCODE_MOV_TO_R8: //        0x8A,
		op2.Decode(addressSize,8,operand);
		op1.DecodeMODR_MForRegister(8,operand[0]);
		break;
	case I486_OPCODE_MOV_TO_R: //         0x8B, // 16/32 depends on OPSIZE_OVERRIDE
		op2.Decode(addressSize,operandSize,operand);
		op1.DecodeMODR_MForRegister(operandSize,operand[0]);
		break;
	case I486_OPCODE_MOV_FROM_SEG: //     0x8C,
		op1.Decode(addressSize,operandSize,operand);
		op2.DecodeMODR_MForSegmentRegister(operand[0]);
		break;
	case I486_OPCODE_MOV_TO_SEG: //       0x8E,
		op2.Decode(addressSize,operandSize,operand);
		op1.DecodeMODR_MForSegmentRegister(operand[0]);
		break;
	case I486_OPCODE_MOV_M_TO_AL: //      0xA0,
		op2.MakeSimpleAddressOffset(*this);
		op1.MakeByRegisterNumber(8,REG_AL-REG_8BIT_REG_BASE);
		break;
	case I486_OPCODE_MOV_M_TO_EAX: //     0xA1, // 16/32 depends on OPSIZE_OVERRIDE
		op2.MakeSimpleAddressOffset(*this);
		op1.MakeByRegisterNumber(operandSize,REG_AL-REG_8BIT_REG_BASE);
		break;
	case I486_OPCODE_MOV_M_FROM_AL: //    0xA2,
		op1.MakeSimpleAddressOffset(*this);
		op2.MakeByRegisterNumber(8,REG_AL-REG_8BIT_REG_BASE);
		break;
	case I486_OPCODE_MOV_M_FROM_EAX: //   0xA3, // 16/32 depends on OPSIZE_OVERRIDE
		op1.MakeSimpleAddressOffset(*this);
		op2.MakeByRegisterNumber(operandSize,REG_AL-REG_8BIT_REG_BASE);
		break;
	case I486_OPCODE_MOV_I8_TO_AL: //     0xB0,
	case I486_OPCODE_MOV_I8_TO_CL: //     0xB1,
	case I486_OPCODE_MOV_I8_TO_DL: //     0xB2,
	case I486_OPCODE_MOV_I8_TO_BL: //     0xB3,
	case I486_OPCODE_MOV_I8_TO_AH: //     0xB4,
	case I486_OPCODE_MOV_I8_TO_CH: //     0xB5,
	case I486_OPCODE_MOV_I8_TO_DH: //     0xB6,
	case I486_OPCODE_MOV_I8_TO_BH: //     0xB7,
		op1.MakeByRegisterNumber(8,opCode&7);
		op2.MakeImm8(*this);
		break;
	case I486_OPCODE_MOV_I_TO_EAX: //     0xB8, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ECX: //     0xB9, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EDX: //     0xBA, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EBX: //     0xBB, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ESP: //     0xBC, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EBP: //     0xBD, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ESI: //     0xBE, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EDI: //     0xBF, // 16/32 depends on OPSIZE_OVERRIDE
		op1.MakeByRegisterNumber(operandSize,opCode&7);
		if(16==operandSize)
		{
			op2.MakeImm16(*this);
		}
		else
		{
			op2.MakeImm32(*this);
		}
		break;
	case I486_OPCODE_MOV_I8_TO_RM8: //    0xC6,
		op1.Decode(addressSize,8,operand);
		op2.MakeImm8(*this);
		break;
	case I486_OPCODE_MOV_I_TO_RM: //      0xC7, // 16/32 depends on OPSIZE_OVERRIDE
		op1.Decode(addressSize,operandSize,operand);
		op2.MakeImm8or16or32(*this,operandSize);
		break;


	case I486_OPCODE_MOV_TO_CR://        0x220F,  Op1=CR, OP2=R32
		op1.DecodeMODR_MForCRRegister(operand[0]);
		op2.Decode(addressSize,32,operand);
		break;
	case I486_OPCODE_MOV_FROM_CR://      0x200F,  Op1=R32, Op2=CR
		op1.Decode(addressSize,32,operand);
		op2.DecodeMODR_MForCRRegister(operand[0]);
		break;
	case I486_OPCODE_MOV_FROM_DR://      0x210F,  Op1=R32, Op2=DR
		op1.Decode(addressSize,32,operand);
		op2.DecodeMODR_MForDRRegister(operand[0]);
		break;
	case I486_OPCODE_MOV_TO_DR://        0x230F,  Op1=DR, Op2=R32
		op1.DecodeMODR_MForDRRegister(operand[0]);
		op2.Decode(addressSize,32,operand);
		break;
	case I486_OPCODE_MOV_FROM_TR://      0x240F,  Op1=R32, Op2=TR  TR is a test register!  Not Task Register!  How confusing!
		op1.Decode(addressSize,32,operand);
		op2.DecodeMODR_MForTestRegister(operand[0]);
		break;
	case I486_OPCODE_MOV_TO_TR://        0x260F,  Op1=TR, Op2=R32
		op1.DecodeMODR_MForTestRegister(operand[0]);
		op2.Decode(addressSize,32,operand);
		break;


	case I486_OPCODE_MOVSB://            0xA4,
	case I486_OPCODE_MOVS://             0xA5,
		break;


	case I486_OPCODE_MOVSX_R_RM8://=      0xBE0F,
	case I486_OPCODE_MOVZX_R_RM8://=      0xB60F,
		op1.DecodeMODR_MForRegister(operandSize,operand[0]);
		op2.Decode(addressSize,8,operand);
		break;
	case I486_OPCODE_MOVSX_R32_RM16://=   0xBF0F,
	case I486_OPCODE_MOVZX_R32_RM16://=   0xB70F,
		op1.DecodeMODR_MForRegister(32,operand[0]);
		op2.Decode(addressSize,16,operand);
		break;


	case I486_OPCODE_NOP://              0x90,
		break;


	case I486_OPCODE_OUT_I8_AL: //        0xE6,
	case I486_OPCODE_OUT_I8_A: //         0xE7,
		op1.MakeImm8(*this);
		break;
	case I486_OPCODE_OUT_DX_AL: //        0xEE,
	case I486_OPCODE_OUT_DX_A: //         0xEF,
		break;


	case I486_OPCODE_OUTSB://            0x6E,
	case I486_OPCODE_OUTS://             0x6F,
		break;


	case I486_OPCODE_PUSHA://            0x60,
	case I486_OPCODE_PUSHF://            0x9C,
		break;


	case I486_OPCODE_PUSH_EAX://         0x50,
	case I486_OPCODE_PUSH_ECX://         0x51,
	case I486_OPCODE_PUSH_EDX://         0x52,
	case I486_OPCODE_PUSH_EBX://         0x53,
	case I486_OPCODE_PUSH_ESP://         0x54,
	case I486_OPCODE_PUSH_EBP://         0x55,
	case I486_OPCODE_PUSH_ESI://         0x56,
	case I486_OPCODE_PUSH_EDI://         0x57,
		break;
	case I486_OPCODE_PUSH_I8://          0x6A,
		op1.MakeImm8(*this);
		break;
	case I486_OPCODE_PUSH_I://           0x68,
		op1.MakeImm8or16or32(*this,operandSize);
		break;
	case I486_OPCODE_PUSH_CS://          0x0E,
	case I486_OPCODE_PUSH_SS://          0x16,
	case I486_OPCODE_PUSH_DS://          0x1E,
	case I486_OPCODE_PUSH_ES://          0x06,
	case I486_OPCODE_PUSH_FS://          0xA00F,
	case I486_OPCODE_PUSH_GS://          0xA80F,
		break;


	case I486_OPCODE_POP_M://            0x8F,
		op1.Decode(addressSize,operandSize,operand);
		break;
	case I486_OPCODE_POP_EAX://          0x58,
	case I486_OPCODE_POP_ECX://          0x59,
	case I486_OPCODE_POP_EDX://          0x5A,
	case I486_OPCODE_POP_EBX://          0x5B,
	case I486_OPCODE_POP_ESP://          0x5C,
	case I486_OPCODE_POP_EBP://          0x5D,
	case I486_OPCODE_POP_ESI://          0x5E,
	case I486_OPCODE_POP_EDI://          0x5F,
	case I486_OPCODE_POP_SS://           0x17,
	case I486_OPCODE_POP_DS://           0x1F,
	case I486_OPCODE_POP_ES://           0x07,
	case I486_OPCODE_POP_FS://           0xA10F,
	case I486_OPCODE_POP_GS://           0xA90F,

	case I486_OPCODE_POPA://             0x61,
	case I486_OPCODE_POPF://             0x9D,
		break;


	case I486_OPCODE_RET://              0xC3,
	case I486_OPCODE_RETF://             0xCB,
		break;
	case I486_OPCODE_RET_I16://          0xC2,
	case I486_OPCODE_RETF_I16://         0xCA,
		op1.MakeImm16(*this);
		break;


	case I486_OPCODE_SAHF://=             0x9E,
		break;


	case I486_OPCODE_SHLD_RM_I8://       0xA40F,
	case I486_OPCODE_SHLD_RM_CL://       0xA50F,
	case I486_OPCODE_SHRD_RM_I8://       0xAC0F,
	case I486_OPCODE_SHRD_RM_CL://       0xAD0F,
		op1.Decode(addressSize,operandSize,operand);
		op2.DecodeMODR_MForRegister(operandSize,operand[0]);
		break;


	case I486_OPCODE_SCASB://            0xAE,
	case I486_OPCODE_SCAS://             0xAF,
		break;


	case I486_OPCODE_SETA://             0x970F,
	case I486_OPCODE_SETAE://            0x930F,
	case I486_OPCODE_SETB://             0x920F,
	case I486_OPCODE_SETBE://            0x960F,
	// I486_OPCODE_SETC://             0x920F,
	case I486_OPCODE_SETE://             0x940F,
	case I486_OPCODE_SETG://             0x9F0F,
	case I486_OPCODE_SETGE://            0x9D0F,
	case I486_OPCODE_SETL://             0x9C0F,
	case I486_OPCODE_SETLE://            0x9E0F,
	//I486_OPCODE_SETNA://            0x960F,
	//I486_OPCODE_SETNAE://           0x920F,
	//I486_OPCODE_SETNB://            0x930F,
	//I486_OPCODE_SETNBE://           0x970F,
	//I486_OPCODE_SETNC://            0x930F,
	case I486_OPCODE_SETNE://            0x950F,
	//I486_OPCODE_SETNG://            0x9E0F,
	//I486_OPCODE_SETNGE://           0x9C0F,
	//I486_OPCODE_SETNL://            0x9D0F,
	//I486_OPCODE_SETNLE://           0x9F0F,
	case I486_OPCODE_SETNO://            0x910F,
	case I486_OPCODE_SETNP://            0x9B0F,
	case I486_OPCODE_SETNS://            0x990F,
	// I486_OPCODE_SETNZ://            0x950F,
	case I486_OPCODE_SETO://             0x900F,
	case I486_OPCODE_SETP://             0x9A0F,
	//I486_OPCODE_SETPE://            0x9A0F,
	//I486_OPCODE_SETPO://            0x9B0F,
	case I486_OPCODE_SETS://             0x980F,
	// I486_OPCODE_SETZ://             0x940F,
		op1.Decode(addressSize,operandSize,operand);
		break;


	case I486_OPCODE_SLDT_STR_LLDT_LTR_VERR_VERW://             0x000F,
		op1.Decode(addressSize,operandSize,operand);
		break;


	case I486_OPCODE_STC://              0xF9,
	case I486_OPCODE_STD://              0xFD,
	case I486_OPCODE_STI://              0xFB,
		break;


	case I486_OPCODE_STOSB://            0xAA,
	case I486_OPCODE_STOS://             0xAB,
		break;


	case  I486_OPCODE_ADC_AL_FROM_I8://  0x14,
	case  I486_OPCODE_ADD_AL_FROM_I8://  0x04,
	case  I486_OPCODE_AND_AL_FROM_I8://  0x24,
	case  I486_OPCODE_CMP_AL_FROM_I8://  0x3C,
	case   I486_OPCODE_OR_AL_FROM_I8://  0x0C,
	case  I486_OPCODE_SBB_AL_FROM_I8://  0x1C,
	case  I486_OPCODE_SUB_AL_FROM_I8://  0x2C,
	case  I486_OPCODE_XOR_AL_FROM_I8:
	case I486_OPCODE_TEST_AL_FROM_I8://  0xA8,
		op1.MakeImm8(*this);
		break;
	case  I486_OPCODE_ADC_A_FROM_I://    0x15,
	case  I486_OPCODE_ADD_A_FROM_I://    0x05,
	case  I486_OPCODE_AND_A_FROM_I://    0x25,
	case  I486_OPCODE_CMP_A_FROM_I://    0x3D,
	case   I486_OPCODE_OR_A_FROM_I://    0x0D,
	case  I486_OPCODE_SBB_A_FROM_I://    0x1D,
	case  I486_OPCODE_SUB_A_FROM_I://    0x2D,
	case I486_OPCODE_TEST_A_FROM_I://    0xA9,
	case  I486_OPCODE_XOR_A_FROM_I:
		op1.MakeImm8or16or32(*this,operandSize);
		break;
	case  I486_OPCODE_ADC_RM8_FROM_R8:// 0x10,
	case  I486_OPCODE_ADD_RM8_FROM_R8:// 0x00,
	case  I486_OPCODE_AND_RM8_FROM_R8:// 0x20,
	case  I486_OPCODE_CMP_RM8_FROM_R8:// 0x38,
	case   I486_OPCODE_OR_RM8_FROM_R8:// 0x08,
	case  I486_OPCODE_SBB_RM8_FROM_R8:// 0x18,
	case  I486_OPCODE_SUB_RM8_FROM_R8:// 0x28,
	case I486_OPCODE_TEST_RM8_FROM_R8:// 0x84,
	case  I486_OPCODE_XOR_RM8_FROM_R8:
		op2.DecodeMODR_MForRegister(8,operand[0]);
		op1.Decode(addressSize,8,operand);
		break;
	case  I486_OPCODE_ADC_RM_FROM_R://   0x11,
	case  I486_OPCODE_ADD_RM_FROM_R://   0x01,
	case  I486_OPCODE_AND_RM_FROM_R://   0x21,
	case  I486_OPCODE_CMP_RM_FROM_R://   0x39,
	case   I486_OPCODE_OR_RM_FROM_R://   0x09,
	case  I486_OPCODE_SBB_RM_FROM_R://   0x19,
	case  I486_OPCODE_SUB_RM_FROM_R://   0x29,
	case I486_OPCODE_TEST_RM_FROM_R://   0x85,
	case I486_OPCODE_XOR_RM_FROM_R:
		op2.DecodeMODR_MForRegister(operandSize,operand[0]);
		op1.Decode(addressSize,operandSize,operand);
		break;
	case I486_OPCODE_ADC_R8_FROM_RM8:// 0x12,
	case I486_OPCODE_ADD_R8_FROM_RM8:// 0x02,
	case I486_OPCODE_AND_R8_FROM_RM8:// 0x22,
	case I486_OPCODE_CMP_R8_FROM_RM8:// 0x3A,
	case  I486_OPCODE_OR_R8_FROM_RM8:// 0x0A,
	case I486_OPCODE_SBB_R8_FROM_RM8:// 0x1A,
	case I486_OPCODE_SUB_R8_FROM_RM8:// 0x2A,
	case I486_OPCODE_XOR_R8_FROM_RM8:
		op1.DecodeMODR_MForRegister(8,operand[0]);
		op2.Decode(addressSize,8,operand);
		break;
	case I486_OPCODE_ADC_R_FROM_RM://   0x13,
	case I486_OPCODE_ADD_R_FROM_RM://   0x03,
	case I486_OPCODE_AND_R_FROM_RM://   0x23,
	case I486_OPCODE_CMP_R_FROM_RM://   0x3B,
	case  I486_OPCODE_OR_R_FROM_RM://   0x0B,
	case I486_OPCODE_SBB_R_FROM_RM://   0x1B,
	case I486_OPCODE_SUB_R_FROM_RM://   0x2B,
	case I486_OPCODE_XOR_R_FROM_RM:
		op1.DecodeMODR_MForRegister(operandSize,operand[0]);
		op2.Decode(addressSize,operandSize,operand);
		break;


	case I486_OPCODE_XCHG_EAX_ECX://     0x91,
	case I486_OPCODE_XCHG_EAX_EDX://     0x92,
	case I486_OPCODE_XCHG_EAX_EBX://     0x93,
	case I486_OPCODE_XCHG_EAX_ESP://     0x94,
	case I486_OPCODE_XCHG_EAX_EBP://     0x95,
	case I486_OPCODE_XCHG_EAX_ESI://     0x96,
	case I486_OPCODE_XCHG_EAX_EDI://     0x97,
		// No operand
		break;
	case I486_OPCODE_XCHG_RM8_R8://      0x86,
		op1.Decode(addressSize,8,operand);
		op2.DecodeMODR_MForRegister(8,operand[0]);
		break;
	case I486_OPCODE_XCHG_RM_R://        0x87,
		op1.Decode(addressSize,operandSize,operand);
		op2.DecodeMODR_MForRegister(operandSize,operand[0]);
		break;
	case I486_OPCODE_XLAT://             0xD7,
		break;
	}
}

std::string i486DX::Instruction::Disassemble(SegmentRegister cs,unsigned int eip,const i486SymbolTable &symTable) const
{
	std::string disasm;
	Operand op1,op2;
	std::string op1SizeQual,op2SizeQual;
	std::string op1SegQual,op2SegQual;

	DecodeOperand(addressSize,operandSize,op1,op2);

	switch(opCode)
	{
	case I486_OPCODE_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8://=0xC0,// ::ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8:// =0xC1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1://=0xD0, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1://=0xD1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL://0xD2,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL://0xD3, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		switch(GetREG())
		{
		case 0:
			disasm="ROL";
			break;
		case 1:
			disasm="ROR";
			break;
		case 2:
			disasm="RCL";
			break;
		case 3:
			disasm="RCR";
			break;
		case 4:
			disasm="SHL";
			break;
		case 5:
			disasm="SHR";
			break;
		case 6:
			disasm=cpputil::Ubtox(opCode)+"?";
			break;
		case 7:
			disasm="SAR";
			break;
		}
		switch(opCode)
		{
		case I486_OPCODE_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8://=0xC0,// ::ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
			disasm=DisassembleTypicalRM8_I8(disasm,op1,op2);
			break;
		case I486_OPCODE_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8:// =0xC1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
			disasm=DisassembleTypicalRM_I8(disasm,op1,op2);
			break;
		case I486_OPCODE_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1://=0xD0, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		case I486_OPCODE_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1://=0xD1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
			disasm=DisassembleTypicalOneOperand(disasm,op1,operandSize)+",1";
			break;
		case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL://0xD2,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL://0xD3, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
			disasm=DisassembleTypicalOneOperand(disasm,op1,operandSize)+",CL";
			break;
		}
		break;


	case I486_OPCODE_ARPL://       0x63,
		disasm=DisassembleTypicalTwoOperands("ARPL",op1,op2);
		break;


	case I486_OPCODE_BTS_RM_R://   0xAB0F,
		disasm=DisassembleTypicalTwoOperands("BTS",op1,op2);
		break;


	case I486_OPCODE_CALL_REL://   0xE8,
	case I486_OPCODE_JMP_REL://          0xE9,   // cw or cd
		disasm=(I486_OPCODE_JMP_REL==opCode ? "JMP" : "CALL");
		cpputil::ExtendString(disasm,8);
		{
			auto offset=GetSimm16or32(operandSize);
			auto destin=eip+offset+numBytes;
			disasm+=cpputil::Uitox(destin);

			auto *sym=symTable.Find(cs.value,destin);
			if(nullptr!=sym)
			{
				disasm+=" {"+sym->Format(true,true,true)+"}";
			}
		}
		break;
	case I486_OPCODE_CALL_FAR://   0x9A,
	case I486_OPCODE_JMP_FAR:
		disasm=(I486_OPCODE_JMP_FAR==opCode ? "JMPF" : "CALLF");
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		{
			auto *sym=symTable.Find(op1.seg,op1.offset);
			if(nullptr!=sym)
			{
				disasm+=" {"+sym->Format(true,true,true)+"}";
			}
		}
		break;


	case I486_OPCODE_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF6
	case I486_OPCODE_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF7,
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalTwoOperands("TEST",op1,op2);
			break;
		case 2:
			disasm=DisassembleTypicalOneOperand("NOT",op1,operandSize);
			break;
		case 3:
			disasm=DisassembleTypicalOneOperand("NEG",op1,operandSize);
			break;
		case 4:
			disasm=DisassembleTypicalOneOperand("MUL",op1,operandSize);
			break;
		case 6:
			disasm=DisassembleTypicalOneOperand("DIV",op1,operandSize);
			break;
		case 7:
			disasm=DisassembleTypicalOneOperand("IDIV",op1,operandSize);
			break;
		default:
			disasm=DisassembleTypicalOneOperand(cpputil::Ubtox(opCode)+"?",op1,operandSize);
			break;
		}
		break;


	case I486_OPCODE_CBW_CWDE://        0x98,
		disasm=(16==operandSize ? "CBW" : "CWDE");
		break;
	case I486_OPCODE_CWD_CDQ://         0x99,
		disasm=(16==operandSize ? "CWD" : "CDQ");
		break;
	case I486_OPCODE_CLC:
		disasm="CLC";
		break;
	case I486_OPCODE_CLD:
		disasm="CLD";
		break;
	case I486_OPCODE_CLI:
		disasm="CLI";
		break;
	case I486_OPCODE_CMC://        0xF5,
		disasm="CMC";
		break;


	case I486_OPCODE_CMPSB://           0xA6,
		disasm="CMPSB";
		if(instPrefix==INST_PREFIX_REPE)
		{
			disasm="REPE "+disasm;
		}
		else if(instPrefix==INST_PREFIX_REPNE)
		{
			disasm="REPNE "+disasm;
		}
		break;
	case I486_OPCODE_CMPS://            0xA7,
		disasm=(16==operandSize ? "CMPSW" : "CMPSD");
		if(instPrefix==INST_PREFIX_REPE)
		{
			disasm="REPE "+disasm;
		}
		else if(instPrefix==INST_PREFIX_REPNE)
		{
			disasm="REPNE "+disasm;
		}
		break;


	case I486_OPCODE_ENTER://      0xC8,
		disasm="ENTER   ";
		{
			// Weird operand.
			unsigned int frameSize=operand[0]|(((unsigned int)operand[1])<<8);
			unsigned int level=operand[2];
			disasm+=cpputil::Ustox(frameSize)+"H,";
			disasm+=cpputil::Ubtox(level)+"H";
		}
		break;


	case I486_OPCODE_FWAIT://      0x9B,
		disasm="FWAIT";
		break;
	case I486_OPCODE_FPU_D9_FNSTCW_M16_FNSTENV_F2XM1_FXAM_FXCH_FXTRACT_FYL2X_FYL2XP1_FABS_:// 0xD9,
		if(0xF0<=operand[0] && operand[0]<=0xFF)
		{
			disasm="?FPUINST"+cpputil::Ubtox(opCode)+" "+cpputil::Ubtox(operand[0]);
		}
		else
		{
			switch(GetREG())
			{
			case 7:
				disasm=DisassembleTypicalOneOperand("FNSTCW",op1,operandSize);
				break;
			default:
				disasm="?FPUINST"+cpputil::Ubtox(opCode)+" "+cpputil::Ubtox(operand[0]);
				break;
			}
		}
		if(FPU_FWAIT==fwait)
		{
			disasm="FWAIT "+disasm;
		}
		break;
	case I486_OPCODE_FPU_DB_FNINIT_FRSTOR://     0xDB, 
		if(0xE3==operand[0])
		{
			disasm="FNINIT";
		}
		else
		{
			disasm="?FPUINST"+cpputil::Ubtox(opCode)+" "+cpputil::Ubtox(operand[0]);
		}
		if(FPU_FWAIT==fwait)
		{
			disasm="FWAIT "+disasm;
		}
		break;
	case I486_OPCODE_FPU_DF_FNSTSW_AX://  0xDF,
		if(0xE0==operand[0])
		{
			disasm="FNSTSW  AX";
		}
		else
		{
			disasm="?FPUINST"+cpputil::Ubtox(opCode)+" "+cpputil::Ubtox(operand[0]);
		}
		if(FPU_FWAIT==fwait)
		{
			disasm="FWAIT "+disasm;
		}
		break;


	case I486_OPCODE_ADC_AL_FROM_I8:
		disasm="ADC     AL,"+op1.Disassemble();
		break;
	case I486_OPCODE_ADC_A_FROM_I:
		if(16==operandSize)
		{
			disasm="ADC     AX,"+op1.Disassemble();;
		}
		else
		{
			disasm="ADC     EAX,"+op1.Disassemble();;
		}
		break;
	case I486_OPCODE_ADC_RM8_FROM_R8:
	case I486_OPCODE_ADC_RM_FROM_R:
	case I486_OPCODE_ADC_R8_FROM_RM8:
	case I486_OPCODE_ADC_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("ADC",op1,op2);
		break;


	case I486_OPCODE_ADD_AL_FROM_I8:
		disasm="ADD     AL,"+op1.Disassemble();
		break;
	case I486_OPCODE_ADD_A_FROM_I:
		if(16==operandSize)
		{
			disasm="ADD     AX,"+op1.Disassemble();;
		}
		else
		{
			disasm="ADD     EAX,"+op1.Disassemble();;
		}
		break;
	case I486_OPCODE_ADD_RM8_FROM_R8:
	case I486_OPCODE_ADD_RM_FROM_R:
	case I486_OPCODE_ADD_R8_FROM_RM8:
	case I486_OPCODE_ADD_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("ADD",op1,op2);
		break;


	case I486_OPCODE_AND_AL_FROM_I8:
		disasm="AND     AL,"+op1.Disassemble();
		break;
	case I486_OPCODE_AND_A_FROM_I:
		if(16==operandSize)
		{
			disasm="AND     AX,"+op1.Disassemble();;
		}
		else
		{
			disasm="AND     EAX,"+op1.Disassemble();;
		}
		break;
	case I486_OPCODE_AND_RM8_FROM_R8:
	case I486_OPCODE_AND_RM_FROM_R:
	case I486_OPCODE_AND_R8_FROM_RM8:
	case I486_OPCODE_AND_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("AND",op1,op2);
		break;


	case I486_OPCODE_CMP_AL_FROM_I8:
		disasm="CMP     AL,"+op1.Disassemble();
		break;
	case I486_OPCODE_CMP_A_FROM_I:
		if(16==operandSize)
		{
			disasm="CMP     AX,"+op1.Disassemble();;
		}
		else
		{
			disasm="CMP     EAX,"+op1.Disassemble();;
		}
		break;
	case I486_OPCODE_CMP_RM8_FROM_R8:
	case I486_OPCODE_CMP_RM_FROM_R:
	case I486_OPCODE_CMP_R8_FROM_RM8:
	case I486_OPCODE_CMP_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("CMP",op1,op2);
		break;


	case I486_OPCODE_SBB_AL_FROM_I8:
		disasm="SBB     AL,"+op1.Disassemble();
		break;
	case I486_OPCODE_SBB_A_FROM_I:
		if(16==operandSize)
		{
			disasm="SBB     AX,"+op1.Disassemble();;
		}
		else
		{
			disasm="SBB     EAX,"+op1.Disassemble();;
		}
		break;
	case I486_OPCODE_SBB_RM8_FROM_R8:
	case I486_OPCODE_SBB_RM_FROM_R:
	case I486_OPCODE_SBB_R8_FROM_RM8:
	case I486_OPCODE_SBB_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("SBB",op1,op2);
		break;


	case I486_OPCODE_SUB_AL_FROM_I8:
		disasm="SUB     AL,"+op1.Disassemble();
		break;
	case I486_OPCODE_SUB_A_FROM_I:
		if(16==operandSize)
		{
			disasm="SUB     AX,"+op1.Disassemble();;
		}
		else
		{
			disasm="SUB     EAX,"+op1.Disassemble();;
		}
		break;
	case I486_OPCODE_SUB_RM8_FROM_R8:
	case I486_OPCODE_SUB_RM_FROM_R:
	case I486_OPCODE_SUB_R8_FROM_RM8:
	case I486_OPCODE_SUB_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("SUB",op1,op2);
		break;


	case I486_OPCODE_TEST_AL_FROM_I8:
		disasm="TEST    AL,"+op1.Disassemble();
		break;
	case I486_OPCODE_TEST_A_FROM_I:
		if(16==operandSize)
		{
			disasm="TEST    AX,"+op1.Disassemble();;
		}
		else
		{
			disasm="TEST    EAX,"+op1.Disassemble();;
		}
		break;
	case I486_OPCODE_TEST_RM8_FROM_R8:
	case I486_OPCODE_TEST_RM_FROM_R:
		disasm=DisassembleTypicalTwoOperands("TEST",op1,op2);
		break;


	case I486_OPCODE_DEC_EAX:
	case I486_OPCODE_DEC_ECX:
	case I486_OPCODE_DEC_EDX:
	case I486_OPCODE_DEC_EBX:
	case I486_OPCODE_DEC_ESP:
	case I486_OPCODE_DEC_EBP:
	case I486_OPCODE_DEC_ESI:
	case I486_OPCODE_DEC_EDI:
		disasm="DEC";
		cpputil::ExtendString(disasm,8);
		if(16==operandSize)
		{
			disasm+=Reg16Str[opCode&7];
		}
		else
		{
			disasm+=Reg32Str[opCode&7];
		}
		break;


	case I486_OPCODE_IN_AL_I8://=        0xE4,
		disasm="IN";
		cpputil::ExtendString(disasm,8);
		disasm+="AL,";
		disasm+=op1.Disassemble();
		break;
	case I486_OPCODE_IN_A_I8://=         0xE5,
		disasm="IN";
		cpputil::ExtendString(disasm,8);
		if(16==operandSize)
		{
			disasm+="AX,";
		}
		else
		{
			disasm+="EAX,";
		}
		disasm+=op1.Disassemble();
		break;
	case I486_OPCODE_IN_AL_DX://=        0xEC,
		disasm="IN      AL,DX";
		break;
	case I486_OPCODE_IN_A_DX://=         0xED,
		if(16==operandSize)
		{
			disasm="IN      AX,DX";
		}
		else
		{
			disasm="IN      EAX,DX";
		}
		break;


	case I486_OPCODE_IMUL_R_RM_I8://0x6B,
		disasm="IMUL    ";
		disasm+=op1.Disassemble()+",";
		disasm+=op2.Disassemble()+",";
		disasm+=cpputil::Itox(GetSimm8());
		break;
	case I486_OPCODE_IMUL_R_RM_IMM://0x69,
		disasm="IMUL    ";
		disasm+=op1.Disassemble()+",";
		disasm+=op2.Disassemble()+",";
		disasm+=cpputil::Itox(GetSimm16or32(operandSize));
		break;
	case I486_OPCODE_IMUL_R_RM://       0xAF0F,
		disasm=DisassembleTypicalTwoOperands("IMUL",op1,op2);
		break;


	case I486_OPCODE_LEAVE://            0xC9,
		disasm="LEAVE";
		break;


	case I486_OPCODE_HLT://        0xF4,
		disasm="HLT";
		break;


	case I486_OPCODE_INC_DEC_R_M8:
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperand("INC",op1,8);
			break;
		case 1:
			disasm=DisassembleTypicalOneOperand("DEC",op1,8);
			break;
		}
		break;
	case I486_OPCODE_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH:
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperand("INC",op1,operandSize);
			break;
		case 1:
			disasm=DisassembleTypicalOneOperand("DEC",op1,operandSize);
			break;
		case 2:
			disasm=DisassembleTypicalOneOperand("CALL",op1,operandSize);
			break;
		case 3:
			disasm=DisassembleTypicalOneOperand("CALLF",op1,operandSize);
			break;
		case 4:
			disasm=DisassembleTypicalOneOperand("JMP",op1,operandSize);
			break;
		case 5:
			disasm=DisassembleTypicalOneOperand("JMPF",op1,operandSize);
			break;
		case 6:
			disasm=DisassembleTypicalOneOperand("PUSH",op1,operandSize);
			break;
		}
		break;
	case I486_OPCODE_INC_EAX://    0x40, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ECX://    0x41, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EDX://    0x42, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EBX://    0x43, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ESP://    0x44, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EBP://    0x45, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ESI://    0x46, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EDI://    0x47, // 16/32 depends on OPSIZE_OVERRIDE
		disasm="INC";
		cpputil::ExtendString(disasm,8);
		if(16==operandSize)
		{
			disasm+=Reg16Str[opCode&7];
		}
		else
		{
			disasm+=Reg32Str[opCode&7];
		}
		break;


	case I486_OPCODE_INT3://       0xCC,
		disasm="INT3";
		break;
	case I486_OPCODE_INTO://        0xCD,
	case I486_OPCODE_INT://        0xCD,
		disasm=(I486_OPCODE_INT==opCode ? "INT" : "INTO");
		disasm=DisassembleTypicalOneOperand(disasm,op1,8);
		break;


	case I486_OPCODE_IRET://   0xCF,
		disasm=(16==operandSize ? "IRET" : "IRETD");
		break;


	case I486_OPCODE_JMP_REL8://         0xEB,   // cb
	case I486_OPCODE_JO_REL8:   // 0x70,
	case I486_OPCODE_JNO_REL8:  // 0x71,
	case I486_OPCODE_JB_REL8:   // 0x72,
	case I486_OPCODE_JAE_REL8:  // 0x73,
	case I486_OPCODE_JE_REL8:   // 0x74,
	case I486_OPCODE_JECXZ_REL8:// 0xE3,  // Depending on the operand size
	case I486_OPCODE_JNE_REL8:  // 0x75,
	case I486_OPCODE_JBE_REL8:  // 0x76,
	case I486_OPCODE_JA_REL8:   // 0x77,
	case I486_OPCODE_JS_REL8:   // 0x78,
	case I486_OPCODE_JNS_REL8:  // 0x79,
	case I486_OPCODE_JP_REL8:   // 0x7A,
	case I486_OPCODE_JNP_REL8:  // 0x7B,
	case I486_OPCODE_JL_REL8:   // 0x7C,
	case I486_OPCODE_JGE_REL8:  // 0x7D,
	case I486_OPCODE_JLE_REL8:  // 0x7E,
	case I486_OPCODE_JG_REL8:   // 0x7F,
	case I486_OPCODE_LOOP://             0xE2,
	case I486_OPCODE_LOOPE://            0xE1,
	case I486_OPCODE_LOOPNE://           0xE0,
		switch(opCode)
		{
		case I486_OPCODE_JMP_REL8://         0xEB,   // cb
			disasm="JMP";
			break;
		case I486_OPCODE_JO_REL8:   // 0x70,
			disasm="JO";
			break;
		case I486_OPCODE_JNO_REL8:  // 0x71,
			disasm="JNO";
			break;
		case I486_OPCODE_JB_REL8:   // 0x72,
			disasm="JB";
			break;
		case I486_OPCODE_JAE_REL8:  // 0x73,
			disasm="JAE";
			break;
		case I486_OPCODE_JE_REL8:   // 0x74,
			disasm="JE";
			break;
		case I486_OPCODE_JECXZ_REL8:// 0xE3,  // Depending on the operand size
			disasm=(16==operandSize ? "JCXZ" : "JECXZ");
			break;
		case I486_OPCODE_JNE_REL8:  // 0x75,
			disasm="JNE";
			break;
		case I486_OPCODE_JBE_REL8:  // 0x76,
			disasm="JBE";
			break;
		case I486_OPCODE_JA_REL8:   // 0x77,
			disasm="JA";
			break;
		case I486_OPCODE_JS_REL8:   // 0x78,
			disasm="JS";
			break;
		case I486_OPCODE_JNS_REL8:  // 0x79,
			disasm="JNS";
			break;
		case I486_OPCODE_JP_REL8:   // 0x7A,
			disasm="JP";
			break;
		case I486_OPCODE_JNP_REL8:  // 0x7B,
			disasm="JNP";
			break;
		case I486_OPCODE_JL_REL8:   // 0x7C,
			disasm="JL";
			break;
		case I486_OPCODE_JGE_REL8:  // 0x7D,
			disasm="JGE";
			break;
		case I486_OPCODE_JLE_REL8:  // 0x7E,
			disasm="JLE";
			break;
		case I486_OPCODE_JG_REL8:   // 0x7F,
			disasm="JG";
			break;
		case I486_OPCODE_LOOP://             0xE2,
			disasm="LOOP";
			break;
		case I486_OPCODE_LOOPE://            0xE1,
			disasm="LOOPE";
			break;
		case I486_OPCODE_LOOPNE://           0xE0,
			disasm="LOOPNE";
			break;
		}
		cpputil::ExtendString(disasm,8);
		{
			auto offset=GetSimm8();
			auto destin=eip+offset+numBytes;
			disasm+=cpputil::Uitox(destin);

			auto *sym=symTable.Find(cs.value,destin);
			if(nullptr!=sym)
			{
				disasm+=" {"+sym->Format(true,true,true)+"}";
			}
		}
		break;


	case I486_OPCODE_JA_REL://    0x870F,
	case I486_OPCODE_JAE_REL://   0x830F,
	case I486_OPCODE_JB_REL://    0x820F,
	case I486_OPCODE_JBE_REL://   0x860F,
	// case I486_OPCODE_JC_REL://    0x820F, Same as JB_REL
	case I486_OPCODE_JE_REL://    0x840F,
	// case I486_OPCODE_JZ_REL://    0x840F, Same as JZ_REL
	case I486_OPCODE_JG_REL://    0x8F0F,
	case I486_OPCODE_JGE_REL://   0x8D0F,
	case I486_OPCODE_JL_REL://    0x8C0F,
	case I486_OPCODE_JLE_REL://   0x8E0F,
	// case I486_OPCODE_JNA_REL://   0x860F, Same as JBE_REL
	// case I486_OPCODE_JNAE_REL://  0x820F, Same as JB_REL
	// case I486_OPCODE_JNB_REL://   0x830F, Same as JAE_REL
	// case I486_OPCODE_JNBE_REL://  0x870F, Same as JA_REL
	// case I486_OPCODE_JNC_REL://   0x830F, Same as JAE_REL
	case I486_OPCODE_JNE_REL://   0x850F,
	// case I486_OPCODE_JNG_REL://   0x8E0F, Same as JLE_REL
	// case I486_OPCODE_JNGE_REL://  0x8C0F, Same as JL_REL
	// case I486_OPCODE_JNL_REL://   0x8D0F, Same as JGE_REL
	// case I486_OPCODE_JNLE_REL://  0x8F0F, Same as JG_REL
	case I486_OPCODE_JNO_REL://   0x810F,
	case I486_OPCODE_JNP_REL://   0x8B0F,
	case I486_OPCODE_JNS_REL://   0x890F,
	// case I486_OPCODE_JNZ_REL://   0x850F, Same as JNE_REL
	case I486_OPCODE_JO_REL://    0x800F,
	case I486_OPCODE_JP_REL://    0x8A0F,
	// case I486_OPCODE_JPE_REL://   0x8A0F, Same as JP_REL
	// case I486_OPCODE_JPO_REL://   0x8B0F, Same as JNP_REL
	case I486_OPCODE_JS_REL://    0x880F,
		switch(opCode)
		{
		case I486_OPCODE_JO_REL:   // 0x70,
			disasm="JO";
			break;
		case I486_OPCODE_JNO_REL:  // 0x71,
			disasm="JNO";
			break;
		case I486_OPCODE_JB_REL:   // 0x72,
			disasm="JB";
			break;
		case I486_OPCODE_JAE_REL:  // 0x73,
			disasm="JAE";
			break;
		case I486_OPCODE_JE_REL:   // 0x74,
			disasm="JE";
			break;
		case I486_OPCODE_JNE_REL:  // 0x75,
			disasm="JNE";
			break;
		case I486_OPCODE_JBE_REL:  // 0x76,
			disasm="JBE";
			break;
		case I486_OPCODE_JA_REL:   // 0x77,
			disasm="JA";
			break;
		case I486_OPCODE_JS_REL:   // 0x78,
			disasm="JS";
			break;
		case I486_OPCODE_JNS_REL:  // 0x79,
			disasm="JNS";
			break;
		case I486_OPCODE_JP_REL:   // 0x7A,
			disasm="JP";
			break;
		case I486_OPCODE_JNP_REL:  // 0x7B,
			disasm="JNP";
			break;
		case I486_OPCODE_JL_REL:   // 0x7C,
			disasm="JL";
			break;
		case I486_OPCODE_JGE_REL:  // 0x7D,
			disasm="JGE";
			break;
		case I486_OPCODE_JLE_REL:  // 0x7E,
			disasm="JLE";
			break;
		case I486_OPCODE_JG_REL:   // 0x7F,
			disasm="JG";
			break;
		default:
			disasm="J?";
			break;
		}
		cpputil::ExtendString(disasm,8);
		{
			auto offset=GetSimm16or32(operandSize);
			auto destin=eip+offset+numBytes;
			disasm+=cpputil::Uitox(destin);

			auto *sym=symTable.Find(cs.value,destin);
			if(nullptr!=sym)
			{
				disasm+=" {"+sym->Format(true,true,true)+"}";
			}
		}
		break;


	case I486_OPCODE_BINARYOP_RM8_FROM_I8://=  0x80, // AND(REG=4), OR(REG=1), or XOR(REG=6) depends on the REG field of MODR/M
	case I486_OPCODE_BINARYOP_RM8_FROM_I8_ALIAS:
	case I486_OPCODE_BINARYOP_R_FROM_I://=     0x81,
	case I486_OPCODE_BINARYOP_RM_FROM_SXI8://= 0x83,
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalTwoOperands("ADD",op1,op2);
			break;
		case 1:
			disasm=DisassembleTypicalTwoOperands("OR",op1,op2);
			break;
		case 2:
			disasm=DisassembleTypicalTwoOperands("ADC",op1,op2);
			break;
		case 3:
			disasm=DisassembleTypicalTwoOperands("SBB",op1,op2);
			break;
		case 4:
			disasm=DisassembleTypicalTwoOperands("AND",op1,op2);
			break;
		case 5:
			disasm=DisassembleTypicalTwoOperands("SUB",op1,op2);
			break;
		case 6:
			disasm=DisassembleTypicalTwoOperands("XOR",op1,op2);
			break;
		case 7:
			disasm=DisassembleTypicalTwoOperands("CMP",op1,op2);
			break;
		default:
			disasm=DisassembleTypicalTwoOperands(cpputil::Ubtox(opCode)+"?",op1,op2);
			break;
		}
		break;


	case I486_OPCODE_LAHF://=             0x9F,
		disasm="LAHF";
		break;


	case I486_OPCODE_LEA://=              0x8D,
		disasm="LEA";
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		disasm.push_back(',');
		if(addressSize!=operandSize)
		{
			disasm+=Operand::GetSizeQualifierToDisassembly(op2,addressSize);
		}
		disasm+=op2.Disassemble();
		break;


	case I486_OPCODE_LDS://              0xC5,
	case I486_OPCODE_LSS://              0xB20F,
	case I486_OPCODE_LES://              0xC4,
	case I486_OPCODE_LFS://              0xB40F,
	case I486_OPCODE_LGS://              0xB50F,
		switch(opCode)
		{
		case I486_OPCODE_LDS://              0xC5,
			disasm="LDS";
			break;
		case I486_OPCODE_LSS://              0xB20F,
			disasm="LSS";
			break;
		case I486_OPCODE_LES://              0xC4,
			disasm="LES";
			break;
		case I486_OPCODE_LFS://              0xB40F,
			disasm="LFS";
			break;
		case I486_OPCODE_LGS://              0xB50F,
			disasm="LGS";
			break;
		}
		disasm=DisassembleTypicalTwoOperands(disasm,op1,op2);
		break;


	case I486_OPCODE_LODSB://            0xAC,
		disasm="LODSB";
		break;
	case I486_OPCODE_LODS://             0xAD,
		disasm=(16==operandSize ? "LODSW" : "LODSD");
		break;


	case I486_OPCODE_LGDT_LIDT_SGDT_SIDT:
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperand("SGDT",op1,16+operandSize);
			break;
		case 1:
			disasm=DisassembleTypicalOneOperand("SIDT",op1,16+operandSize);
			break;
		case 2:
			disasm=DisassembleTypicalOneOperand("LGDT",op1,16+operandSize);
			break;
		case 3:
			disasm=DisassembleTypicalOneOperand("LIDT",op1,16+operandSize);
			break;
		case 4:
			disasm=DisassembleTypicalOneOperand("SMSW",op1,16+operandSize);
			break;
		case 6:
			disasm=DisassembleTypicalOneOperand("LMSW",op1,16+operandSize);
			break;
		case 7:
			disasm=DisassembleTypicalOneOperand("INVLPG",op1,16+operandSize);
			break;
		default:
			disasm=DisassembleTypicalTwoOperands(cpputil::Ubtox(opCode)+"?",op1,op2)+" REG="+cpputil::Ubtox(GetREG());
			break;
		}
		break;


	case I486_OPCODE_LSL://              0x030F,
		disasm=DisassembleTypicalTwoOperands("LSL",op1,op2);
		break;


	case I486_OPCODE_MOV_FROM_R8: //      0x88,
	case I486_OPCODE_MOV_FROM_R: //       0x89, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_TO_R8: //        0x8A,
	case I486_OPCODE_MOV_TO_R: //         0x8B, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_FROM_SEG: //     0x8C,
	case I486_OPCODE_MOV_TO_SEG: //       0x8E,
	case I486_OPCODE_MOV_M_TO_AL: //      0xA0,
	case I486_OPCODE_MOV_M_TO_EAX: //     0xA1, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_M_FROM_AL: //    0xA2,
	case I486_OPCODE_MOV_M_FROM_EAX: //   0xA3, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I8_TO_AL: //     0xB0,
	case I486_OPCODE_MOV_I8_TO_CL: //     0xB1,
	case I486_OPCODE_MOV_I8_TO_DL: //     0xB2,
	case I486_OPCODE_MOV_I8_TO_BL: //     0xB3,
	case I486_OPCODE_MOV_I8_TO_AH: //     0xB4,
	case I486_OPCODE_MOV_I8_TO_CH: //     0xB5,
	case I486_OPCODE_MOV_I8_TO_DH: //     0xB6,
	case I486_OPCODE_MOV_I8_TO_BH: //     0xB7,
	case I486_OPCODE_MOV_I_TO_EAX: //     0xB8, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ECX: //     0xB9, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EDX: //     0xBA, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EBX: //     0xBB, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ESP: //     0xBC, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EBP: //     0xBD, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ESI: //     0xBE, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EDI: //     0xBF, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I8_TO_RM8: //    0xC6,
	case I486_OPCODE_MOV_I_TO_RM: //      0xC7, // 16/32 depends on OPSIZE_OVERRIDE

	case I486_OPCODE_MOV_TO_CR://        0x220F,
	case I486_OPCODE_MOV_FROM_CR://      0x200F,
	case I486_OPCODE_MOV_FROM_DR://      0x210F,
	case I486_OPCODE_MOV_TO_DR://        0x230F,
	case I486_OPCODE_MOV_FROM_TR://      0x240F,
	case I486_OPCODE_MOV_TO_TR://        0x260F,

		disasm=DisassembleTypicalTwoOperands("MOV",op1,op2);
		break;


	case I486_OPCODE_MOVSB://            0xA4,
		disasm="MOVSB";
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		break;
	case I486_OPCODE_MOVS://             0xA5,
		disasm=(16==operandSize ? "MOVSW" : "MOVSD");
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		break;


	case I486_OPCODE_MOVSX_R_RM8://=      0xBE0F,
	case I486_OPCODE_MOVZX_R_RM8://=      0xB60F,
		disasm=(I486_OPCODE_MOVZX_R_RM8==opCode ? "MOVZX" : "MOVSX");
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		disasm.push_back(',');
		disasm+=Operand::GetSizeQualifierToDisassembly(op2,8);
		disasm+=op2.Disassemble();
		break;
	case I486_OPCODE_MOVSX_R32_RM16://=   0xBF0F,
	case I486_OPCODE_MOVZX_R32_RM16://=   0xB70F,
		disasm=(I486_OPCODE_MOVZX_R32_RM16==opCode ? "MOVZX" : "MOVSX");
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		disasm.push_back(',');
		disasm+=Operand::GetSizeQualifierToDisassembly(op2,16);
		disasm+=op2.Disassemble();
		break;


	case I486_OPCODE_RET://              0xC3,
		disasm="RET";
		break;
	case I486_OPCODE_RETF://             0xCB,
		disasm="RETF";
		break;
	case I486_OPCODE_RET_I16://          0xC2,
		disasm="RET";
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		break;
	case I486_OPCODE_RETF_I16://         0xCA,
		disasm="RETF";
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		break;


	case I486_OPCODE_SAHF://=             0x9E,
		disasm="SAHF";
		break;


	case I486_OPCODE_SHLD_RM_I8://       0xA40F,
	case I486_OPCODE_SHLD_RM_CL://       0xA50F,
	case I486_OPCODE_SHRD_RM_I8://       0xAC0F,
	case I486_OPCODE_SHRD_RM_CL://       0xAD0F,
		{
			std::string count;
			if(I486_OPCODE_SHLD_RM_CL==opCode || I486_OPCODE_SHRD_RM_CL==opCode)
			{
				count="CL";
			}
			else
			{
				count=cpputil::Ubtox(GetUimm8())+"H";
			}
			switch(opCode)
			{
			case I486_OPCODE_SHLD_RM_I8://       0xA40F,
			case I486_OPCODE_SHLD_RM_CL://       0xA50F,
				disasm="SHLD    ";
				break;
			case I486_OPCODE_SHRD_RM_I8://       0xAC0F,
			case I486_OPCODE_SHRD_RM_CL://       0xAD0F,
				disasm="SHRD    ";
				break;
			}
			disasm+=op1.Disassemble()+",";
			disasm+=op2.Disassemble()+",";
			disasm+=count;
		}
		break;


	case I486_OPCODE_SCASB://            0xAE,
		disasm="SCASB";
		if(instPrefix==INST_PREFIX_REPE)
		{
			disasm="REPE "+disasm;
		}
		else if(instPrefix==INST_PREFIX_REPNE)
		{
			disasm="REPNE "+disasm;
		}
		break;
	case I486_OPCODE_SCAS://             0xAF,
		disasm=(16==operandSize ? "SCASW" : "SCASD");
		if(instPrefix==INST_PREFIX_REPE)
		{
			disasm="REPE "+disasm;
		}
		else if(instPrefix==INST_PREFIX_REPNE)
		{
			disasm="REPNE "+disasm;
		}
		break;


	case I486_OPCODE_SETA://             0x970F,
		disasm=DisassembleTypicalOneOperand("SETA",op1,8);
		break;
	case I486_OPCODE_SETAE://            0x930F,
		disasm=DisassembleTypicalOneOperand("SETAE",op1,8);
		break;
	case I486_OPCODE_SETB://             0x920F,
		disasm=DisassembleTypicalOneOperand("SETB",op1,8);
		break;
	case I486_OPCODE_SETBE://            0x960F,
		disasm=DisassembleTypicalOneOperand("SETBE",op1,8);
		break;
	// I486_OPCODE_SETC://             0x920F,
	case I486_OPCODE_SETE://             0x940F,
		disasm=DisassembleTypicalOneOperand("SETE",op1,8);
		break;
	case I486_OPCODE_SETG://             0x9F0F,
		disasm=DisassembleTypicalOneOperand("SETG",op1,8);
		break;
	case I486_OPCODE_SETGE://            0x9D0F,
		disasm=DisassembleTypicalOneOperand("SETGE",op1,8);
		break;
	case I486_OPCODE_SETL://             0x9C0F,
		disasm=DisassembleTypicalOneOperand("SETL",op1,8);
		break;
	case I486_OPCODE_SETLE://            0x9E0F,
		disasm=DisassembleTypicalOneOperand("SETLE",op1,8);
		break;
	//I486_OPCODE_SETNA://            0x960F,
	//I486_OPCODE_SETNAE://           0x920F,
	//I486_OPCODE_SETNB://            0x930F,
	//I486_OPCODE_SETNBE://           0x970F,
	//I486_OPCODE_SETNC://            0x930F,
	case I486_OPCODE_SETNE://            0x950F,
		disasm=DisassembleTypicalOneOperand("SETNE",op1,8);
		break;
	//I486_OPCODE_SETNG://            0x9E0F,
	//I486_OPCODE_SETNGE://           0x9C0F,
	//I486_OPCODE_SETNL://            0x9D0F,
	//I486_OPCODE_SETNLE://           0x9F0F,
	case I486_OPCODE_SETNO://            0x910F,
		disasm=DisassembleTypicalOneOperand("SETNO",op1,8);
		break;
	case I486_OPCODE_SETNP://            0x9B0F,
		disasm=DisassembleTypicalOneOperand("SETNP",op1,8);
		break;
	case I486_OPCODE_SETNS://            0x990F,
		disasm=DisassembleTypicalOneOperand("SETNS",op1,8);
		break;
	// I486_OPCODE_SETNZ://            0x950F,
	case I486_OPCODE_SETO://             0x900F,
		disasm=DisassembleTypicalOneOperand("SETO",op1,8);
		break;
	case I486_OPCODE_SETP://             0x9A0F,
		disasm=DisassembleTypicalOneOperand("SETP",op1,8);
		break;
	//I486_OPCODE_SETPE://            0x9A0F,
	//I486_OPCODE_SETPO://            0x9B0F,
	case I486_OPCODE_SETS://             0x980F,
		disasm=DisassembleTypicalOneOperand("SETS",op1,8);
		break;
	// I486_OPCODE_SETZ://             0x940F,


	case I486_OPCODE_SLDT_STR_LLDT_LTR_VERR_VERW://             0x000F,
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperand("SLDT",op1,16);
			break;
		case 1:
			disasm=DisassembleTypicalOneOperand("STR",op1,16);
			break;
		case 2:
			disasm=DisassembleTypicalOneOperand("LLDT",op1,16);
			break;
		case 3:
			disasm=DisassembleTypicalOneOperand("LTR",op1,16);
			break;
		case 4:
			disasm=DisassembleTypicalOneOperand("VERR",op1,16);
			break;
		case 5:
			disasm=DisassembleTypicalOneOperand("VERW",op1,16);
			break;
		}
		break;


	case I486_OPCODE_STC://              0xF9,
		disasm="STC";
		break;
	case I486_OPCODE_STD://              0xFD,
		disasm="STD";
		break;
	case I486_OPCODE_STI://              0xFB,
		disasm="STI";
		break;


	case I486_OPCODE_STOSB://            0xAA,
		disasm="STOSB";
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		break;
	case I486_OPCODE_STOS://             0xAB,
		if(16==operandSize)
		{
			disasm="STOSW";
		}
		else
		{
			disasm="STOSD";
		}
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		break;


	case I486_OPCODE_NOP://              0x90,
		disasm="NOP";
		break;


	case I486_OPCODE_OUT_I8_AL: //        0xE6,
		disasm="OUT";
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		disasm+=",AL";
		break;
	case I486_OPCODE_OUT_I8_A: //         0xE7,
		disasm="OUT";
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		if(16==operandSize)
		{
			disasm+=",AX";
		}
		else
		{
			disasm+=",EAX";
		}
		break;
	case I486_OPCODE_OUT_DX_AL: //        0xEE,
		disasm="OUT     DX,AL";
		break;
	case I486_OPCODE_OUT_DX_A: //         0xEF,
		if(16==operandSize)
		{
			disasm="OUT     DX,AX";
		}
		else
		{
			disasm="OUT     DX,EAX";
		}
		break;


	case I486_OPCODE_OUTSB://            0x6E,
		disasm="OUTSB";
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		break;
	case I486_OPCODE_OUTS://             0x6F,
		disasm=(16==operandSize ? "OUTSW" : "OUTSD");
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		break;


	case I486_OPCODE_PUSHA://            0x60,
		disasm=(16==operandSize ? "PUSHA" : "PUSHAD");
		break;
	case I486_OPCODE_PUSHF://            0x9C,
		disasm=(16==operandSize ? "PUSHF" : "PUSHFD");
		break;


	case I486_OPCODE_PUSH_EAX://         0x50,
	case I486_OPCODE_PUSH_ECX://         0x51,
	case I486_OPCODE_PUSH_EDX://         0x52,
	case I486_OPCODE_PUSH_EBX://         0x53,
	case I486_OPCODE_PUSH_ESP://         0x54,
	case I486_OPCODE_PUSH_EBP://         0x55,
	case I486_OPCODE_PUSH_ESI://         0x56,
	case I486_OPCODE_PUSH_EDI://         0x57,
		if(16==operandSize)
		{
			disasm="PUSH    ";
			disasm+=Reg16Str[opCode&7];
		}
		else
		{
			disasm="PUSH    ";
			disasm+=Reg32Str[opCode&7];
		}
		break;
	case I486_OPCODE_PUSH_I8://          0x6A,
		disasm=DisassembleTypicalOneOperand("PUSH",op1,8);
		break;
	case I486_OPCODE_PUSH_I://           0x68,
		disasm=DisassembleTypicalOneOperand("PUSH",op1,operandSize);
		break;
	case I486_OPCODE_PUSH_CS://          0x0E,
		disasm="PUSH    CS";
		break;
	case I486_OPCODE_PUSH_SS://          0x16,
		disasm="PUSH    SS";
		break;
	case I486_OPCODE_PUSH_DS://          0x1E,
		disasm="PUSH    DS";
		break;
	case I486_OPCODE_PUSH_ES://          0x06,
		disasm="PUSH    ES";
		break;
	case I486_OPCODE_PUSH_FS://          0xA00F,
		disasm="PUSH    FS";
		break;
	case I486_OPCODE_PUSH_GS://          0xA80F,
		disasm="PUSH    GS";
		break;


	case I486_OPCODE_POP_M://            0x8F,
		disasm=DisassembleTypicalOneOperand("POP",op1,operandSize);
		break;
	case I486_OPCODE_POP_EAX://          0x58,
	case I486_OPCODE_POP_ECX://          0x59,
	case I486_OPCODE_POP_EDX://          0x5A,
	case I486_OPCODE_POP_EBX://          0x5B,
	case I486_OPCODE_POP_ESP://          0x5C,
	case I486_OPCODE_POP_EBP://          0x5D,
	case I486_OPCODE_POP_ESI://          0x5E,
	case I486_OPCODE_POP_EDI://          0x5F,
		if(16==operandSize)
		{
			disasm="POP     ";
			disasm+=Reg16Str[opCode&7];
		}
		else
		{
			disasm="POP     ";
			disasm+=Reg32Str[opCode&7];
		}
		break;
	case I486_OPCODE_POP_SS://           0x17,
		disasm="POP     SS";
		break;
	case I486_OPCODE_POP_DS://           0x1F,
		disasm="POP     DS";
		break;
	case I486_OPCODE_POP_ES://           0x07,
		disasm="POP     ES";
		break;
	case I486_OPCODE_POP_FS://           0xA10F,
		disasm="POP     FS";
		break;
	case I486_OPCODE_POP_GS://           0xA90F,
		disasm="POP     GS";
		break;


	case I486_OPCODE_POPA://             0x61,
		switch(operandSize)
		{
		case 16:
			disasm="POPA";
			break;
		case 32:
			disasm="POPAD";
			break;
		}
		break;
	case I486_OPCODE_POPF://             0x9D,
		switch(operandSize)
		{
		case 16:
			disasm="POPF";
			break;
		case 32:
			disasm="POPFD";
			break;
		}
		break;


	case I486_OPCODE_OR_AL_FROM_I8:
		disasm="OR      AL,"+op1.Disassemble();
		break;
	case I486_OPCODE_OR_A_FROM_I:
		if(16==operandSize)
		{
			disasm="OR      AX,"+op1.Disassemble();;
		}
		else
		{
			disasm="OR      EAX,"+op1.Disassemble();;
		}
		break;
	case I486_OPCODE_OR_RM8_FROM_R8:
	case I486_OPCODE_OR_RM_FROM_R:
	case I486_OPCODE_OR_R8_FROM_RM8:
	case I486_OPCODE_OR_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("OR",op1,op2);
		break;


	case I486_OPCODE_XOR_AL_FROM_I8:
		disasm="XOR     AL,"+op1.Disassemble();
		break;
	case I486_OPCODE_XOR_A_FROM_I:
		if(16==operandSize)
		{
			disasm="XOR     AX,"+op1.Disassemble();;
		}
		else
		{
			disasm="XOR     EAX,"+op1.Disassemble();;
		}
		break;
	case I486_OPCODE_XOR_RM8_FROM_R8:
	case I486_OPCODE_XOR_RM_FROM_R:
	case I486_OPCODE_XOR_R8_FROM_RM8:
	case I486_OPCODE_XOR_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("XOR",op1,op2);
		break;


	case I486_OPCODE_XCHG_EAX_ECX://     0x91,
	case I486_OPCODE_XCHG_EAX_EDX://     0x92,
	case I486_OPCODE_XCHG_EAX_EBX://     0x93,
	case I486_OPCODE_XCHG_EAX_ESP://     0x94,
	case I486_OPCODE_XCHG_EAX_EBP://     0x95,
	case I486_OPCODE_XCHG_EAX_ESI://     0x96,
	case I486_OPCODE_XCHG_EAX_EDI://     0x97,
		if(16==operandSize)
		{
			disasm="XCHG    AX,";
			disasm+=Reg16Str[opCode&7];
		}
		else
		{
			disasm="XCHG    EAX,";
			disasm+=Reg32Str[opCode&7];
		}
		break;
	case I486_OPCODE_XCHG_RM8_R8://           0x86,
	case I486_OPCODE_XCHG_RM_R://             0x87,
		disasm=DisassembleTypicalTwoOperands("XCHG",op1,op2);
		break;


	case I486_OPCODE_XLAT://             0xD7,
		{
			disasm="XLAT    ";
			disasm+=SegmentOverrideString(segOverride);
			if(16==addressSize)
			{
				disasm+="[BX]";
			}
			else
			{
				disasm+="[EBX]";
			}
		}
		break;
	}

	return disasm;
}

std::string i486DX::Instruction::DisassembleTypicalOneOperand(std::string inst,const Operand &op,int operandSize) const
{
	auto sizeQual=i486DX::Operand::GetSizeQualifierToDisassembly(op,operandSize);
	auto segQual=i486DX::Operand::GetSegmentQualifierToDisassembly(segOverride,op);
	auto disasm=inst;
	cpputil::ExtendString(disasm,8);
	disasm+=sizeQual+segQual+op.Disassemble();
	return disasm;
}

std::string i486DX::Instruction::DisassembleTypicalRM8_I8(std::string inst,const Operand &op1,const Operand &op2) const
{
	auto sizeQual=i486DX::Operand::GetSizeQualifierToDisassembly(op1,8);
	auto segQual=i486DX::Operand::GetSegmentQualifierToDisassembly(segOverride,op1);
	auto disasm=inst;
	cpputil::ExtendString(disasm,8);
	disasm+=sizeQual+segQual+op1.Disassemble()+","+op2.Disassemble();
	return disasm;
}

std::string i486DX::Instruction::DisassembleTypicalRM_I8(std::string inst,const Operand &op1,const Operand &op2) const
{
	auto sizeQual=i486DX::Operand::GetSizeQualifierToDisassembly(op1,operandSize);
	auto segQual=i486DX::Operand::GetSegmentQualifierToDisassembly(segOverride,op1);
	auto disasm=inst;
	cpputil::ExtendString(disasm,8);
	disasm+=sizeQual+segQual+op1.Disassemble()+","+op2.Disassemble();
	return disasm;
}

std::string i486DX::Instruction::DisassembleTypicalTwoOperands(std::string inst,const Operand &op1,const Operand &op2) const
{
	std::string disasm=inst,op1SizeQual,op2SizeQual,op1SegQual,op2SegQual;
	cpputil::ExtendString(disasm,8);

	i486DX::Operand::GetSizeQualifierToDisassembly(op1SizeQual,op2SizeQual,op1,op2);
	op1SegQual=i486DX::Operand::GetSegmentQualifierToDisassembly(segOverride,op1);
	op2SegQual=i486DX::Operand::GetSegmentQualifierToDisassembly(segOverride,op2);
	disasm+=op1SizeQual+op1SegQual+op1.Disassemble();
	disasm.push_back(',');
	disasm+=op2SizeQual+op2SegQual+op2.Disassemble();

	return disasm;
}

unsigned int i486DX::Instruction::GetUimm8(void) const
{
	return operand[operandLen-1];
}
unsigned int i486DX::Instruction::GetUimm16(void) const
{
	return cpputil::GetWord(operand+operandLen-2);
}
unsigned int i486DX::Instruction::GetUimm32(void) const
{
	return cpputil::GetDword(operand+operandLen-4);
}
unsigned int i486DX::Instruction::GetUimm16or32(unsigned int operandSize) const
{
	if(16==operandSize)
	{
		return GetUimm16();
	}
	else
	{
		return GetUimm32();
	}
}
int i486DX::Instruction::GetSimm8(void) const
{
	return cpputil::GetSignedByte(operand[operandLen-1]);
}
int i486DX::Instruction::GetSimm16(void) const
{
	return cpputil::GetSignedWord(operand+operandLen-2);
}
int i486DX::Instruction::GetSimm32(void) const
{
	return cpputil::GetSignedDword(operand+operandLen-4);
}
int i486DX::Instruction::GetSimm16or32(unsigned int operandSize) const
{
	if(16==operandSize)
	{
		return GetSimm16();
	}
	else
	{
		return GetSimm32();
	}
}

/* static */ std::string i486DX::Get8BitRegisterNameFromMODR_M(unsigned char MOD_RM)
{
	auto REG_OPCODE=((MOD_RM>>3)&7);
	return Reg8Str[REG_OPCODE];
}
/* static */ std::string i486DX::Get16BitRegisterNameFromMODR_M(unsigned char MOD_RM)
{
	auto REG_OPCODE=((MOD_RM>>3)&7);
	return Reg16Str[REG_OPCODE];
}
/* static */ std::string i486DX::Get32BitRegisterNameFromMODR_M(unsigned char MOD_RM)
{
	auto REG_OPCODE=((MOD_RM>>3)&7);
	return Reg32Str[REG_OPCODE];
}
/* static */ std::string i486DX::Get16or32BitRegisterNameFromMODR_M(int dataSize,unsigned char MOD_RM)
{
	if(16==dataSize)
	{
		auto REG_OPCODE=((MOD_RM>>3)&7);
		return Reg16Str[REG_OPCODE];
	}
	else
	{
		auto REG_OPCODE=((MOD_RM>>3)&7);
		return Reg32Str[REG_OPCODE];
	}
}


unsigned int i486DX::RunOneInstruction(Memory &mem,InOut &io)
{
	if(true==state.halt)
	{
		return 1;
	}

	state.holdIRQ=false;

	auto inst=FetchInstruction(state.CS(),state.EIP,mem);
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->BeforeRunOneInstruction(*this,mem,io,inst);
	}

	Operand op1,op2;
	inst.DecodeOperand(inst.addressSize,inst.operandSize,op1,op2);

	bool EIPSetByInstruction=false;
	unsigned int clocksPassed=0;

	switch(inst.opCode)
	{
	case I486_OPCODE_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8://0xC0,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1://0xD0, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL://0xD2,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,1);
			auto i=value.GetAsDword();
			unsigned int ctr;
			if(I486_OPCODE_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8==inst.opCode)
			{
				ctr=inst.GetUimm8()&31; // [1] pp.26-243 Only bottom 5 bits are used.
			}
			else if(I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL==inst.opCode)
			{
				ctr=GetCL();
			}
			else
			{
				ctr=1;
			}
			if(true==state.exception)
			{
				break;
			}
			switch(inst.GetREG())
			{
			case 0:// "ROL";
				RolByte(i,ctr);
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			case 1:// "ROR";
				RorByte(i,ctr);
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			case 2:// "RCL";
				RclByte(i,ctr);
				clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11);  // See reminder #20200123-1
				break;
			case 3:// "RCR";
				RcrByte(i,ctr);
				clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11);  // See reminder #20200123-1
				break;
			case 4:// "SHL";
				ShlByte(i,ctr);
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			case 5:// "SHR";
				ShrByte(i,ctr);
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			case 6:// cpputil::Ubtox(opCode)+"?";
				Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			case 7:// "SAR";
				Abort("C1 SAR not implemented yet.");
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			}
			value.SetDword(i);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
		}
		break;
	case I486_OPCODE_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8:// =0xC1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1://=0xD1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL://0xD3, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,1);
			auto i=value.GetAsDword();
			unsigned int ctr=0;
			if(I486_OPCODE_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8==inst.opCode)
			{
				ctr=inst.GetUimm8()&31; // [1] pp.26-243 Only bottom 5 bits are used.
			}
			else if(I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL==inst.opCode)
			{
				ctr=GetCL();
			}
			else
			{
				ctr=1;
			}
			if(true==state.exception)
			{
				break;
			}
			switch(inst.GetREG())
			{
			case 0:// "ROL";
				RolByteWordOrDword(inst.operandSize,i,ctr);
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			case 1:// "ROR";
				Abort("C1 ROR not implemented yet.");
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			case 2:// "RCL";
				RclWordOrDword(inst.operandSize,i,ctr);
				clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11);  // See reminder #20200123-1
				break;
			case 3:// "RCR";
				clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11);  // See reminder #20200123-1
				RcrWordOrDword(inst.operandSize,i,ctr);
				break;
			case 4:// "SHL";
				ShlWordOrDword(inst.operandSize,i,ctr);
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			case 5:// "SHR";
				ShrWordOrDword(inst.operandSize,i,ctr);
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			case 6:// cpputil::Ubtox(opCode)+"?";
				Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			case 7:// "SAR";
				SarByteWordOrDword(inst.operandSize,i,ctr);
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
				break;
			}
			value.SetDword(i);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
		}
		break;

	case I486_OPCODE_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF6
		switch(inst.GetREG())
		{
		case 0: // TEST
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 2 : 1);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				unsigned int byte=value.byteData[0];
				AndByte(byte,inst.GetUimm8());
				SetCF(false);
				SetOverflowFlag(false);
			}
			break;
		case 2: // NOT
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 1);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				if(true!=state.exception)
				{
					value.byteData[0]=~value.byteData[0];
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
				}
			}
			break;
		case 3: // NEG
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 1);
				auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				auto i=value1.GetAsSignedDword();
				SetOverflowFlag(-128==i);
				SetZeroFlag(0==i);
				SetCF(0!=i);
				i=-i;
				SetSignFlag(i<0);
				value1.SetSignedDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
			break;
		case 4: // MUL
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 18 : 13);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				auto mul=GetAL()*value.byteData[0];
				SetAX(mul);
				if(0!=(mul&0xff00))
				{
					SetCF(true);
					SetOverflowFlag(true);
				}
				else
				{
					SetCF(false);
					SetOverflowFlag(false);
				}
			}
			break;
		case 6: // DIV
			{
				clocksPassed=16;
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				if(0==value.byteData[0])
				{
					Interrupt(0,mem,0); // [1] pp.26-28
					// I don't think INT 0 was issued unless division by zero.
					// I thought it just overflew if quo didn't fit in the target register, am I wrong?
				}
				else
				{
					unsigned int quo=GetAX()/value.byteData[0];
					unsigned int rem=GetAX()%value.byteData[0];
					SetAL(quo);
					SetAH(rem);
				}
			}
			break;
		default:
			{
				std::string msg;
				msg="Undefined REG for ";
				msg+=cpputil::Ubtox(inst.opCode);
				msg+="(REG=";
				msg+=cpputil::Ubtox(inst.GetREG());
				msg+=")";
				Abort(msg);
			}
			clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
			break;
		}
		break;
	case I486_OPCODE_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF7,
		switch(inst.GetREG())
		{
		case 0: // TEST
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 2 : 1);
				auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
				unsigned int i1=value1.GetAsDword();
				AndWordOrDword(inst.operandSize,i1,value2.GetAsDword());
				SetCF(false);
				SetOverflowFlag(false);
			}
			break;
		case 2: // NOT
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 2 : 1);
				auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				value1.byteData[0]=~value1.byteData[0];
				value1.byteData[1]=~value1.byteData[1];
				value1.byteData[2]=~value1.byteData[2];
				value1.byteData[3]=~value1.byteData[3];
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
			break;
		case 3: // NEG
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 1);
				auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				auto i=value1.GetAsSignedDword();
				if(16==inst.operandSize)
				{
					SetOverflowFlag(-32768==i);
				}
				else if(32==inst.operandSize)
				{
					SetOverflowFlag(-0x80000000LL==i);
				}
				i=-i;
				SetZeroFlag(0==i);
				SetCF(0!=i);
				SetSignFlag(i<0);
				value1.SetSignedDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
			break;
		case 4: // MUL
			if(16==inst.operandSize)
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 26 : 13);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
				auto DXAX=GetAX()*value.GetAsWord();
				SetAX(DXAX&0xffff);
				SetDX((DXAX>>16)&0xffff);
				if(0!=(DXAX&0xffff0000))
				{
					SetCF(true);
					SetOverflowFlag(true);
				}
				else
				{
					SetCF(false);
					SetOverflowFlag(false);
				}
			}
			else
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 42 : 13);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
				unsigned long long EDXEAX=GetEAX()*value.GetAsDword();
				SetEAX(EDXEAX&0xffffffff);
				SetEDX((EDXEAX>>32)&0xffffffff);
				if(0!=(EDXEAX&0xffffffff00000000))
				{
					SetCF(true);
					SetOverflowFlag(true);
				}
				else
				{
					SetCF(false);
					SetOverflowFlag(false);
				}
			}
			break;
		case 6: // DIV
			{
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				unsigned int denom=value.GetAsDword();
				if(true==state.exception)
				{
					EIPSetByInstruction=true;
				}
				else if(0==denom)
				{
					Interrupt(0,mem,0); // [1] pp.26-28
					// I don't think INT 0 was issued unless division by zero.
					// I thought it just overflew if quo didn't fit in the target register, am I wrong?
				}
				else if(16==inst.operandSize)
				{
					clocksPassed=24;
					unsigned int DXAX=(GetDX()<<16)|GetAX();
					unsigned int quo=DXAX/denom;
					unsigned int rem=DXAX%denom;
					SetAX(quo);
					SetDX(rem);
				}
				else if(32==inst.operandSize)
				{
					clocksPassed=40;
					unsigned long long int EDXEAX=GetEDX();
					EDXEAX<<=32;
					EDXEAX|=GetEAX();
					unsigned int quo=(unsigned int)(EDXEAX/denom);
					unsigned int rem=(unsigned int)(EDXEAX%denom);
					SetEAX(quo);
					SetEDX(rem);
				}
			}
			break;
		case 7: // IDIV
			{
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				int denom=value.GetAsSignedDword();
				if(true==state.exception)
				{
					EIPSetByInstruction=true;
				}
				else if(0==denom)
				{
					Interrupt(0,mem,0); // [1] pp.26-28
					// I don't think INT 0 was issued unless division by zero.
					// I thought it just overflew if quo didn't fit in the target register, am I wrong?
				}
				else if(16==inst.operandSize)
				{
					clocksPassed=24;

					int DXAX=GetDX();
					if(0!=(0x8000&DXAX))
					{
						DXAX-=10000LL;
					}
					DXAX<<=32;
					DXAX|=GetAX();

					int quo=DXAX/denom;
					int rem=DXAX%denom;

					SetAX(quo&0xFFFF);
					SetDX(rem&0xFFFF);
				}
				else if(32==inst.operandSize)
				{
					clocksPassed=40;

					long long int EDXEAX=GetEDX();
					if(0!=(0x80000000&EDXEAX))
					{
						EDXEAX-=100000000LL;
					}
					EDXEAX<<=32;
					EDXEAX|=GetEAX();

					long long int quo=EDXEAX/denom;
					long long int rem=EDXEAX%denom;

					SetEAX(quo&0xFFFFFFFF);
					SetEDX(rem&0xFFFFFFFF);
				}
			}
			break;
		default:
			Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
			clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
			break;
		}
		break;


	case  I486_OPCODE_ADC_AL_FROM_I8://  0x14,
	case  I486_OPCODE_ADD_AL_FROM_I8://  0x04,
	case  I486_OPCODE_AND_AL_FROM_I8://  0x24,
	case  I486_OPCODE_CMP_AL_FROM_I8://  0x3C,
	case   I486_OPCODE_OR_AL_FROM_I8://  0x0C,
	case  I486_OPCODE_SBB_AL_FROM_I8://  0x1C,
	case  I486_OPCODE_SUB_AL_FROM_I8://  0x2C,
	case I486_OPCODE_TEST_AL_FROM_I8://  0xA8,
	case  I486_OPCODE_XOR_AL_FROM_I8:
		{
			clocksPassed=1;
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,1);
			if(true==state.exception)
			{
				break;
			}
			auto al=GetAL();
			auto v=value.GetAsDword();
			switch(inst.opCode)
			{
			case I486_OPCODE_ADC_AL_FROM_I8://  0x14,
				AdcByte(al,v);
				break;
			case I486_OPCODE_ADD_AL_FROM_I8://  0x04,
				AddByte(al,v);
				break;
			case I486_OPCODE_AND_AL_FROM_I8://  0x24,
			case I486_OPCODE_TEST_AL_FROM_I8://  0xA8,
				AndByte(al,v);
				break;
			case  I486_OPCODE_SBB_AL_FROM_I8://  0x1C,
				SbbByte(al,v);
				break;
			case I486_OPCODE_CMP_AL_FROM_I8://  0x3C,
			case I486_OPCODE_SUB_AL_FROM_I8://  0x2C,
				SubByte(al,v);
				break;
			case I486_OPCODE_OR_AL_FROM_I8://    0x0C,
				OrByte(al,v);
				break;
			case I486_OPCODE_XOR_AL_FROM_I8:
				XorByte(al,v);
				break;
			}
			if(I486_OPCODE_TEST_AL_FROM_I8!=inst.opCode &&
			   I486_OPCODE_CMP_AL_FROM_I8!=inst.opCode) // Don't actually update value if TEST or CMP.
			{
				SetAL(al);
			}
		}
		break;
	case  I486_OPCODE_ADC_A_FROM_I://    0x15,
	case  I486_OPCODE_ADD_A_FROM_I://    0x05,
	case  I486_OPCODE_AND_A_FROM_I://    0x25,
	case  I486_OPCODE_CMP_A_FROM_I://    0x3D,
	case   I486_OPCODE_OR_A_FROM_I://    0x0D,
	case  I486_OPCODE_SBB_A_FROM_I://    0x1D,
	case  I486_OPCODE_SUB_A_FROM_I://    0x2D,
	case I486_OPCODE_TEST_A_FROM_I://    0xA9,
	case  I486_OPCODE_XOR_A_FROM_I:
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			if(true==state.exception)
			{
				break;
			}
			auto ax=GetAX();
			auto v=value.GetAsDword();
			switch(inst.opCode)
			{
			case I486_OPCODE_ADC_A_FROM_I://    0x15,
				AdcWord(ax,v);
				break;
			case I486_OPCODE_ADD_A_FROM_I://    0x05,
				AddWord(ax,v);
				break;
			case I486_OPCODE_AND_A_FROM_I://    0x25,
			case I486_OPCODE_TEST_A_FROM_I://    0xA9,
				AndWord(ax,v);
				break;
			case  I486_OPCODE_SBB_A_FROM_I://    0x1D,
				SbbWord(ax,v);
				break;
			case I486_OPCODE_CMP_A_FROM_I://    0x3D,
			case I486_OPCODE_SUB_A_FROM_I://    0x2D,
				SubWord(ax,v);
				break;
			case I486_OPCODE_OR_A_FROM_I://      0x0D,
				OrWord(ax,v);
				break;
			case I486_OPCODE_XOR_A_FROM_I:
				XorWord(ax,v);
				break;
			}
			if(I486_OPCODE_TEST_A_FROM_I!=inst.opCode &&
			   I486_OPCODE_CMP_A_FROM_I!=inst.opCode)
			{
				SetAX(ax);
			}
		}
		else
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			if(true==state.exception)
			{
				break;
			}
			auto eax=GetEAX();
			auto v=value.GetAsDword();
			switch(inst.opCode)
			{
			case I486_OPCODE_ADC_A_FROM_I://    0x15,
				AdcDword(eax,v);
				break;
			case I486_OPCODE_ADD_A_FROM_I://    0x05,
				AddDword(eax,v);
				break;
			case I486_OPCODE_AND_A_FROM_I://    0x25,
			case I486_OPCODE_TEST_A_FROM_I://    0xA9,
				AndDword(eax,v);
				break;
			case  I486_OPCODE_SBB_A_FROM_I://    0x1D,
				SbbDword(eax,v);
				break;
			case I486_OPCODE_CMP_A_FROM_I://    0x3D,
			case I486_OPCODE_SUB_A_FROM_I://    0x2D,
				SubDword(eax,v);
				break;
			case I486_OPCODE_OR_A_FROM_I://      0x0D,
				OrDword(eax,v);
				break;
			case I486_OPCODE_XOR_A_FROM_I:
				XorDword(eax,v);
				break;
			}
			if(I486_OPCODE_TEST_A_FROM_I!=inst.opCode &&
			   I486_OPCODE_CMP_A_FROM_I!=inst.opCode)
			{
				SetEAX(eax);
			}
		}
		break;
	case  I486_OPCODE_ADC_RM8_FROM_R8:// 0x10,
	case  I486_OPCODE_ADD_RM8_FROM_R8:// 0x00,
	case  I486_OPCODE_AND_RM8_FROM_R8:// 0x20,
	case  I486_OPCODE_CMP_RM8_FROM_R8:// 0x38,
	case   I486_OPCODE_OR_RM8_FROM_R8:// 0x08,
	case  I486_OPCODE_SBB_RM8_FROM_R8:// 0x18,
	case  I486_OPCODE_SUB_RM8_FROM_R8:// 0x28,
	case  I486_OPCODE_XOR_RM8_FROM_R8:
	case I486_OPCODE_TEST_RM8_FROM_R8:// 0x84,

	case I486_OPCODE_ADC_R8_FROM_RM8:// 0x12,
	case I486_OPCODE_ADD_R8_FROM_RM8:// 0x02,
	case I486_OPCODE_AND_R8_FROM_RM8:// 0x22,
	case I486_OPCODE_CMP_R8_FROM_RM8:// 0x3A,
	case  I486_OPCODE_OR_R8_FROM_RM8:// 0x0A,
	case I486_OPCODE_SBB_R8_FROM_RM8:// 0x1A,
	case I486_OPCODE_SUB_R8_FROM_RM8:// 0x2A,
	case I486_OPCODE_XOR_R8_FROM_RM8:
		{
			if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR)
			{
				if(I486_OPCODE_TEST_RM8_FROM_R8!=inst.opCode)
				{
					clocksPassed=3;
				}
				else
				{
					clocksPassed=2;
				}
			}
			else
			{
				clocksPassed=1;
			}

			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,1);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,1);
			if(true==state.exception)
			{
				break;
			}
			auto i=value1.GetAsDword();
			switch(inst.opCode)
			{
			case I486_OPCODE_ADC_RM8_FROM_R8:// 0x10,
			case I486_OPCODE_ADC_R8_FROM_RM8:// 0x12,
				AdcByte(i,value2.GetAsDword());
				break;
			case I486_OPCODE_ADD_RM8_FROM_R8:// 0x00,
			case I486_OPCODE_ADD_R8_FROM_RM8:// 0x02,
				AddByte(i,value2.GetAsDword());
				break;
			case I486_OPCODE_TEST_RM8_FROM_R8:// 0x84,
			case  I486_OPCODE_AND_RM8_FROM_R8:// 0x20,
			case  I486_OPCODE_AND_R8_FROM_RM8:// 0x22,
				AndByte(i,value2.GetAsDword());
				break;
			case I486_OPCODE_SBB_RM8_FROM_R8:// 0x18,
			case I486_OPCODE_SBB_R8_FROM_RM8:// 0x1A,
				SbbByte(i,value2.GetAsDword());
				break;
			case I486_OPCODE_CMP_RM8_FROM_R8:// 0x38,
			case I486_OPCODE_CMP_R8_FROM_RM8:// 0x3A,
			case I486_OPCODE_SUB_RM8_FROM_R8:// 0x28,
			case I486_OPCODE_SUB_R8_FROM_RM8:// 0x2A,
				SubByte(i,value2.GetAsDword());
				break;
			case I486_OPCODE_OR_RM8_FROM_R8://   0x08,
			case I486_OPCODE_OR_R8_FROM_RM8://   0x0A,
				OrByte(i,value2.GetAsDword());
				break;
			case I486_OPCODE_XOR_RM8_FROM_R8:
			case I486_OPCODE_XOR_R8_FROM_RM8:
				XorByte(i,value2.GetAsDword());
				break;
			}
			if(I486_OPCODE_TEST_RM8_FROM_R8!=inst.opCode &&
			   I486_OPCODE_CMP_RM8_FROM_R8!=inst.opCode &&
			   I486_OPCODE_CMP_R8_FROM_RM8!=inst.opCode)
			{
				value1.SetDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
		}
		break;
	case I486_OPCODE_ADC_RM_FROM_R://   0x11,
	case  I486_OPCODE_ADD_RM_FROM_R://   0x01,
	case  I486_OPCODE_AND_RM_FROM_R://   0x21,
	case  I486_OPCODE_CMP_RM_FROM_R://   0x39,
	case  I486_OPCODE_SBB_RM_FROM_R://   0x19,
	case  I486_OPCODE_SUB_RM_FROM_R://   0x29,
	case I486_OPCODE_TEST_RM_FROM_R://   0x85,
	case   I486_OPCODE_OR_RM_FROM_R://   0x09,
	case  I486_OPCODE_XOR_RM_FROM_R:

	case I486_OPCODE_ADC_R_FROM_RM://   0x13,
	case I486_OPCODE_ADD_R_FROM_RM://    0x03,
	case I486_OPCODE_AND_R_FROM_RM://    0x23,
	case I486_OPCODE_CMP_R_FROM_RM://    0x3B,
	case I486_OPCODE_SBB_R_FROM_RM://    0x1B,
	case I486_OPCODE_SUB_R_FROM_RM://    0x2B,
	case  I486_OPCODE_OR_R_FROM_RM://    0x0B,
	case I486_OPCODE_XOR_R_FROM_RM:
		{
			if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR)
			{
				if(I486_OPCODE_TEST_RM_FROM_R!=inst.opCode)
				{
					clocksPassed=3;
				}
				else
				{
					clocksPassed=2;
				}
			}
			else
			{
				clocksPassed=1;
			}

			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			if(true==state.exception)
			{
				break;
			}
			auto i=value1.GetAsDword();
			switch(inst.opCode)
			{
			case I486_OPCODE_ADC_RM_FROM_R://   0x11,
			case I486_OPCODE_ADC_R_FROM_RM://   0x13,
				AdcWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case I486_OPCODE_ADD_RM_FROM_R://    0x01,
			case I486_OPCODE_ADD_R_FROM_RM://    0x03,
				AddWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case  I486_OPCODE_AND_RM_FROM_R://   0x21,
			case I486_OPCODE_TEST_RM_FROM_R://   0x85,
			case  I486_OPCODE_AND_R_FROM_RM://   0x23,
				AndWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case  I486_OPCODE_SBB_RM_FROM_R://   0x19,
			case I486_OPCODE_SBB_R_FROM_RM://    0x1B,
				SbbWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case I486_OPCODE_CMP_RM_FROM_R://    0x39,
			case I486_OPCODE_CMP_R_FROM_RM://    0x3B,
			case I486_OPCODE_SUB_RM_FROM_R://    0x29,
			case I486_OPCODE_SUB_R_FROM_RM://    0x2B,
				SubWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case I486_OPCODE_OR_RM_FROM_R://     0x09,
			case I486_OPCODE_OR_R_FROM_RM://     0x0B,
				OrWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case I486_OPCODE_XOR_RM_FROM_R:
			case I486_OPCODE_XOR_R_FROM_RM:
				XorWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			}
			if(I486_OPCODE_TEST_RM_FROM_R!=inst.opCode &&
			   I486_OPCODE_CMP_RM_FROM_R!=inst.opCode &&
			   I486_OPCODE_CMP_R_FROM_RM!=inst.opCode)
			{
				value1.SetDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
		}
		break;


	case I486_OPCODE_ARPL://       0x63,
		{
			clocksPassed=9;
			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			auto dst=value1.GetAsWord();
			auto src=value2.GetAsWord();
			if((dst&3)<(src&3))
			{
				dst&=(~3);
				dst|=(src&3);
				SetZeroFlag(false);
				value1.MakeWord(dst);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
			else
			{
				SetZeroFlag(true);
			}
		}
		break;


	case I486_OPCODE_BTS_RM_R://   0xAB0F,
		{
			clocksPassed=(OPER_ADDR==op1.operandType ? 13 : 6);
			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			if(true!=state.exception)
			{
				auto bitOffset=value2.GetAsByte()&0x1F;
				auto bit=(1<<bitOffset);
				auto src=value2.GetAsDword();
				SetCF(0!=(src&bit));
				if(0==(src&bit))
				{
					src|=bit;
					value2.SetDword(src);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
				}
			}
		}
		break;


	case I486_OPCODE_CALL_FAR://   0x9A,
		{
			if(true==IsInRealMode())
			{
				clocksPassed=18;
			}
			else
			{
				clocksPassed=20;
			}
			Push(mem,inst.operandSize,state.CS().value);
			Push(mem,inst.operandSize,state.EIP+inst.numBytes);

			if(true==enableCallStack)
			{
				PushCallStack(
				    false,0xffff,0xffff,
				    state.CS().value,state.EIP,inst.numBytes,
				    op1.seg,op1.offset);
			}

			LoadSegmentRegister(state.CS(),op1.seg,mem);
			state.EIP=op1.offset;
			EIPSetByInstruction=true;
		}
		break;
	case I486_OPCODE_CALL_REL://   0xE8,
	case I486_OPCODE_JMP_REL://          0xE9,   // cw or cd
		{
			clocksPassed=3;

			auto offset=inst.GetSimm16or32(inst.operandSize);
			auto destin=state.EIP+offset+inst.numBytes;
			if(16==inst.operandSize)
			{
				destin&=0xffff;
			}

			if(I486_OPCODE_CALL_REL==inst.opCode) // Otherwise it is JMP.
			{
				Push(mem,inst.operandSize,state.EIP+inst.numBytes);
				if(true==enableCallStack)
				{
					PushCallStack(
					    false,0xffff,0xffff,
					    state.CS().value,state.EIP,inst.numBytes,
					    state.CS().value,destin);
				}
			}

			state.EIP=destin;
			EIPSetByInstruction=true;
		}
		break;


	case I486_OPCODE_CBW_CWDE://        0x98,
		clocksPassed=3;
		if(16==inst.operandSize) // Sign Extend AL to AX
		{
			unsigned int AL=GetAL();
			if(0!=(0x80&AL))
			{
				AL|=0xff00;
			}
			SetAX(AL);
		}
		else // Sign Extend AX to EAX
		{
			unsigned int AX=GetAX();
			if(0!=(0x8000&AX))
			{
				AX|=0xffff0000;
			}
			SetEAX(AX);
		}
		break;
	case I486_OPCODE_CWD_CDQ://         0x99,
		clocksPassed=3;
		if(16==inst.operandSize) // CWD AX->DX:AX
		{
			SetDX(0!=(GetAX()&0x8000) ? 0xFFFF : 0);
		}
		else // CDQ
		{
			SetEDX(0!=(GetAX()&0x80000000) ? 0xFFFFFFFF : 0);
		}
		break;
	case I486_OPCODE_CLC:
		state.EFLAGS&=(~EFLAGS_CARRY);
		clocksPassed=2;
		break;
	case I486_OPCODE_CLD:
		state.EFLAGS&=(~EFLAGS_DIRECTION);
		clocksPassed=2;
		break;
	case I486_OPCODE_CLI:
		state.EFLAGS&=(~EFLAGS_INT_ENABLE);
		clocksPassed=2;
		break;


	case I486_OPCODE_CMC://        0xF5,
		SetCF(GetCF()==true ? false : true);
		clocksPassed=2;
		break;


	case I486_OPCODE_CMPSB://           0xA6,
	case I486_OPCODE_CMPS://            0xA7,
		if(true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize))
		{
			auto data1=FetchByteWordOrDword(inst.operandSize,inst.addressSize,state.DS(),state.ESI(),mem);
			auto data2=FetchByteWordOrDword(inst.operandSize,inst.addressSize,state.ES(),state.EDI(),mem);
			if(true!=state.exception)
			{
				SubByteWordOrDword(inst.operandSize,data1,data2);
				UpdateDIorEDIAfterStringOp(inst.addressSize,inst.operandSize);
				UpdateSIorESIAfterStringOp(inst.addressSize,inst.operandSize);
				clocksPassed+=8;
				if(true==REPEorNECheck(clocksPassed,inst.instPrefix,inst.addressSize))
				{
					EIPSetByInstruction=true;
				}
			}
		}
		break;


	case I486_OPCODE_ENTER://      0xC8,
		{
			// Weird operand.
			unsigned int frameSize=inst.operand[0]|(((unsigned int)inst.operand[1])<<8);
			unsigned int level=inst.operand[2]&0x1F;

			clocksPassed=14+level*3;

			Push(mem,inst.operandSize,state.EBP());
			auto framePtr=state.ESP();
			while(0<level)
			{
				if(16==inst.operandSize)
				{
					SetBP(GetBP()-2);
					Push(mem,inst.operandSize,state.BP());
				}
				else
				{
					SetEBP(GetEBP()-4);
					Push(mem,inst.operandSize,state.EBP());
				}
			}
			Push(mem,inst.operandSize,framePtr);  // Should it be operandSize or addressSize?  Extremely confusing!
			if(16==inst.operandSize)
			{
				SetBP(framePtr&0xFFFF);
			}
			else
			{
				SetEBP(framePtr);
			}
			if(16==GetStackAddressingSize())
			{
				SetSP(GetSP()-frameSize);
			}
			else
			{
				SetESP(GetESP()-frameSize);
			}
		}
		break;


	case I486_OPCODE_FWAIT://      0x9B,
		if(true==state.fpuState.ExceptionPending())
		{
			EIPSetByInstruction=true;
		}
		clocksPassed=3;
		break;
	case I486_OPCODE_FPU_D9_FNSTCW_M16_FNSTENV_F2XM1_FXAM_FXCH_FXTRACT_FYL2X_FYL2XP1_FABS_:// 0xD9,
		if(0xF0<=inst.operand[0] && inst.operand[0]<=0xFF)
		{
		}
		else
		{
			switch(inst.GetREG())
			{
			case 7: // "FNSTCW"
				{
					OperandValue value;
					value.MakeWord(state.fpuState.GetControlWord());
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
					clocksPassed=3;
				}
				break;
			}
		}
		break;
	case I486_OPCODE_FPU_DB_FNINIT_FRSTOR://     0xDB, 
		if(0xE3==inst.operand[0])
		{
			state.fpuState.FNINIT();
			clocksPassed=17;
		}
		else
		{
		}
		break;
	case I486_OPCODE_FPU_DF_FNSTSW_AX://  0xDF,
		if(0xE0==inst.operand[0])
		{
			SetAX(state.fpuState.GetStatusWord());
			clocksPassed=3;
		}
		else
		{
		}
		break;


	case I486_OPCODE_DEC_EAX:
	case I486_OPCODE_DEC_ECX:
	case I486_OPCODE_DEC_EDX:
	case I486_OPCODE_DEC_EBX:
	case I486_OPCODE_DEC_ESP:
	case I486_OPCODE_DEC_EBP:
	case I486_OPCODE_DEC_ESI:
	case I486_OPCODE_DEC_EDI:
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,op1.GetSize());
			if(true!=state.exception)
			{
				auto i=value.GetAsDword();
				DecrementWordOrDword(inst.operandSize,i);
				value.SetDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
		}
		clocksPassed=1;
		break;


	case I486_OPCODE_IN_AL_I8://=        0xE4,
		{
			auto ioRead=IOIn8(io,inst.operand[0]);
			if(true!=state.exception)
			{
				SetAL(ioRead);
			}
		}
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		break;
	case I486_OPCODE_IN_A_I8://=         0xE5,
		if(16==inst.operandSize)
		{
			auto ioRead=IOIn16(io,inst.operand[0]);
			if(true!=state.exception)
			{
				SetAX(ioRead);
			}
		}
		else
		{
			auto ioRead=IOIn32(io,inst.operand[0]);
			if(true!=state.exception)
			{
				SetEAX(ioRead);
			}
		}
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		break;
	case I486_OPCODE_IN_AL_DX://=        0xEC,
		{
			auto ioRead=IOIn8(io,GetDX());
			if(true!=state.exception)
			{
				SetAL(ioRead);
			}
		}
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		break;
	case I486_OPCODE_IN_A_DX://=         0xED,
		if(16==inst.operandSize)
		{
			auto ioRead=IOIn16(io,GetDX());
			if(true!=state.exception)
			{
				SetAX(ioRead);
			}
		}
		else
		{
			auto ioRead=IOIn32(io,GetDX());
			if(true!=state.exception)
			{
				SetEAX(ioRead);
			}
		}
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		break;


	case I486_OPCODE_IMUL_R_RM_I8://0x6B,
	case I486_OPCODE_IMUL_R_RM_IMM://0x69,
	case I486_OPCODE_IMUL_R_RM://       0xAF0F,
		{
			// Clocks should be 13-26 for 16-bit operand, 13-42 for 32-bit operand, (I486_OPCODE_IMUL_R_RM)
			// or 13-42 (I486_OPCODE_IMUL_R_RM_IMM).
			// I don't know how it should be calculated.
			// I just make it 20 clocks.
			clocksPassed=20;
			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			if(true!=state.exception)
			{
				long long int result;
				if(I486_OPCODE_IMUL_R_RM_I8==inst.opCode)
				{
					long long int i2=value2.GetAsSignedDword();
					long long int i3=inst.GetSimm8();
					result=i2*i3;
					value1.SetSignedDword((int)result);
				}
				else if(I486_OPCODE_IMUL_R_RM_IMM==inst.opCode)
				{
					long long int i2=value2.GetAsSignedDword();
					long long int i3=inst.GetSimm16or32(inst.operandSize);
					result=i2*i3;
					value1.SetSignedDword((int)result);
				}
				else if(I486_OPCODE_IMUL_R_RM==inst.opCode)
				{
					long long int i1=value1.GetAsSignedDword();
					long long int i2=value2.GetAsSignedDword();
					result=i1*i2;
					value1.SetSignedDword((int)result);
				}
				else
				{
					Abort("What IMUL?");
				}
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
				bool clearOFCF=false;
				switch(inst.operandSize)
				{
				case 8:
					clearOFCF=(result<-128 || 127<result);
					break;
				case 16:
					clearOFCF=(result<-32768 || 32767<result);
					break;
				case 32:
					clearOFCF=(result<-0x80000000LL || 0x7FFFFFFFLL<result);
					break;
				}
				if(true==clearOFCF)
				{
					SetCF(false);
					SetOverflowFlag(false);
				}
			}
		}
		break;


	case I486_OPCODE_LEAVE://            0xC9,
		clocksPassed=5;
		if(16==GetStackAddressingSize())
		{
			SetSP(state.BP());
		}
		else
		{
			SetESP(state.EBP());
		}
		if(16==inst.operandSize)
		{
			SetBP(Pop(mem,inst.operandSize));
		}
		else
		{
			SetEBP(Pop(mem,inst.operandSize));
		}
		break;


	case I486_OPCODE_HLT://        0xF4,
		clocksPassed=4;
		state.halt=true;
		break;


	case I486_OPCODE_INC_DEC_R_M8:
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,1);
			if(true!=state.exception)
			{
				auto i=value.GetAsDword();
				switch(inst.GetREG())
				{
				case 0:
					IncrementByte(i);
					break;
				case 1:
					DecrementByte(i);
					break;
				default:
					Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
					break;
				}
				value.SetDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			if(op1.operandType==OPER_ADDR)
			{
				clocksPassed=3;
			}
			else
			{
				clocksPassed=1;
			}
		}
		break;
	case I486_OPCODE_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH:
		{
			auto REG=inst.GetREG();
			switch(REG)
			{
			case 0:
			case 1:
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					if(true!=state.exception)
					{
						auto i=value.GetAsDword();
						if(0==REG)
						{
							IncrementWordOrDword(inst.operandSize,i);
						}
						else
						{
							DecrementWordOrDword(inst.operandSize,i);
						}
						value.SetDword(i);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
					}
					if(op1.operandType==OPER_ADDR)
					{
						clocksPassed=3;
					}
					else
					{
						clocksPassed=1;
					}
				}
				break;
			case 2: // CALL Indirect
			case 4: // JMP Indirect
				{
					clocksPassed=5;  // Same for CALL Indirect and JMP Indirect.
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					if(true!=state.exception)
					{
						if(2==REG) // CALL
						{
							Push(mem,inst.operandSize,state.EIP+inst.numBytes);
							if(true==enableCallStack)
							{
								PushCallStack(
								    false,0xffff,0xffff,
								    state.CS().value,state.EIP,inst.numBytes,
								    state.CS().value,value.GetAsDword());
							}
						}
						auto destin=value.GetAsDword();
						if(16==inst.operandSize)
						{
							destin&=0xffff;
						}
						state.EIP=destin;
						EIPSetByInstruction=true;
					}
				}
				break;
			case 3: // CALLF Indirect
			case 5: // JMPF Indirect
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,(inst.operandSize+16)/8);
					if(true!=state.exception)
					{
						if(3==REG) // Call
						{
							Push(mem,inst.operandSize,state.CS().value);
							Push(mem,inst.operandSize,state.EIP+inst.numBytes);
							if(true==enableCallStack)
							{
								auto destSeg=value.GetFwordSegment();
								auto destEIP=value.GetAsDword();
								if(16==inst.operandSize)
								{
									destEIP&=0xFFFF;
								}
								PushCallStack(
								    false,0xffff,0xffff,
								    state.CS().value,state.EIP,inst.numBytes,
								    destSeg,destEIP);
							}
						}
						SetIPorEIP(inst.operandSize,value.GetAsDword());
						LoadSegmentRegister(state.CS(),value.GetFwordSegment(),mem);
						EIPSetByInstruction=true;
					}
					if(3==REG) // CALLF Indirect
					{
						if(true==IsInRealMode())
						{
							clocksPassed=17;
						}
						else
						{
							clocksPassed=20;
						}
					}
					else if(op1.operandType==OPER_ADDR)
					{
						clocksPassed=3;
					}
					else
					{
						clocksPassed=1;
					}
				}
				break;
			case 6: // PUSH
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					clocksPassed=4;
					if(true!=state.exception)
					{
						Push(mem,inst.operandSize,value.GetAsDword());
					}
				}
				break;
			default:
				Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
				break;
			}
		}
		break;
	case I486_OPCODE_INC_EAX://    0x40, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ECX://    0x41, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EDX://    0x42, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EBX://    0x43, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ESP://    0x44, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EBP://    0x45, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ESI://    0x46, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EDI://    0x47, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,op1.GetSize());
			if(true!=state.exception)
			{
				auto i=value.GetAsDword();
				IncrementWordOrDword(inst.operandSize,i);
				value.SetDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
		}
		break;


	case I486_OPCODE_INT3://       0xCC,
		Interrupt(3,mem,1);
		EIPSetByInstruction=true;
		clocksPassed=26;
		break;
	case I486_OPCODE_INT://        0xCD,
		clocksPassed=(IsInRealMode() ? 30 : 44);
		Interrupt(inst.GetUimm8(),mem,2);
		EIPSetByInstruction=true;
		break;
	case I486_OPCODE_INTO://       0xCE,
		if(GetOF())
		{
			Interrupt(inst.GetUimm8(),mem,2);
			EIPSetByInstruction=true;
			clocksPassed=(IsInRealMode() ? 28 : 46);
		}
		else
		{
			clocksPassed=3;
		}
		break;


	case I486_OPCODE_JMP_REL8://         0xEB,   // cb
	case I486_OPCODE_JO_REL8:   // 0x70,
	case I486_OPCODE_JNO_REL8:  // 0x71,
	case I486_OPCODE_JB_REL8:   // 0x72,
	case I486_OPCODE_JAE_REL8:  // 0x73,
	case I486_OPCODE_JE_REL8:   // 0x74,
	case I486_OPCODE_JECXZ_REL8:// 0xE3,  // Depending on the operand size
	case I486_OPCODE_JNE_REL8:  // 0x75,
	case I486_OPCODE_JBE_REL8:  // 0x76,
	case I486_OPCODE_JA_REL8:   // 0x77,
	case I486_OPCODE_JS_REL8:   // 0x78,
	case I486_OPCODE_JNS_REL8:  // 0x79,
	case I486_OPCODE_JP_REL8:   // 0x7A,
	case I486_OPCODE_JNP_REL8:  // 0x7B,
	case I486_OPCODE_JL_REL8:   // 0x7C,
	case I486_OPCODE_JGE_REL8:  // 0x7D,
	case I486_OPCODE_JLE_REL8:  // 0x7E,
	case I486_OPCODE_JG_REL8:   // 0x7F,
	case I486_OPCODE_LOOP://             0xE2,
	case I486_OPCODE_LOOPE://            0xE1,
	case I486_OPCODE_LOOPNE://           0xE0,
		{
			bool jumpCond=false;
			switch(inst.opCode)
			{
			case I486_OPCODE_JMP_REL8://         0xEB,   // cb
				jumpCond=true;
				break;
			case I486_OPCODE_JO_REL8:   // 0x70,
				jumpCond=CondJO();
				break;
			case I486_OPCODE_JNO_REL8:  // 0x71,
				jumpCond=CondJNO();
				break;
			case I486_OPCODE_JB_REL8:   // 0x72,
				jumpCond=CondJB();
				break;
			case I486_OPCODE_JAE_REL8:  // 0x73,
				jumpCond=CondJAE();
				break;
			case I486_OPCODE_JE_REL8:   // 0x74,
				jumpCond=CondJE();
				break;
			case I486_OPCODE_JECXZ_REL8:// 0xE3,  // Depending on the operand size
				if(16==inst.operandSize)
				{
					jumpCond=(GetCX()==0);
				}
				else
				{
					jumpCond=(GetECX()==0);
				}
				break;
			case I486_OPCODE_JNE_REL8:  // 0x75,
				jumpCond=CondJNE();
				break;
			case I486_OPCODE_JBE_REL8:  // 0x76,
				jumpCond=CondJBE();
				break;
			case I486_OPCODE_JA_REL8:   // 0x77,
				jumpCond=CondJA();
				break;
			case I486_OPCODE_JS_REL8:   // 0x78,
				jumpCond=CondJS();
				break;
			case I486_OPCODE_JNS_REL8:  // 0x79,
				jumpCond=CondJNS();
				break;
			case I486_OPCODE_JP_REL8:   // 0x7A,
				jumpCond=CondJP();
				break;
			case I486_OPCODE_JNP_REL8:  // 0x7B,
				jumpCond=CondJNP();
				break;
			case I486_OPCODE_JL_REL8:   // 0x7C,
				jumpCond=CondJL();
				break;
			case I486_OPCODE_JGE_REL8:  // 0x7D,
				jumpCond=CondJGE();
				break;
			case I486_OPCODE_JLE_REL8:  // 0x7E,
				jumpCond=CondJLE();
				break;
			case I486_OPCODE_JG_REL8:   // 0x7F,
				jumpCond=CondJG();
				break;
			case I486_OPCODE_LOOP://             0xE2,
			case I486_OPCODE_LOOPE://            0xE1,
			case I486_OPCODE_LOOPNE://           0xE0,
				{
					unsigned int ctr;
					if(16==inst.operandSize)
					{
						ctr=((state.ECX())&0xffff)-1;
						state.ECX()=(state.ECX()&0xffff0000)|ctr;
					}
					else
					{
						ctr=state.ECX()-1;
						state.ECX()=(ctr&0xffffffff);
					}
					if(0==ctr ||
					  (I486_OPCODE_LOOPE==inst.opCode && true!=GetZF()) ||
					  (I486_OPCODE_LOOPNE==inst.opCode && true==GetZF()))
					{
						jumpCond=false;
						break;
					}
					jumpCond=true;
				}
				break;
			}
			if(true==jumpCond)
			{
				auto offset=inst.GetSimm8();
				auto destin=state.EIP+offset+inst.numBytes;
				if(16==inst.operandSize)
				{
					destin&=0xffff;
				}
				state.EIP=destin;
				clocksPassed=3;
				EIPSetByInstruction=true;
			}
			else
			{
				clocksPassed=1;
			}
		}
		break;


	case I486_OPCODE_JA_REL://    0x870F,
	case I486_OPCODE_JAE_REL://   0x830F,
	case I486_OPCODE_JB_REL://    0x820F,
	case I486_OPCODE_JBE_REL://   0x860F,
	// case I486_OPCODE_JC_REL://    0x820F, Same as JB_REL
	case I486_OPCODE_JE_REL://    0x840F,
	// case I486_OPCODE_JZ_REL://    0x840F, Same as JZ_REL
	case I486_OPCODE_JG_REL://    0x8F0F,
	case I486_OPCODE_JGE_REL://   0x8D0F,
	case I486_OPCODE_JL_REL://    0x8C0F,
	case I486_OPCODE_JLE_REL://   0x8E0F,
	// case I486_OPCODE_JNA_REL://   0x860F, Same as JBE_REL
	// case I486_OPCODE_JNAE_REL://  0x820F, Same as JB_REL
	// case I486_OPCODE_JNB_REL://   0x830F, Same as JAE_REL
	// case I486_OPCODE_JNBE_REL://  0x870F, Same as JA_REL
	// case I486_OPCODE_JNC_REL://   0x830F, Same as JAE_REL
	case I486_OPCODE_JNE_REL://   0x850F,
	// case I486_OPCODE_JNG_REL://   0x8E0F, Same as JLE_REL
	// case I486_OPCODE_JNGE_REL://  0x8C0F, Same as JL_REL
	// case I486_OPCODE_JNL_REL://   0x8D0F, Same as JGE_REL
	// case I486_OPCODE_JNLE_REL://  0x8F0F, Same as JG_REL
	case I486_OPCODE_JNO_REL://   0x810F,
	case I486_OPCODE_JNP_REL://   0x8B0F,
	case I486_OPCODE_JNS_REL://   0x890F,
	// case I486_OPCODE_JNZ_REL://   0x850F, Same as JNE_REL
	case I486_OPCODE_JO_REL://    0x800F,
	case I486_OPCODE_JP_REL://    0x8A0F,
	// case I486_OPCODE_JPE_REL://   0x8A0F, Same as JP_REL
	// case I486_OPCODE_JPO_REL://   0x8B0F, Same as JNP_REL
	case I486_OPCODE_JS_REL://    0x880F,
		{
			bool jumpCond=false;
			switch(inst.opCode)
			{
			case I486_OPCODE_JO_REL:   // 0x70,
				jumpCond=CondJO();
				break;
			case I486_OPCODE_JNO_REL:  // 0x71,
				jumpCond=CondJNO();
				break;
			case I486_OPCODE_JB_REL:   // 0x72,
				jumpCond=CondJB();
				break;
			case I486_OPCODE_JAE_REL:  // 0x73,
				jumpCond=CondJAE();
				break;
			case I486_OPCODE_JE_REL:   // 0x74,
				jumpCond=CondJE();
				break;
			case I486_OPCODE_JNE_REL:  // 0x75,
				jumpCond=CondJNE();
				break;
			case I486_OPCODE_JBE_REL:  // 0x76,
				jumpCond=CondJBE();
				break;
			case I486_OPCODE_JA_REL:   // 0x77,
				jumpCond=CondJA();
				break;
			case I486_OPCODE_JS_REL:   // 0x78,
				jumpCond=CondJS();
				break;
			case I486_OPCODE_JNS_REL:  // 0x79,
				jumpCond=CondJNS();
				break;
			case I486_OPCODE_JP_REL:   // 0x7A,
				jumpCond=CondJP();
				break;
			case I486_OPCODE_JNP_REL:  // 0x7B,
				jumpCond=CondJNP();
				break;
			case I486_OPCODE_JL_REL:   // 0x7C,
				jumpCond=CondJL();
				break;
			case I486_OPCODE_JGE_REL:  // 0x7D,
				jumpCond=CondJGE();
				break;
			case I486_OPCODE_JLE_REL:  // 0x7E,
				jumpCond=CondJLE();
				break;
			case I486_OPCODE_JG_REL:   // 0x7F,
				jumpCond=CondJG();
				break;
			default:
				Abort("Unhandled Conditional Jump");
				break;
			}

			if(true==jumpCond)
			{
				auto offset=inst.GetSimm16or32(inst.operandSize);
				auto destin=state.EIP+offset+inst.numBytes;
				if(16==inst.operandSize)
				{
					destin&=0xffff;
				}
				state.EIP=destin;
				clocksPassed=3;
				EIPSetByInstruction=true;
			}
			else
			{
				clocksPassed=1;
			}
		}
		break;


	case I486_OPCODE_JMP_FAR:
		{
			switch(inst.operandSize)
			{
			case 16:
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
			LoadSegmentRegister(state.CS(),op1.seg,mem);
			state.EIP=op1.offset;
			EIPSetByInstruction=true;
		}
		break;


	case I486_OPCODE_BINARYOP_RM8_FROM_I8://=  0x80, // AND(REG=4), OR(REG=1), or XOR(REG=6) depends on the REG field of MODR/M
	case I486_OPCODE_BINARYOP_RM8_FROM_I8_ALIAS:
		{
			if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR)
			{
				clocksPassed=3;
			}
			else
			{
				clocksPassed=1;
			}

			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,1);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,1);
			if(true==state.exception)
			{
				break;
			}

			auto i=value1.GetAsDword();
			auto REG=inst.GetREG();
			switch(REG)
			{
			case 0:
				AddByte(i,value2.GetAsDword());
				break;
			case 1:
				OrByte(i,value2.GetAsDword());
				break;
			case 2:
				AdcByte(i,value2.GetAsDword());
				break;
			case 3:
				SbbByte(i,value2.GetAsDword());
				break;
			case 4:
				AndByte(i,value2.GetAsDword());
				break;
			case 5:
				SubByte(i,value2.GetAsDword());
				break;
			case 6:
				XorByte(i,value2.GetAsDword());
				break;
			case 7: // CMP
				SubByte(i,value2.GetAsDword());
				break;
			default:
				Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
				break;
			}
			if(7!=REG) // Don't store a value if it is CMP
			{
				value1.SetDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
		}
		break;

	case I486_OPCODE_BINARYOP_R_FROM_I://=     0x81,
	case I486_OPCODE_BINARYOP_RM_FROM_SXI8://= 0x83, Sign of op2 is already extended when decoded.
		{
			if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR)
			{
				clocksPassed=3;
			}
			else
			{
				clocksPassed=1;
			}

			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			if(true==state.exception)
			{
				break;
			}

			auto i=value1.GetAsDword();
			auto REG=inst.GetREG();
			switch(REG)
			{
			case 0:
				AddWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case 1:
				OrWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case 2:
				AdcWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case 3:
				SbbWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case 4:
				AndWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case 5:
				SubWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case 6:
				XorWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			case 7:
				SubWordOrDword(inst.operandSize,i,value2.GetAsDword());
				break;
			default:
				Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
				break;
			}
			if(7!=REG) // Don't store a value if it is CMP
			{
				value1.SetDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
		}
		break;


	case I486_OPCODE_LAHF://=             0x9F,
		SetAH(state.EFLAGS&0xFF);
		clocksPassed=2;
		break;


	case I486_OPCODE_LEA://=              0x8D,
		clocksPassed=1;
		if(OPER_ADDR==op2.operandType && OPER_REG==op1.operandType)
		{
			unsigned int offset=
			   GetRegisterValue(op2.baseReg)+
			   GetRegisterValue(op2.indexReg)*op2.indexScaling+
			   op2.offset;
			if(16==inst.addressSize)
			{
				offset&=0xFFFF;
			}
			OperandValue value;
			value.MakeDword(offset);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
		}
		else
		{
			RaiseException(EXCEPTION_UD,0);
		}
		break;


	case I486_OPCODE_LDS://              0xC5,
	case I486_OPCODE_LSS://              0xB20F,
	case I486_OPCODE_LES://              0xC4,
	case I486_OPCODE_LFS://              0xB40F,
	case I486_OPCODE_LGS://              0xB50F,
		if(OPER_ADDR==op2.operandType)
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,(inst.operandSize+16)/8);
			if(true!=state.exception)
			{
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
				auto seg=value.GetFwordSegment();
				switch(inst.opCode)
				{
				case I486_OPCODE_LDS://              0xC5,
					LoadSegmentRegister(state.DS(),seg,mem);
					break;
				case I486_OPCODE_LSS://              0xB20F,
					if(0==seg)
					{
						RaiseException(EXCEPTION_GP,0);
					}
					else
					{
						LoadSegmentRegister(state.SS(),seg,mem);
					}
					break;
				case I486_OPCODE_LES://              0xC4,
					LoadSegmentRegister(state.ES(),seg,mem);
					break;
				case I486_OPCODE_LFS://              0xB40F,
					LoadSegmentRegister(state.FS(),seg,mem);
					break;
				case I486_OPCODE_LGS://              0xB50F,
					LoadSegmentRegister(state.GS(),seg,mem);
					break;
				}
			}
			clocksPassed=9;  // It is described as 6/12, but what makes it 6 clocks or 12 clocks is not given.  Quaaaaack!!!!
		}
		else
		{
			RaiseException(EXCEPTION_GP,0);
		}
		break;


	case I486_OPCODE_LODSB://            0xAC,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		if(true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize))
		{
			SetAL(FetchByte(inst.addressSize,state.DS(),state.ESI(),mem));
			UpdateSIorESIAfterStringOp(inst.addressSize,8);
			EIPSetByInstruction=(INST_PREFIX_REP==inst.instPrefix);
			clocksPassed+=5;
		}
		break;
	case I486_OPCODE_LODS://             0xAD,
		if(true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize))
		{
			if(16==inst.operandSize)
			{
				SetAX(FetchWord(inst.addressSize,state.DS(),state.ESI(),mem));
			}
			else
			{
				SetEAX(FetchDword(inst.addressSize,state.DS(),state.ESI(),mem));
			}
			UpdateSIorESIAfterStringOp(inst.addressSize,inst.operandSize);
			EIPSetByInstruction=(INST_PREFIX_REP==inst.instPrefix);
			clocksPassed+=5;
		}
		break;


	case I486_OPCODE_LGDT_LIDT_SGDT_SIDT:
		switch(inst.GetREG())
		{
		case 2: // LGDT
		case 3: // LIDT
			clocksPassed=11;
			if(OPER_ADDR==op1.operandType)
			{
				// If operand size==16, take first 3 bytes of linear-base address.
				// Which means it nees 5 bytes minimum.
				auto numBytes=(16==inst.operandSize ? 5 : 6);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,numBytes);
				switch(inst.GetREG())
				{
				case 2:
					LoadDescriptorTableRegister(state.GDTR,inst.operandSize,value.byteData);
					break;
				case 3:
					LoadDescriptorTableRegister(state.IDTR,inst.operandSize,value.byteData);
					break;
				}
			}
			else
			{
				if(IsInRealMode())
				{
					Interrupt(6,mem,0);
					EIPSetByInstruction=true;
				}
				else
				{
					RaiseException(EXCEPTION_UD,0);
					EIPSetByInstruction=true;
				}
			}
			break;
		case 0: // SGDT
		case 1: // SIDT
			clocksPassed=11;
			if(OPER_ADDR==op1.operandType)
			{
				auto numBytes=(16==inst.operandSize ? 5 : 6);;
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,numBytes);
				OperandValue fwordData;
				switch(inst.GetREG())
				{
				case 0:
					fwordData=DescriptorTableToOperandValue(state.GDTR,inst.operandSize);
					break;
				case 1:
					fwordData=DescriptorTableToOperandValue(state.IDTR,inst.operandSize);
					break;
				}
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,fwordData);
			}
			else
			{
				if(IsInRealMode())
				{
					Interrupt(6,mem,0);
					EIPSetByInstruction=true;
				}
				else
				{
					RaiseException(EXCEPTION_UD,0);
					EIPSetByInstruction=true;
				}
			}
			break;
		case 4: // SMSW
			clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 2);
			{
				OperandValue value;
				value.MakeWord(GetRegisterValue(REG_CR0));
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			break;
		case 6: // LMSW
			clocksPassed=13;
			{
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
				auto i=value.GetAsDword();
				auto cr0=state.GetCR(0);
				cr0&=(~0xFFFF);
				cr0|=(i&0xFFFF);
				SetCR(0,cr0);
			}
			break;
		default:
			Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
			break;
		}
		break;


	case I486_OPCODE_LSL://              0x030F,
		clocksPassed=10;
		{
			auto selectorValue=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8); // What to do with high 16 bits?
			auto selector=selectorValue.GetAsWord();
			SegmentRegister seg;
			LoadSegmentRegisterQuiet(seg,selector,mem,false);
			OperandValue limit;
			limit.MakeDword(seg.limit);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,limit);
			SetZeroFlag(true);
		}
		break;


	case I486_OPCODE_MOV_FROM_SEG: //     0x8C,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=3;
		break;
	case I486_OPCODE_MOV_TO_SEG: //       0x8E,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		if(true==IsInRealMode())
		{
			clocksPassed=3;
		}
		else
		{
			clocksPassed=9;
		}
		break;
	case I486_OPCODE_MOV_FROM_R8: //      0x88,
	case I486_OPCODE_MOV_FROM_R: //       0x89, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_TO_R8: //        0x8A,
	case I486_OPCODE_MOV_TO_R: //         0x8B, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_M_TO_AL: //      0xA0,
	case I486_OPCODE_MOV_M_TO_EAX: //     0xA1, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_M_FROM_AL: //    0xA2,
	case I486_OPCODE_MOV_M_FROM_EAX: //   0xA3, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I8_TO_AL: //     0xB0,
	case I486_OPCODE_MOV_I8_TO_CL: //     0xB1,
	case I486_OPCODE_MOV_I8_TO_DL: //     0xB2,
	case I486_OPCODE_MOV_I8_TO_BL: //     0xB3,
	case I486_OPCODE_MOV_I8_TO_AH: //     0xB4,
	case I486_OPCODE_MOV_I8_TO_CH: //     0xB5,
	case I486_OPCODE_MOV_I8_TO_DH: //     0xB6,
	case I486_OPCODE_MOV_I8_TO_BH: //     0xB7,
	case I486_OPCODE_MOV_I_TO_EAX: //     0xB8, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ECX: //     0xB9, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EDX: //     0xBA, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EBX: //     0xBB, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ESP: //     0xBC, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EBP: //     0xBD, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ESI: //     0xBE, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EDI: //     0xBF, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I8_TO_RM8: //    0xC6,
	case I486_OPCODE_MOV_I_TO_RM: //      0xC7, // 16/32 depends on OPSIZE_OVERRIDE
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=1;
		break;

	case I486_OPCODE_MOV_TO_CR://        0x220F,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=16;
		break;
	case I486_OPCODE_MOV_FROM_CR://      0x200F,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=4;
		break;
	case I486_OPCODE_MOV_FROM_DR://      0x210F,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=10;
		break;
	case I486_OPCODE_MOV_TO_DR://        0x230F,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=11;
		break;
	case I486_OPCODE_MOV_FROM_TR://      0x240F,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=4;  // 3 for TR3 strictly speaking.
		break;
	case I486_OPCODE_MOV_TO_TR://        0x260F,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=4;  // 6 for TR6 strictly speaking.
		break;


	case I486_OPCODE_MOVSB://            0xA4,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		if(true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize))
		{
			auto data=FetchByte(inst.addressSize,state.DS(),state.ESI(),mem);
			StoreByte(mem,inst.addressSize,state.ES(),state.EDI(),data);
			UpdateSIorESIAfterStringOp(inst.addressSize,8);
			UpdateDIorEDIAfterStringOp(inst.addressSize,8);
			EIPSetByInstruction=(INST_PREFIX_REP==inst.instPrefix);
			clocksPassed+=7;
		}
		break;
	case I486_OPCODE_MOVS://             0xA5,
		if(true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize))
		{
			auto data=FetchWordOrDword(inst.operandSize,inst.addressSize,state.DS(),state.ESI(),mem);
			StoreWordOrDword(mem,inst.operandSize,inst.addressSize,state.ES(),state.EDI(),data);
			UpdateSIorESIAfterStringOp(inst.addressSize,inst.operandSize);
			UpdateDIorEDIAfterStringOp(inst.addressSize,inst.operandSize);
			EIPSetByInstruction=(INST_PREFIX_REP==inst.instPrefix);
			clocksPassed+=7;
		}
		break;


	case I486_OPCODE_MOVSX_R_RM8://=      0xBE0F,
	case I486_OPCODE_MOVZX_R_RM8://=      0xB60F, 8bit to 16or32bit
		{
			clocksPassed=3;
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,1);
			if(true!=state.exception)
			{
				value.numBytes=4;
				if(I486_OPCODE_MOVZX_R_RM8==inst.opCode || 0==(value.byteData[0]&0x80))
				{
					value.byteData[1]=0;
					value.byteData[2]=0;
					value.byteData[3]=0;
				}
				else
				{
					value.byteData[1]=0xff;
					value.byteData[2]=0xff;
					value.byteData[3]=0xff;
				}
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
		}
		break;
	case I486_OPCODE_MOVSX_R32_RM16://=   0xBF0F,
	case I486_OPCODE_MOVZX_R32_RM16://=   0xB70F, 16bit to 32bit
		{
			clocksPassed=3;
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,2);
			if(true!=state.exception)
			{
				value.numBytes=4;
				if(I486_OPCODE_MOVZX_R32_RM16==inst.opCode || 0==(value.byteData[1]&0x80))
				{
					value.byteData[2]=0;
					value.byteData[3]=0;
				}
				else
				{
					value.byteData[2]=0xff;
					value.byteData[3]=0xff;
				}
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
		}
		break;


	case I486_OPCODE_NOP://              0x90,
		clocksPassed=1;
		break;


	case I486_OPCODE_OUT_I8_AL: //        0xE6,
		IOOut8(io,inst.operand[0],GetRegisterValue(REG_AL));
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=11; // 31 if CPL>IOPL
		}
		break;
	case I486_OPCODE_OUT_I8_A: //         0xE7,
		if(16==inst.operandSize)
		{
			IOOut16(io,inst.operand[0],GetRegisterValue(REG_AX));
		}
		else
		{
			IOOut32(io,inst.operand[0],GetRegisterValue(REG_EAX));
		}
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=11; // 31 if CPL>IOPL
		}
		break;
	case I486_OPCODE_OUT_DX_AL: //        0xEE,
		IOOut8(io,GetRegisterValue(REG_DX),GetRegisterValue(REG_AL));
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=10; // 30 if CPL>IOPL
		}
		break;
	case I486_OPCODE_OUT_DX_A: //         0xEF,
		if(16==inst.operandSize)
		{
			IOOut16(io,GetRegisterValue(REG_DX),GetRegisterValue(REG_AX));
		}
		else
		{
			IOOut32(io,GetRegisterValue(REG_DX),GetRegisterValue(REG_EAX));
		}
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=10; // 30 if CPL>IOPL
		}
		break;


	case I486_OPCODE_OUTSB://            0x6E,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		if(true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize))
		{
			IOOut8(io,GetDX(),FetchByte(inst.addressSize,state.DS(),state.ESI(),mem));
			UpdateSIorESIAfterStringOp(inst.addressSize,8);
			EIPSetByInstruction=(INST_PREFIX_REP==inst.instPrefix);
			clocksPassed+=(IsInRealMode() ? 17 : 10); // Protected Mode 32 if CPL>IOPL
		}
		break;
	case I486_OPCODE_OUTS://             0x6F,
		if(true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize))
		{
			if(16==inst.operandSize)
			{
				IOOut16(io,GetDX(),FetchWord(inst.addressSize,state.DS(),state.ESI(),mem));
			}
			else
			{
				IOOut32(io,GetDX(),FetchDword(inst.addressSize,state.DS(),state.ESI(),mem));
			}
			UpdateSIorESIAfterStringOp(inst.addressSize,inst.operandSize);
			EIPSetByInstruction=(INST_PREFIX_REP==inst.instPrefix);
			clocksPassed+=(IsInRealMode() ? 17 : 10); // Protected Mode 32 if CPL>IOPL
		}
		break;


	case I486_OPCODE_PUSHA://            0x60,
		clocksPassed=11;
		{
			auto temp=state.ESP();
			Push(mem,inst.operandSize,state.EAX());
			Push(mem,inst.operandSize,state.ECX());
			Push(mem,inst.operandSize,state.EDX());
			Push(mem,inst.operandSize,state.EBX());
			Push(mem,inst.operandSize,temp);
			Push(mem,inst.operandSize,state.EBP());
			Push(mem,inst.operandSize,state.ESI());
			Push(mem,inst.operandSize,state.EDI());
		}
		break;
	case I486_OPCODE_PUSHF://            0x9C,
		clocksPassed=4; // If running as 386 and in protected mode, 3 clocks.
		{
			Push(mem,inst.operandSize,state.EFLAGS);
		}
		break;


	case I486_OPCODE_PUSH_EAX://         0x50,
	case I486_OPCODE_PUSH_ECX://         0x51,
	case I486_OPCODE_PUSH_EDX://         0x52,
	case I486_OPCODE_PUSH_EBX://         0x53,
	case I486_OPCODE_PUSH_ESP://         0x54,
	case I486_OPCODE_PUSH_EBP://         0x55,
	case I486_OPCODE_PUSH_ESI://         0x56,
	case I486_OPCODE_PUSH_EDI://         0x57,
		clocksPassed=1;
		Push(mem,inst.operandSize,state.reg32[(inst.opCode&7)]);
		break;
	case I486_OPCODE_PUSH_I8://          0x6A,
		clocksPassed=1;
		Push(mem,inst.operandSize,inst.GetUimm8());
		break;
	case I486_OPCODE_PUSH_I://           0x68,
		clocksPassed=1;
		Push(mem,inst.operandSize,inst.GetUimm16or32(inst.operandSize));
		break;
	case I486_OPCODE_PUSH_CS://          0x0E,
		Push(mem,inst.operandSize,state.CS().value);
		clocksPassed=3;
		break;
	case I486_OPCODE_PUSH_SS://          0x16,
		Push(mem,inst.operandSize,state.SS().value);
		clocksPassed=3;
		break;
	case I486_OPCODE_PUSH_DS://          0x1E,
		Push(mem,inst.operandSize,state.DS().value);
		clocksPassed=3;
		break;
	case I486_OPCODE_PUSH_ES://          0x06,
		Push(mem,inst.operandSize,state.ES().value);
		clocksPassed=3;
		break;
	case I486_OPCODE_PUSH_FS://          0xA00F,
		Push(mem,inst.operandSize,state.FS().value);
		clocksPassed=3;
		break;
	case I486_OPCODE_PUSH_GS://          0xA80F,
		Push(mem,inst.operandSize,state.GS().value);
		clocksPassed=3;
		break;


	case I486_OPCODE_POP_M://            0x8F,
		clocksPassed=6;
		{
			OperandValue value;
			value.MakeByteWordOrDword(inst.operandSize,Pop(mem,inst.operandSize));
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
		}
		break;
	case I486_OPCODE_POP_EAX://          0x58,
	case I486_OPCODE_POP_ECX://          0x59,
	case I486_OPCODE_POP_EDX://          0x5A,
	case I486_OPCODE_POP_EBX://          0x5B,
	case I486_OPCODE_POP_ESP://          0x5C,
	case I486_OPCODE_POP_EBP://          0x5D,
	case I486_OPCODE_POP_ESI://          0x5E,
	case I486_OPCODE_POP_EDI://          0x5F,
		clocksPassed=4;
		{
			auto value=Pop(mem,inst.operandSize);
			if(16==inst.operandSize)
			{
				state.reg32[(inst.opCode&7)]&=0xffff0000;
				state.reg32[(inst.opCode&7)]|=(value&0xffff);
			}
			else
			{
				state.reg32[(inst.opCode&7)]=value;
			}
		}
		break;
	case I486_OPCODE_POP_SS://           0x17,
		clocksPassed=3;
		LoadSegmentRegister(state.SS(),Pop(mem,inst.operandSize),mem);
		break;
	case I486_OPCODE_POP_DS://           0x1F,
		clocksPassed=3;
		LoadSegmentRegister(state.DS(),Pop(mem,inst.operandSize),mem);
		break;
	case I486_OPCODE_POP_ES://           0x07,
		clocksPassed=3;
		LoadSegmentRegister(state.ES(),Pop(mem,inst.operandSize),mem);
		break;
	case I486_OPCODE_POP_FS://           0xA10F,
		clocksPassed=3;
		LoadSegmentRegister(state.FS(),Pop(mem,inst.operandSize),mem);
		break;
	case I486_OPCODE_POP_GS://           0xA90F,
		clocksPassed=3;
		LoadSegmentRegister(state.GS(),Pop(mem,inst.operandSize),mem);
		break;

	case I486_OPCODE_POPA://             0x61,
		clocksPassed=9;
		if(16==inst.operandSize)
		{
			state.EDI()=((state.EDI()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			state.ESI()=((state.ESI()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			state.EBP()=((state.EBP()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			Pop(mem,inst.operandSize);
			state.EBX()=((state.EBX()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			state.EDX()=((state.EDX()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			state.ECX()=((state.ECX()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			state.EAX()=((state.EAX()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
		}
		else
		{
			state.EDI()=Pop(mem,inst.operandSize);
			state.ESI()=Pop(mem,inst.operandSize);
			state.EBP()=Pop(mem,inst.operandSize);
			Pop(mem,inst.operandSize);
			state.EBX()=Pop(mem,inst.operandSize);
			state.EDX()=Pop(mem,inst.operandSize);
			state.ECX()=Pop(mem,inst.operandSize);
			state.EAX()=Pop(mem,inst.operandSize);
		}
		break;

	case I486_OPCODE_POPF://             0x9D,
		SetFLAGSorEFLAGS(inst.operandSize,Pop(mem,inst.operandSize));
		clocksPassed=(IsInRealMode() ? 9 : 6);
		break;


	case I486_OPCODE_RET://              0xC3,
		clocksPassed=5;
		SetIPorEIP(inst.operandSize,Pop(mem,inst.operandSize));
		EIPSetByInstruction=true;
		if(enableCallStack)
		{
			PopCallStack(state.CS().value,state.EIP);
		}
		break;
	case I486_OPCODE_IRET://   0xCF,
	case I486_OPCODE_RETF://             0xCB,
		if(I486_OPCODE_RETF==inst.opCode)
		{
			if(true==IsInRealMode())
			{
				clocksPassed=13;
			}
			else
			{
				clocksPassed=18;
			}
		}
		else
		{
			if(true==IsInRealMode())
			{
				clocksPassed=15;
			}
			else
			{
				clocksPassed=36;
			}
		}
		SetIPorEIP(inst.operandSize,Pop(mem,inst.operandSize));
		LoadSegmentRegister(state.CS(),Pop(mem,inst.operandSize),mem);
		if(I486_OPCODE_IRET==inst.opCode)
		{
			SetFLAGSorEFLAGS(inst.operandSize,Pop(mem,inst.operandSize));
		}
		EIPSetByInstruction=true;
		if(enableCallStack)
		{
			PopCallStack(state.CS().value,state.EIP);
		}
		break;
	case I486_OPCODE_RET_I16://          0xC2,
		clocksPassed=5;
		SetIPorEIP(inst.operandSize,Pop(mem,inst.operandSize));
		state.ESP()+=inst.GetUimm16(); // Do I need to take &0xffff if address mode is 16? 
		EIPSetByInstruction=true;
		if(enableCallStack)
		{
			PopCallStack(state.CS().value,state.EIP);
		}
		break;
	case I486_OPCODE_RETF_I16://         0xCA,
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=17;
		}
		SetIPorEIP(inst.operandSize,Pop(mem,inst.operandSize));
		LoadSegmentRegister(state.CS(),Pop(mem,inst.operandSize),mem);
		state.ESP()+=inst.GetUimm16(); // Do I need to take &0xffff if address mode is 16? 
		EIPSetByInstruction=true;
		if(enableCallStack)
		{
			PopCallStack(state.CS().value,state.EIP);
		}
		break;


	case I486_OPCODE_SAHF://=             0x9E,
		state.EFLAGS&=(~0xFF);
		state.EFLAGS|=GetAH();
		clocksPassed=2;
		break;


	case I486_OPCODE_SHLD_RM_I8://       0xA40F,
	case I486_OPCODE_SHLD_RM_CL://       0xA50F,
	case I486_OPCODE_SHRD_RM_I8://       0xAC0F,
	case I486_OPCODE_SHRD_RM_CL://       0xAD0F,
		{
			unsigned int count;
			if(I486_OPCODE_SHLD_RM_CL==inst.opCode)
			{
				clocksPassed=3;
				count=GetCL()&0x1F;
			}
			else
			{
				clocksPassed=2;
				count=inst.GetUimm8()&0x1F;
			}
			if(OPER_ADDR==op1.operandType)
			{
				++clocksPassed;
			}
			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);

			unsigned long long int concat;
			switch(inst.opCode)
			{
			case I486_OPCODE_SHLD_RM_I8://       0xA40F,
			case I486_OPCODE_SHLD_RM_CL://       0xA50F,
				if(16==inst.operandSize)
				{
					auto v1=value1.GetAsWord();
					concat=(v1<<16)|value2.GetAsWord();
					concat>>=(16-count);
					value1.MakeWord(concat&0xFFFF);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					SetCF(0!=(concat&0x10000));
					SetOverflowFlag((concat&0x8000)!=(v1&0x8000));
					SetZeroFlag(0==(concat&0xFFFF));
					SetSignFlag(0!=(concat&0x8000));
					SetParityFlag(CheckParity(concat&0xFF));
				}
				else
				{
					unsigned long long int v1=value1.GetAsDword();
					concat=(v1<<32)|value2.GetAsDword();
					concat>>=(32-count);
					value1.MakeDword(concat&0xFFFFFFFF);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					SetCF(0!=(concat&0x100000000LL));
					SetOverflowFlag((concat&0x80000000)!=(v1&0x80000000));
					SetZeroFlag(0==(concat&0xFFFFFFFF));
					SetSignFlag(0!=(concat&0x80000000));
					SetParityFlag(CheckParity(concat&0xFF));
				}
			case I486_OPCODE_SHRD_RM_I8://       0xAC0F,
			case I486_OPCODE_SHRD_RM_CL://       0xAD0F,
				if(16==inst.operandSize)
				{
					auto v1=value1.GetAsWord();
					concat=(value2.GetAsWord()<<16)|v1;
					SetCF(0!=count && 0!=(concat&(1LL<<(count-1))));
					concat>>=count;
					value1.MakeWord(concat&0xffff);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					SetOverflowFlag((concat&0x8000)!=(v1&0x8000));
					SetZeroFlag(0==(concat&0xFFFF));
					SetSignFlag(0!=(concat&0x8000));
					SetParityFlag(CheckParity(concat&0xFF));
				}
				else
				{
					unsigned long long int v1=value1.GetAsDword();
					concat=value2.GetAsDword();
					concat=(concat<<32)|v1;
					SetCF(0!=count && 0!=(concat&(1LL<<(count-1))));
					concat>>=count;
					value1.MakeDword(concat&0xffffffff);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					SetOverflowFlag((concat&0x80000000)!=(v1&0x80000000));
					SetZeroFlag(0==(concat&0xFFFFFFFF));
					SetSignFlag(0!=(concat&0x80000000));
					SetParityFlag(CheckParity(concat&0xFF));
				}
				break;
			}
		}
		break;


	case I486_OPCODE_SCASB://            0xAE,
		if(true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize))
		{
			auto data=FetchByte(inst.addressSize,state.ES(),state.EDI(),mem);
			auto AL=GetAL();
			SubByte(AL,data);
			UpdateDIorEDIAfterStringOp(inst.addressSize,8);
			clocksPassed+=6;
			if(true==REPEorNECheck(clocksPassed,inst.instPrefix,inst.addressSize))
			{
				EIPSetByInstruction=true;
			}
		}
		break;
	case I486_OPCODE_SCAS://             0xAF,
		if(true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize))
		{
			auto data=FetchWordOrDword(inst.operandSize,inst.addressSize,state.ES(),state.EDI(),mem);
			auto EAX=GetEAX();
			SubWordOrDword(inst.operandSize,EAX,data);
			UpdateDIorEDIAfterStringOp(inst.addressSize,inst.operandSize);
			clocksPassed+=6;
			if(true==REPEorNECheck(clocksPassed,inst.instPrefix,inst.addressSize))
			{
				EIPSetByInstruction=true;
			}
		}
		break;


	case I486_OPCODE_SETA://             0x970F,
	case I486_OPCODE_SETAE://            0x930F,
	case I486_OPCODE_SETB://             0x920F,
	case I486_OPCODE_SETBE://            0x960F,
	// I486_OPCODE_SETC://             0x920F,
	case I486_OPCODE_SETE://             0x940F,
	case I486_OPCODE_SETG://             0x9F0F,
	case I486_OPCODE_SETGE://            0x9D0F,
	case I486_OPCODE_SETL://             0x9C0F,
	case I486_OPCODE_SETLE://            0x9E0F,
	//I486_OPCODE_SETNA://            0x960F,
	//I486_OPCODE_SETNAE://           0x920F,
	//I486_OPCODE_SETNB://            0x930F,
	//I486_OPCODE_SETNBE://           0x970F,
	//I486_OPCODE_SETNC://            0x930F,
	case I486_OPCODE_SETNE://            0x950F,
	//I486_OPCODE_SETNG://            0x9E0F,
	//I486_OPCODE_SETNGE://           0x9C0F,
	//I486_OPCODE_SETNL://            0x9D0F,
	//I486_OPCODE_SETNLE://           0x9F0F,
	case I486_OPCODE_SETNO://            0x910F,
	case I486_OPCODE_SETNP://            0x9B0F,
	case I486_OPCODE_SETNS://            0x990F,
	// case I486_OPCODE_SETNZ://            0x950F,
	case I486_OPCODE_SETO://             0x900F,
	case I486_OPCODE_SETP://             0x9A0F,
	//I486_OPCODE_SETPE://            0x9A0F,
	//I486_OPCODE_SETPO://            0x9B0F,
	case I486_OPCODE_SETS://             0x980F,
	// I486_OPCODE_SETZ://             0x940F,
		{
			bool cond=false;
			switch(inst.opCode)
			{
			case I486_OPCODE_SETA://             0x970F,
				cond=CondJA();
				break;
			case I486_OPCODE_SETAE://            0x930F,
				cond=CondJAE();
				break;
			case I486_OPCODE_SETB://             0x920F,
				cond=CondJB();
				break;
			case I486_OPCODE_SETBE://            0x960F,
				cond=CondJBE();
				break;
			// I486_OPCODE_SETC://             0x920F,
			case I486_OPCODE_SETE://             0x940F,
				cond=CondJE();
				break;
			case I486_OPCODE_SETG://             0x9F0F,
				cond=CondJG();
				break;
			case I486_OPCODE_SETGE://            0x9D0F,
				cond=CondJGE();
				break;
			case I486_OPCODE_SETL://             0x9C0F,
				cond=CondJL();
				break;
			case I486_OPCODE_SETLE://            0x9E0F,
				cond=CondJLE();
				break;
			//I486_OPCODE_SETNA://            0x960F,
			//I486_OPCODE_SETNAE://           0x920F,
			//I486_OPCODE_SETNB://            0x930F,
			//I486_OPCODE_SETNBE://           0x970F,
			//I486_OPCODE_SETNC://            0x930F,
			case I486_OPCODE_SETNE://            0x950F,
				cond=CondJNE();
				break;
			//I486_OPCODE_SETNG://            0x9E0F,
			//I486_OPCODE_SETNGE://           0x9C0F,
			//I486_OPCODE_SETNL://            0x9D0F,
			//I486_OPCODE_SETNLE://           0x9F0F,
			case I486_OPCODE_SETNO://            0x910F,
				cond=CondJNO();
				break;
			case I486_OPCODE_SETNP://            0x9B0F,
				cond=CondJNP();
				break;
			case I486_OPCODE_SETNS://            0x990F,
				cond=CondJNS();
				break;
			//I486_OPCODE_SETNZ://            0x950F,
			case I486_OPCODE_SETO://             0x900F,
				cond=CondJO();
				break;
			case I486_OPCODE_SETP://             0x9A0F,
				cond=CondJP();
				break;
			//I486_OPCODE_SETPE://            0x9A0F,
			//I486_OPCODE_SETPO://            0x9B0F,
			case I486_OPCODE_SETS://             0x980F,
				cond=CondJS();
				break;
			// I486_OPCODE_SETZ://             0x940F,
			}
			OperandValue value;
			if(true==cond)
			{
				value.MakeByte(1);
				clocksPassed=4;
			}
			else
			{
				value.MakeByte(0);
				clocksPassed=3;
			}
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
		}
		break;


	case I486_OPCODE_SLDT_STR_LLDT_LTR_VERR_VERW://             0x000F,
		switch(inst.GetREG())
		{
		case 0: // "SLDT"
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 2);
				OperandValue value;
				value.MakeWord(state.LDTR.selector);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			break;
		case 1: // "STR"
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 2);
				OperandValue value;
				value.MakeWord(state.TR.selector);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			break;
		case 2: // "LLDT"
			{
				clocksPassed=11;
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				if(true!=state.exception)
				{
					auto selector=value.GetAsWord();
					auto TI=(0!=(selector&4));
					state.LDTR.selector=selector;
					if(0==selector)
					{
						state.LDTR.linearBaseAddr=0;
						state.LDTR.limit=0;
					}
					else if(0!=TI) // Pointing LDT
					{
						RaiseException(EXCEPTION_GP,selector); // [1] pp.26-199
					}
					else
					{
						SegmentRegister seg;
						LoadSegmentRegisterQuiet(seg,selector,mem,false); // Force to read from GDT by setting isInRealMode=false
						const unsigned char byteData[]=
						{
							 seg.limit    &0xff,
							(seg.limit>>8)&0xff,
							 seg.baseLinearAddr     &0xff,
							(seg.baseLinearAddr>>8) &0xff,
							(seg.baseLinearAddr>>16)&0xff,
							(seg.baseLinearAddr>>24)&0xff,
						};
						LoadDescriptorTableRegister(state.LDTR,32,byteData);
					}
				}
			}
			break;
		case 3: // 
			break;
		case 4: // "VERR"
			break;
		case 5: // "VERW"
			break;
		}
		break;


	case I486_OPCODE_STC://              0xFB,
		SetCF(true);
		clocksPassed=2;
		break;
	case I486_OPCODE_STD://              0xFD,
		SetDF(true);
		clocksPassed=2;
		break;
	case I486_OPCODE_STI://              0xFB,
		SetIF(true);
		clocksPassed=5;
		break;


	case I486_OPCODE_STOSB://            0xAA,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		if(true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize))
		{
			StoreByte(mem,inst.addressSize,state.ES(),state.EDI(),GetAL());
			UpdateDIorEDIAfterStringOp(inst.addressSize,8);
			EIPSetByInstruction=(INST_PREFIX_REP==inst.instPrefix);
			clocksPassed+=5;
		}
		break;
	case I486_OPCODE_STOS://             0xAB,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		if(true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize))
		{
			StoreWordOrDword(mem,inst.operandSize,inst.addressSize,state.ES(),state.EDI(),GetEAX());
			UpdateDIorEDIAfterStringOp(inst.addressSize,inst.operandSize);
			EIPSetByInstruction=(INST_PREFIX_REP==inst.instPrefix);
			clocksPassed+=5;
		}
		break;


	case I486_OPCODE_XCHG_EAX_ECX://     0x91,
	case I486_OPCODE_XCHG_EAX_EDX://     0x92,
	case I486_OPCODE_XCHG_EAX_EBX://     0x93,
	case I486_OPCODE_XCHG_EAX_ESP://     0x94,
	case I486_OPCODE_XCHG_EAX_EBP://     0x95,
	case I486_OPCODE_XCHG_EAX_ESI://     0x96,
	case I486_OPCODE_XCHG_EAX_EDI://     0x97,
		clocksPassed=3;
		if(16==inst.operandSize)
		{
			auto op1=GetAX();
			auto op2=state.reg32[inst.opCode&7];
			SetAX(op2);
			state.reg32[inst.opCode&7]=(state.reg32[inst.opCode&7]&0xffff0000)|(op1&0xffff);
		}
		else
		{
			auto op1=GetEAX();
			auto op2=state.reg32[inst.opCode&7];
			SetEAX(op2);
			state.reg32[inst.opCode&7]=op1;
		}
		break;
	case I486_OPCODE_XCHG_RM8_R8://           0x86,
		clocksPassed=(OPER_ADDR==op1.operandType ? 5 : 3);
		{
			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,1);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,1);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value2);
			StoreOperandValue(op2,mem,inst.addressSize,inst.segOverride,value1);
		}
		break;
	case I486_OPCODE_XCHG_RM_R://             0x87,
		clocksPassed=(OPER_ADDR==op1.operandType ? 5 : 3);
		{
			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value2);
			StoreOperandValue(op2,mem,inst.addressSize,inst.segOverride,value1);
		}
		break;


	case I486_OPCODE_XLAT://             0xD7,
		clocksPassed=4;
		{
			SegmentRegister seg;
			if(0!=inst.segOverride)
			{
				seg=state.GetSegmentFromSegmentOverridePrefix(inst.segOverride);
			}
			else
			{
				seg=state.DS();
			}
			unsigned int offset=GetAL();
			if(32==inst.addressSize)
			{
				offset+=GetEBX();
			}
			else
			{
				offset+=GetBX();
			}
			auto nextAL=FetchByte(inst.addressSize,seg,offset,mem);
			if(true!=state.exception)
			{
				SetAL(nextAL);
			}
		}
		break;


	default:
		Abort("Undefined instruction or simply not supported yet.");
		break;
	}

	if(true!=abort && 0==clocksPassed)
	{
		Abort("Clocks-Passed is not set.");
	}
	if(true!=EIPSetByInstruction && true!=abort)
	{
		state.EIP+=inst.numBytes;
	}

	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->AfterRunOneInstruction(clocksPassed,*this,mem,io,inst);
	}

	return clocksPassed;
}
