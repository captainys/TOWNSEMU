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
	"BL",
	"DL",
	"AH",
	"CH",
	"BH",
	"DH",

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
	"TR",
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
