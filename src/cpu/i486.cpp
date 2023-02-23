/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <algorithm>
#include <iostream>
#include <limits>

#include "cpputil.h"
#include "i486.h"


i486DX::State::State()
{
#ifdef YS_LITTLE_ENDIAN
	reg8Ptr[0]=(uint8_t *)&NULL_and_reg32[REG_EAX];
	reg8Ptr[1]=(uint8_t *)&NULL_and_reg32[REG_ECX];
	reg8Ptr[2]=(uint8_t *)&NULL_and_reg32[REG_EDX];
	reg8Ptr[3]=(uint8_t *)&NULL_and_reg32[REG_EBX];
	reg8Ptr[4]=((uint8_t *)&NULL_and_reg32[REG_EAX])+1;
	reg8Ptr[5]=((uint8_t *)&NULL_and_reg32[REG_ECX])+1;
	reg8Ptr[6]=((uint8_t *)&NULL_and_reg32[REG_EDX])+1;
	reg8Ptr[7]=((uint8_t *)&NULL_and_reg32[REG_EBX])+1;
#endif
}


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

	"EAX",
	"ECX",
	"EDX",
	"EBX",
	"ESP",
	"EBP",
	"ESI",
	"EDI",

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

	"IP",
	"EIP",
	"EFLAGS",

	"ES",
	"CS",
	"SS",
	"DS",
	"FS",
	"GS",
};

const bool i486DX::ParityTable[256]=
{
true, false,false,true, false,true, true, false,false,true, true, false,true, false,false,true,
false,true, true, false,true, false,false,true, true, false,false,true, false,true, true, false,
false,true, true, false,true, false,false,true, true, false,false,true, false,true, true, false,
true, false,false,true, false,true, true, false,false,true, true, false,true, false,false,true,
false,true, true, false,true, false,false,true, true, false,false,true, false,true, true, false,
true, false,false,true, false,true, true, false,false,true, true, false,true, false,false,true,
true, false,false,true, false,true, true, false,false,true, true, false,true, false,false,true,
false,true, true, false,true, false,false,true, true, false,false,true, false,true, true, false,
false,true, true, false,true, false,false,true, true, false,false,true, false,true, true, false,
true, false,false,true, false,true, true, false,false,true, true, false,true, false,false,true,
true, false,false,true, false,true, true, false,false,true, true, false,true, false,false,true,
false,true, true, false,true, false,false,true, true, false,false,true, false,true, true, false,
true, false,false,true, false,true, true, false,false,true, true, false,true, false,false,true,
false,true, true, false,true, false,false,true, true, false,false,true, false,true, true, false,
false,true, true, false,true, false,false,true, true, false,false,true, false,true, true, false,
true, false,false,true, false,true, true, false,false,true, true, false,true, false,false,true,
};

std::string i486DX::FarPointer::Format(void) const
{
	return cpputil::Uitox(SEG)+":"+cpputil::Uitox(OFFSET);
}

void i486DX::FarPointer::MakeFromString(const std::string &str)
{
	Nullify();
	for(unsigned int i=0; i<str.size(); ++i)
	{
		if(':'==str[i])
		{
			auto offsetPtr=str.data()+i+1;
			this->OFFSET=cpputil::Xtoi(offsetPtr);

			auto segPart=str;
			segPart.resize(i);
			cpputil::Capitalize(segPart);
			if("CS"==segPart)
			{
				this->SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_CS;
			}
			else if("SS"==segPart)
			{
				this->SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_SS;
			}
			else if("DS"==segPart)
			{
				this->SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_DS;
			}
			else if("ES"==segPart)
			{
				this->SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_ES;
			}
			else if("FS"==segPart)
			{
				this->SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_FS;
			}
			else if("GS"==segPart)
			{
				this->SEG=i486DX::FarPointer::SEG_REGISTER|i486DX::REG_GS;
			}
			else if("PHYS"==segPart || "P"==segPart)
			{
				this->SEG=i486DX::FarPointer::PHYS_ADDR;
			}
			else if("LINE"==segPart || "L"==segPart)
			{
				this->SEG=i486DX::FarPointer::LINEAR_ADDR;
			}
			else if('R'==segPart[0] || 'r'==segPart[0])
			{
				this->SEG=i486DX::FarPointer::REAL_ADDR|cpputil::Xtoi(str.data()+1);
			}
			else
			{
				this->SEG=cpputil::Xtoi(str.data());
			}

			if('*'==*offsetPtr)
			{
				this->SEG&=0xFFFF;
				this->SEG|=SEG_WILDCARD;
			}

			return;
		}
	}

	this->SEG=i486DX::FarPointer::NO_SEG;
	this->OFFSET=cpputil::Xtoi(str.data());
}

i486DX::FarPointer i486DX::TranslateFarPointer(FarPointer ptr) const
{
	if(ptr.SEG==FarPointer::NO_SEG)
	{
		ptr.SEG=state.CS().value;
	}
	else if((ptr.SEG&0xFFFF0000)==FarPointer::SEG_REGISTER)
	{
		ptr.SEG=GetRegisterValue(ptr.SEG&0xFFFF);
	}
	return ptr;
}

void i486DX::FarPointer::LoadSegmentRegister(SegmentRegister &seg,i486DX &cpu,const Memory &mem) const
{
	if(SEG==NO_SEG)
	{
		seg=cpu.state.CS();
	}
	else if(0==(SEG&0xFFFF0000))
	{
		cpu.LoadSegmentRegister(seg,SEG&0xFFFF,mem);
	}
	else if((SEG&0xFFFF0000)==SEG_REGISTER)
	{
		seg=cpu.state.GetSegmentRegister(SEG&0xFFFF);
	}
	else if((SEG&0xFFFF0000)==LINEAR_ADDR)
	{
		seg.value=0;
		seg.baseLinearAddr=0;
		seg.operandSize=32;
		seg.addressSize=32;
		seg.limit=0xFFFFFFFF;
	}
	else if((SEG&0xFFFF0000)==REAL_ADDR)
	{
		seg.value=SEG&0xFFFF;
		seg.baseLinearAddr=seg.value*0x10;
		seg.operandSize=16;
		seg.addressSize=16;
		seg.limit=0xFFFF;
	}
}

////////////////////////////////////////////////////////////


i486DX::i486DX(VMBase *vmPtr) : CPU(vmPtr)
{
	state.NULL_and_reg32[ 0]=0;

	stackAddressSizePointer[0]=&sixteen;
	stackAddressSizePointer[1]=&state.sreg[REG_SS-REG_SEGMENT_REG_BASE].addressSize;

	CSOperandSizePointer[0]=&sixteen;
	CSOperandSizePointer[1]=&state.sreg[REG_CS-REG_SEGMENT_REG_BASE].operandSize;
	CSAddressSizePointer[0]=&sixteen;
	CSAddressSizePointer[1]=&state.sreg[REG_CS-REG_SEGMENT_REG_BASE].addressSize;

	MakeOpCodeRenumberTable();

	Reset();
	enableCallStack=false;
	debuggerPtr=nullptr;

	for(auto &sregPtr : baseRegisterToDefaultSegment)
	{
		sregPtr=&state.sreg[REG_DS-REG_SEGMENT_REG_BASE];
	}
	baseRegisterToDefaultSegment[REG_SP]=&state.sreg[REG_SS-REG_SEGMENT_REG_BASE];
	baseRegisterToDefaultSegment[REG_BP]=&state.sreg[REG_SS-REG_SEGMENT_REG_BASE];
	baseRegisterToDefaultSegment[REG_ESP]=&state.sreg[REG_SS-REG_SEGMENT_REG_BASE];
	baseRegisterToDefaultSegment[REG_EBP]=&state.sreg[REG_SS-REG_SEGMENT_REG_BASE];

	for(int i=0; i<sizeof(state.sreg)/sizeof(state.sreg[0]); ++i)
	{
		sregIndexToSregPtrTable[i]=&state.sreg[i];
	}
	sregIndexToSregPtrTable[NUM_SEGMENT_REGISTERS]=nullptr;

	for(auto &i : segPrefixToSregIndex)
	{
		i=NUM_SEGMENT_REGISTERS;
	}
	segPrefixToSregIndex[SEG_OVERRIDE_CS]=REG_CS-REG_SEGMENT_REG_BASE;
	segPrefixToSregIndex[SEG_OVERRIDE_SS]=REG_SS-REG_SEGMENT_REG_BASE;
	segPrefixToSregIndex[SEG_OVERRIDE_DS]=REG_DS-REG_SEGMENT_REG_BASE;
	segPrefixToSregIndex[SEG_OVERRIDE_ES]=REG_ES-REG_SEGMENT_REG_BASE;
	segPrefixToSregIndex[SEG_OVERRIDE_FS]=REG_FS-REG_SEGMENT_REG_BASE;
	segPrefixToSregIndex[SEG_OVERRIDE_GS]=REG_GS-REG_SEGMENT_REG_BASE;

	ClearPageTableCache();
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

	state.CS().limit=0xffff;
	state.SS().limit=0xffff;
	state.DS().limit=0xffff;
	state.ES().limit=0xffff;
	state.FS().limit=0xffff;
	state.GS().limit=0xffff;

	state.GDTR.linearBaseAddr=RESET_GDTRBASE;
	state.GDTR.limit=RESET_GDTRLIMIT;

	state.IDTR.linearBaseAddr=RESET_IDTRBASE;
	state.IDTR.limit=RESET_IDTRLIMIT;

	state.LDTR.linearBaseAddr=RESET_LDTRBASE;
	state.LDTR.limit=RESET_LDTRLIMIT;
	state.LDTR.selector=RESET_LDTRSELECTOR;

	state.TR.baseLinearAddr=RESET_TRBASE;
	state.TR.limit=RESET_TRLIMIT;
	state.TR.value=RESET_TRSELECTOR;
	state.TR.attrib=RESET_TRATTRIB;

	state.DR[7]=RESET_DR7;

	state.EAX()=RESET_EAX;
	SetDX(RESET_DX);
	SetCR(0,RESET_CR0);
	if(true!=state.fpuState.enabled)
	{
		auto cr0=state.GetCR(0);
		cr0&=(~CR0_MATH_PRESENT);
		SetCR(0,cr0);
	}
	// Better than leaving it uninitialized
	SetCR(1,0);
	SetCR(2,0);
	ClearPageTableCache();
	ClearDescriptorCache();

	for(auto &t : state.TEST)
	{
		t=0;
	}

	state.halt=false;
	state.holdIRQ=false;
	state.exception=false;
}

void i486DX::ClearPageTableCache(void)
{
	for(auto &c : state.pageTableCache)
	{
		c=0;
	}
	state.pageTableCacheValidCounter=1;
	for(auto &c : state.pageTableCacheValid)
	{
		c=0;
	}
}

void i486DX::InvalidatePageTableCache()
{
	++state.pageTableCacheValidCounter;
	if(state.pageTableCacheValidCounter==0xffffffff)
	{
		ClearPageTableCache();
	}
}

void i486DX::ClearDescriptorCache(void)
{
	state.descriptorCacheValidCounter=1;
	for(auto &c : state.descriptorCacheValid)
	{
		c=0;
	}
	for(auto &ptr : state.descriptorCache)
	{
		ptr=nullptr;
	}
}
void i486DX::InvalidateDescriptorCache(void)
{
	++state.descriptorCacheValidCounter;
	if(0xffffffff==state.descriptorCacheValidCounter)
	{
		ClearDescriptorCache();
	}
}

void i486DX::HandleException(bool wasReadOp,Memory &mem,unsigned int numInstBytesForCallStack)
{
	/* Should add a flag.
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->ExternalBreak("Exception!");
	} */

	// Only some of the exceptions push error code onto the stack.
	// See Section 9.9 of i486 Programmer's Reference Manual for the information.
	switch(state.exceptionType)
	{
	case EXCEPTION_PF:
		Interrupt(INT_PAGE_FAULT,mem,0,numInstBytesForCallStack,false);
		Push(mem,32,(wasReadOp ? 0 : 2));
		SetCR(2,state.exceptionLinearAddr);
		break;
	case EXCEPTION_GP:
		Interrupt(INT_GENERAL_PROTECTION,mem,0,numInstBytesForCallStack,false);
		if(true!=IsInRealMode()) // As HIMEM.SYS's expectation.
		{
			Push(mem,32,state.exceptionCode);
		}
		break;
	case EXCEPTION_ND:
		Interrupt(INT_SEGMENT_NOT_PRESENT,mem,0,numInstBytesForCallStack,false);
		if(true!=IsInRealMode())
		{
			Push(mem,32,state.exceptionCode);
		}
		break;
	case EXCEPTION_UD:
		Interrupt(INT_INVALID_OPCODE,mem,0,numInstBytesForCallStack,false);
		break;
	case EXCEPTION_SS:
		Abort("SS handling not implemented yet.");
		break;
	default:
		Abort("Undefined exception.");
		break;
	}
	state.exception=false;
}

std::vector <std::string> i486DX::GetStateText(void) const
{
	std::vector <std::string> text;

	text.push_back(
	     "CS:EIP="
	    +cpputil::Ustox(state.CS().value)+":"+cpputil::Uitox(state.EIP)
	    +"  LINEAR:"+cpputil::Uitox(state.CS().baseLinearAddr+state.EIP)
	    +"  EFLAGS="+cpputil::Uitox(state.EFLAGS)
	    +"  CPL="+cpputil::Ubtox(state.CS().DPL));

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
	     "CR0="+cpputil::Uitox(state.GetCR(0))
	    +"  CR1="+cpputil::Uitox(state.GetCR(1))
	    +"  CR2="+cpputil::Uitox(state.GetCR(2))
	    +"  CR3="+cpputil::Uitox(state.GetCR(3))
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

std::vector <std::string> i486DX::GetSegRegText(void) const
{
	std::vector <std::string> text;

	text.push_back(
	     "CS="+cpputil::Ustox(state.CS().value)
	    +"(LIN:"+cpputil::Uitox(state.CS().baseLinearAddr)
	    +" LMT:"+cpputil::Uitox(state.CS().limit)
	    +")"
	    +"  "
	     "DS="+cpputil::Ustox(state.DS().value)
	    +"(LIN:"+cpputil::Uitox(state.DS().baseLinearAddr)
	    +" LMT:"+cpputil::Uitox(state.DS().limit)
	    +")"
	    +"  "
	    );

	text.push_back(
	     "ES="+cpputil::Ustox(state.ES().value)
	    +"(LIN:"+cpputil::Uitox(state.ES().baseLinearAddr)
	    +" LMT:"+cpputil::Uitox(state.ES().limit)
	    +")"
	    +"  "
	     "FS="+cpputil::Ustox(state.FS().value)
	    +"(LIN:"+cpputil::Uitox(state.FS().baseLinearAddr)
	    +" LMT:"+cpputil::Uitox(state.FS().limit)
	    +")"
	    +"  "
	    );

	text.push_back(
	     "GS="+cpputil::Ustox(state.GS().value)
	    +"(LIN:"+cpputil::Uitox(state.GS().baseLinearAddr)
	    +" LMT:"+cpputil::Uitox(state.GS().limit)
	    +")"
	    +"  "
	    +"SS="+cpputil::Ustox(state.SS().value)
	    +"(LIN:"+cpputil::Uitox(state.SS().baseLinearAddr)
	    +" LMT:"+cpputil::Uitox(state.SS().limit)
	    +")"
	    +"  "
	    );

	return text;
}

std::vector <std::string> i486DX::GetGDTText(const Memory &mem) const
{
	std::vector <std::string> text;
	text.push_back("GDT  Limit=");
	text.back()+=cpputil::Ustox(state.GDTR.limit);
	text.back()+="  LinearBase=";
	text.back()+=cpputil::Uitox(state.GDTR.linearBaseAddr);


	for(unsigned int selector=0; selector<state.GDTR.limit; selector+=8)
	{
		unsigned int DTLinearBaseAddr=state.GDTR.linearBaseAddr+selector;
		unsigned int excType,excCode;
		unsigned int DTPhysicalAddr=LinearAddressToPhysicalAddress(excType,excCode,DTLinearBaseAddr,mem);
		const unsigned char rawDesc[8]=
		{
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+1),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+2),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+3),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+4),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+5),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+6),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+7)
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

		text.push_back("");
		text.back()+=cpputil::Ustox(selector);
		text.back()+=":";
		text.back()+="LnBase=";
		text.back()+=cpputil::Uitox(segBase);
		text.back()+=" Lim=";
		text.back()+=cpputil::Uitox(segLimit);
		text.back()+=" OpSz=";
		text.back()+=cpputil::Ubtox(operandSize);
		text.back()+="H AdSz=";
		text.back()+=cpputil::Ubtox(addressSize);
		text.back()+="H ";
		text.back()+="P=";
		text.back()+=cpputil::Ubtox((rawDesc[5]>>7)&1);
		text.back()+=" DPL=";
		text.back()+=cpputil::Ubtox((rawDesc[5]>>5)&3);
		text.back()+=" Type=";
		text.back()+=cpputil::Ubtox(rawDesc[5]&31);
		text.back()+="H ";
		text.back()+="@ PHYS:";
		text.back()+=cpputil::Uitox(DTPhysicalAddr);
		text.back()+="H";
	}
	return text;
}

std::vector <std::string> i486DX::GetLDTText(const Memory &mem) const
{
	std::vector <std::string> text;
	text.push_back("LDT  Limit=");
	text.back()+=cpputil::Ustox(state.LDTR.limit);
	text.back()+="  LinearBase=";
	text.back()+=cpputil::Uitox(state.LDTR.linearBaseAddr);
	text.back()+="  Selector=";
	text.back()+=cpputil::Uitox(state.LDTR.selector);


	for(unsigned int selector=0; selector<state.LDTR.limit; selector+=8)
	{
		unsigned int DTLinearBaseAddr=state.LDTR.linearBaseAddr+selector;
		unsigned int excType,excCode;
		unsigned int DTPhysicalAddr=LinearAddressToPhysicalAddress(excType,excCode,DTLinearBaseAddr,mem);
		const unsigned char rawDesc[8]=
		{
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+1),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+2),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+3),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+4),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+5),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+6),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+7)
		};

		// Sample LDT from WRHIGH.ASM
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
		text.back()+=cpputil::Ustox(selector|4);
		text.back()+=":";
		text.back()+="LiBase=";
		text.back()+=cpputil::Uitox(segBase);
		text.back()+=" Lim=";
		text.back()+=cpputil::Uitox(segLimit);
		text.back()+=" OpSz=";
		text.back()+=cpputil::Ubtox(operandSize);
		text.back()+="H AdSz=";
		text.back()+=cpputil::Ubtox(addressSize);
		text.back()+="H ";
		text.back()+="P=";
		text.back()+=cpputil::Ubtox((rawDesc[5]>>7)&1);
		text.back()+=" DPL=";
		text.back()+=cpputil::Ubtox((rawDesc[5]>>5)&3);
		text.back()+=" Type=";
		text.back()+=cpputil::Ubtox(rawDesc[5]&15);
		text.back()+="H ";
		text.back()+="@ PHYS:";
		text.back()+=cpputil::Uitox(DTPhysicalAddr);
		text.back()+="H";
	}
	return text;
}

std::vector <std::string> i486DX::GetIDTText(const Memory &mem) const
{
	std::vector <std::string> text;
	std::string empty;

	text.push_back(empty);
	text.back()+="IDT at Linear Address="+cpputil::Uitox(state.IDTR.linearBaseAddr);
	text.back()+="  Limit="+cpputil::Uitox(state.IDTR.limit);
	for(unsigned int offset=0; offset<state.IDTR.limit && offset<0x800; offset+=8)
	{
		auto desc=DebugGetInterruptDescriptor(offset/8,mem);
		text.push_back(empty);
		text.back()=cpputil::Ubtox(offset/8);
		text.back()+=":";
		text.back()+="SEG=";
		text.back()+=cpputil::Ustox(desc.SEG);
		text.back()+=" OFFSET=";
		text.back()+=cpputil::Uitox(desc.OFFSET);

		auto P=(desc.flags>>15)&1;
		text.back()+=" P=";
		text.back()+=cpputil::Ubtox(P);

		auto DPL=(desc.flags>>13)&3;
		text.back()+=" DPL=";
		text.back()+=cpputil::Ubtox(DPL);

		auto type=(desc.flags>>8)&0x1F;
		text.back()+=" TYPE=";
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

std::vector <std::string> i486DX::GetTSSText(const Memory &mem) const
{
	std::vector <std::string> text;

	unsigned int addrSize=32;

	text.push_back("");
	text.back()+="TR="+cpputil::Ustox(state.TR.value)+" TR Linear Base Addr="+cpputil::Uitox(state.TR.baseLinearAddr);

	text.push_back("");
	text.back()+="LINK(OLD TSS SELECTOR)="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,0,mem));

	text.push_back("");
	text.back()+="SS0:ESP0="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,8,mem))+":"+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,4,mem));
	text.push_back("");
	text.back()+="SS1:ESP1="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,0x10,mem))+":"+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x0C,mem));
	text.push_back("");
	text.back()+="SS2:ESP2="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,0x18,mem))+":"+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x14,mem));
	text.push_back("");
	text.back()+="CS:EIP="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,0x4C,mem))+":"+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x20,mem));
	text.push_back("");
	text.back()+="EFLAGS="+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x24,mem));
	text.push_back("");
	text.back()+="EAX="+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x28,mem))+" ECX="+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x2C,mem));
	text.push_back("");
	text.back()+="EDX="+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x30,mem))+" EBX="+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x34,mem));
	text.push_back("");
	text.back()+="ESP="+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x38,mem))+" EBP="+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x3C,mem));
	text.push_back("");
	text.back()+="ESI="+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x40,mem))+" EDI="+cpputil::Uitox(DebugFetchDword(addrSize,state.TR,0x44,mem));
	text.push_back("");
	text.back()+="ES="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,0x48,mem))+" SS="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,0x50,mem));
	text.push_back("");
	text.back()+="DS="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,0x54,mem))+" FS="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,0x58,mem));
	text.push_back("");
	text.back()+="GS="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,0x5C,mem));
	text.push_back("");
	text.back()+="LDT="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,0x60,mem));
	text.push_back("");
	text.back()+="T="+cpputil::Ustox(DebugFetchDword(addrSize,state.TR,0x64,mem));
	text.back()+="  I/O MAP Base="+cpputil::Ustox(DebugFetchDword(addrSize,state.TR,0x66,mem));

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

void i486DX::PrintLDT(const Memory &mem) const
{
	for(auto &str : GetLDTText(mem))
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

void i486DX::PrintPageTranslation(const Memory &mem,uint32_t linearAddr) const
{
	auto pageIndex=(linearAddr>>LINEARADDR_TO_PAGE_SHIFT);

	std::cout << "LINE:" << cpputil::Uitox(linearAddr) << "H" << std::endl;

	auto pageInfo=state.pageTableCache[pageIndex];
	if(state.pageTableCacheValid[pageIndex]<state.pageTableCacheValidCounter)
	{
		std::cout << "Page Info Not Cached" << std::endl;
	}
	else
	{
		std::cout << "Cached Page Info:" << cpputil::Uitox(pageInfo) << "H" << std::endl;
		if(0!=(pageInfo&PAGEINFO_FLAG_PRESENT))
		{
			auto offset=(linearAddr&4095);
			auto physicalAddr=(pageInfo&0xFFFFF000)+offset;
			std::cout << "Cache PHYS:" << cpputil::Uitox(physicalAddr) << "H" << std::endl;
		}
		else
		{
			std::cout << "Cached Page Not Present" << std::endl;
		}
	}

	uint32_t pageDirectoryIndex=((linearAddr>>22)&1023);
	uint32_t pageTableIndex=((linearAddr>>12)&1023);

	auto pageDirectoryPtr=state.GetCR(3)&0xFFFFF000;
	auto pageTableInfo=mem.FetchDword(pageDirectoryPtr+(pageDirectoryIndex<<2));

	std::cout << "Page Directory Index  :" << cpputil::Uitox(pageDirectoryIndex) << "H" << std::endl;
	std::cout << "Page Directory Pointer:" << cpputil::Uitox(pageDirectoryPtr) << "H" << std::endl;
	std::cout << "Page Directory Info   :" << cpputil::Uitox(pageTableInfo) << "H" << std::endl;
	if(0==(pageTableInfo&1))
	{
		std::cout << "Page Table Not Present" << std::endl;
	}
	else
	{
		const uint32_t pageTablePtr=(pageTableInfo&0xFFFFF000);
		const uint32_t pagePtr=pageTablePtr+(pageTableIndex<<2);
		unsigned int pageInfo=mem.FetchDword(pagePtr);
		std::cout << "Page Table Index  :" << cpputil::Uitox(pageTableIndex) << "H" << std::endl;
		std::cout << "Page Table Pointer:" << cpputil::Uitox(pageTablePtr) << "H" << std::endl;
		std::cout << "Page Pointer      :" << cpputil::Uitox(pagePtr) << "H" << std::endl;
		std::cout << "Page Table Info   :" << cpputil::Uitox(pageInfo) << "H" << std::endl;
		if(0==(pageInfo&1))
		{
			std::cout << "Page Not Present" << std::endl;
		}
		else
		{
			auto offset=(linearAddr&4095);
			auto physicalAddr=(pageInfo&0xFFFFF000)+offset;
			std::cout << "PHYS:" << cpputil::Uitox(physicalAddr) << "H" << std::endl;
		}
	}
}

template <class CPUCLASS>
class i486DX::LoadSegmentRegisterTemplate
{
public:
	unsigned char rawDescBuf[8];
	const unsigned char *rawDesc;


	// For mutable i486DX >>
	static inline unsigned int FetchByteByLinearAddress(i486DX &cpu,Memory &mem,unsigned int linearAddr)
	{
		return cpu.FetchByteByLinearAddress(mem,linearAddr);
	}
	static inline MemoryAccess::ConstMemoryWindow GetConstMemoryWindowFromLinearAddress(i486DX &cpu,unsigned int linearAddr,Memory &mem)
	{
		return cpu.GetConstMemoryWindowFromLinearAddress(linearAddr,mem);
	}
	static inline const unsigned char *LoadFromDescriptorCache(i486DX &cpu,uint16_t selectorValue)
	{
		auto index=(selectorValue>>DESCRIPTOR_TO_INDEX_SHIFT);
		if(cpu.state.descriptorCacheValidCounter<=cpu.state.descriptorCacheValid[index])
		{
			return cpu.state.descriptorCache[index];
		}
		return nullptr;
	}
	static inline void StoreToDescriptorCache(i486DX &cpu,uint16_t selectorValue,const unsigned char *descPtr)
	{
		auto index=(selectorValue>>DESCRIPTOR_TO_INDEX_SHIFT);
		cpu.state.descriptorCache[index]=descPtr;
		cpu.state.descriptorCacheValid[index]=cpu.state.descriptorCacheValidCounter;
	}
	// For mutable i486DX <<

	// For constant i486DX >>
	static inline unsigned int FetchByteByLinearAddress(const i486DX &cpu,const Memory &mem,unsigned int linearAddr)
	{
		return cpu.DebugFetchByteByLinearAddress(mem,linearAddr);
	}
	static inline MemoryAccess::ConstMemoryWindow GetConstMemoryWindowFromLinearAddress(const i486DX &cpu,unsigned int linearAddr,const Memory &mem)
	{
		return cpu.DebugGetConstMemoryWindowFromLinearAddress(linearAddr,mem);
	}
	static inline const unsigned char *LoadFromDescriptorCache(const i486DX &,uint16_t)
	{
		return nullptr;
	}
	static inline void StoreToDescriptorCache(const i486DX &,uint16_t selectorValue,const unsigned char *)
	{
	}
	// For constant i486DX <<



	inline void LoadProtectedModeDescriptor(CPUCLASS &cpu,unsigned int value,const Memory &mem)
	{
		rawDesc=LoadFromDescriptorCache(cpu,value);
		if(nullptr!=rawDesc)
		{
			return;
		}


		auto RPL=(value&3);
		auto TI=(0!=(value&4));

		unsigned int DTLinearBaseAddr=0;
		if(0==TI)
		{
			DTLinearBaseAddr=cpu.state.GDTR.linearBaseAddr;
		}
		else
		{
			DTLinearBaseAddr=cpu.state.LDTR.linearBaseAddr;
		}
		DTLinearBaseAddr+=(value&0xfff8); // Use upper 13 bits.

		auto memWin=GetConstMemoryWindowFromLinearAddress(cpu,DTLinearBaseAddr,mem);
		if(nullptr!=memWin.ptr && (DTLinearBaseAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1))<=(MemoryAccess::MEMORY_WINDOW_SIZE-8))
		{
			rawDesc=memWin.ptr+(DTLinearBaseAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1));
			StoreToDescriptorCache(cpu,value,rawDesc);
		}
		else
		{
			rawDesc=rawDescBuf;
			rawDescBuf[0]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr);
			rawDescBuf[1]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+1);
			rawDescBuf[2]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+2);
			rawDescBuf[3]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+3);
			rawDescBuf[4]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+4);
			rawDescBuf[5]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+5);
			rawDescBuf[6]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+6);
			rawDescBuf[7]=(unsigned char)FetchByteByLinearAddress(cpu,mem,DTLinearBaseAddr+7);
		}
	}

	inline unsigned int LoadSegmentRegister(CPUCLASS &cpu,SegmentRegister &reg,unsigned int value,const Memory &mem,bool isInRealMode)
	{
		if(true==isInRealMode || 0!=(i486DX::EFLAGS_VIRTUAL86&cpu.state.EFLAGS))
		{
			reg.value=value;
			reg.baseLinearAddr=(value<<4);
			reg.addressSize=16;
			reg.operandSize=16;
			// reg.limit=0xffff;   Surprisingly, reg.limit isn't affected!?  According to https://wiki.osdev.org/Unreal_Mode
			reg.limit=std::max<unsigned int>(reg.limit,0xffff);
			reg.DPL=(0!=(i486DX::EFLAGS_VIRTUAL86&cpu.state.EFLAGS) ? 3 : 0);
			return 0xFFFFFFFF;
		}
		else
		{
			LoadProtectedModeDescriptor(cpu,value,mem);

			// Sample GDT from WRHIGH.ASM
			//	DB		0FFH,0FFH	; Segment Limit (0-15)
			//	DB		0,0,010H		; Base Address 0-23
			//	DB		10010010B	; P=1, DPL=00, S=1, TYPE=0010
			//	DB		11000111B	; G=1, DB=1, (Unused)=0, A=0, LIMIT 16-19=0011
			//	DB		0			; Base Address 24-31

		#ifdef YS_LITTLE_ENDIAN
			uint32_t segLimit,segBase;
			uint8_t *segLimitPtr=(uint8_t *)&segLimit,*segBasePtr=(uint8_t *)&segBase;
			cpputil::CopyWord(segLimitPtr,rawDesc+0);
			segLimitPtr[2]=rawDesc[6]&0x0F;
			segLimitPtr[3]=0;
			cpputil::CopyWord(segBasePtr,rawDesc+2);
			segBasePtr[2]=rawDesc[4];
			segBasePtr[3]=rawDesc[7];
		#else
			unsigned int segLimit=cpputil::GetWord(rawDesc+0)|((rawDesc[6]&0x0F)<<16);
			unsigned int segBase=cpputil::GetWord(rawDesc+2)|(rawDesc[4]<<16)|(rawDesc[7]<<24);
		#endif
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
			reg.DPL=(rawDesc[5]>>5)&3;

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
			return cpputil::GetDword(rawDesc+4);
		}
	}

	inline i486DX::FarPointer GetCallGate(CPUCLASS &cpu,unsigned int value,const Memory &mem)
	{
		LoadProtectedModeDescriptor(cpu,value,mem);

		// i486 Programmer's Reference Manual pp.6-11 Figure 6-5 Call Gate
		// What is "COUNT" used for?
		i486DX::FarPointer ptr;
		ptr.SEG=(rawDesc[2]|(rawDesc[3]<<8));
		ptr.OFFSET=(rawDesc[0]|(rawDesc[1]<<8)|(rawDesc[6]<<16)|(rawDesc[7]<<24));
		return ptr;
	}
};

unsigned int i486DX::DebugLoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem,bool isInRealMode) const
{
	LoadSegmentRegisterTemplate<const i486DX> loader;
	return loader.LoadSegmentRegister(*this,reg,value,mem,IsInRealMode());
}

unsigned int i486DX::LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem)
{
	if(&reg==&state.SS())
	{
		state.holdIRQ=true;
	}
	LoadSegmentRegisterTemplate<i486DX> loader;
	return loader.LoadSegmentRegister(*this,reg,value,mem,IsInRealMode());
}

unsigned i486DX::LoadSegmentRegister(SegmentRegister &reg,unsigned int value,const Memory &mem,bool isInRealMode)
{
	if(&reg==&state.SS())
	{
		state.holdIRQ=true;
	}
	LoadSegmentRegisterTemplate<i486DX> loader;
	return loader.LoadSegmentRegister(*this,reg,value,mem,isInRealMode);
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
	reg.DPL=0; // In real mode, there is no restrictioin, so I think it should be set to highest privilege.
	// reg.limit=0xffff;  Surprisingly, reg.limit isn't affected!?  According to https://wiki.osdev.org/Unreal_Mode
}

void i486DX::LoadTaskRegister(unsigned int value,const Memory &mem)
{
	LoadSegmentRegister(state.TR,value,mem);
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

i486DX::FarPointer i486DX::GetCallGate(unsigned int selector,const Memory &mem)
{
	LoadSegmentRegisterTemplate<i486DX> loader;
	return loader.GetCallGate(*this,selector,mem);
}
i486DX::FarPointer i486DX::DebugGetCallGate(unsigned int selector,const Memory &mem) const
{
	LoadSegmentRegisterTemplate<const i486DX> loader;
	return loader.GetCallGate(*this,selector,mem);
}

i486DX::InterruptDescriptor i486DX::GetInterruptDescriptor(unsigned int INTNum,Memory &mem)
{
	InterruptDescriptor desc;
	if(8*INTNum<state.IDTR.limit)
	{
		auto DTLinearBaseAddr=state.IDTR.linearBaseAddr;
		DTLinearBaseAddr+=(8*INTNum);

		const unsigned char *rawDesc;
		unsigned char rawDescBuf[8];
		auto memWin=GetConstMemoryWindowFromLinearAddress(DTLinearBaseAddr,mem);
		if(nullptr!=memWin.ptr && (DTLinearBaseAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1))<=(MemoryAccess::MEMORY_WINDOW_SIZE-8))
		{
			rawDesc=memWin.ptr+(DTLinearBaseAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1));
		}
		else
		{
			rawDesc=rawDescBuf;
			rawDescBuf[0]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr);
			rawDescBuf[1]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+1);
			rawDescBuf[2]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+2);
			rawDescBuf[3]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+3);
			rawDescBuf[4]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+4);
			rawDescBuf[5]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+5);
			rawDescBuf[6]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+6);
			rawDescBuf[7]=(unsigned char)FetchByteByLinearAddress(mem,DTLinearBaseAddr+7);
		}

		desc.SEG=cpputil::GetWord(rawDesc+2);
		desc.OFFSET=cpputil::GetWord(rawDesc+0)|(cpputil::GetWord(rawDesc+6)<<16);
		desc.flags=cpputil::GetWord(rawDesc+4);
	}
	else
	{
		desc.SEG=FarPointer::NO_SEG;
		desc.OFFSET=0;
	}
	return desc;
}

i486DX::InterruptDescriptor i486DX::DebugGetInterruptDescriptor(unsigned int INTNum,const Memory &mem) const
{
	InterruptDescriptor desc;
	if(8*INTNum<state.IDTR.limit)
	{
		auto DTLinearBaseAddr=state.IDTR.linearBaseAddr;
		DTLinearBaseAddr+=(8*INTNum);
		const unsigned char rawDesc[8]=
		{
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+1),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+2),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+3),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+4),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+5),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+6),
			(unsigned char)DebugFetchByteByLinearAddress(mem,DTLinearBaseAddr+7)
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
	cpputil::PutWord(operaValue.byteData,reg.limit);
	//operaValue.byteData[0]=reg.limit&0xFF;
	//operaValue.byteData[1]=(reg.limit>>8)&0xFF;
	cpputil::PutDword(operaValue.byteData+2,reg.linearBaseAddr);
	//operaValue.byteData[2]=reg.linearBaseAddr&0xFF;
	//operaValue.byteData[3]=(reg.linearBaseAddr>>8)&0xFF;
	//operaValue.byteData[4]=(reg.linearBaseAddr>>16)&0xFF;
	//operaValue.byteData[5]=(reg.linearBaseAddr>>24)&0xFF;
	if(16==operandSize)
	{
		operaValue.byteData[5]=0;
	}
	return operaValue;
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
	}
	return 0;
}

inline unsigned char *i486DX::GetStackAccessPointer(Memory &mem,uint32_t linearAddr,const unsigned int numBytes)
{
	if((linearAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1))<=(MemoryAccess::MEMORY_WINDOW_SIZE-numBytes))
	{
		if(nullptr==state.SSESPWindow.ptr || true!=state.SSESPWindow.IsLinearAddressInRange(linearAddr))
		{
			auto physAddr=linearAddr;
			if(true==PagingEnabled())
			{
				physAddr=LinearAddressToPhysicalAddressWrite(linearAddr,mem); // Assume write-operation for stack.
			}
			state.SSESPWindow=mem.GetMemoryWindow(physAddr);
			state.SSESPWindow.linearBaseAddr=(linearAddr&(~(MemoryAccess::MEMORY_WINDOW_SIZE-1)));
		}
		// The second conidition in the next line is automatic.
		if(nullptr!=state.SSESPWindow.ptr /*&& true==state.SSESPWindow.IsLinearAddressInRange(linearAddr)*/)
		{
			return state.SSESPWindow.ptr+(linearAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1));
		}
	}
	return nullptr;
}

void i486DX::Push16(Memory &mem,unsigned int value)
{
	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();

	const unsigned int bytesToStore=2;
	ESP-=bytesToStore;

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToStore);
	if(nullptr!=accessPtr)
	{
		cpputil::PutWord(accessPtr,value);
		return;
	}

	// When addressSize==16, ESP will be ANDed with 0xFFFF in StoreWord/StoreDword.
	// Also ESP crossing 16-bit boundary would be an exception if addressSize==16.
	// I cannot check it here, but to run a valid application, it shouldn't happen.
	StoreWord(mem,addressSize,state.SS(),ESP,value);
}

void i486DX::Push32(Memory &mem,unsigned int value)
{
	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();

	const unsigned int bytesToStore=4;
	ESP-=bytesToStore;

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToStore);
	if(nullptr!=accessPtr)
	{
		cpputil::PutDword(accessPtr,value);
		return;
	}
	StoreDword(mem,addressSize,state.SS(),ESP,value);
}

unsigned int i486DX::Pop16(Memory &mem)
{
	NUM_BYTES_MASK;
	unsigned int value;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();
	const unsigned int bytesToPop=2;

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToPop);
	if(nullptr!=accessPtr)
	{
		ESP+=bytesToPop;
		return cpputil::GetWord(accessPtr);
	}

	// When addressSize==16, ESP will be &ed with 0xFFFF in StoreWord/StoreDword.
	// Also ESP crossing 16-bit boundary would be an exception if addressSize==16.
	// I cannot check it here, but to run a valid application, it shouldn't happen.
	value=FetchWord(addressSize,state.SS(),ESP,mem);
	ESP+=2;
	return value;
}

unsigned int i486DX::Pop32(Memory &mem)
{
	NUM_BYTES_MASK;
	unsigned int value;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();
	const unsigned int bytesToPop=4;

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToPop);
	if(nullptr!=accessPtr)
	{
		ESP+=bytesToPop;
		return cpputil::GetDword(accessPtr);
	}

	value=FetchDword(addressSize,state.SS(),ESP,mem);
	ESP+=4;
	return value;
}

void i486DX::Push(Memory &mem,unsigned int operandSize,uint32_t firstPush,uint32_t secondPush)
{
	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();

	const unsigned int bytesToStore=(operandSize>>3)*2;
	ESP-=bytesToStore;

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToStore);
	if(nullptr!=accessPtr)
	{
		if(16==operandSize)
		{
			cpputil::PutWord(accessPtr+2,firstPush);
			cpputil::PutWord(accessPtr  ,secondPush);
		}
		else // if(32==operandSize)
		{
			cpputil::PutDword(accessPtr+4,firstPush);
			cpputil::PutDword(accessPtr  ,secondPush);
		}
		return;
	}

	if(16==operandSize)
	{
		StoreWord(mem,addressSize,state.SS(),ESP+2,firstPush);
		StoreWord(mem,addressSize,state.SS(),ESP  ,secondPush);
	}
	else // if(32==operandSize)
	{
		StoreDword(mem,addressSize,state.SS(),ESP+4,firstPush);
		StoreDword(mem,addressSize,state.SS(),ESP  ,secondPush);
	}
}
void i486DX::Pop(uint32_t &firstPop,uint32_t &secondPop,Memory &mem,unsigned int operandSize)
{
	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();
	const unsigned int bytesToPop=(operandSize>>3)*2;

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToPop);
	if(nullptr!=accessPtr)
	{
		if(16==operandSize)
		{
			firstPop=cpputil::GetWord(accessPtr);
			secondPop=cpputil::GetWord(accessPtr+2);
		}
		else // if(32==operandSize)
		{
			firstPop=cpputil::GetDword(accessPtr);
			secondPop=cpputil::GetDword(accessPtr+4);
		}
		ESP+=bytesToPop;
		return;
	}

	if(16==operandSize)
	{
		firstPop=FetchWord(addressSize,state.SS(),ESP,mem);
		secondPop=FetchWord(addressSize,state.SS(),ESP+2,mem);
		ESP+=4;
	}
	else // if(32==operandSize)
	{
		firstPop=FetchDword(addressSize,state.SS(),ESP,mem);
		secondPop=FetchDword(addressSize,state.SS(),ESP+4,mem);
		ESP+=8;
	}
}

void i486DX::Push(Memory &mem,unsigned int operandSize,uint32_t firstPush,uint32_t secondPush,uint32_t thirdPush)
{
	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();

	const unsigned int bytesToStore=(operandSize>>3)*3;
	ESP-=bytesToStore;

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToStore);
	if(nullptr!=accessPtr)
	{
		if(16==operandSize)
		{
			cpputil::PutWord(accessPtr+4,firstPush);
			cpputil::PutWord(accessPtr+2,secondPush);
			cpputil::PutWord(accessPtr  ,thirdPush);
		}
		else // if(32==operandSize)
		{
			cpputil::PutDword(accessPtr+8,firstPush);
			cpputil::PutDword(accessPtr+4,secondPush);
			cpputil::PutDword(accessPtr  ,thirdPush);
		}
		return;
	}

	if(16==operandSize)
	{
		StoreWord(mem,addressSize,state.SS(),ESP+4,firstPush);
		StoreWord(mem,addressSize,state.SS(),ESP+2,secondPush);
		StoreWord(mem,addressSize,state.SS(),ESP  ,thirdPush);
	}
	else // if(32==operandSize)
	{
		StoreDword(mem,addressSize,state.SS(),ESP+8,firstPush);
		StoreDword(mem,addressSize,state.SS(),ESP+4,secondPush);
		StoreDword(mem,addressSize,state.SS(),ESP  ,thirdPush);
	}
}
void i486DX::Pop(uint32_t &firstPop,uint32_t &secondPop,uint32_t &thirdPop,Memory &mem,unsigned int operandSize)
{
	NUM_BYTES_MASK;
	auto addressSize=GetStackAddressingSize();
	auto &ESP=state.ESP();
	const unsigned int bytesToPop=(operandSize>>3)*3;

	unsigned int linearAddr=state.SS().baseLinearAddr+(ESP&(numBytesMask[addressSize>>3]));
	auto accessPtr=GetStackAccessPointer(mem,linearAddr,bytesToPop);
	if(nullptr!=accessPtr)
	{
		if(16==operandSize)
		{
			firstPop=cpputil::GetWord(accessPtr);
			secondPop=cpputil::GetWord(accessPtr+2);
			thirdPop=cpputil::GetWord(accessPtr+4);
		}
		else // if(32==operandSize)
		{
			firstPop=cpputil::GetDword(accessPtr);
			secondPop=cpputil::GetDword(accessPtr+4);
			thirdPop=cpputil::GetDword(accessPtr+8);
		}
		ESP+=bytesToPop;
		return;
	}

	if(16==operandSize)
	{
		firstPop=FetchWord(addressSize,state.SS(),ESP,mem);
		secondPop=FetchWord(addressSize,state.SS(),ESP+2,mem);
		thirdPop=FetchWord(addressSize,state.SS(),ESP+4,mem);
		ESP+=6;
	}
	else // if(32==operandSize)
	{
		firstPop=FetchDword(addressSize,state.SS(),ESP,mem);
		secondPop=FetchDword(addressSize,state.SS(),ESP+4,mem);
		thirdPop=FetchDword(addressSize,state.SS(),ESP+8,mem);
		ESP+=12;
	}
}

unsigned int i486DX::PhysicalAddressToLinearAddress(unsigned physAddr,const Memory &mem) const
{
	if(true==IsInRealMode() || true!=PagingEnabled())
	{
		return physAddr;
	}

	// Go reverse order.  Smaller linear address maps to the same physical address for linear access.
	for(int pageDirectoryIndex=1023; 0<=pageDirectoryIndex; --pageDirectoryIndex)
	{
		const unsigned int pageDirectoryPtr=state.GetCR(3)&0xFFFFF000;
		unsigned int pageTableInfo=mem.FetchDword(pageDirectoryPtr+(pageDirectoryIndex<<2));
		if(0==(pageTableInfo&1))
		{
			continue;
		}

		const unsigned int pageTablePtr=(pageTableInfo&0xFFFFF000);
		for(int pageTableIndex=1023; 0<=pageTableIndex; --pageTableIndex)
		{
			unsigned int linearAddrBase=(pageDirectoryIndex<<22)|(pageTableIndex<<12);
			unsigned int pageInfo=mem.FetchDword(pageTablePtr+(pageTableIndex<<2));
			if(0!=(pageInfo&1))
			{
				if((physAddr&0xFFFFF000)==(pageInfo&0xFFFFF000))
				{
					return linearAddrBase+(physAddr&0xFFF);
				}
			}
		}
	}
	return 0;
}

std::string i486DX::Disassemble(const Instruction &inst,const Operand &op1,const Operand &op2,SegmentRegister seg,unsigned int offset,const Memory &mem,const class i486SymbolTable &symTable,const std::map <unsigned int,std::string> &ioTable) const
{
	std::string disasm;
	disasm+=cpputil::Ustox(seg.value);
	disasm+=":";
	disasm+=cpputil::Uitox(offset);
	disasm+=" ";

	for(unsigned int i=0; i<inst.numBytes; ++i)
	{
		disasm+=cpputil::Ubtox(DebugFetchByte(seg.addressSize,seg,offset+i,mem));
	}
	disasm+=" ";

	cpputil::ExtendString(disasm,40);
	disasm+=inst.Disassemble(op1,op2,seg,offset,symTable,ioTable);

	return disasm;
}

std::string i486DX::DisassembleData(unsigned int addressSize,SegmentRegister seg,unsigned int offset,const Memory &mem,unsigned int unitBytes,unsigned int segBytes,unsigned int repeat,unsigned int chopOff) const
{
	if(4<unitBytes)
	{
		return "4+ unit byte count not supported";
	}
	if(4<segBytes)
	{
		return "4+ segment byte count not supported";
	}

	std::string disasm;

	unsigned int perLine;
	if(1==unitBytes && 0==segBytes)
	{
		perLine=8;
	}
	else
	{
		perLine=1;
	}
	for(unsigned int i=0; i<repeat && i<chopOff; ++i)
	{
		if(0==i)
		{
			disasm+=cpputil::Ustox(seg.value);
			disasm+=":";
			disasm+=cpputil::Uitox(offset);
			disasm+=" ";
		}
		else if(0==(i%perLine))
		{
			disasm+="              ";
		}

		if(1==unitBytes && 0==segBytes)
		{
			disasm+=cpputil::Ubtox(DebugFetchByte(addressSize,seg,offset,mem));
			++offset;
		}
		else
		{
			unsigned char dword[4],segpart[4]={0,0,0,0};
			for(int i=0; i<(int)unitBytes; ++i)
			{
				dword[i]=DebugFetchByte(addressSize,seg,offset,mem);
				++offset;
			}
			for(int i=0; i<(int)segBytes; ++i)
			{
				segpart[i]=DebugFetchByte(addressSize,seg,offset,mem);
				++offset;
			}

			if(0<segBytes)
			{
				disasm+=cpputil::Ubtox(segpart[1]);
				disasm+=cpputil::Ubtox(segpart[0]);
				disasm.push_back(':');
			}
			for(int i=unitBytes-1; 0<=i; --i)
			{
				disasm+=cpputil::Ubtox(dword[i]);
			}
		}

		if(0==(i+1)%perLine)
		{
			disasm.push_back('\n');
		}
	}
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
	SetOF(value==0x7FFFFFFF);
	SetSF(0!=(value&0x80000000));
	SetZF(0==value);
	SetAF(0x0F==(value&0x0F));
	SetPF(CheckParity(value));
}
void i486DX::DecrementWord(unsigned int &value)
{
	value=((value-1)&0xFFFF);
	SetOF(value==0x7FFF);
	SetSF(0!=(value&0x8000));
	SetZF(0==value);
	SetAF(0x0F==(value&0x0F));
	SetPF(CheckParity(value));
}
void i486DX::DecrementByte(unsigned int &value)
{
	value=((value-1)&0xFF);
	SetOF(value==0x7F);
	SetSF(0!=(value&0x80));
	SetZF(0==value);
	SetAF(0x0F==(value&0x0F));
	SetPF(CheckParity(value));
}
void i486DX::DecrementWithMask(unsigned int &value,unsigned int mask,unsigned int signBit)
{
	value=((value-1)&mask);
	SetOF(signBit-1==value);
	SetAF(0x0F==(value&0x0F));
	SetSF(value&signBit);
	SetZF(0==value);
	SetPF(CheckParity(value));
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
	SetAF(0x0F==(value&0x0F));
	++value;
	SetOF(value==0x80000000);
	SetSF(0!=(value&0x80000000));
	SetZF(0==value);
	SetPF(CheckParity(value));
}
void i486DX::IncrementWord(unsigned int &value)
{
	SetAF(0x0F==(value&0x0F));
	value=(value+1)&0xffff;
	SetOF(value==0x8000);
	SetSF(0!=(value&0x8000));
	SetZF(0==value);
	SetPF(CheckParity(value));
}
void i486DX::IncrementByte(unsigned int &value)
{
	SetAF(0x0F==(value&0x0F));
	value=(value+1)&0xff;
	SetOF(value==0x80);
	SetSF(0!=(value&0x80));
	SetZF(0==value);
	SetPF(CheckParity(value));
}
void i486DX::IncrementWithMask(unsigned int &value,unsigned int mask,unsigned int signBit)
{
	SetAF(0x0F==(value&0x0F));
	value=(value+1)&mask;
	SetOF(value==signBit);
	SetSF(value&signBit);
	SetZF(0==value);
	SetPF(CheckParity(value));
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
	state.EFLAGS&=~(
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_AUX_CARRY|
		EFLAGS_CARRY|
		EFLAGS_PARITY
	);
	RaiseOF((prevValue&0x80000000)==(value2&0x80000000) && (prevValue&0x80000000)!=(value1&0x80000000)); // Two sources have same sign, but the result sign is different.
	RaiseSF(0!=(value1&0x80000000));
	RaiseZF(0==value1);
	RaiseAF((value1&0x0F)<(prevValue&0x0F));
	RaiseCF(value1<prevValue);
	RaisePF(CheckParity(value1));
}
void i486DX::AddWord(unsigned int &value1,unsigned int value2)
{
	auto prevValue=value1&0xffff;
	value1=(value1+value2)&0xffff;
	state.EFLAGS&=~(
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_AUX_CARRY|
		EFLAGS_CARRY|
		EFLAGS_PARITY
	);
	RaiseOF((prevValue&0x8000)==(value2&0x8000) && (prevValue&0x8000)!=(value1&0x8000)); // Two sources have same sign, but the result sign is different.
	RaiseSF(0!=(value1&0x8000));
	RaiseZF(0==value1);
	RaiseAF((value1&0x0F)<(prevValue&0x0F));
	RaiseCF(value1<prevValue);
	RaisePF(CheckParity(value1));
}
void i486DX::AddByte(unsigned int &value1,unsigned int value2)
{
	auto prevValue=value1&0xff;
	value1=(value1+value2)&0xff;
	state.EFLAGS&=~(
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_AUX_CARRY|
		EFLAGS_CARRY|
		EFLAGS_PARITY
	);
	RaiseOF((prevValue&0x80)==(value2&0x80) && (prevValue&0x80)!=(value1&0x80)); // Two sources have same sign, but the result sign is different.
	RaiseSF(0!=(value1&0x80));
	RaiseZF(0==value1);
	RaiseAF((value1&0x0F)<(prevValue&0x0F));
	RaiseCF(value1<prevValue);
	RaisePF(CheckParity(value1));
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
	value1&=value2;
	state.EFLAGS&=~(
		EFLAGS_CARRY|
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_PARITY);
	RaiseSF(0!=(0x80000000&value1));
	RaiseZF(0==value1);
	RaisePF(CheckParity(value1));
}
void i486DX::AndWord(unsigned int &value1,unsigned int value2)
{
	value1&=value2;
	value1&=0xFFFF;
	state.EFLAGS&=~(
		EFLAGS_CARRY|
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_PARITY);
	RaiseSF(0!=(0x8000&value1));
	RaiseZF(0==value1);
	RaisePF(CheckParity(value1));
}
void i486DX::AndByte(unsigned int &value1,unsigned int value2)
{
	value1&=value2;
	value1&=0xFF;
	state.EFLAGS&=~(
		EFLAGS_CARRY|
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_PARITY);
	RaiseSF(0!=(0x80&value1));
	RaiseZF(0==value1);
	RaisePF(CheckParity(value1));
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
	state.EFLAGS&=~(
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_AUX_CARRY|
		EFLAGS_CARRY|
		EFLAGS_PARITY
	);
	RaiseOF((prevValue&0x80000000)!=(value2&0x80000000) && (prevValue&0x80000000)!=(value1&0x80000000)); // Source values have different signs, but the sign flipped.
	RaiseSF(0!=(value1&0x80000000));
	RaiseZF(0==value1);
	RaiseAF((prevValue&0xF)<(value1&0xF));
	RaiseCF(value1>prevValue);
	RaisePF(CheckParity(value1));
}
void i486DX::SubWord(unsigned int &value1,unsigned int value2)
{
	auto prevValue=value1&0xffff;
	value1=(value1-value2)&0xffff;
	state.EFLAGS&=~(
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_AUX_CARRY|
		EFLAGS_CARRY|
		EFLAGS_PARITY
	);
	RaiseOF((prevValue&0x8000)!=(value2&0x8000) && (prevValue&0x8000)!=(value1&0x8000)); // Source values have different signs, but the sign flipped.
	RaiseSF(0!=(value1&0x8000));
	RaiseZF(0==value1);
	RaiseAF((prevValue&0xF)<(value1&0xF));
	RaiseCF(value1>prevValue);
	RaisePF(CheckParity(value1));
}
void i486DX::SubByte(unsigned int &value1,unsigned int value2)
{
	auto prevValue=value1&0xff;
	value1=(value1-value2)&0xff;
	state.EFLAGS&=~(
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_AUX_CARRY|
		EFLAGS_CARRY|
		EFLAGS_PARITY
	);
	RaiseOF((prevValue&0x80)!=(value2&0x80) && (prevValue&0x80)!=(value1&0x80)); // Source values have different signs, but the sign flipped.
	RaiseSF(0!=(value1&0x80));
	RaiseZF(0==value1);
	RaiseAF((prevValue&0xF)<(value1&0xF));
	RaiseCF(value1>prevValue);
	RaisePF(CheckParity(value1));
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
	state.EFLAGS&=~(
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_AUX_CARRY|
		EFLAGS_CARRY|
		EFLAGS_PARITY
	);
	value1=(value1+value2+carry)&0xffffffff;
	RaiseOF((prevValue&0x80000000)==(value2&0x80000000) && (prevValue&0x80000000)!=(value1&0x80000000)); // Two sources have same sign, but the result sign is different.
	RaiseSF(0!=(value1&0x80000000));
	RaiseZF(0==value1);
	RaiseAF((value1&0x0F)<(prevValue&0x0F) || (0!=carry && (prevValue&0x0F)==(value1&0x0F))); // 2nd condition for 0xFFFFFFFF+0xFFFFFFFF+1
	RaiseCF(value1<prevValue || (0!=carry && value1==prevValue)); // 2nd condition for 0xFFFFFFFF+0xFFFFFFFF+1
	RaisePF(CheckParity(value1));
}
void i486DX::AdcWord(unsigned int &value1,unsigned int value2)
{
	auto carry=(0!=(state.EFLAGS&EFLAGS_CARRY) ? 1 : 0);
	auto prevValue=value1&0xffff;
	value1=(value1+value2+carry)&0xffff;
	state.EFLAGS&=~(
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_AUX_CARRY|
		EFLAGS_CARRY|
		EFLAGS_PARITY
	);
	RaiseOF((prevValue&0x8000)==(value2&0x8000) && (prevValue&0x8000)!=(value1&0x8000)); // Two sources have same sign, but the result sign is different.
	RaiseSF(0!=(value1&0x8000));
	RaiseZF(0==value1);
	RaiseAF((value1&0x0F)<(prevValue&0x0F) || (0!=carry && (prevValue&0x0F)==(value1&0x0F))); // 2nd condition for 0xFFFF+0xFFFF+1
	RaiseCF(value1<prevValue || (0!=carry && value1==prevValue)); // 2nd condition for 0xFFFF+0xFFFF+1
	RaisePF(CheckParity(value1));
}
void i486DX::AdcByte(unsigned int &value1,unsigned int value2)
{
	auto carry=(0!=(state.EFLAGS&EFLAGS_CARRY) ? 1 : 0);
	auto prevValue=value1&0xff;
	value1=(value1+value2+carry)&0xff;
	state.EFLAGS&=~(
		EFLAGS_OVERFLOW|
		EFLAGS_SIGN|
		EFLAGS_ZERO|
		EFLAGS_AUX_CARRY|
		EFLAGS_CARRY|
		EFLAGS_PARITY
	);
	RaiseOF((prevValue&0x80)==(value2&0x80) && (prevValue&0x80)!=(value1&0x80)); // Two sources have same sign, but the result sign is different.
	RaiseSF(0!=(value1&0x80));
	RaiseZF(0==value1);
	RaiseAF((value1&0x0F)<(prevValue&0x0F) || (0!=carry && (prevValue&0x0F)==(value1&0x0F))); // 2nd condition for 0xFF+0xFF+1
	RaiseCF(value1<prevValue || (0!=carry && value1==prevValue)); // 2nd condition for 0xFF+0xFF+1
	RaisePF(CheckParity(value1));
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
	SetOF((prevValue&0x80000000)!=(value2&0x80000000) && (prevValue&0x80000000)!=(value1&0x80000000)); // Source values have different signs, but the sign flipped.
	SetSF(0!=(value1&0x80000000));
	SetZF(0==value1);
	SetAF((prevValue&0xF)<(value1&0xF) || (0!=carry && (prevValue&0x0F)==(value1&0x0F)));
	SetCF(value1>prevValue || (0!=carry && value1==prevValue));
	SetPF(CheckParity(value1));
}
void i486DX::SbbWord(unsigned int &value1,unsigned int value2)
{
	auto carry=(0!=(state.EFLAGS&EFLAGS_CARRY) ? 1 : 0);
	auto prevValue=value1&0xffff;
	value1=(value1-value2-carry)&0xffff;
	SetOF((prevValue&0x8000)!=(value2&0x8000) && (prevValue&0x8000)!=(value1&0x8000)); // Source values have different signs, but the sign flipped.
	SetSF(0!=(value1&0x8000));
	SetZF(0==value1);
	SetAF((prevValue&0xF)<(value1&0xF) || (0!=carry && (prevValue&0x0F)==(value1&0x0F)));
	SetCF(value1>prevValue || (0!=carry && value1==prevValue));
	SetPF(CheckParity(value1));
}
void i486DX::SbbByte(unsigned int &value1,unsigned int value2)
{
	auto carry=(0!=(state.EFLAGS&EFLAGS_CARRY) ? 1 : 0);
	auto prevValue=value1&0xff;
	value1=(value1-value2-carry)&0xff;
	SetOF((prevValue&0x80)!=(value2&0x80) && (prevValue&0x80)!=(value1&0x80)); // Source values have different signs, but the sign flipped.
	SetSF(0!=(value1&0x80));
	SetZF(0==value1);
	SetAF((prevValue&0xF)<(value1&0xF) || (0!=carry && (prevValue&0x0F)==(value1&0x0F)));
	SetCF(value1>prevValue || (0!=carry && value1==prevValue));
	SetPF(CheckParity(value1));
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
	ClearCFOF();
	//SetCF(false);
	//SetOF(false);
	value1|=value2;
	SetSF(0!=(0x80000000&value1));
	SetZF(0==value1);
	SetPF(CheckParity(value1));
}
void i486DX::OrWord(unsigned int &value1,unsigned int value2)
{
	ClearCFOF();
	//SetCF(false);
	//SetOF(false);
	value1|=value2;
	value1&=0xFFFF;
	SetSF(0!=(0x8000&value1));
	SetZF(0==value1);
	SetPF(CheckParity(value1));
}
void i486DX::OrByte(unsigned int &value1,unsigned int value2)
{
	ClearCFOF();
	//SetCF(false);
	//SetOF(false);
	value1|=value2;
	value1&=0xFF;
	SetSF(0!=(0x80&value1));
	SetZF(0==value1);
	SetPF(CheckParity(value1));
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
	ClearCFOF();
	//SetCF(false);
	//SetOF(false);
	value1^=value2;
	SetSF(0!=(0x80000000&value1));
	SetZF(0==value1);
	SetPF(CheckParity(value1));
}
void i486DX::XorWord(unsigned int &value1,unsigned int value2)
{
	ClearCFOF();
	//SetCF(false);
	//SetOF(false);
	value1^=value2;
	value1&=0xFFFF;
	SetSF(0!=(0x8000&value1));
	SetZF(0==value1);
	SetPF(CheckParity(value1));
}
void i486DX::XorByte(unsigned int &value1,unsigned int value2)
{
	ClearCFOF();
	//SetCF(false);
	//SetOF(false);
	value1^=value2;
	value1&=0xFF;
	SetSF(0!=(0x80&value1));
	SetZF(0==value1);
	SetPF(CheckParity(value1));
}

template<typename T, typename _>
inline void i486DX::RolTemplate(unsigned int &value, unsigned int c) {

	constexpr auto all = std::numeric_limits<T>::max();
	constexpr auto sign = all ^ (all >> 1);

	T src = value;

	auto u = src + 0U;
	auto e = c & (std::numeric_limits<T>::digits - 1);
	T res = (u << e) | (u >> (std::numeric_limits<T>::digits - e));

	value = res;
	bool lsb = (res & 1) != 0;
	SetCF(c != 0 && lsb);
	if (c == 1) {
		bool msb = (res & sign) != 0;
		SetOF(msb != lsb);
	}
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

void i486DX::RolDword(unsigned int &value, unsigned int ctr)
{
	RolTemplate<uint32_t>(value, ctr);
}

void i486DX::RolWord(unsigned int &value, unsigned int ctr)
{
	RolTemplate<uint16_t>(value, ctr);
}

void i486DX::RolByte(unsigned int &value, unsigned int ctr)
{
	RolTemplate<uint8_t>(value, ctr);
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

template<typename T, typename _>
inline void i486DX::RorTemplate(unsigned int &value, unsigned int c) {

	constexpr auto all = std::numeric_limits<T>::max();
	constexpr auto sign = all ^ (all >> 1);

	T src = value;

	auto u = src + 0U;
	auto e = c & (std::numeric_limits<T>::digits - 1);
	T res = (u >> e) | (u << (std::numeric_limits<T>::digits - e));

	value = res;
	SetCF(c != 0 && (res & sign) != 0);
	if (c == 1) {
		SetOF(((src ^ res) & sign) != 0);
	}
}

void i486DX::RorDword(unsigned int &value, unsigned int ctr)
{
	RorTemplate<uint32_t>(value, ctr);
}

void i486DX::RorWord(unsigned int &value, unsigned int ctr)
{
	RorTemplate<uint16_t>(value, ctr);
}

void i486DX::RorByte(unsigned int &value, unsigned int ctr)
{
	RorTemplate<uint8_t>(value, ctr);
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
		SetOF((prevValue&0x80000000)!=(value&0x80000000));
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
		SetOF((prevValue&0x8000)!=(value&0x8000));
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
		SetOF((prevValue&0x80)!=(value&0x80));
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
	if(1==ctr)
	{
		bool sgn=(0!=(value&0x80000000));
		SetOF(sgn!=GetCF());
	}
	for(unsigned int i=0; i<ctr; ++i)
	{
		unsigned int highBit=(GetCF() ? 0x80000000 : 0);
		SetCF(0!=(value&1));
		value=(value>>1)|highBit;
	}
}
void i486DX::RcrWord(unsigned int &value,unsigned int ctr)
{
	value&=0xffff;
	if(1==ctr)
	{
		bool sgn=(0!=(value&0x8000));
		SetOF(sgn!=GetCF());
	}
	for(unsigned int i=0; i<ctr; ++i)
	{
		unsigned int highBit=(GetCF() ? 0x8000 : 0);
		SetCF(0!=(value&1));
		value=(value>>1)|highBit;
	}
}
void i486DX::RcrByte(unsigned int &value,unsigned int ctr)
{
	value&=0xff;
	if(1==ctr)
	{
		bool sgn=(0!=(value&0x80));
		SetOF(sgn!=GetCF());
	}
	for(unsigned int i=0; i<ctr; ++i)
	{
		unsigned int highBit=(GetCF() ? 0x80 : 0);
		SetCF(0!=(value&1));
		value=(value>>1)|highBit;
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
	SetZF(0==value64);
	SetSF(0!=(value64&0x80000000));
	SetPF(CheckParity(value64&0xFF));
	if(1==ctr)
	{
		SetOF(false);
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
	SetZF(0==value32);
	SetSF(0!=(value32&0x8000));
	SetPF(CheckParity(value32&0xFF));
	if(1==ctr)
	{
		SetOF(false);
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
	SetZF(0==value16);
	SetSF(0!=(value16&0x80));
	SetPF(CheckParity(value16&0xFF));
	if(1==ctr)
	{
		SetOF(false);
	}
	value=value16;
}

template <unsigned int bitCount,unsigned int maskBits,unsigned int signBit>
void i486DX::ShlTemplate(unsigned int &value,unsigned int ctr)
{
	// OF CF ZF PF SF
	ctr&=31;
	SetCF(0!=ctr && 0!=(value&(signBit>>(ctr-1))));
	if(1<ctr)
	{
		value=(value<<(ctr-1));
		value=(value<<1)&maskBits;
	}
	else if(1==ctr)
	{
		auto prevValue=value;
		value=(value<<1)&maskBits;
		SetOF((prevValue&signBit)!=(value&signBit));
	}
	SetZF(0==value);
	SetPF(CheckParity(value));
	SetSF(0!=(value&signBit));
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
	ShlTemplate<32,0xFFFFFFFF,0x80000000>(value,ctr);
}
void i486DX::ShlWord(unsigned int &value,unsigned int ctr)
{
	ShlTemplate<16,0xFFFF,0x8000>(value,ctr);
}
void i486DX::ShlByte(unsigned int &value,unsigned int ctr)
{
	ShlTemplate<8,0xFF,0x80>(value,ctr);
}

template <unsigned int bitCount,unsigned int maskBits,unsigned int signBit>
inline void i486DX::ShrTemplate(unsigned int &value,unsigned int ctr)
{
	// OF CF ZF PF SF
	ctr&=31;
	SetCF(0!=ctr && 0!=(value&(1<<(ctr-1))));
	if(1<ctr)
	{
		value&=maskBits;
		value>>=ctr;
	}
	else if(1==ctr)
	{
		SetOF(0!=(value&signBit));
		value&=maskBits;
		value>>=1;
	}
	SetZF(0==value);
	SetPF(CheckParity(value));
	SetSF(0!=(value&signBit));
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
	ShrTemplate<32,0xffffffff,0x80000000>(value,ctr);
}
void i486DX::ShrWord(unsigned int &value,unsigned int ctr)
{
	ShrTemplate<16,0xFFFF,0x8000>(value,ctr);
}
void i486DX::ShrByte(unsigned int &value,unsigned int ctr)
{
	ShrTemplate<8,0xFF,0x80>(value,ctr);
}



i486DX::OperandValue i486DX::EvaluateOperand(
    Memory &mem,int addressSize,int segmentOverride,const Operand &op,int destinationBytes)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

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

			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);

			offset&=addressMask[addressSize>>5];
			switch(value.numBytes)
			{
			case 1:
				value.byteData[0]=FetchByte(addressSize,seg,offset,mem);
				break;
			case 2:
				value.SetWord(FetchWord(addressSize,seg,offset,mem));
				break;
			case 4:
				value.SetDword(FetchDword(addressSize,seg,offset,mem));
				break;
			default:
				for(unsigned int i=0; i<value.numBytes; ++i)
				{
					value.byteData[i]=FetchByte(addressSize,seg,offset+i,mem);
				}
				break;
			}
		}
		break;
	case OPER_FARADDR:
		Abort("Tried to evaluate FAR ADDRESS.");
		break;
	case OPER_REG32:
		{
			unsigned int reg=state.NULL_and_reg32[op.reg];
			value.numBytes=4;
			cpputil::PutDword(value.byteData,reg);
		}
		break;
	case OPER_REG16:
		{
			unsigned int reg=state.NULL_and_reg32[op.reg&15];
			value.numBytes=2;
			cpputil::PutWord(value.byteData,reg);
		}
		break;
	case OPER_REG8:
		{
			value.numBytes=1;
			value.byteData[0]=GetRegisterValue8(op.reg);
		}
		break;
	case OPER_SREG:
		value.numBytes=2;
		cpputil::PutWord(value.byteData,state.sreg[op.reg-REG_SEGMENT_REG_BASE].value);
		break;

	case OPER_CR0:
	case OPER_CR1:
	case OPER_CR2:
	case OPER_CR3:
		value.numBytes=4;
		cpputil::PutDword(value.byteData,state.GetCR(op.operandType-OPER_CR0));
		break;

	case OPER_DR0:
	case OPER_DR1:
	case OPER_DR2:
	case OPER_DR3:
	case OPER_DR4:
	case OPER_DR5:
	case OPER_DR6:
	case OPER_DR7:
		value.numBytes=4;
		cpputil::PutDword(value.byteData,state.DR[op.operandType-OPER_DR0]);
		break;
	case OPER_TEST0:
	case OPER_TEST1:
	case OPER_TEST2:
	case OPER_TEST3:
	case OPER_TEST4:
	case OPER_TEST5:
	case OPER_TEST6:
	case OPER_TEST7:
		value.numBytes=4;
		cpputil::PutDword(value.byteData,state.TEST[op.operandType-OPER_TEST0]);
		break;
	default:
		#ifdef _WIN32
			__assume(0);
		#elif defined(__clang__) || defined(__GNUC__)
			__builtin_unreachable();
		#else
			break;
		#endif
	}
	return value;
}

uint16_t i486DX::EvaluateOperandRegOrMem16(Memory &mem,int addressSize,int segmentOverride,const Operand &op)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	i486DX::OperandValue value;
	value.numBytes=0;
	if(OPER_REG32==op.operandType)
	{
		return state.NULL_and_reg32[op.reg];
	}
	else if(OPER_REG16==op.operandType)
	{
		return INT_LOW_WORD(state.NULL_and_reg32[op.reg&15]);
	}
	else
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);

		offset&=addressMask[addressSize>>5];
		return FetchWord(addressSize,seg,offset,mem);
	}
}
uint32_t i486DX::EvaluateOperandRegOrMem32(Memory &mem,int addressSize,int segmentOverride,const Operand &op)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	i486DX::OperandValue value;
	value.numBytes=0;
	if(OPER_REG32==op.operandType)
	{
		return state.NULL_and_reg32[op.reg];
	}
	else if(OPER_REG16==op.operandType)
	{
		return INT_LOW_WORD(state.NULL_and_reg32[op.reg&15]);
	}
	else
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);

		offset&=addressMask[addressSize>>5];
		return FetchDword(addressSize,seg,offset,mem);
	}
}

i486DX::OperandValue i486DX::EvaluateOperandReg16OrReg32OrMem(
	    Memory &mem,int addressSize,int segmentOverride,const Operand &op,int destinationBytes)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	i486DX::OperandValue value;
	value.numBytes=0;
	if(OPER_REG32==op.operandType)
	{
		unsigned int reg=state.NULL_and_reg32[op.reg];
		value.numBytes=4;
		cpputil::PutDword(value.byteData,reg);
	}
	else if(OPER_REG16==op.operandType)
	{
		unsigned int reg=state.NULL_and_reg32[op.reg&15];
		value.numBytes=2;
		cpputil::PutWord(value.byteData,reg);
	}
	else
	{
		value.numBytes=destinationBytes;

		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);

		offset&=addressMask[addressSize>>5];
		switch(value.numBytes)
		{
		case 1:
			value.byteData[0]=FetchByte(addressSize,seg,offset,mem);
			break;
		case 2:
			value.SetWord(FetchWord(addressSize,seg,offset,mem));
			break;
		case 4:
			value.SetDword(FetchDword(addressSize,seg,offset,mem));
			break;
		default:
			for(unsigned int i=0; i<value.numBytes; ++i)
			{
				value.byteData[i]=FetchByte(addressSize,seg,offset+i,mem);
			}
			break;
		}
	}
	return value;
}

i486DX::OperandValue i486DX::EvaluateOperand8(
    Memory &mem,int addressSize,int segmentOverride,const Operand &op)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	i486DX::OperandValue value;
	value.numBytes=1;
	switch(op.operandType)
	{
	default:
		Abort("Tried to evaluate non 8-bit operand with EvaluateOperand8.");
		break;
	case OPER_ADDR:
		{
			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);
			offset&=addressMask[addressSize>>5];
			value.byteData[0]=FetchByte(addressSize,seg,offset,mem);
		}
		break;
	case OPER_REG8:
		value.byteData[0]=GetRegisterValue8(op.reg);
		break;
	}
	return value;
}

i486DX::OperandValue i486DX::EvaluateOperand64(
	    Memory &mem,int addressSize,int segmentOverride,const Operand &op)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	i486DX::OperandValue value;
	value.numBytes=0;
	switch(op.operandType)
	{
	default:
		Abort("Tried to evaluate 64-bit from an inappropriate operandType.");
		break;
	case OPER_ADDR:
		{
			value.numBytes=8;

			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);

			offset&=addressMask[addressSize>>5];

			cpputil::PutDword(value.byteData  ,FetchDword(addressSize,seg,offset,mem));
			cpputil::PutDword(value.byteData+4,FetchDword(addressSize,seg,offset+4,mem));
		}
		break;
	}
	return value;
}

i486DX::OperandValue i486DX::EvaluateOperand80(
	    Memory &mem,int addressSize,int segmentOverride,const Operand &op)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	i486DX::OperandValue value;
	value.numBytes=0;
	switch(op.operandType)
	{
	default:
		Abort("Tried to evaluate 64-bit from an inappropriate operandType.");
		break;
	case OPER_ADDR:
		{
			value.numBytes=8;

			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,op,segmentOverride);

			offset&=addressMask[addressSize>>5];

			cpputil::PutDword(value.byteData  ,FetchDword(addressSize,seg,offset,mem));
			cpputil::PutDword(value.byteData+4,FetchDword(addressSize,seg,offset+4,mem));
			cpputil::PutWord(value.byteData+8,FetchWord(addressSize,seg,offset+8,mem));
		}
		break;
	}
	return value;
}

void i486DX::StoreOperandValue(
    const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	switch(dst.operandType)
	{
	case OPER_UNDEFINED:
		Abort("Tried to evaluate an undefined operand.");
		break;
	case OPER_ADDR:
		{
			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);

			offset&=addressMask[addressSize>>5];
			switch(value.numBytes)
			{
			case 1:
				StoreByte(mem,addressSize,seg,offset,value.byteData[0]);
				break;
			case 2:
				StoreWord(mem,addressSize,seg,offset,cpputil::GetWord(value.byteData));// cpputil::GetWord is faster than using value.GetAsWord.
				break;
			case 4:
				StoreDword(mem,addressSize,seg,offset,cpputil::GetDword(value.byteData));// cpputil::GetWord is faster than using value.GetAsDword.
				break;
			default:
				for(unsigned int i=0; i<value.numBytes; ++i)
				{
					StoreByte(mem,addressSize,seg,offset+i,value.byteData[i]);
				}
				break;
			}
		}
		break;
	case OPER_FARADDR:
		Abort("Tried to evaluate FAR ADDRESS.");
		break;
	case OPER_REG32:
		{
			state.NULL_and_reg32[dst.reg]=cpputil::GetDword(value.byteData);
		}
		break;
	case OPER_REG16:
		{
			SET_INT_LOW_WORD(state.NULL_and_reg32[dst.reg&15],cpputil::GetWord(value.byteData));
		}
		break;
	case OPER_REG8:
		SetRegisterValue8(dst.reg,value.byteData[0]);
		break;
	case OPER_SREG:
		LoadSegmentRegister(state.sreg[dst.reg-REG_SEGMENT_REG_BASE],cpputil::GetWord(value.byteData),mem);
		break;

	case OPER_CR0:
	case OPER_CR1:
	case OPER_CR2:
	case OPER_CR3:
		SetCR(dst.operandType-OPER_CR0,cpputil::GetDword(value.byteData));
		break;
	case OPER_DR0:
	case OPER_DR1:
	case OPER_DR2:
	case OPER_DR3:
	case OPER_DR4:
	case OPER_DR5:
	case OPER_DR6:
	case OPER_DR7:
		state.DR[dst.operandType-OPER_DR0]=cpputil::GetDword(value.byteData);
		break;
	case OPER_TEST0:
	case OPER_TEST1:
	case OPER_TEST2:
	case OPER_TEST3:
	case OPER_TEST4:
	case OPER_TEST5:
	case OPER_TEST6:
	case OPER_TEST7:
		state.TEST[dst.operandType-OPER_TEST0]=cpputil::GetDword(value.byteData);
		break;
	default:
		#ifdef _WIN32
			__assume(0);
		#elif defined(__clang__) || defined(__GNUC__)
			__builtin_unreachable();
		#else
			break;
		#endif
	}
}

void i486DX::StoreOperandValueRegOrMem16(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,uint16_t value)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	if(OPER_REG32==dst.operandType)
	{
		state.NULL_and_reg32[dst.reg]=value;
	}
	else if(OPER_REG16==dst.operandType)
	{
		SET_INT_LOW_WORD(state.NULL_and_reg32[dst.reg&15],value);
	}
	else
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);

		offset&=addressMask[addressSize>>5];
		StoreWord(mem,addressSize,seg,offset,value);
	}
}
void i486DX::StoreOperandValueRegOrMem32(const Operand &dst,Memory &mem,int addressSize,int segmentOverride,uint32_t value)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	if(OPER_REG32==dst.operandType)
	{
		state.NULL_and_reg32[dst.reg]=value;
	}
	else if(OPER_REG16==dst.operandType)
	{
		SET_INT_LOW_WORD(state.NULL_and_reg32[dst.reg&15],value);
	}
	else
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);

		offset&=addressMask[addressSize>>5];
		StoreDword(mem,addressSize,seg,offset,value);
	}
}

void i486DX::StoreOperandValueReg16OrReg32OrMem(
    const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	if(OPER_REG32==dst.operandType)
	{
		state.NULL_and_reg32[dst.reg]=cpputil::GetDword(value.byteData);
	}
	else if(OPER_REG16==dst.operandType)
	{
		SET_INT_LOW_WORD(state.NULL_and_reg32[dst.reg&15],cpputil::GetWord(value.byteData));
	}
	else
	{
		unsigned int offset;
		const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);

		offset&=addressMask[addressSize>>5];
		switch(value.numBytes)
		{
		case 1:
			StoreByte(mem,addressSize,seg,offset,value.byteData[0]);
			break;
		case 2:
			StoreWord(mem,addressSize,seg,offset,cpputil::GetWord(value.byteData));// cpputil::GetWord is faster than using value.GetAsWord.
			break;
		case 4:
			StoreDword(mem,addressSize,seg,offset,cpputil::GetDword(value.byteData));// cpputil::GetWord is faster than using value.GetAsDword.
			break;
		default:
			for(unsigned int i=0; i<value.numBytes; ++i)
			{
				StoreByte(mem,addressSize,seg,offset+i,value.byteData[i]);
			}
			break;
		}
	}
}

void i486DX::StoreOperandValue8(
    const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	switch(dst.operandType)
	{
	default:
		Abort("Tried to store value to a non 8-bit operand with StoreOperandValue8.");
		break;
	case OPER_ADDR:
		{
			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);
			offset&=addressMask[addressSize>>5];
			StoreByte(mem,addressSize,seg,offset,value.byteData[0]);
		}
		break;
	case OPER_REG8:
		SetRegisterValue8(dst.reg,value.byteData[0]);
		break;
	}
}

void i486DX::StoreOperandValue64(
    const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	switch(dst.operandType)
	{
	default:
		Abort("Tried to store 64-bit value to a non-address operand.");
		break;
	case OPER_ADDR:
		{
			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);
			offset&=addressMask[addressSize>>5];

			StoreDword(mem,addressSize,seg,offset,  cpputil::GetDword(value.byteData));
			StoreDword(mem,addressSize,seg,offset+4,cpputil::GetDword(value.byteData+4));
		}
		break;
	}
}

void i486DX::StoreOperandValue80(
    const Operand &dst,Memory &mem,int addressSize,int segmentOverride,const OperandValue &value)
{
	static const unsigned int addressMask[2]=
	{
		0x0000FFFF,
		0xFFFFFFFF,
	};

	switch(dst.operandType)
	{
	default:
		Abort("Tried to store 64-bit value to a non-address operand.");
		break;
	case OPER_ADDR:
		{
			unsigned int offset;
			const SegmentRegister &seg=*ExtractSegmentAndOffset(offset,dst,segmentOverride);
			offset&=addressMask[addressSize>>5];

			StoreDword(mem,addressSize,seg,offset,  cpputil::GetDword(value.byteData));
			StoreDword(mem,addressSize,seg,offset+4,cpputil::GetDword(value.byteData+4));
			StoreWord(mem,addressSize,seg,offset+8,cpputil::GetWord(value.byteData+8));
		}
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
			clocksPassed+=5;
			return false;
		}
		--counter;
		SetCXorECX(addressSize,counter);
		clocksPassed+=7;
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

inline i486DX::CallStack i486DX::MakeCallStack(
	    bool isInterrupt,unsigned short INTNum,unsigned short AX,
	    unsigned int CR0,
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
	stk.INTNum0=0xffff;
	stk.AX0=0xffff;
	stk.fromCS=fromCS;
	stk.fromEIP=fromEIP;
	stk.callOpCodeLength=callOpCodeLength;
	stk.procCS=procCS;
	stk.procEIP=procEIP;
	return stk;
}
void i486DX::PushCallStack(
	    bool isInterrupt,unsigned short INTNum,unsigned short AX,
	    unsigned int CR0,
	    unsigned int fromCS,unsigned int fromEIP,unsigned int callOpCodeLength,
	    unsigned int procCS,unsigned int procEIP,
	    const Memory &mem)
{
	callStack.push_back(MakeCallStack(isInterrupt,INTNum,AX,CR0,fromCS,fromEIP,callOpCodeLength,procCS,procEIP));
	if(true==isInterrupt)
	{
		if(0x21==INTNum)
		{
			if((0x3D00==(AX&0xFF00) || 0x4B00==(AX&0xFF00)))
			{
				if(0==(CR0&1))  // Real Mode
				{
					callStack.back().str=DebugFetchString(16,state.DS(),GetDX(),mem);
				}
				else
				{
					callStack.back().str=DebugFetchString(32,state.DS(),GetEDX(),mem);
				}
			}
			if(nullptr!=int21HInterceptorPtr)
			{
				int21HInterceptorPtr->InterceptINT21H(GetAX(),callStack.back().str);
			}
		}
	}
}
void i486DX::PopCallStack(unsigned int CS,unsigned int EIP)
{
	if(true!=callStack.empty())
	{
		int nPop=1;
		bool match=false;
		for(auto iter=callStack.rbegin(); iter!=callStack.rend(); ++iter)
		{
			// Interrupt caused by an exception may return to the same CS:EIP that caused the exception,
			// or the exception handler may process what the instruction meant to do and return to the
			// next instruction.  Therefore, CS:EIP needs to be checked against return pointer, and
			// one instruction after the return pointer.

			// Windows 3.1 uses ARPL to take over control from VM86 mode, in which case, the return
			// address is +1 byte from ARPL.  However, ARPL instruction takes operands, therefore
			// the number of bytes is 2+ bytes.  Therefore, it should instead of checking two
			// return addresses, it should accept anywhere between the instruction address and
			// the instruction address plus op code length.  That also covers INT 20H in Windows 3.1
			// which returns to the address of the instruction.
			if(CS==iter->fromCS && (iter->fromEIP<=EIP && EIP<=iter->fromEIP+iter->callOpCodeLength))
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
	offset&=AddressMask((unsigned char)addressSize);
	auto addr=seg.baseLinearAddr+offset;
	if(true==PagingEnabled())
	{
		unsigned int type,code;
		addr=LinearAddressToPhysicalAddress(type,code,addr,mem);
	}
	auto returnValue=mem.FetchByte(addr);
	return returnValue;
}

/*! Fetch a dword.  It won't change exception status.
*/
unsigned int i486DX::DebugFetchWord(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	offset&=AddressMask((unsigned char)addressSize);
	auto addr=seg.baseLinearAddr+offset;
	if(true==PagingEnabled())
	{
		unsigned int type,code;
		addr=LinearAddressToPhysicalAddress(type,code,addr,mem);
		if(0xFFC<(addr&0xfff)) // May hit the page boundary
		{
			return DebugFetchByte(addressSize,seg,offset,mem)|(DebugFetchByte(addressSize,seg,offset+1,mem)<<8);
		}
	}
	auto returnValue=mem.FetchWord(addr);
	return returnValue;
}

/*! Fetch a dword for debugging.  It won't change exception status.
*/
unsigned int i486DX::DebugFetchDword(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	offset&=AddressMask((unsigned char)addressSize);
	auto addr=seg.baseLinearAddr+offset;
	if(true==PagingEnabled())
	{
		unsigned int type,code;
		addr=LinearAddressToPhysicalAddress(type,code,addr,mem);
		if(0xFF8<(addr&0xfff)) // May hit the page boundary
		{
			auto returnValue=
			     DebugFetchByte(addressSize,seg,offset,mem)
			   |(DebugFetchByte(addressSize,seg,offset+1,mem)<<8)
			   |(DebugFetchByte(addressSize,seg,offset+2,mem)<<16)
			   |(DebugFetchByte(addressSize,seg,offset+3,mem)<<24);
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
unsigned int i486DX::DebugFetchByteWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	return DebugFetchWordOrDword(operandSize,addressSize,seg,offset,mem);
}

/*! Fetch a byte by linear address for debugging.  It won't change exception status.
*/
unsigned int i486DX::DebugFetchByteByLinearAddress(const Memory &mem,unsigned int linearAddr) const
{
	if(true==PagingEnabled())
	{
		unsigned int type,code;
		linearAddr=LinearAddressToPhysicalAddress(type,code,linearAddr,mem);
	}
	auto returnValue=mem.FetchByte(linearAddr);
	return returnValue;
}

std::string i486DX::DebugFetchString(int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	std::string str;
	for(int i=0; i<255; ++i)
	{
		auto c=DebugFetchByte(addressSize,seg,offset++,mem);
		if(0==c)
		{
			break;
		}
		str.push_back(c);
	}
	return str;
}

/* static */ int i486DX::StrToReg(const std::string &regName)
{
	for(int i=0; i<REG_TOTAL_NUMBER_OF_REGISTERS; ++i)
	{
		if(regName==RegToStr[i])
		{
			return i;
		}
	}
	return REG_NULL;
}

void i486DX::DebugStoreByte(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned char byteData)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;
	auto physicalAddr=linearAddr;
	if(true==PagingEnabled())
	{
		unsigned int exceptionType,exceptionCode;
		physicalAddr=LinearAddressToPhysicalAddress(exceptionType,exceptionCode,linearAddr,mem);
	}
	return mem.StoreByte(physicalAddr,byteData);
}

void i486DX::DebugStoreWord(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;
	auto physicalAddr=linearAddr;
	if(true==PagingEnabled())
	{
		unsigned int exceptionType,exceptionCode;
		physicalAddr=LinearAddressToPhysicalAddress(exceptionType,exceptionCode,linearAddr,mem);
		if(0xFFE<(physicalAddr&0xfff)) // May hit the page boundary
		{
			StoreByte(mem,addressSize,seg,offset  , data    &255);// May hit the page boundary. Don't use StoreWord
			StoreByte(mem,addressSize,seg,offset+1,(data>>8)&255);// May hit the page boundary. Don't use StoreWord
			return;
		}
	}
	mem.StoreWord(physicalAddr,data);
}
void i486DX::DebugStoreDword(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;
	auto physicalAddr=linearAddr;
	if(true==PagingEnabled())
	{
		unsigned int exceptionType,exceptionCode;
		physicalAddr=LinearAddressToPhysicalAddress(exceptionType,exceptionCode,linearAddr,mem);
		if(0xFFC<(physicalAddr&0xfff)) // May hit the page boundary
		{
			StoreByte(mem,addressSize,seg,offset  , data     &255);
			StoreByte(mem,addressSize,seg,offset+1,(data>> 8)&255);// May hit the page boundary. Don't use StoreDword
			StoreByte(mem,addressSize,seg,offset+2,(data>>16)&255);// May hit the page boundary. Don't use StoreDword
			StoreByte(mem,addressSize,seg,offset+3,(data>>24)&255);// May hit the page boundary. Don't use StoreDword
			return;
		}
	}
	mem.StoreDword(physicalAddr,data);
}

bool i486DX::TestIOMapPermission(const SegmentRegister &TR,unsigned int ioMin,unsigned int accessSize,Memory &mem)
{
	unsigned int IOMapOffset0=FetchWord(32,TR,0x66,mem);
	for(auto ioport=ioMin; ioport<ioMin+accessSize; ++ioport)
	{
		unsigned int IOMapOffset=IOMapOffset0+(ioport>>3);
		unsigned int IOMapBit=(1<<(ioport&7));
		if(TR.limit<IOMapOffset)
		{
			return false;
		}
		if(0!=(FetchByte(32,TR,IOMapOffset,mem)&IOMapBit))
		{
			return false;
		}
	}
	return true;
}
bool i486DX::DebugTestIOMapPermission(const SegmentRegister &TR,unsigned int ioMin,unsigned int accessSize,const Memory &mem) const
{
	unsigned int IOMapOffset0=DebugFetchWord(32,TR,0x66,mem);
	for(auto ioport=ioMin; ioport<ioMin+accessSize; ++ioport)
	{
		unsigned int IOMapOffset=IOMapOffset0+(ioport>>3);
		unsigned int IOMapBit=(1<<(ioport&7));
		if(TR.limit<IOMapOffset)
		{
			return false;
		}
		if(0!=(DebugFetchByte(32,TR,IOMapOffset,mem)&IOMapBit))
		{
			return false;
		}
	}
	return true;
}

/* static */ std::string i486DX::ExceptionTypeToStr(unsigned int exceptionType)
{
	switch(exceptionType)
	{
	case EXCEPTION_NONE:
		return "NONE";
	case EXCEPTION_GP:
		return "GP";
	case EXCEPTION_ND:
		return "ND";
	case EXCEPTION_UD:
		return "UD";
	case EXCEPTION_SS:
		return "SS";
	case EXCEPTION_PF:
		return "PF";
	default:
		break;
	}
	return "?";
}
