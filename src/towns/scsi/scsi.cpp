/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "device.h"
#include "townsdef.h"
#include "cpputil.h"

#include "scsi.h"

#include "towns.h"



////////////////////////////////////////////////////////////



void TownsSCSI::State::PowerOn(void)
{
	Reset();
}
void TownsSCSI::State::Reset(void)
{
	REQ=false;
	I_O=false;
	MSG=false;
	C_D=false;
	BUSY=false;
	INT=false;
	PERR=false;

	DMAE=false;
	SEL=false;
	ATN=false;
	IMSK=false;
	WEN=false;

	selId=0;
	phase=PHASE_BUSFREE;
	lastDataByte=0;
}



////////////////////////////////////////////////////////////



TownsSCSI::TownsSCSI(class FMTowns *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;

	for(auto &n : commandLength)
	{
		n=0;
	}
	commandLength[SCSICMD_INQUIRY]=6;
}
/* virtual */ void TownsSCSI::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsSCSI::Reset(void)
{
	state.Reset();
}

bool TownsSCSI::LoadHardDiskImage(unsigned int scsiId,std::string fName)
{
	if(scsiId<MAX_NUM_SCSIDEVICES)
	{
		auto fSize=cpputil::FileSize(fName);
		if(0<fSize)
		{
			state.dev[scsiId].imageSize=fSize;
			state.dev[scsiId].imageFName=fName;
			state.dev[scsiId].devType=SCSIDEVICE_HARDDISK;
			return true;
		}
	}
	return false;
}

/* static */ std::string TownsSCSI::PhaseToStr(unsigned int phase)
{
	switch(phase)
	{
	case PHASE_BUSFREE:
		return "BUSFREE";
	case PHASE_ARBITRATION:
		return "ARBITRATION";
	case PHASE_SELECTION:
		return "SELECTION";
	case PHASE_RESELECTION:
		return "RESELECTION";
	case PHASE_MESSAGE_OUT:
		return "MESSAGE_OUT";
	case PHASE_MESSAGE_IN:
		return "MESSAGE_IN";
	case PHASE_COMMAND:
		return "COMMAND";
	case PHASE_DATA_IN:
		return "DATA_IN";
	case PHASE_DATA_OUT:
		return "DATA_OUT";
	case PHASE_STATUS:
		return "STATUS";
	}
	return "UNDEFINED";
}

void TownsSCSI::SetUpIO_MSG_CDfromPhase(void)
{
	switch(state.phase)
	{
	case PHASE_ARBITRATION:
	case PHASE_SELECTION:
	case PHASE_RESELECTION:
		// No change
		break;
	case PHASE_BUSFREE:
		state.I_O=false;
		state.MSG=false;
		state.C_D=false;
		break;
	case PHASE_MESSAGE_OUT:
		state.I_O=false;
		state.MSG=true;
		state.C_D=true;
		break;
	case PHASE_MESSAGE_IN:
		state.I_O=true;
		state.MSG=true;
		state.C_D=true;
		break;
	case PHASE_COMMAND:
		state.I_O=false;
		state.MSG=false;
		state.C_D=true;
		break;
	case PHASE_DATA_IN:
		state.I_O=true;
		state.MSG=false;
		state.C_D=false;
		break;
	case PHASE_DATA_OUT:
		state.I_O=false;
		state.MSG=false;
		state.C_D=false;
		break;
	case PHASE_STATUS:
		state.I_O=true;
		state.MSG=false;
		state.C_D=true;
		break;
	}
}

void TownsSCSI::EnterBusFreePhase(void)
{
	state.BUSY=false;
	state.REQ=false;
	state.phase=PHASE_BUSFREE;
	SetUpIO_MSG_CDfromPhase();
}
void TownsSCSI::EnterSelectionPhase(void)
{
	for(unsigned int id=0; id<MAX_NUM_SCSIDEVICES; ++id)
	{
		if(0!=(state.lastDataByte&(1<<id)) && SCSIDEVICE_NONE!=state.dev[id].devType)
		{
			state.selId=id;
			state.phase=PHASE_SELECTION;
			state.BUSY=true;
			SetUpIO_MSG_CDfromPhase();
			return;
		}
	}
	// What's the correct way of handling no-device?
	state.phase=PHASE_BUSFREE;
	state.BUSY=false;
	SetUpIO_MSG_CDfromPhase();
}
void TownsSCSI::EnterCommandPhase(void)
{
	// SCSI2 Spec tells it should change to MESSAGE_OUT phase.
	// Seriously?  FM Towns BIOS Looks to be waiting for the COMMAND phase.
	state.phase=PHASE_COMMAND;
	state.nCommandFilled=0;
	state.REQ=true;
	SetUpIO_MSG_CDfromPhase();
	if(true==IRQEnabled())
	{
		townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_SCSI,true);
	}
}
void TownsSCSI::EnterDataInPhase(void)
{
	state.phase=PHASE_DATA_IN;
	state.REQ=true;
	SetUpIO_MSG_CDfromPhase();
	townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DATA_INTERVAL);
}

/* virtual */ void TownsSCSI::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_SCSI_DATA: //            0xC30, // [2] pp.263
		townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_SCSI,false);
		state.lastDataByte=data;
		ProcessPhaseData(data);
		break;
	case TOWNSIO_SCSI_STATUS_CONTROL: //  0xC32, // [2] pp.262
		if(0!=(data&0x01)) // RST
		{
			state.Reset();
		}
		if(0!=(data&0x02))
		{
			state.DMAE=true;
		}
		{
			auto nextSEL=(0!=(data&0x04));
			if(state.SEL!=nextSEL)
			{
				if(true==nextSEL && true!=state.BUSY)
				{
					EnterSelectionPhase();
				}
				else if(true!=nextSEL && PHASE_SELECTION==state.phase)
				{
					EnterCommandPhase();
				}
				state.SEL=nextSEL;
			}
		}
		if(0!=(data&0x08))
		{
		}
		if(0!=(data&0x10))
		{
			state.ATN=true;
		}
		if(0!=(data&0x20))
		{
		}
		if(0!=(data&0x40))
		{
			state.IMSK=true;
		}
		if(0!=(data&0x80))
		{
			state.WEN=true;
		}
		break;
	}
}
/* virtual */ unsigned int TownsSCSI::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_SCSI_DATA: //            0xC30, // [2] pp.263
		townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_SCSI,false);
		break;
	case TOWNSIO_SCSI_STATUS_CONTROL: //  0xC32, // [2] pp.262
		{
			unsigned char data=0;
			data|=(true==state.PERR ? 0x01 : 0);
			data|=(true==state.INT  ? 0x02 : 0);
			data|=(true==state.BUSY ? 0x08 : 0);
			data|=(true==state.C_D  ? 0x10 : 0);
			data|=(true==state.MSG  ? 0x20 : 0);
			data|=(true==state.I_O  ? 0x40 : 0);
			data|=(true==state.REQ  ? 0x80 : 0);
			return data;
		}
		break;
	}
	return 0xff;
}

void TownsSCSI::ProcessPhaseData(unsigned int dataByte)
{
	if(PHASE_COMMAND==state.phase)
	{
		state.commandBuffer[state.nCommandFilled++]=(unsigned char)dataByte;
		state.REQ=false;
		if(0==commandLength[state.commandBuffer[0]])
		{
			Abort("Command Length not set for this command.");
		}
		else if(commandLength[state.commandBuffer[0]]<=state.nCommandFilled)
		{
			// Execute command
			townsPtr->debugger.ExternalBreak("SCSI command.");
			ExecSCSICommand();
		}
		else
		{
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+COMMAND_REQUEST_INTERVAL);
		}
	}
}
void TownsSCSI::ExecSCSICommand(void)
{
	switch(state.commandBuffer[0])
	{
	case SCSICMD_INQUIRY:
		EnterDataInPhase();
		break;
	default:
		townsPtr->debugger.ExternalBreak("SCSI command not implemented yet.");
		EnterBusFreePhase();
		break;
	}
}
/* virtual */ void TownsSCSI::RunScheduledTask(unsigned long long int townsTime)
{
	if(PHASE_COMMAND==state.phase)
	{
		state.REQ=true;
		if(true==IRQEnabled())
		{
			townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_SCSI,true);
		}
	}
	else if(PHASE_DATA_IN==state.phase)
	{
		auto DMACh=townsPtr->dmac.GetDMAChannel(TOWNSDMA_SCSI);
		if(nullptr!=DMACh)
		{
			townsPtr->debugger.ExternalBreak("DATA Phase DMA Ready.");
			switch(state.commandBuffer[0])
			{
			case SCSICMD_INQUIRY:
				break;
			}
		}
		else // DMA Not Ready check again after waiting for some period.
		{
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DATA_INTERVAL);
		}
	}
}

std::vector <std::string> TownsSCSI::GetStatusText(void) const
{
	std::vector <std::string> text;

	text.push_back("");
	text.back()="PHASE:";
	text.back()+=PhaseToStr(state.phase);

	text.push_back("");
	text.back()+="REQ:"+std::string(cpputil::BoolToNumberStr(state.REQ));
	text.back()+=" I/O:"+std::string(cpputil::BoolToNumberStr(state.I_O));
	text.back()+=" MSG:"+std::string(cpputil::BoolToNumberStr(state.MSG));
	text.back()+=" C/D:"+std::string(cpputil::BoolToNumberStr(state.MSG));
	text.back()+=" BUSY:"+std::string(cpputil::BoolToNumberStr(state.BUSY));
	text.back()+=" INT:"+std::string(cpputil::BoolToNumberStr(state.INT));
	text.back()+=" PERR:"+std::string(cpputil::BoolToNumberStr(state.PERR));

	text.push_back("");
	text.back()+="DMAE:"+std::string(cpputil::BoolToNumberStr(state.DMAE));
	text.back()+=" SEL:"+std::string(cpputil::BoolToNumberStr(state.SEL));
	text.back()+=" ATN:"+std::string(cpputil::BoolToNumberStr(state.ATN));
	text.back()+=" IMSK:"+std::string(cpputil::BoolToNumberStr(state.IMSK));
	text.back()+=" WEN:"+std::string(cpputil::BoolToNumberStr(state.WEN));

	text.push_back("");
	text.back()+="Selected SCSI ID:"+cpputil::Uitoa(state.selId);
	text.back()+=" Last Data Byte:"+cpputil::Ubtox(state.lastDataByte)+"H";

	text.push_back("");
	text.back()+="COMMAND BUFFER:";
	for(unsigned int i=0; i<state.nCommandFilled; ++i)
	{
		text.back()+=" ";
		text.back()+=cpputil::Ubtox(state.commandBuffer[i]);
	}

	return text;
}
