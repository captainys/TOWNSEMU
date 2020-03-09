#include "townsio.h"



/* virtual */ void FMTowns::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_POWER_CONTROL:
		if(0!=(data&0x40))
		{
			var.powerOff=true;
		}
		break;
	case TOWNSIO_RESET_REASON:
		if(0!=(data&0x40))
		{
			var.powerOff=true;
		}
		break;
	case TOWNSIO_SERIAL_ROM_CTRL://=        0x32,
		if((0x60&data)==0x60 && (0x80&state.lastSerialROMCommand)!=0 && (0x80&data)==0)
		{
			state.serialROMBitCount=0;
		}
		else if((0xA0&data)==0x20 && (0x40&state.lastSerialROMCommand)==0 && (0x40&data)!=0)
		{
			state.serialROMBitCount=(state.serialROMBitCount+1)&255;
		}
		state.lastSerialROMCommand=data;
		break;

	case TOWNSIO_VM_HOST_IF_CMD_STATUS:
		ProcessVMToHostCommand(data,var.nVM2HostParam,var.VM2HostParam);
		var.nVM2HostParam=0;
		break;
	case TOWNSIO_VM_HOST_IF_DATA:
		if(var.nVM2HostParam<Variable::VM2HOST_PARAM_QUEUE_LENGTH)
		{
			var.VM2HostParam[var.nVM2HostParam++]=data;
		}
		break;
	}
}
/* virtual */ void FMTowns::IOWriteWord(unsigned int ioport,unsigned int data)
{
	Device::IOWriteWord(ioport,data);
}
/* virtual */ void FMTowns::IOWriteDword(unsigned int ioport,unsigned int data)
{
	Device::IOWriteDword(ioport,data);
}
/* virtual */ unsigned int FMTowns::IOReadByte(unsigned int ioport)
{

	switch(ioport)
	{
	case TOWNSIO_RESET_REASON://      0x20,
		{
			auto ret=state.resetReason;
			state.resetReason&=(~3);
			return ret;
		}
		break;
	case TOWNSIO_MACHINE_ID_LOW://         0x30
		return MachineID()&0xFF;
	case TOWNSIO_MACHINE_ID_HIGH://=        0x31,
		return (MachineID()>>8)&0xFF;
	case TOWNSIO_FREERUN_TIMER_LOW ://0x26,
		return (state.townsTime<<var.freeRunTimerShift)&0xff;
	case TOWNSIO_FREERUN_TIMER_HIGH://0x28,
		return ((state.townsTime<<var.freeRunTimerShift)>>8)&0xff;


	case TOWNSIO_SERIAL_ROM_CTRL://=        0x32,
		{
			unsigned int data=(state.lastSerialROMCommand&0xC0);
			unsigned int index=255-(state.serialROMBitCount>>3);
			unsigned int bit=(1<<(state.serialROMBitCount&7));
			if(0!=(physMem.serialROM[index]&bit))
			{
				data|=1;
			}
			return data;
		}
		break;


	case TOWNSIO_FMR_RESOLUTION: // 0x400
		// Bit0 should always be 0.
		// In FM-R, Bit3 is labeled as "160P".  Meaning unknown.
		return 0xFE;
	}
	return 0xff;
}
/* virtual */ unsigned int FMTowns::IOReadWord(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_FREERUN_TIMER:// 0x26
		return (state.townsTime<<var.freeRunTimerShift)&0xffff;
	}
	return Device::IOReadWord(ioport);
}
/* virtual */ unsigned int FMTowns::IOReadDword(unsigned int ioport)
{
	return Device::IOReadWord(ioport);
}
