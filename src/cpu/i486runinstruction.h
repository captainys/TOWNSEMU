#ifndef I486RUNINSTRUCTION_IS_INCLUDED
#define I486RUNINSTRUCTION_IS_INCLUDED
/* { */

#include <iostream>

template <class CPUCLASS,class MEMCLASS,class FUNCCLASS>
inline unsigned int i486DXCommon::FetchOperandRMandDecode(
    Operand &op,int addressSize,int dataSize,
    CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,MEMCLASS &mem)
{
	auto *operandPtr=inst.operand;
	operandPtr[0]=FUNCCLASS::FetchInstructionByte(cpu,ptr,inst.codeAddressSize,seg,offset,mem);

	NUM_BYTES_TO_BASIC_REG_BASE
	NUM_BYTES_TO_REGISTER_OPERAND_TYPE

	const auto MODR_M=operandPtr[0];
	const auto MOD=((MODR_M>>6)&3);
	// const auto REG_OPCODE=((MODR_M>>3)&7);
	#define R_M (MODR_M&7)

	op.Clear();

	if(16==addressSize)
	{
		static const unsigned char caseTable[256]=
		{
			3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,
			3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
			4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
		};
		static const unsigned int MODR_M_to_BaseIndex[256][2]=
		{
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},

			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},

			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},

			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
		};

		switch(caseTable[operandPtr[0]])
		{
		case 0:
			FUNCCLASS::FetchInstructionTwoBytes(operandPtr+1,cpu,ptr,inst.codeAddressSize,seg,offset+1,mem);

			op.operandType=OPER_ADDR;
			op.baseReg=REG_NULL;
			op.indexReg=REG_NULL;
			// indexShift=0; Already cleared in Clear()
			op.offset=cpputil::GetSignedWord(operandPtr+1);
			op.offsetBits=16;
			inst.operandLen=3;
			break;
		case 1:
			operandPtr[1]=FUNCCLASS::FetchInstructionByte(cpu,ptr,inst.codeAddressSize,seg,offset+1,mem);

			op.operandType=OPER_ADDR;
			// indexShisft=0; Already cleared in Clear()
			op.baseReg=MODR_M_to_BaseIndex[MODR_M][0];
			op.indexReg=MODR_M_to_BaseIndex[MODR_M][1];
			op.offsetBits=8;
			op.offset=cpputil::GetSignedByte(operandPtr[1]);
			inst.operandLen=2;
			break;
		case 2:
			FUNCCLASS::FetchInstructionTwoBytes(operandPtr+1,cpu,ptr,inst.codeAddressSize,seg,offset+1,mem);

			op.operandType=OPER_ADDR;
			// indexShisft=0; Already cleared in Clear()
			op.baseReg=MODR_M_to_BaseIndex[MODR_M][0];
			op.indexReg=MODR_M_to_BaseIndex[MODR_M][1];
			op.offsetBits=16;
			op.offset=cpputil::GetSignedWord(operandPtr+1);
			inst.operandLen=3;
			break;
		case 3:
			op.operandType=OPER_ADDR;
			// indexShisft=0; Already cleared in Clear()
			op.baseReg=MODR_M_to_BaseIndex[MODR_M][0];
			op.indexReg=MODR_M_to_BaseIndex[MODR_M][1];
			op.offset=0;  // Tentative
			op.offsetBits=16;
			inst.operandLen=1;
			break;
		case 4:
			op.operandType=numBytesToRegisterOperandType[dataSize>>3];
			op.reg=R_M+(numBytesToBasicRegBase[dataSize>>3]);
			inst.operandLen=1;
			break;
		}
	}
	else // if(32==addressSize)
	{
		enum
		{
			A=0,
			B=1,
			C=2,
			D=3,
			E=4,
			F=5,
			G=6,
			H=7,
		};

		static const unsigned char caseTable[256]=
		{
			E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,
			E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,
			C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,
			C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,
			D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,
			D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,
			F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,
			F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,
		};

		static const unsigned int SIB_INDEX[8]=
		{
			REG_EAX,
			REG_ECX,
			REG_EDX,
			REG_EBX,
			REG_NULL,
			REG_EBP,
			REG_ESI,
			REG_EDI,
		};

		switch (caseTable[operandPtr[0]])
		{
		case A:
			FUNCCLASS::FetchInstructionFourBytes(operandPtr + 1, cpu, ptr, inst.codeAddressSize, seg, offset + 1, mem);

			op.operandType = OPER_ADDR;
			op.baseReg = REG_NULL;
			op.indexReg = REG_NULL;
			// indexShift=0; Already cleared in Clear()
			op.offset = cpputil::GetSignedDword(operandPtr + 1);
			op.offsetBits = 32;
			inst.operandLen = 5;
			break;
		case B: // MOD==0
		{
			operandPtr[1] = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset + 1, mem);

			op.operandType = OPER_ADDR;
			// indexShift=0; Already cleared in Clear()
			op.offset = 0;

			auto SIB = operandPtr[1];
			auto SS = ((SIB >> 6) & 3);
			auto INDEX = ((SIB >> 3) & 7);
			auto BASE = (SIB & 7);

			if (5 != BASE)
			{
				op.baseReg = REG_32BIT_REG_BASE + BASE;
				inst.operandLen = 2;
			}
			else
			{
				FUNCCLASS::FetchInstructionFourBytes(operandPtr + 2, cpu, ptr, inst.codeAddressSize, seg, offset + 2, mem);

				op.baseReg = REG_NULL; // 0b00==MOD && 5==BASE  disp32[index]
				op.offsetBits = 32;
				op.offset = cpputil::GetSignedDword(operandPtr + 2);
				inst.operandLen = 6;
			}

			op.indexReg = SIB_INDEX[INDEX];
			op.indexShift = SS;
		}
		break;
		case C:
			operandPtr[1] = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset + 1, mem);

			op.operandType = OPER_ADDR;
			// indexShift=0; Already cleared in Clear()

			op.baseReg = REG_32BIT_REG_BASE + R_M;
			op.indexReg = REG_NULL;

			op.offsetBits = 8;
			op.offset = cpputil::GetSignedByte(operandPtr[1]);
			inst.operandLen = 2;
			break;
		case D:
			FUNCCLASS::FetchInstructionFourBytes(operandPtr + 1, cpu, ptr, inst.codeAddressSize, seg, offset + 1, mem);

			op.operandType = OPER_ADDR;
			// indexShift=0; Already cleared in Clear()

			op.baseReg = REG_32BIT_REG_BASE + R_M;
			op.indexReg = REG_NULL;

			op.offsetBits = 32;
			op.offset = cpputil::GetSignedDword(operandPtr + 1);
			inst.operandLen = 5;
			break;
		case E:
			op.operandType = OPER_ADDR;
			// indexShift=0; Already cleared in Clear()
			op.offset = 0;
			inst.operandLen = 1;

			op.baseReg = REG_32BIT_REG_BASE + R_M;
			op.indexReg = REG_NULL;
			break;
		case F:
			op.operandType = numBytesToRegisterOperandType[dataSize >> 3];
			op.reg = R_M + (numBytesToBasicRegBase[dataSize >> 3]);
			inst.operandLen = 1;
			break;
		case G: // MOD==1
		{
			FUNCCLASS::FetchInstructionTwoBytes(operandPtr + 1, cpu, ptr, inst.codeAddressSize, seg, offset + 1, mem);

			op.operandType = OPER_ADDR;
			// indexShift=0; Already cleared in Clear()
			op.offset = 0;

			auto SIB = operandPtr[1];
			auto SS = ((SIB >> 6) & 3);
			auto INDEX = ((SIB >> 3) & 7);
			auto BASE = (SIB & 7);

			if (5 != BASE)
			{
				op.baseReg = REG_32BIT_REG_BASE + BASE;
			}
			else
			{
				op.baseReg = REG_EBP;
			}

			op.indexReg = SIB_INDEX[INDEX];
			op.indexShift = SS;

			op.offsetBits = 8;
			op.offset = cpputil::GetSignedByte(operandPtr[2]);

			inst.operandLen = 3;
		}
		break;
		case H: // MOD==2
		{
			operandPtr[1] = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset + 1, mem);
			FUNCCLASS::FetchInstructionFourBytes(operandPtr + 2, cpu, ptr, inst.codeAddressSize, seg, offset + 2, mem);

			op.operandType = OPER_ADDR;
			// indexShift=0; Already cleared in Clear()
			op.offset = 0;

			auto SIB = operandPtr[1];
			auto SS = ((SIB >> 6) & 3);
			auto INDEX = ((SIB >> 3) & 7);
			auto BASE = (SIB & 7);

			if (5 != BASE)
			{
				op.baseReg = REG_32BIT_REG_BASE + BASE;
			}
			else
			{
				op.baseReg = REG_EBP; // 0b10==MOD   disp[EBP][index]
			}

			op.indexReg = SIB_INDEX[INDEX];
			op.indexShift = SS;

			op.offsetBits = 32;
			op.offset = cpputil::GetSignedDword(operandPtr + 2);

			inst.operandLen = 6;
		}
		break;
		}
	}

	inst.numBytes+=inst.operandLen;
	return inst.operandLen;
}

template <class CPUCLASS,class MEMCLASS,class FUNCCLASS>
void i486DXCommon::FetchOperand(CPUCLASS &cpu,InstructionAndOperand &instOp,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,int offset,MEMCLASS &mem,unsigned int defOperSize,unsigned int defAddrSize)
{
	auto &inst=instOp.inst;
	auto &op1=instOp.op1;
	auto &op2=instOp.op2;

	bool refetch;

	do
	{
		refetch = false;

		switch (opCodeNeedOperandTable[inst.opCode])
		{
			// No Operand
		case I486_NEEDOPERAND_NONE:
			break;



			// RM_IMM8
		case I486_NEEDOPERAND_RM_IMM8:
			offset += FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, 8, cpu, inst, ptr, seg, offset, mem);
			FUNCCLASS::FetchImm8(cpu, inst, ptr, seg, offset, mem);
			break;



			// RM_IMM
		case I486_NEEDOPERAND_RM_IMM:
			offset += FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			FUNCCLASS::FetchImm8(cpu, inst, ptr, seg, offset, mem);
			break;



			// RM8
		case I486_NEEDOPERAND_RM8:
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, 8, cpu, inst, ptr, seg, offset, mem);
			break;



			// RM_X
		case I486_NEEDOPERAND_RM_X:
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			break;
			// RM_R(Will be integrated with RM_X)
		case I486_NEEDOPERAND_BT_R_RM://    0x0FA3,
		case I486_NEEDOPERAND_BTC_RM_R://   0x0FBB,
		case I486_NEEDOPERAND_BTS_RM_R://   0x0FAB,
		case I486_NEEDOPERAND_BTR_RM_R://   0x0FB3,
		case I486_NEEDOPERAND_SHLD_RM_CL://       0x0FA5,
		case I486_NEEDOPERAND_SHRD_RM_CL://       0x0FAD,
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			op2.DecodeMODR_MForRegister(inst.operandSize, inst.operand[0]);
			break;




			// IMM8
		case I486_NEEDOPERAND_IMM8:
			FUNCCLASS::FetchImm8(cpu, inst, ptr, seg, offset, mem);
			break;


			// IMM
		case I486_NEEDOPERAND_IMM:
			FUNCCLASS::FetchImm16or32(cpu, inst, ptr, seg, offset, mem);
			break;



			// R_RM(WIll be integrated with X_RM)
		case I486_NEEDOPERAND_BSF_R_RM://   0x0FBC,
		case I486_NEEDOPERAND_BSR_R_RM://   0x0FBD,
		case I486_NEEDOPERAND_LAR:
		case I486_NEEDOPERAND_IMUL_R_RM://       0x0FAF,
		case I486_NEEDOPERAND_LEA://=              0x8D,
		case I486_NEEDOPERAND_LSL://              0x0F03,
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op2, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			op1.DecodeMODR_MForRegister(inst.operandSize, inst.operand[0]);
			break;



			// X_RM
		case I486_NEEDOPERAND_X_RM:
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op2, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			break;



			// X_RM8
		case I486_NEEDOPERAND_X_RM8:
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op2, inst.addressSize, 8, cpu, inst, ptr, seg, offset, mem);
			break;



		case I486_NEEDOPERAND_FARPTR:
			offset += FUNCCLASS::FetchOperand16or32(cpu, inst, ptr, seg, offset, mem);
			FUNCCLASS::FetchOperand16(cpu, inst, ptr, seg, offset, mem);
			op1.DecodeFarAddr(inst.addressSize, inst.operandSize, inst.operand);
			break;



		case I486_NEEDOPERAND_BINARYOP_R_FROM_I:
		case I486_NEEDOPERAND_MOV_I_TO_RM: //      0xC7, // 16/32 depends on OPSIZE_OVERRIDE
			offset += FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			FUNCCLASS::FetchImm16or32(cpu, inst, ptr, seg, offset, mem);
			break;



		case I486_NEEDOPERAND_MOV_M_TO_AL: //      0xA0, // 16/32 depends on ADDRESSSIZE_OVERRIDE
		case I486_NEEDOPERAND_MOV_M_TO_EAX: //     0xA1, // 16/32 depends on ADDRESSSIZE_OVERRIDE
		case I486_NEEDOPERAND_MOV_M_FROM_AL: //    0xA2, // 16/32 depends on ADDRESSSIZE_OVERRIDE
		case I486_NEEDOPERAND_MOV_M_FROM_EAX: //   0xA3, // 16/32 depends on ADDRESSSIZE_OVERRIDE
			switch (inst.addressSize)
			{
			case 16:
				FUNCCLASS::FetchImm16(cpu, inst, ptr, seg, offset, mem);
				break;
			default:
				FUNCCLASS::FetchImm32(cpu, inst, ptr, seg, offset, mem);
				break;
			}
			break;



		case I486_NEEDOPERAND_MOVSX_R32_RM16://=   0x0FBF,
		case I486_NEEDOPERAND_MOVZX_R32_RM16://=   0x0FB7,
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op2, inst.addressSize, 16, cpu, inst, ptr, seg, offset, mem);
			op1.DecodeMODR_MForRegister(32, inst.operand[0]);
			break;



		case I486_NEEDOPERAND_RET_I16://          0xC2,
		case I486_NEEDOPERAND_RETF_I16://         0xCA,
			FUNCCLASS::FetchImm16(cpu, inst, ptr, seg, offset, mem);
			break;



		case I486_NEEDOPERAND_SHLD_RM_I8://       0x0FA4,
		case I486_NEEDOPERAND_SHRD_RM_I8://       0x0FAC,
			offset += FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			FUNCCLASS::FetchImm8(cpu, inst, ptr, seg, offset, mem);
			op2.DecodeMODR_MForRegister(inst.operandSize, inst.operand[0]);
			break;



		case I486_NEEDOPERAND_SETX:
			inst.operandSize = 8;
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			break;



			// Yes Operand
		case I486_NEEDOPERAND_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF6
			inst.operandSize = 8;
			offset += FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			if (0 == inst.GetREG()) // TEST RM8,I8
			{
				FUNCCLASS::FetchImm8(cpu, inst, ptr, seg, offset, mem);
			}
			break;
		case I486_NEEDOPERAND_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF7,
			offset += FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			if (0 == inst.GetREG()) // TEST RM8,I8
			{
				FUNCCLASS::FetchImm16or32(cpu, inst, ptr, seg, offset, mem);
			}
			break;



		case I486_NEEDOPERAND_ARPL://       0x63,
			inst.operandSize = 16;
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			op2.DecodeMODR_MForRegister(inst.operandSize, inst.operand[0]);
			break;






		case I486_NEEDOPERAND_ENTER://      0xC8,
			FUNCCLASS::FetchOperand16(cpu, inst, ptr, seg, offset, mem);
			offset += 2;
			FUNCCLASS::FetchOperand8(cpu, inst, ptr, seg, offset, mem);
			break;


		case I486_NEEDOPERAND_FPU_D8: // 0xD8
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu, MODR_M, inst, ptr, seg, offset, mem);
			if (0xC0 <= MODR_M) // FADD ST,STi  FMUL ST,STI, FCOM, FCOMP, FSUB, FDIV, FDIVR
			{
				FUNCCLASS::FetchOperand8(cpu, inst, ptr, seg, offset, mem);
			}
			else
			{
				FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			}
		}
		break;
		case I486_NEEDOPERAND_FPU_D9:// 0xD9,
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu, MODR_M, inst, ptr, seg, offset, mem);
			if ((0xC0 <= MODR_M && MODR_M <= 0xCF) || // FLD_ST, FXCHG
				0xE0 == MODR_M || // FCHS
				0xE1 == MODR_M || // FABS
				0xE4 == MODR_M || // FTST
				0xE5 == MODR_M || // FXAM
				(0xE8 <= MODR_M && MODR_M <= 0xEE) ||
				(0xF0 <= MODR_M && MODR_M <= 0xFF))
			{
				FUNCCLASS::FetchOperand8(cpu, inst, ptr, seg, offset, mem);
			}
			else
			{
				FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			}
		}
		break;
		case I486_NEEDOPERAND_FPU_DA:
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu, MODR_M, inst, ptr, seg, offset, mem);
			if (0xE9 == MODR_M) // FUCOMPP
			{
				FUNCCLASS::FetchOperand8(cpu, inst, ptr, seg, offset, mem);
			}
			else
			{
				FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			}
		}
		break;
		case I486_NEEDOPERAND_FPU_DB://     0xDB, 
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu, MODR_M, inst, ptr, seg, offset, mem);
			if (0xE3 == MODR_M || // FNINIT
				0xE2 == MODR_M || // FCLEX
				0xE4 == MODR_M || // FSETPM
				0xE0 == MODR_M || // FNENI
				0xE1 == MODR_M)   // FNDISI
			{
				FUNCCLASS::FetchOperand8(cpu, inst, ptr, seg, offset, mem);
			}
			else
			{
				switch (Instruction::GetREG(MODR_M))
				{
				case 0: // FILD m32int
				case 2: // FIST m32int
				case 3: // FISTP m32int
				case 5: // FLD m80real
				case 7: // FSTP m80real
					FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
					break;
				case 1:
				case 4:
				case 6:
				default:
					FUNCCLASS::FetchOperand8(cpu, inst, ptr, seg, offset, mem);
					break;
				}
			}
		}
		break;
		case I486_NEEDOPERAND_FPU_DC:
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu, MODR_M, inst, ptr, seg, offset, mem);
			if (0xC0 <= MODR_M)
			{
				FUNCCLASS::FetchOperand8(cpu, inst, ptr, seg, offset, mem);
			}
			else
			{
				FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			}
		}
		break;
		case I486_NEEDOPERAND_FPU_DD:
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu, MODR_M, inst, ptr, seg, offset, mem);
			if (0xC0 <= (MODR_M & 0xF8) && (MODR_M & 0xF8) <= 0xE8)
			{
				FUNCCLASS::FetchOperand8(cpu, inst, ptr, seg, offset, mem);
			}
			else
			{
				switch (Instruction::GetREG(MODR_M))
				{
				default:
					FUNCCLASS::FetchOperand8(cpu, inst, ptr, seg, offset, mem);
					break;
					break;
				case 0:	// FLD m64real
				case 2: // FST m64real
				case 3: // FSTP m64real
				case 4: // FRSTOR m94/108byte
				case 6: // FSAVE m94/108byte
				case 7: // FNSTSW m2byte
					FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
					break;
				}
			}
		}
		break;

		case I486_NEEDOPERAND_FPU_DE:
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu, MODR_M, inst, ptr, seg, offset, mem);
			if ((0xC8 <= MODR_M && MODR_M <= 0xCF) ||
				(0xE0 <= MODR_M && MODR_M <= 0xEF) ||
				(0xF0 <= MODR_M && MODR_M <= 0xFF) ||
				0xD9 == MODR_M ||
				(0xC0 <= MODR_M && MODR_M <= 0xC7))
			{
				// 0xC8+i:FMULP
				// 0xD9:FCOMPP
				// 0xF0+i:FDIVR
				// 0xF8+i:FDIVRP
				// 0xC0 to 0xC7:FADDP
				FUNCCLASS::FetchOperand8(cpu, inst, ptr, seg, offset, mem);
			}
			else
			{
				FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			}
		}
		break;

		case I486_NEEDOPERAND_FPU_DF://  0xDF,
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu, MODR_M, inst, ptr, seg, offset, mem);
			if (0xE0 == MODR_M) // FNSTSW
			{
				FUNCCLASS::FetchOperand8(cpu, inst, ptr, seg, offset, mem);
			}
			else
			{
				FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
				switch (Instruction::GetREG(MODR_M))
				{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				default:
					break;
				}
			}
		}
		break;






		case I486_NEEDOPERAND_IMUL_R_RM_I8://0x6B,
			offset += FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op2, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			FUNCCLASS::FetchImm8(cpu, inst, ptr, seg, offset, mem);
			op1.DecodeMODR_MForRegister(inst.operandSize, inst.operand[0]);
			break;
		case I486_NEEDOPERAND_IMUL_R_RM_IMM://0x69,
			offset += FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op2, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			if (16 == inst.operandSize)
			{
				FUNCCLASS::FetchImm16(cpu, inst, ptr, seg, offset, mem);
			}
			else
			{
				FUNCCLASS::FetchImm32(cpu, inst, ptr, seg, offset, mem);
			}
			op1.DecodeMODR_MForRegister(inst.operandSize, inst.operand[0]);
			break;




		case I486_NEEDOPERAND_LGDT_LIDT_SGDT_SIDT:
		{
			auto MODR_M = FUNCCLASS::PeekInstructionByte(cpu, ptr, inst.addressSize, seg, offset, mem);
			auto REG = inst.GetREG(MODR_M);
			if (4 == REG || 6 == REG)
			{
				inst.operandSize = 16;
			}
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
		}
		break;



		case I486_NEEDOPERAND_MOV_FROM_SEG: //     0x8C,
			// Example:  8c c6           MOV SI,ES
			// Sreg: ES=0, CS=1, SS=2, DS=3, FD=4, GS=5 (OPCODE part of MODR_M)  [1] pp.26-10
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			// From observation on 2020/04/22, if the upper-16 bit of the destination register is undefined
			// F-BASIC386 booted from CD may fail to open windows.
			// From observation on 2020/10/24, if the operandSize is 32, upper 32-bit of the destination
			// register must be clear instead of being preserved.
			// Therefore, MOV AX,DS and MOV EAX,DS probably should behave differently.
			// Hence the following clause "Force it to be 16-bit" is reverted and commented out.
			// inst.operandSize=16; // Force it to be 16-bit
			op2.DecodeMODR_MForSegmentRegister(inst.operand[0]);
			break;
		case I486_NEEDOPERAND_MOV_TO_SEG: //       0x8E,
			inst.operandSize = 16; // Force it to be 16-bit
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op2, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			op1.DecodeMODR_MForSegmentRegister(inst.operand[0]);
			break;




		case I486_NEEDOPERAND_MOV_TO_CR://        0x0F22,
			inst.operandSize = 32; // [1] pp.26-213 32bit operands are always used with these instructions, 
			//      regardless of the opreand-size attribute.
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op2, inst.addressSize, 32, cpu, inst, ptr, seg, offset, mem);
			op1.DecodeMODR_MForCRRegister(inst.operand[0]);
			break;
		case I486_NEEDOPERAND_MOV_TO_DR://        0x0F23,
			inst.operandSize = 32; // [1] pp.26-213 32bit operands are always used with these instructions, 
			//      regardless of the opreand-size attribute.
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op2, inst.addressSize, 32, cpu, inst, ptr, seg, offset, mem);
			op1.DecodeMODR_MForDRRegister(inst.operand[0]);
			break;
		case I486_NEEDOPERAND_MOV_TO_TR://        0x0F26,
			inst.operandSize = 32; // [1] pp.26-213 32bit operands are always used with these instructions, 
			//      regardless of the opreand-size attribute.
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op2, inst.addressSize, 32, cpu, inst, ptr, seg, offset, mem);
			op1.DecodeMODR_MForTestRegister(inst.operand[0]);
			break;
		case I486_NEEDOPERAND_MOV_FROM_CR://      0x0F20,
			inst.operandSize = 32; // [1] pp.26-213 32bit operands are always used with these instructions, 
			//      regardless of the opreand-size attribute.
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, 32, cpu, inst, ptr, seg, offset, mem);
			op2.DecodeMODR_MForCRRegister(inst.operand[0]);
			break;
		case I486_NEEDOPERAND_MOV_FROM_DR://      0x0F21,
			inst.operandSize = 32; // [1] pp.26-213 32bit operands are always used with these instructions, 
			//      regardless of the opreand-size attribute.
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, 32, cpu, inst, ptr, seg, offset, mem);
			op2.DecodeMODR_MForDRRegister(inst.operand[0]);
			break;
		case I486_NEEDOPERAND_MOV_FROM_TR://      0x0F24,
			inst.operandSize = 32; // [1] pp.26-213 32bit operands are always used with these instructions, 
			//      regardless of the opreand-size attribute.
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, 32, cpu, inst, ptr, seg, offset, mem);
			op2.DecodeMODR_MForTestRegister(inst.operand[0]);
			break;



		case I486_NEEDOPERAND_SLDT_STR_LLDT_LTR_VERR_VERW://             0x0F00,
			inst.operandSize = 16;
			FetchOperandRMandDecode<CPUCLASS, MEMCLASS, FUNCCLASS>(op1, inst.addressSize, inst.operandSize, cpu, inst, ptr, seg, offset, mem);
			break;



			// Handling multi-byte instruction and pre-fixes >>
	// This instruction (0x0F) needs the second byte.
	// Fetch it, and then re-fetch operand.
		case I486_NEEDOPERAND_NEED_SECOND_BYTE: //0x0F
			inst.opCode = I486_OPCODE_TWOBYTE_FLAG | FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
			// The following cases are not really operand type, but handling these here saves a prefix-fetch loop.
		case I486_NEEDOPERAND_PREFIX_REP: // REP/REPE/REPZ
			inst.instPrefix = INST_PREFIX_REP;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
		case I486_NEEDOPERAND_PREFIX_REPNE:
			inst.instPrefix = INST_PREFIX_REPNE;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
		case I486_NEEDOPERAND_PREFIX_LOCK:
			inst.instPrefix = INST_PREFIX_LOCK;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
		case I486_NEEDOPERAND_SEG_OVERRIDE_CS:
			inst.segOverride = SEG_OVERRIDE_CS;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
		case I486_NEEDOPERAND_SEG_OVERRIDE_SS:
			inst.segOverride = SEG_OVERRIDE_SS;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
		case I486_NEEDOPERAND_SEG_OVERRIDE_DS:
			inst.segOverride = SEG_OVERRIDE_DS;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
		case I486_NEEDOPERAND_SEG_OVERRIDE_ES:
			inst.segOverride = SEG_OVERRIDE_ES;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
		case I486_NEEDOPERAND_SEG_OVERRIDE_FS:
			inst.segOverride = SEG_OVERRIDE_FS;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
		case I486_NEEDOPERAND_SEG_OVERRIDE_GS:
			inst.segOverride = SEG_OVERRIDE_GS;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
		case I486_NEEDOPERAND_OPERSIZE_OVERRIDE:
			inst.operandSize = defOperSize ^ 48;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
		case I486_NEEDOPERAND_ADDRSIZE_OVERRIDE:
			inst.addressSize = defAddrSize ^ 48;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
		case I486_NEEDOPERAND_FPU_FWAIT:
			inst.fwait = FPU_FWAIT;
			inst.opCode = FUNCCLASS::FetchInstructionByte(cpu, ptr, inst.codeAddressSize, seg, offset++, mem);
			++inst.numBytes;
			refetch = true;
			break;
			// Handling multi-byte instruction and pre-fixes <<



		default:
#if defined(_MSC_VER)
			__assume(0);
#elif defined(__clang__) || defined(__GNUC__)
			__builtin_unreachable();
#else
			break;
#endif
			// Undefined operand, or probably not implemented yet.
		}

	} while (refetch);

}

#define i486DXFidelityLayer__GetOperandPointerAddr(addrCond) \
		unsigned int offset; \
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride); \
		if(16==addressSize) \
		{ \
			offset&=0xFFFF; \
		} \
		if(seg.limit<offset+3) \
		{ \
			return nullptr; \
		} \
		auto physAddr=seg.baseLinearAddr+offset; \
		auto low12bit=physAddr&0xFFF; \
		if(addrCond) \
		{ \
			if(true==PagingEnabled()) \
			{ \
				if(true==forWrite) \
				{ \
					physAddr=LinearAddressToPhysicalAddressWrite(physAddr,mem); \
				} \
				else \
				{ \
					physAddr=LinearAddressToPhysicalAddressRead(physAddr,mem); \
				} \
			} \
			auto memWin=mem.GetMemoryWindow(physAddr); \
			if(nullptr!=memWin.ptr) \
			{ \
				return memWin.ptr+low12bit; \
			} \
		}


template <class FIDELITY>
inline uint8_t *i486DXFidelityLayer<FIDELITY>::GetOperandPointer(
    Memory &mem,int addressSize,int segmentOverride,const Operand &op,bool forWrite)
{
#ifndef YS_DISABLE_OPERAND_POINTER

#ifdef YS_LITTLE_ENDIAN
	if(OPER_REG32==op.operandType)
	{
		return (uint8_t *)&(state.NULL_and_reg32[op.reg]);
	}
	else if(OPER_REG16==op.operandType)
	{
		return (uint8_t *)&(state.NULL_and_reg32[op.reg-(REG_AX-REG_EAX)]);
	}
	else if(OPER_REG8==op.operandType)
	{
		return (uint8_t *)state.reg8Ptr[op.reg-REG_AL];
	}
#ifdef YS_CPU_DEBUG
	else if(OPER_ADDR!=op.operandType)
	{
		Abort(__FUNCTION__ " is used for a wrong operand type.");
		return nullptr;
	}
#endif // YS_CPU_DEBUG
	else // Assume OPER_ADDR
	{
		i486DXFidelityLayer__GetOperandPointerAddr(low12bit<=0xFFC)
	}
#endif

#endif // YS_DISABLE_OPERAND_POINTER
	return nullptr;
}

template <class FIDELITY>
inline uint8_t *i486DXFidelityLayer<FIDELITY>::GetOperandPointer8(
    Memory &mem,int addressSize,int segmentOverride,const Operand &op,bool forWrite)
{
#ifndef YS_DISABLE_OPERAND_POINTER

#ifdef YS_LITTLE_ENDIAN
	if(OPER_REG8==op.operandType)
	{
		return (uint8_t *)state.reg8Ptr[op.reg-REG_AL];
	}
#ifdef YS_CPU_DEBUG
	else if(OPER_ADDR!=op.operandType)
	{
		Abort(__FUNCTION__ " is used for a wrong operand type.");
		return nullptr;
	}
#endif // YS_CPU_DEBUG
	else // Assume OPER_ADDR
	{
		// Four-byte border check is not needed for an 8-bit address operand
		i486DXFidelityLayer__GetOperandPointerAddr(true)
	}
#endif

#endif // YS_DISABLE_OPERAND_POINTER
	return nullptr;
}

template <class FIDELITY>
inline uint8_t *i486DXFidelityLayer<FIDELITY>::GetOperandPointer16(
    Memory &mem,int addressSize,int segmentOverride,const Operand &op,bool forWrite)
{
#ifndef YS_DISABLE_OPERAND_POINTER

#ifdef YS_LITTLE_ENDIAN
	if(OPER_REG16==op.operandType)
	{
		return (uint8_t *)&(state.NULL_and_reg32[op.reg-(REG_AX-REG_EAX)]);
	}
#ifdef YS_CPU_DEBUG
	else if(OPER_ADDR!=op.operandType)
	{
		Abort(__FUNCTION__ " is used for a wrong operand type.");
		return nullptr;
	}
#endif // YS_CPU_DEBUG
	else // Assume OPER_ADDR
	{
		i486DXFidelityLayer__GetOperandPointerAddr(low12bit<=0xFFE)
	}
#endif

#endif // YS_DISABLE_OPERAND_POINTER
	return nullptr;
}

template <class FIDELITY>
inline uint8_t *i486DXFidelityLayer<FIDELITY>::GetOperandPointer32(
    Memory &mem,int addressSize,int segmentOverride,const Operand &op,bool forWrite)
{
#ifndef YS_DISABLE_OPERAND_POINTER

#ifdef YS_LITTLE_ENDIAN
	if(OPER_REG32==op.operandType)
	{
		return (uint8_t *)&(state.NULL_and_reg32[op.reg]);
	}
#ifdef YS_CPU_DEBUG
	else if(OPER_ADDR!=op.operandType)
	{
		Abort(__FUNCTION__ " is used for a wrong operand type.");
		return nullptr;
	}
#endif // YS_CPU_DEBUG
	else // Assume OPER_ADDR
	{
		i486DXFidelityLayer__GetOperandPointerAddr(low12bit<=0xFFC)
	}
#endif

#endif // YS_DISABLE_OPERAND_POINTER
	return nullptr;
}

template <class FIDELITY>
inline uint8_t *i486DXFidelityLayer<FIDELITY>::GetOperandPointer16or32(
    Memory &mem,int addressSize,int segmentOverride,const Operand &op,bool forWrite)
{
#ifndef YS_DISABLE_OPERAND_POINTER

#ifdef YS_LITTLE_ENDIAN
	if(OPER_REG32==op.operandType)
	{
		return (uint8_t *)&(state.NULL_and_reg32[op.reg]);
	}
	else if(OPER_REG16==op.operandType)
	{
		return (uint8_t *)&(state.NULL_and_reg32[op.reg-(REG_AX-REG_EAX)]);
	}
#ifdef YS_CPU_DEBUG
	else if(OPER_ADDR!=op.operandType)
	{
		Abort(__FUNCTION__ " is used for a wrong operand type.");
		return nullptr;
	}
#endif // YS_CPU_DEBUG
	else // Assume OPER_ADDR
	{
		i486DXFidelityLayer__GetOperandPointerAddr(low12bit<=0xFFC)
	}
#endif

#endif // YS_DISABLE_OPERAND_POINTER
	return nullptr;
}

template <class FIDELITY>
inline unsigned int i486DXFidelityLayer<FIDELITY>::CALLF(Memory &mem,uint16_t opSize,uint16_t instNumBytes,uint16_t newCS,uint32_t newEIP,uint16_t defClocks)
{
	typename FIDELITY::SavedCSEIP savedCSEIP;
	FIDELITY::SaveCSEIP(savedCSEIP,*this);

	auto prevCS=state.CS().value;
	auto returnEIP=state.EIP+instNumBytes;

	if(true==enableCallStack)
	{
		PushCallStack(
		    false,0xffff,0xffff,
		    state.GetCR(0),
		    state.CS().value,state.EIP,instNumBytes,
		    newCS,newEIP,
		    mem);
	}

	if(MODE_NATIVE!=state.mode) // <-> (true==IsInRealMode() || true==GetVM())
	{
		LoadSegmentRegister(state.CS(),newCS,mem);
		state.EIP=newEIP&0xFFFF;
	}
	else
	{
		FIDELITY fidelity;
		unsigned int copyParamCount=0;
		uint32_t copyParams[64]; // Maximum should be 31 parameters.

		auto prevCPL=state.CS().DPL;

		LoadSegmentRegister(state.CS(),newCS,mem);
		if(fidelity.HandleExceptionIfAny(*this,mem,instNumBytes))
		{
			return defClocks;
		}

		auto descType=state.CS().GetType();
		switch(descType)
		{
		case DESC_TYPE_16BIT_CALL_GATE:
		case DESC_TYPE_32BIT_CALL_GATE:
			{
				auto ptr=GetCallGate(copyParamCount,newCS,mem);

				if(0!=copyParamCount)
				{
					std::cout << "Call Gate with parameter count " << copyParamCount << " operand size " << opSize << std::endl;
				}
				if(0!=copyParamCount && 16!=opSize && nullptr!=debuggerPtr)
				{
					std::cout << "I cannot find if the number of bytes copied should differ by operand size, or always times two!!!! WTF!!!!" << std::endl;
					debuggerPtr->ExternalBreak("Call Gate used with non-zero parameter count and 32-bit operand size.");
				}

				copyParamCount=copyParamCount*opSize/16;
				uint32_t ESP=state.ESP();
				if(16==GetStackAddressingSize())
				{
					ESP&=0xFFFF;
				}
				for(int i=0; i<copyParamCount; ++i)
				{
					copyParams[i]=FetchWord(GetStackAddressingSize(),state.SS(),ESP+i*2,mem);
				}
				if(descType==DESC_TYPE_16BIT_CALL_GATE)
				{
					ptr.OFFSET&=0xFFFF;
				}
				LoadSegmentRegister(state.CS(),ptr.SEG,mem);
				state.EIP=ptr.OFFSET;
				defClocks=35;
			}
			break;
		case DESCTYPE_AVAILABLE_286_TSS: //               1,
		case DESCTYPE_BUSY_286_TSS: //                    3,
		case DESCTYPE_TASK_GATE: //                       5,
		case DESCTYPE_AVAILABLE_386_TSS: //               9,
		case DESCTYPE_BUSY_386_TSS: //                 0x0B,
			Abort("Call to Task not supported.");
			break;
		case SEGTYPE_CODE_NONCONFORMING_EXECONLY: //0b11000, // Code Non-Conforming Execute-Only
		case SEGTYPE_CODE_NONCONFORMING_READABLE: //0b11010, // Code Non-Conforming Readable
		case SEGTYPE_CODE_CONFORMING_EXECONLY: //   0b11100, // Code Conforming     Execute-Only
		case SEGTYPE_CODE_CONFORMING_READABLE: //   0b11110, // Code Conforming     Readable
			{
				state.EIP=newEIP;
				if(16==opSize)
				{
					state.EIP&=0xFFFF;
				}
			}
			break;
		default:
			Abort("Unsupported call to descriptor type.");
			break;
		}

		if(state.CS().DPL<prevCPL)
		{
			auto newCPL=state.CS().DPL;
			auto prevSS=state.SS().value;
			auto prevESP=state.ESP();
			if(DESCTYPE_AVAILABLE_286_TSS==state.TR.GetType() ||
			   DESCTYPE_BUSY_286_TSS==state.TR.GetType())
			{
				LoadSegmentRegister(state.SS(),FetchWord(32,state.TR,TSS286_OFFSET_SS0+newCPL*4,mem),mem);
				state.ESP()=FetchWord(32,state.TR,TSS286_OFFSET_SP0+newCPL*4,mem);
			}
			else
			{
				// Assume 386 TSS.
				LoadSegmentRegister(state.SS(),FetchWord(32,state.TR,TSS_OFFSET_SS0+newCPL*8,mem),mem);
				state.ESP()=FetchDword(32,state.TR,TSS_OFFSET_ESP0+newCPL*8,mem);
			}
			Push(mem,opSize,prevSS,prevESP);

			for(int i=0; i<copyParamCount; ++i)
			{
				Push16(mem,copyParams[copyParamCount-1-i]);
			}
		}
	}

	Push(mem,opSize,prevCS,returnEIP);

	FIDELITY::RestoreCSEIPIfException(*this,savedCSEIP);

	return defClocks;
}

template <class FIDELITY>
inline unsigned int i486DXFidelityLayer<FIDELITY>::JMPF(Memory &mem,uint16_t opSize,uint16_t instNumBytes,uint16_t newCS,uint32_t newEIP,uint16_t defClocks)
{
	if(MODE_NATIVE!=state.mode) // <-> (true==IsInRealMode() || true==GetVM())
	{
		LoadSegmentRegister(state.CS(),newCS,mem);
		state.EIP=newEIP&0xFFFF;
	}
	else
	{
		FIDELITY fidelity;

		auto prevCPL=state.CS().DPL;
		auto prevCS=state.CS().value;

		LoadSegmentRegister(state.CS(),newCS,mem);
		if(fidelity.HandleExceptionIfAny(*this,mem,instNumBytes))
		{
			return defClocks;
		}

		auto descType=state.CS().GetType();
		switch(descType)
		{
		case DESC_TYPE_16BIT_CALL_GATE:
		case DESC_TYPE_32BIT_CALL_GATE:
			Abort("JMPF to gate not supported.");
			break;
		case DESCTYPE_AVAILABLE_286_TSS: //               1,
		case DESCTYPE_BUSY_286_TSS: //                    3,
		case DESCTYPE_TASK_GATE: //                       5,
		case DESCTYPE_BUSY_386_TSS: //                 0x0B,
			Abort("JMPF to Task not supported.");
			break;
		case DESCTYPE_AVAILABLE_386_TSS: //               9,
			SwitchTaskToTSS(mem,instNumBytes,prevCS,state.CS(),false);
			break;

		case SEGTYPE_CODE_NONCONFORMING_EXECONLY: //0b11000, // Code Non-Conforming Execute-Only
		case SEGTYPE_CODE_NONCONFORMING_READABLE: //0b11010, // Code Non-Conforming Readable
		case SEGTYPE_CODE_CONFORMING_EXECONLY: //   0b11100, // Code Conforming     Execute-Only
		case SEGTYPE_CODE_CONFORMING_READABLE: //   0b11110, // Code Conforming     Readable
			{
				state.EIP=newEIP;
				if(16==opSize)
				{
					state.EIP&=0xFFFF;
				}
			}
			break;
		default:
			Abort("Unsupported call to descriptor type.");
			break;
		}
	}

	return defClocks;
}

template <class FIDELITY>
void i486DXFidelityLayer<FIDELITY>::SwitchTaskToTSS(Memory &mem,uint32_t instNumBytes,uint16_t prevCS,const SegmentRegister &newTSS,bool nested)
{
	// INTEL 80386 PROGRAMMER'S REFERENCE MANUAL 1986
	// Section 7.5 Page 138

	// 1. Check the current task is allowed to switch.
	//    I'll worry about it later.

	// 2. Check if the next TSS descriptor is marked present and has a valid limit.
	//    I'll worry about it later.

	// 3. Save the state of the current task.
	//      LDT, EFLAGS, EIP, EAX, ECX, EDX, EBX, ESP, EBP, ESI,EDI, ES, CS, SS, DS, FS, GS
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_LDT,state.LDTR.selector);
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_EFLAGS,state.EFLAGS);
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_EIP,state.EIP+instNumBytes);
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_EAX,state.EAX());
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_ECX,state.ECX());
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_EDX,state.EDX());
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_EBX,state.EBX());
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_ESP,state.ESP());
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_EBP,state.EBP());
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_ESI,state.ESI());
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_EDI,state.EDI());
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_ES,state.ES().value);
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_CS,prevCS);
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_SS,state.SS().value);
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_DS,state.DS().value);
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_FS,state.FS().value);
	DebugStoreDword(mem,32,state.TR,TSS_OFFSET_GS,state.GS().value);

	// 4. Load Task Register
	LoadTaskRegister(newTSS.value,mem);

	// 5. Load incoming task's state.
	//      LDT, EFLAGS, EIP, EAX, ECX, EDX, EBX, ESP, EBP, ESI,EDI, ES, CS, SS, DS, FS, GS, PDBR(=CR3)
	InvalidateDescriptorCache();
	auto selector=DebugFetchWord(32,state.TR,TSS_OFFSET_LDT,mem);
	auto TI=(0!=(selector&4));
	state.LDTR.selector=selector;
	if(0==selector)
	{
		state.LDTR.linearBaseAddr=0;
		state.LDTR.limit=0;
	}
	else if(0!=TI) // Pointing LDT
	{
		Abort("Task Switching Failure (LDT)");
	}
	else
	{
		SegmentRegister seg;
		LoadSegmentRegister(seg,selector,mem,MODE_NATIVE); // Force to read from GDT by setting mode=MODE_NATIVE
		const unsigned char byteData[]=
		{
			(unsigned char)( seg.limit    &0xff),
			(unsigned char)((seg.limit>>8)&0xff),
			(unsigned char)( seg.baseLinearAddr     &0xff),
			(unsigned char)((seg.baseLinearAddr>>8) &0xff),
			(unsigned char)((seg.baseLinearAddr>>16)&0xff),
			(unsigned char)((seg.baseLinearAddr>>24)&0xff),
		};
		LoadDescriptorTableRegister(state.LDTR,32,byteData);
	}

	state.EFLAGS=DebugFetchDword(32,state.TR,TSS_OFFSET_EFLAGS,mem);
	state.EIP=DebugFetchDword(32,state.TR,TSS_OFFSET_EIP,mem);
	state.EAX()=DebugFetchDword(32,state.TR,TSS_OFFSET_EAX,mem);
	state.ECX()=DebugFetchDword(32,state.TR,TSS_OFFSET_ECX,mem);
	state.EDX()=DebugFetchDword(32,state.TR,TSS_OFFSET_EDX,mem);
	state.EBX()=DebugFetchDword(32,state.TR,TSS_OFFSET_EBX,mem);
	state.ESP()=DebugFetchDword(32,state.TR,TSS_OFFSET_ESP,mem);
	state.EBP()=DebugFetchDword(32,state.TR,TSS_OFFSET_EBP,mem);
	state.ESI()=DebugFetchDword(32,state.TR,TSS_OFFSET_ESI,mem);
	state.EDI()=DebugFetchDword(32,state.TR,TSS_OFFSET_EDI,mem);

	uint16_t ES=DebugFetchWord(32,state.TR,TSS_OFFSET_ES,mem);
	uint16_t SS=DebugFetchWord(32,state.TR,TSS_OFFSET_SS,mem);
	uint16_t DS=DebugFetchWord(32,state.TR,TSS_OFFSET_DS,mem);
	uint16_t FS=DebugFetchWord(32,state.TR,TSS_OFFSET_FS,mem);
	uint16_t GS=DebugFetchWord(32,state.TR,TSS_OFFSET_GS,mem);
	uint16_t CS=DebugFetchWord(32,state.TR,TSS_OFFSET_CS,mem);

	// Set CS first so that the rest segment selectors can be loaded with the new CPL.
	LoadSegmentRegister(state.CS(),CS,mem,MODE_NATIVE);

	LoadSegmentRegister(state.ES(),ES,mem,MODE_NATIVE);
	LoadSegmentRegister(state.SS(),SS,mem,MODE_NATIVE);
	LoadSegmentRegister(state.DS(),DS,mem,MODE_NATIVE);
	LoadSegmentRegister(state.FS(),FS,mem,MODE_NATIVE);
	LoadSegmentRegister(state.GS(),GS,mem,MODE_NATIVE);
	SetCR(3,DebugFetchDword(32,state.TR,TSS_OFFSET_CR3,mem),mem);
}

template <class FIDELITY>
unsigned int i486DXFidelityLayer<FIDELITY>::RunOneInstruction(Memory &mem,InOut &io)
{
	FIDELITY fidelity;

	// Considered to make it state.EIP=((state.EIP+offset)&operandSizeMask[inst.operandSize>>3]);
	// and delete EIPIncrement=0;  This will save one add and one mov instructions per jump.
	// However, this change may break backward jump to offset 0000H, when operandSize=16.
	// Therefore not taken at this time.
	#define CONDITIONALJUMP8(jumpCond) \
	{ \
		if(true==(jumpCond)) \
		{ \
			auto offset=inst.EvalSimm8(); \
			state.EIP=(state.EIP+offset+inst.numBytes); \
			if(16==inst.operandSize)\
			{\
				state.EIP&=0xFFFF;\
			}\
			clocksPassed=3; \
			EIPIncrement=0; \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
	}

	#define CONDITIONALJUMP16OR32(jumpCond)  \
	{ \
		if(true==(jumpCond)) \
		{ \
			if(16==inst.operandSize) \
			{ \
				auto offset=inst.EvalSimm16(); \
				auto destin=state.EIP+offset+inst.numBytes; \
				state.EIP=(destin&0xFFFF); \
			} \
			else \
			{ \
				auto offset=inst.EvalSimm32(); \
				auto destin=state.EIP+offset+inst.numBytes; \
				state.EIP=destin; \
			} \
			clocksPassed=3; \
			EIPIncrement=0; \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
	}

	#define SETxx(cond) \
	{ \
		OperandValue value; \
		if(true==(cond)) \
		{ \
			value.MakeByte(1); \
			clocksPassed=4; \
		} \
		else \
		{ \
			value.MakeByte(0); \
			clocksPassed=3; \
		} \
		StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value); \
	}

	#define BINARYOP_R_FROM_RM(func16,func32,clock_for_addr,update) \
	{ \
		if(op2.operandType==OPER_ADDR) \
		{ \
			clocksPassed=(clock_for_addr); \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
		auto regNum=inst.GetREG(); \
		if(16==inst.operandSize) \
		{ \
			unsigned int dst=INT_LOW_WORD(state.reg32()[regNum]); \
			auto src=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op2); \
			if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
			{ \
				EIPIncrement=0; \
				break; \
			} \
			(func16)(dst,src); \
			if(true==(update)) \
			{ \
				SET_INT_LOW_WORD(state.reg32()[regNum],dst); \
			} \
		} \
		else \
		{ \
			unsigned int dst=state.reg32()[regNum]; \
			auto src=EvaluateOperandRegOrMem32(mem,inst.addressSize,inst.segOverride,op2); \
			if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
			{ \
				EIPIncrement=0; \
				break; \
			} \
			(func32)(dst,src); \
			if(true==(update)) \
			{ \
				state.reg32()[regNum]=dst; \
			} \
		} \
	}

	#define BINARYOP_RM_FROM_R(func16,func32,clock_for_addr,update) \
	{ \
		if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR) \
		{ \
			clocksPassed=(clock_for_addr); \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
		auto regNum=inst.GetREG(); \
		if(16==inst.operandSize)\
		{\
			auto operPtr=GetOperandPointer16(mem,inst.addressSize,inst.segOverride,op1,update);\
			if(nullptr!=operPtr)\
			{\
				uint32_t i=cpputil::GetWord(operPtr);\
				auto src=INT_LOW_WORD(state.reg32()[regNum]); \
				if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
				{ \
					EIPIncrement=0; \
					break; \
				} \
				(func16)(i,src);\
				if(true==(update))\
				{\
					cpputil::PutWord(operPtr,i);\
				}\
			}\
			else\
			{\
				auto dst=EvaluateOperandReg16OrReg32OrMem(mem,inst.addressSize,inst.segOverride,op1,2); \
				auto src=INT_LOW_WORD(state.reg32()[regNum]); \
				if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
				{ \
					EIPIncrement=0; \
					break; \
				} \
				auto i=dst.GetAsWord(); \
				(func16)(i,src); \
				if(true==(update)) \
				{ \
					dst.SetDword(i); \
					StoreOperandValueReg16OrReg32OrMem(op1,mem,inst.addressSize,inst.segOverride,dst); \
				} \
				if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
				{ \
					EIPIncrement=0; \
					break; \
				} \
			} \
		} \
		else \
		{ \
			auto operPtr=GetOperandPointer32(mem,inst.addressSize,inst.segOverride,op1,update);\
			if(nullptr!=operPtr)\
			{\
				uint32_t i=cpputil::GetDword(operPtr);\
				auto src=state.reg32()[regNum]; \
				if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
				{ \
					EIPIncrement=0; \
					break; \
				} \
				(func32)(i,src);\
				if(true==(update))\
				{\
					cpputil::PutDword(operPtr,i);\
				}\
			}\
			else \
			{\
				auto dst=EvaluateOperandReg16OrReg32OrMem(mem,inst.addressSize,inst.segOverride,op1,4); \
				auto src=state.reg32()[regNum]; \
				if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
				{ \
					EIPIncrement=0; \
					break; \
				} \
				auto i=dst.GetAsDword(); \
				(func32)(i,src); \
				if(true==(update)) \
				{ \
					dst.SetDword(i); \
					StoreOperandValueReg16OrReg32OrMem(op1,mem,inst.addressSize,inst.segOverride,dst); \
				} \
				if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
				{ \
					EIPIncrement=0; \
					break; \
				} \
			} \
		} \
	} \

	#define BINARYOP_RM8_R8(func,clock_for_addr,update) \
	{ \
		if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR) \
		{ \
			clocksPassed=(clock_for_addr); \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
		unsigned int reg=REG_AL+inst.GetREG(); \
		auto operPtr=GetOperandPointer8(mem,inst.addressSize,inst.segOverride,op1,update); \
		if(nullptr!=operPtr) \
		{ \
			auto value2=GetRegisterValue8(reg); \
			if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
			{ \
				EIPIncrement=0; \
				break; \
			} \
			uint32_t i=*operPtr; \
			(func)(i,value2); \
			if(true==update) \
			{ \
				*operPtr=i; \
			} \
		} \
		else \
		{ \
			uint32_t value1=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1); \
			auto value2=GetRegisterValue8(reg); \
			if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
			{ \
				EIPIncrement=0; \
				break; \
			} \
			(func)(value1,value2); \
			if(true==update) \
			{ \
				StoreOperandValueRegOrMem8(op1,mem,inst.addressSize,inst.segOverride,cpputil::LowByte(value1)); \
			} \
			if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
			{ \
				EIPIncrement=0; \
				break; \
			} \
		} \
	}

	#define BINARYOP_R8_RM8(func,clock_for_addr,update) \
	{ \
		if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR) \
		{ \
			clocksPassed=(clock_for_addr); \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
		unsigned int reg=REG_AL+inst.GetREG(); \
		auto value1=GetRegisterValue8(reg); \
		auto value2=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op2); \
		if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
		{ \
			EIPIncrement=0; \
			break; \
		} \
		(func)(value1,value2); \
		if(true==update) \
		{ \
			SetRegisterValue8(reg,value1); \
		} \
	}

	#define BINARYOP_AL_I8(func,update) \
	{ \
		clocksPassed=1; \
		auto al=GetAL(); \
		auto v=inst.EvalUimm8(); \
		(func)(al,v); \
		if(true==update) \
		{ \
			SetAL(al); \
		} \
	}

	#define BINARYOP_xAX_I(func16,func32,update) \
	{ \
		clocksPassed=1; \
		if(16==inst.operandSize) \
		{ \
			auto ax=GetAX(); \
			auto v=inst.EvalUimm16(); \
			(func16)(ax,v); \
			if(true==update) \
			{ \
				SetAX(ax); \
			} \
		} \
		else \
		{ \
			auto eax=GetEAX(); \
			auto v=inst.EvalUimm32(); \
			(func32)(eax,v); \
			if(true==update) \
			{ \
				SetEAX(eax); \
			} \
		} \
	}

	// For RCL and RCR see reminder #20200123-1
	#define ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8(ctr) \
	{ \
		auto operPtr=GetOperandPointer8(mem,inst.addressSize,inst.segOverride,op1,true); \
		if(nullptr!=operPtr) \
		{ \
			uint32_t i=*operPtr; \
			if(true!=state.exception) \
			{ \
				switch(inst.GetREG()) \
				{ \
				case 0: \
					RolByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					break; \
				case 1: \
					RorByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					break; \
				case 2: \
					RclByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
					break; \
				case 3: \
					RcrByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
					break; \
				case 4: \
					ShlByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					break; \
				case 5: \
					ShrByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					break; \
				case 6: \
					Abort("Undefined REG for "+cpputil::Ustox(inst.RealOpCode())); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					return 0; \
				case 7: \
					SarByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					break; \
				default: \
					std_unreachable; \
				} \
				*operPtr=i; \
			} \
		} \
		else {\
			unsigned int i=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1); \
			if(true!=state.exception) \
			{ \
				switch(inst.GetREG()) \
				{ \
				case 0: \
					RolByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					break; \
				case 1: \
					RorByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					break; \
				case 2: \
					RclByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
					break; \
				case 3: \
					RcrByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
					break; \
				case 4: \
					ShlByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					break; \
				case 5: \
					ShrByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					break; \
				case 6: \
					Abort("Undefined REG for "+cpputil::Ustox(inst.RealOpCode())); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					return 0; \
				case 7: \
					SarByte(i,ctr); \
					clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
					break; \
				default: \
					std_unreachable; \
				} \
				StoreOperandValueRegOrMem8(op1,mem,inst.addressSize,inst.segOverride,cpputil::LowByte(i)); \
			} \
		}\
	}

	// For RCL and RCR See reminder #20200123-1
	#define ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM(ctr) \
	{ \
		if(16==inst.operandSize)\
		{\
			auto operPtr=GetOperandPointer16(mem,inst.addressSize,inst.segOverride,op1,true); \
			if(nullptr!=operPtr) \
			{ \
				if(true!=state.exception) \
				{ \
					uint32_t i;\
					i=cpputil::GetWord(operPtr);\
					switch(inst.GetREG()) \
					{ \
					case 0: \
						RolWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 1: \
						RorWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 2: \
						RclWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
						break; \
					case 3: \
						clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
						RcrWord(i,ctr); \
						break; \
					case 4: \
						ShlWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 5: \
						ShrWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 6: \
						Abort("Undefined REG for "+cpputil::Ustox(inst.RealOpCode())); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						return 0; \
					case 7: \
						SarWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					default: \
						std_unreachable; \
					} \
					cpputil::PutWord(operPtr,i);\
				} \
			} \
			else \
			{\
				auto value=EvaluateOperandReg16OrReg32OrMem(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8); \
				auto i=value.GetAsWord(); \
				if(true!=state.exception) \
				{ \
					switch(inst.GetREG()) \
					{ \
					case 0: \
						RolWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 1: \
						RorWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 2: \
						RclWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
						break; \
					case 3: \
						clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
						RcrWord(i,ctr); \
						break; \
					case 4: \
						ShlWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 5: \
						ShrWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 6: \
						Abort("Undefined REG for "+cpputil::Ustox(inst.RealOpCode())); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						return 0; \
					case 7: \
						SarWord(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					default: \
						std_unreachable; \
					} \
					value.SetWord(i); \
					StoreOperandValueReg16OrReg32OrMem(op1,mem,inst.addressSize,inst.segOverride,value); \
				} \
			} \
		}\
		else \
		{\
			auto operPtr=GetOperandPointer32(mem,inst.addressSize,inst.segOverride,op1,true); \
			if(nullptr!=operPtr) \
			{ \
				if(true!=state.exception) \
				{ \
					uint32_t i;\
					i=cpputil::GetDword(operPtr);\
					switch(inst.GetREG()) \
					{ \
					case 0: \
						RolDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 1: \
						RorDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 2: \
						RclDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
						break; \
					case 3: \
						clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
						RcrDword(i,ctr); \
						break; \
					case 4: \
						ShlDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 5: \
						ShrDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 6: \
						Abort("Undefined REG for "+cpputil::Ustox(inst.RealOpCode())); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						return 0; \
					case 7: \
						SarDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					default: \
						std_unreachable; \
					} \
					cpputil::PutDword(operPtr,i);\
				} \
			} \
			else \
			{\
				auto value=EvaluateOperandReg16OrReg32OrMem(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8); \
				auto i=value.GetAsDword(); \
				if(true!=state.exception) \
				{ \
					switch(inst.GetREG()) \
					{ \
					case 0: \
						RolDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 1: \
						RorDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 2: \
						RclDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
						break; \
					case 3: \
						clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
						RcrDword(i,ctr); \
						break; \
					case 4: \
						ShlDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 5: \
						ShrDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					case 6: \
						Abort("Undefined REG for "+cpputil::Ustox(inst.RealOpCode())); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						return 0; \
					case 7: \
						SarDword(i,ctr); \
						clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
						break; \
					default: \
						std_unreachable; \
					} \
					value.SetDword(i); \
					StoreOperandValueReg16OrReg32OrMem(op1,mem,inst.addressSize,inst.segOverride,value); \
				} \
			} \
		}\
		if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
		{ \
			clocksPassed=35; \
			EIPIncrement=0; \
			break; \
		} \
	}

	// op1 is a register.
	#define LOAD_FAR_POINTER(SEGREG) \
		if(OPER_ADDR==op2.operandType) \
		{ \
			clocksPassed=9; \
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,(inst.operandSize+16)/8); \
			if(true!=state.exception) \
			{ \
				auto seg=value.GetFwordSegment(); \
				LoadSegmentRegister(state.SEGREG(),seg,mem); \
				if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
				{ \
					EIPIncrement=0; \
					break; \
				} \
				auto regNum=inst.GetREG(); \
				if(16==inst.operandSize) \
				{ \
					SET_INT_LOW_WORD(state.reg32()[regNum],value.GetAsWord()); \
				} \
				else \
				{ \
					state.reg32()[regNum]=value.GetAsDword(); \
				} \
			} \
		} \
		else \
		{ \
			RaiseException(EXCEPTION_GP,0); \
		}

	#define MOV_REG32_FROM_I(REG) \
		{ \
			if(16==inst.operandSize)\
			{\
				SET_INT_LOW_WORD(state.NULL_and_reg32[REG],inst.EvalUimm16());\
			}\
			else\
			{\
				state.NULL_and_reg32[REG]=inst.EvalUimm32();\
			}\
			clocksPassed=1; \
		}

	#define HANDLE_EXCEPTION_IF_ANY \
		if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes)) \
		{ \
			EIPIncrement=0; \
			clocksPassed+=ClocksForHandlingException(); \
			break; \
		}

	// Use the following as a pair.
	#define SAVE_ESP_BEFORE_PUSH_POP \
		typename FIDELITY::SavedESP savedESP; \
		FIDELITY::SaveESP(savedESP,inst.addressSize,state.ESP());

	#define HANDLE_EXCEPTION_PUSH_POP \
		if(true==fidelity.HandleExceptionAndRestoreESPIfAny(*this,mem,inst.numBytes,savedESP)) \
		{ \
			clocksPassed+=ClocksForHandlingException(); \
			EIPIncrement=0; \
			break; \
		}

	// Use the following as a pair.
	#define SAVE_ESPEBP_BEFORE_ENTER \
		typename FIDELITY::SavedESPEBP saved; \
		FIDELITY::SaveESPEBP(saved,inst.addressSize,state.ESP(),state.EBP());

	#define HANDLE_EXCEPTION_ENTER \
		if(true==fidelity.HandleExceptionAndRestoreESPEBPIfAny(*this,mem,inst.numBytes,saved)) \
		{ \
			EIPIncrement=0; \
			clocksPassed+=ClocksForHandlingException(); \
			break; \
		}
	#define HANDLE_EXCEPTION_ENTER_INSIDE \
		if(true==fidelity.HandleExceptionAndRestoreESPEBPIfAny(*this,mem,inst.numBytes,saved)) \
		{ \
			EIPIncrement=0; \
			clocksPassed+=ClocksForHandlingException(); \
			goto BREAK_ENTER; \
		}

	// Use the following as a pair.
	#define SAVE_ECX_BEFORE_STRINGOP \
		typename FIDELITY::SavedECX savedECX; \
		FIDELITY::SaveECX(savedECX,state.ECX());

	#define HANDLE_EXCEPTION_STRINGOP \
		if(true==fidelity.HandleExceptionAndRestoreECXIfAny(*this,mem,inst.numBytes,savedECX)) \
		{ \
			EIPIncrement=0; \
			clocksPassed+=ClocksForHandlingException(); \
			break; \
		}

	#define UPDATED_SAVED_ECX_AFTER_STRINGOP \
		FIDELITY::SaveECX(savedECX,state.ECX());

	#define FPU_TRAP \
		if(true!=state.fpuState.enabled) \
		{ \
			Interrupt(INT_DEVICE_NOT_AVAILABLE,mem,0,0,false); \
			clocksPassed=ClocksForHandlingException(); \
			EIPIncrement=0; \
			break; \
		}


	static const unsigned int reg8AndPattern[]=
	{
		0xFFFFFF00,   // AL
		0xFFFFFF00,   // CL
		0xFFFFFF00,   // DL
		0xFFFFFF00,   // BL
		0xFFFF00FF,   // AH
		0xFFFF00FF,   // CH
		0xFFFF00FF,   // DH
		0xFFFF00FF,   // BH
	};
	static const unsigned char reg8Shift[]=
	{
		0,   // AL
		0,   // CL
		0,   // DL
		0,   // BL
		8,   // AH
		8,   // CH
		8,   // DH
		8,   // BH
	};



	if(true==state.halt)
	{
		return 1;
	}

	state.holdIRQ=false;

	InstructionAndOperand instOp;
	const auto &inst=instOp.inst;
	const auto &op1=instOp.op1;
	const auto &op2=instOp.op2;
	FetchInstruction(state.CSEIPWindow,instOp,state.CS(),state.EIP,mem);
	if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
	{
		return ClocksForHandlingException();
	}

	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->BeforeRunOneInstruction(*this,mem,io,inst);
		fidelity.BeforeRunOneInstruction(*this,inst,debuggerPtr);
	}

	if(true==fidelity.LockNotAllowed(*this,mem,inst,op1))
	{
		return ClocksForHandlingException();
	}

	int EIPIncrement=inst.numBytes;
	unsigned int clocksPassed=0;

	switch(opCodeRenumberTable[inst.opCode])
	{
	case I486_RENUMBER_UNDEFINED_SHOOT_INT6:
	case I486_RENUMBER_UNDEFINED_SHOOT_INT6_WIN31:
	case I486_RENUMBER_EMMS: // 0x0F77 MMX instruction.  Used in the Win95 Media-Player Installer.
		Interrupt(INT_INVALID_OPCODE,mem,0,0,false);
		EIPIncrement=0;
		clocksPassed=ClocksForHandlingException();
		break;


	case I486_RENUMBER_REALLY_UNDEFINED:
		std::cout << "Undefined instruction (" << cpputil::Ustox(inst.RealOpCode()) << ") at " << cpputil::Ustox(state.CS().value) << ":" << cpputil::Uitox(state.EIP) << "\n";
		Interrupt(INT_INVALID_OPCODE,mem,0,0,false);
		EIPIncrement=0;
		clocksPassed=ClocksForHandlingException();
		// clocksPassed=0; // Uncomment this line to abort on undefined instruction.
		break;


	case I486_RENUMBER_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8://0xC0,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8(inst.EvalUimm8()&31); // [1] pp.26-243 Only bottom 5 bits are used.
		break;
	case I486_RENUMBER_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1://0xD0, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8(1)
		break;
	case I486_RENUMBER_D2_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL://0xD2,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8(GetCL()&31)
		break;

	case I486_RENUMBER_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8:// =0xC1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM(inst.EvalUimm8()&31); // [1] pp.26-243 Only bottom 5 bits are used.
		break;
	case I486_RENUMBER_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1://=0xD1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM(1);
		break;
	case I486_RENUMBER_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL://0xD3, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM(GetCL()&31);
		break;

	case I486_RENUMBER_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF6
		switch(inst.GetREG())
		{
		case 0: // TEST
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 2 : 1);
				uint32_t byte=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1);
				HANDLE_EXCEPTION_IF_ANY;
				AndByte(byte,inst.EvalUimm8());
				// SetCF(false); Done in AndByte
				// SetOF(false); Done in AndByte
			}
			break;
		case 2: // NOT
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 1);
				auto operPtr=GetOperandPointer8(mem,inst.addressSize,inst.segOverride,op1,true);
				if(nullptr!=operPtr)
				{
					if(true!=state.exception)
					{
						operPtr[0]=~operPtr[0];
					}
				}
				else
				{
					auto value=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1);
					HANDLE_EXCEPTION_IF_ANY;
					if(true!=state.exception)
					{
						value=~value;
						StoreOperandValueRegOrMem8(op1,mem,inst.addressSize,inst.segOverride,value);
					}
				}
			}
			break;
		case 3: // NEG
			{
				clocksPassed = (OPER_ADDR == op1.operandType ? 3 : 1);
				auto operPtr=GetOperandPointer8(mem, inst.addressSize, inst.segOverride, op1,true);
				if(nullptr!=operPtr)
				{
					uint32_t r=0;
					uint32_t i=operPtr[0];
					SubByte(r,i);
					operPtr[0]=r;
				}
				else
				{
					auto value1 = EvaluateOperandRegOrMem8(mem, inst.addressSize, inst.segOverride, op1);
					HANDLE_EXCEPTION_IF_ANY;
					uint32_t r = 0;
					SubByte(r, value1);
					StoreOperandValueRegOrMem8(op1, mem, inst.addressSize, inst.segOverride, r);
				}
			}
			break;
		case 4: // MUL
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 18 : 13);
				uint8_t value=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1);
				HANDLE_EXCEPTION_IF_ANY;
				auto mul=GetAL()*value;
				SetAX(mul);
				if(0!=cpputil::GetWordHighByte(mul))
				{
					SetCFOF();
					//SetCF(true);
					//SetOF(true);
				}
				else
				{
					ClearCFOF();
					//SetCF(false);
					//SetOF(false);
				}
			}
			break;
		case 5: // IMUL R/M8
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 18 : 13);
				int OP=cpputil::ByteToSigned32(EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1));
				HANDLE_EXCEPTION_IF_ANY;
				if(true!=state.exception)
				{
					int AL=cpputil::ByteToSigned32(GetAL());
					auto imul=AL*OP;
					SetAX(cpputil::LowWord(imul));
					if(0==(imul&0xFF80) || 0xFF80==(imul&0xFF80))
					{
						ClearCFOF();
						//SetCF(false);
						//SetOF(false);
					}
					else
					{
						SetCFOF();
						//SetCF(true);
						//SetOF(true);
					}
				}
			}
			break;
		case 6: // DIV
			{
				clocksPassed=16;
				uint16_t value=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1);
				HANDLE_EXCEPTION_IF_ANY;
				if(0==value)
				{
					Interrupt(0,mem,0,0,false); // [1] pp.26-28
					EIPIncrement=0;
					// I don't think INT 0 was issued unless division by zero.
					// I thought it just overflew if quo didn't fit in the target register, am I wrong?
				}
				else
				{
					uint16_t AX=GetAX();
					uint16_t quo=AX/value;
					uint16_t rem=AX%value;
					SetAL(quo);
					SetAH(rem);
				}
			}
			break;
		case 7: // IDIV
			{
				clocksPassed=20;
				auto value=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1);
				HANDLE_EXCEPTION_IF_ANY;
				if(0==value)
				{
					Interrupt(0,mem,0,0,false); // [1] pp.26-28
					EIPIncrement=0;
					// I don't think INT 0 was issued unless division by zero.
					// I thought it just overflew if quo didn't fit in the target register, am I wrong?
				}
				else
				{
					int16_t ax=cpputil::WordToSigned32(GetAX());
					int16_t rm8=cpputil::ByteToSigned32(value);
					int quo=ax/rm8;
					int rem=ax%rm8;
					quo=(quo+0x10000)&0xff;
					rem=(rem+0x10000)&0xff;
					SetAL(quo);
					SetAH(rem);
				}
			}
			break;
		case 1:
			{
				std::string msg;
				msg="Undefined REG for ";
				msg+=cpputil::Ubtox(inst.opCode);
				msg+="(REG=";
				msg+=cpputil::Ubtox(inst.GetREG());
				msg+=")";
				Abort(msg);
				return 0;
			}
			clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
			break;
		default:
			std_unreachable;
		}
		break;
	case I486_RENUMBER_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF7,
		switch(inst.GetREG())
		{
		case 0: // TEST
			clocksPassed=(OPER_ADDR==op1.operandType ? 2 : 1);
			if(16==inst.operandSize)
			{
				uint32_t value1=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op1);
				uint32_t value2=inst.EvalUimm16();
				HANDLE_EXCEPTION_IF_ANY;
				AndWord(value1,value2);
				// SetCF(false); Done in AndWordOrDword
				// SetOF(false);
			}
			else
			{
				auto value1=EvaluateOperandRegOrMem32(mem,inst.addressSize,inst.segOverride,op1);
				HANDLE_EXCEPTION_IF_ANY;
				auto value2=inst.EvalUimm32();
				AndDword(value1,value2);
				// SetCF(false); Done in AndWordOrDword
				// SetOF(false);
			}
			break;
		case 2: // NOT
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 2 : 1);
				auto operPtr=GetOperandPointer16or32(mem,inst.addressSize,inst.segOverride,op1,true);
				if(nullptr!=operPtr)
				{
					HANDLE_EXCEPTION_IF_ANY;
					operPtr[0]=~operPtr[0];
					operPtr[1]=~operPtr[1];
					if(32==inst.operandSize)
					{
						operPtr[2]=~operPtr[2];
						operPtr[3]=~operPtr[3];
					}
				}
				else
				{
					auto value1=EvaluateOperandReg16OrReg32OrMem(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					HANDLE_EXCEPTION_IF_ANY;
					value1.byteData[0]=~value1.byteData[0];
					value1.byteData[1]=~value1.byteData[1];
					value1.byteData[2]=~value1.byteData[2];
					value1.byteData[3]=~value1.byteData[3];
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
				}
			}
			break;
		case 3: // NEG
			{
				clocksPassed = (OPER_ADDR == op1.operandType ? 3 : 1);
				auto operPtr=GetOperandPointer16or32(mem, inst.addressSize, inst.segOverride, op1,true);
				if(nullptr!=operPtr)
				{
					HANDLE_EXCEPTION_IF_ANY;
					uint32_t r=0;
					if(16==inst.operandSize)
					{
						SubWord(r,cpputil::GetWord(operPtr));
						cpputil::PutWord(operPtr,r);
					}
					else
					{
						SubDword(r,cpputil::GetDword(operPtr));
						cpputil::PutDword(operPtr,r);
					}
				}
				else
				{
					auto value1 = EvaluateOperandReg16OrReg32OrMem(mem, inst.addressSize, inst.segOverride, op1, inst.operandSize / 8);
					HANDLE_EXCEPTION_IF_ANY;
					uint32_t r = 0;
					uint32_t i = value1.GetAsDword();
					SubWordOrDword(inst.operandSize, r, i);
					value1.SetDword(r);
					StoreOperandValue(op1, mem, inst.addressSize, inst.segOverride, value1);
				}
			}
			break;
		case 4: // MUL
			if(16==inst.operandSize)
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 26 : 13);
				auto value=EvaluateOperandReg16OrReg32OrMem(mem,inst.addressSize,inst.segOverride,op1,2);
				auto DXAX=GetAX()*value.GetAsWord();
				SetAX(cpputil::LowWord(DXAX));
				SetDX(cpputil::HighWord(DXAX));
				if(0!=(DXAX&0xffff0000))
				{
					SetCFOF();
					//SetCF(true);
					//SetOF(true);
				}
				else
				{
					ClearCFOF();
					//SetCF(false);
					//SetOF(false);
				}
			}
			else
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 42 : 13);
				auto value=EvaluateOperandReg16OrReg32OrMem(mem,inst.addressSize,inst.segOverride,op1,4);
				uint64_t EAX=GetEAX();
				uint64_t MUL=value.GetAsDword();
				uint64_t EDXEAX=EAX*MUL;
				SetEAX(cpputil::LowDword(EDXEAX));
				SetEDX(cpputil::HighDword(EDXEAX));
				if(0!=(EDXEAX&0xffffffff00000000))
				{
					SetCFOF();
					//SetCF(true);
					//SetOF(true);
				}
				else
				{
					ClearCFOF();
					//SetCF(false);
					//SetOF(false);
				}
			}
			break;
		case 5: // IMUL
			{
				auto value=EvaluateOperandReg16OrReg32OrMem(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				if(true==state.exception)
				{
					EIPIncrement=0;
				}
				else
				{
					int multiplicand=value.GetAsSignedDword();
					if(16==inst.operandSize)
					{
						clocksPassed=20; // 13-26.  I don't know exactly how to calculate it.
						int DXAX=cpputil::WordToSigned32(GetAX());
						DXAX*=multiplicand;

						SetAX(cpputil::LowWord(DXAX));
						SetDX(cpputil::HighWord(DXAX));

						auto signExtCheck=DXAX&0xFFFF8000;
						if(0==signExtCheck || signExtCheck==0xFFFF8000)
						{
							ClearCFOF();
							//SetOF(false);
							//SetCF(false);
						}
						else
						{
							SetCFOF();
							//SetOF(true);
							//SetCF(true);
						}
					}
					else
					{
						clocksPassed=30; // 13-42.  I don't know exactly how to calculate it.
						int64_t EDXEAX=cpputil::DwordToSigned64(GetEAX());
						EDXEAX*=(int64_t)multiplicand;

						SetEAX(cpputil::LowDword(EDXEAX));
						SetEDX(cpputil::HighDword(EDXEAX));

						auto signExtCheck=EDXEAX&0xFFFFFFFF80000000LL;
						if(0==signExtCheck || signExtCheck==0xFFFFFFFF80000000LL)
						{
							ClearCFOF();
							//SetOF(false);
							//SetCF(false);
						}
						else
						{
							SetCFOF();
							//SetOF(true);
							//SetCF(true);
						}
					}
				}
			}
			break;
		case 6: // DIV
			{
				auto value=EvaluateOperandReg16OrReg32OrMem(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				unsigned int denom=value.GetAsDword();
				if(true==state.exception)
				{
					clocksPassed=40;
					EIPIncrement=0;
					HANDLE_EXCEPTION_IF_ANY;
				}
				else if(0==denom)
				{
					clocksPassed=40;
					Interrupt(0,mem,0,0,false); // [1] pp.26-28
					EIPIncrement=0;
					// I don't think INT 0 was issued unless division by zero.
					// I thought it just overflew if quo didn't fit in the target register, am I wrong?
				}
				else if(16==inst.operandSize)
				{
					clocksPassed=24;
					unsigned int DXAX=cpputil::WordPairToUnsigned32(GetAX(),GetDX());
					unsigned int quo=DXAX/denom;
					unsigned int rem=DXAX%denom;
					SetAX(quo);
					SetDX(rem);
				}
				else if(32==inst.operandSize)
				{
					clocksPassed=40;
					unsigned long long int EDXEAX=cpputil::DwordPairToUnsigned64(GetEAX(),GetEDX());
					unsigned int quo=(unsigned int)(EDXEAX/denom);
					unsigned int rem=(unsigned int)(EDXEAX%denom);
					SetEAX(quo);
					SetEDX(rem);
				}
			}
			break;
		case 7: // IDIV
			{
				auto value=EvaluateOperandReg16OrReg32OrMem(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				int denom=value.GetAsSignedDword();
				if(true==state.exception)
				{
					clocksPassed=40;
					EIPIncrement=0;
					HANDLE_EXCEPTION_IF_ANY;
				}
				else if(0==denom)
				{
					clocksPassed=40;
					Interrupt(0,mem,0,0,false); // [1] pp.26-28
					EIPIncrement=0;
					// I don't think INT 0 was issued unless division by zero.
					// I thought it just overflew if quo didn't fit in the target register, am I wrong?
				}
				else if(16==inst.operandSize)
				{
					clocksPassed=24;

					int32_t DXAX=cpputil::WordPairToSigned32(GetAX(),GetDX());

					int quo=DXAX/denom;
					int rem=DXAX%denom;

					SetAX(cpputil::LowWord(quo));
					SetDX(cpputil::LowWord(rem));
				}
				else if(32==inst.operandSize)
				{
					clocksPassed=40;

					int64_t EDXEAX=cpputil::DwordPairToSigned64(GetEAX(),GetEDX());

					int64_t quo=EDXEAX/denom;
					int64_t rem=EDXEAX%denom;

					SetEAX(cpputil::LowDword(quo));
					SetEDX(cpputil::LowDword(rem));
				}
			}
			break;
		case 1:
			Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
			clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
			return 0;
		default:
			std_unreachable;
		}
		break;

	case I486_RENUMBER_ADC_AL_FROM_I8://  0x14,
		BINARYOP_AL_I8(AdcByte,true);
		break;
	case I486_RENUMBER_ADD_AL_FROM_I8://  0x04,
		BINARYOP_AL_I8(AddByte,true);
		break;
	case I486_RENUMBER_AND_AL_FROM_I8://  0x24,
		BINARYOP_AL_I8(AndByte,true);
		break;
	case I486_RENUMBER_CMP_AL_FROM_I8://  0x3C,
		BINARYOP_AL_I8(SubByte,false);
		break;
	case I486_RENUMBER_OR_AL_FROM_I8://  0x0C,
		BINARYOP_AL_I8(OrByte,true);
		break;
	case I486_RENUMBER_SBB_AL_FROM_I8://  0x1C,
		BINARYOP_AL_I8(SbbByte,true);
		break;
	case I486_RENUMBER_SUB_AL_FROM_I8://  0x2C,
		BINARYOP_AL_I8(SubByte,true);
		break;
	case I486_RENUMBER_TEST_AL_FROM_I8://  0xA8,
		BINARYOP_AL_I8(AndByte,false);
		break;
	case I486_RENUMBER_XOR_AL_FROM_I8:
		BINARYOP_AL_I8(XorByte,true);
		break;

	case I486_RENUMBER_ADC_A_FROM_I://    0x15,
		BINARYOP_xAX_I(AdcWord,AdcDword,true);
		break;
	case I486_RENUMBER_ADD_A_FROM_I://    0x05,
		BINARYOP_xAX_I(AddWord,AddDword,true);
		break;
	case I486_RENUMBER_AND_A_FROM_I://    0x25,
		BINARYOP_xAX_I(AndWord,AndDword,true);
		break;
	case I486_RENUMBER_CMP_A_FROM_I://    0x3D,
		BINARYOP_xAX_I(SubWord,SubDword,false);
		break;
	case I486_RENUMBER_OR_A_FROM_I://    0x0D,
		BINARYOP_xAX_I(OrWord,OrDword,true);
		break;
	case I486_RENUMBER_SBB_A_FROM_I://    0x1D,
		BINARYOP_xAX_I(SbbWord,SbbDword,true);
		break;
	case I486_RENUMBER_SUB_A_FROM_I://    0x2D,
		BINARYOP_xAX_I(SubWord,SubDword,true);
		break;
	case I486_RENUMBER_TEST_A_FROM_I://    0xA9,
		BINARYOP_xAX_I(AndWord,AndDword,false);
		break;
	case I486_RENUMBER_XOR_A_FROM_I:
		BINARYOP_xAX_I(XorWord,XorDword,true);
		break;

	case I486_RENUMBER_ADC_RM8_FROM_R8:// 0x10,
		BINARYOP_RM8_R8(AdcByte,3,true);
		break;
	case I486_RENUMBER_ADD_RM8_FROM_R8:// 0x00,
		BINARYOP_RM8_R8(AddByte,3,true);
		break;
	case I486_RENUMBER_AND_RM8_FROM_R8:// 0x20,
		BINARYOP_RM8_R8(AndByte,3,true);
		break;
	case I486_RENUMBER_CMP_RM8_FROM_R8:// 0x38,
		BINARYOP_RM8_R8(SubByte,3,false);
		break;
	case I486_RENUMBER_OR_RM8_FROM_R8:// 0x08,
		BINARYOP_RM8_R8(OrByte,3,true);
		break;
	case I486_RENUMBER_SBB_RM8_FROM_R8:// 0x18,
		BINARYOP_RM8_R8(SbbByte,3,true);
		break;
	case I486_RENUMBER_SUB_RM8_FROM_R8:// 0x28,
		BINARYOP_RM8_R8(SubByte,3,true);
		break;
	case I486_RENUMBER_XOR_RM8_FROM_R8:
		BINARYOP_RM8_R8(XorByte,3,true);
		break;
	case I486_RENUMBER_TEST_RM8_FROM_R8:// 0x84,
		BINARYOP_RM8_R8(AndByte,2,false);
		break;

	case I486_RENUMBER_ADC_R8_FROM_RM8:// 0x12,
		BINARYOP_R8_RM8(AdcByte,3,true);
		break;
	case I486_RENUMBER_ADD_R8_FROM_RM8:// 0x02,
		BINARYOP_R8_RM8(AddByte,3,true);
		break;
	case I486_RENUMBER_AND_R8_FROM_RM8:// 0x22,
		BINARYOP_R8_RM8(AndByte,3,true);
		break;
	case I486_RENUMBER_CMP_R8_FROM_RM8:// 0x3A,
		BINARYOP_R8_RM8(SubByte,3,false);
		break;
	case I486_RENUMBER_OR_R8_FROM_RM8:// 0x0A,
		BINARYOP_R8_RM8(OrByte,3,true);
		break;
	case I486_RENUMBER_SBB_R8_FROM_RM8:// 0x1A,
		BINARYOP_R8_RM8(SbbByte,3,true);
		break;
	case I486_RENUMBER_SUB_R8_FROM_RM8:// 0x2A,
		BINARYOP_R8_RM8(SubByte,3,true);
		break;
	case I486_RENUMBER_XOR_R8_FROM_RM8:
		BINARYOP_R8_RM8(XorByte,3,true);
		break;

	case I486_RENUMBER_ADC_RM_FROM_R://   0x11,
		BINARYOP_RM_FROM_R(AdcWord,AdcDword,3,true);
		break;
	case I486_RENUMBER_ADD_RM_FROM_R://   0x01,
		BINARYOP_RM_FROM_R(AddWord,AddDword,3,true);
		break;
	case I486_RENUMBER_AND_RM_FROM_R://   0x21,
		BINARYOP_RM_FROM_R(AndWord,AndDword,3,true);
		break;
	case I486_RENUMBER_CMP_RM_FROM_R://   0x39,
		BINARYOP_RM_FROM_R(SubWord,SubDword,3,false);
		break;
	case I486_RENUMBER_SBB_RM_FROM_R://   0x19,
		BINARYOP_RM_FROM_R(SbbWord,SbbDword,3,true);
		break;
	case I486_RENUMBER_SUB_RM_FROM_R://   0x29,
		BINARYOP_RM_FROM_R(SubWord,SubDword,3,true);
		break;
	case I486_RENUMBER_TEST_RM_FROM_R://   0x85,
		BINARYOP_RM_FROM_R(AndWord,AndDword,1,false);
		break;
	case I486_RENUMBER_OR_RM_FROM_R://   0x09,
		BINARYOP_RM_FROM_R(OrWord,OrDword,3,true);
		break;
	case I486_RENUMBER_XOR_RM_FROM_R:
		BINARYOP_RM_FROM_R(XorWord,XorDword,3,true);
		break;

	case I486_RENUMBER_ADC_R_FROM_RM://   0x13,
		BINARYOP_R_FROM_RM(AdcWord,AdcDword,3,true);
		break;
	case I486_RENUMBER_ADD_R_FROM_RM://    0x03,
		BINARYOP_R_FROM_RM(AddWord,AddDword,3,true);
		break;
	case I486_RENUMBER_AND_R_FROM_RM://    0x23,
		BINARYOP_R_FROM_RM(AndWord,AndDword,3,true);
		break;
	case I486_RENUMBER_CMP_R_FROM_RM://    0x3B,
		BINARYOP_R_FROM_RM(SubWord,SubDword,3,false);
		break;
	case I486_RENUMBER_SBB_R_FROM_RM://    0x1B,
		BINARYOP_R_FROM_RM(SbbWord,SbbDword,3,true);
		break;
	case I486_RENUMBER_SUB_R_FROM_RM://    0x2B,
		BINARYOP_R_FROM_RM(SubWord,SubDword,3,true);
		break;
	case I486_RENUMBER_OR_R_FROM_RM://    0x0B,
		BINARYOP_R_FROM_RM(OrWord,OrDword,3,true);
		break;
	case I486_RENUMBER_XOR_R_FROM_RM:
		BINARYOP_R_FROM_RM(XorWord,XorDword,3,true);
		break;


	case I486_RENUMBER_AAA: // 0x37
		clocksPassed=3;
		if(9<(GetAL()&0x0f) || true==GetAF())
		{
			auto AL=((GetAL()+6)&0x0F);
			auto AH=(GetAH()+1);
			SetAX((AH<<8)|AL);
			SetCF(true);
			SetAF(true);
		}
		else
		{
			SetAL(GetAL()&0x0F);
			SetCF(false);
			SetAF(false);
		}
		break;

	case I486_RENUMBER_AAD://    0xD5,
		{
			clocksPassed=14;
			auto AL=GetAH()*inst.EvalUimm8()+GetAL();
			SetAL(AL);
			SetAH(0);
			SetZF(0==GetAX());
			SetSF(0!=(GetAL()&0x80));
			SetPF(CheckParity(AL));
		}
		break;
	case I486_RENUMBER_AAM://    0xD4,
		{
			clocksPassed=15;
			auto AL=GetAL();
			auto quo=AL/inst.EvalUimm8();
			auto rem=AL%inst.EvalUimm8();
			SetAH(quo);
			SetAL(rem);
			SetZF(0==GetAL());   // ?
			SetSF(0!=(GetAH()&0x80));
			SetPF(CheckParity(GetAL()));
		}
		break;

	case I486_RENUMBER_AAS:
		{ // BP 000C:0371
			clocksPassed=3;
			auto AL=GetAL();
			if((AL&0x0F)>9 || true==GetAF())
			{
				SetAL((AL-6)&0x0F);
				SetAH((GetAH()-1)&0xFF);
				SetAF(true);
				SetCF(true);
			}
			else
			{
				SetAL(AL&0x0F); // [1] pp. 26-21 "In either case, the AL register is left with its top nibble set to 0."
				SetAF(false);
				SetCF(false);
			}
		}
		break;

	case I486_RENUMBER_ARPL://       0x63,
		{
			clocksPassed=9;
			if(0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
			{
				RaiseException(EXCEPTION_UD,0);
				HandleException(true,mem,inst.numBytes);
				clocksPassed+=ClocksForHandlingException();
				EIPIncrement=0;
				break;
			}

			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			auto dst=value1.GetAsWord();
			auto src=value2.GetAsWord();
			if((dst&3)<(src&3))
			{
				dst&=(~3);
				dst|=(src&3);
				SetZF(true);
				value1.MakeWord(dst);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
			else
			{
				SetZF(false);
			}
		}
		break;


	case I486_RENUMBER_BOUND: // 0x62
		{
			clocksPassed=7;
			if(OPER_ADDR==op2.operandType)
			{
				unsigned int offset;
				auto &seg=*ExtractSegmentAndOffset(offset,op2,inst.segOverride);
				int min,max,idx;
				if(16==inst.operandSize)
				{
					unsigned int reg=REG_AX+inst.GetREG();
					unsigned int i=GetRegisterValue(reg);
					idx=(i&0x7FFF)-(i&0x8000);

					int MIN,MAX;
					MIN=FetchWord(inst.addressSize,seg,offset  ,mem);
					MAX=FetchWord(inst.addressSize,seg,offset+2,mem);

					min=(MIN&0x7FFF)-(MIN&0x8000);
					max=(MAX&0x7FFF)-(MAX&0x8000);
				}
				else
				{
					unsigned int reg=REG_EAX+inst.GetREG();
					unsigned int i=GetRegisterValue(reg);
					idx=(i&0x7FFFFFFF)-(i&0x80000000);

					int MIN,MAX;
					MIN=FetchDword(inst.addressSize,seg,offset  ,mem);
					MAX=FetchDword(inst.addressSize,seg,offset+4,mem);

					min=(MIN&0x7FFFFFFF)-(MIN&0x80000000);
					max=(MAX&0x7FFFFFFF)-(MAX&0x80000000);
				}
				if(idx<min || max<idx)
				{
					Interrupt(5,mem,0,0,false); // inst.numBytes,inst.numBytes ?
					EIPIncrement=0;
				}
			}
			else
			{
				if(MODE_NATIVE!=state.mode) // <-> (IsInRealMode() || 0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
				{
					Interrupt(6,mem,0,0,false);
					EIPIncrement=0;
				}
				else
				{
					RaiseException(EXCEPTION_UD,0);
					HandleException(true,mem,inst.numBytes);
					clocksPassed+=ClocksForHandlingException();
					EIPIncrement=0;
					break;
				}
			}
		}
		break;


	case I486_RENUMBER_BT_BTS_BTR_BTC_RM_I8:// 0FBA
		{
			clocksPassed=(OPER_ADDR==op1.operandType ? 8 : 6);
			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			if(true!=state.exception)
			{
				auto bitOffset=inst.EvalUimm8()&0x1F;
				auto bit=(1<<bitOffset);
				auto src=value1.GetAsDword();
				bool CF=(0!=(src&bit));
				switch(inst.GetREG())
				{
				case 4: // BT (Bit Test)
					break;
				case 5: // BTS (Bit Test and Set)
					if(0==(src&bit))
					{
						src|=bit;
						value1.SetDword(src);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					}
					break;
				case 6: // BTR (Bit Test and Reset)
					if(0!=(src&bit))
					{
						src&=(~bit);
						value1.SetDword(src);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					}
					break;
				case 7: // BTC (Bit Test and Complement)
					{
						src^=bit;
						value1.SetDword(src);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					}
					break;
				case 0:
				case 1:
				case 2:
				case 3:
					Abort("Undefined REG for "+cpputil::Ustox(inst.RealOpCode()));
					break;
				default:
					std_unreachable;
				}
				HANDLE_EXCEPTION_IF_ANY;
				SetCF(CF);
			}
			HANDLE_EXCEPTION_IF_ANY;
		}
		break;


	case I486_RENUMBER_BSF_R_RM://   0x0FBC,
	case I486_RENUMBER_BSR_R_RM://   0x0FBD,
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			auto src=value.GetAsDword();
			clocksPassed=(I486_OPCODE_BSF_R_RM==inst.opCode ? 6 : 7);
			if(0==src)
			{
				SetZF(true);
			}
			else
			{
				unsigned int bit;
				unsigned int count;
				if(I486_OPCODE_BSF_R_RM==inst.opCode)
				{
					bit=1;
					for(count=0; count<inst.operandSize; ++count)
					{
						if(0!=(src&bit))
						{
							break;
						}
						bit<<=1;
					}
					clocksPassed+=count; // On actual CPU 6-42  ?? Why is it not 6+count ?? [1] pp. 26-31
				}
				else // if(I486_OPCODE_BSR_R_RM==inst.opCode)
				{
					bit=(1<<(inst.operandSize-1));
					for(count=inst.operandSize-1; 0!=bit; --count)
					{
						if(0!=(src&bit))
						{
							break;
						}
						bit>>=1;
					}
					clocksPassed+=count*2; // On actual CPU 6-103 clocks.
				}
				if(OPER_ADDR==op2.operandType)
				{
					++clocksPassed;
				}
				if(count<inst.operandSize)
				{
					value.SetDword(count);
					SetZF(false);
				}
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
		}
		break;

	case I486_RENUMBER_BT_R_RM://    0x0FA3,
	case I486_RENUMBER_BTC_RM_R://   0x0FBB,
	case I486_RENUMBER_BTR_RM_R://   0x0FB3,
	case I486_RENUMBER_BTS_RM_R://   0x0FAB,
		if(OPER_ADDR!=op1.operandType)
		{
			clocksPassed=6;
			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			if(true!=state.exception)
			{
				auto bitOffset=value2.GetAsByte()&0x1F;
				auto bit=(1<<bitOffset);
				auto src=value1.GetAsDword();
				SetCF(0!=(src&bit));
				// Nothing more to do for I486_OPCODE_BT_R_RM
				if(I486_OPCODE_BTS_RM_R==inst.opCode)
				{
					if(0==(src&bit))
					{
						src|=bit;
						value1.SetDword(src);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					}
				}
				else if(I486_OPCODE_BTC_RM_R==inst.opCode)
				{
					src^=bit;
					value1.SetDword(src);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
				}
				else if(I486_OPCODE_BTR_RM_R==inst.opCode)
				{
					if(0!=(src&bit))
					{
						src&=(~bit);
						value1.SetDword(src);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					}
				}
			}
		}
		else // if(OPER_ADDR==op1.operandType)
		{
			clocksPassed=13;
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			unsigned bitCount=value2.GetAsDword();

			unsigned int offset;
			auto &seg=*ExtractSegmentAndOffset(offset,op1,inst.segOverride);
			offset+=(bitCount>>3);

			unsigned int src=FetchByte(inst.addressSize,seg,offset,mem);
			if(true!=state.exception)
			{
				auto bitOffset=bitCount&7;
				auto bit=(1<<bitOffset);
				SetCF(0!=(src&bit));
				// Nothing more to do for I486_OPCODE_BT_R_RM
				if(I486_OPCODE_BTS_RM_R==inst.opCode)
				{
					if(0==(src&bit))
					{
						src|=bit;
						StoreByte(mem,inst.addressSize,seg,offset,src);
					}
				}
				else if(I486_OPCODE_BTC_RM_R==inst.opCode)
				{
					src^=bit;
					StoreByte(mem,inst.addressSize,seg,offset,src);
				}
				else if(I486_OPCODE_BTR_RM_R==inst.opCode)
				{
					if(0!=(src&bit))
					{
						src&=(~bit);
						StoreByte(mem,inst.addressSize,seg,offset,src);
					}
				}
			}
		}
		HANDLE_EXCEPTION_IF_ANY;
		break;


	case I486_RENUMBER_LAR:
		if(IsInRealMode())
		{
			Interrupt(6,mem,0,0,false);
			EIPIncrement=0;
		}
		else
		{
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			auto selector=value2.GetAsWord();

			i486DXCommon::LoadSegmentRegisterTemplate<const i486DXCommon,FIDELITY> loader;
			loader.LoadProtectedModeDescriptor(*this,selector,mem);
			if(true==state.exception || nullptr==loader.rawDesc)
			{
				SetZF(false);
				break;
			}
			uint8_t CPL=state.CS().DPL;
			uint8_t RPL=selector&3;
			uint8_t descDPL=(loader.rawDesc[5]>>5)&3;
			uint8_t descType=loader.rawDesc[5]&0x1F;
			if(0!=(descType&0x10))
			{
				descType&=~1;
			}

			bool valid=false;
			switch(descType)
			{
			case DESCTYPE_CALL_GATE://                       4,
			case DESCTYPE_TASK_GATE://                       5,
			case DESCTYPE_386_CALL_GATE://                0x0C,

			case DESCTYPE_AVAILABLE_286_TSS://               1,
			case DESCTYPE_LDT://                             2,
			case DESCTYPE_BUSY_286_TSS://                    3,
			// case 8: // Should LSL return for this one????  80386 Programmer's Reference Manual 1986 tells so.
			//            But actually measurement tells otherwise.
			case DESCTYPE_AVAILABLE_386_TSS://               9,
			case DESCTYPE_BUSY_386_TSS://                 0x0B,
			// Reserved                            0x0D
			case SEGTYPE_DATA_NORMAL_READONLY://       0b10000, // Data Normal         Read-Only
			case SEGTYPE_DATA_NORMAL_RW://             0b10010, // Data Normal         Read/Write
			case SEGTYPE_DATA_EXPAND_DOWN_READONLY://  0b10100, // Data Expand-Down    Read-Only
			case SEGTYPE_DATA_EXPAND_DOWN_RW://        0b10110, // Data Expand-Down    Read/Write
			case SEGTYPE_CODE_NONCONFORMING_EXECONLY://0b11000, // Code Non-Conforming Execute-Only
			case SEGTYPE_CODE_NONCONFORMING_READABLE://0b11010, // Code Non-Conforming Readable
				if(CPL<=descDPL && RPL<=descDPL)
				{
					valid=true;
				}
				break;

			case SEGTYPE_CODE_CONFORMING_EXECONLY://   0b11100, // Code Conforming     Execute-Only
			case SEGTYPE_CODE_CONFORMING_READABLE://   0b11110, // Code Conforming     Readable
				valid=true;
				break;

			case DESCTYPE_286_INT_GATE://                    6,
			case DESCTYPE_286_TRAP_GATE://                   7,
			case DESCTYPE_386_INT_GATE://                 0x0E,
			case DESCTYPE_386_TRAP_GATE://                0x0F,
				break;
			}

			if(true==valid)
			{
				uint32_t accessRightBytes=cpputil::GetDword(loader.rawDesc+4);
				accessRightBytes&=0x00FFFF00;
				// Although INTEL 80386 PROGRAMMER'S REFERENCE MANUAL 1986 pp. 325 tells upper four bits
				// of the limit will be undefined, 80486 returns exactly as defined in the descriptor.
				OperandValue value1;
				value1.SetDword(accessRightBytes);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
				SetZF(true);
			}
			else
			{
				SetZF(false);
			}
			clocksPassed=16;
		}
		break;


	case I486_RENUMBER_CALL_FAR://   0x9A,
		{
			if(true==IsInRealMode())
			{
				clocksPassed=18;
			}
			else
			{
				clocksPassed=20;
			}

			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=CALLF(mem,inst.operandSize,inst.numBytes,op1.seg,op1.offset,clocksPassed);
			HANDLE_EXCEPTION_PUSH_POP;

			EIPIncrement=0;
		}
		break;
	case I486_RENUMBER_CALL_REL://   0xE8,
		{
			clocksPassed=3;

			SAVE_ESP_BEFORE_PUSH_POP;
			uint32_t destin;

			if(16==inst.operandSize)
			{
				auto offset=inst.EvalSimm16();
				destin=cpputil::LowWord(state.EIP+offset+inst.numBytes);
				Push16(mem,state.EIP+inst.numBytes);
			}
			else // if(32==inst.operandSize)
			{
				auto offset=inst.EvalSimm32();
				destin=state.EIP+offset+inst.numBytes;
				Push32(mem,state.EIP+inst.numBytes);
			}
			HANDLE_EXCEPTION_PUSH_POP;

			if(true==enableCallStack)
			{
				PushCallStack(
				    false,0xffff,0xffff,
				    state.GetCR(0),
				    state.CS().value,state.EIP,inst.numBytes,
				    state.CS().value,destin,
				    mem);
			}

			state.EIP=destin;
			EIPIncrement=0;
		}
		break;
	case I486_RENUMBER_JMP_REL://          0xE9,   // cw or cd
		{
			clocksPassed=3;

			if(16==inst.operandSize)
			{
				auto offset=inst.EvalSimm16();
				state.EIP=cpputil::LowWord(state.EIP+offset+inst.numBytes);
			}
			else // if(32==inst.operandSize)
			{
				auto offset=inst.EvalSimm32();
				state.EIP=state.EIP+offset+inst.numBytes;
			}

			EIPIncrement=0;
		}
		break;


	case I486_RENUMBER_CBW_CWDE://        0x98,
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
	case I486_RENUMBER_CWD_CDQ://         0x99,
		clocksPassed=3;
		if(16==inst.operandSize) // CWD AX->DX:AX
		{
			SetDX(0!=(GetAX()&0x8000) ? 0xFFFF : 0);
		}
		else // CDQ
		{
			SetEDX(0!=(GetEAX()&0x80000000) ? 0xFFFFFFFF : 0);
		}
		break;
	case I486_RENUMBER_CLC:
		state.EFLAGS&=(~EFLAGS_CARRY);
		clocksPassed=2;
		break;
	case I486_RENUMBER_CLD:
		state.EFLAGS&=(~EFLAGS_DIRECTION);
		clocksPassed=2;
		break;
	case I486_RENUMBER_CLI:
		if(true==fidelity.IOPLException(*this,EXCEPTION_GP,mem,inst.numBytes))
		{
			EIPIncrement=0;
			clocksPassed=2;
			break;
		}
		state.EFLAGS&=(~EFLAGS_INT_ENABLE);
		clocksPassed=2;
		break;
	case I486_RENUMBER_CLTS:
		{
			clocksPassed=7;
			auto CR0=state.GetCR(0);
			CR0&=(~CR0_TASK_SWITCHED);
			SetCR(0,CR0);
		}
		break;


	case I486_RENUMBER_CMC://        0xF5,
		SetCF(GetCF()==true ? false : true);
		clocksPassed=2;
		break;


	case I486_RENUMBER_CMPSB://           0xA6,
		{
			auto ECX=state.ECX();
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);

			#define CMPSB_Template(addrSize)\
				for(int ctr=0;\
				    ctr<MAX_REP_BUNDLE_COUNT &&\
				    true==REPCheckA##addrSize(clocksPassed,inst.instPrefix);\
				    ++ctr)\
				{\
					auto data1=FetchByte(addrSize,seg,state.ESI(),mem);\
					auto data2=FetchByte(addrSize,state.ES(),state.EDI(),mem);\
					if(true!=state.exception)\
					{\
						SubByte(data1,data2);\
						UpdateESIandEDIAfterStringOpO8A##addrSize();\
						clocksPassed+=4;\
						if(true==REPEorNECheck(inst.instPrefix))\
						{\
							EIPIncrement=0;\
						}\
						else\
						{\
							EIPIncrement=inst.numBytes;\
							break;\
						}\
					}\
					else\
					{\
						SetECX(ECX);\
						HandleException(true,mem,inst.numBytes);\
						clocksPassed+=ClocksForHandlingException();\
						EIPIncrement=0;\
						break;\
					}\
					ECX=state.ECX();\
				}\

			if(16==inst.addressSize)
			{
				CMPSB_Template(16);
			}
			else
			{
				CMPSB_Template(32);
			}
		}
		break;

	case I486_RENUMBER_CMPS://            0xA7,
		{
			auto ECX=state.ECX();
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);

			#define CMPS_Template(addrSize,wordOrDword,updateFunc)\
				for(int ctr=0;\
				    ctr<MAX_REP_BUNDLE_COUNT &&\
				    true==REPCheckA##addrSize(clocksPassed,inst.instPrefix);\
				    ++ctr)\
				{\
					auto data1=Fetch##wordOrDword(addrSize,seg,state.ESI(),mem);\
					auto data2=Fetch##wordOrDword(addrSize,state.ES(),state.EDI(),mem);\
					if(true!=state.exception)\
					{\
						Sub##wordOrDword(data1,data2);\
						updateFunc();\
						clocksPassed+=4;\
						if(true==REPEorNECheck(inst.instPrefix))\
						{\
							EIPIncrement=0;\
						}\
						else\
						{\
							EIPIncrement=inst.numBytes;\
							break;\
						}\
					}\
					else\
					{\
						SetECX(ECX);\
						HandleException(true,mem,inst.numBytes);\
						clocksPassed+=ClocksForHandlingException();\
						EIPIncrement=0;\
						break;\
					}\
					ECX=state.ECX();\
				}

			if(16==inst.operandSize)
			{
				if(16==inst.addressSize)
				{
					CMPS_Template(16,Word,UpdateESIandEDIAfterStringOpO16A16);
				}
				else
				{
					CMPS_Template(32,Word,UpdateESIandEDIAfterStringOpO16A32);
				}
			}
			else // 32-bit operand size
			{
				if(16==inst.addressSize)
				{
					CMPS_Template(16,Dword,UpdateESIandEDIAfterStringOpO32A16);
				}
				else
				{
					CMPS_Template(32,Dword,UpdateESIandEDIAfterStringOpO32A32);
				}
			}
		}
		break;


	case I486_RENUMBER_DAA://             0x27,
		clocksPassed=2;
		if(true==GetAF() || 9<(GetAL()&0x0F))
		{
			SetAL(GetAL()+6);
			SetAF(true);
		}
		else
		{
			SetAF(false);
		}
		if(0x9F<GetAL() || true==GetCF())
		{
			SetAL(GetAL()+0x60);
			SetCF(true);
		}
		else
		{
			SetCF(false);
		}
		SetZF(0==GetAL());
		SetSF(0!=(GetAL()&0x80));
		SetPF(CheckParity(GetAL()));
		break;

	case I486_RENUMBER_DAS://             0x2F,
		clocksPassed=2;
		if(true==GetAF() || 9<(GetAL()&0x0F))
		{
			SetAL(GetAL()-6);
			SetAF(true);
		}
		else
		{
			SetAF(false);
		}
		if(0x9F<GetAL() || true==GetCF())
		{
			SetAL(GetAL()-0x60);
			SetCF(true);
		}
		else
		{
			SetCF(false);
		}
		SetZF(0==GetAL());
		SetSF(0!=(GetAL()&0x80));
		SetPF(CheckParity(GetAL()));
		break;

	case I486_RENUMBER_ENTER://      0xC8,
		{
			SAVE_ESPEBP_BEFORE_ENTER;

			// Weird operand.
			unsigned int frameSize=cpputil::GetWord(inst.operand);
			unsigned int level=inst.operand[2]&0x1F;

			clocksPassed=14+level*3;

			Push(mem,inst.operandSize,state.EBP());
			HANDLE_EXCEPTION_ENTER;
			auto framePtr=state.ESP();
			if(0<level)
			{
				// Rewritten based on the psudo-code in https://www.scs.stanford.edu/05au-cs240c/lab/i386/ENTER.htm
				// Then rewritten based on Intel 80386 Programmer's Reference Manual 1986 pp.295.
				for(unsigned int i=1; i<=level-1; ++i)
				{
					if(16==inst.operandSize)
					{
						SetBP(GetBP()-2);
						auto dat=FetchWord(GetStackAddressingSize(),state.SS(),state.BP(),mem);
						HANDLE_EXCEPTION_ENTER_INSIDE;
						Push16(mem,dat);
						HANDLE_EXCEPTION_ENTER_INSIDE;
					}
					else
					{
						SetEBP(GetEBP()-4);
						auto dat=FetchDword(GetStackAddressingSize(),state.SS(),state.EBP(),mem);
						HANDLE_EXCEPTION_ENTER_INSIDE;
						Push32(mem,dat);
						HANDLE_EXCEPTION_ENTER_INSIDE;
					}
				}

				Push(mem,inst.operandSize,framePtr);  // Should it be operandSize or addressSize?  Extremely confusing!
				HANDLE_EXCEPTION_ENTER;
			}
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
			if(true!=IsInRealMode())
			{
				fidelity.PageFaultCheckAfterEnter(*this,mem);
				HANDLE_EXCEPTION_ENTER;
			}
		}
	BREAK_ENTER:
		break;


	case I486_RENUMBER_FWAIT://      0x9B,
		if(true==state.fpuState.ExceptionPending())
		{
			EIPIncrement=0;
		}
		clocksPassed=3;
		break;

	case I486_RENUMBER_FPU_D8: // 0xD8
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif
		{
			auto MODR_M=inst.operand[0];
			if(0xC0==(MODR_M&0xF8))   // FADD ST,ST(i)
			{
				FPU_TRAP;
				clocksPassed=state.fpuState.FADD_ST_STi(*this,MODR_M&7);
			}
			else if(0xC8==(MODR_M&0xF8))   // FMUL ST,ST(i)
			{
				FPU_TRAP;
				clocksPassed=state.fpuState.FMUL_ST_STi(*this,MODR_M&7);
			}
			else if(0xD0==(MODR_M&0xF8))   // FCOM
			{
				FPU_TRAP;
				clocksPassed=state.fpuState.FCOM(*this,MODR_M&7);
			}
			else if(0xD8==(MODR_M&0xF8))   // FCOMP
			{
				FPU_TRAP;
				clocksPassed=state.fpuState.FCOMP(*this,MODR_M&7);
			}
			else if(0xE0==(MODR_M&0xF8))
			{
				FPU_TRAP;
				clocksPassed=state.fpuState.FSUB_ST_STi(*this,MODR_M&7);
			}
			else if(0xE8==(MODR_M&0xF8))
			{
				FPU_TRAP;
				clocksPassed=state.fpuState.FSUBR_ST_STi(*this,MODR_M&7);
			}
			else if(0xF0==(MODR_M&0xF8))
			{
				FPU_TRAP;
			   	clocksPassed=state.fpuState.FDIV_ST_STi(*this,MODR_M&7);
			}
			else if(0xF8==(MODR_M&0xF8))
			{
			   FPU_TRAP;
			   clocksPassed=state.fpuState.FDIVR_ST_STi(*this,MODR_M&7);
			}
			else
			{
				switch(inst.GetREG())
				{
				case 0:	// FADD m32real
					FPU_TRAP;
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FADD_m32real(*this,value.byteData);
					}
					break;
				case 1:
					FPU_TRAP;
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FMUL_m32real(*this,value.byteData);
					}
					break;
				case 2:
					FPU_TRAP;
					{
						auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FCOM_m32real(*this,value.byteData);
					}
					break;
				case 3:
					FPU_TRAP;
					{
						auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FCOMP_m32real(*this,value.byteData);
					}
					break;
				case 4:
					FPU_TRAP;
					{
						auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FSUB_m32real(*this,value.byteData);
					}
					break;
				case 5:
					FPU_TRAP;
					{
						auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FSUBR_m32real(*this,value.byteData);
					}
					break;
				case 6:
					FPU_TRAP;
					{
						auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FDIV_m32real(*this,value.byteData);
					}
					break;
				case 7:
					FPU_TRAP;
					{
						auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FDIVR_m32real(*this,value.byteData);
					}
					break;
				}
			}
		}
		break;

	case I486_RENUMBER_FPU_D9:// 0xD9,
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif

		if(0xC0<=inst.operand[0] && inst.operand[0]<=0xC7)
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FLD_ST(*this,inst.operand[0]&7);
		}
		else if(0xC8<=inst.operand[0] && inst.operand[0]<=0xCF)
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FXCH(*this,inst.operand[0]&7);
		}
		else if(0xD0==inst.operand[0])
		{
			clocksPassed=3; // FNOP
		}
		else if(0xE0==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FCHS(*this);
		}
		else if(0xE1==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FABS(*this);
		}
		else if(0xE4==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FTST(*this);
		}
		else if(0xE5==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FXAM(*this);
		}
		else if(0xE8==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FLD1(*this);
		}
		else if(0xE9==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FLDL2T(*this);
		}
		else if(0xEA==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FLDL2E(*this);
		}
		else if(0xEB==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FLDPI(*this);
		}
		else if(0xEC==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FLDLG2(*this);
		}
		else if(0xED==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FLDLN2(*this);
		}
		else if(0xEE==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FLDZ(*this);
		}
		else if(0xF0==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.F2XM1(*this);
		}
		else if(0xF1==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FYL2X(*this);
		}
		else if(0xF2==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FPTAN(*this);
		}
		else if(0xF3==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FPATAN(*this);
		}
		else if(0xF4==inst.operand[0])
		{
			// FXTRACT
		}
		else if(0xF5==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FPREM1(*this);
		}
		else if(0xF6==inst.operand[0])
		{
			// FDECSTP
		}
		else if(0xF7==inst.operand[0])
		{
			// FINCSTP
		}
		else if(0xF8==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FPREM(*this);
		}
		else if(0xF9==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FYL2XP1(*this);
		}
		else if(0xFA==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FSQRT(*this);
		}
		else if(0xFB==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FSINCOS(*this);
		}
		else if(0xFC==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FRNDINT(*this);
		}
		else if(0xFD==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FSCALE(*this);
		}
		else if(0xFE==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FSIN(*this);
		}
		else if(0xFF==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FCOS(*this);
		}
		else
		{
			switch(inst.GetREG())
			{
			case 0: // "FLD m32real"
				FPU_TRAP;
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
					HANDLE_EXCEPTION_IF_ANY;
					clocksPassed=state.fpuState.FLD32(*this,value.byteData);
				}
				break;
			case 2: // "FST m32real"
				FPU_TRAP;
				{
					OperandValue value;
					state.fpuState.GetSTAsFloat(*this,value);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
					HANDLE_EXCEPTION_IF_ANY;
					clocksPassed=7;
				}
				break;
			case 3: // FSTP m32real
				FPU_TRAP;
				{
					OperandValue value;
					state.fpuState.GetSTAsFloat(*this,value);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
					HANDLE_EXCEPTION_IF_ANY;
					state.fpuState.Pop(*this);
					clocksPassed=7;
				}
				break;
			case 4: // "FLDENV"
				FPU_TRAP;
				{
					uint8_t data[28];
					unsigned int dataLen=14;
					if(16==inst.operandSize)
					{
						dataLen=14;
					}
					else
					{
						dataLen=28;
					}
					unsigned int offset;
					auto segPtr=ExtractSegmentAndOffset(offset,op1,inst.segOverride);
					for(unsigned int i=0; i<dataLen; ++i)
					{
						data[i]=FetchByte(inst.addressSize,*segPtr,offset+i,mem);
						if(state.exception)
						{
							break;
						}
					}
					HANDLE_EXCEPTION_IF_ANY;
					clocksPassed=state.fpuState.FLDENV(*this,inst.operandSize,data);
				}
				break;
			case 5: // "FLDCW"
				FPU_TRAP;
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					auto cw=value.GetAsWord();
					clocksPassed=state.fpuState.FLDCW(*this,cw);
				}
				break;
			case 6: // FNSTENV
				FPU_TRAP;
				if(true==state.fpuState.enabled)
				{
					unsigned int offset;
					auto segPtr=ExtractSegmentAndOffset(offset,op1,inst.segOverride);
					clocksPassed=67;
					auto data=state.fpuState.FNSTENV(*this,inst.operandSize);
					for(auto b : data)
					{
						StoreByte(mem,inst.addressSize,*segPtr,offset++,b);
					}
				}
				break;
			case 7: // "FNSTCW"
				if(0!=(state.GetCR(0)&(CR0_TASK_SWITCHED|CR0_EMULATION)))
				{
					FPU_TRAP;
				}
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
	case I486_RENUMBER_FPU_DA:// 0xDA,
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif
		if(0xC0==(inst.operand[0]&0xF8))
		{
			 // FCMOVB
		}
		else if(0xC8==(inst.operand[0]&0xF8))
		{
			 // FCMOVE
		}
		else if(0xD0==(inst.operand[0]&0xF8))
		{
			 // FCMOVBE
		}
		else if(0xD8==(inst.operand[0]&0xF8))
		{
			 // FCMOVU
		}
		else if(0xE9==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed = state.fpuState.FUCOMPP(*this);
		}
		else
		{
			switch(Instruction::GetREG(inst.operand[0]))
			{
			case 0:  // FIADD m32int
				FPU_TRAP;
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
					clocksPassed=state.fpuState.FIADD_m32int(*this,value.byteData);
				}
				break;
			case 1:  // FIMUL m32int
				FPU_TRAP;
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
					clocksPassed=state.fpuState.FIMUL_m32int(*this,value.byteData);
				}
				break;
			case 2:  // FICOM m32int
				break;
			case 3:  // FICOMP m32int
				break;
			case 4:  // FISUB m32int
				FPU_TRAP;
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
					clocksPassed=state.fpuState.FISUB_m32int(*this,value.byteData);
				}
				break;
			case 5:  // FISUBR m32int
				FPU_TRAP;
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
					clocksPassed=state.fpuState.FISUBR_m32int(*this,value.byteData);
				}
				break;
			case 6:  // FIDIV m32int
				FPU_TRAP;
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
					clocksPassed=state.fpuState.FIDIV_m32int(*this,value.byteData);
				}
				break;
			case 7:  // FIDIVR m32int
				FPU_TRAP;
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
					clocksPassed=state.fpuState.FIDIVR_m32int(*this,value.byteData);
				}
				break;
			}
		}
		break;
	case I486_RENUMBER_FPU_DB://     0xDB, 
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif

		if(0xC0==(inst.operand[0]&0xF8))
		{
			// FCMOVNB
		}
		else if(0xC8==(inst.operand[0]&0xF8))
		{
			// FCMOVNE
		}
		else if(0xD0==(inst.operand[0]&0xF8))
		{
			// FCMOVNBE
		}
		else if(0xD8==(inst.operand[0]&0xF8))
		{
			// FCMOVNU
		}
		else if(0xE8==(inst.operand[0]&0xF8))
		{
			// FUCOMI
		}
		else if(0xF0==(inst.operand[0]&0xF8))
		{
			// FCOMI
		}
		else if(0xE2==inst.operand[0])
		{
			FPU_TRAP;
			clocksPassed=state.fpuState.FCLEX(*this);
		}
		else if(0xE4==inst.operand[0])
		{
			clocksPassed=1; // FSETPM does nothing in 80386 and later.
		}
		else if(0xE3==inst.operand[0])
		{
			if(0!=(state.GetCR(0)&(CR0_TASK_SWITCHED|CR0_EMULATION)))
			{
				FPU_TRAP;
			}
			state.fpuState.FNINIT();
			clocksPassed=17;
		}
		else if(0xE0==inst.operand[0] || // FNENI
		        0xE1==inst.operand[0])   // FNDISI
		{
			// Apparently legacy instruction from 8087 and no effect in 80386 and later.  (Maybe 80286 and later)
			clocksPassed=1;
		}
		else
		{
			switch(Instruction::GetREG(inst.operand[0]))
			{
			case 1:
			case 4:
			case 6:
			default:
				break;
			case 0: // FILD m32int
				FPU_TRAP;
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
					clocksPassed=state.fpuState.FILD_m32int(*this,value.byteData);
				}
				break;
			case 2: // FIST m32int
				FPU_TRAP;
				{
					OperandValue value;
					state.fpuState.GetSTAsSignedInt(*this, value);
					value.numBytes = 4;
					StoreOperandValue(op1, mem, inst.addressSize, inst.segOverride, value);
					clocksPassed = 32;
				}
				break;
			case 3: // FISTP m32int
				FPU_TRAP;
				{
					OperandValue value;
					state.fpuState.GetSTAsSignedInt(*this,value);
					state.fpuState.Pop(*this);
					value.numBytes=4;
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
					clocksPassed=33;
				}
				break;
			case 5: // FLD m80real
				FPU_TRAP;
				{
					auto value=EvaluateOperand80(mem,inst.addressSize,inst.segOverride,op1);
					clocksPassed=state.fpuState.FLD80(*this,value.byteData);
				}
				break;
			case 7: // FSTP m80real
				FPU_TRAP;
				{
					OperandValue value;
					i486DXCommon::FPUState::DoubleTo80Bit(value,state.fpuState.ST(*this).value);
					state.fpuState.Pop(*this);
					StoreOperandValue80(op1,mem,inst.addressSize,inst.segOverride,value);
					clocksPassed=6;
				}
				break;
			}
		}
		break;
	case I486_RENUMBER_FPU_DC:
		{
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif
			unsigned int MODR_M=inst.operand[0];
			if(0xC0==(MODR_M&0xF8))
			{
				FPU_TRAP;
				clocksPassed=state.fpuState.FADD_STi_ST(*this,MODR_M&7);
			}
			else if(0xC8==(MODR_M&0xF8))
			{
				FPU_TRAP;
				clocksPassed=state.fpuState.FMUL_STi_ST(*this,MODR_M&7);
			}
			else if(0xD0==(MODR_M&0xF8))
			{
				// FCOM2 ST,STi? (Need check)
			}
			else if(0xD8==(MODR_M&0xF8))
			{
				// FCOMP3 ST,STi? (Need check)
			}
			else if(0xE0==(MODR_M&0xF8))
			{
				// FSUBR STi,ST
				FPU_TRAP;
				clocksPassed=state.fpuState.FSUBR_STi_ST(*this,MODR_M&7);
			}
			else if(0xE8==(MODR_M&0xF8))
			{
				FPU_TRAP;
				clocksPassed=state.fpuState.FSUB_STi_ST(*this,MODR_M&7);
			}
			else if(0xF0==(MODR_M&0xF8))
			{
				// FDIVR STi,ST
				FPU_TRAP;
				clocksPassed=state.fpuState.FDIVR_STi_ST(*this,MODR_M&7);
			}
			else if(0xF8==(MODR_M&0xF8))
			{
				// FDIV STi,ST
				FPU_TRAP;
				clocksPassed=state.fpuState.FDIV_STi_ST(*this,MODR_M&7);
			}
			else
			{
				switch(Instruction::GetREG(MODR_M))
				{
				case 0:	// FADD m64real
					FPU_TRAP;
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FADD64(*this,value.byteData);
					}
					break;
				case 1: // FMUL m64real
					FPU_TRAP;
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FMUL_m64real(*this,value.byteData);
					}
					break;
				case 2: //
					FPU_TRAP;
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FCOM_m64real(*this,value.byteData);
					}
					break;
				case 3: // FCOMP m64real
					FPU_TRAP;
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FCOMP_m64real(*this,value.byteData);
					}
					break;
				case 4:
					FPU_TRAP;
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FSUB_m64real(*this,value.byteData);
					}
					break;
				case 5:
					FPU_TRAP;
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FSUBR_m64real(*this,value.byteData);
					}
					break;
				case 6: // FDIV m64real
					FPU_TRAP;
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FDIV_m64real(*this,value.byteData);
					}
					break;
				case 7: // FDIVR m64real
					FPU_TRAP;
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FDIVR_m64real(*this,value.byteData);
					}
					break;
				default:
					break;
				}
			}
		}
		break;
	case I486_RENUMBER_FPU_DD:
		{
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif
			switch(inst.operand[0]&0xF8)
			{
			case 0xC0: // C0 11000xxx
				FPU_TRAP;
				clocksPassed=state.fpuState.FFREE(*this,(inst.operand[0]&7));
				break;
			case 0xC8: // FXCH4? Need check
				break;
			case 0xD0: // D0 11010xxx    [1] pp.151  0<=i<=7
				FPU_TRAP;
				clocksPassed=state.fpuState.FST_STi(*this,(inst.operand[0]&7));
				break;
			case 0xD8: // D8 11011xxx
				FPU_TRAP;
				clocksPassed=state.fpuState.FSTP_STi(*this,(inst.operand[0]&7));
				break;
			case 0xE0:
				FPU_TRAP;
				clocksPassed=state.fpuState.FUCOM_STi(*this,inst.operand[0]&7);
				break;
			case 0xE8:
				FPU_TRAP;
				clocksPassed = state.fpuState.FUCOMP_STi(*this, inst.operand[0] & 7);
				break;
			default:
				switch(Instruction::GetREG(inst.operand[0]))
				{
				case 0:	// FLD m64real
					FPU_TRAP;
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FLD64(*this,value.byteData);
					}
					break;
				case 2: // FST m64real
					FPU_TRAP;
					{
						OperandValue value;
						state.fpuState.GetSTAsDouble(*this,value);
						StoreOperandValue64(op1,mem,inst.addressSize,inst.segOverride,value);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=8;
					}
					break;
				case 3: // FSTP m64real
					FPU_TRAP;
					{
						OperandValue value;
						state.fpuState.GetSTAsDouble(*this,value);
						state.fpuState.Pop(*this);
						StoreOperandValue64(op1,mem,inst.addressSize,inst.segOverride,value);
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=8;
					}
					break;
				case 4: // FRSTOR m94/108byte
					FPU_TRAP;
					{
						std::vector <uint8_t> data;
						unsigned int offset;
						auto segPtr=ExtractSegmentAndOffset(offset,op1,inst.segOverride);

						unsigned int size=(16==inst.operandSize ? 94 : 108);
						data.resize(size);
						for(unsigned int i=0; i<size; ++i)
						{
							data[i]=FetchByte(inst.addressSize,*segPtr,offset+i,mem);
							if(state.exception)
							{
								break;
							}
						}
						HANDLE_EXCEPTION_IF_ANY;
						clocksPassed=state.fpuState.FRSTOR(*this,inst.operandSize,data.data());
					}
					break;
				case 6: // FSAVE m94/108byte
					FPU_TRAP;
					{
						clocksPassed=154;

						unsigned int offset;
						auto segPtr=ExtractSegmentAndOffset(offset,op1,inst.segOverride);

						for(auto c : state.fpuState.FSAVE(*this,inst.operandSize))
						{
							StoreByte(mem,inst.addressSize,*segPtr,offset++,c);
							if(state.exception)
							{
								break;
							}
						}
						HANDLE_EXCEPTION_IF_ANY;
					}
					break;
				case 7: // FNSTSW m2byte
					if(0!=(state.GetCR(0)&(CR0_TASK_SWITCHED|CR0_EMULATION)))
					{
						FPU_TRAP;
					}
					{
						clocksPassed=3;
						OperandValue value;
						value.MakeWord(state.fpuState.GetStatusWord());
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
						HANDLE_EXCEPTION_IF_ANY;
					}
					break;
				}
			}
		}
		break;
	case I486_RENUMBER_FPU_DE:
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif
		switch(inst.operand[0]&0xF8)
		{
		case 0xC0:
			FPU_TRAP;
			clocksPassed=state.fpuState.FADDP_STi_ST(*this,inst.operand[0]&7);
			break;
		case 0xC8:
			FPU_TRAP;
			clocksPassed=state.fpuState.FMULP(*this,inst.operand[0]&7);
			break;
		case 0xE0:
			FPU_TRAP;
			clocksPassed=state.fpuState.FSUBRP_STi_ST(*this,inst.operand[0]&7);
			break;
		case 0xE8:
			FPU_TRAP;
			clocksPassed=state.fpuState.FSUBP_STi_ST(*this,inst.operand[0]&7);
			break;
		case 0xF0:
			FPU_TRAP;
			clocksPassed=state.fpuState.FDIVRP_STi_ST(*this,inst.operand[0]&7);
			break;
		case 0xF8:
			FPU_TRAP;
			clocksPassed=state.fpuState.FDIVP_STi_ST(*this,inst.operand[0]&7);
			break;
		default:
			if(0xD9==inst.operand[0])
			{
				FPU_TRAP;
				clocksPassed=state.fpuState.FCOMPP(*this);
			}
			else
			{
				switch(inst.GetREG())
				{
				case 0:
					FPU_TRAP;
					{
						auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
						clocksPassed=state.fpuState.FIADD_m16int(*this,value.byteData);
					}
					break;
				case 1:
					FPU_TRAP;
					{
						auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
						clocksPassed=state.fpuState.FIMUL_m16int(*this,value.byteData);
					}
					break;
				case 2:
					// FICOM m16int
					break;
				case 3:
					// FICOMP m16int
					break;
				case 4:
					FPU_TRAP;
					{
						auto value = EvaluateOperand(mem, inst.addressSize, inst.segOverride, op1, 2);
						clocksPassed = state.fpuState.FISUB_m16int(*this, value.byteData);
					}
					break;
				case 5:
					// FISUBR m16int
					FPU_TRAP;
					{
						auto value = EvaluateOperand(mem, inst.addressSize, inst.segOverride, op1, 2);
						clocksPassed = state.fpuState.FISUBR_m16int(*this, value.byteData);
					}
					break;
				case 6:
					FPU_TRAP;
					{
						auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
						clocksPassed=state.fpuState.FIDIV_m16int(*this,value.byteData);
					}
					break;
				case 7:
					// FIDIVR m16int
					FPU_TRAP;
					{
						auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
						clocksPassed=state.fpuState.FIDIVR_m16int(*this,value.byteData);
					}
					break;
				}
			}
			break;
		}
		break;
	case I486_RENUMBER_FPU_DF://  0xDF,
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif
		if(0xE0==inst.operand[0])
		{
			// FNSTSW AX
			if(0!=(state.GetCR(0)&(CR0_TASK_SWITCHED|CR0_EMULATION)))
			{
				FPU_TRAP;
			}
			SetAX(state.fpuState.GetStatusWord());
			clocksPassed=3;
		}
		else if(0xE8==(inst.operand[0]&0xF8))
		{
			// FUCOMIP
		}
		else if(0xF0==(inst.operand[0]&0xF8))
		{
			// FCOMIP
		}
		else
		{
			switch(Instruction::GetREG(inst.operand[0]))
			{
			case 0:
				FPU_TRAP;
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
					clocksPassed=state.fpuState.FILD_m16int(*this,value.byteData);
				}
				break;
			case 2: // FIST m16int
				FPU_TRAP;
				{
					OperandValue value;
					state.fpuState.GetSTAsSignedInt(*this,value);
					value.numBytes=2;
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
					clocksPassed=33;
				}
				break;
			case 3: // FISTP m16int
				FPU_TRAP;
				{
					OperandValue value;
					state.fpuState.GetSTAsSignedInt(*this,value);
					state.fpuState.Pop(*this);
					value.numBytes=2;
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
					clocksPassed=33;
				}
				break;
			case 4:
				FPU_TRAP;
				{
					auto value=EvaluateOperand80(mem,inst.addressSize,inst.segOverride,op1);
					clocksPassed=state.fpuState.FBLD(*this,value.byteData);
				}
				break;
			case 5: // FILD m64int
				FPU_TRAP;
				{
					auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
					clocksPassed=state.fpuState.FILD_m64int(*this,value.byteData);
				}
				break;
			case 6: // FBSTP m80dec
				FPU_TRAP;
				{
					OperandValue value;
					state.fpuState.GetSTAs80BitBCD(*this,value);
					state.fpuState.Pop(*this);
					StoreOperandValue80(op1,mem,inst.addressSize,inst.segOverride,value);
					clocksPassed=175;
				}
				break;
			case 7: // FISTP m64int
				FPU_TRAP;
				{
					OperandValue value;
					state.fpuState.GetSTAsSignedInt(*this,value);
					state.fpuState.Pop(*this);
					StoreOperandValue64(op1,mem,inst.addressSize,inst.segOverride,value);
					clocksPassed=33;
				}
				break;
			}
		}
		break;


	case I486_RENUMBER_DEC_EAX:
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			auto value=GetAX();
			DecrementWord(value);
			SetAX(value);
		}
		else
		{
			DecrementDword(state.EAX());
		}
		break;
	case I486_RENUMBER_DEC_ECX:
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			auto value=GetCX();
			DecrementWord(value);
			SetCX(value);
		}
		else
		{
			DecrementDword(state.ECX());
		}
		break;
	case I486_RENUMBER_DEC_EDX:
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			auto value=GetDX();
			DecrementWord(value);
			SetDX(value);
		}
		else
		{
			DecrementDword(state.EDX());
		}
		break;
	case I486_RENUMBER_DEC_EBX:
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			auto value=GetBX();
			DecrementWord(value);
			SetBX(value);
		}
		else
		{
			DecrementDword(state.EBX());
		}
		break;
	case I486_RENUMBER_DEC_ESP:
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			auto value=GetSP();
			DecrementWord(value);
			SetSP(value);
		}
		else
		{
			DecrementDword(state.ESP());
		}
		break;
	case I486_RENUMBER_DEC_EBP:
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			auto value=GetBP();
			DecrementWord(value);
			SetBP(value);
		}
		else
		{
			DecrementDword(state.EBP());
		}
		break;
	case I486_RENUMBER_DEC_ESI:
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			auto value=GetSI();
			DecrementWord(value);
			SetSI(value);
		}
		else
		{
			DecrementDword(state.ESI());
		}
		break;
	case I486_RENUMBER_DEC_EDI:
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			auto value=GetDI();
			DecrementWord(value);
			SetDI(value);
		}
		else
		{
			DecrementDword(state.EDI());
		}
		break;


	case I486_RENUMBER_INSB://     0x6C,
		{
			auto prefix=REPNEtoREP(inst.instPrefix);
			if(true==REPCheck(clocksPassed,prefix,inst.addressSize))
			{
				clocksPassed+=(IsInRealMode() ? 17 : 10); // Protected Mode 32 if CPL>IOPL
				if(true==fidelity.TakeIOReadException(*this,GetDX(),1,mem,inst.numBytes))
				{
					EIPIncrement=0;
					break;
				}
				auto ioRead=IOIn8(io,GetDX());
				StoreByte(mem,inst.addressSize,state.ES(),state.EDI(),ioRead);
				UpdateDIorEDIAfterStringOp(inst.addressSize,8);
				if(INST_PREFIX_REP==prefix)
				{
					EIPIncrement=0;
				}
			}
		}
		break;


	case I486_RENUMBER_IN_AL_I8://=        0xE4,
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		if(true==fidelity.TakeIOReadException(*this,inst.EvalUimm8(),1,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		{
			auto ioRead=IOIn8(io,inst.EvalUimm8());
			if(true!=state.exception)
			{
				SetAL(ioRead);
			}
		}
		break;
	case I486_RENUMBER_IN_A_I8://=         0xE5,
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		if(true==fidelity.TakeIOReadException(*this,inst.EvalUimm8(),inst.operandSize>>3,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		if(16==inst.operandSize)
		{
			auto ioRead=IOIn16(io,inst.EvalUimm8());
			if(true!=state.exception)
			{
				SetAX(ioRead);
			}
		}
		else
		{
			auto ioRead=IOIn32(io,inst.EvalUimm8());
			if(true!=state.exception)
			{
				SetEAX(ioRead);
			}
		}
		break;
	case I486_RENUMBER_IN_AL_DX://=        0xEC,
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		if(true==fidelity.TakeIOReadException(*this,GetDX(),1,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		{
			auto ioRead=IOIn8(io,GetDX());
			if(true!=state.exception)
			{
				SetAL(ioRead);
			}
		}
		break;
	case I486_RENUMBER_IN_A_DX://=         0xED,
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		if(true==fidelity.TakeIOReadException(*this,GetDX(),inst.operandSize>>3,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
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
		break;


	case I486_RENUMBER_IMUL_R_RM_I8://0x6B,
	case I486_RENUMBER_IMUL_R_RM_IMM://0x69,
	case I486_RENUMBER_IMUL_R_RM://       0x0FAF,
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
					long long int i3=inst.EvalSimm8();
					result=i2*i3;
					value1.SetSignedDword((int)result);
				}
				else if(I486_OPCODE_IMUL_R_RM_IMM==inst.opCode)
				{
					long long int i2=value2.GetAsSignedDword();
					long long int i3=inst.EvalSimm16or32(inst.operandSize);
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
					return 0;
				}
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
				bool setOFCF=false;
				switch(inst.operandSize)
				{
				case 8:
					setOFCF=(result<-128 || 127<result);
					break;
				case 16:
					setOFCF=(result<-32768 || 32767<result);
					break;
				case 32:
					setOFCF=(result<-0x80000000LL || 0x7FFFFFFFLL<result);
					break;
				}
				if(true==setOFCF)
				{
					SetCFOF();
				}
				else
				{
					ClearCFOF();
					//SetCF(false);
					//SetOF(false);
				}
			}
			HANDLE_EXCEPTION_IF_ANY;
		}
		break;


	case I486_RENUMBER_LEAVE://            0xC9,
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
			SetBP(Pop16(mem));
		}
		else
		{
			SetEBP(Pop32(mem));
		}
		break;


	case I486_RENUMBER_HLT://        0xF4,
		if(true==fidelity.IOPLException(*this,EXCEPTION_GP,mem,inst.numBytes))
		{
			EIPIncrement=0;
			clocksPassed=2;
			break;
		}
		if(0==(state.EFLAGS&EFLAGS_VIRTUAL86))
		{
			state.halt=true;
		}
		else
		{
			RaiseException(EXCEPTION_GP,0);
		}
		clocksPassed=4;
		break;


	case I486_RENUMBER_INC_DEC_R_M8: // 0xFE
		{
			if(op1.operandType==OPER_ADDR)
			{
				clocksPassed=3;
			}
			else
			{
				clocksPassed=1;
			}

			uint32_t i;
			uint8_t *operPtr=GetOperandPointer8(mem,inst.addressSize,inst.segOverride,op1,true);
			if(nullptr!=operPtr)
			{
				i=operPtr[0];
			}
			else
			{
				i=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1);
			}
			HANDLE_EXCEPTION_IF_ANY;

			switch(inst.GetREG())
			{
			case 0:
				IncrementByte(i);
				break;
			case 1:
				DecrementByte(i);
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				Abort("Undefined REG for "+cpputil::Ustox(inst.RealOpCode()));
				return 0;
			default:
				std_unreachable;
			}
			if(nullptr!=operPtr)
			{
				operPtr[0]=i;
			}
			else
			{
				StoreOperandValueRegOrMem8(op1,mem,inst.addressSize,inst.segOverride,i);
			}

			HANDLE_EXCEPTION_IF_ANY;
		}
		break;
	case I486_RENUMBER_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH: // 0xFF
		{
			auto REG=inst.GetREG();
			switch(REG)
			{
			case 0: // INC
				if(op1.operandType==OPER_ADDR)
				{
					clocksPassed=3;
				}
				else
				{
					clocksPassed=1;
				}
				if(16==inst.operandSize)
				{
					uint8_t *operPtr=GetOperandPointer16(mem,inst.addressSize,inst.segOverride,op1,true);
					uint32_t i;
					if(nullptr!=operPtr)
					{
						i=cpputil::GetWord(operPtr);
					}
					else
					{
						i=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op1);
					}
					HANDLE_EXCEPTION_IF_ANY;
					if(true!=state.exception)
					{
						IncrementWord(i);
						if(nullptr!=operPtr)
						{
							cpputil::PutWord(operPtr,i);
						}
						else
						{
							StoreOperandValueRegOrMem16(op1,mem,inst.addressSize,inst.segOverride,i);
							HANDLE_EXCEPTION_IF_ANY;
						}
					}
				}
				else
				{
					uint8_t *operPtr=GetOperandPointer32(mem,inst.addressSize,inst.segOverride,op1,true);
					uint32_t i;
					if(nullptr!=operPtr)
					{
						i=cpputil::GetDword(operPtr);
					}
					else
					{
						i=EvaluateOperandRegOrMem32(mem,inst.addressSize,inst.segOverride,op1);
					}
					HANDLE_EXCEPTION_IF_ANY;
					if(true!=state.exception)
					{
						IncrementDword(i);
						if(nullptr!=operPtr)
						{
							cpputil::PutDword(operPtr,i);
						}
						else
						{
							StoreOperandValueRegOrMem32(op1,mem,inst.addressSize,inst.segOverride,i);
							HANDLE_EXCEPTION_IF_ANY;
						}
					}
				}
				break;
			case 1: // DEC
				if(op1.operandType==OPER_ADDR)
				{
					clocksPassed=3;
				}
				else
				{
					clocksPassed=1;
				}
				if(16==inst.operandSize)
				{
					uint8_t *operPtr=GetOperandPointer16(mem,inst.addressSize,inst.segOverride,op1,true);
					uint32_t i;
					if(nullptr!=operPtr)
					{
						i=cpputil::GetWord(operPtr);
					}
					else
					{
						i=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op1);
					}
					HANDLE_EXCEPTION_IF_ANY;
					if(true!=state.exception)
					{
						DecrementWord(i);
						if(nullptr!=operPtr)
						{
							cpputil::PutWord(operPtr,i);
						}
						else
						{
							StoreOperandValueRegOrMem16(op1,mem,inst.addressSize,inst.segOverride,i);
							HANDLE_EXCEPTION_IF_ANY;
						}
					}
				}
				else
				{
					uint8_t *operPtr=GetOperandPointer32(mem,inst.addressSize,inst.segOverride,op1,true);
					uint32_t i;
					if(nullptr!=operPtr)
					{
						i=cpputil::GetDword(operPtr);
					}
					else
					{
						i=EvaluateOperandRegOrMem32(mem,inst.addressSize,inst.segOverride,op1);
					}
					HANDLE_EXCEPTION_IF_ANY;
					if(true!=state.exception)
					{
						DecrementDword(i);
						if(nullptr!=operPtr)
						{
							cpputil::PutDword(operPtr,i);
						}
						else
						{
							StoreOperandValueRegOrMem32(op1,mem,inst.addressSize,inst.segOverride,i);
							HANDLE_EXCEPTION_IF_ANY;
						}
					}
				}
				break;
			case 2: // CALL Indirect
				{
					clocksPassed=5;  // Same for CALL Indirect and JMP Indirect.
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					HANDLE_EXCEPTION_IF_ANY;
					if(true!=state.exception)
					{
						SAVE_ESP_BEFORE_PUSH_POP;
						Push(mem,inst.operandSize,state.EIP+inst.numBytes);
						HANDLE_EXCEPTION_PUSH_POP;
						if(true==enableCallStack)
						{
							PushCallStack(
							    false,0xffff,0xffff,
							    state.GetCR(0),
							    state.CS().value,state.EIP,inst.numBytes,
							    state.CS().value,value.GetAsDword(),
							    mem);
						}
						EIPIncrement=0;
						state.EIP=value.GetAsDword();
						if(16==inst.operandSize)
						{
							state.EIP&=0xFFFF;
						}
					}
				}
				break;
			case 4: // JMP Indirect
				{
					clocksPassed=5;  // Same for CALL Indirect and JMP Indirect.
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					HANDLE_EXCEPTION_IF_ANY;
					if(true!=state.exception)
					{
						EIPIncrement=0;
						state.EIP=value.GetAsDword();
						if(16==inst.operandSize)
						{
							state.EIP&=0xFFFF;
						}
					}
				}
				break;
			case 3: // CALLF Indirect
				{
					if(true==IsInRealMode())
					{
						clocksPassed=17;
					}
					else
					{
						clocksPassed=20;
					}

					EIPIncrement=0;

					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,(inst.operandSize+16)/8);
					HANDLE_EXCEPTION_IF_ANY;

					/* What is this?
					   FM TOWNS BIOS uses 
							MOV		AX,0110H
							MOV		FS,AX
							CALL	FAR PTR FS:[0040H]
						for reading from a mouse.  That is a perfect opportunity for the emulator to
						identify the operating system version.  The CPU class fires:
							mouseBIOSInterceptorPtr->Intercept();
						when indirect CALL to 0110:[0040H].
					*/
					if(nullptr!=mouseBIOSInterceptorPtr)
					{
						unsigned int offset;
						auto segPtr=ExtractSegmentAndOffset(offset,op1,inst.segOverride);
						if(0x0110==segPtr->value && 0x0040==offset)
						{
							mouseBIOSInterceptorPtr->InterceptMouseBIOS();
						}
					}

					auto destSeg=value.GetFwordSegment();
					auto destEIP=value.GetAsDword();

					SAVE_ESP_BEFORE_PUSH_POP;
					clocksPassed=CALLF(mem,inst.operandSize,inst.numBytes,destSeg,destEIP,clocksPassed);
					HANDLE_EXCEPTION_PUSH_POP;
				}
				break;
			case 5: // JMPF Indirect
				{
					if(op1.operandType==OPER_ADDR)
					{
						clocksPassed=3;
					}
					else
					{
						clocksPassed=1;
					}
					EIPIncrement=0;

					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,(inst.operandSize+16)/8);
					HANDLE_EXCEPTION_IF_ANY;
					JMPF(mem,inst.operandSize,inst.numBytes,value.GetFwordSegment(),value.GetAsDword(),clocksPassed);
				}
				break;
			case 6: // PUSH
				{
					clocksPassed=4;
					SAVE_ESP_BEFORE_PUSH_POP;
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					HANDLE_EXCEPTION_PUSH_POP;
					Push(mem,inst.operandSize,value.GetAsDword());
					HANDLE_EXCEPTION_PUSH_POP;
				}
				break;
			case 7:
				Abort("Undefined REG for "+cpputil::Ustox(inst.RealOpCode()));
				return 0;
			default:
				std_unreachable;
			}
		}
		break;
	case I486_RENUMBER_INC_EAX://    0x40, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			uint32_t value=GetAX();
			IncrementWord(value);
			SetAX(value);
		}
		else
		{
			IncrementDword(state.EAX());
		}
		break;
	case I486_RENUMBER_INC_ECX://    0x41, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			uint32_t value=GetCX();
			IncrementWord(value);
			SetCX(value);
		}
		else
		{
			IncrementDword(state.ECX());
		}
		break;
	case I486_RENUMBER_INC_EDX://    0x42, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			uint32_t value=GetDX();
			IncrementWord(value);
			SetDX(value);
		}
		else
		{
			IncrementDword(state.EDX());
		}
		break;
	case I486_RENUMBER_INC_EBX://    0x43, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			uint32_t value=GetBX();
			IncrementWord(value);
			SetBX(value);
		}
		else
		{
			IncrementDword(state.EBX());
		}
		break;
	case I486_RENUMBER_INC_ESP://    0x44, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			uint32_t value=GetSP();
			IncrementWord(value);
			SetSP(value);
		}
		else
		{
			IncrementDword(state.ESP());
		}
		break;
	case I486_RENUMBER_INC_EBP://    0x45, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			uint32_t value=GetBP();
			IncrementWord(value);
			SetBP(value);
		}
		else
		{
			IncrementDword(state.EBP());
		}
		break;
	case I486_RENUMBER_INC_ESI://    0x46, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			uint32_t value=GetSI();
			IncrementWord(value);
			SetSI(value);
		}
		else
		{
			IncrementDword(state.ESI());
		}
		break;
	case I486_RENUMBER_INC_EDI://    0x47, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		if(16==inst.operandSize)
		{
			uint32_t value=GetDI();
			IncrementWord(value);
			SetDI(value);
		}
		else
		{
			IncrementDword(state.EDI());
		}
		break;


	case I486_RENUMBER_INT3://       0xCC,
		Interrupt(3,mem,1,1,true);
		EIPIncrement=0;
		clocksPassed=26;
		break;
	case I486_RENUMBER_INT://        0xCD,
		clocksPassed=(IsInRealMode() ? 30 : 44);
		// if(true==fidelity.IOPLExceptionInVM86Mode(*this,EXCEPTION_GP,mem,inst.numBytes))
		// {
		// 	EIPIncrement=0;
		// 	break;
		// }
		Interrupt(inst.EvalUimm8(),mem,2,2,true);
		EIPIncrement=0;
		break;
	case I486_RENUMBER_INTO://       0xCE,
		if(GetOF())
		{
			Interrupt(INT_INTO_OVERFLOW, mem, 1, 1,true);
			EIPIncrement=0;
			clocksPassed=(IsInRealMode() ? 28 : 46);
		}
		else
		{
			clocksPassed=3;
		}
		break;


	case I486_RENUMBER_JMP_REL8://         0xEB,   // cb
		{
			auto offset=inst.EvalSimm8();
			clocksPassed=3;
			EIPIncrement=0;
			state.EIP=state.EIP+offset+inst.numBytes;
			if(16==inst.operandSize)
			{
				state.EIP&=0xFFFF;
			}
		}
		break;
	case I486_RENUMBER_JO_REL8:   // 0x70,
		CONDITIONALJUMP8(CondJO());
		break;
	case I486_RENUMBER_JNO_REL8:  // 0x71,
		CONDITIONALJUMP8(CondJNO());
		break;
	case I486_RENUMBER_JB_REL8:   // 0x72,
		CONDITIONALJUMP8(CondJB());
		break;
	case I486_RENUMBER_JAE_REL8:  // 0x73,
		CONDITIONALJUMP8(CondJAE());
		break;
	case I486_RENUMBER_JE_REL8:   // 0x74,
		CONDITIONALJUMP8(CondJE());
		break;
	case I486_RENUMBER_JECXZ_REL8:// 0xE3,  // Depending on the operand size
		{
			if(16==inst.addressSize)
			{
				CONDITIONALJUMP8(GetCX()==0);
			}
			else
			{
				CONDITIONALJUMP8(GetECX()==0);
			}
		}
		break;
	case I486_RENUMBER_JNE_REL8:  // 0x75,
		CONDITIONALJUMP8(CondJNE());
		break;
	case I486_RENUMBER_JBE_REL8:  // 0x76,
		CONDITIONALJUMP8(CondJBE());
		break;
	case I486_RENUMBER_JA_REL8:   // 0x77,
		CONDITIONALJUMP8(CondJA());
		break;
	case I486_RENUMBER_JS_REL8:   // 0x78,
		CONDITIONALJUMP8(CondJS());
		break;
	case I486_RENUMBER_JNS_REL8:  // 0x79,
		CONDITIONALJUMP8(CondJNS());
		break;
	case I486_RENUMBER_JP_REL8:   // 0x7A,
		CONDITIONALJUMP8(CondJP());
		break;
	case I486_RENUMBER_JNP_REL8:  // 0x7B,
		CONDITIONALJUMP8(CondJNP());
		break;
	case I486_RENUMBER_JL_REL8:   // 0x7C,
		CONDITIONALJUMP8(CondJL());
		break;
	case I486_RENUMBER_JGE_REL8:  // 0x7D,
		CONDITIONALJUMP8(CondJGE());
		break;
	case I486_RENUMBER_JLE_REL8:  // 0x7E,
		CONDITIONALJUMP8(CondJLE());
		break;
	case I486_RENUMBER_JG_REL8:   // 0x7F,
		CONDITIONALJUMP8(CondJG());
		break;
	case I486_RENUMBER_LOOP://             0xE2,
		if(32==inst.addressSize)
		{
			state.ECX()-=1;
			CONDITIONALJUMP8(0!=state.ECX());
		}
		else
		{
			unsigned int ctr=GetCX()-1;
			SetCX(ctr);
			CONDITIONALJUMP8(0!=ctr);
		}
		break;
	case I486_RENUMBER_LOOPE://            0xE1,
		if(32==inst.addressSize)
		{
			state.ECX()-=1;
			CONDITIONALJUMP8(0!=state.ECX() && true==GetZF());
		}
		else
		{
			unsigned int ctr=GetCX()-1;
			SetCX(ctr);
			CONDITIONALJUMP8(0!=ctr && true==GetZF());
		}
		break;
	case I486_RENUMBER_LOOPNE://           0xE0,
		if(32==inst.addressSize)
		{
			state.ECX()-=1;
			CONDITIONALJUMP8(0!=state.ECX() && true!=GetZF());
		}
		else
		{
			unsigned int ctr=GetCX()-1;
			SetCX(ctr);
			CONDITIONALJUMP8(0!=ctr && true!=GetZF());
		}
		break;


	case I486_RENUMBER_JA_REL://    0x0F87,
		CONDITIONALJUMP16OR32(CondJA());
		break;
	case I486_RENUMBER_JAE_REL://   0x0F83,
		CONDITIONALJUMP16OR32(CondJAE());
		break;
	case I486_RENUMBER_JB_REL://    0x0F82,
	// case I486_RENUMBER_JC_REL://    0x0F82, Same as JB_REL
		CONDITIONALJUMP16OR32(CondJB());
		break;
	case I486_RENUMBER_JBE_REL://   0x0F86,
		CONDITIONALJUMP16OR32(CondJBE());
		break;
	case I486_RENUMBER_JE_REL://    0x0F84,
	// case I486_RENUMBER_JZ_REL://    0x0F84, Same as JE_REL
		CONDITIONALJUMP16OR32(CondJE());
		break;
	case I486_RENUMBER_JG_REL://    0x0F8F,
		CONDITIONALJUMP16OR32(CondJG());
		break;
	case I486_RENUMBER_JGE_REL://   0x0F8D,
		CONDITIONALJUMP16OR32(CondJGE());
		break;
	case I486_RENUMBER_JL_REL://    0x0F8C,
		CONDITIONALJUMP16OR32(CondJL());
		break;
	case I486_RENUMBER_JLE_REL://   0x0F8E,
		CONDITIONALJUMP16OR32(CondJLE());
		break;
	// case I486_RENUMBER_JNA_REL://   0x0F86, Same as JBE_REL
	// case I486_RENUMBER_JNAE_REL://  0x0F82, Same as JB_REL
	// case I486_RENUMBER_JNB_REL://   0x0F83, Same as JAE_REL
	// case I486_RENUMBER_JNBE_REL://  0x0F87, Same as JA_REL
	// case I486_RENUMBER_JNC_REL://   0x0F83, Same as JAE_REL
	case I486_RENUMBER_JNE_REL://   0x0F85,
		CONDITIONALJUMP16OR32(CondJNE());
		break;
	// case I486_RENUMBER_JNG_REL://   0x0F8E, Same as JLE_REL
	// case I486_RENUMBER_JNGE_REL://  0x0F8C, Same as JL_REL
	// case I486_RENUMBER_JNL_REL://   0x0F8D, Same as JGE_REL
	// case I486_RENUMBER_JNLE_REL://  0x0F8F, Same as JG_REL
	case I486_RENUMBER_JNO_REL://   0x0F81,
		CONDITIONALJUMP16OR32(CondJNO());
		break;
	case I486_RENUMBER_JNP_REL://   0x0F8B,
		CONDITIONALJUMP16OR32(CondJNP());
		break;
	case I486_RENUMBER_JNS_REL://   0x0F89,
		CONDITIONALJUMP16OR32(CondJNS());
		break;
	// case I486_RENUMBER_JNZ_REL://   0x0F85, Same as JNE_REL
	case I486_RENUMBER_JO_REL://    0x0F80,
		CONDITIONALJUMP16OR32(CondJO());
		break;
	case I486_RENUMBER_JP_REL://    0x0F8A,
		CONDITIONALJUMP16OR32(CondJP());
		break;
	// case I486_RENUMBER_JPE_REL://   0x0F8A, Same as JP_REL
	// case I486_RENUMBER_JPO_REL://   0x0F8B, Same as JNP_REL
	case I486_RENUMBER_JS_REL://    0x0F88,
		CONDITIONALJUMP16OR32(CondJS());
		break;


	case I486_RENUMBER_JMP_FAR:
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

			JMPF(mem,inst.operandSize,inst.numBytes,op1.seg,op1.offset,clocksPassed);

			EIPIncrement=0;
		}
		break;


	case I486_RENUMBER_BINARYOP_RM8_FROM_I8://=  0x80, // AND(REG=4), OR(REG=1), or XOR(REG=6) depends on the REG field of MODR/M
	case I486_RENUMBER_BINARYOP_RM8_FROM_I8_ALIAS:
		{
			if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR)
			{
				clocksPassed=3;
			}
			else
			{
				clocksPassed=1;
			}
			auto REG=inst.GetREG();

			uint32_t i;
			auto operPtr=GetOperandPointer8(mem,inst.addressSize,inst.segOverride,op1,(7!=REG)); // forWrite is true if REG!=7 (CMP)
			if(nullptr!=operPtr)
			{
				i=operPtr[0];
			}
			else
			{
				i=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1);
			}
			auto value2=inst.EvalUimm8();
			if(true==state.exception)
			{
				fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes);
				EIPIncrement=0;
				break;
			}

			switch(REG)
			{
			case 0:
				AddByte(i,value2);
				break;
			case 1:
				OrByte(i,value2);
				break;
			case 2:
				AdcByte(i,value2);
				break;
			case 3:
				SbbByte(i,value2);
				break;
			case 4:
				AndByte(i,value2);
				break;
			case 5:
				SubByte(i,value2);
				break;
			case 6:
				XorByte(i,value2);
				break;
			case 7: // CMP
				SubByte(i,value2);
				break;
			default:
				std_unreachable;
			}
			if(7!=REG) // Don't store a value if it is CMP
			{
				if(nullptr!=operPtr)
				{
					operPtr[0]=i;
				}
				else
				{
					StoreOperandValueRegOrMem8(op1,mem,inst.addressSize,inst.segOverride,cpputil::LowByte(i));
					if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
					{
						EIPIncrement=0;
					}
				}
			}
		}
		break;

	case I486_RENUMBER_BINARYOP_R_FROM_I://=     0x81,
	case I486_RENUMBER_BINARYOP_RM_FROM_SXI8://= 0x83, Sign of op2 is already extended when decoded.
		{
			if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR)
			{
				clocksPassed=3;
			}
			else
			{
				clocksPassed=1;
			}

			auto REG=inst.GetREG();
			OperandValue value1;
			uint32_t i,value2;
			if(16==inst.operandSize)
			{
				auto operPtr=GetOperandPointer16(mem,inst.addressSize,inst.segOverride,op1,(7!=REG)); // forWrite is true if REG!=7 (CMP)
				if(nullptr!=operPtr)
				{
					i=cpputil::GetWord(operPtr);
				}
				else
				{
					value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,16/8);
					i=value1.GetAsDword();
				}

				if(I486_OPCODE_BINARYOP_R_FROM_I==inst.opCode)
				{
					value2=inst.EvalUimm16();
				}
				else
				{
					value2=inst.EvalSimm8();
				}
				HANDLE_EXCEPTION_IF_ANY;

				switch(REG)
				{
				case 0:
					AddWord(i,value2);
					break;
				case 1:
					OrWord(i,value2);
					break;
				case 2:
					AdcWord(i,value2);
					break;
				case 3:
					SbbWord(i,value2);
					break;
				case 4:
					AndWord(i,value2);
					break;
				case 5:
					SubWord(i,value2);
					break;
				case 6:
					XorWord(i,value2);
					break;
				case 7:
					SubWord(i,value2);
					break;
				default:
					std_unreachable;
				}
				HANDLE_EXCEPTION_IF_ANY;
				if(7!=REG) // Don't store a value if it is CMP
				{
					if(nullptr!=operPtr)
					{
						cpputil::PutWord(operPtr,i);
					}
					else
					{
						value1.SetDword(i);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
						HANDLE_EXCEPTION_IF_ANY;
					}
				}
			}
			else // 32-bit operand size
			{
				auto operPtr=GetOperandPointer32(mem,inst.addressSize,inst.segOverride,op1,(7!=REG)); // forWrite is true if REG!=7 (CMP)
				if(nullptr!=operPtr)
				{
					i=cpputil::GetDword(operPtr);
				}
				else
				{
					value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,32/8);
					i=value1.GetAsDword();
				}

				if(I486_OPCODE_BINARYOP_R_FROM_I==inst.opCode)
				{
					value2=inst.EvalUimm32();
				}
				else
				{
					value2=inst.EvalSimm8();
				}
				HANDLE_EXCEPTION_IF_ANY;

				switch(REG)
				{
				case 0:
					AddDword(i,value2);
					break;
				case 1:
					OrDword(i,value2);
					break;
				case 2:
					AdcDword(i,value2);
					break;
				case 3:
					SbbDword(i,value2);
					break;
				case 4:
					AndDword(i,value2);
					break;
				case 5:
					SubDword(i,value2);
					break;
				case 6:
					XorDword(i,value2);
					break;
				case 7:
					SubDword(i,value2);
					break;
				default:
					std_unreachable;
				}
				HANDLE_EXCEPTION_IF_ANY;
				if(7!=REG) // Don't store a value if it is CMP
				{
					if(nullptr!=operPtr)
					{
						cpputil::PutDword(operPtr,i);
					}
					else
					{
						value1.SetDword(i);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
						HANDLE_EXCEPTION_IF_ANY;
					}
				}
			}
		}
		break;


	case I486_RENUMBER_LAHF://=             0x9F,
		SetAH(state.EFLAGS&0xFF);
		clocksPassed=2;
		break;


	case I486_RENUMBER_LEA://=              0x8D,
		clocksPassed=1;
		if(OPER_ADDR==op2.operandType)
		{
			unsigned int offset=
			    state.NULL_and_reg32[op2.baseReg&15]+
			   (state.NULL_and_reg32[op2.indexReg&15]<<op2.indexShift)+
			   op2.offset;
			if(OPER_REG32==op1.operandType)
			{
				state.reg32()[op1.reg-REG_EAX]=offset;
			}
			else if(OPER_REG16==op1.operandType)
			{
				SET_INT_LOW_WORD(state.reg32()[op1.reg-REG_AX],offset&0xFFFF);
			}
			else
			{
				RaiseException(EXCEPTION_UD,0);
			}
		}
		else
		{
			RaiseException(EXCEPTION_UD,0);
		}
		break;


	case I486_RENUMBER_LDS://              0xC5,
		LOAD_FAR_POINTER(DS);
		if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
		{
			EIPIncrement=0;
		}
		break;
	case I486_RENUMBER_LES://              0xC4,
		LOAD_FAR_POINTER(ES);
		if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
		{
			EIPIncrement=0;
		}
		break;
	case I486_RENUMBER_LFS://              0x0FB4,
		LOAD_FAR_POINTER(FS);
		if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
		{
			EIPIncrement=0;
		}
		break;
	case I486_RENUMBER_LGS://              0x0FB5,
		LOAD_FAR_POINTER(GS);
		if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
		{
			EIPIncrement=0;
		}
		break;
	case I486_RENUMBER_LSS://              0x0FB2,
		if(OPER_ADDR==op2.operandType)
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,(inst.operandSize+16)/8);
			if(true!=state.exception)
			{
				// op1 is a register.
				auto regNum=inst.GetREG();
				if(16==inst.operandSize)
				{
					SET_INT_LOW_WORD(state.reg32()[regNum],value.GetAsWord());
				}
				else
				{
					state.reg32()[regNum]=value.GetAsDword();
				}

				auto seg=value.GetFwordSegment();

				if(0==seg)
				{
					RaiseException(EXCEPTION_GP,0);
				}
				else
				{
					LoadSegmentRegister(state.SS(),seg,mem);
				}
			}
			clocksPassed=9;  // It is described as 6/12, but what makes it 6 clocks or 12 clocks is not given.  Quaaaaack!!!!
		}
		else
		{
			RaiseException(EXCEPTION_GP,0);
		}
		if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
		{
			EIPIncrement=0;
		}
		break;


	case I486_RENUMBER_LODSB://            0xAC,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		{
			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);
			if(true==REPCheck(clocksPassed,prefix,inst.addressSize))
			{
				auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
				auto newAL=FetchByte(inst.addressSize,seg,state.ESI(),mem);
				if(true!=state.exception)
				{
					SetAL(newAL);
					UpdateSIorESIAfterStringOp(inst.addressSize,8);
					if(INST_PREFIX_REP==prefix)
					{
						EIPIncrement=0;
					}
				}
				else
				{
					SetECX(ECX);
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
				}
				clocksPassed+=5;
				ECX=state.ECX();
			}
		}
		break;
	case I486_RENUMBER_LODS://             0xAD,
		{
			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);
			if(true==REPCheck(clocksPassed,prefix,inst.addressSize))
			{
				auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
				unsigned int newEAX=FetchWordOrDword(inst.operandSize,inst.addressSize,seg,state.ESI(),mem);
				if(true!=state.exception)
				{
					if(16==inst.operandSize)
					{
						SetAX(FetchWord(inst.addressSize,seg,state.ESI(),mem));
					}
					else
					{
						SetEAX(FetchDword(inst.addressSize,seg,state.ESI(),mem));
					}
					UpdateSIorESIAfterStringOp(inst.addressSize,inst.operandSize);
					if(INST_PREFIX_REP==prefix)
					{
						EIPIncrement=0;
					}
				}
				else
				{
					SetECX(ECX);
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
				}
				clocksPassed+=5;
				ECX=state.ECX();
			}
		}
		break;


	case I486_RENUMBER_LGDT_LIDT_SGDT_SIDT:
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
					InvalidateDescriptorCache();
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
					Interrupt(6,mem,0,0,false);
					EIPIncrement=0;
				}
				else
				{
					RaiseException(EXCEPTION_UD,0);
					EIPIncrement=0;
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
					Interrupt(6,mem,0,0,false);
					EIPIncrement=0;
				}
				else
				{
					RaiseException(EXCEPTION_UD,0);
					EIPIncrement=0;
				}
			}
			break;
		case 4: // SMSW
			clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 2);
			{
				OperandValue value;
				value.MakeWord(state.GetCR(0));
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
				state.mode=state.RecalculateMode();
			}
			break;
		case 7: // INVLPG
			clocksPassed=11;
			if(IsInRealMode())
			{
				RaiseException(EXCEPTION_UD,0);
				EIPIncrement=0;
			}
			else if(0!=state.CS().DPL)
			{
				RaiseException(EXCEPTION_GP,0);
				EIPIncrement=0;
			}
			else
			{
				if(OPER_ADDR==op1.operandType)
				{
					unsigned int offset;
					const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op1,inst.segOverride);
					if(16==inst.addressSize)
					{
						offset&=0xFFFF;
					}
					auto linearAddr=seg.baseLinearAddr+offset; // Tentative.

					auto pageIndex=(linearAddr>>LINEARADDR_TO_PAGE_SHIFT);
					state.pageTableCache[pageIndex].valid=state.pageTableCacheValidCounter-1;
				}
				else
				{
					RaiseException(EXCEPTION_UD,0);
					EIPIncrement=0;
				}
			}
			break;
		default:
			Abort("Undefined REG for "+cpputil::Ustox(inst.RealOpCode()));
			return 0;
		}
		break;


	case I486_RENUMBER_LSL://              0x0F03,
		clocksPassed=10;
		if(MODE_NATIVE==state.mode) // <-> (true!=IsInRealMode() && true!=GetVM())
		{
			auto selectorValue=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8); // What to do with high 16 bits?
			auto selector=selectorValue.GetAsWord();
			if(true!=state.exception)
			{
				i486DXCommon::LoadSegmentRegisterTemplate<const i486DXCommon,FIDELITY> loader;
				loader.LoadProtectedModeDescriptor(*this,selector,mem);
				if(true==state.exception || nullptr==loader.rawDesc)
				{
					SetZF(false);
					break;
				}

				// Should I check P flag or not?
				//const uint8_t Pflag=(loader.rawDesc[5]&0x80);
				//if(0==Pflag)
				//{
				//	SetZF(false);
				//	break;
				//}

				uint8_t CPL=state.CS().DPL;
				uint8_t RPL=selector&3;
				uint8_t descDPL=(loader.rawDesc[5]>>5)&3;
				uint8_t descType=loader.rawDesc[5]&0x1F;
				if(0!=(descType&0x10))
				{
					descType&=~1;
				}

				bool valid=false;
				switch(descType)
				{
				case DESCTYPE_AVAILABLE_286_TSS://               1,
				case DESCTYPE_LDT://                             2,
				case DESCTYPE_BUSY_286_TSS://                    3,
				// case 8: // Should LSL return for this one????  80386 Programmer's Reference Manual 1986 tells so.
				//            But actually measurement tells otherwise.
				case DESCTYPE_AVAILABLE_386_TSS://               9,
				case DESCTYPE_BUSY_386_TSS://                 0x0B,
				// Reserved                            0x0D
				case SEGTYPE_DATA_NORMAL_READONLY://       0b10000, // Data Normal         Read-Only
				case SEGTYPE_DATA_NORMAL_RW://             0b10010, // Data Normal         Read/Write
				case SEGTYPE_DATA_EXPAND_DOWN_READONLY://  0b10100, // Data Expand-Down    Read-Only
				case SEGTYPE_DATA_EXPAND_DOWN_RW://        0b10110, // Data Expand-Down    Read/Write
				case SEGTYPE_CODE_NONCONFORMING_EXECONLY://0b11000, // Code Non-Conforming Execute-Only
				case SEGTYPE_CODE_NONCONFORMING_READABLE://0b11010, // Code Non-Conforming Readable
					if(CPL<=descDPL && RPL<=descDPL)
					{
						valid=true;
					}
					break;

				case SEGTYPE_CODE_CONFORMING_EXECONLY://   0b11100, // Code Conforming     Execute-Only
				case SEGTYPE_CODE_CONFORMING_READABLE://   0b11110, // Code Conforming     Readable
					valid=true;
					break;

				case DESCTYPE_CALL_GATE://                       4,
				case DESCTYPE_TASK_GATE://                       5,
				case DESCTYPE_286_INT_GATE://                    6,
				case DESCTYPE_286_TRAP_GATE://                   7,
				case DESCTYPE_386_CALL_GATE://                0x0C,
				case DESCTYPE_386_INT_GATE://                 0x0E,
				case DESCTYPE_386_TRAP_GATE://                0x0F,
					break;
				}

				if(true==valid)
				{
					unsigned int segLimit=cpputil::GetWord(loader.rawDesc+0)|((loader.rawDesc[6]&0x0F)<<16);
					if(0x80&loader.rawDesc[6])
					{
						segLimit<<=12;
						segLimit|=0xFFF;
					}

					OperandValue limit;
					limit.MakeDword(segLimit);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,limit);

					SetZF(true);
				}
				else
				{
					SetZF(false);
				}
			}
		}
		else
		{
			Abort("LSL from Real or VM86 mode.  Should be INT 6, but for the time being, I am stopping the VM.");
		}
		break;


	case I486_RENUMBER_MOV_FROM_SEG: //     0x8C,
		clocksPassed=3;
		{
			auto seg=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,2);
			HANDLE_EXCEPTION_IF_ANY;
			seg.byteData[2]=0;
			seg.byteData[3]=0;
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,seg);
			HANDLE_EXCEPTION_IF_ANY;
		}
		break;
	case I486_RENUMBER_MOV_TO_SEG: //       0x8E,
		if(true==IsInRealMode())
		{
			clocksPassed=3;
		}
		else
		{
			clocksPassed=9;
		}

		// I don't think it was the reason why Windows 3.1 stops while booting, but MOV CS,r_m apparently causes UD exception.
		if(true==fidelity.UDException_MOV_TO_CS(*this,op1.reg,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,op1.GetSize());
			HANDLE_EXCEPTION_IF_ANY;
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			HANDLE_EXCEPTION_IF_ANY;
		}
		break;
	case I486_RENUMBER_MOV_M_TO_AL: //      0xA0,
		{
			clocksPassed=1;
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			auto byteData=FetchByte(inst.addressSize,seg,inst.EvalUimm32(),mem);
			HANDLE_EXCEPTION_IF_ANY;
			SetAL(byteData);
		}
		break;
	case I486_RENUMBER_MOV_M_TO_EAX: //     0xA1, // 16/32 depends on OPSIZE_OVERRIDE
		{
			clocksPassed=1;
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			if(16==inst.operandSize)
			{
				unsigned int value=FetchWord(inst.addressSize,seg,inst.EvalUimm32(),mem);
				HANDLE_EXCEPTION_IF_ANY;
				SetAX(value);
			}
			else
			{
				unsigned int value=FetchWordOrDword(inst.operandSize,inst.addressSize,seg,inst.EvalUimm32(),mem);
				HANDLE_EXCEPTION_IF_ANY;
				state.EAX()=value;
			}
		}
		break;
	case I486_RENUMBER_MOV_M_FROM_AL: //    0xA2,
		{
			clocksPassed=1;
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			StoreByte(mem,inst.addressSize,seg,inst.EvalUimm32(),GetAL());
			if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
			{
				EIPIncrement=0;
				break;
			}
		}
		break;
	case I486_RENUMBER_MOV_M_FROM_EAX: //   0xA3, // 16/32 depends on OPSIZE_OVERRIDE
		{
			clocksPassed=1;
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			StoreWordOrDword(mem,inst.operandSize,inst.addressSize,seg,inst.EvalUimm32(),GetEAX());
			if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
			{
				EIPIncrement=0;
				break;
			}
		}
		break;
	case I486_RENUMBER_MOV_I8_TO_RM8: //    0xC6,
		{
			OperandValue src;
			src.MakeByte(inst.EvalUimm8());
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,src);
			clocksPassed=1;
			if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
			{
				EIPIncrement=0;
				break;
			}
		}
		break;
	case I486_RENUMBER_MOV_I_TO_RM: //      0xC7, // 16/32 depends on OPSIZE_OVERRIDE
		{
			OperandValue src;
			if(16==inst.operandSize)
			{
				src.MakeWord(inst.EvalUimm16());
			}
			else
			{
				src.MakeDword(inst.EvalUimm32());
			}
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,src);
			clocksPassed=1;
			if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
			{
				EIPIncrement=0;
				break;
			}
		}
		break;

	case I486_RENUMBER_MOV_FROM_R: //       0x89, // 16/32 depends on OPSIZE_OVERRIDE
		{
			auto regNum=inst.GetREG();
			if(16==inst.operandSize)
			{
				uint32_t value=INT_LOW_WORD(state.reg32()[regNum]);
				StoreOperandValueRegOrMem16(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			else
			{
				uint32_t value=state.reg32()[regNum];
				StoreOperandValueRegOrMem32(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			clocksPassed=1;
			if(true==state.exception)
			{
				HandleException(true,mem,inst.numBytes);
				EIPIncrement=0;
			}
		}
		break;
	case I486_RENUMBER_MOV_TO_R: //         0x8B, // 16/32 depends on OPSIZE_OVERRIDE
		{
			if(16==inst.operandSize)
			{
				auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
				auto value=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op2);
				if(true!=state.exception)
				{
					SET_INT_LOW_WORD(state.reg32()[regNum],value);
				}
				else
				{
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
				}
			}
			else
			{
				auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
				auto value=EvaluateOperandRegOrMem32(mem,inst.addressSize,inst.segOverride,op2);
				if(true!=state.exception)
				{
					state.reg32()[regNum]=value;
				}
				else
				{
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
				}
			}
			clocksPassed=1;
		}
		break;

	case I486_RENUMBER_MOV_FROM_R8: //      0x88,
		{
			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
		#ifdef YS_LITTLE_ENDIAN
			unsigned int value=*state.reg8Ptr[regNum];
		#else
			unsigned int value=(255&(state.reg32()[regNum&3]>>reg8Shift[regNum]));
		#endif
			StoreOperandValueRegOrMem8(op1,mem,inst.addressSize,inst.segOverride,cpputil::LowByte(value));
			clocksPassed=1;
			HANDLE_EXCEPTION_IF_ANY;
		}
		break;
	case I486_RENUMBER_MOV_TO_R8: //        0x8A,
		{
			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
			uint32_t value=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op2);
			if(true!=state.exception)
			{
			#ifdef YS_LITTLE_ENDIAN
				*state.reg8Ptr[regNum]=value;
			#else
				state.reg32()[regNum&3]&=reg8AndPattern[regNum];
				state.reg32()[regNum&3]|=(value<<reg8Shift[regNum]);
			#endif
			}
			else
			{
				HandleException(true,mem,inst.numBytes);
				EIPIncrement=0;
			}
			clocksPassed=1;
		}
		break;

	case I486_RENUMBER_MOV_I8_TO_AL: //     0xB0,
		SetAL(inst.EvalUimm8());
		clocksPassed=1;
		break;
	case I486_RENUMBER_MOV_I8_TO_CL: //     0xB1,
		SetCL(inst.EvalUimm8());
		clocksPassed=1;
		break;
	case I486_RENUMBER_MOV_I8_TO_DL: //     0xB2,
		SetDL(inst.EvalUimm8());
		clocksPassed=1;
		break;
	case I486_RENUMBER_MOV_I8_TO_BL: //     0xB3,
		SetBL(inst.EvalUimm8());
		clocksPassed=1;
		break;

	case I486_RENUMBER_MOV_I8_TO_AH: //     0xB4,
		SetAH(inst.EvalUimm8());
		clocksPassed=1;
		break;
	case I486_RENUMBER_MOV_I8_TO_CH: //     0xB5,
		SetCH(inst.EvalUimm8());
		clocksPassed=1;
		break;
	case I486_RENUMBER_MOV_I8_TO_DH: //     0xB6,
		SetDH(inst.EvalUimm8());
		clocksPassed=1;
		break;
	case I486_RENUMBER_MOV_I8_TO_BH: //     0xB7,
		SetBH(inst.EvalUimm8());
		clocksPassed=1;
		break;

	case I486_RENUMBER_MOV_I_TO_EAX: //     0xB8, // 16/32 depends on OPSIZE_OVERRIDE
		MOV_REG32_FROM_I(REG_EAX);
		break;
	case I486_RENUMBER_MOV_I_TO_ECX: //     0xB9, // 16/32 depends on OPSIZE_OVERRIDE
		MOV_REG32_FROM_I(REG_ECX);
		break;
	case I486_RENUMBER_MOV_I_TO_EDX: //     0xBA, // 16/32 depends on OPSIZE_OVERRIDE
		MOV_REG32_FROM_I(REG_EDX);
		break;
	case I486_RENUMBER_MOV_I_TO_EBX: //     0xBB, // 16/32 depends on OPSIZE_OVERRIDE
		MOV_REG32_FROM_I(REG_EBX);
		break;
	case I486_RENUMBER_MOV_I_TO_ESP: //     0xBC, // 16/32 depends on OPSIZE_OVERRIDE
		MOV_REG32_FROM_I(REG_ESP);
		break;
	case I486_RENUMBER_MOV_I_TO_EBP: //     0xBD, // 16/32 depends on OPSIZE_OVERRIDE
		MOV_REG32_FROM_I(REG_EBP);
		break;
	case I486_RENUMBER_MOV_I_TO_ESI: //     0xBE, // 16/32 depends on OPSIZE_OVERRIDE
		MOV_REG32_FROM_I(REG_ESI);
		break;
	case I486_RENUMBER_MOV_I_TO_EDI: //     0xBF, // 16/32 depends on OPSIZE_OVERRIDE
		MOV_REG32_FROM_I(REG_EDI);
		break;


	case I486_RENUMBER_MOV_TO_CR://        0x0F22,
		if(0==(state.EFLAGS&EFLAGS_VIRTUAL86))
		{
			unsigned int MODR_M=inst.operand[0];
			auto crNum=((MODR_M>>3)&3); // I think it should be &3 not &7.  Only CR0 to CR3.
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,4);
			SetCR(crNum,value.GetAsDword(),mem);
			state.mode=state.RecalculateMode();
		}
		else
		{
			RaiseException(EXCEPTION_GP,0);
			HandleException(false,mem,inst.numBytes);
			EIPIncrement=0;
		}
		clocksPassed=16;
		break;
	case I486_RENUMBER_MOV_FROM_CR://      0x0F20,
		if(0==(state.EFLAGS&EFLAGS_VIRTUAL86))
		{
			// Used to be Move(mem,inst.addressSize,inst.segOverride,op1,op2);
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,op1.GetSize());
			HANDLE_EXCEPTION_IF_ANY;
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			HANDLE_EXCEPTION_IF_ANY;
		}
		else
		{
			RaiseException(EXCEPTION_GP,0);
			HandleException(false,mem,inst.numBytes);
			EIPIncrement=0;
		}
		clocksPassed=4;
		break;
	case I486_RENUMBER_MOV_FROM_DR://      0x0F21,
		// Used to be Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=10;
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,op1.GetSize());
			HANDLE_EXCEPTION_IF_ANY;
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			HANDLE_EXCEPTION_IF_ANY;
		}
		break;
	case I486_RENUMBER_MOV_TO_DR://        0x0F23,
		// Used to be Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=11;
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,op1.GetSize());
			HANDLE_EXCEPTION_IF_ANY;
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			HANDLE_EXCEPTION_IF_ANY;
		}
		break;
	case I486_RENUMBER_MOV_FROM_TR://      0x0F24,
		// Used to be Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=4;  // 3 for TR3 strictly speaking.
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,op1.GetSize());
			HANDLE_EXCEPTION_IF_ANY;
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			HANDLE_EXCEPTION_IF_ANY;
		}
		break;
	case I486_RENUMBER_MOV_TO_TR://        0x0F26,
		// Used to be Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=4;  // 6 for TR6 strictly speaking.
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,op1.GetSize());
			HANDLE_EXCEPTION_IF_ANY;
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			HANDLE_EXCEPTION_IF_ANY;
		}
		break;


	case I486_RENUMBER_MOVSB://            0xA4,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		{
			clocksPassed=7;
			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);

			#define MOVSB_Template(addrSize)\
				for(int ctr=0;\
				    ctr<MAX_REP_BUNDLE_COUNT &&\
				    true==REPCheckA##addrSize(clocksPassed,prefix);\
				    ++ctr)\
				{\
					auto data=FetchByte(addrSize,seg,state.ESI(),mem);\
					StoreByte(mem,addrSize,state.ES(),state.EDI(),data);\
					if(true!=state.exception)\
					{\
						UpdateESIandEDIAfterStringOpO8A##addrSize();\
						if(INST_PREFIX_REP==prefix)\
						{\
							EIPIncrement=0;\
						}\
						else\
						{\
							EIPIncrement=inst.numBytes;\
							break;\
						}\
					}\
					else\
					{\
						SetECX(ECX);\
						HandleException(true,mem,inst.numBytes);\
						EIPIncrement=0;\
						break;\
					}\
					ECX=state.ECX();\
				}

			if(16==inst.addressSize)
			{
				MOVSB_Template(16);
			}
			else
			{
				MOVSB_Template(32);
			}
		}
		break;
	case I486_RENUMBER_MOVS://             0xA5,
		{
			#define MOVS_Template(addrSize,FetchFunc,StoreFunc,UpdateFunc) \
				for(int ctr=0; \
				    ctr<MAX_REP_BUNDLE_COUNT && \
				    true==REPCheckA##addrSize(clocksPassed,prefix); \
				    ++ctr) \
				{ \
					auto data=(FetchFunc)((addrSize),seg,state.ESI(),mem); \
					(StoreFunc)(mem,(addrSize),state.ES(),state.EDI(),data); \
					if(true!=state.exception) \
					{ \
						(UpdateFunc)(); \
						if(INST_PREFIX_REP==prefix) \
						{ \
							EIPIncrement=0; \
						} \
						else \
						{ \
							EIPIncrement=inst.numBytes; \
							break; \
						} \
					} \
					else \
					{ \
						SetECX(ECX); \
						HandleException(true,mem,inst.numBytes); \
						EIPIncrement=0; \
						break; \
					} \
					ECX=state.ECX(); \
				}

			clocksPassed=7;
			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			if(16==inst.operandSize)
			{
				if(16==inst.addressSize)
				{
					MOVS_Template(16,FetchWord,StoreWord,UpdateESIandEDIAfterStringOpO16A16);
				}
				else
				{
					MOVS_Template(32,FetchWord,StoreWord,UpdateESIandEDIAfterStringOpO16A32);
				}
			}
			else // 32-bit operandSize
			{
				if(16==inst.addressSize)
				{
					MOVS_Template(16,FetchDword,StoreDword,UpdateESIandEDIAfterStringOpO32A16);
				}
				else
				{
					MOVS_Template(32,FetchDword,StoreDword,UpdateESIandEDIAfterStringOpO32A32);
				}
			}
		}
		break;


	case I486_RENUMBER_MOVSX_R_RM8://=      0x0FBE,
	case I486_RENUMBER_MOVZX_R_RM8://=      0x0FB6, 8bit to 16or32bit
		{
			clocksPassed=3;
			uint32_t value=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op2);
			if(true!=state.exception)
			{
				if(I486_OPCODE_MOVZX_R_RM8==inst.opCode || 0==(value&0x80))
				{
					value&=0xFF;
				}
				else
				{
					value|=0xFFFFFF00;
				}

				// op1 is a register.
				auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
				if(32==inst.operandSize)
				{
					state.reg32()[regNum]=value;
				}
				else // if(16==operandSize)
				{
					SET_INT_LOW_WORD(state.reg32()[regNum],cpputil::LowWord(value));
				}
			}
			else
			{
				HandleException(true,mem,inst.numBytes);
				EIPIncrement=0;
			}
		}
		break;
	case I486_RENUMBER_MOVSX_R32_RM16://=   0x0FBF,
		{
			clocksPassed=3;
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,2);
			if(true!=state.exception)
			{
				// op1 is a register.
				auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
				if(0!=(value.byteData[1]&0x80))
				{
					cpputil::PutWord(value.byteData+2,0xFFFF);
					state.reg32()[regNum]=cpputil::GetDword(value.byteData);
				}
				else
				{
					state.reg32()[regNum]=cpputil::GetWord(value.byteData);
				}
			}
			else
			{
				HandleException(true,mem,inst.numBytes);
				EIPIncrement=0;
			}
		}
		break;
	case I486_RENUMBER_MOVZX_R32_RM16://=   0x0FB7, 16bit to 32bit
		{
			clocksPassed=3;
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,2);
			if(true!=state.exception)
			{
				// op1 is a register.
				auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
				state.reg32()[regNum]=cpputil::GetWord(value.byteData);
			}
			else
			{
				HandleException(true,mem,inst.numBytes);
				EIPIncrement=0;
			}
		}
		break;


	case I486_RENUMBER_NOP://              0x90,
		clocksPassed=1;
		break;


	case I486_RENUMBER_OUT_I8_AL: //        0xE6,
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=11; // 31 if CPL>IOPL
			if(true==fidelity.TakeIOWriteException(*this,inst.EvalUimm8(),1,mem,inst.numBytes))
			{
				EIPIncrement=0;
				break;
			}
		}
		IOOut8(io,inst.EvalUimm8(),GetAL());
		break;
	case I486_RENUMBER_OUT_I8_A: //         0xE7,
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=11; // 31 if CPL>IOPL
			if(true==fidelity.TakeIOWriteException(*this,inst.EvalUimm8(),inst.operandSize>>3,mem,inst.numBytes))
			{
				EIPIncrement=0;
				break;
			}
		}
		if(16==inst.operandSize)
		{
			IOOut16(io,inst.EvalUimm8(),GetAX());
		}
		else
		{
			IOOut32(io,inst.EvalUimm8(),GetEAX());
		}
		break;
	case I486_RENUMBER_OUT_DX_AL: //        0xEE,
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=10; // 30 if CPL>IOPL
			if(true==fidelity.TakeIOWriteException(*this,GetDX(),1,mem,inst.numBytes))
			{
				EIPIncrement=0;
				break;
			}
		}
		IOOut8(io,GetDX(),GetAL());
		break;
	case I486_RENUMBER_OUT_DX_A: //         0xEF,
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=10; // 30 if CPL>IOPL
			if(true==fidelity.TakeIOWriteException(*this,GetDX(),inst.operandSize>>3,mem,inst.numBytes))
			{
				EIPIncrement=0;
				break;
			}
		}
		if(16==inst.operandSize)
		{
			IOOut16(io,GetDX(),GetAX());
		}
		else
		{
			IOOut32(io,GetDX(),GetEAX());
		}
		break;


	case I486_RENUMBER_OUTSB://            0x6E,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		{
			auto prefix=REPNEtoREP(inst.instPrefix);
			if(true==REPCheck(clocksPassed,prefix,inst.addressSize))
			{
				clocksPassed+=(IsInRealMode() ? 17 : 10); // Protected Mode 32 if CPL>IOPL
				if(true==fidelity.TakeIOWriteException(*this,GetDX(),1,mem,inst.numBytes))
				{
					EIPIncrement=0;
					break;
				}
				auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
				IOOut8(io,GetDX(),FetchByte(inst.addressSize,seg,state.ESI(),mem));
				UpdateSIorESIAfterStringOp(inst.addressSize,8);
				if(INST_PREFIX_REP==prefix)
				{
					EIPIncrement=0;
				}
			}
		}
		break;
	case I486_RENUMBER_OUTS://             0x6F,
		{
			auto prefix=REPNEtoREP(inst.instPrefix);
			if(true==REPCheck(clocksPassed,prefix,inst.addressSize))
			{
				clocksPassed+=(IsInRealMode() ? 17 : 10); // Protected Mode 32 if CPL>IOPL
				if(true==fidelity.TakeIOWriteException(*this,GetDX(),inst.operandSize>>3,mem,inst.numBytes))
				{
					EIPIncrement=0;
					break;
				}
				auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
				if(16==inst.operandSize)
				{
					IOOut16(io,GetDX(),FetchWord(inst.addressSize,seg,state.ESI(),mem));
				}
				else
				{
					IOOut32(io,GetDX(),FetchDword(inst.addressSize,seg,state.ESI(),mem));
				}
				UpdateSIorESIAfterStringOp(inst.addressSize,inst.operandSize);
				if(INST_PREFIX_REP==prefix)
				{
					EIPIncrement=0;
				}
			}
		}
		break;


	case I486_RENUMBER_PUSHA://            0x60,
		clocksPassed=11;
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			auto temp=state.ESP();
			Push(mem,inst.operandSize,state.EAX(),state.ECX(),state.EDX());
			Push(mem,inst.operandSize,state.EBX(),temp       ,state.EBP());
			Push(mem,inst.operandSize,state.ESI(),state.EDI());
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSHF://            0x9C,
		clocksPassed=4; // If running as 386 and in protected mode, 3 clocks.
		if(true==fidelity.IOPLExceptionInVM86Mode(*this,EXCEPTION_GP,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			Push(mem,inst.operandSize,state.EFLAGS);
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;


	case I486_RENUMBER_PUSH_EAX://         0x50,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=1;
			Push(mem,inst.operandSize,GetEAX());
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_ECX://         0x51,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=1;
			Push(mem,inst.operandSize,GetECX());
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_EDX://         0x52,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=1;
			Push(mem,inst.operandSize,GetEDX());
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_EBX://         0x53,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=1;
			Push(mem,inst.operandSize,GetEBX());
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_ESP://         0x54,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=1;
			Push(mem,inst.operandSize,GetESP());
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_EBP://         0x55,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=1;
			Push(mem,inst.operandSize,GetEBP());
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_ESI://         0x56,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=1;
			Push(mem,inst.operandSize,GetESI());
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_EDI://         0x57,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=1;
			Push(mem,inst.operandSize,GetEDI());
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_I8://          0x6A,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=1;
			Push(mem,inst.operandSize,inst.EvalSimm8());
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_I://           0x68,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=1;
			Push(mem,inst.operandSize,inst.EvalSimm16or32(inst.operandSize));
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_CS://          0x0E,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			Push(mem,inst.operandSize,state.CS().value);
			clocksPassed=3;
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_SS://          0x16,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			Push(mem,inst.operandSize,state.SS().value);
			clocksPassed=3;
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_DS://          0x1E,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			Push(mem,inst.operandSize,state.DS().value);
			clocksPassed=3;
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_ES://          0x06,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			Push(mem,inst.operandSize,state.ES().value);
			clocksPassed=3;
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_FS://          0x0FA0,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			Push(mem,inst.operandSize,state.FS().value);
			clocksPassed=3;
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_PUSH_GS://          0x0FA8,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			Push(mem,inst.operandSize,state.GS().value);
			clocksPassed=3;
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;


	case I486_RENUMBER_POP_M://            0x8F,
		clocksPassed=6;
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			OperandValue value;
			value.MakeByteWordOrDword(inst.operandSize,Pop(mem,inst.operandSize));
			HANDLE_EXCEPTION_PUSH_POP;
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;


	case I486_RENUMBER_POP_EAX://          0x58,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=4;
			if(16==inst.operandSize)
			{
				auto pop=Pop16(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				SetAX(pop);
			}
			else
			{
				auto pop=Pop32(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				state.EAX()=pop;
			}
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_POP_ECX://          0x59,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=4;
			if(16==inst.operandSize)
			{
				auto pop=Pop16(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				SetCX(pop);
			}
			else
			{
				auto pop=Pop32(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				state.ECX()=pop;
			}
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_POP_EDX://          0x5A,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=4;
			if(16==inst.operandSize)
			{
				auto pop=Pop16(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				SetDX(pop);
			}
			else
			{
				auto pop=Pop32(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				state.EDX()=pop;
			}
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_POP_EBX://          0x5B,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=4;
			if(16==inst.operandSize)
			{
				auto pop=Pop16(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				SetBX(pop);
			}
			else
			{
				auto pop=Pop32(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				state.EBX()=pop;
			}
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_POP_ESP://          0x5C,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=4;
			if(16==inst.operandSize)
			{
				auto pop=Pop16(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				SetSP(pop);
			}
			else
			{
				auto pop=Pop32(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				state.ESP()=pop;
			}
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_POP_EBP://          0x5D,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=4;
			if(16==inst.operandSize)
			{
				auto pop=Pop16(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				SetBP(pop);
			}
			else
			{
				auto pop=Pop32(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				state.EBP()=pop;
			}
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_POP_ESI://          0x5E,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=4;
			if(16==inst.operandSize)
			{
				auto pop=Pop16(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				SetSI(pop);
			}
			else
			{
				auto pop=Pop32(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				state.ESI()=pop;
			}
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_POP_EDI://          0x5F,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=4;
			if(16==inst.operandSize)
			{
				auto pop=Pop16(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				SetDI(pop);
			}
			else
			{
				auto pop=Pop32(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				state.EDI()=pop;
			}
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;


	case I486_RENUMBER_POP_SS://           0x17,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=3;
			auto selector=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			LoadSegmentRegister(state.SS(),selector,mem);
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_POP_DS://           0x1F,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=3;
			auto selector=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			LoadSegmentRegister(state.DS(),selector,mem);
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_POP_ES://           0x07,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=3;
			auto selector=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			LoadSegmentRegister(state.ES(),selector,mem);
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_POP_FS://           0x0FA1,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=3;
			auto selector=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			LoadSegmentRegister(state.FS(),selector,mem);
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;
	case I486_RENUMBER_POP_GS://           0x0FA9,
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			clocksPassed=3;
			auto selector=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			LoadSegmentRegister(state.GS(),selector,mem);
			HANDLE_EXCEPTION_PUSH_POP;
		}
		break;

	case I486_RENUMBER_POPA://             0x61,
		clocksPassed=9;
		if(16==inst.operandSize)
		{
			SAVE_ESP_BEFORE_PUSH_POP;

			auto pop=Pop(mem,inst.operandSize)&0xffff;
			HANDLE_EXCEPTION_PUSH_POP;
			state.EDI()=((state.EDI()&0xffff0000)|pop);

			pop=Pop(mem,inst.operandSize)&0xffff;
			HANDLE_EXCEPTION_PUSH_POP;
			state.ESI()=((state.ESI()&0xffff0000)|pop);

			pop=Pop(mem,inst.operandSize)&0xffff;
			HANDLE_EXCEPTION_PUSH_POP;
			state.EBP()=((state.EBP()&0xffff0000)|pop);

			Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;

			pop=Pop(mem,inst.operandSize)&0xffff;
			HANDLE_EXCEPTION_PUSH_POP;
			state.EBX()=((state.EBX()&0xffff0000)|pop);

			pop=Pop(mem,inst.operandSize)&0xffff;
			HANDLE_EXCEPTION_PUSH_POP;
			state.EDX()=((state.EDX()&0xffff0000)|pop);

			pop=Pop(mem,inst.operandSize)&0xffff;
			HANDLE_EXCEPTION_PUSH_POP;
			state.ECX()=((state.ECX()&0xffff0000)|pop);

			pop=Pop(mem,inst.operandSize)&0xffff;
			HANDLE_EXCEPTION_PUSH_POP;
			state.EAX()=((state.EAX()&0xffff0000)|pop);
		}
		else
		{
			SAVE_ESP_BEFORE_PUSH_POP;

			auto pop=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			state.EDI()=pop;

			pop=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			state.ESI()=pop;

			pop=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			state.EBP()=pop;

			Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;

			pop=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			state.EBX()=pop;

			pop=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			state.EDX()=pop;

			pop=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			state.ECX()=pop;

			pop=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			state.EAX()=pop;
		}
		break;

	case I486_RENUMBER_POPF://             0x9D,
		clocksPassed=(IsInRealMode() ? 9 : 6);
		if(true==fidelity.IOPLExceptionInVM86Mode(*this,EXCEPTION_GP,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		{
			typename FIDELITY::EFLAGS ioplBits;
			FIDELITY::SaveEFLAGS(ioplBits,*this);

			// VM and RF flags must be preserved.
			SAVE_ESP_BEFORE_PUSH_POP;
			uint32_t incoming=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;

			const uint32_t mutableBits=0b1001000111111111010101; // Bits that POPF can change.

			incoming&=mutableBits;

			auto EFLAGS=state.EFLAGS;
			EFLAGS&=~mutableBits;
			EFLAGS|=incoming;

			SetFLAGSorEFLAGS(inst.operandSize,EFLAGS);

			FIDELITY::RestoreIOPLBits(*this,ioplBits);
			FIDELITY::RestoreIF(*this,ioplBits);
		}
		break;


	case I486_RENUMBER_RET://              0xC3,
		clocksPassed=5;
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			if(16==inst.operandSize)
			{
				auto IP=Pop16(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				SetIP(IP);
			}
			else
			{
				auto EIP=Pop32(mem);
				HANDLE_EXCEPTION_PUSH_POP;
				SetEIP(EIP);
			}
			EIPIncrement=0;
			if(enableCallStack)
			{
				PopCallStack(state.CS().value,state.EIP);
			}
		}
		break;
	case I486_RENUMBER_IRET://   0xCF,
		{
			if(true==IsInRealMode())
			{
				clocksPassed=15;

				uint32_t eip,cs,eflags;
				Pop(eip,cs,eflags,mem,inst.operandSize);

				SetIPorEIP(inst.operandSize,eip);
				auto segRegValue=cs;

				typename FIDELITY::EFLAGS ioplBits;
				FIDELITY::SaveEFLAGS(ioplBits,*this);
				SetFLAGSorEFLAGS(inst.operandSize,eflags);
				FIDELITY::RestoreIOPLBits(*this,ioplBits);
				FIDELITY::RestoreIF(*this,ioplBits);

				// If I print state.EFLAGS here, it is updated correctly.
				// If I do not print state.EFLAGS, it is not updated from time to time.
				// Looks like I stepped on a bug of clang.  I need to be careful to declare it as a clang bug,
				// but it happens only in Linux using clang 6.0.0-1ubuntu2.
				// It does not seem to happen with my clang on macOS.
				// I need to let state.EFLAGS consumed by outside of this function, outside of this object file,
				// to make it run correctly.  Otherwise, the compiler seems to ignore above SetFLAGSorEFLAGS.
			#ifdef __linux__
				ConsumeVariable(state.EFLAGS);
			#endif

				state.EFLAGS&=EFLAGS_MASK;
				state.EFLAGS|=EFLAGS_ALWAYS_ON;

				LoadSegmentRegisterRealMode(state.CS(),segRegValue);
				EIPIncrement=0;
				if(true==enableCallStack)
				{
					PopCallStack(state.CS().value,state.EIP);
				}
			}
			else
			{
				clocksPassed=36;
				if(true==fidelity.IOPLExceptionInVM86Mode(*this,EXCEPTION_GP,mem,inst.numBytes))
				{
					EIPIncrement=0;
					break;
				}

				// I still do not understand the logic of task return.
				// But, none of TownsOS, Windows 3.1, Windows 95, and Linux for TOWNS seems to be using it anyway.
				// if(true==fidelity.IsTaskReturn(*this))
				// {
				// 	Abort("Task Return Not Supported");
				// 	break;
				// }

				auto prevVMFlag=state.EFLAGS&EFLAGS_VIRTUAL86;
				bool IRET_TO_VM86=false;

				uint32_t eip,cs,eflags;
				Pop(eip,cs,eflags,mem,inst.operandSize);

				SetIPorEIP(inst.operandSize,eip);
				auto segRegValue=cs;

				typename FIDELITY::EFLAGS ioplBits;
				FIDELITY::SaveEFLAGS(ioplBits,*this);
				SetFLAGSorEFLAGS(inst.operandSize,eflags);

				// The pseudo code in i486 Programmer's Reference Manual suggests that IRET in VM86 mode will cause #GP(0).
				// The textual explanation in i486 Programmer's Reference Manual tells IRET will cause #GP(0) if IOPL<3.
				// i486 Programmer's Reference Manual also says IRETD can be used to enter VM86 mode.  However,
				// Figure 23-2 (pp. 23-5) clearly indicates that IRETD cannot be used to exit VM86 mode.
				// G*d D**n it!  What should I believe?
				// For the time being, I make sure IRETD won't exit VM86 mode.
				state.EFLAGS|=prevVMFlag;  // Can stay in the VM86 mode, but not allowed to get out of the VM86 mode by IRET.
				state.EFLAGS&=EFLAGS_MASK;
				state.EFLAGS|=EFLAGS_ALWAYS_ON;

				state.mode=state.RecalculateMode(); // Next VM-flag must be set before recalculating the running mode.

			#ifdef __linux__
				ConsumeVariable(state.EFLAGS);
			#endif
				FIDELITY::RestoreIOPLBits(*this,ioplBits);
				FIDELITY::RestoreIF(*this,ioplBits);

				// if(state.EFLAGS&EFLAGS_NESTED)
				//{
				//	TaskReturn
				//	Not supported, but in this case, restoration of prevVMFlag above may not be correct.
				//}
				// else
				if(0==prevVMFlag && 0!=(state.EFLAGS&EFLAGS_VIRTUAL86)) // Stack-Return-To-V86
				{
					uint32_t TempESP,TempSS,ES,DS,FS,GS;

					Pop(TempESP,TempSS,ES,mem,inst.operandSize);
					Pop(DS,FS,GS,mem,inst.operandSize);

					LoadSegmentRegister(state.ES(),ES,mem);
					LoadSegmentRegister(state.DS(),DS,mem);
					LoadSegmentRegister(state.FS(),FS,mem);
					LoadSegmentRegister(state.GS(),GS,mem);
					if(16==inst.operandSize)
					{
						SET_INT_LOW_BYTE(state.ESP(),TempESP&0xFFFF);
					}
					else
					{
						state.ESP()=TempESP;
					}
					LoadSegmentRegister(state.SS(),TempSS,mem);
					IRET_TO_VM86=true;
				}

				// IRET to Virtual86 mode requires EFLAGS be loaded before the segment register.
				auto CPL=state.CS().DPL;
				LoadSegmentRegister(state.CS(),segRegValue,mem);
				EIPIncrement=0;
				if(true==enableCallStack)
				{
					PopCallStack(state.CS().value,state.EIP);
				}

				if(state.CS().DPL>CPL && 0==(state.EFLAGS&EFLAGS_VIRTUAL86))
				{
					// IRET to outer level
					uint32_t TempESP,TempSS;
					Pop(TempESP,TempSS,mem,inst.operandSize);
					LoadSegmentRegister(state.SS(),TempSS,mem);
					state.ESP()=TempESP;

					auto nextCPL=state.CS().value&3;

					unsigned int regs[]=
					{
						REG_ES,REG_FS,REG_GS,REG_DS
					};
					for(auto reg : regs)
					{
						auto &s=state.sreg[reg-REG_SEGMENT_REG_BASE];
						// (1) Selector within desc table limits
						// (2) AR byte must be data or readable CS
						// (3) If data or non-conforming code (What's non-conforming?), DPL>=CPL or DPL>=RPL.
						bool valid=true;
						auto RPL=(s.value&3);
						// Currently only checking (3)
						if(s.DPL<nextCPL) // The DPL>=RPL condition is questionable.
						{
							valid=false;
						}
						//if(condition(1) || condition(2))
						//{
						//	valid=false;
						//}

						if(true!=valid)
						{
							s.value=0;
							s.limit=0;
						}
					}
				}
			}

			if(16==state.CS().addressSize)
			{
				state.EIP&=0xFFFF;
			}
		}
		break;
	case I486_RENUMBER_RETF://             0xCB,
		{
			if(true==IsInRealMode())
			{
				clocksPassed=13;
			}
			else
			{
				clocksPassed=18;
			}
			auto prevDPL=state.CS().DPL;

			uint32_t eip,cs;
			SAVE_ESP_BEFORE_PUSH_POP;
			Pop(eip,cs,mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;

			LoadSegmentRegister(state.CS(),cs,mem);
			HANDLE_EXCEPTION_PUSH_POP;

			SetIPorEIP(inst.operandSize,eip);
			EIPIncrement=0;
			if(enableCallStack)
			{
				PopCallStack(state.CS().value,state.EIP);
			}

			fidelity.CheckRETFtoOuterLevel(*this,mem,inst.operandSize,prevDPL,0);

			if(16==state.CS().addressSize)
			{
				state.EIP&=0xFFFF;
			}
		}
		break;
	case I486_RENUMBER_RET_I16://          0xC2,
		clocksPassed=5;
		{
			SAVE_ESP_BEFORE_PUSH_POP;
			auto EIP=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;
			SetIPorEIP(inst.operandSize,EIP);
		}
		state.ESP()+=inst.EvalUimm16(); // Do I need to take &0xffff if address mode is 16? 
		EIPIncrement=0;
		if(enableCallStack)
		{
			PopCallStack(state.CS().value,state.EIP);
		}
		break;
	case I486_RENUMBER_RETF_I16://         0xCA,
		{
			if(true==IsInRealMode())
			{
				clocksPassed=14;
			}
			else
			{
				clocksPassed=17;
			}
			auto prevDPL=state.CS().DPL;

			SAVE_ESP_BEFORE_PUSH_POP;
			auto EIP=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;

			auto cs=Pop(mem,inst.operandSize);
			HANDLE_EXCEPTION_PUSH_POP;

			LoadSegmentRegister(state.CS(),cs,mem);
			HANDLE_EXCEPTION_PUSH_POP;

			SetIPorEIP(inst.operandSize,EIP);
			EIPIncrement=0;
			if(enableCallStack)
			{
				PopCallStack(state.CS().value,state.EIP);
			}
			if(true!=fidelity.CheckRETFtoOuterLevel(*this,mem,inst.operandSize,prevDPL,inst.EvalUimm16()))
			{
				// true means IMM16 is already consumed in CheckRETFtoOuterLevel.
				state.ESP()+=inst.EvalUimm16(); // Do I need to take &0xffff if address mode is 16? 
			}
			else
			{
				std::cout << "RETF to lower level with 0<IMM16." << std::endl;
			}

			if(16==state.CS().addressSize)
			{
				state.EIP&=0xFFFF;
			}
		}
		break;


	case I486_RENUMBER_SAHF://=             0x9E,
		{
			state.EFLAGS&=(~0b11010101); // b7, b6, b4, b2, b0 only.
			state.EFLAGS|=(GetAH()&0b11010101);
		}
		clocksPassed=2;
		break;


	case I486_RENUMBER_SHLD_RM_I8://       0x0FA4,
	case I486_RENUMBER_SHLD_RM_CL://       0x0FA5,
	case I486_RENUMBER_SHRD_RM_I8://       0x0FAC,
	case I486_RENUMBER_SHRD_RM_CL://       0x0FAD,
		{
			unsigned int count;
			if(I486_OPCODE_SHLD_RM_CL==inst.opCode ||
			   I486_OPCODE_SHRD_RM_CL==inst.opCode)
			{
				clocksPassed=3;
				count=GetCL()&0x1F;
			}
			else
			{
				clocksPassed=2;
				count=inst.EvalUimm8()&0x1F;
			}
			if(OPER_ADDR==op1.operandType)
			{
				++clocksPassed;
			}
			if(0!=count)
			{
				switch(opCodeRenumberTable[inst.opCode])
				{
				case I486_RENUMBER_SHLD_RM_I8://       0x0FA4,
				case I486_RENUMBER_SHLD_RM_CL://       0x0FA5,
					if(16==inst.operandSize)
					{
						auto operPtr=GetOperandPointer16(mem,inst.addressSize,inst.segOverride,op1,true);
						uint16_t v1;
						if(nullptr!=operPtr)
						{
							v1=cpputil::GetWord(operPtr);
						}
						else
						{
							v1=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op1);
						}
						uint16_t v2=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op2);
						uint32_t concat=cpputil::WordPairToUnsigned32(v2,v1);
						concat>>=(16-count);
						if(nullptr!=operPtr)
						{
							cpputil::PutWord(operPtr,cpputil::LowWord(concat));
						}
						else
						{
							StoreOperandValueRegOrMem16(op1,mem,inst.addressSize,inst.segOverride,cpputil::LowWord(concat));
						}
						SetCF(0!=(concat&0x10000));
						SetOF((concat&0x8000)!=(v1&0x8000));
						SetZF(0==(concat&0xFFFF));
						SetSF(0!=(concat&0x8000));
						SetPF(CheckParity(concat));
					}
					else
					{
						auto operPtr=GetOperandPointer32(mem,inst.addressSize,inst.segOverride,op1,true);
						uint32_t v1;
						if(nullptr!=operPtr)
						{
							v1=cpputil::GetDword(operPtr);
						}
						else
						{
							v1=EvaluateOperandRegOrMem32(mem,inst.addressSize,inst.segOverride,op1);
						}
						uint32_t v2=EvaluateOperandRegOrMem32(mem,inst.addressSize,inst.segOverride,op2);
						uint64_t concat=cpputil::DwordPairToUnsigned64(v2,v1);
						concat>>=(32-count);
						if(nullptr!=operPtr)
						{
							cpputil::PutDword(operPtr,cpputil::LowDword(concat));
						}
						else
						{
							StoreOperandValueRegOrMem32(op1,mem,inst.addressSize,inst.segOverride,cpputil::LowDword(concat));
						}
						SetCF(0!=(concat&0x100000000LL));
						SetOF((concat&0x80000000)!=(v1&0x80000000));
						SetZF(0==(concat&0xFFFFFFFF));
						SetSF(0!=(concat&0x80000000));
						SetPF(CheckParity(concat));
					}
					break;
				case I486_RENUMBER_SHRD_RM_I8://       0x0FAC,
				case I486_RENUMBER_SHRD_RM_CL://       0x0FAD,
					if(16==inst.operandSize)
					{
						auto operPtr=GetOperandPointer16(mem,inst.addressSize,inst.segOverride,op1,true);
						uint16_t v1;
						if(nullptr!=operPtr)
						{
							v1=cpputil::GetWord(operPtr);
						}
						else
						{
							v1=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op1);
						}
						uint16_t v2=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op2);
						uint32_t concat=cpputil::WordPairToUnsigned32(v1,v2);
						SetCF(0!=count && 0!=(concat&(1LL<<(count-1))));
						concat>>=count;
						if(nullptr!=operPtr)
						{
							cpputil::PutWord(operPtr,cpputil::LowWord(concat));
						}
						else
						{
							StoreOperandValueRegOrMem16(op1,mem,inst.addressSize,inst.segOverride,cpputil::LowWord(concat));
						}
						SetOF((concat&0x8000)!=(v1&0x8000));
						SetZF(0==(concat&0xFFFF));
						SetSF(0!=(concat&0x8000));
						SetPF(CheckParity(concat));
					}
					else
					{
						auto operPtr=GetOperandPointer32(mem,inst.addressSize,inst.segOverride,op1,true);
						uint32_t v1;
						if(nullptr!=operPtr)
						{
							v1=cpputil::GetDword(operPtr);
						}
						else
						{
							v1=EvaluateOperandRegOrMem32(mem,inst.addressSize,inst.segOverride,op1);
						}
						auto v2=EvaluateOperandRegOrMem32(mem,inst.addressSize,inst.segOverride,op2);
						uint64_t concat=cpputil::DwordPairToUnsigned64(v1,v2);;
						SetCF(0!=count && 0!=(concat&(1LL<<(count-1))));
						concat>>=count;
						if(nullptr!=operPtr)
						{
							cpputil::PutDword(operPtr,cpputil::LowDword(concat));
						}
						else
						{
							StoreOperandValueRegOrMem32(op1,mem,inst.addressSize,inst.segOverride,cpputil::LowDword(concat));
						}
						SetOF((concat&0x80000000)!=(v1&0x80000000));
						SetZF(0==(concat&0xFFFFFFFF));
						SetSF(0!=(concat&0x80000000));
						SetPF(CheckParity(concat));
					}
					break;
				}
			}
		}
		break;


	case I486_RENUMBER_SCASB://            0xAE,
		{
			SAVE_ECX_BEFORE_STRINGOP;

			#define SCASB_Template(addrSize) \
				for(int ctr=0; \
				    ctr<MAX_REP_BUNDLE_COUNT && \
				    true==REPCheckA##addrSize(clocksPassed,inst.instPrefix); \
				    ++ctr) \
				{ \
					clocksPassed+=2; \
					auto data=FetchByte(addrSize,state.ES(),state.EDI(),mem); \
					HANDLE_EXCEPTION_STRINGOP; \
					auto AL=GetAL(); \
					SubByte(AL,data); \
					UpdateDIorEDIAfterStringOpO8A##addrSize(); \
					if(true==REPEorNECheck(inst.instPrefix)) \
					{ \
						EIPIncrement=0; \
					} \
					else \
					{ \
						EIPIncrement=inst.numBytes; \
						break; \
					} \
					UPDATED_SAVED_ECX_AFTER_STRINGOP; \
				}

			if(16==inst.addressSize)
			{
				SCASB_Template(16);
			}
			else
			{
				SCASB_Template(32);
			}
		}
		break;
	case I486_RENUMBER_SCAS://             0xAF,
		{
			SAVE_ECX_BEFORE_STRINGOP;

			#define SCAS_Template(addrSize,WordOrDword,UpdateFunc) \
				for(int ctr=0;\
				    ctr<MAX_REP_BUNDLE_COUNT &&\
				    true==REPCheckA##addrSize(clocksPassed,inst.instPrefix);\
				    ++ctr)\
				{\
					auto data=Fetch##WordOrDword(addrSize,state.ES(),state.EDI(),mem);\
					HANDLE_EXCEPTION_STRINGOP;\
					auto EAX=GetEAX();\
					Sub##WordOrDword(EAX,data);\
					UpdateFunc();\
					clocksPassed+=2;\
					if(true==REPEorNECheck(inst.instPrefix))\
					{\
						EIPIncrement=0;\
					}\
					else\
					{\
						EIPIncrement=inst.numBytes;\
						break;\
					}\
					UPDATED_SAVED_ECX_AFTER_STRINGOP;\
				}

			if(16==inst.operandSize)
			{
				if(16==inst.addressSize)
				{
					SCAS_Template(16,Word,UpdateDIorEDIAfterStringOpO16A16)
				}
				else
				{
					SCAS_Template(32,Word,UpdateDIorEDIAfterStringOpO16A32)
				}
			}
			else // 32-bit operand
			{
				if(16==inst.addressSize)
				{
					SCAS_Template(16,Dword,UpdateDIorEDIAfterStringOpO32A16)
				}
				else
				{
					SCAS_Template(32,Dword,UpdateDIorEDIAfterStringOpO32A32)
				}
			}
		}
		break;


	case I486_RENUMBER_SETA://             0x0F97,
		SETxx(CondJA());
		break;
	case I486_RENUMBER_SETAE://            0x0F93,
		SETxx(CondJAE());
		break;
	case I486_RENUMBER_SETB://             0x0F92,
		SETxx(CondJB());
		break;
	case I486_RENUMBER_SETBE://            0x0F96,
		SETxx(CondJBE());
		break;
	// I486_OPCODE_SETC://             0x0F92,
	case I486_RENUMBER_SETE://             0x0F94,
		SETxx(CondJE());
		break;
	case I486_RENUMBER_SETG://             0x0F9F,
		SETxx(CondJG());
		break;
	case I486_RENUMBER_SETGE://            0x0F9D,
		SETxx(CondJGE());
		break;
	case I486_RENUMBER_SETL://             0x0F9C,
		SETxx(CondJL());
		break;
	case I486_RENUMBER_SETLE://            0x0F9E,
		SETxx(CondJLE());
		break;
	//I486_OPCODE_SETNA://            0x0F96,
	//I486_OPCODE_SETNAE://           0x0F92,
	//I486_OPCODE_SETNB://            0x0F93,
	//I486_OPCODE_SETNBE://           0x0F97,
	//I486_OPCODE_SETNC://            0x0F93,
	case I486_RENUMBER_SETNE://            0x0F95,
		SETxx(CondJNE());
		break;
	//I486_OPCODE_SETNG://            0x0F9E,
	//I486_OPCODE_SETNGE://           0x0F9C,
	//I486_OPCODE_SETNL://            0x0F9D,
	//I486_OPCODE_SETNLE://           0x0F9F,
	case I486_RENUMBER_SETNO://            0x0F91,
		SETxx(CondJNO());
		break;
	case I486_RENUMBER_SETNP://            0x0F9B,
		SETxx(CondJNP());
		break;
	case I486_RENUMBER_SETNS://            0x0F99,
		SETxx(CondJNS());
		break;
	// case I486_RENUMBER_SETNZ://            0x0F95,
	case I486_RENUMBER_SETO://             0x0F90,
		SETxx(CondJO());
		break;
	case I486_RENUMBER_SETP://             0x0F9A,
		SETxx(CondJP());
		break;
	//I486_OPCODE_SETPE://            0x0F9A,
	//I486_OPCODE_SETPO://            0x0F9B,
	case I486_RENUMBER_SETS://             0x0F98,
		SETxx(CondJS());
		break;
	// I486_OPCODE_SETZ://             0x0F94,

	case I486_RENUMBER_SLDT_STR_LLDT_LTR_VERR_VERW://             0x0F00,
		if(true==fidelity.SLDT_STR_LLDT_LTR_VERR_VERW_Cause_INT6_InRealModeVM86Mode(*this))
		{
			RaiseException(i486DXCommon::EXCEPTION_UD,0);
			HandleException(false,mem,inst.numBytes);
			EIPIncrement=0;
			break;
		}
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
				value.MakeWord(state.TR.value);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			break;
		case 2: // "LLDT"
			{
				clocksPassed=11;
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				if(true!=state.exception)
				{
					InvalidateDescriptorCache();
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
						HandleException(false,mem,inst.numBytes);
						EIPIncrement=0;
					}
					else
					{
						SegmentRegister seg;
						LoadSegmentRegister(seg,selector,mem,MODE_NATIVE); // Force to read from GDT by setting mode=MODE_NATIVE
						const unsigned char byteData[]=
						{
							(unsigned char)( seg.limit    &0xff),
							(unsigned char)((seg.limit>>8)&0xff),
							(unsigned char)( seg.baseLinearAddr     &0xff),
							(unsigned char)((seg.baseLinearAddr>>8) &0xff),
							(unsigned char)((seg.baseLinearAddr>>16)&0xff),
							(unsigned char)((seg.baseLinearAddr>>24)&0xff),
						};
						LoadDescriptorTableRegister(state.LDTR,32,byteData);
					}
				}
			}
			break;
		case 3: // LTR
			{
				// I need to correct implementation of task behavior to support EMM386.EXE
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				LoadTaskRegister(value.GetAsDword(),mem);
				clocksPassed=20;
			}
			break;
		case 4: // "VERR"
			{
				clocksPassed=11;
				if(true==IsInRealMode())
				{
					Interrupt(6,mem,0,0,false);
					EIPIncrement=0;
				}
				else if(0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
				{
					RaiseException(EXCEPTION_UD,0);
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
				}
				else
				{
					auto selector=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op1);
					if(true==state.exception)
					{
						HandleException(true,mem,inst.numBytes);
						EIPIncrement=0;
						break;
					}

					// Intel 80386 Programmer's Reference Manual pp.406.
					// The pseudo code implies ZF=0 if accessible.
					// However, textual explanation tells ZF=1 if accessible.
					// Looks like textual explanation is correct.

					if(0==selector) // Null selector
					{
						SetZF(false);
						break;
					}

					// Assume state.exception==false here.
					// Also protected mode, not VM86
					SegmentRegister seg;
					auto fourBytes=LoadSegmentRegister(seg,selector,mem,MODE_NATIVE);
					if(true==state.exception) // Could not be loaded.
					{
						state.exception=false;
						SetZF(false); // Inaccessible=not readable
						break;
					}

					// This type-check is not performance critical.
					// However, state-file saved in the earlier version Tsugarudid not update
					// segment type bytes, and therefore it may not work.
					SetZF(fidelity.VERRTypeCheck(*this,seg));
				}
			}
			break;
		case 5: // "VERW"
			{
				clocksPassed=11;
				if(true==IsInRealMode())
				{
					Interrupt(6,mem,0,0,false);
					EIPIncrement=0;
				}
				else if(0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
				{
					RaiseException(EXCEPTION_UD,0);
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
				}
				else
				{
					auto selector=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op1);
					if(true==state.exception)
					{
						HandleException(true,mem,inst.numBytes);
						EIPIncrement=0;
						break;
					}

					if(0==selector) // Null selector
					{
						SetZF(false);
						break;
					}

					SegmentRegister seg;
					auto fourBytes=LoadSegmentRegister(seg,selector,mem,MODE_NATIVE);
					if(true==state.exception) // Could not be loaded.
					{
						state.exception=false;
						SetZF(false); // Inaccessible=not readable
						break;
					}

					// This type-check is not performance critical.
					// However, state-file saved in the earlier version Tsugarudid not update
					// segment type bytes, and therefore it may not work.
					SetZF(fidelity.VERWTypeCheck(*this,seg));
				}
			}
			break;
		}
		break;


	case I486_RENUMBER_STC://              0xFB,
		SetCF(true);
		clocksPassed=2;
		break;
	case I486_RENUMBER_STD://              0xFD,
		SetDF(true);
		clocksPassed=2;
		break;
	case I486_RENUMBER_STI://              0xFB,
		clocksPassed=5;
		if(true==fidelity.IOPLException(*this,EXCEPTION_GP,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		SetIF(true);
		// i486 Programmer's Reference Manual says:
		// The processor then responds to the external interrupts after executing the next instruction
		// if the next instruction allows the IF flag to remain enabled.
		// Therefore, STI behaves like substituting a value to SS register, which holds an IRQ for the
		// next instruction.
		state.holdIRQ=true;
		break;


	case I486_RENUMBER_STOSB://            0xAA,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		{
			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);

			#define STOSB_Template(addrSize)\
				for(int ctr=0; \
				    ctr<MAX_REP_BUNDLE_COUNT && \
				    true==REPCheckA##addrSize(clocksPassed,prefix);\
				    ++ctr)\
				{\
					StoreByte(mem,addrSize,state.ES(),state.EDI(),GetAL());\
					clocksPassed+=1;\
					if(true!=state.exception)\
					{\
						UpdateDIorEDIAfterStringOpO8A##addrSize();\
						if(INST_PREFIX_REP==prefix)\
						{\
							EIPIncrement=0;\
						}\
						else\
						{\
							EIPIncrement=inst.numBytes;\
							break;\
						}\
					}\
					else\
					{\
						SetECX(ECX);\
						HandleException(false,mem,inst.numBytes);\
						EIPIncrement=0;\
						break;\
					}\
					ECX=state.ECX();\
				}

			if(16==inst.addressSize)
			{
				STOSB_Template(16);
			}
			else
			{
				STOSB_Template(32);
			}
		}
		break;
	case I486_RENUMBER_STOS://             0xAB,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		{
			#define STOS_Template(addrSize,StoreFunc,UpdateFunc) \
				for(int ctr=0; \
				    ctr<MAX_REP_BUNDLE_COUNT && \
				    true==REPCheckA##addrSize(clocksPassed,prefix); \
				    ++ctr) \
				{ \
					(StoreFunc)(mem,(addrSize),state.ES(),state.EDI(),GetEAX()); \
					clocksPassed+=1; \
					if(true!=state.exception) \
					{ \
						(UpdateFunc)(); \
						if(INST_PREFIX_REP==prefix) \
						{ \
							EIPIncrement=0; \
						} \
						else \
						{ \
							EIPIncrement=inst.numBytes; \
							break; \
						} \
					} \
					else \
					{ \
						SetECX(ECX); \
						HandleException(false,mem,inst.numBytes); \
						EIPIncrement=0; \
						break; \
					} \
					ECX=state.ECX(); \
				}


			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);
			if(16==inst.operandSize)
			{
				if(16==inst.addressSize)
				{
					STOS_Template(16,StoreWord,UpdateDIorEDIAfterStringOpO16A16);
				}
				else
				{
					STOS_Template(32,StoreWord,UpdateDIorEDIAfterStringOpO16A32);
				}
			}
			else // 32-bit OperandSize
			{
				if(16==inst.addressSize)
				{
					STOS_Template(16,StoreDword,UpdateDIorEDIAfterStringOpO32A16);
				}
				else
				{
					STOS_Template(32,StoreDword,UpdateDIorEDIAfterStringOpO32A32);
				}
			}
		}
		break;


	case I486_RENUMBER_XCHG_EAX_ECX://     0x91,
	case I486_RENUMBER_XCHG_EAX_EDX://     0x92,
	case I486_RENUMBER_XCHG_EAX_EBX://     0x93,
	case I486_RENUMBER_XCHG_EAX_ESP://     0x94,
	case I486_RENUMBER_XCHG_EAX_EBP://     0x95,
	case I486_RENUMBER_XCHG_EAX_ESI://     0x96,
	case I486_RENUMBER_XCHG_EAX_EDI://     0x97,
		clocksPassed=3;
		if(16==inst.operandSize)
		{
			auto op1=GetAX();
			auto op2=state.reg32()[inst.opCode&7];
			SetAX(op2);
			SET_INT_LOW_WORD(state.reg32()[inst.opCode&7],op1);
		}
		else
		{
			auto op1=GetEAX();
			auto op2=state.reg32()[inst.opCode&7];
			SetEAX(op2);
			state.reg32()[inst.opCode&7]=op1;
		}
		break;
	case I486_RENUMBER_XCHG_RM8_R8://           0x86,
		clocksPassed=(OPER_ADDR==op1.operandType ? 5 : 3);
		{
			auto operPtr=GetOperandPointer8(mem,inst.addressSize,inst.segOverride,op1,true);
			HANDLE_EXCEPTION_IF_ANY;
			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
			if(nullptr!=operPtr)
			{
			#ifdef YS_LITTLE_ENDIAN
				std::swap(operPtr[0],*state.reg8Ptr[regNum]);
			#else
				uint8_t R=(state.reg32()[regNum&3]>>reg8Shift[regNum]);
				uint32_t RM=operPtr[0];

				operPtr[0]=R;

				state.reg32()[regNum&3]&=reg8AndPattern[regNum];
				state.reg32()[regNum&3]|=(RM<<reg8Shift[regNum]);
			#endif
			}
			else
			{
				// op2 is a register.
				uint32_t RM=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1);

				HANDLE_EXCEPTION_IF_ANY;

				uint8_t R=cpputil::LowByte(state.reg32()[regNum&3]>>reg8Shift[regNum]);

				StoreOperandValueRegOrMem8(op1,mem,inst.addressSize,inst.segOverride,R);

				HANDLE_EXCEPTION_IF_ANY;

				state.reg32()[regNum&3]&=reg8AndPattern[regNum];
				state.reg32()[regNum&3]|=(RM<<reg8Shift[regNum]);
			}
		}
		break;
	case I486_RENUMBER_XCHG_RM_R://             0x87,
		clocksPassed=(OPER_ADDR==op1.operandType ? 5 : 3);
		{
			auto operPtr=GetOperandPointer16or32(mem,inst.addressSize,inst.segOverride,op1,true);
			HANDLE_EXCEPTION_IF_ANY;
			if(nullptr!=operPtr)
			{
				if(16==inst.operandSize)
				{
					uint16_t RM=cpputil::GetWord(operPtr);
					uint16_t R=cpputil::LowWord(state.reg32()[inst.GetREG()]);

					SET_INT_LOW_WORD(state.reg32()[inst.GetREG()],RM);

					cpputil::PutWord(operPtr,R);
				}
				else
				{
					uint32_t RM=cpputil::GetDword(operPtr);
					uint32_t R=state.reg32()[inst.GetREG()];

					state.reg32()[inst.GetREG()]=RM;

					cpputil::PutDword(operPtr,R);
				}
			}
			else
			{
				if(16==inst.operandSize)
				{
					auto RM=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,16/8);
					auto R=state.reg32()[inst.GetREG()]&0xFFFF;
					auto newR=RM.GetAsDword();

					HANDLE_EXCEPTION_IF_ANY;

					RM.SetDword(R);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,RM);

					HANDLE_EXCEPTION_IF_ANY;

					SET_INT_LOW_WORD(state.reg32()[inst.GetREG()],newR&0xFFFF);
				}
				else
				{
					auto RM=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,32/8);
					auto R=state.reg32()[inst.GetREG()];
					auto newR=RM.GetAsDword();

					HANDLE_EXCEPTION_IF_ANY;

					RM.SetDword(R);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,RM);

					HANDLE_EXCEPTION_IF_ANY;

					state.reg32()[inst.GetREG()]=newR;
				}
			}
		}
		break;


	case I486_RENUMBER_XLAT://             0xD7,
		clocksPassed=4;
		{
 			SegmentRegister seg=SegmentOverrideDefaultDS(inst.segOverride);
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

			if(true==fidelity.HandleExceptionIfAny(*this,mem,inst.numBytes))
			{
				EIPIncrement=0;
				break;
			}
		}
		break;

	case I486_RENUMBER_BSWAP_EAX:
	case I486_RENUMBER_BSWAP_ECX:
	case I486_RENUMBER_BSWAP_EDX:
	case I486_RENUMBER_BSWAP_EBX:
	case I486_RENUMBER_BSWAP_ESP:
	case I486_RENUMBER_BSWAP_EBP:
	case I486_RENUMBER_BSWAP_ESI:
	case I486_RENUMBER_BSWAP_EDI:
		{
			auto regNum=inst.opCode&0x07;
			unsigned int &reg=state.NULL_and_reg32[REG_EAX+regNum];
			uint8_t *bytes=(uint8_t *)&reg;
			std::swap(bytes[0],bytes[3]);
			std::swap(bytes[1],bytes[2]);
			clocksPassed=1;
		}
		break;

	case I486_RENUMBER_CMPXCHG_RM8_R8:
		clocksPassed=6;
		{
			// op2 is a register.
			uint32_t RM=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1);

			HANDLE_EXCEPTION_IF_ANY;

			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
			uint8_t R=cpputil::LowByte(state.reg32()[regNum&3]>>reg8Shift[regNum]);

			uint32_t AL=GetAL();
			SubByte(AL,RM);
			if(true==GetZF())
			{
				StoreOperandValueRegOrMem8(op1,mem,inst.addressSize,inst.segOverride,R);
				HANDLE_EXCEPTION_IF_ANY;
			}
			else
			{
				SetAL(RM);
			}
		}
		break;
	case I486_RENUMBER_CMPXCHG_RM_R:
		clocksPassed=6;
		if(16==inst.operandSize)
		{
			// op2 is a register.
			uint32_t RM=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op1);

			HANDLE_EXCEPTION_IF_ANY;

			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
			uint32_t R=cpputil::LowWord(state.reg32()[regNum]);

			uint32_t AX=GetAX();
			SubWord(AX,RM);
			if(true==GetZF())
			{
				StoreOperandValueRegOrMem16(op1,mem,inst.addressSize,inst.segOverride,R);
				HANDLE_EXCEPTION_IF_ANY;
			}
			else
			{
				SetAX(RM);
			}
		}
		else
		{
			// op2 is a register.
			uint32_t RM=EvaluateOperandRegOrMem32(mem,inst.addressSize,inst.segOverride,op1);

			HANDLE_EXCEPTION_IF_ANY;

			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
			uint32_t R=state.reg32()[regNum];

			uint32_t EAX=GetEAX();
			SubDword(EAX,RM);
			if(true==GetZF())
			{
				StoreOperandValueRegOrMem32(op1,mem,inst.addressSize,inst.segOverride,R);
				HANDLE_EXCEPTION_IF_ANY;
			}
			else
			{
				SetEAX(RM);
			}
		}
		break;
	case I486_RENUMBER_XADD_RM8_R8:
		clocksPassed=3;
		{
			// Based on the description in http://asm.inightmare.org/opcodelst/index.php?op=XADD
			// op2 is a register.
			uint32_t dst=EvaluateOperandRegOrMem8(mem,inst.addressSize,inst.segOverride,op1);

			HANDLE_EXCEPTION_IF_ANY;

			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
			uint8_t src=cpputil::LowByte(state.reg32()[regNum&3]>>reg8Shift[regNum]);

			auto temporary=dst;
			AddByte(dst,src);
			src=temporary;

			SetRegisterValue8(regNum,src);
			StoreOperandValueRegOrMem8(op1,mem,inst.addressSize,inst.segOverride,dst);
			HANDLE_EXCEPTION_IF_ANY;
		}
		break;
	case I486_RENUMBER_XADD_RM_R:
		clocksPassed=3;
		if(16==inst.operandSize)
		{
			// op2 is a register.
			uint32_t dst=EvaluateOperandRegOrMem16(mem,inst.addressSize,inst.segOverride,op1);

			HANDLE_EXCEPTION_IF_ANY;

			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
			uint32_t src=cpputil::LowWord(state.reg32()[regNum]);

			auto temporary=dst;
			AddWord(dst,src);
			src=temporary;

			SET_INT_LOW_WORD(state.reg32()[regNum],src);
			StoreOperandValueRegOrMem16(op1,mem,inst.addressSize,inst.segOverride,dst);
			HANDLE_EXCEPTION_IF_ANY;
		}
		else
		{
			// op2 is a register.
			uint32_t dst=EvaluateOperandRegOrMem32(mem,inst.addressSize,inst.segOverride,op1);

			HANDLE_EXCEPTION_IF_ANY;

			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
			uint32_t src=state.reg32()[regNum];

			auto temporary=dst;
			AddDword(dst,src);
			src=temporary;

			state.reg32()[regNum]=src;
			StoreOperandValueRegOrMem32(op1,mem,inst.addressSize,inst.segOverride,dst);
			HANDLE_EXCEPTION_IF_ANY;
		}
		break;

	case I486_RENUMBER_CPUID:
		clocksPassed=9;
		switch(GetEAX())
		{
		case 0:
			state.NULL_and_reg32[REG_EAX]=0;
			state.NULL_and_reg32[REG_EBX]=0x67756F52; // "Roug"
			state.NULL_and_reg32[REG_EDX]=0x38796C68; // "hly8"
			state.NULL_and_reg32[REG_ECX]=0x36383430; // "0486"
			break;
		case 1:
			state.NULL_and_reg32[REG_EAX]=1024;
			state.NULL_and_reg32[REG_EBX]=0;
			if(true==state.fpuState.enabled)
			{
				state.NULL_and_reg32[REG_EDX]=1;
			}
			else
			{
				state.NULL_and_reg32[REG_EDX]=0;
			}
			state.NULL_and_reg32[REG_ECX]=0;
			clocksPassed=14;
			break;
		default:
			state.NULL_and_reg32[REG_EAX]=0;
			state.NULL_and_reg32[REG_EBX]=0;
			state.NULL_and_reg32[REG_EDX]=0;
			state.NULL_and_reg32[REG_ECX]=0;
			break;
		}
		break;

	default:
#if defined(_MSC_VER)
		__assume(0);
#elif defined(__clang__) || defined(__GNUC__)
		__builtin_unreachable();
#endif
	}


	// Tentative
	// if(state.mode!=state.RecalculateMode())
	// {
	// 	std::string msg="Running mode is not updated.  Opcode=";
	// 	msg+=cpputil::Ustox(inst.RealOpCode());
	// 	msg+="H";
	// 	Abort(msg);
	// 	EIPIncrement=0;
	// }
	// Tentative


	if(0==clocksPassed)
	{
		std::string msg="Clocks-Passed is not set.  Opcode=";
		msg+=cpputil::Ustox(inst.RealOpCode());
		msg+="H";
		Abort(msg);
		EIPIncrement=0;
	}
	state.EIP+=EIPIncrement;

	if(nullptr!=debuggerPtr)
	{
		if(true==state.exception)
		{
			std::string msg="Unhandled exception!  Opcode=";
			msg+=cpputil::Ustox(inst.RealOpCode());
			msg+="H  ";
			msg+=ExceptionTypeToStr(state.exceptionType);
			msg+="(0x";
			msg+=cpputil::Uitox(state.exceptionCode);
			msg+=")";
			Abort(msg);
		}
		debuggerPtr->AfterRunOneInstruction(clocksPassed,*this,mem,io,inst,op1,op2);
	}

	return clocksPassed;
}

/* } */
#endif
