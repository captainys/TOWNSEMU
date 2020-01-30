#include <iostream>
#include "cdrom.h"
#include "townsdef.h"
#include "cpputil.h"



void TownsCDROM::State::Reset(void)
{
	// Do not clear imgFileName on reset.
	ResetMPU();
	enableSIRQ=false;
	enableDEI=false;
}

void TownsCDROM::State::ResetMPU(void)
{
	SIRQ=false; // 4C0H bit 7
	DEI=false;  // 4C0H bit 6 (DMA End Flag)
	STSF=false; // 4C0H bit 5 (Software Transfer End)
	DTSF=false; // 4C0H bit 4 (DMA Transfer in progress)
	DRY=true;   // 4C0H bit 0 (Ready to receive command)

	cmdReceived=false;
	nParamQueue=0;
	nStatusQueue=0;
	DMATransfer=false;
	CPUTransfer=true;
}

////////////////////////////////////////////////////////////

TownsCDROM::TownsCDROM()
{
	state.Reset();
}

/* virtual */ void TownsCDROM::PowerOn(void)
{
	state.Reset();
}
/* virtual */ void TownsCDROM::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsCDROM::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_CDROM_MASTER_CTRL_STATUS://0x4C0, // [2] pp.224
		if(0!=(data&0x80)) // SMIC: Clear SIRQ
		{
			state.SIRQ=false;
		}
		if(0!=(data&0x40)) // DEIC: Clera DMA-End IRQ
		{
			state.DEI=false;
		}
		if(0!=(data&0x04))
		{
			state.ResetMPU();
		}
		state.enableSIRQ=(0!=(data&0x02));
		state.enableDEI=(0!=(data&0x01));
		break;
	case TOWNSIO_CDROM_COMMAND_STATUS://    0x4C2, // [2] pp.224
		std::cout << "CDROM Command " << cpputil::Ubtox(data) << std::endl;
		state.cmdReceived=true;
		state.cmd=data;
		break;
	case TOWNSIO_CDROM_PARAMETER_DATA://    0x4C4, // [2] pp.224
		if(8<=state.nParamQueue)
		{
			for(int i=0; i<PARAM_QUEUE_LEN-1; ++i)
			{
				state.paramQueue[i]=state.paramQueue[i+1];
			}
			state.nParamQueue=PARAM_QUEUE_LEN-1;
		}
		state.paramQueue[state.nParamQueue++]=data;
		break;
	case TOWNSIO_CDROM_TRANSFER_CTRL://     0x4C6, // [2] pp.227
		state.DMATransfer=(0!=(data&0x10));
		state.CPUTransfer=(0!=(data&0x08));
		break;
	case TOWNSIO_CDROM_SUBCODE_STATUS://    0x4CC,
		// No write
		break;
	case TOWNSIO_CDROM_SUBCODE_DATA://      0x4CD,
		// No write
		break;
	}
	if(true==state.cmdReceived && PARAM_QUEUE_LEN<=state.nParamQueue)
	{
		std::cout << "Exec CDROM Command not implemented yet." << std::endl;
		state.cmdReceived=false;
		state.nParamQueue=0;
		state.nStatusQueue=0;

		// Tentatively Drive Not Ready.
		state.nStatusQueue=4;
		state.statusQueue[0]=0x21;
		state.statusQueue[1]=0x07;
		state.statusQueue[2]=0;
		state.statusQueue[3]=0;
	}
}
/* virtual */ unsigned int TownsCDROM::IOReadByte(unsigned int ioport)
{
	unsigned char data=0;
	switch(ioport)
	{
	case TOWNSIO_CDROM_MASTER_CTRL_STATUS://0x4C0, // [2] pp.224
		data|=(state.SIRQ ? 0x80 : 0);
		data|=(state.DEI  ? 0x40 : 0);
		data|=(state.STSF ? 0x20 : 0);
		data|=(state.DTSF ? 0x10 : 0);
		data|=(0<state.nStatusQueue ? 2 : 0);
		data|=(state.DRY  ?    1 : 0);
		return data;
	case TOWNSIO_CDROM_COMMAND_STATUS://    0x4C2, // [2] pp.224
		if(0<state.nStatusQueue)
		{
			data=state.statusQueue[0];
			--state.nStatusQueue;
			for(int i=0; i<state.nStatusQueue; ++i)
			{
				state.statusQueue[i]=state.statusQueue[i+1];
			}
			return data;
		}
		break;
	case TOWNSIO_CDROM_PARAMETER_DATA://    0x4C4, // [2] pp.224
		// Data register for software transfer.  Not supported yet.
		Abort("Unsupported CDROM I/O Read 04C4H");
		break;
	case TOWNSIO_CDROM_TRANSFER_CTRL://     0x4C6, // [2] pp.227
		// Write only.
		break;
	case TOWNSIO_CDROM_SUBCODE_STATUS://    0x4CC,
		Abort("Unsupported CDROM I/O Read 04CCH");
		return 0;
	case TOWNSIO_CDROM_SUBCODE_DATA://      0x4CD,
		Abort("Unsupported CDROM I/O Read 04CDH");
		return 0;
	}
	return 0xff;
}
