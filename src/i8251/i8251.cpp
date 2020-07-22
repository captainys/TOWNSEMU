#include <iostream>

#include "i8251.h"



i8251::i8251()
{
	Reset();
}
void i8251::Reset(void)
{
	state.immediatelyAfterReset=true;
	state.stopBits=1;
	state.preScale=64;

	state.SCS=false;
	state.ESD=false;
	state.dataLength=8;
	state.evenParity=true;
	state.parityEnabled=false;

	state.RxEN=false;
	state.TxEN=false;
	state.SYNDET=false;
	state.FE=false;
	state.OE=false;
	state.PE=false;
	state.TxEMPTY=false;
	state.RTS=false;
	state.DTR=false;

	state.lastTxTime=0;
	state.lastRxTime=0;
}
void i8251::VMWriteCommnand(unsigned char data)
{
	if(true==state.immediatelyAfterReset)
	{
		state.immediatelyAfterReset=false;
		state.preScale=((data&MODESEL_B)>>MODESEL_B_SHIFT);    // B1, B2

		state.parityEnabled=(0!=(data&MODESEL_PEN));   // PEN
		state.evenParity=(0!=(data&MODESEL_EP));               // EP
		state.dataLength=5+((data&MODESEL_L)>>MODESEL_L_SHIFT);// L1, L2

		if(0==state.preScale) // Synchronous mode
		{
			state.SCS=(0!=(data&MODESEL_SYNC_SCS));
			state.ESD=(0!=(data&MODESEL_SYNC_ESD));
			if(nullptr!=clientPtr)
			{
				clientPtr->SetParity(state.parityEnabled,state.evenParity);
				clientPtr->SetDataLength(state.dataLength);
			}
		}
		else // Asynchronous mode
		{
			state.stopBits=((data&MODESEL_ASYNC_S)>>MODESEL_ASYNC_S_SHIFT);   // S1,S2
			if(nullptr!=clientPtr)
			{
				clientPtr->SetStopBits(state.stopBits);
				clientPtr->SetParity(state.parityEnabled,state.evenParity);
				clientPtr->SetDataLength(state.dataLength);
			}
		}
	}
	else
	{
		if(0!=(data&CMD_WRITE_RESET))
		{
			Reset();
		}
		else
		{
			state.TxEN=(0!=(data&CMD_WRITE_TXEN));
			state.DTR=(0!=(data&CMD_WRITE_DTR));
			state.RxEN=(0!=(data&CMD_WRITE_RXEN));
			state.BREAK=(0!=(data&CMD_WRITE_SBRK));
			if(0!=(data&CMD_WRITE_CLRER))
			{
				state.FE=false;
				state.OE=false;
				state.PE=false;
			}
			state.RTS=(0!=(data&CMD_WRITE_RTS));

			if(nullptr!=clientPtr)
			{
				clientPtr->Command(state.RTS,state.DTR,state.BREAK);
			}

			// Disregard EH.
		}
	}
}
unsigned char i8251::VMReadState(void) const
{
	unsigned char data=0;

	data|=(TxRDY() ? STATUS_READ_TXRDY : 0);
	data|=(RxRDY() ? STATUS_READ_RXRDY : 0);
	data|=(state.TxEMPTY ? STATUS_READ_TXEMPTY : 0);
	data|=(state.PE ? STATUS_READ_PE : 0);
	data|=(state.OE ? STATUS_READ_OE : 0);
	data|=(state.FE ? STATUS_READ_FE : 0);
	data|=(state.SYNDET ? STATUS_READ_SYNDET : 0);
	data|=(DSR() ? STATUS_READ_DSR : 0);

	return data;
}
void i8251::VMWriteData(unsigned char data,long long int VMTime)
{
	state.lastTxTime=VMTime;
	state.TxEMPTY=false;
	state.TxRDY=false;
	if(nullptr!=clientPtr)
	{
		clientPtr->Tx(data);
	}
}
unsigned char i8251::VMReadData(long long int VMTime)
{
	state.lastRxTime=VMTime;
	state.RxRDY=false;
	if(nullptr!=clientPtr)
	{
		return clientPtr->Rx();
	}
	return 0xFF;
}
void i8251::Update(long long int newVMTime)
{
	if(true==state.TxEN && state.lastTxTime+state.nanoSecondsPerByte<newVMTime)
	{
		state.TxRDY=true;
		state.TxEMPTY=true;
	}
	if(true==state.RxEN && state.lastRxTime+state.nanoSecondsPerByte<newVMTime)
	{
		state.RxRDY=true;
	}
}
bool i8251::RxRDY(void) const
{
	if(nullptr!=clientPtr)
	{
		return state.RxRDY && clientPtr->RxRDY();
	}
	return false;
}
bool i8251::TxRDY(void) const
{
	if(nullptr!=clientPtr)
	{
		return state.TxRDY && clientPtr->TxRDY();
	}
	return false;
}
bool i8251::TxEMPTY(void) const
{
	return state.TxEMPTY;
}
bool i8251::DSR(void) const
{
	if(nullptr!=clientPtr)
	{
		return clientPtr->DSR();
	}
	return false;
}
bool i8251::SYNDET(void) const
{
	return state.SYNDET;
}
