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
#include <string.h> // for memcpy.

#include "cpputil.h"
#include "i486.h"


i486DXCommon::State::State()
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


const char *const i486DXCommon::Reg8Str[8]=
{
	"AL","CL","DL","BL","AH","CH","DH","BH"
};

const char *const i486DXCommon::Reg16Str[8]=
{
	"AX","CX","DX","BX","SP","BP","SI","DI"
};

const char *const i486DXCommon::Reg32Str[8]=
{
	"EAX","ECX","EDX","EBX","ESP","EBP","ESI","EDI"
};

const char *const i486DXCommon::Sreg[8]=
{
	"ES","CS","SS","DS","FS","GS"
};

const char *const i486DXCommon::RegToStr[REG_TOTAL_NUMBER_OF_REGISTERS]=
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

const bool i486DXCommon::ParityTable[256]=
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

std::string i486DXCommon::FarPointer::Format(void) const
{
	return cpputil::Uitox(SEG)+":"+cpputil::Uitox(OFFSET);
}

void i486DXCommon::FarPointer::MakeFromString(const std::string &str)
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
				this->SEG=i486DXCommon::FarPointer::SEG_REGISTER|i486DXCommon::REG_CS;
			}
			else if("SS"==segPart)
			{
				this->SEG=i486DXCommon::FarPointer::SEG_REGISTER|i486DXCommon::REG_SS;
			}
			else if("DS"==segPart)
			{
				this->SEG=i486DXCommon::FarPointer::SEG_REGISTER|i486DXCommon::REG_DS;
			}
			else if("ES"==segPart)
			{
				this->SEG=i486DXCommon::FarPointer::SEG_REGISTER|i486DXCommon::REG_ES;
			}
			else if("FS"==segPart)
			{
				this->SEG=i486DXCommon::FarPointer::SEG_REGISTER|i486DXCommon::REG_FS;
			}
			else if("GS"==segPart)
			{
				this->SEG=i486DXCommon::FarPointer::SEG_REGISTER|i486DXCommon::REG_GS;
			}
			else if("PHYS"==segPart || "P"==segPart)
			{
				this->SEG=i486DXCommon::FarPointer::PHYS_ADDR;
			}
			else if("LINE"==segPart || "L"==segPart)
			{
				this->SEG=i486DXCommon::FarPointer::LINEAR_ADDR;
			}
			else if('R'==segPart[0] || 'r'==segPart[0])
			{
				this->SEG=i486DXCommon::FarPointer::REAL_ADDR|cpputil::Xtoi(str.data()+1);
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

	this->SEG=i486DXCommon::FarPointer::NO_SEG;
	this->OFFSET=cpputil::Xtoi(str.data());
}

i486DXCommon::FarPointer i486DXCommon::TranslateFarPointer(FarPointer ptr) const
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

////////////////////////////////////////////////////////////


i486DXCommon::i486DXCommon(VMBase *vmPtr) : CPU(vmPtr)
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

void i486DXCommon::Reset(void)
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

void i486DXCommon::ClearPageTableCache(void)
{
	for(auto &c : state.pageTableCache)
	{
		c.valid=0;
		c.info.dir=0;
		c.info.table=0;
	}
	state.pageTableCacheValidCounter=1;
}

void i486DXCommon::InvalidatePageTableCache()
{
	++state.pageTableCacheValidCounter;
	if(state.pageTableCacheValidCounter==0xffffffff)
	{
		ClearPageTableCache();
	}
}

void i486DXCommon::ClearDescriptorCache(void)
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
void i486DXCommon::InvalidateDescriptorCache(void)
{
	++state.descriptorCacheValidCounter;
	if(0xffffffff==state.descriptorCacheValidCounter)
	{
		ClearDescriptorCache();
	}
}

std::vector <std::string> i486DXCommon::GetStateText(void) const
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

std::vector <std::string> i486DXCommon::GetSegRegText(void) const
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

std::vector <std::string> i486DXCommon::GetGDTText(const Memory &mem) const
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
		unsigned int DTPhysicalAddr=DebugLinearAddressToPhysicalAddress(excType,excCode,DTLinearBaseAddr,mem);
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

std::vector <std::string> i486DXCommon::GetLDTText(const Memory &mem) const
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
		unsigned int DTPhysicalAddr=DebugLinearAddressToPhysicalAddress(excType,excCode,DTLinearBaseAddr,mem);
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
		text.back()+=cpputil::Ubtox(rawDesc[5]&31);
		text.back()+="H ";
		text.back()+="@ PHYS:";
		text.back()+=cpputil::Uitox(DTPhysicalAddr);
		text.back()+="H";
	}
	return text;
}

std::vector <std::string> i486DXCommon::GetIDTText(const Memory &mem) const
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

std::vector <std::string> i486DXCommon::GetTSSText(const Memory &mem) const
{
	std::vector <std::string> text;

	unsigned int addrSize=32;

	text.push_back("");
	text.back()+="TR="+cpputil::Ustox(state.TR.value)+" TR Linear Base Addr="+cpputil::Uitox(state.TR.baseLinearAddr);

	if(DESCTYPE_AVAILABLE_386_TSS==state.TR.GetType() ||
	   DESCTYPE_BUSY_386_TSS==state.TR.GetType())
	{
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
	}
	else if(DESCTYPE_AVAILABLE_286_TSS==state.TR.GetType() ||
	        DESCTYPE_BUSY_286_TSS==state.TR.GetType())
	{
		text.push_back("");
		text.back()+="LINK(OLD TSS SELECTOR)="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,0,mem));

		text.push_back("");
		text.back()+="SS0:SP0="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,4,mem))+":"+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,2,mem));
		text.push_back("");
		text.back()+="SS1:SP1="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,8,mem))+":"+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,6,mem));
		text.push_back("");
		text.back()+="SS1:SP1="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,12,mem))+":"+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,10,mem));

		text.push_back("");
		text.back()+="IP="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,14,mem));
		text.push_back("");
		text.back()+="FLAGS="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,16,mem));
		text.push_back("");
		text.back()+="AX="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,18,mem));
		text.push_back("");
		text.back()+="CX="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,20,mem));
		text.push_back("");
		text.back()+="DX="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,22,mem));
		text.push_back("");
		text.back()+="BX="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,24,mem));
		text.push_back("");
		text.back()+="SP="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,26,mem));
		text.push_back("");
		text.back()+="BP="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,28,mem));
		text.push_back("");
		text.back()+="SI="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,30,mem));
		text.push_back("");
		text.back()+="DI="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,32,mem));
		text.push_back("");
		text.back()+="ES="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,34,mem));
		text.push_back("");
		text.back()+="CS="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,36,mem));
		text.push_back("");
		text.back()+="SS="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,38,mem));
		text.push_back("");
		text.back()+="DS="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,40,mem));
		text.push_back("");
		text.back()+="LDTR="+cpputil::Ustox(DebugFetchWord(addrSize,state.TR,42,mem));
	}
	else
	{
		text.push_back("TR is not pointing to a task-state segment.");
	}


	return text;
}

std::vector <std::string> i486DXCommon::GetDRText(void) const
{
	std::vector <std::string> text;
	int i=0;
	for(auto DR : state.DR)
	{
		if(0==i%4)
		{
			text.push_back("");
		}
		else
		{
			text.back()+="  ";
		}
		text.back()+="DR";
		text.back().push_back('0'+i);
		text.back()+="="+cpputil::Uitox(DR);
		++i;
	}
	return text;
}
std::vector <std::string> i486DXCommon::GetTESTText(void) const
{
	std::vector <std::string> text;
	int i=0;
	for(auto TEST : state.TEST)
	{
		if(0==i%4)
		{
			text.push_back("");
		}
		else
		{
			text.back()+="  ";
		}
		text.back()+="TEST";
		text.back().push_back('0'+i);
		text.back()+="="+cpputil::Uitox(TEST);
		++i;
	}
	return text;
}

void i486DXCommon::PrintState(void) const
{
	for(auto &str : GetStateText())
	{
		std::cout << str << std::endl;
		if(nullptr!=debuggerPtr)
		{
			debuggerPtr->WriteLogFile(str);
		}
	}
}

void i486DXCommon::PrintGDT(const Memory &mem) const
{
	for(auto &str : GetGDTText(mem))
	{
		std::cout << str << std::endl;
		if(nullptr!=debuggerPtr)
		{
			debuggerPtr->WriteLogFile(str);
		}
	}
}

void i486DXCommon::PrintLDT(const Memory &mem) const
{
	for(auto &str : GetLDTText(mem))
	{
		std::cout << str << std::endl;
		if(nullptr!=debuggerPtr)
		{
			debuggerPtr->WriteLogFile(str);
		}
	}
}

void i486DXCommon::PrintIDT(const Memory &mem) const
{
	for(auto &str : GetIDTText(mem))
	{
		std::cout << str << std::endl;
		if(nullptr!=debuggerPtr)
		{
			debuggerPtr->WriteLogFile(str);
		}
	}
}

void i486DXCommon::PrintPageTranslation(const Memory &mem,uint32_t linearAddr) const
{
	for(int i=0; i<2; ++i)
	{
		if(1==i && (nullptr==debuggerPtr || true!=debuggerPtr->LogFileStream().is_open()))
		{
			break;
		}

		std::ostream &ofs=(0==i ? std::cout : debuggerPtr->LogFileStream());

		auto pageIndex=(linearAddr>>LINEARADDR_TO_PAGE_SHIFT);

		ofs << "LINE:" << cpputil::Uitox(linearAddr) << "H" << std::endl;

		auto pageInfo=state.pageTableCache[pageIndex].info;
		if(state.pageTableCache[pageIndex].valid<state.pageTableCacheValidCounter)
		{
			ofs << "Page Info Not Cached" << std::endl;
		}
		else
		{
			ofs << "Cached Page Info:" << cpputil::Uitox(pageInfo.table) << "H" << std::endl;
			if(0!=(pageInfo.table&PAGEINFO_FLAG_PRESENT))
			{
				auto offset=(linearAddr&4095);
				auto physicalAddr=(pageInfo.table&0xFFFFF000)+offset;
				ofs << "Cache PHYS:" << cpputil::Uitox(physicalAddr) << "H" << std::endl;
			}
			else
			{
				ofs << "Cached Page Not Present" << std::endl;
			}
		}

		uint32_t pageDirectoryIndex=((linearAddr>>22)&1023);
		uint32_t pageTableIndex=((linearAddr>>12)&1023);

		auto pageDirectoryPtr=state.GetCR(3)&0xFFFFF000;
		auto pageTableInfo=mem.FetchDword(pageDirectoryPtr+(pageDirectoryIndex<<2));

		ofs << "Page Directory Index  :" << cpputil::Uitox(pageDirectoryIndex) << "H" << std::endl;
		ofs << "Page Directory Pointer:" << cpputil::Uitox(pageDirectoryPtr) << "H" << std::endl;
		ofs << "Page Directory Info   :" << cpputil::Uitox(pageTableInfo) << "H" << std::endl;
		if(0==(pageTableInfo&1))
		{
			ofs << "Page Table Not Present" << std::endl;
		}
		else
		{
			const uint32_t pageTablePtr=(pageTableInfo&0xFFFFF000);
			const uint32_t pagePtr=pageTablePtr+(pageTableIndex<<2);
			unsigned int pageInfo=mem.FetchDword(pagePtr);
			ofs << "Page Table Index  :" << cpputil::Uitox(pageTableIndex) << "H" << std::endl;
			ofs << "Page Table Pointer:" << cpputil::Uitox(pageTablePtr) << "H" << std::endl;
			ofs << "Page Pointer      :" << cpputil::Uitox(pagePtr) << "H" << std::endl;
			ofs << "Page Table Info   :" << cpputil::Uitox(pageInfo) << "H" << std::endl;
			if(0==(pageInfo&1))
			{
				ofs << "Page Not Present" << std::endl;
			}
			else
			{
				auto offset=(linearAddr&4095);
				auto physicalAddr=(pageInfo&0xFFFFF000)+offset;
				ofs << "PHYS:" << cpputil::Uitox(physicalAddr) << "H" << std::endl;
			}
		}
	}
}

void i486DXCommon::LoadSegmentRegisterRealMode(SegmentRegister &reg,unsigned int value)
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

void i486DXCommon::LoadDescriptorTableRegister(SystemAddressRegister &reg,int operandSize,const unsigned char byteData[])
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

i486DXCommon::InterruptDescriptor i486DXCommon::DebugGetInterruptDescriptor(unsigned int INTNum,const Memory &mem) const
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

i486DXCommon::OperandValue i486DXCommon::DescriptorTableToOperandValue(const SystemAddressRegister &reg,int operandSize) const
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

/* static */ unsigned int i486DXCommon::GetRegisterSize(int reg)
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

unsigned int i486DXCommon::PhysicalAddressToLinearAddress(unsigned physAddr,const Memory &mem) const
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

std::string i486DXCommon::Disassemble(const Instruction &inst,const Operand &op1,const Operand &op2,SegmentRegister seg,unsigned int offset,const Memory &mem,const class i486SymbolTable &symTable,const std::map <unsigned int,std::string> &ioTable) const
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

std::string i486DXCommon::DisassembleData(unsigned int addressSize,SegmentRegister seg,unsigned int offset,const Memory &mem,unsigned int unitBytes,unsigned int segBytes,unsigned int repeat,unsigned int chopOff) const
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

// OF SF ZF AF PF
void i486DXCommon::DecrementWordOrDword(unsigned int operandSize,unsigned int &value)
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
void i486DXCommon::DecrementDword(unsigned int &value)
{
	--value;
	SetOF(value==0x7FFFFFFF);
	SetSF(0!=(value&0x80000000));
	SetZF(0==value);
	SetAF(0x0F==(value&0x0F));
	SetPF(CheckParity(value));
}
void i486DXCommon::DecrementWord(unsigned int &value)
{
	value=((value-1)&0xFFFF);
	SetOF(value==0x7FFF);
	SetSF(0!=(value&0x8000));
	SetZF(0==value);
	SetAF(0x0F==(value&0x0F));
	SetPF(CheckParity(value));
}
void i486DXCommon::DecrementByte(unsigned int &value)
{
	value=((value-1)&0xFF);
	SetOF(value==0x7F);
	SetSF(0!=(value&0x80));
	SetZF(0==value);
	SetAF(0x0F==(value&0x0F));
	SetPF(CheckParity(value));
}
void i486DXCommon::DecrementWithMask(unsigned int &value,unsigned int mask,unsigned int signBit)
{
	value=((value-1)&mask);
	SetOF(signBit-1==value);
	SetAF(0x0F==(value&0x0F));
	SetSF(value&signBit);
	SetZF(0==value);
	SetPF(CheckParity(value));
}

void i486DXCommon::IncrementWordOrDword(unsigned int operandSize,unsigned int &value)
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
void i486DXCommon::IncrementDword(unsigned int &value)
{
	SetAF(0x0F==(value&0x0F));
	++value;
	SetOF(value==0x80000000);
	SetSF(0!=(value&0x80000000));
	SetZF(0==value);
	SetPF(CheckParity(value));
}
void i486DXCommon::IncrementWord(unsigned int &value)
{
	SetAF(0x0F==(value&0x0F));
	value=(value+1)&0xffff;
	SetOF(value==0x8000);
	SetSF(0!=(value&0x8000));
	SetZF(0==value);
	SetPF(CheckParity(value));
}
void i486DXCommon::IncrementByte(unsigned int &value)
{
	SetAF(0x0F==(value&0x0F));
	value=(value+1)&0xff;
	SetOF(value==0x80);
	SetSF(0!=(value&0x80));
	SetZF(0==value);
	SetPF(CheckParity(value));
}
void i486DXCommon::IncrementWithMask(unsigned int &value,unsigned int mask,unsigned int signBit)
{
	SetAF(0x0F==(value&0x0F));
	value=(value+1)&mask;
	SetOF(value==signBit);
	SetSF(value&signBit);
	SetZF(0==value);
	SetPF(CheckParity(value));
}



void i486DXCommon::AddWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
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
void i486DXCommon::AddDword(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::AddWord(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::AddByte(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::AndWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
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
void i486DXCommon::AndDword(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::AndWord(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::AndByte(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::SubByteWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
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
void i486DXCommon::SubWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
{
	SubByteWordOrDword(operandSize,value1,value2);
}
void i486DXCommon::SubDword(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::SubWord(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::SubByte(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::AdcWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
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
void i486DXCommon::AdcDword(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::AdcWord(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::AdcByte(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::SbbWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
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
void i486DXCommon::SbbDword(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::SbbWord(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::SbbByte(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::OrWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
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
void i486DXCommon::OrDword(unsigned int &value1,unsigned int value2)
{
	ClearCFOF();
	//SetCF(false);
	//SetOF(false);
	value1|=value2;
	SetSF(0!=(0x80000000&value1));
	SetZF(0==value1);
	SetPF(CheckParity(value1));
}
void i486DXCommon::OrWord(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::OrByte(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::XorWordOrDword(int operandSize,unsigned int &value1,unsigned int value2)
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
void i486DXCommon::XorDword(unsigned int &value1,unsigned int value2)
{
	ClearCFOF();
	//SetCF(false);
	//SetOF(false);
	value1^=value2;
	SetSF(0!=(0x80000000&value1));
	SetZF(0==value1);
	SetPF(CheckParity(value1));
}
void i486DXCommon::XorWord(unsigned int &value1,unsigned int value2)
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
void i486DXCommon::XorByte(unsigned int &value1,unsigned int value2)
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
inline void i486DXCommon::RolTemplate(unsigned int &value, unsigned int c) {

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

void i486DXCommon::RolByteWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
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

void i486DXCommon::RolDword(unsigned int &value, unsigned int ctr)
{
	RolTemplate<uint32_t>(value, ctr);
}

void i486DXCommon::RolWord(unsigned int &value, unsigned int ctr)
{
	RolTemplate<uint16_t>(value, ctr);
}

void i486DXCommon::RolByte(unsigned int &value, unsigned int ctr)
{
	RolTemplate<uint8_t>(value, ctr);
}

void i486DXCommon::RorByteWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
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
inline void i486DXCommon::RorTemplate(unsigned int &value, unsigned int c) {

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

void i486DXCommon::RorDword(unsigned int &value, unsigned int ctr)
{
	RorTemplate<uint32_t>(value, ctr);
}

void i486DXCommon::RorWord(unsigned int &value, unsigned int ctr)
{
	RorTemplate<uint16_t>(value, ctr);
}

void i486DXCommon::RorByte(unsigned int &value, unsigned int ctr)
{
	RorTemplate<uint8_t>(value, ctr);
}

void i486DXCommon::RclWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
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
void i486DXCommon::RclDword(unsigned int &value,unsigned int ctr)
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
void i486DXCommon::RclWord(unsigned int &value,unsigned int ctr)
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
void i486DXCommon::RclByte(unsigned int &value,unsigned int ctr)
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

void i486DXCommon::RcrWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
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
void i486DXCommon::RcrDword(unsigned int &value,unsigned int ctr)
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
void i486DXCommon::RcrWord(unsigned int &value,unsigned int ctr)
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
void i486DXCommon::RcrByte(unsigned int &value,unsigned int ctr)
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

void i486DXCommon::SarByteWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
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
void i486DXCommon::SarDword(unsigned int &value,unsigned int ctr)
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
void i486DXCommon::SarWord(unsigned int &value,unsigned int ctr)
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
void i486DXCommon::SarByte(unsigned int &value,unsigned int ctr)
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
void i486DXCommon::ShlTemplate(unsigned int &value,unsigned int ctr)
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
void i486DXCommon::ShlWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
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
void i486DXCommon::ShlDword(unsigned int &value,unsigned int ctr)
{
	ShlTemplate<32,0xFFFFFFFF,0x80000000>(value,ctr);
}
void i486DXCommon::ShlWord(unsigned int &value,unsigned int ctr)
{
	ShlTemplate<16,0xFFFF,0x8000>(value,ctr);
}
void i486DXCommon::ShlByte(unsigned int &value,unsigned int ctr)
{
	ShlTemplate<8,0xFF,0x80>(value,ctr);
}

template <unsigned int bitCount,unsigned int maskBits,unsigned int signBit>
inline void i486DXCommon::ShrTemplate(unsigned int &value,unsigned int ctr)
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
void i486DXCommon::ShrWordOrDword(int operandSize,unsigned int &value,unsigned int ctr)
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
void i486DXCommon::ShrDword(unsigned int &value,unsigned int ctr)
{
	ShrTemplate<32,0xffffffff,0x80000000>(value,ctr);
}
void i486DXCommon::ShrWord(unsigned int &value,unsigned int ctr)
{
	ShrTemplate<16,0xFFFF,0x8000>(value,ctr);
}
void i486DXCommon::ShrByte(unsigned int &value,unsigned int ctr)
{
	ShrTemplate<8,0xFF,0x80>(value,ctr);
}

bool i486DXCommon::REPCheck(unsigned int &clocksPassed,unsigned int instPrefix,unsigned int addressSize)
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
		clocksPassed+=3;
	}
	return true;
}

bool i486DXCommon::REPEorNECheck(unsigned int &clocksForRep,unsigned int instPrefix,unsigned int addressSize)
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

inline i486DXCommon::CallStack i486DXCommon::MakeCallStack(
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
void i486DXCommon::PushCallStack(
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
				if(true==IsInRealMode() || true==GetVM())
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
	if(nullptr!=debuggerPtr && debuggerPtr->breakOnCallStackDepth<=callStack.size())
	{
		debuggerPtr->ExternalBreak("Call Stack Depth Exceeds Threshold.");
	}
}
void i486DXCommon::PopCallStack(unsigned int CS,unsigned int EIP)
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
void i486DXCommon::AttachDebugger(i486Debugger *debugger)
{
	this->debuggerPtr=debugger;
}
void i486DXCommon::DetachDebugger(void)
{
	debuggerPtr=nullptr;
}

/*! Fetch a byte for debugger.  It won't change exception status.
*/
unsigned int i486DXCommon::DebugFetchByte(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	offset&=AddressMask((unsigned char)addressSize);
	auto addr=seg.baseLinearAddr+offset;
	if(true==PagingEnabled())
	{
		unsigned int type,code;
		addr=DebugLinearAddressToPhysicalAddress(type,code,addr,mem);
	}
	auto returnValue=mem.FetchByte(addr);
	return returnValue;
}

/*! Fetch a dword.  It won't change exception status.
*/
unsigned int i486DXCommon::DebugFetchWord(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	offset&=AddressMask((unsigned char)addressSize);
	auto addr=seg.baseLinearAddr+offset;
	if(true==PagingEnabled())
	{
		unsigned int type,code;
		addr=DebugLinearAddressToPhysicalAddress(type,code,addr,mem);
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
unsigned int i486DXCommon::DebugFetchDword(unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	offset&=AddressMask((unsigned char)addressSize);
	auto addr=seg.baseLinearAddr+offset;
	if(true==PagingEnabled())
	{
		unsigned int type,code;
		addr=DebugLinearAddressToPhysicalAddress(type,code,addr,mem);
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
unsigned int i486DXCommon::DebugFetchWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
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
unsigned int i486DXCommon::DebugFetchByteWordOrDword(unsigned int operandSize,unsigned int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	return DebugFetchWordOrDword(operandSize,addressSize,seg,offset,mem);
}

/*! Fetch a byte by linear address for debugging.  It won't change exception status.
*/
unsigned int i486DXCommon::DebugFetchByteByLinearAddress(const Memory &mem,unsigned int linearAddr) const
{
	if(true==PagingEnabled())
	{
		unsigned int type,code;
		linearAddr=DebugLinearAddressToPhysicalAddress(type,code,linearAddr,mem);
	}
	auto returnValue=mem.FetchByte(linearAddr);
	return returnValue;
}

std::string i486DXCommon::DebugFetchString(int addressSize,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
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

/* static */ int i486DXCommon::StrToReg(const std::string &regName)
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

void i486DXCommon::DebugStoreByte(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned char byteData)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;
	auto physicalAddr=linearAddr;
	if(true==PagingEnabled())
	{
		unsigned int exceptionType,exceptionCode;
		physicalAddr=DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,linearAddr,mem);
	}
	return mem.StoreByte(physicalAddr,byteData);
}

void i486DXCommon::DebugStoreWord(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;
	auto physicalAddr=linearAddr;
	if(true==PagingEnabled())
	{
		unsigned int exceptionType,exceptionCode;
		physicalAddr=DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,linearAddr,mem);
		if(0xFFE<(physicalAddr&0xfff)) // May hit the page boundary
		{
			DebugStoreByte(mem,addressSize,seg,offset  , data    &255);// May hit the page boundary. Don't use StoreWord
			DebugStoreByte(mem,addressSize,seg,offset+1,(data>>8)&255);// May hit the page boundary. Don't use StoreWord
			return;
		}
	}
	mem.StoreWord(physicalAddr,data);
}
void i486DXCommon::DebugStoreDword(Memory &mem,int addressSize,SegmentRegister seg,unsigned int offset,unsigned int data)
{
	offset&=AddressMask((unsigned char)addressSize);
	auto linearAddr=seg.baseLinearAddr+offset;
	auto physicalAddr=linearAddr;
	if(true==PagingEnabled())
	{
		unsigned int exceptionType,exceptionCode;
		physicalAddr=DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,linearAddr,mem);
		if(0xFFC<(physicalAddr&0xfff)) // May hit the page boundary
		{
			DebugStoreByte(mem,addressSize,seg,offset  , data     &255);
			DebugStoreByte(mem,addressSize,seg,offset+1,(data>> 8)&255);// May hit the page boundary. Don't use StoreDword
			DebugStoreByte(mem,addressSize,seg,offset+2,(data>>16)&255);// May hit the page boundary. Don't use StoreDword
			DebugStoreByte(mem,addressSize,seg,offset+3,(data>>24)&255);// May hit the page boundary. Don't use StoreDword
			return;
		}
	}
	mem.StoreDword(physicalAddr,data);
}

bool i486DXCommon::DebugTestIOMapPermission(const SegmentRegister &TR,unsigned int ioMin,unsigned int accessSize,const Memory &mem) const
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

/* static */ std::string i486DXCommon::ExceptionTypeToStr(unsigned int exceptionType)
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
