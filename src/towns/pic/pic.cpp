#include "pic.h"
#include "towns.h"



void TownsPIC::I8259A::Reset(void)
{
	IRR_ISR=0;
	IMR=0;
	OCW=0;
	for(auto &byte : ICW)
	{
		byte=0;
	}
	init_stage=0;
}
void TownsPIC::I8259A::WriteReg0(unsigned char data)
{
	ICW[0]=data;
	init_stage=1;
}
bool TownsPIC::I8259A::WriteReg1(unsigned char data)
{
	if(0==init_stage)
	{
		OCW=data;
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
