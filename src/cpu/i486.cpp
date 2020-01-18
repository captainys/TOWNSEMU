#include "i486.h"



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
}

void i486DX::LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem)
{
}

void i486DX::LoadSegmentRegisterRealMode(SegmentRegister &reg,unsigned int value)
{
	reg.value=value;
	reg.baseLinearAddr=(value<<4);
}
