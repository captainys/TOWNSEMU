#include "pic.h"
#include "towns.h"
#include "cpputil.h"



void TownsPIC::I8259A::Reset(void)
{
	IRR_ISR=0;
	IMR=0;
	for(auto &byte : ICW)
	{
		byte=0;
	}
	for(auto &byte : OCW)
	{
		byte=0;
	}
	init_stage=0;
}
void TownsPIC::I8259A::WriteReg0(unsigned char data)
{
	if(0!=(data&0x10))
	{
		ICW[0]=data;
		init_stage=1;
	}
	else if(0==(data&0x18))
	{
		OCW[1]=data;
	}
	else if(0!=(data&0x18))
	{
		OCW[2]=data;
	}
}
bool TownsPIC::I8259A::WriteReg1(unsigned char data)
{
	if(0==init_stage)
	{
		OCW[0]=data;
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
		return state.i8259A[0].IRR_ISR;
	case TOWNSIO_PIC_PRIMARY_ICW2_3_4_OCW://  0x02,
		return state.i8259A[0].IMR;
	case TOWNSIO_PIC_SECONDARY_ICW1://        0x10,
		return state.i8259A[1].IRR_ISR;
	case TOWNSIO_PIC_SECONDARY_ICW2_3_4_OCW://0x12,
		return state.i8259A[1].IMR;
	}
	return 0xFF;
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
