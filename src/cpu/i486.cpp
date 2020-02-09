#include <iostream>

#include "cpputil.h"
#include "i486.h"


const char *const i486DX::Reg8Str[8]=
{
	"AL","CL","DL","BL","AH","CH","DH","BH"
};

const char *const i486DX::Reg16Str[8]=
{
	"AX","CX","DX","BX","SP","BP","SI","DI"
};

const char *const i486DX::Reg32Str[8]=
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
	enableCallStack=false;
	debuggerPtr=nullptr;
}

void i486DX::Reset(void)
{
	// page 10-1 [1]
	state.EFLAGS=RESET_EFLAGS;

	state.EIP=RESET_EIP;
	state.CS().value=RESET_CS;
	state.CS().baseLinearAddr=0xFFFF0000;

	LoadSegmentRegisterRealMode(state.DS(),RESET_DS);
	LoadSegmentRegisterRealMode(state.SS(),RESET_SS);
	LoadSegmentRegisterRealMode(state.ES(),RESET_ES);
	LoadSegmentRegisterRealMode(state.FS(),RESET_FS);
	LoadSegmentRegisterRealMode(state.GS(),RESET_GS);

	state.IDTR.linearBaseAddr=RESET_IDTRBASE;
	state.IDTR.limit=RESET_IDTRLIMIT;

	state.DR[7]=RESET_DR7;

	state.EAX()=RESET_EAX;
	SetDX(RESET_DX);
	state.CR[0]=RESET_CR0;
	if(true!=state.fpuState.FPUEnabled)
	{
		state.CR[0]&=(~CR0_MATH_PRESENT);
	}

	state.halt=false;
	state.holdIRQ=false;
	state.exception=false;
}

std::vector <std::string> i486DX::GetStateText(void) const
{
	std::vector <std::string> text;

	text.push_back(
	     "CS:EIP="
	    +cpputil::Ustox(state.CS().value)+":"+cpputil::Uitox(state.EIP)
	    +"  LINEAR:"+cpputil::Uitox(state.CS().baseLinearAddr+state.EIP)
	    +"  EFLAGS="+cpputil::Uitox(state.EFLAGS));

	text.push_back(
	     "EAX="+cpputil::Uitox(state.EAX())
	    +"  EBX="+cpputil::Uitox(state.EBX())
	    +"  ECX="+cpputil::Uitox(state.ECX())
	    +"  EDX="+cpputil::Uitox(state.EDX())
	    );

	text.push_back(
	     "ESI="+cpputil::Uitox(state.ESI())
	    +"  EDI="+cpputil::Uitox(state.EDI())
	    +"  EBP="+cpputil::Uitox(state.EBP())
	    +"  ESP="+cpputil::Uitox(state.ESP())
	    );

	text.push_back(
	     "CS="+cpputil::Ustox(state.CS().value)
	    +"(LIN:"+cpputil::Uitox(state.CS().baseLinearAddr)+")"
	    +"  "
	     "DS="+cpputil::Ustox(state.DS().value)
	    +"(LIN:"+cpputil::Uitox(state.DS().baseLinearAddr)+")"
	    +"  "
	     "ES="+cpputil::Ustox(state.ES().value)
	    +"(LIN:"+cpputil::Uitox(state.ES().baseLinearAddr)+")"
	    +"  ");

	text.push_back(
	     "FS="+cpputil::Ustox(state.FS().value)
	    +"(LIN:"+cpputil::Uitox(state.FS().baseLinearAddr)+")"
	    +"  "
	     "GS="+cpputil::Ustox(state.GS().value)
	    +"(LIN:"+cpputil::Uitox(state.GS().baseLinearAddr)+")"
	    +"  "
	    +"SS="+cpputil::Ustox(state.SS().value)
	    +"(LIN:"+cpputil::Uitox(state.SS().baseLinearAddr)+")"
	    +"  ");

	text.push_back(
	     "CR0="+cpputil::Uitox(state.CR[0])
	    +"  CR1="+cpputil::Uitox(state.CR[1])
	    +"  CR2="+cpputil::Uitox(state.CR[2])
	    +"  CR3="+cpputil::Uitox(state.CR[3])
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

	text.push_back(
	     std::string("Default Operand Size=")+cpputil::Itoa(state.CS().operandSize)
	    +std::string("  Default Address Size=")+cpputil::Itoa(state.CS().addressSize)
	    +std::string("  Stack Address Size=")+cpputil::Itoa(state.SS().addressSize));

	if(true==state.exception)
	{
		text.push_back("!EXCEPTION!");
	}
	if(true==state.holdIRQ)
	{
		text.push_back("HOLD IRQ for 1 Instruction");
	}

	return text;
}

std::vector <std::string> i486DX::GetGDTText(const Memory &mem) const
{
	std::vector <std::string> text;
	text.push_back("GDT  Limit=");
	text.back()+=cpputil::Ustox(state.GDTR.limit);

	for(unsigned int selector=0; selector<state.GDTR.limit; selector+=8)
	{
		unsigned int DTLinearBaseAddr=state.GDTR.linearBaseAddr+selector;
		const unsigned char rawDesc[8]=
		{
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+1),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+2),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+3),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+4),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+5),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+6),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+7)
		};

		// Sample GDT from WRHIGH.ASM
		//	DB		0FFH,0FFH	; Segment Limit (0-15)
		//	DB		0,0,010H		; Base Address 0-23
		//	DB		10010010B	; P=1, DPL=00, S=1, TYPE=0010
		//	DB		11000111B	; G=1, DB=1, (Unused)=0, A=0, LIMIT 16-19=0011
		//	DB		0			; Base Address 24-31

		unsigned int segLimit=rawDesc[0]|(rawDesc[1]<<8)|((rawDesc[6]&0x0F)<<16);
		unsigned int segBase=rawDesc[2]|(rawDesc[3]<<8)|(rawDesc[4]<<16)|(rawDesc[7]<<24);
		if((0x80&rawDesc[6])==0) // G==0
		{
			segLimit=segLimit;
		}
		else
		{
			segLimit=(segLimit+1)*4096-1;
		}

		unsigned int addressSize,operandSize;
		if((0x40&rawDesc[6])==0) // D==0
		{
			addressSize=16;
			operandSize=16;
		}
		else
		{
			addressSize=32;
			operandSize=32;
		}

		std::string empty;
		text.push_back(empty);
		text.back()+=cpputil::Ustox(selector);
		text.back()+=":";
		text.back()+="Lin-Base=";
		text.back()+=cpputil::Uitox(segBase);
		text.back()+="  Limit=";
		text.back()+=cpputil::Uitox(segLimit);
		text.back()+="  OperSize=";
		text.back()+=cpputil::Ubtox(operandSize);
		text.back()+="H  AddrSize=";
		text.back()+=cpputil::Ubtox(addressSize);
		text.back()+="H";
	}
	return text;
}

std::vector <std::string> i486DX::GetIDTText(const Memory &mem) const
{
	std::vector <std::string> text;
	std::string empty;

	text.push_back(empty);
	text.back()="Limit="+cpputil::Uitox(state.IDTR.limit);
	for(unsigned int offset=0; offset<state.IDTR.limit && offset<0x800; offset+=8)
	{
		auto desc=GetInterruptDescriptor(offset/8,mem);
		text.push_back(empty);
		text.back()=cpputil::Ubtox(offset/8);
		text.back()+=":";
		text.back()+="SEG=";
		text.back()+=cpputil::Ustox(desc.SEG);
		text.back()+="  OFFSET=";
		text.back()+=cpputil::Uitox(desc.OFFSET);

		auto type=(desc.flags>>8)&0x1F;
		text.back()+="  TYPE=";
		text.back()+=cpputil::Ubtox(type);
		text.back()+="(";
		// https://wiki.osdev.org/Interrupt_Descriptor_Table
		switch(type)
		{
		default:
			text.back()+="UNKNOWN        ";
			break;
		case 0b0101:
			text.back()+="386 32-bit Task";
			break;
		case 0b0110:
			text.back()+="286 16-bit INT";
			break;
		case 0b0111:
			text.back()+="286 16-bit Trap";
			break;
		case 0b1110:
			text.back()+="386 32-bit INT";
			break;
		case 0b1111:
			text.back()+="386 32-bit Trap";
			break;
		}
		text.back()+=")";
	}

	return text;
}

void i486DX::PrintState(void) const
{
	for(auto &str : GetStateText())
	{
		std::cout << str << std::endl;
	}
}

void i486DX::PrintGDT(const Memory &mem) const
{
	for(auto &str : GetGDTText(mem))
	{
		std::cout << str << std::endl;
	}
}

void i486DX::PrintIDT(const Memory &mem) const
{
	for(auto &str : GetIDTText(mem))
	{
		std::cout << str << std::endl;
	}
}

inline void i486DX::LoadSegmentRegisterQuiet(SegmentRegister &reg,unsigned int value,const Memory &mem,bool isInRealMode) const
{
	if(true==isInRealMode)
	{
		reg.value=value;
		reg.baseLinearAddr=(value<<4);
		reg.addressSize=16;
		reg.operandSize=16;
		reg.limit=0xffff;
	}
	else
	{
		auto RPL=(value&3);
		auto TI=(0!=(value&4));
		auto INDEX=(value>>3)&0b0001111111111111;

		unsigned int DTLinearBaseAddr=0;
		if(0==TI)
		{
			DTLinearBaseAddr=state.GDTR.linearBaseAddr;
		}
		else
		{
			Abort("LDT not supported yet.");
		}
		DTLinearBaseAddr+=(8*INDEX);

		const unsigned char rawDesc[8]=
		{
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+1),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+2),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+3),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+4),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+5),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+6),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+7)
		};

		// Sample GDT from WRHIGH.ASM
		//	DB		0FFH,0FFH	; Segment Limit (0-15)
		//	DB		0,0,010H		; Base Address 0-23
		//	DB		10010010B	; P=1, DPL=00, S=1, TYPE=0010
		//	DB		11000111B	; G=1, DB=1, (Unused)=0, A=0, LIMIT 16-19=0011
		//	DB		0			; Base Address 24-31

		unsigned int segLimit=rawDesc[0]|(rawDesc[1]<<8)|((rawDesc[6]&0x0F)<<16);
		unsigned int segBase=rawDesc[2]|(rawDesc[3]<<8)|(rawDesc[4]<<16)|(rawDesc[7]<<24);
		if((0x80&rawDesc[6])==0) // G==0
		{
			reg.limit=segLimit;
		}
		else
		{
			reg.limit=(segLimit+1)*4096-1;
		}
		reg.baseLinearAddr=segBase;
		reg.value=value;

		if((0x40&rawDesc[6])==0) // D==0
		{
			reg.addressSize=16;
			reg.operandSize=16;
		}
		else
		{
			reg.addressSize=32;
			reg.operandSize=32;
		}
	}
}

void i486DX::LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem)
{
	if(&reg==&state.SS())
	{
		state.holdIRQ=true;
	}
	LoadSegmentRegisterQuiet(reg,value,mem,IsInRealMode());
}

void i486DX::LoadSegmentRegisterRealMode(SegmentRegister &reg,unsigned int value)
{
	if(&reg==&state.SS())
	{
		state.holdIRQ=true;
	}
	reg.value=value;
	reg.baseLinearAddr=(value<<4);
	reg.addressSize=16;
	reg.operandSize=16;
	reg.limit=0xffff;
}

void i486DX::LoadDescriptorTableRegister(SystemAddressRegister &reg,int operandSize,const unsigned char byteData[])
{
	reg.limit=byteData[0]|(byteData[1]<<8);
	if(16==operandSize)
	{
		reg.linearBaseAddr=byteData[2]|(byteData[3]<<8)|(byteData[4]<<16);
	}
	else
	{
		reg.linearBaseAddr=byteData[2]|(byteData[3]<<8)|(byteData[4]<<16)|(byteData[5]<<24);
	}
}

i486DX::InterruptDescriptor i486DX::GetInterruptDescriptor(unsigned int INTNum,const Memory &mem) const
{
	InterruptDescriptor desc;
	if(8*INTNum<state.IDTR.limit)
	{
		auto DTLinearBaseAddr=state.IDTR.linearBaseAddr;
		DTLinearBaseAddr+=(8*INTNum);
		const unsigned char rawDesc[8]=
		{
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+1),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+2),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+3),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+4),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+5),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+6),
			(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+7)
		};
		desc.SEG=rawDesc[2]|(rawDesc[3]<<8);
		desc.OFFSET= (unsigned int)rawDesc[0]
		           |((unsigned int)rawDesc[1]<<8)
		           |((unsigned int)rawDesc[6]<<16)
		           |((unsigned int)rawDesc[7]<<24);
		desc.flags=  (unsigned short)rawDesc[4]
		           |((unsigned short)rawDesc[5]<<8);
	}
	else
	{
		desc.SEG=FarPointer::NO_SEG;
		desc.OFFSET=0;
	}
	return desc;
}

i486DX::OperandValue i486DX::DescriptorTableToOperandValue(const SystemAddressRegister &reg,int operandSize) const
{
	OperandValue operaValue;
	operaValue.numBytes=6;
	operaValue.byteData[0]=reg.limit&0xFF;
	operaValue.byteData[1]=(reg.limit>>8)&0xFF;
	operaValue.byteData[2]=reg.linearBaseAddr&0xFF;
	operaValue.byteData[3]=(reg.linearBaseAddr>>8)&0xFF;
	operaValue.byteData[4]=(reg.linearBaseAddr>>16)&0xFF;
	operaValue.byteData[5]=(reg.linearBaseAddr>>24)&0xFF;
	if(16==operandSize)
	{
		operaValue.byteData[5]=0;
	}
	return operaValue;
}

unsigned int i486DX::GetRegisterValue(int reg) const
{
	switch(reg)
	{
	case REG_AL:
		return state.EAX()&255;
	case REG_CL:
		return state.ECX()&255;
	case REG_DL:
		return state.EDX()&255;
	case REG_BL:
		return state.EBX()&255;
	case REG_AH:
		return (state.EAX()>>8)&255;
	case REG_CH:
		return (state.ECX()>>8)&255;
	case REG_DH:
		return (state.EDX()>>8)&255;
	case REG_BH:
		return (state.EBX()>>8)&255;

	case REG_AX:
		return state.EAX()&65535;
	case REG_CX:
		return state.ECX()&65535;
	case REG_DX:
		return state.EDX()&65535;
	case REG_BX:
		return state.EBX()&65535;
	case REG_SP:
		return state.ESP()&65535;
	case REG_BP:
		return state.EBP()&65535;
	case REG_SI:
		return state.ESI()&65535;
	case REG_DI:
		return state.EDI()&65535;

	case REG_EAX:
		return state.EAX();
	case REG_ECX:
		return state.ECX();
	case REG_EDX:
		return state.EDX();
	case REG_EBX:
		return state.EBX();
	case REG_ESP:
		return state.ESP();
	case REG_EBP:
		return state.EBP();
	case REG_ESI:
		return state.ESI();
	case REG_EDI:
		return state.EDI();

	case REG_EIP:
		return state.EIP;
	case REG_EFLAGS:
		return state.EFLAGS;

	case REG_ES:
		return state.ES().value;
	case REG_CS:
		return state.CS().value;
	case REG_SS:
		return state.SS().value;
	case REG_DS:
		return state.DS().value;
	case REG_FS:
		return state.FS().value;
	case REG_GS:
		return state.GS().value;

	//case REG_GDT:
	//case REG_LDT:
	//case REG_TR:
	//case REG_IDTR:

	case REG_CR0:
		return state.CR[0];
	case REG_CR1:
		return state.CR[1];
	case REG_CR2:
		return state.CR[2];
	case REG_CR3:
		return state.CR[3];
	case REG_DR0:
		return state.DR[0];
	case REG_DR1:
		return state.DR[1];
	case REG_DR2:
		return state.DR[2];
	case REG_DR3:
		return state.DR[3];
	case REG_DR4:
		return state.DR[4];
	case REG_DR5:
		return state.DR[5];
	case REG_DR6:
		return state.DR[6];
	case REG_DR7:
		return state.DR[7];
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

unsigned int i486DX::GetStackAddressingSize(void) const
{
	if(true==IsInRealMode())
	{
		return 16;
	}
	else
	{
		return state.SS().addressSize;
	}
	return 0;
}

void i486DX::Push(Memory &mem,unsigned int operandSize,unsigned int value)
{
	auto addressSize=GetStackAddressingSize();
	if(16==addressSize)
	{
		auto SP=GetSP();
		if(16==operandSize)
		{
			SP-=2;
			SP&=65535;
			StoreByte(mem,addressSize,state.SS(),SP  ,value&255);
			StoreByte(mem,addressSize,state.SS(),SP+1,(value>>8)&255);
		}
		else if(32==operandSize)
		{
			SP-=4;
			SP&=65535;
			StoreByte(mem,addressSize,state.SS(),SP  ,value&255);
			StoreByte(mem,addressSize,state.SS(),SP+1,(value>>8)&255);
			StoreByte(mem,addressSize,state.SS(),SP+2,(value>>16)&255);
			StoreByte(mem,addressSize,state.SS(),SP+3,(value>>24)&255);
		}
		SetSP(SP);
	}
	else
	{
		auto ESP=GetESP();
		if(16==operandSize)
		{
			ESP-=2;
			StoreByte(mem,addressSize,state.SS(),ESP  ,value&255);
			StoreByte(mem,addressSize,state.SS(),ESP+1,(value>>8)&255);
		}
		else if(32==operandSize)
		{
			ESP-=4;
			StoreByte(mem,addressSize,state.SS(),ESP  ,value&255);
			StoreByte(mem,addressSize,state.SS(),ESP+1,(value>>8)&255);
			StoreByte(mem,addressSize,state.SS(),ESP+2,(value>>16)&255);
			StoreByte(mem,addressSize,state.SS(),ESP+3,(value>>24)&255);
		}
		SetESP(ESP);
	}
}

unsigned int i486DX::Pop(Memory &mem,unsigned int operandSize)
{
	unsigned int value;
	auto addressSize=GetStackAddressingSize();
	if(16==addressSize)
	{
		auto SP=GetSP();
		if(16==operandSize)
		{
			value=FetchByte(addressSize,state.SS(),SP,mem)|(FetchByte(addressSize,state.SS(),SP+1,mem)<<8);
			SP+=2;
		}
		else if(32==operandSize)
		{
			value= FetchByte(addressSize,state.SS(),SP,mem)
			     |(FetchByte(addressSize,state.SS(),SP+1,mem)<<8)
			     |(FetchByte(addressSize,state.SS(),SP+2,mem)<<16)
			     |(FetchByte(addressSize,state.SS(),SP+3,mem)<<24);
			SP+=4;
		}
		SetSP(SP); // SetSP does SP&=0xffff;
	}
	else
	{
		auto ESP=GetESP();
		if(16==operandSize)
		{
			value= FetchByte(addressSize,state.SS(),ESP,mem)
			     |(FetchByte(addressSize,state.SS(),ESP+1,mem)<<8);
			ESP+=2;
		}
		else if(32==operandSize)
		{
			value= FetchByte(addressSize,state.SS(),ESP,mem)
			     |(FetchByte(addressSize,state.SS(),ESP+1,mem)<<8)
			     |(FetchByte(addressSize,state.SS(),ESP+2,mem)<<16)
			     |(FetchByte(addressSize,state.SS(),ESP+3,mem)<<24);
			ESP+=4;
		}
		SetESP(ESP);
	}
	return value;
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
		disasm+=cpputil::Ubtox(FetchByte(inst.addressSize,seg,offset+i,mem));
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
void i486DX::IncrementWordOrDword(unsigned int operandSize,unsigned int &value)
{
	if(16==operandSize)
	{
		IncrementWord(value);
	}
	else
	{
		IncrementDword(value);
	}
}
void i486DX::IncrementDword(unsigned int &value)
{
	SetAuxCarryFlag(0x0F==(value&0x0F));
	++value;
	SetOverflowFlag(value==0x80000000);
	SetSignFlag(0!=(value&0x80000000));
	SetZeroFlag(0==value);
	SetParityFlag(CheckParity(value&0xFF));
}
void i486DX::IncrementWord(unsigned int &value)
{
	SetAuxCarryFlag(0x0F==(value&0x0F));
	value=(value+1)&0xffff;
	SetOverflowFlag(value==0x8000);
	SetSignFlag(0!=(value&0x8000));
	SetZeroFlag(0==value);
	SetParityFlag(CheckParity(value&0xFF));
}
void i486DX::IncrementByte(unsigned int &value)
{
	SetAuxCarryFlag(0x0F==(value&0x0F));
	value=(value+1)&0xff;
	SetOverflowFlag(value==0x80);
	SetSignFlag(0!=(value&0x80));
	SetZeroFlag(0==value);
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
	SetCF(value1<prevValue);
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
	SetCF(value1<prevValue);
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
	SetCF(value1<prevValue);
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
	SetCF(false);
	SetOverflowFlag(false);
	value1&=value2;
	SetSignFlag(0!=(0x80000000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::AndWord(unsigned int &value1,unsigned int value2)
{
	SetCF(false);
	SetOverflowFlag(false);
	value1&=value2;
	value1&=0xFFFF;
	SetSignFlag(0!=(0x8000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::AndByte(unsigned int &value1,unsigned int value2)
{
	SetCF(false);
	SetOverflowFlag(false);
	value1&=value2;
	value1&=0xFF;
	SetSignFlag(0!=(0x80&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::SubByteWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
{
	if(8==operandSize)
	{
		SubByte(value1,value2);
	}
	else if(16==operandSize)
	{
		SubWord(value1,value2);
	}
	else
	{
		SubDword(value1,value2);
	}
}
void i486DX::SubWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
{
	SubByteWordOrDword(operandSize,value1,value2);
}
void i486DX::SubDword(unsigned int &value1,unsigned int value2)
{
	auto prevValue=value1&0xffffffff;
	value1=(value1-value2)&0xffffffff;
	SetOverflowFlag(prevValue>=0x80000000 && 0x80000000>value1);
	SetSignFlag(0!=(value1&0x80000000));
	SetZeroFlag(0==value1);
	SetAuxCarryFlag((prevValue&0xFF)>=0x10 && (value1&0xFF)<=0x10);
	SetCF(value1>prevValue);
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
	SetCF(value1>prevValue);
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
	SetCF(value1>prevValue);
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
	SetCF(value1<prevValue);
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
	SetCF(value1<prevValue);
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
	SetCF(value1<prevValue);
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
	SetCF(value1>prevValue);
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
	SetCF(value1>prevValue);
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
	SetCF(value1>prevValue);
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
	SetCF(false);
	SetOverflowFlag(false);
	value1|=value2;
	SetSignFlag(0!=(0x80000000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::OrWord(unsigned int &value1,unsigned int value2)
{
	SetCF(false);
	SetOverflowFlag(false);
	value1|=value2;
	value1&=0xFFFF;
	SetSignFlag(0!=(0x8000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::OrByte(unsigned int &value1,unsigned int value2)
{
	SetCF(false);
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
	SetCF(false);
	SetOverflowFlag(false);
	value1^=value2;
	SetSignFlag(0!=(0x80000000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::XorWord(unsigned int &value1,unsigned int value2)
{
	SetCF(false);
	SetOverflowFlag(false);
	value1^=value2;
	value1&=0xFFFF;
	SetSignFlag(0!=(0x8000&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}
void i486DX::XorByte(unsigned int &value1,unsigned int value2)
{
	SetCF(false);
	SetOverflowFlag(false);
	value1^=value2;
	value1&=0xFF;
	SetSignFlag(0!=(0x80&value1));
	SetZeroFlag(0==value1);
	SetParityFlag(CheckParity(value1&0xFF));
}

void i486DX::RolByteWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
{
	switch(operandSize)
	{
	case 8:
		RolByte(value,ctr);
		break;
	case 16:
		RolWord(value,ctr);
		break;
	default:
	case 32:
		RolDword(value,ctr);
		break;
	}
}

void i486DX::RolDword(unsigned int &value,unsigned int ctr)
{
	unsigned long long int mask=0xFFFFFFFF;
	ctr&=0x1F;
	mask>>=(32-ctr);
	value=(value<<ctr)|((value>>(32-ctr))&mask);
	value&=0xFFFFFFFF;
}

void i486DX::RolWord(unsigned int &value,unsigned int ctr)
{
	unsigned long long int mask=0xFFFF;
	ctr&=0xF;
	mask>>=(16-ctr);
	value=(value<<ctr)|((value>>(16-ctr))&mask);
	value&=0xFFFF;
}

void i486DX::RolByte(unsigned int &value,unsigned int ctr)
{
	unsigned long long int mask=0xFF;
	ctr&=0x7;
	mask>>=(8-ctr);
	value=(value<<ctr)|((value>>(8-ctr))&mask);
	value&=0xFF;
}

void i486DX::RorByteWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
{
	switch(operandSize)
	{
	case 8:
		RorByte(value,ctr);
		break;
	case 16:
		RorWord(value,ctr);
		break;
	default:
	case 32:
		RorDword(value,ctr);
		break;
	}
}

template <unsigned int bitCount,unsigned int allBits,unsigned int signBit>
inline void i486DX::RorTemplate(unsigned int &value,unsigned int ctr)
{
	auto prevValue=value;
	unsigned int rightBitsMask=(allBits>>(bitCount-ctr));
	unsigned int rightBits=(value&rightBitsMask);
	value&=allBits;
	value>>=ctr;
	value|=(rightBits<<(bitCount-ctr));
	SetCF(0!=(value&signBit));
	if(1==ctr)
	{
		SetOverflowFlag((prevValue&signBit)!=(value&signBit));
	}
}

void i486DX::RorDword(unsigned int &value,unsigned int ctr)
{
	RorTemplate<32,0xffffffff,0x80000000>(value,ctr);
}

void i486DX::RorWord(unsigned int &value,unsigned int ctr)
{
	RorTemplate<16,0xffff,0x8000>(value,ctr);
}

void i486DX::RorByte(unsigned int &value,unsigned int ctr)
{
	RorTemplate<8,0xff,0x80>(value,ctr);
}

void i486DX::RclWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
{
	if(16==operandSize)
	{
		RclWord(value,ctr);
	}
	else
	{
		RclDword(value,ctr);
	}
}
void i486DX::RclDword(unsigned int &value,unsigned int ctr)
{
	auto prevValue=value;
	for(unsigned int i=0; i<ctr; ++i)
	{
		auto orValue=(GetCF() ? 1 : 0);
		SetCF(0!=(value&0x80000000));
		prevValue=value;
		value=(value<<1)|orValue;
	}
	if(1==ctr)
	{
		SetOverflowFlag((prevValue&0x80000000)!=(value&0x80000000));
	}
}
void i486DX::RclWord(unsigned int &value,unsigned int ctr)
{
	auto prevValue=value;
	for(unsigned int i=0; i<ctr; ++i)
	{
		auto orValue=(GetCF() ? 1 : 0);
		SetCF(0!=(value&0x8000));
		prevValue=value;
		value=(value<<1)|orValue;
	}
	value&=0xffff;
	if(1==ctr)
	{
		SetOverflowFlag((prevValue&0x8000)!=(value&0x8000));
	}
}
void i486DX::RclByte(unsigned int &value,unsigned int ctr)
{
	auto prevValue=value;
	for(unsigned int i=0; i<ctr; ++i)
	{
		auto orValue=(GetCF() ? 1 : 0);
		SetCF(0!=(value&0x80));
		prevValue=value;
		value=(value<<1)|orValue;
	}
	value&=0xff;
	if(1==ctr)
	{
		SetOverflowFlag((prevValue&0x80)!=(value&0x80));
	}
}

void i486DX::RcrWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
{
	if(16==operandSize)
	{
		RcrWord(value,ctr);
	}
	else
	{
		RcrDword(value,ctr);
	}
}
void i486DX::RcrDword(unsigned int &value,unsigned int ctr)
{
	for(unsigned int i=0; i<ctr; ++i)
	{
		unsigned int highBit=(GetCF() ? 0x80000000 : 0);
		SetCF(0!=(value&1));
		value=(value>>1)|highBit;
	}
	if(1==ctr)
	{
		SetOverflowFlag(false);
	}
}
void i486DX::RcrWord(unsigned int &value,unsigned int ctr)
{
	value&=0xffff;
	for(unsigned int i=0; i<ctr; ++i)
	{
		unsigned int highBit=(GetCF() ? 0x8000 : 0);
		SetCF(0!=(value&1));
		value=(value>>1)|highBit;
	}
	if(1==ctr)
	{
		SetOverflowFlag(false);
	}
}
void i486DX::RcrByte(unsigned int &value,unsigned int ctr)
{
	value&=0xff;
	for(unsigned int i=0; i<ctr; ++i)
	{
		unsigned int highBit=(GetCF() ? 0x80 : 0);
		SetCF(0!=(value&1));
		value=(value>>1)|highBit;
	}
	if(1==ctr)
	{
		SetOverflowFlag(false);
	}
}

void i486DX::SarByteWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
{
	switch(operandSize)
	{
	case 8:
		SarByte(value,ctr);
		break;
	case 16:
		SarWord(value,ctr);
		break;
	default:
	case 32:
		SarDword(value,ctr);
		break;
	}
}
void i486DX::SarDword(unsigned int &value,unsigned int ctr)
{
	unsigned long long int value64=value;
	if(0!=(value64&0x80000000))
	{
		value64|=0xFFFFFFFF00000000LL;
	}
	SetCF(0<ctr && 0!=(value&(1<<(ctr-1))));
	value64>>=ctr;
	value64&=0xFFFFFFFF;
	SetZeroFlag(0==value64);
	SetSignFlag(0!=(value64&0x80000000));
	SetParityFlag(CheckParity(value64&0xFF));
	if(1==ctr)
	{
		SetOverflowFlag(false);
	}
	value=(unsigned int)(value64);
}
void i486DX::SarWord(unsigned int &value,unsigned int ctr)
{
	unsigned int value32=value;
	if(0!=(value32&0x8000))
	{
		value32|=0xFFFF0000;
	}
	SetCF(0<ctr && 0!=(value&(1<<(ctr-1))));
	value32>>=ctr;
	value32&=0xFFFF;
	SetZeroFlag(0==value32);
	SetSignFlag(0!=(value32&0x8000));
	SetParityFlag(CheckParity(value32&0xFF));
	if(1==ctr)
	{
		SetOverflowFlag(false);
	}
	value=value32;
}
void i486DX::SarByte(unsigned int &value,unsigned int ctr)
{
	unsigned int value16=value;
	if(0!=(value16&0x80))
	{
		value16|=0xFF00;
	}
	SetCF(0<ctr && 0!=(value&(1<<(ctr-1))));
	value16>>=ctr;
	value16&=0xFF;
	SetZeroFlag(0==value16);
	SetSignFlag(0!=(value16&0x80));
	SetParityFlag(CheckParity(value16&0xFF));
	if(1==ctr)
	{
		SetOverflowFlag(false);
	}
	value=value16;
}

void i486DX::ShlWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
{
	if(16==operandSize)
	{
		ShlWord(value,ctr);
	}
	else
	{
		ShlDword(value,ctr);
	}
}
void i486DX::ShlDword(unsigned int &value,unsigned int ctr)
{
	if(1<ctr)
	{
		value=(value<<(ctr-1));
		SetCF(0!=(value&0x80000000));
		value=(value<<1);
	}
	else if(1==ctr)
	{
		SetCF(0!=(value&0x80000000));
		auto prevValue=value;
		value=(value<<1);
		SetOverflowFlag((prevValue&0x80000000)!=(value&0x80000000));
	}
}
void i486DX::ShlWord(unsigned int &value,unsigned int ctr)
{
	if(1<ctr)
	{
		value=(value<<(ctr-1));
		SetCF(0!=(value&0x8000));
		value=(value<<1)&0xffff;
	}
	else if(1==ctr)
	{
		SetCF(0!=(value&0x8000));
		auto prevValue=value;
		value=(value<<1)&0xffff;
		SetOverflowFlag((prevValue&0x8000)!=(value&0x8000));
	}
}
void i486DX::ShlByte(unsigned int &value,unsigned int ctr)
{
	if(1<ctr)
	{
		value=(value<<(ctr-1));
		SetCF(0!=(value&0x80));
		value=(value<<1)&0xff;
	}
	else if(1==ctr)
	{
		SetCF(0!=(value&0x80));
		auto prevValue=value;
		value=(value<<1)&0xff;
		SetOverflowFlag((prevValue&0x80)!=(value&0x80));
	}
}
void i486DX::ShrWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
{
	if(16==operandSize)
	{
		ShrWord(value,ctr);
	}
	else
	{
		ShrDword(value,ctr);
	}
}
void i486DX::ShrDword(unsigned int &value,unsigned int ctr)
{
	SetCF(0!=(value&1));
	if(1<ctr)
	{
		value>>=ctr;
	}
	else if(1==ctr)
	{
		SetOverflowFlag(false);
		value>>=1;
	}
}
void i486DX::ShrWord(unsigned int &value,unsigned int ctr)
{
	SetCF(0!=(value&1));
	if(1<ctr)
	{
		value&=0xffff;
		value>>=ctr;
	}
	else if(1==ctr)
	{
		SetOverflowFlag(false);
		value&=0xffff;
		value>>=1;
	}
}
void i486DX::ShrByte(unsigned int &value,unsigned int ctr)
{
	SetCF(0!=(value&1));
	if(1<ctr)
	{
		value&=0xff;
		value>>=ctr;
	}
	else if(1==ctr)
	{
		SetOverflowFlag(false);
		value&=0xff;
		value>>=1;
	}
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
				seg=state.CS();
				break;
			case SEG_OVERRIDE_SS:
				seg=state.SS();
				break;
			case SEG_OVERRIDE_DS:
				seg=state.DS();
				break;
			case SEG_OVERRIDE_ES:
				seg=state.ES();
				break;
			case SEG_OVERRIDE_FS:
				seg=state.FS();
				break;
			case SEG_OVERRIDE_GS:
				seg=state.GS();
				break;
			default:
				if(op.baseReg==REG_ESP || op.baseReg==REG_SP ||
				   op.baseReg==REG_EBP || op.baseReg==REG_BP)
				{
					seg=state.SS();
				}
				else
				{
					seg=state.DS();
				}
				break;
			}
			unsigned int offset=
			   GetRegisterValue(op.baseReg)+
			   GetRegisterValue(op.indexReg)*op.indexScaling+
			   op.offset;
			if(addressSize==16)
			{
				for(unsigned int i=0; i<value.numBytes; ++i)
				{
					value.byteData[i]=FetchByte(addressSize,seg,(offset+i)&65535,mem);
				}
			}
			else
			{
				for(unsigned int i=0; i<value.numBytes; ++i)
				{
					value.byteData[i]=FetchByte(addressSize,seg,offset+i,mem);
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
			value.byteData[0]=(state.EAX()&255);
			break;
		case REG_CL:
			value.numBytes=1;
			value.byteData[0]=(state.ECX()&255);
			break;
		case REG_BL:
			value.numBytes=1;
			value.byteData[0]=(state.EBX()&255);
			break;
		case REG_DL:
			value.numBytes=1;
			value.byteData[0]=(state.EDX()&255);
			break;
		case REG_AH:
			value.numBytes=1;
			value.byteData[0]=((state.EAX()>>8)&255);
			break;
		case REG_CH:
			value.numBytes=1;
			value.byteData[0]=((state.ECX()>>8)&255);
			break;
		case REG_BH:
			value.numBytes=1;
			value.byteData[0]=((state.EBX()>>8)&255);
			break;
		case REG_DH:
			value.numBytes=1;
			value.byteData[0]=((state.EDX()>>8)&255);
			break;

		case REG_AX:
			value.numBytes=2;
			value.byteData[0]=(state.EAX()&255);
			value.byteData[1]=((state.EAX()>>8)&255);
			break;
		case REG_CX:
			value.numBytes=2;
			value.byteData[0]=(state.ECX()&255);
			value.byteData[1]=((state.ECX()>>8)&255);
			break;
		case REG_DX:
			value.numBytes=2;
			value.byteData[0]=(state.EDX()&255);
			value.byteData[1]=((state.EDX()>>8)&255);
			break;
		case REG_BX:
			value.numBytes=2;
			value.byteData[0]=(state.EBX()&255);
			value.byteData[1]=((state.EBX()>>8)&255);
			break;
		case REG_SP:
			value.numBytes=2;
			value.byteData[0]=(state.ESP()&255);
			value.byteData[1]=((state.ESP()>>8)&255);
			break;
		case REG_BP:
			value.numBytes=2;
			value.byteData[0]=(state.EBP()&255);
			value.byteData[1]=((state.EBP()>>8)&255);
			break;
		case REG_SI:
			value.numBytes=2;
			value.byteData[0]=(state.ESI()&255);
			value.byteData[1]=((state.ESI()>>8)&255);
			break;
		case REG_DI:
			value.numBytes=2;
			value.byteData[0]=(state.EDI()&255);
			value.byteData[1]=((state.EDI()>>8)&255);
			break;

		case REG_EAX:
			value.numBytes=4;
			value.byteData[0]=(state.EAX()&255);
			value.byteData[1]=((state.EAX()>>8)&255);
			value.byteData[2]=((state.EAX()>>16)&255);
			value.byteData[3]=((state.EAX()>>24)&255);
			break;
		case REG_ECX:
			value.numBytes=4;
			value.byteData[0]=(state.ECX()&255);
			value.byteData[1]=((state.ECX()>>8)&255);
			value.byteData[2]=((state.ECX()>>16)&255);
			value.byteData[3]=((state.ECX()>>24)&255);
			break;
		case REG_EDX:
			value.numBytes=4;
			value.byteData[0]=(state.EDX()&255);
			value.byteData[1]=((state.EDX()>>8)&255);
			value.byteData[2]=((state.EDX()>>16)&255);
			value.byteData[3]=((state.EDX()>>24)&255);
			break;
		case REG_EBX:
			value.numBytes=4;
			value.byteData[0]=(state.EBX()&255);
			value.byteData[1]=((state.EBX()>>8)&255);
			value.byteData[2]=((state.EBX()>>16)&255);
			value.byteData[3]=((state.EBX()>>24)&255);
			break;
		case REG_ESP:
			value.numBytes=4;
			value.byteData[0]=(state.ESP()&255);
			value.byteData[1]=((state.ESP()>>8)&255);
			value.byteData[2]=((state.ESP()>>16)&255);
			value.byteData[3]=((state.ESP()>>24)&255);
			break;
		case REG_EBP:
			value.numBytes=4;
			value.byteData[0]=(state.EBP()&255);
			value.byteData[1]=((state.EBP()>>8)&255);
			value.byteData[2]=((state.EBP()>>16)&255);
			value.byteData[3]=((state.EBP()>>24)&255);
			break;
		case REG_ESI:
			value.numBytes=4;
			value.byteData[0]=(state.ESI()&255);
			value.byteData[1]=((state.ESI()>>8)&255);
			value.byteData[2]=((state.ESI()>>16)&255);
			value.byteData[3]=((state.ESI()>>24)&255);
			break;
		case REG_EDI:
			value.numBytes=4;
			value.byteData[0]=(state.EDI()&255);
			value.byteData[1]=((state.EDI()>>8)&255);
			value.byteData[2]=((state.EDI()>>16)&255);
			value.byteData[3]=((state.EDI()>>24)&255);
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
			value.byteData[0]=(state.ES().value&255);
			value.byteData[1]=((state.ES().value>>8)&255);
			break;
		case REG_CS:
			value.numBytes=2;
			value.byteData[0]=(state.CS().value&255);
			value.byteData[1]=((state.CS().value>>8)&255);
			break;
		case REG_SS:
			value.numBytes=2;
			value.byteData[0]=(state.SS().value&255);
			value.byteData[1]=((state.SS().value>>8)&255);
			break;
		case REG_DS:
			value.numBytes=2;
			value.byteData[0]=(state.DS().value&255);
			value.byteData[1]=((state.DS().value>>8)&255);
			break;
		case REG_FS:
			value.numBytes=2;
			value.byteData[0]=(state.FS().value&255);
			value.byteData[1]=((state.FS().value>>8)&255);
			break;
		case REG_GS:
			value.numBytes=2;
			value.byteData[0]=(state.GS().value&255);
			value.byteData[1]=((state.GS().value>>8)&255);
			break;

		case REG_GDT:
			Abort("i486DX::EvaluateOperand, Check GDT Byte Order");
			value.numBytes=6;
			value.byteData[0]=(state.GDTR.linearBaseAddr&255);
			value.byteData[1]=((state.GDTR.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.GDTR.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.GDTR.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.GDTR.limit&255);
			value.byteData[5]=((state.GDTR.limit>>8)&255);
			break;
		case REG_LDT:
			Abort("i486DX::EvaluateOperand, Check LDT Byte Order");
			value.numBytes=6;
			value.byteData[0]=(state.LDTR.linearBaseAddr&255);
			value.byteData[1]=((state.LDTR.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.LDTR.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.LDTR.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.LDTR.limit&255);
			value.byteData[5]=((state.LDTR.limit>>8)&255);
			break;
		case REG_TR0:
			Abort("i486DX::EvaluateOperand, Check TR0 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR[0].linearBaseAddr&255);
			value.byteData[1]=((state.TR[0].linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR[0].linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR[0].linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR[0].limit&255);
			value.byteData[5]=((state.TR[0].limit>>8)&255);
			value.byteData[6]=(state.TR[0].selector&255);
			value.byteData[7]=((state.TR[0].selector>>8)&255);
			value.byteData[8]=(state.TR[0].attrib&255);
			value.byteData[9]=((state.TR[0].attrib>>8)&255);
			break;
		case REG_TR1:
			Abort("i486DX::EvaluateOperand, Check TR1 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR[1].linearBaseAddr&255);
			value.byteData[1]=((state.TR[1].linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR[1].linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR[1].linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR[1].limit&255);
			value.byteData[5]=((state.TR[1].limit>>8)&255);
			value.byteData[6]=(state.TR[1].selector&255);
			value.byteData[7]=((state.TR[1].selector>>8)&255);
			value.byteData[8]=(state.TR[1].attrib&255);
			value.byteData[9]=((state.TR[1].attrib>>8)&255);
			break;
		case REG_TR2:
			Abort("i486DX::EvaluateOperand, Check TR2 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR[2].linearBaseAddr&255);
			value.byteData[1]=((state.TR[2].linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR[2].linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR[2].linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR[2].limit&255);
			value.byteData[5]=((state.TR[2].limit>>8)&255);
			value.byteData[6]=(state.TR[2].selector&255);
			value.byteData[7]=((state.TR[2].selector>>8)&255);
			value.byteData[8]=(state.TR[2].attrib&255);
			value.byteData[9]=((state.TR[2].attrib>>8)&255);
			break;
		case REG_TR3:
			Abort("i486DX::EvaluateOperand, Check TR3 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR[3].linearBaseAddr&255);
			value.byteData[1]=((state.TR[3].linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR[3].linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR[3].linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR[3].limit&255);
			value.byteData[5]=((state.TR[3].limit>>8)&255);
			value.byteData[6]=(state.TR[3].selector&255);
			value.byteData[7]=((state.TR[3].selector>>8)&255);
			value.byteData[8]=(state.TR[3].attrib&255);
			value.byteData[9]=((state.TR[3].attrib>>8)&255);
			break;
		case REG_TR4:
			Abort("i486DX::EvaluateOperand, Check TR4 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR[4].linearBaseAddr&255);
			value.byteData[1]=((state.TR[4].linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR[4].linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR[4].linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR[4].limit&255);
			value.byteData[5]=((state.TR[4].limit>>8)&255);
			value.byteData[6]=(state.TR[4].selector&255);
			value.byteData[7]=((state.TR[4].selector>>8)&255);
			value.byteData[8]=(state.TR[4].attrib&255);
			value.byteData[9]=((state.TR[4].attrib>>8)&255);
			break;
		case REG_TR5:
			Abort("i486DX::EvaluateOperand, Check TR5 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR[5].linearBaseAddr&255);
			value.byteData[1]=((state.TR[5].linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR[5].linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR[5].linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR[5].limit&255);
			value.byteData[5]=((state.TR[5].limit>>8)&255);
			value.byteData[6]=(state.TR[5].selector&255);
			value.byteData[7]=((state.TR[5].selector>>8)&255);
			value.byteData[8]=(state.TR[5].attrib&255);
			value.byteData[9]=((state.TR[5].attrib>>8)&255);
			break;
		case REG_TR6:
			Abort("i486DX::EvaluateOperand, Check TR6 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR[6].linearBaseAddr&255);
			value.byteData[1]=((state.TR[6].linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR[6].linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR[6].linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR[6].limit&255);
			value.byteData[5]=((state.TR[6].limit>>8)&255);
			value.byteData[6]=(state.TR[6].selector&255);
			value.byteData[7]=((state.TR[6].selector>>8)&255);
			value.byteData[8]=(state.TR[6].attrib&255);
			value.byteData[9]=((state.TR[6].attrib>>8)&255);
			break;
		case REG_TR7:
			Abort("i486DX::EvaluateOperand, Check TR7 Byte Order");
			value.numBytes=10;
			value.byteData[0]=(state.TR[7].linearBaseAddr&255);
			value.byteData[1]=((state.TR[7].linearBaseAddr>>8)&255);
			value.byteData[2]=((state.TR[7].linearBaseAddr>>16)&255);
			value.byteData[3]=((state.TR[7].linearBaseAddr>>24)&255);
			value.byteData[4]=(state.TR[7].limit&255);
			value.byteData[5]=((state.TR[7].limit>>8)&255);
			value.byteData[6]=(state.TR[7].selector&255);
			value.byteData[7]=((state.TR[7].selector>>8)&255);
			value.byteData[8]=(state.TR[7].attrib&255);
			value.byteData[9]=((state.TR[7].attrib>>8)&255);
			break;
		case REG_IDTR:
			Abort("i486DX::EvaluateOperand, Check IDTR Byte Order");
			value.numBytes=6;
			value.byteData[0]=(state.IDTR.linearBaseAddr&255);
			value.byteData[1]=((state.IDTR.linearBaseAddr>>8)&255);
			value.byteData[2]=((state.IDTR.linearBaseAddr>>16)&255);
			value.byteData[3]=((state.IDTR.linearBaseAddr>>24)&255);
			value.byteData[4]=(state.IDTR.limit&255);
			value.byteData[5]=((state.IDTR.limit>>8)&255);
			break;
		case REG_CR0:
			value.numBytes=4;
			value.byteData[0]=(state.CR[0]&255);
			value.byteData[1]=((state.CR[0]>>8)&255);
			value.byteData[2]=((state.CR[0]>>16)&255);
			value.byteData[3]=((state.CR[0]>>24)&255);
			break;
		case REG_CR1:
			value.numBytes=4;
			value.byteData[0]=(state.CR[1]&255);
			value.byteData[1]=((state.CR[1]>>8)&255);
			value.byteData[2]=((state.CR[1]>>16)&255);
			value.byteData[3]=((state.CR[1]>>24)&255);
			break;
		case REG_CR2:
			value.numBytes=4;
			value.byteData[0]=(state.CR[2]&255);
			value.byteData[1]=((state.CR[2]>>8)&255);
			value.byteData[2]=((state.CR[2]>>16)&255);
			value.byteData[3]=((state.CR[2]>>24)&255);
			break;
		case REG_CR3:
			value.numBytes=4;
			value.byteData[0]=(state.CR[3]&255);
			value.byteData[1]=((state.CR[3]>>8)&255);
			value.byteData[2]=((state.CR[3]>>16)&255);
			value.byteData[3]=((state.CR[3]>>24)&255);
			break;
		case REG_DR0:
			value.numBytes=4;
			value.byteData[0]=(state.DR[0]&255);
			value.byteData[1]=((state.DR[0]>>8)&255);
			value.byteData[2]=((state.DR[0]>>16)&255);
			value.byteData[3]=((state.DR[0]>>24)&255);
			break;
		case REG_DR1:
			value.numBytes=4;
			value.byteData[0]=(state.DR[1]&255);
			value.byteData[1]=((state.DR[1]>>8)&255);
			value.byteData[2]=((state.DR[1]>>16)&255);
			value.byteData[3]=((state.DR[1]>>24)&255);
			break;
		case REG_DR2:
			value.numBytes=4;
			value.byteData[0]=(state.DR[2]&255);
			value.byteData[1]=((state.DR[2]>>8)&255);
			value.byteData[2]=((state.DR[2]>>16)&255);
			value.byteData[3]=((state.DR[2]>>24)&255);
			break;
		case REG_DR3:
			value.numBytes=4;
			value.byteData[0]=(state.DR[3]&255);
			value.byteData[1]=((state.DR[3]>>8)&255);
			value.byteData[2]=((state.DR[3]>>16)&255);
			value.byteData[3]=((state.DR[3]>>24)&255);
			break;
		case REG_DR4:
			value.numBytes=4;
			value.byteData[0]=(state.DR[4]&255);
			value.byteData[1]=((state.DR[4]>>8)&255);
			value.byteData[2]=((state.DR[4]>>16)&255);
			value.byteData[3]=((state.DR[4]>>24)&255);
			break;
		case REG_DR5:
			value.numBytes=4;
			value.byteData[0]=(state.DR[5]&255);
			value.byteData[1]=((state.DR[5]>>8)&255);
			value.byteData[2]=((state.DR[5]>>16)&255);
			value.byteData[3]=((state.DR[5]>>24)&255);
			break;
		case REG_DR6:
			value.numBytes=4;
			value.byteData[0]=(state.DR[6]&255);
			value.byteData[1]=((state.DR[6]>>8)&255);
			value.byteData[2]=((state.DR[6]>>16)&255);
			value.byteData[3]=((state.DR[6]>>24)&255);
			break;
		case REG_DR7:
			value.numBytes=4;
			value.byteData[0]=(state.DR[7]&255);
			value.byteData[1]=((state.DR[7]>>8)&255);
			value.byteData[2]=((state.DR[7]>>16)&255);
			value.byteData[3]=((state.DR[7]>>24)&255);
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
				seg=state.CS();
				break;
			case SEG_OVERRIDE_SS:
				seg=state.SS();
				break;
			case SEG_OVERRIDE_DS:
				seg=state.DS();
				break;
			case SEG_OVERRIDE_ES:
				seg=state.ES();
				break;
			case SEG_OVERRIDE_FS:
				seg=state.FS();
				break;
			case SEG_OVERRIDE_GS:
				seg=state.GS();
				break;
			default:
				if(dst.baseReg==REG_ESP || dst.baseReg==REG_SP ||
				   dst.baseReg==REG_EBP || dst.baseReg==REG_BP)
				{
					seg=state.SS();
				}
				else
				{
					seg=state.DS();
				}
				break;
			}
			unsigned int offset=
			   GetRegisterValue(dst.baseReg)+
			   GetRegisterValue(dst.indexReg)*dst.indexScaling+
			   dst.offset;
			if(addressSize==16)
			{
				for(unsigned int i=0; i<value.numBytes; ++i)
				{
					StoreByte(mem,addressSize,seg,(offset+i)&65535,value.byteData[i]);
				}
			}
			else
			{
				for(unsigned int i=0; i<value.numBytes; ++i)
				{
					StoreByte(mem,addressSize,seg,offset+i,value.byteData[i]);
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
			state.EAX()&=0xffffff00;
			state.EAX()|=value.byteData[0];
			break;
		case REG_CL:
			state.ECX()&=0xffffff00;
			state.ECX()|=value.byteData[0];
			break;
		case REG_BL:
			state.EBX()&=0xffffff00;
			state.EBX()|=value.byteData[0];
			break;
		case REG_DL:
			state.EDX()&=0xffffff00;
			state.EDX()|=value.byteData[0];
			break;
		case REG_AH:
			state.EAX()&=0xffff00ff;
			state.EAX()|=(((unsigned int)value.byteData[0])<<8);
			break;
		case REG_CH:
			state.ECX()&=0xffff00ff;
			state.ECX()|=(((unsigned int)value.byteData[0])<<8);
			break;
		case REG_BH:
			state.EBX()&=0xffff00ff;
			state.EBX()|=(((unsigned int)value.byteData[0])<<8);
			break;
		case REG_DH:
			state.EDX()&=0xffff00ff;
			state.EDX()|=(((unsigned int)value.byteData[0])<<8);
			break;

		case REG_AX:
			state.EAX()&=0xffff0000;
			state.EAX()|=cpputil::GetWord(value.byteData);
			break;
		case REG_CX:
			state.ECX()&=0xffff0000;
			state.ECX()|=cpputil::GetWord(value.byteData);
			break;
		case REG_DX:
			state.EDX()&=0xffff0000;
			state.EDX()|=cpputil::GetWord(value.byteData);
			break;
		case REG_BX:
			state.EBX()&=0xffff0000;
			state.EBX()|=cpputil::GetWord(value.byteData);
			break;
		case REG_SP:
			state.ESP()&=0xffff0000;
			state.ESP()|=cpputil::GetWord(value.byteData);
			break;
		case REG_BP:
			state.EBP()&=0xffff0000;
			state.EBP()|=cpputil::GetWord(value.byteData);
			break;
		case REG_SI:
			state.ESI()&=0xffff0000;
			state.ESI()|=cpputil::GetWord(value.byteData);
			break;
		case REG_DI:
			state.EDI()&=0xffff0000;
			state.EDI()|=cpputil::GetWord(value.byteData);
			break;

		case REG_EAX:
			state.EAX()=cpputil::GetDword(value.byteData);
			break;
		case REG_ECX:
			state.ECX()=cpputil::GetDword(value.byteData);
			break;
		case REG_EDX:
			state.EDX()=cpputil::GetDword(value.byteData);
			break;
		case REG_EBX:
			state.EBX()=cpputil::GetDword(value.byteData);
			break;
		case REG_ESP:
			state.ESP()=cpputil::GetDword(value.byteData);
			break;
		case REG_EBP:
			state.EBP()=cpputil::GetDword(value.byteData);
			break;
		case REG_ESI:
			state.ESI()=cpputil::GetDword(value.byteData);
			break;
		case REG_EDI:
			state.EDI()=cpputil::GetDword(value.byteData);
			break;

		case REG_EIP:
			state.EIP=cpputil::GetDword(value.byteData);
			break;
		case REG_EFLAGS:
			state.EFLAGS=cpputil::GetDword(value.byteData);
			break;

		case REG_ES:
			LoadSegmentRegister(state.ES(),cpputil::GetWord(value.byteData),mem);
			break;
		case REG_CS:
			LoadSegmentRegister(state.CS(),cpputil::GetWord(value.byteData),mem);
			break;
		case REG_SS:
			LoadSegmentRegister(state.SS(),cpputil::GetWord(value.byteData),mem);
			break;
		case REG_DS:
			LoadSegmentRegister(state.DS(),cpputil::GetWord(value.byteData),mem);
			break;
		case REG_FS:
			LoadSegmentRegister(state.FS(),cpputil::GetWord(value.byteData),mem);
			break;
		case REG_GS:
			LoadSegmentRegister(state.GS(),cpputil::GetWord(value.byteData),mem);
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
			state.CR[0]=cpputil::GetDword(value.byteData);
			break;
		case REG_CR1:
			state.CR[1]=cpputil::GetDword(value.byteData);
			break;
		case REG_CR2:
			state.CR[2]=cpputil::GetDword(value.byteData);
			break;
		case REG_CR3:
			state.CR[3]=cpputil::GetDword(value.byteData);
			break;
		case REG_DR0:
			state.DR[0]=cpputil::GetDword(value.byteData);
			break;
		case REG_DR1:
			state.DR[1]=cpputil::GetDword(value.byteData);
			break;
		case REG_DR2:
			state.DR[2]=cpputil::GetDword(value.byteData);
			break;
		case REG_DR3:
			state.DR[3]=cpputil::GetDword(value.byteData);
			break;
		case REG_DR4:
			state.DR[4]=cpputil::GetDword(value.byteData);
			break;
		case REG_DR5:
			state.DR[5]=cpputil::GetDword(value.byteData);
			break;
		case REG_DR6:
			state.DR[6]=cpputil::GetDword(value.byteData);
			break;
		case REG_DR7:
			state.DR[7]=cpputil::GetDword(value.byteData);
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

bool i486DX::REPCheck(unsigned int &clocksPassed,unsigned int instPrefix,unsigned int addressSize)
{
	if(INST_PREFIX_REP==instPrefix || INST_PREFIX_REPNE==instPrefix)
	{
		auto counter=GetCXorECX(addressSize);
		if(0==counter)
		{
			clocksPassed=5;
			return false;
		}
		--counter;
		SetCXorECX(addressSize,counter);
		clocksPassed=7;
	}
	return true;
}

bool i486DX::REPEorNECheck(unsigned int &clocksForRep,unsigned int instPrefix,unsigned int addressSize)
{
	if(INST_PREFIX_REPE==instPrefix)
	{
		return GetZF();
	}
	else if(INST_PREFIX_REPNE==instPrefix)
	{
		return (true!=GetZF());
	}
	return false;
}

i486DX::CallStack i486DX::MakeCallStack(
	    bool isInterrupt,unsigned short INTNum,unsigned short AX,
	    unsigned int fromCS,unsigned int fromEIP,unsigned int callOpCodeLength,
	    unsigned int procCS,unsigned int procEIP)
{
	CallStack stk;
	if(true==isInterrupt)
	{
		stk.INTNum=INTNum;
		stk.AX=AX;
	}
	else
	{
		stk.INTNum=0xffff;
		stk.AX=0xffff;
	}
	stk.fromCS=fromCS;
	stk.fromEIP=fromEIP;
	stk.callOpCodeLength=callOpCodeLength;
	stk.procCS=procCS;
	stk.procEIP=procEIP;
	return stk;
}
void i486DX::PushCallStack(
	    bool isInterrupt,unsigned short INTNum,unsigned short AX,
	    unsigned int fromCS,unsigned int fromEIP,unsigned int callOpCodeLength,
	    unsigned int procCS,unsigned int procEIP)
{
	callStack.push_back(MakeCallStack(isInterrupt,INTNum,AX,fromCS,fromEIP,callOpCodeLength,procCS,procEIP));
}
void i486DX::PopCallStack(unsigned int CS,unsigned int EIP)
{
	if(true!=callStack.empty())
	{
		int nPop=1;
		bool match=false;
		for(auto iter=callStack.rbegin(); iter!=callStack.rend(); ++iter)
		{
			if(CS==iter->fromCS && EIP==iter->fromEIP+iter->callOpCodeLength)
			{
				match=true;
				break;
			}
			++nPop;
		}

		if(true!=match) // Prob: Jump by RET?
		{
			nPop=0;
		}
		while(0<nPop)
		{
			callStack.pop_back();
			--nPop;
		}
	}
}
void i486DX::AttachDebugger(i486Debugger *debugger)
{
	this->debuggerPtr=debugger;
}
void i486DX::DetachDebugger(void)
{
	debuggerPtr=nullptr;
}

/*! Fetch a byte for debugger.  It won't change exception status.
*/
unsigned int i486DX::DebugFetchByte(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	if(true==AddressingMode16Bit(addressSize))
	{
		offset&=0xffff;
	}
	auto addr=seg.baseLinearAddr+offset;
	if(true==PagingEnabled())
	{
		addr=LinearAddressToPhysicalAddress(addr,mem);
	}
	auto returnValue=mem.FetchByte(addr);
	return returnValue;
}

/*! Fetch a dword.  It won't change exception status.
*/
unsigned int i486DX::DebugFetchWord(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	if(true==AddressingMode16Bit(addressSize))
	{
		offset&=0xffff;
	}
	auto addr=seg.baseLinearAddr+offset;
	if(true==PagingEnabled())
	{
		addr=LinearAddressToPhysicalAddress(addr,mem);
		if(0xFFC<(addr&0xfff)) // May hit the page boundary
		{
			return FetchByte(addressSize,seg,offset,mem)|(FetchByte(addressSize,seg,offset+1,mem)<<8);
		}
	}
	auto returnValue=mem.FetchWord(addr);
	return returnValue;
}

/*! Fetch a dword for debugging.  It won't change exception status.
*/
unsigned int i486DX::DebugFetchDword(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	if(true==AddressingMode16Bit(addressSize))
	{
		offset&=0xffff;
	}
	auto addr=seg.baseLinearAddr+offset;
	if(true==PagingEnabled())
	{
		addr=LinearAddressToPhysicalAddress(addr,mem);
		if(0xFF8<(addr&0xfff)) // May hit the page boundary
		{
			auto returnValue=
			     FetchByte(addressSize,seg,offset,mem)
			   |(FetchByte(addressSize,seg,offset+1,mem)<<8)
			   |(FetchByte(addressSize,seg,offset+2,mem)<<16)
			   |(FetchByte(addressSize,seg,offset+3,mem)<<24);
			return returnValue;
		}
	}
	auto returnValue=mem.FetchDword(addr);
	return returnValue;
}
/*! Fetch a byte, word, or dword for debugging.
    It won't change the exception status.
    Function name is left as FetchWordOrDword temporarily for the time being.
    Will be unified to FetchByteWordOrDword in the future.
*/
unsigned int i486DX::DebugFetchWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	switch(operandSize)
	{
	case 8:
		return DebugFetchByte(addressSize,seg,offset,mem);
	case 16:
		return DebugFetchWord(addressSize,seg,offset,mem);
	default:
	case 32:
		return DebugFetchDword(addressSize,seg,offset,mem);
	}
}
inline unsigned int i486DX::DebugFetchByteWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	return DebugFetchWordOrDword(operandSize,addressSize,seg,offset,mem);
}

/*! Fetch a byte by linear address for debugging.  It won't change exception status.
*/
inline unsigned int i486DX::DebugFetchByteByLinearAddress(const Memory &mem,unsigned int linearAddr) const
{
	if(true==PagingEnabled())
	{
		linearAddr=LinearAddressToPhysicalAddress(linearAddr,mem);
	}
	auto returnValue=mem.FetchByte(linearAddr);
	return returnValue;
}

