#include <iostream>

#include "i486.h"
#include "i486inst.h"



bool i486DX::OpCodeNeedsOneMoreByte(unsigned int firstByte) const
{
	switch(firstByte)
	{
	case I486_OPCODE_Jcc_FIRSTBYTE:
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

int i486DX::FetchOperand16(Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const
{
	inst.operand[inst.operandLen++]=FetchByte(seg,offset++,mem);
	inst.operand[inst.operandLen++]=FetchByte(seg,offset++,mem);
	inst.numBytes+=2;
	return 2;
}
int i486DX::FetchOperand32(Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const
{
	inst.operand[inst.operandLen++]=FetchByte(seg,offset++,mem);
	inst.operand[inst.operandLen++]=FetchByte(seg,offset++,mem);
	inst.operand[inst.operandLen++]=FetchByte(seg,offset++,mem);
	inst.operand[inst.operandLen++]=FetchByte(seg,offset++,mem);
	inst.numBytes+=4;
	return 4;
}
void i486DX::FetchOperand(Instruction &inst,SegmentRegister seg,int offset,const Memory &mem) const
{
	switch(inst.opCode)
	{
	case I486_OPCODE_JMP_FAR:
		switch(inst.operandSize)
		{
		case 16:
			offset+=FetchOperand16(inst,seg,offset,mem);
			break;
		case 32:
			offset+=FetchOperand32(inst,seg,offset,mem);
			break;
		}
		FetchOperand16(inst,seg,offset,mem);
		break;

	default:
		// Undefined operand, or probably not implemented yet.
		break;
	}
}
