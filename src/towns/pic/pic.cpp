/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "pic.h"
#include "towns.h"
#include "cpputil.h"



void TownsPIC::I8259A::Reset(void)
{
	IRR=0;
	ISR=0;
	IMR=0;
	for(auto &byte : ICW)
	{
		byte=0;
	}
	for(auto &byte : OCW)
	{
		byte=0;
	}
	highestPriorityInt=0;
	init_stage=0;
	SMM=false;
	autoRotateOnAEOI=false;
}
void TownsPIC::I8259A::WriteReg0(unsigned char data)
{
	if(0x10==(data&0x10))
	{
		ICW[0]=data;
		init_stage=1;
	}
	else if(0x00==(data&0x18))
	{
		OCW[1]=data; // OCW2
		unsigned char R_SL_EOI=((data>>5)&7);
		unsigned char L=data&7;
		switch(R_SL_EOI) // [2] pp.64,  i8259A Data Sheet pp.13
		{
		case 1:   // Issue Non-Specific EOI.  pp.15 of i8259A Data Sheet.  Clear IS flag of the highest priority INT.
			{
				auto intNum=GetHighestPriorityINTInService();
				ISR&=(~(1<<intNum));
			}
			break;
		case 3:   // Issue Specific EOI.  pp.15 of i8259A Data Sheet.  Clear IS flag of the INT specified in L.
			{
				ISR&=(~(1<<L));
			}
			break;

		case 5:   // Rotate on Non-Specific EOI Command
			{
				auto intNum=GetHighestPriorityINTInService();
				ISR&=(~(1<<intNum));
				highestPriorityInt=((intNum+1)&7);
			}
			break;
		case 4:   // Rotate in Automatic EOI Mode (SET)
			autoRotateOnAEOI=true;
			break;
		case 0:   // Rotate in Automatic EOI Mode (CLEAR)
			autoRotateOnAEOI=false;
			break;
		case 7:   // Rotate on Specific EOI Command
			{
				ISR&=(~(1<<L));
				highestPriorityInt=((L+1)&7);
			}
			break;

		case 6:   // Set Priority Command
			highestPriorityInt=((L+1)&7);
			break;

		case 2:   // NOP
			break;
		}
	}
	else if(0x08==(data&0x18))
	{
		OCW[2]=data;
		switch((data>>5)&3)
		{
		case 2:
			SMM=false;
			break;
		case 3:
			SMM=true;
			break;
		}
	}
}
bool TownsPIC::I8259A::WriteReg1(unsigned char data)
{
	if(0==init_stage)
	{
		OCW[0]=data;
		IMR=data;
		// Are OCR1 and IMR the same thing?  Probably.
	}
	else
	{
		ICW[init_stage++]=data;
		init_stage&=3;
		if(0==init_stage)
		{
			return true;
		}
	}
	return false;
}
bool TownsPIC::I8259A::PollingMode(void) const
{
	return (0!=(OCW[2]&4));
}
bool TownsPIC::I8259A::AutoEOIMode(void) const
{
	return (0!=(ICW[3]&2));
}
unsigned int TownsPIC::I8259A::TriggerMode(void) const
{
	// [2] pp.61 tells that in FM Towns, it must be fixed to Level-Trigger Mode.
	return (0!=(ICW1()&0x08) ? TRIGGER_LEVEL : TRIGGER_EDGE);
}
unsigned int TownsPIC::I8259A::GetHighestPriorityINTInService(void) const
{
	for(unsigned int pri=0; pri<8; ++pri)
	{
		auto INTNum=(highestPriorityInt+pri)&7;
		if(0!=(ISR&(1<<INTNum)))
		{
			return INTNum;
		}
	}
	return 0;
}
void TownsPIC::I8259A::SetInterruptRequestBit(unsigned int intNum,bool request)
{
	// [2] pp.58
	unsigned int bit=(1<<intNum);
	if(true==request)
	{
		if(0==(IRR&bit) && TriggerMode()==TRIGGER_EDGE)
		{
			// Try to fire an IRQ this point?
		}
		IRR|=bit;
	}
	else
	{
		IRR&=(~bit);
	}
}

unsigned int TownsPIC::I8259A::INTToGo(void) const
{
	for(unsigned int pri=0; pri<7; ++pri)
	{
		auto INTNum=(highestPriorityInt+pri)&7;
		if(0!=(IRR&(1<<INTNum)) && 0==(ISR&(1<<INTNum)))
		{
			return INTNum;
		}
	}
	return 0xffffffff;
}

void TownsPIC::I8259A::FireIRQ(i486DX &cpu,Memory &mem,unsigned int INTToGo)
{
	// What should I do in AEOI mode?
	// If I fire INT without raising ISR, in the next cycle it will re-fire an INT without CPU having a time to run CLI instruction.
	// It's going to run out of stack.  What will prevent INT from being fired in the next cycle?
	if(true==AutoEOIMode())
	{
		cpu.Abort("Still trying to figure out what to do with PIC AEOI mode.");
	}
	else
	{
		ISR|=(1<<INTToGo);
		auto CPUINT=(GetT()&0xF8)|(INTToGo&7);
		cpu.Interrupt(CPUINT,mem,0);
	}
}


////////////////////////////////////////////////////////////


void TownsPIC::State::Reset(void)
{
	for(auto &chip : i8259A)
	{
		chip.Reset();
	}
}


////////////////////////////////////////////////////////////


TownsPIC::TownsPIC(FMTowns *townsPtr)
{
	this->townsPtr=townsPtr;
	state.Reset();
	debugBreakOnICW1Write=false;
	debugBreakOnICW4Write=false;;
}

/* virtual */ void TownsPIC::PowerOn(void)
{
	state.Reset();
}
/* virtual */ void TownsPIC::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsPIC::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_PIC_PRIMARY_ICW1://          0x00,
		state.i8259A[0].WriteReg0(data);
		if(debugBreakOnICW1Write)
		{
			townsPtr->debugger.ExternalBreak("PIC Primary ICW1");
		}
		break;
	case TOWNSIO_PIC_PRIMARY_ICW2_3_4_OCW://  0x02,
		if(state.i8259A[0].WriteReg1(data) && debugBreakOnICW4Write)
		{
			townsPtr->debugger.ExternalBreak("PIC Primary ICW4");
		}
		break;
	case TOWNSIO_PIC_SECONDARY_ICW1://        0x10,
		state.i8259A[1].WriteReg0(data);
		if(debugBreakOnICW1Write)
		{
			townsPtr->debugger.ExternalBreak("PIC Secondary ICW1");
		}
		break;
	case TOWNSIO_PIC_SECONDARY_ICW2_3_4_OCW://0x12,
		if(state.i8259A[1].WriteReg1(data) && debugBreakOnICW4Write)
		{
			townsPtr->debugger.ExternalBreak("PIC Secondary ICW4");
		}
		break;
	}
}
/* virtual */ unsigned int TownsPIC::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_PIC_PRIMARY_ICW1://          0x00,
		switch(state.i8259A[0].OCW[2]&3) // [2] pp.65
		{
		case 2:
			return state.i8259A[0].IRR;
		case 3:
			return state.i8259A[0].ISR;
		}
		break;
	case TOWNSIO_PIC_PRIMARY_ICW2_3_4_OCW://  0x02,
		return state.i8259A[0].IMR;
	case TOWNSIO_PIC_SECONDARY_ICW1://        0x10,
		switch(state.i8259A[1].OCW[2]&3) // [2] pp.65
		{
		case 2:
			return state.i8259A[1].IRR;
		case 3:
			return state.i8259A[1].ISR;
		}
		break;
	case TOWNSIO_PIC_SECONDARY_ICW2_3_4_OCW://0x12,
		return state.i8259A[1].IMR;
	}
	return 0xFF;
}

void TownsPIC::SetInterruptRequestBit(unsigned int intNum,bool request)
{
	int unit=(0!=(intNum&0x08) ? 1 : 0);
	state.i8259A[unit].SetInterruptRequestBit(intNum&7,request);
}

std::vector <std::string> TownsPIC::GetStateText(void) const
{
	std::vector <std::string> text;
	for(int i=0; i<2; ++i)
	{
		const auto &i8259A=state.i8259A[i];

		std::string newline;

		text.push_back(newline);
		text.back()="PIC#";
		text.back().push_back('0'+i);

		text.push_back(newline);
		text.back()="A="+cpputil::Ustox(i8259A.GetA())+"  T="+cpputil::Ustox(i8259A.GetT());

		text.push_back(newline);
		text.back()="IRR="+cpputil::Ubtox(i8259A.IRR)+" ISR="+cpputil::Ubtox(i8259A.ISR);

		text.push_back(newline);
		text.back()="ICW1="+cpputil::Ubtox(i8259A.ICW[0]);
		text.back()=text.back()+"  LTIM:"+((i8259A.ICW[0]&0x08) ? "1" : "0");
		text.back()=text.back()+"  ADI:" +((i8259A.ICW[0]&0x04) ? "1" : "0");
		text.back()=text.back()+"  SNGL:"+((i8259A.ICW[0]&0x02) ? "1" : "0");
		text.back()=text.back()+"  IC4:" +((i8259A.ICW[0]&0x01) ? "1" : "0");

		text.push_back(newline);
		text.back()="ICW2="+cpputil::Ubtox(i8259A.ICW[1]);

		text.push_back(newline);
		text.back()="ICW3="+cpputil::Ubtox(i8259A.ICW[2]);

		text.push_back(newline);
		text.back()="ICW4="+cpputil::Ubtox(i8259A.ICW[3]);
		text.back()=text.back()+"  SFNM:"+((i8259A.ICW[3]&0x10) ? "1" : "0");
		text.back()=text.back()+"  BUF:" +((i8259A.ICW[3]&0x08) ? "1" : "0");
		text.back()=text.back()+"  M/S:" +((i8259A.ICW[3]&0x04) ? "1" : "0");
		text.back()=text.back()+"  AEOI:"+((i8259A.ICW[3]&0x02) ? "1" : "0");
		text.back()=text.back()+"  uPM:" +((i8259A.ICW[3]&0x01) ? "1" : "0");

		text.push_back(newline);
		text.back()="OCW1="+cpputil::Ubtox(i8259A.OCW[0]);

		text.push_back(newline);
		text.back()="OCW2="+cpputil::Ubtox(i8259A.OCW[1]);

		text.push_back(newline);
		text.back()="OCW3="+cpputil::Ubtox(i8259A.OCW[2]);
	}
	return text;
}

