#include <iostream>

#include "cpputil.h"
#include "i486.h"


const char *const i486DX::Reg8[8]=
{
	"AL","CL","DL","BL","AH","CH","DH","BH"
};

const char *const i486DX::Reg16[8]=
{
	"AX","CX","DX","BX","SP","BP","SI","DI"
};

const char *const i486DX::Reg32[8]=
{
	"EAX","ECX","EDX","EBX","ESP","EBP","ESI","EDI"
};

const char *const i486DX::Sreg[8]=
{
	"ES","CS","SS","DS","FS","GS"
};

const char *const i486DX::RegToStr[REG_TOTAL_NUMBER_OF_REGISTERS]=
{
	"(none)",

	"AL",
	"CL",
	"DL",
	"BL",
	"AH",
	"CH",
	"DH",
	"BH",

	"AX",
	"CX",
	"DX",
	"BX",
	"SP",
	"BP",
	"SI",
	"DI",

	"EAX",
	"ECX",
	"EDX",
	"EBX",
	"ESP",
	"EBP",
	"ESI",
	"EDI",

	"EIP",
	"EFLAGS",

	"ES",
	"CS",
	"SS",
	"DS",
	"FS",
	"GS",
	"GDT",
	"LDT",
	"TR0",
	"TR1",
	"TR2",
	"TR3",
	"TR4",
	"TR5",
	"TR6",
	"TR7",
	"IDTR",
	"CR0",
	"CR1",
	"CR2",
	"CR3",
	"DR0",
	"DR1",
	"DR2",
	"DR3",
	"DR4",
	"DR5",
	"DR6",
	"DR7",
};



i486DX::i486DX()
{
	Reset();
}

void i486DX::Reset(void)
{
	// page 10-1 [1]
	state.EFLAGS=RESET_EFLAGS;

	state.EIP=RESET_EIP;
	state.CS.value=RESET_CS;
	state.CS.baseLinearAddr=0xFFFF0000;

	LoadSegmentRegisterRealMode(state.DS,RESET_DS);
	LoadSegmentRegisterRealMode(state.SS,RESET_SS);
	LoadSegmentRegisterRealMode(state.ES,RESET_ES);
	LoadSegmentRegisterRealMode(state.FS,RESET_FS);
	LoadSegmentRegisterRealMode(state.GS,RESET_GS);

	state.IDTR.selector=0;
	state.IDTR.linearBaseAddr=RESET_IDTRBASE;
	state.IDTR.limit=RESET_IDTRLIMIT;
	state.IDTR.attrib=0;

	state.DR7=RESET_DR7;

	state.EAX=RESET_EAX;
	SetDX(RESET_DX);
	state.CR0=RESET_CR0;

	state.holdIRQ=false;
}

std::vector <std::string> i486DX::GetStateText(void) const
{
	std::vector <std::string> text;

	text.push_back(
	     "CS:EIP="
	    +cpputil::Ustox(state.CS.value)+":"+cpputil::Uitox(state.EIP)
	    +"  EFLAGS="+cpputil::Uitox(state.EFLAGS));

	text.push_back(
	     "EAX="+cpputil::Uitox(state.EAX)
	    +"  EBX="+cpputil::Uitox(state.EBX)
	    +"  ECX="+cpputil::Uitox(state.ECX)
	    +"  EDX="+cpputil::Uitox(state.EDX)
	    );

	text.push_back(
	     "ESI="+cpputil::Uitox(state.ESI)
	    +"  EDI="+cpputil::Uitox(state.EDI)
	    +"  EBP="+cpputil::Uitox(state.EBP)
	    +"  ESP="+cpputil::Uitox(state.ESP)
	    );

	text.push_back(
	     "CR0="+cpputil::Uitox(state.CR0)
	    +"  CR1="+cpputil::Uitox(state.CR1)
	    +"  CR2="+cpputil::Uitox(state.CR2)
	    +"  CR3="+cpputil::Uitox(state.CR3)
	    );

	text.push_back(
	     std::string("CF")+cpputil::BoolToNumberStr(GetCF())
	    +"  PF"+cpputil::BoolToNumberStr(GetPF())
	    +"  AF"+cpputil::BoolToNumberStr(GetAF())
	    +"  ZF"+cpputil::BoolToNumberStr(GetZF())
	    +"  SF"+cpputil::BoolToNumberStr(GetSF())
	    +"  TF"+cpputil::BoolToNumberStr(GetTF())
	    +"  IF"+cpputil::BoolToNumberStr(GetIF())
	    +"  DF"+cpputil::BoolToNumberStr(GetDF())
	    +"  OF"+cpputil::BoolToNumberStr(GetOF())
	    +"  IOPL"+cpputil::Ubtox(GetIOPL())
	    +"  NT"+cpputil::BoolToNumberStr(GetNT())
	    +"  RF"+cpputil::BoolToNumberStr(GetRF())
	    +"  VM"+cpputil::BoolToNumberStr(GetVM())
	    +"  AC"+cpputil::BoolToNumberStr(GetAC())
	    );

	return text;
}

void i486DX::PrintState(void) const
{
	for(auto &str : GetStateText())
	{
		std::cout << str << std::endl;
	}
}

void i486DX::LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem)
{
	if(&reg==&state.SS)
	{
		state.holdIRQ=true;
	}
	if(true==IsInRealMode())
	{
		LoadSegmentRegisterRealMode(reg,value);
	}
	else
	{
		Abort("Protected mode not supported yet.");
	}
}

void i486DX::LoadSegmentRegisterRealMode(SegmentRegister &reg,unsigned int value)
{
	if(&reg==&state.SS)
	{
		state.holdIRQ=true;
	}
	reg.value=value;
	reg.baseLinearAddr=(value<<4);
}

unsigned int i486DX::GetRegisterValue(int reg) const
{
	switch(reg)
	{
	case REG_AL:
		return state.EAX&255;
	case REG_CL:
		return state.ECX&255;
	case REG_DL:
		return state.EDX&255;
	case REG_BL:
		return state.EBX&255;
	case REG_AH:
		return (state.EAX>>8)&255;
	case REG_CH:
		return (state.ECX>>8)&255;
	case REG_DH:
		return (state.EDX>>8)&255;
	case REG_BH:
		return (state.EBX>>8)&255;

	case REG_AX:
		return state.EAX&65535;
	case REG_CX:
		return state.ECX&65535;
	case REG_DX:
		return state.EDX&65535;
	case REG_BX:
		return state.EBX&65535;
	case REG_SP:
		return state.ESP&65535;
	case REG_BP:
		return state.EBP&65535;
	case REG_SI:
		return state.ESI&65535;
	case REG_DI:
		return state.EDI&65535;

	case REG_EAX:
		return state.EAX;
	case REG_ECX:
		return state.ECX;
	case REG_EDX:
		return state.EDX;
	case REG_EBX:
		return state.EBX;
	case REG_ESP:
		return state.ESP;
	case REG_EBP:
		return state.EBP;
	case REG_ESI:
		return state.ESI;
	case REG_EDI:
		return state.EDI;

	case REG_EIP:
		return state.EIP;
	case REG_EFLAGS:
		return state.EFLAGS;

	case REG_ES:
		return state.ES.value;
	case REG_CS:
		return state.CS.value;
	case REG_SS:
		return state.SS.value;
	case REG_DS:
		return state.DS.value;
	case REG_FS:
		return state.FS.value;
	case REG_GS:
		return state.GS.value;

	//case REG_GDT:
	//case REG_LDT:
	//case REG_TR:
	//case REG_IDTR:

	case REG_CR0:
		return state.CR0;
	case REG_CR1:
		return state.CR1;
	case REG_CR2:
		return state.CR2;
	case REG_CR3:
		return state.CR3;
	case REG_DR0:
		return state.DR0;
	case REG_DR1:
		return state.DR1;
	case REG_DR2:
		return state.DR2;
	case REG_DR3:
		return state.DR3;
	case REG_DR4:
		return state.DR4;
	case REG_DR5:
		return state.DR5;
	case REG_DR6:
		return state.DR6;
	case REG_DR7:
		return state.DR7;
	}
	return 0;
}

/* static */ unsigned int i486DX::GetRegisterSize(int reg)
{
	switch(reg)
	{
	case REG_AL:
	case REG_CL:
	case REG_DL:
	case REG_BL:
	case REG_AH:
	case REG_CH:
	case REG_DH:
	case REG_BH:
		return 1;

	case REG_AX:
	case REG_CX:
	case REG_DX:
	case REG_BX:
	case REG_SP:
	case REG_BP:
	case REG_SI:
	case REG_DI:
		return 2;

	case REG_EAX:
	case REG_ECX:
	case REG_EDX:
	case REG_EBX:
	case REG_ESP:
	case REG_EBP:
	case REG_ESI:
	case REG_EDI:
	case REG_EIP:
	case REG_EFLAGS:
		return 4;

	case REG_ES:
	case REG_CS:
	case REG_SS:
	case REG_DS:
	case REG_FS:
	case REG_GS:
		return 2;

	//case REG_GDT:
	//case REG_LDT:
	//case REG_TR:
	//case REG_IDTR:

	case REG_CR0:
	case REG_CR1:
	case REG_CR2:
	case REG_CR3:
	case REG_DR0:
	case REG_DR1:
	case REG_DR2:
	case REG_DR3:
	case REG_DR4:
	case REG_DR5:
	case REG_DR6:
	case REG_DR7:
		return 4;
	}
	return 0;
}

std::string i486DX::Disassemble(const Instruction &inst,SegmentRegister seg,unsigned int offset,const Memory &mem) const
{
	std::string disasm;
	disasm+=cpputil::Ustox(seg.value);
	disasm+=":";
	disasm+=cpputil::Uitox(offset);
	disasm+=" ";

	for(unsigned int i=0; i<inst.numBytes; ++i)
	{
		disasm+=cpputil::Ubtox(FetchByte(seg,offset+i,mem));
	}
	disasm+=" ";

	cpputil::ExtendString(disasm,40);
	disasm+=inst.Disassemble(seg,offset);

	return disasm;
}

void i486DX::Move(Memory &mem,int addressSize,int segmentOverride,const Operand &dst,const Operand &src)
{
	auto value=EvaluateOperand(mem,addressSize,segmentOverride,src,dst.GetSize());
	StoreOperandValue(dst,mem,addressSize,segmentOverride,value);
}

// OF SF ZF AF PF
void i486DX::DecrementWordOrDword(unsigned int operandSize,unsigned int &value)
{
	if(16==operandSize)
	{
		DecrementWord(value);
	}
	else
	{
		DecrementDword(value);
	}
}
void i486DX::DecrementDword(unsigned int &value)
{
	--value;
	SetOverflowFlag(value==0x7FFFFFFF);
	SetSignFlag(0!=(value&0x80000000));
	SetZeroFlag(0==value);
	SetAuxCarryFlag(0x0F==(value&0x0F));
	SetParityFlag(CheckParity(value&0xFF));
}
void i486DX::DecrementWord(unsigned int &value)
{
	value=((value-1)&0xFFFF);
	SetOverflowFlag(value==0x7FFF);
	SetSignFlag(0!=(value&0x8000));
	SetZeroFlag(0==value);
	SetAuxCarryFlag(0x0F==(value&0x0F));
	SetParityFlag(CheckParity(value&0xFF));
}
void i486DX::DecrementByte(unsigned int &value)
{
	value=((value-1)&0xFF);
	SetOverflowFlag(value==0x7F);
	SetSignFlag(0!=(value&0x80));
	SetZeroFlag(0==value);
	SetAuxCarryFlag(0x0F==(value&0x0F));
	SetParityFlag(CheckParity(value&0xFF));
}



void i486DX::AddWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
{
	if(16==operandSize)
	{
		AddWord(value1,value2);
	}
	else
	{
		AddDword(value1,value2);
	}
}
void i486DX::AddDword(unsigned int &value1,unsigned int value2)
{
	auto prevValue=value1&0xffffffff;
	value1=(value1+value2)&0xffffffff;
	SetOverflowFlag(prevValue<0x80000000 && 0x80000000<=value1);
	SetSignFlag(0!=(value1&0x80000000));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0x0F)<(value1&0x0F));
	SetCarryFlag(value1<prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::AddWord(unsigned int &value1,unsigned int value2)
{
	auto prevValue=value1&0xffff;
	value1=(value1+value2)&0xffff;
	SetOverflowFlag(prevValue<0x8000 && 0x8000<=value1);
	SetSignFlag(0!=(value1&0x8000));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0x0F)<(value1&0x0F));
	SetCarryFlag(value1<prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::AddByte(unsigned int &value1,unsigned int value2)
{
	auto prevValue=value1&0xff;
	value1=(value1+value2)&0xff;
	SetOverflowFlag(prevValue<0x80 && 0x80<=value1);
	SetSignFlag(0!=(value1&0x80));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0x0F)<(value1&0x0F));
	SetCarryFlag(value1<prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::AndWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
{
	if(16==operandSize)
	{
		AndWord(value1,value2);
	}
	else
	{
		AndDword(value1,value2);
	}
}
void i486DX::AndDword(unsigned int &value1,unsigned int value2)
{
	SetCarryFlag(false);
	SetOverflowFlag(false);
	value1&=value2;
	SetSignFlag(0!=(0x80000000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::AndWord(unsigned int &value1,unsigned int value2)
{
	SetCarryFlag(false);
	SetOverflowFlag(false);
	value1&=value2;
	value1&=0xFFFF;
	SetSignFlag(0!=(0x8000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::AndByte(unsigned int &value1,unsigned int value2)
{
	SetCarryFlag(false);
	SetOverflowFlag(false);
	value1&=value2;
	value1&=0xFF;
	SetSignFlag(0!=(0x80&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::SubWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
{
	if(16==operandSize)
	{
		SubWord(value1,value2);
	}
	else
	{
		SubDword(value1,value2);
	}
}
void i486DX::SubDword(unsigned int &value1,unsigned int value2)
{
	auto prevValue=value1&0xffffffff;
	value1=(value1-value2)&0xffffffff;
	SetOverflowFlag(prevValue>=0x80000000 && 0x80000000>value1);
	SetSignFlag(0!=(value1&0x80000000));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0xFF)>=0x10 && (value1&0xFF)<=0x10);
	SetCarryFlag(value1>prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::SubWord(unsigned int &value1,unsigned int value2)
{
	auto prevValue=value1&0xffff;
	value1=(value1-value2)&0xffff;
	SetOverflowFlag(prevValue>=0x8000 && 0x8000>value1);
	SetSignFlag(0!=(value1&0x8000));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0xFF)>=0x10 && (value1&0xFF)<=0x10);
	SetCarryFlag(value1>prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::SubByte(unsigned int &value1,unsigned int value2)
{
	auto prevValue=value1&0xff;
	value1=(value1-value2)&0xff;
	SetOverflowFlag(prevValue>=0x80 && 0x80>value1);
	SetSignFlag(0!=(value1&0x80));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0xFF)>=0x10 && (value1&0xFF)<=0x10);
	SetCarryFlag(value1>prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::AdcWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
{
	if(16==operandSize)
	{
		AdcWord(value1,value2);
	}
	else
	{
		AdcDword(value1,value2);
	}
}
void i486DX::AdcDword(unsigned int &value1,unsigned int value2)
{
	auto carry=(0!=(state.EFLAGS&EFLAGS_CARRY) ? 1 : 0);
	auto prevValue=value1&0xffffffff;
	value1=(value1+value2+carry)&0xffffffff;
	SetOverflowFlag(prevValue<0x80000000 && 0x80000000<=value1);
	SetSignFlag(0!=(value1&0x80000000));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0x0F)<(value1&0x0F));
	SetCarryFlag(value1<prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::AdcWord(unsigned int &value1,unsigned int value2)
{
	auto carry=(0!=(state.EFLAGS&EFLAGS_CARRY) ? 1 : 0);
	auto prevValue=value1&0xffff;
	value1=(value1+value2+carry)&0xffff;
	SetOverflowFlag(prevValue<0x8000 && 0x8000<=value1);
	SetSignFlag(0!=(value1&0x8000));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0x0F)<(value1&0x0F));
	SetCarryFlag(value1<prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::AdcByte(unsigned int &value1,unsigned int value2)
{
	auto carry=(0!=(state.EFLAGS&EFLAGS_CARRY) ? 1 : 0);
	auto prevValue=value1&0xff;
	value1=(value1+value2+carry)&0xff;
	SetOverflowFlag(prevValue<0x80 && 0x80<=value1);
	SetSignFlag(0!=(value1&0x80));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0x0F)<(value1&0x0F));
	SetCarryFlag(value1<prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::SbbWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
{
	if(16==operandSize)
	{
		SbbWord(value1,value2);
	}
	else
	{
		SbbDword(value1,value2);
	}
}
void i486DX::SbbDword(unsigned int &value1,unsigned int value2)
{
	auto carry=(0!=(state.EFLAGS&EFLAGS_CARRY) ? 1 : 0);
	auto prevValue=value1&0xffffffff;
	value1=(value1-value2-carry)&0xffffffff;
	SetOverflowFlag(prevValue>=0x80000000 && 0x80000000>value1);
	SetSignFlag(0!=(value1&0x80000000));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0xFF)>=0x10 && (value1&0xFF)<=0x10);
	SetCarryFlag(value1>prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::SbbWord(unsigned int &value1,unsigned int value2)
{
	auto carry=(0!=(state.EFLAGS&EFLAGS_CARRY) ? 1 : 0);
	auto prevValue=value1&0xffff;
	value1=(value1-value2-carry)&0xffff;
	SetOverflowFlag(prevValue>=0x8000 && 0x8000>value1);
	SetSignFlag(0!=(value1&0x8000));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0xFF)>=0x10 && (value1&0xFF)<=0x10);
	SetCarryFlag(value1>prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::SbbByte(unsigned int &value1,unsigned int value2)
{
	auto carry=(0!=(state.EFLAGS&EFLAGS_CARRY) ? 1 : 0);
	auto prevValue=value1&0xff;
	value1=(value1-value2-carry)&0xff;
	SetOverflowFlag(prevValue>=0x80 && 0x80>value1);
	SetSignFlag(0!=(value1&0x80));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0xFF)>=0x10 && (value1&0xFF)<=0x10);
	SetCarryFlag(value1>prevValue);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::OrWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
{
	if(16==operandSize)
	{
		OrWord(value1,value2);
	}
	else
	{
		OrDword(value1,value2);
	}
}
void i486DX::OrDword(unsigned int &value1,unsigned int value2)
{
	SetCarryFlag(false);
	SetOverflowFlag(false);
	value1|=value2;
	SetSignFlag(0!=(0x80000000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::OrWord(unsigned int &value1,unsigned int value2)
{
	SetCarryFlag(false);
	SetOverflowFlag(false);
	value1|=value2;
	value1&=0xFFFF;
	SetSignFlag(0!=(0x8000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::OrByte(unsigned int &value1,unsigned int value2)
{
	SetCarryFlag(false);
	SetOverflowFlag(false);
	value1|=value2;
	value1&=0xFF;
	SetSignFlag(0!=(0x80&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::XorWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
{
	if(16==operandSize)
	{
		XorWord(value1,value2);
	}
	else
	{
		XorDword(value1,value2);
	}
}
void i486DX::XorDword(unsigned int &value1,unsigned int value2)
{
	SetCarryFlag(false);
	SetOverflowFlag(false);
	value1^=value2;
	SetSignFlag(0!=(0x80000000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::XorWord(unsigned int &value1,unsigned int value2)
{
	SetCarryFlag(false);
	SetOverflowFlag(false);
	value1^=value2;
	value1&=0xFFFF;
	SetSignFlag(0!=(0x8000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::XorByte(unsigned int &value1,unsigned int value2)
{
	SetCarryFlag(false);
	SetOverflowFlag(false);
	value1^=value2;
	value1&=0xFF;
	SetSignFlag(0!=(0x80&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}



i486DX::OperandValue i486DX::EvaluateOperand(
    const Memory &mem,int addressSize,int segmentOverride,const Operand &op,int destinationBytes) const
{
	i486DX::OperandValue value;
	value.numBytes=0;
	switch(op.operandType)
	{
	case OPER_UNDEFINED:
		Abort("Tried to evaluate an undefined operand.");
		break;
	case OPER_ADDR:
		{
			value.numBytes=destinationBytes;
			SegmentRegister seg;
			switch(segmentOverride)
			{
			case SEG_OVERRIDE_CS:
				seg=state.CS;
				break;
			case SEG_OVERRIDE_SS:
				seg=state.SS;
				break;
			case SEG_OVERRIDE_DS:
				seg=state.DS;
				break;
			case SEG_OVERRIDE_ES:
				seg=state.ES;
				break;
			case SEG_OVERRIDE_FS:
				seg=state.FS;
				break;
			case SEG_OVERRIDE_GS:
				seg=state.GS;
				break;
			default:
				if(op.baseReg==REG_ESP || op.baseReg==REG_SP ||
				   op.baseReg==REG_EBP || op.baseReg==REG_BP)
				{
					seg=state.SS;
				}
				else
				{
					seg=state.DS;
				}
				break;
			}
			unsigned int offset=
			   GetRegisterValue(op.baseReg)+
			   GetRegisterValue(op.indexReg)*op.indexScaling+
			   op.offset;
			if(addressSize==16)
			{
				for(int i=0; i<value.numBytes; ++i)
				{
					value.byteData[i]=FetchByte(seg,(offset+i)&65535,mem);
				}
			}
			else
			{
				for(int i=0; i<value.numBytes; ++i)
				{
					value.byteData[i]=FetchByte(seg,offset+i,mem);
				}
			}
		}
		break;
	case OPER_FARADDR:
		Abort("Tried to evaluate FAR ADDRESS.");
		break;
	case OPER_REG:
		switch(op.reg)
		{
		case REG_AL:
			value.numBytes=1;
			value.byteData[0]=(state.EAX&255);
			break;
		case REG_CL:
			value.numBytes=1;
			value.byteData[0]=(state.ECX&255);
			break;
		case REG_BL:
			value.numBytes=1;
			value.byteData[0]=(state.EBX&255);
			break;
		case REG_DL:
			value.numBytes=1;
			value.byteData[0]=(state.EDX&255);
			break;
		case REG_AH:
			value.numBytes=1;
			value.byteData[0]=((state.EAX>>8)&255);
			break;
		case REG_CH:
			value.numBytes=1;
			value.byteData[0]=((state.EAX>>8)&255);
			break;
		case REG_BH:
			value.numBytes=1;
			value.byteData[0]=((state.EAX>>8)&255);
			break;
		case REG_DH:
			value.numBytes=1;
			value.byteData[0]=((state.EAX>>8)&255);
			break;

		case REG_AX:
			value.numBytes=2;
			value.byteData[0]=(state.EAX&255);
			value.byteData[1]=((state.EAX>>8)&255);
			break;
		case REG_CX:
			value.numBytes=2;
			value.byteData[0]=(state.ECX&255);
			value.byteData[1]=((state.ECX>>8)&255);
			break;
		case REG_DX:
			value.numBytes=2;
			value.byteData[0]=(state.EDX&255);
			value.byteData[1]=((state.EDX>>8)&255);
			break;
		case REG_BX:
			value.numBytes=2;
			value.byteData[0]=(state.EBX&255);
			value.byteData[1]=((state.EBX>>8)&255);
			break;
		case REG_SP:
			value.numBytes=2;
			value.byteData[0]=(state.ESP&255);
			value.byteData[1]=((state.ESP>>8)&255);
			break;
		case REG_BP:
			value.numBytes=2;
			value.byteData[0]=(state.EBP&255);
			value.byteData[1]=((state.EBP>>8)&255);
			break;
		case REG_SI:
			value.numBytes=2;
			value.byteData[0]=(state.ESI&255);
			value.byteData[1]=((state.ESI>>8)&255);
			break;
		case REG_DI:
			value.numBytes=2;
			value.byteData[0]=(state.EDI&255);
			value.byteData[1]=((state.EDI>>8)&255);
			break;

		case REG_EAX:
			value.numBytes=4;
			value.byteData[0]=(state.EAX&255);
			value.byteData[1]=((state.EAX>>8)&255);
			value.byteData[2]=((state.EAX>>16)&255);
			value.byteData[3]=((state.EAX>>24)&255);
			break;
		case REG_ECX:
			value.numBytes=4;
			value.byteData[0]=(state.ECX&255);
			value.byteData[1]=((state.ECX>>8)&255);
			value.byteData[2]=((state.ECX>>16)&255);
			value.byteData[3]=((state.ECX>>24)&255);
			break;
		case REG_EDX:
			value.numBytes=4;
			value.byteData[0]=(state.EDX&255);
			value.byteData[1]=((state.EDX>>8)&255);
			value.byteData[2]=((state.EDX>>16)&255);
			value.byteData[3]=((state.EDX>>24)&255);
			break;
		case REG_EBX:
			value.numBytes=4;
			value.byteData[0]=(state.EBX&255);
			value.byteData[1]=((state.EBX>>8)&255);
			value.byteData[2]=((state.EBX>>16)&255);
			value.byteData[3]=((state.EBX>>24)&255);
			break;
		case REG_ESP:
			value.numBytes=4;
			value.byteData[0]=(state.ESP&255);
			value.byteData[1]=((state.ESP>>8)&255);
			value.byteData[2]=((state.ESP>>16)&255);
			value.byteData[3]=((state.ESP>>24)&255);
			break;
		case REG_EBP:
			value.numBytes=4;
			value.byteData[0]=(state.EBP&255);
			value.byteData[1]=((state.EBP>>8)&255);
			value.byteData[2]=((state.EBP>>16)&255);
			value.byteData[3]=((state.EBP>>24)&255);
			break;
		case REG_ESI:
			value.numBytes=4;
			value.byteData[0]=(state.ESI&255);
			value.byteData[1]=((state.ESI>>8)&255);
			value.byteData[2]=((state.ESI>>16)&255);
			value.byteData[3]=((state.ESI>>24)&255);
			break;
		case REG_EDI:
			value.numBytes=4;
			value.byteData[0]=(state.EDI&255);
			value.byteData[1]=((state.EDI>>8)&255);
			value.byteData[2]=((state.EDI>>16)&255);
			value.byteData[3]=((state.EDI>>24)&255);
			break;

		case REG_EIP:
			value.numBytes=4;
			value.byteData[0]=(state.EIP&255);
			value.byteData[1]=((state.EIP>>8)&255);
			value.byteData[2]=((state.EIP>>16)&255);
			value.byteData[3]=((state.EIP>>24)&255);
			break;
		case REG_EFLAGS:
			value.numBytes=4;
			value.byteData[0]=(state.EFLAGS&255);
			value.byteData[1]=((state.EFLAGS>>8)&255);
			value.byteData[2]=((state.EFLAGS>>16)&255);
			value.byteData[3]=((state.EFLAGS>>24)&255);
			break;

		case REG_ES:
			value.numBytes=2;
			value.byteData[0]=(state.ES.value&255);
			value.byteData[1]=((state.ES.value>>8)&255);
			break;
		case REG_CS:
			value.numBytes=2;
			value.byteData[0]=(state.CS.value&255);
			value.byteData[1]=((state.CS.value>>8)&255);
			break;
		case REG_SS:
			value.numBytes=2;
			value.byteData[0]=(state.SS.value&255);
			value.byteData[1]=((state.SS.value>>8)&255);
			break;
		case REG_DS:
			value.numBytes=2;
			value.byteData[0]=(state.DS.value&255);
			value.byteData[1]=((state.DS.value>>8)&255);
			break;
		case REG_FS:
			value.numBytes=2;
			value.byteData[0]=(state.FS.value&255);
			value.byteData[1]=((state.FS.value>>8)&255);
			break;
		case REG_GS:
			value.numBytes=2;
			value.byteData[0]=(state.GS.value&255);
			value.byteData[1]=((state.GS.value>>8)&255);
			break;

		case REG_GDT:
			Abort("i486DX::EvaluateOperand, Check GDT Byte Order");
			value.numBytes=6;
			value.byteData[0]=(state.GDT.linearBaseAddr&255);
			value.byteData[1]=((state.GDT.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.GDT.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.GDT.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.GDT.limit&255);
			value.byteData[5]=((state.GDT.limit>>8)&255);
			break;
		case REG_LDT:
			Abort("i486DX::EvaluateOperand, Check LDT Byte Order");
			value.numBytes=6;
			value.byteData[0]=(state.LDT.linearBaseAddr&255);
			value.byteData[1]=((state.LDT.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.LDT.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.LDT.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.LDT.limit&255);
			value.byteData[5]=((state.LDT.limit>>8)&255);
			break;
		case REG_TR0:
			Abort("i486DX::EvaluateOperand, Check TR0 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR0.linearBaseAddr&255);
			value.byteData[1]=((state.TR0.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR0.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR0.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR0.limit&255);
			value.byteData[5]=((state.TR0.limit>>8)&255);
			value.byteData[6]=(state.TR0.selector&255);
			value.byteData[7]=((state.TR0.selector>>8)&255);
			value.byteData[8]=(state.TR0.attrib&255);
			value.byteData[9]=((state.TR0.attrib>>8)&255);
			break;
		case REG_TR1:
			Abort("i486DX::EvaluateOperand, Check TR1 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR1.linearBaseAddr&255);
			value.byteData[1]=((state.TR1.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR1.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR1.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR1.limit&255);
			value.byteData[5]=((state.TR1.limit>>8)&255);
			value.byteData[6]=(state.TR1.selector&255);
			value.byteData[7]=((state.TR1.selector>>8)&255);
			value.byteData[8]=(state.TR1.attrib&255);
			value.byteData[9]=((state.TR1.attrib>>8)&255);
			break;
		case REG_TR2:
			Abort("i486DX::EvaluateOperand, Check TR2 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR2.linearBaseAddr&255);
			value.byteData[1]=((state.TR2.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR2.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR2.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR2.limit&255);
			value.byteData[5]=((state.TR2.limit>>8)&255);
			value.byteData[6]=(state.TR2.selector&255);
			value.byteData[7]=((state.TR2.selector>>8)&255);
			value.byteData[8]=(state.TR2.attrib&255);
			value.byteData[9]=((state.TR2.attrib>>8)&255);
			break;
		case REG_TR3:
			Abort("i486DX::EvaluateOperand, Check TR3 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR3.linearBaseAddr&255);
			value.byteData[1]=((state.TR3.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR3.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR3.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR3.limit&255);
			value.byteData[5]=((state.TR3.limit>>8)&255);
			value.byteData[6]=(state.TR3.selector&255);
			value.byteData[7]=((state.TR3.selector>>8)&255);
			value.byteData[8]=(state.TR3.attrib&255);
			value.byteData[9]=((state.TR3.attrib>>8)&255);
			break;
		case REG_TR4:
			Abort("i486DX::EvaluateOperand, Check TR4 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR4.linearBaseAddr&255);
			value.byteData[1]=((state.TR4.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR4.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR4.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR4.limit&255);
			value.byteData[5]=((state.TR4.limit>>8)&255);
			value.byteData[6]=(state.TR4.selector&255);
			value.byteData[7]=((state.TR4.selector>>8)&255);
			value.byteData[8]=(state.TR4.attrib&255);
			value.byteData[9]=((state.TR4.attrib>>8)&255);
			break;
		case REG_TR5:
			Abort("i486DX::EvaluateOperand, Check TR5 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR5.linearBaseAddr&255);
			value.byteData[1]=((state.TR5.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR5.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR5.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR5.limit&255);
			value.byteData[5]=((state.TR5.limit>>8)&255);
			value.byteData[6]=(state.TR5.selector&255);
			value.byteData[7]=((state.TR5.selector>>8)&255);
			value.byteData[8]=(state.TR5.attrib&255);
			value.byteData[9]=((state.TR5.attrib>>8)&255);
			break;
		case REG_TR6:
			Abort("i486DX::EvaluateOperand, Check TR6 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR6.linearBaseAddr&255);
			value.byteData[1]=((state.TR6.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR6.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR6.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR6.limit&255);
			value.byteData[5]=((state.TR6.limit>>8)&255);
			value.byteData[6]=(state.TR6.selector&255);
			value.byteData[7]=((state.TR6.selector>>8)&255);
			value.byteData[8]=(state.TR6.attrib&255);
			value.byteData[9]=((state.TR6.attrib>>8)&255);
			break;
		case REG_TR7:
			Abort("i486DX::EvaluateOperand, Check TR7 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR7.linearBaseAddr&255);
			value.byteData[1]=((state.TR7.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR7.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR7.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR7.limit&255);
			value.byteData[5]=((state.TR7.limit>>8)&255);
			value.byteData[6]=(state.TR7.selector&255);
			value.byteData[7]=((state.TR7.selector>>8)&255);
			value.byteData[8]=(state.TR7.attrib&255);
			value.byteData[9]=((state.TR7.attrib>>8)&255);
			break;
		case REG_IDTR:
			Abort("i486DX::EvaluateOperand, Check IDTR Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.IDTR.linearBaseAddr&255);
			value.byteData[1]=((state.IDTR.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.IDTR.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.IDTR.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.IDTR.limit&255);
			value.byteData[5]=((state.IDTR.limit>>8)&255);
			value.byteData[6]=(state.IDTR.selector&255);
			value.byteData[7]=((state.IDTR.selector>>8)&255);
			value.byteData[8]=(state.IDTR.attrib&255);
			value.byteData[9]=((state.IDTR.attrib>>8)&255);
			break;
		case REG_CR0:
			value.numBytes=4;
			value.byteData[0]=(state.CR0&255);
			value.byteData[1]=((state.CR0>>8)&255);
			value.byteData[2]=((state.CR0>>16)&255);
			value.byteData[3]=((state.CR0>>24)&255);
			break;
		case REG_CR1:
			value.numBytes=4;
			value.byteData[0]=(state.CR1&255);
			value.byteData[1]=((state.CR1>>8)&255);
			value.byteData[2]=((state.CR1>>16)&255);
			value.byteData[3]=((state.CR1>>24)&255);
			break;
		case REG_CR2:
			value.numBytes=4;
			value.byteData[0]=(state.CR2&255);
			value.byteData[1]=((state.CR2>>8)&255);
			value.byteData[2]=((state.CR2>>16)&255);
			value.byteData[3]=((state.CR2>>24)&255);
			break;
		case REG_CR3:
			value.numBytes=4;
			value.byteData[0]=(state.CR3&255);
			value.byteData[1]=((state.CR3>>8)&255);
			value.byteData[2]=((state.CR3>>16)&255);
			value.byteData[3]=((state.CR3>>24)&255);
			break;
		case REG_DR0:
			value.numBytes=4;
			value.byteData[0]=(state.DR0&255);
			value.byteData[1]=((state.DR0>>8)&255);
			value.byteData[2]=((state.DR0>>16)&255);
			value.byteData[3]=((state.DR0>>24)&255);
			break;
		case REG_DR1:
			value.numBytes=4;
			value.byteData[0]=(state.DR1&255);
			value.byteData[1]=((state.DR1>>8)&255);
			value.byteData[2]=((state.DR1>>16)&255);
			value.byteData[3]=((state.DR1>>24)&255);
			break;
		case REG_DR2:
			value.numBytes=4;
			value.byteData[0]=(state.DR2&255);
			value.byteData[1]=((state.DR2>>8)&255);
			value.byteData[2]=((state.DR2>>16)&255);
			value.byteData[3]=((state.DR2>>24)&255);
			break;
		case REG_DR3:
			value.numBytes=4;
			value.byteData[0]=(state.DR3&255);
			value.byteData[1]=((state.DR3>>8)&255);
			value.byteData[2]=((state.DR3>>16)&255);
			value.byteData[3]=((state.DR3>>24)&255);
			break;
		case REG_DR4:
			value.numBytes=4;
			value.byteData[0]=(state.DR4&255);
			value.byteData[1]=((state.DR4>>8)&255);
			value.byteData[2]=((state.DR4>>16)&255);
			value.byteData[3]=((state.DR4>>24)&255);
			break;
		case REG_DR5:
			value.numBytes=4;
			value.byteData[0]=(state.DR5&255);
			value.byteData[1]=((state.DR5>>8)&255);
			value.byteData[2]=((state.DR5>>16)&255);
			value.byteData[3]=((state.DR5>>24)&255);
			break;
		case REG_DR6:
			value.numBytes=4;
			value.byteData[0]=(state.DR6&255);
			value.byteData[1]=((state.DR6>>8)&255);
			value.byteData[2]=((state.DR6>>16)&255);
			value.byteData[3]=((state.DR6>>24)&255);
			break;
		case REG_DR7:
			value.numBytes=4;
			value.byteData[0]=(state.DR7&255);
			value.byteData[1]=((state.DR7>>8)&255);
			value.byteData[2]=((state.DR7>>16)&255);
			value.byteData[3]=((state.DR7>>24)&255);
			break;
		}
		break;
	case OPER_IMM8:
		value.numBytes=1;
		value.byteData[0]=op.imm;
		break;
	case OPER_IMM16:
		value.numBytes=2;
		value.byteData[0]=(op.imm&255);
		value.byteData[1]=((op.imm>>8)&255);
		break;
	case OPER_IMM32:
		value.numBytes=4;
		value.byteData[0]=(op.imm&255);
		value.byteData[1]=((op.imm>>8)&255);
		value.byteData[2]=((op.imm>>16)&255);
		value.byteData[3]=((op.imm>>24)&255);
		break;
	}
	return value;
}

void i486DX::StoreOperandValue(
    const Operand &dst,Memory &mem,int addressSize,int segmentOverride,OperandValue value)
{
	switch(dst.operandType)
	{
	case OPER_UNDEFINED:
		Abort("Tried to evaluate an undefined operand.");
		break;
	case OPER_ADDR:
		{
			SegmentRegister seg;
			switch(segmentOverride)
			{
			case SEG_OVERRIDE_CS:
				seg=state.CS;
				break;
			case SEG_OVERRIDE_SS:
				seg=state.SS;
				break;
			case SEG_OVERRIDE_DS:
				seg=state.DS;
				break;
			case SEG_OVERRIDE_ES:
				seg=state.ES;
				break;
			case SEG_OVERRIDE_FS:
				seg=state.FS;
				break;
			case SEG_OVERRIDE_GS:
				seg=state.GS;
				break;
			default:
				if(dst.baseReg==REG_ESP || dst.baseReg==REG_SP ||
				   dst.baseReg==REG_EBP || dst.baseReg==REG_BP)
				{
					seg=state.SS;
				}
				else
				{
					seg=state.DS;
				}
				break;
			}
			unsigned int offset=
			   GetRegisterValue(dst.baseReg)+
			   GetRegisterValue(dst.indexReg)*dst.indexScaling+
			   dst.offset;
			if(addressSize==16)
			{
				for(int i=0; i<value.numBytes; ++i)
				{
					StoreByte(mem,seg,(offset+i)&65535,value.byteData[i]);
				}
			}
			else
			{
				for(int i=0; i<value.numBytes; ++i)
				{
					StoreByte(mem,seg,offset+i,value.byteData[i]);
				}
			}
		}
		break;
	case OPER_FARADDR:
		Abort("Tried to evaluate FAR ADDRESS.");
		break;
	case OPER_REG:
		switch(dst.reg)
		{
		case REG_AL:
			state.EAX&=0xffffff00;
			state.EAX|=value.byteData[0];
			break;
		case REG_CL:
			state.ECX&=0xffffff00;
			state.ECX|=value.byteData[0];
			break;
		case REG_BL:
			state.EBX&=0xffffff00;
			state.EBX|=value.byteData[0];
			break;
		case REG_DL:
			state.EDX&=0xffffff00;
			state.EDX|=value.byteData[0];
			break;
		case REG_AH:
			state.EAX&=0xffff00ff;
			state.EAX|=(((unsigned int)value.byteData[0])<<8);
			break;
		case REG_CH:
			state.ECX&=0xffff00ff;
			state.ECX|=(((unsigned int)value.byteData[0])<<8);
			break;
		case REG_BH:
			state.EBX&=0xffff00ff;
			state.EBX|=(((unsigned int)value.byteData[0])<<8);
			break;
		case REG_DH:
			state.EDX&=0xffff00ff;
			state.EDX|=(((unsigned int)value.byteData[0])<<8);
			break;

		case REG_AX:
			state.EAX&=0xffff0000;
			state.EAX|=cpputil::GetWord(value.byteData);
			break;
		case REG_CX:
			state.ECX&=0xffff0000;
			state.ECX|=cpputil::GetWord(value.byteData);
			break;
		case REG_DX:
			state.EDX&=0xffff0000;
			state.EDX|=cpputil::GetWord(value.byteData);
			break;
		case REG_BX:
			state.EBX&=0xffff0000;
			state.EBX|=cpputil::GetWord(value.byteData);
			break;
		case REG_SP:
			state.ESP&=0xffff0000;
			state.ESP|=cpputil::GetWord(value.byteData);
			break;
		case REG_BP:
			state.EBP&=0xffff0000;
			state.EBP|=cpputil::GetWord(value.byteData);
			break;
		case REG_SI:
			state.ESI&=0xffff0000;
			state.ESI|=cpputil::GetWord(value.byteData);
			break;
		case REG_DI:
			state.EDI&=0xffff0000;
			state.EDI|=cpputil::GetWord(value.byteData);
			break;

		case REG_EAX:
			state.EAX=cpputil::GetDword(value.byteData);
			break;
		case REG_ECX:
			state.ECX=cpputil::GetDword(value.byteData);
			break;
		case REG_EDX:
			state.EDX=cpputil::GetDword(value.byteData);
			break;
		case REG_EBX:
			state.EBX=cpputil::GetDword(value.byteData);
			break;
		case REG_ESP:
			state.ESP=cpputil::GetDword(value.byteData);
			break;
		case REG_EBP:
			state.EBP=cpputil::GetDword(value.byteData);
			break;
		case REG_ESI:
			state.ESI=cpputil::GetDword(value.byteData);
			break;
		case REG_EDI:
			state.ESI=cpputil::GetDword(value.byteData);
			break;

		case REG_EIP:
			state.EIP=cpputil::GetDword(value.byteData);
			break;
		case REG_EFLAGS:
			state.EFLAGS=cpputil::GetDword(value.byteData);
			break;

		case REG_ES:
			LoadSegmentRegister(state.ES,cpputil::GetWord(value.byteData),mem);
			break;
		case REG_CS:
			LoadSegmentRegister(state.CS,cpputil::GetWord(value.byteData),mem);
			break;
		case REG_SS:
			LoadSegmentRegister(state.SS,cpputil::GetWord(value.byteData),mem);
			break;
		case REG_DS:
			LoadSegmentRegister(state.DS,cpputil::GetWord(value.byteData),mem);
			break;
		case REG_FS:
			LoadSegmentRegister(state.FS,cpputil::GetWord(value.byteData),mem);
			break;
		case REG_GS:
			LoadSegmentRegister(state.GS,cpputil::GetWord(value.byteData),mem);
			break;

		case REG_GDT:
			Abort("i486DX::StoreOperandValue, Check GDT Byte Order");
			break;
		case REG_LDT:
			Abort("i486DX::StoreOperandValue, Check LDT Byte Order");
			break;
		case REG_TR0:
			Abort("i486DX::StoreOperandValue, Check TR Byte Order");
			break;
		case REG_TR1:
			Abort("i486DX::StoreOperandValue, Check TR Byte Order");
			break;
		case REG_TR2:
			Abort("i486DX::StoreOperandValue, Check TR Byte Order");
			break;
		case REG_TR3:
			Abort("i486DX::StoreOperandValue, Check TR Byte Order");
			break;
		case REG_TR4:
			Abort("i486DX::StoreOperandValue, Check TR Byte Order");
			break;
		case REG_TR5:
			Abort("i486DX::StoreOperandValue, Check TR Byte Order");
			break;
		case REG_TR6:
			Abort("i486DX::StoreOperandValue, Check TR Byte Order");
			break;
		case REG_TR7:
			Abort("i486DX::StoreOperandValue, Check TR Byte Order");
			break;
		case REG_IDTR:
			Abort("i486DX::StoreOperandValue, Check IDTR Byte Order");
			break;
		case REG_CR0:
			state.CR0=cpputil::GetDword(value.byteData);
			break;
		case REG_CR1:
			state.CR1=cpputil::GetDword(value.byteData);
			break;
		case REG_CR2:
			state.CR2=cpputil::GetDword(value.byteData);
			break;
		case REG_CR3:
			state.CR3=cpputil::GetDword(value.byteData);
			break;
		case REG_DR0:
			state.DR0=cpputil::GetDword(value.byteData);
			break;
		case REG_DR1:
			state.DR1=cpputil::GetDword(value.byteData);
			break;
		case REG_DR2:
			state.DR2=cpputil::GetDword(value.byteData);
			break;
		case REG_DR3:
			state.DR3=cpputil::GetDword(value.byteData);
			break;
		case REG_DR4:
			state.DR4=cpputil::GetDword(value.byteData);
			break;
		case REG_DR5:
			state.DR5=cpputil::GetDword(value.byteData);
			break;
		case REG_DR6:
			state.DR6=cpputil::GetDword(value.byteData);
			break;
		case REG_DR7:
			state.DR7=cpputil::GetDword(value.byteData);
			break;
		}
		break;
	case OPER_IMM8:
	case OPER_IMM16:
	case OPER_IMM32:
		Abort("Immediate value specified as a destination.");
		break;
	}
}

