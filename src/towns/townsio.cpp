/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
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
		if(0!=(data&1))
		{
			state.resetReason=RESET_REASON_SOFTWARE;
			Reset();
		}
		break;
	case TOWNSIO_RESET_REASON:
		if(0!=(data&0x40))
		{
			var.powerOff=true;
		}
		if(0!=(data&0x01))
		{
			debugger.ExternalBreak("RESET(IO 0x20 Bit 0)");
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
	case TOWNSIO_TIMER_1US_WAIT:
		state.townsTime+=1000;
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
		return ( (state.townsTime/1000)<<var.freeRunTimerShift)&0xff;
	case TOWNSIO_FREERUN_TIMER_HIGH://0x28,
		return (((state.townsTime/1000)<<var.freeRunTimerShift)>>8)&0xff;


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

	case TOWNSIO_TIMER_1US_WAIT:
		// Supposed to be 1us wait when written.  But, mouse BIOS is often reading from this register.
		state.townsTime+=1000;
		break;
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
