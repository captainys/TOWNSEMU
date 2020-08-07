/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <algorithm>

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
	commandLength[SCSICMD_TEST_UNIT_READY]=6;
	commandLength[SCSICMD_REZERO_UNIT]    =6;
	commandLength[SCSICMD_INQUIRY]        =6;
	commandLength[SCSICMD_PREVENT_REMOVAL]=6;
	commandLength[SCSICMD_READ_CAPACITY]  =10;
	commandLength[SCSICMD_READ_10]        =10;
	commandLength[SCSICMD_WRITE_10]       =10;
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
			state.deviceConnected=true;
			state.dev[scsiId].imageSize=fSize;
			state.dev[scsiId].imageFName=fName;
			state.dev[scsiId].devType=SCSIDEVICE_HARDDISK;
			return true;
		}
	}
	return false;
}
bool TownsSCSI::LoadCDImage(unsigned int scsiId,std::string fName)
{
	std::cout << "SCSI CD-ROM is not supported yet." << std::endl;
	if(scsiId<MAX_NUM_SCSIDEVICES)
	{
		auto fSize=cpputil::FileSize(fName);
		if(0<fSize)
		{
			state.deviceConnected=true;

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
	state.INT=true;
	SetUpIO_MSG_CDfromPhase();
	townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+COMMAND_DELAY);
}
void TownsSCSI::EnterDataInPhase(void)
{
	state.phase=PHASE_DATA_IN;
	state.REQ=true;
	state.bytesTransferred=0;
	SetUpIO_MSG_CDfromPhase();
	townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DATA_INTERVAL);
}
void TownsSCSI::EnterDataOutPhase(void)
{
	state.phase=PHASE_DATA_OUT;
	state.REQ=true;
	state.bytesTransferred=0;
	SetUpIO_MSG_CDfromPhase();
	townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DATA_INTERVAL);
}
void TownsSCSI::EnterMessageInPhase(void)
{
	state.phase=PHASE_MESSAGE_IN;
	state.REQ=true;
	state.INT=true;
	SetUpIO_MSG_CDfromPhase();
	townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+MESSAGE_DELAY);
}
void TownsSCSI::EnterStatusPhase(void)
{
	state.phase=PHASE_STATUS;
	state.REQ=true;
	state.INT=true;
	SetUpIO_MSG_CDfromPhase();
	townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+MESSAGE_DELAY);
}


/* virtual */ void TownsSCSI::IOWriteByte(unsigned int ioport,unsigned int data)
{
	if(true!=state.deviceConnected)
	{
		return;
	}

	switch(ioport)
	{
	case TOWNSIO_SCSI_DATA: //            0xC30, // [2] pp.263
		townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_SCSI,false);
		state.INT=false;
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
	if(true!=state.deviceConnected)
	{
		return 0xff;
	}

	switch(ioport)
	{
	case TOWNSIO_SCSI_DATA: //            0xC30, // [2] pp.263
		townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_SCSI,false);
		state.INT=false;
		return PhaseReturnData();
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

unsigned char TownsSCSI::PhaseReturnData(void)
{
	if(PHASE_STATUS==state.phase)
	{
		EnterMessageInPhase();
		return state.status;
	}
	else if(PHASE_MESSAGE_IN==state.phase)
	{
		EnterBusFreePhase();
		return state.message;
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
			townsPtr->debugger.ExternalBreak("SCSI Command Length not set for this command.");
		}
		else if(commandLength[state.commandBuffer[0]]<=state.nCommandFilled)
		{
			// Execute command
			if(true==breakOnSCSICommand)
			{
				std::string msg;
				msg="SCSI Command (";
				msg+=cpputil::Ubtox(state.commandBuffer[0]);
				msg+="H)";
				townsPtr->debugger.ExternalBreak(msg);
			}
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
	if(true==monitorSCSICmd)
	{
		std::cout << "SCSI Cmd:";
		for(int i=0; i<state.nCommandFilled; ++i)
		{
			std::cout << " " << cpputil::Ubtox(state.commandBuffer[i]);
		}
		std::cout << " @ ";
		std::cout << cpputil::Ustox(townsPtr->cpu.state.CS().value) << ":";
		std::cout << cpputil::Uitox(townsPtr->cpu.state.EIP) << std::endl;
	}
	switch(state.commandBuffer[0])
	{
	case SCSICMD_INQUIRY:
		EnterDataInPhase();
		break;
	case SCSICMD_PREVENT_REMOVAL:
		// I just ignore this command, and let it return STATUSCODE_GOOD by falling down to SCSICMD_TEST_UNIT_READY.
		// (state.commandBuffer[4]&1)==1 means Prevent, or 0 Allow.
	case SCSICMD_REZERO_UNIT:
		// [9] 9.1.8 Seek and rezero
		// "Some devices return GOOD status without attempting any action."
		// Just let it do the same thing as TEST_UNIT_READY.
	case SCSICMD_TEST_UNIT_READY:
		if(SCSIDEVICE_NONE!=state.dev[state.selId].devType)
		{
			state.status=STATUSCODE_GOOD;
			state.message=0; // What am I supposed to return?
			state.senseKey=SENSEKEY_NO_SENSE;
		}
		else
		{
			state.status=STATUSCODE_CHECK_CONDITION;
			state.message=0; // What am I supposed to return?
			state.senseKey=SENSEKEY_NOT_READY;
		}
		EnterStatusPhase();
		break;
	case SCSICMD_READ_CAPACITY:
		if(0!=(state.commandBuffer[8]&1) && // PMI bit on
		   (0!=state.commandBuffer[2] ||
		    0!=state.commandBuffer[3] ||
		    0!=state.commandBuffer[4] ||
		    0!=state.commandBuffer[5]))
		{
			state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
			state.status=STATUSCODE_CHECK_CONDITION;
			state.message=0;
			EnterStatusPhase();
		}
		else if(SCSIDEVICE_HARDDISK==state.dev[state.selId].devType)
		{
			EnterDataInPhase();
		}
		// else if(SCSIDEVICE_CDROM==state.dev[state.selId].devType)
		// {
		// 	state.status=;
		// 	state.message=0; // What am I supposed to return?
		// 	state.senseKey=;
		// }
		else
		{
			state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
			state.status=STATUSCODE_CHECK_CONDITION;
			state.message=0; // What am I supposed to return?
			EnterStatusPhase();
		}
		break;
	case SCSICMD_READ_10:
		if(SCSIDEVICE_HARDDISK==state.dev[state.selId].devType)
		{
			EnterDataInPhase();
		}
		else
		{
			state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
			state.status=STATUSCODE_CHECK_CONDITION;
			state.message=0; // What am I supposed to return?
			EnterStatusPhase();
		}
		break;
	case SCSICMD_WRITE_10:
		if(SCSIDEVICE_HARDDISK==state.dev[state.selId].devType)
		{
			EnterDataOutPhase();
		}
		else
		{
			state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
			state.status=STATUSCODE_CHECK_CONDITION;
			state.message=0; // What am I supposed to return?
			EnterStatusPhase();
		}
		break;
	default:
		townsPtr->debugger.ExternalBreak("SCSI command not implemented yet.");
		EnterBusFreePhase();
		break;
	}
}
/* virtual */ void TownsSCSI::RunScheduledTask(unsigned long long int townsTime)
{
	if(PHASE_COMMAND==state.phase ||
	   PHASE_MESSAGE_IN==state.phase ||
	   PHASE_STATUS==state.phase)
	{
		state.REQ=true;
		state.INT=true;
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
			if(true==breakOnDMATransfer)
			{
				townsPtr->debugger.ExternalBreak("SCSI DMA Transfer");
			}

			switch(state.commandBuffer[0])
			{
			case SCSICMD_INQUIRY:
				{
					auto data=MakeInquiryData(state.selId);
					state.bytesTransferred+=townsPtr->dmac.DeviceToMemory(
					    DMACh,
					    data.size()-state.bytesTransferred,
					    data.data()+state.bytesTransferred);
					townsPtr->dmac.SetDMATransferEnd(TOWNSDMA_SCSI);
					if(data.size()<=state.bytesTransferred)
					{
						state.status=STATUSCODE_GOOD;
						state.message=0;
						EnterStatusPhase();
					}
					else
					{
						townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DATA_INTERVAL);
					}
				}
				break;
			case SCSICMD_READ_CAPACITY:
				{
					auto data=MakeReadCapacityData(state.selId);
					state.bytesTransferred+=townsPtr->dmac.DeviceToMemory(
					    DMACh,
					    data.size()-state.bytesTransferred,
					    data.data()+state.bytesTransferred);
					townsPtr->dmac.SetDMATransferEnd(TOWNSDMA_SCSI);
					if(data.size()<=state.bytesTransferred)
					{
						state.status=STATUSCODE_GOOD;
						state.message=0;
						EnterStatusPhase();
					}
					else
					{
						townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DATA_INTERVAL);
					}
				}
				break;
			case SCSICMD_READ_10:
				if(SCSIDEVICE_HARDDISK==state.dev[state.selId].devType)
				{
					townsPtr->NotifyDiskRead();

					unsigned int LBA=(state.commandBuffer[2]<<24)|
					                 (state.commandBuffer[3]<<16)|
					                 (state.commandBuffer[4]<<8)|
					                  state.commandBuffer[5];
					unsigned int LEN=(state.commandBuffer[7]<<8)|
					                  state.commandBuffer[8];

					LBA*=HARDDISK_SECTOR_LENGTH;
					LEN*=HARDDISK_SECTOR_LENGTH;
					state.bytesTransferred+=townsPtr->dmac.DeviceToMemory(
					    DMACh,
					    cpputil::ReadBinaryFile(
					        state.dev[state.selId].imageFName,
					        LBA+state.bytesTransferred,
					        LEN-state.bytesTransferred));
					townsPtr->dmac.SetDMATransferEnd(TOWNSDMA_SCSI);
					if(LEN<=state.bytesTransferred)
					{
						state.status=STATUSCODE_GOOD;
						state.message=0;
						EnterStatusPhase();
					}
					else
					{
						townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DATA_INTERVAL);
					}
				}
				else
				{
					state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
					state.status=STATUSCODE_CHECK_CONDITION;
					state.message=0; // What am I supposed to return?
					EnterStatusPhase();
				}
				break;
			default:
				townsPtr->debugger.ExternalBreak("DATA IN Phase: Command Not Supported.");
				break;
			}
		}
		else // DMA Not Ready check again after waiting for some period.
		{
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DATA_INTERVAL);
		}
	}
	else if(PHASE_DATA_OUT==state.phase)
	{
		auto DMACh=townsPtr->dmac.GetDMAChannel(TOWNSDMA_SCSI);
		if(nullptr!=DMACh)
		{
			if(true==breakOnDMATransfer)
			{
				townsPtr->debugger.ExternalBreak("SCSI DMA Transfer");
			}

			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DATA_INTERVAL);
			switch(state.commandBuffer[0])
			{
			case SCSICMD_WRITE_10:
				if(SCSIDEVICE_HARDDISK==state.dev[state.selId].devType)
				{
					unsigned int LBA=(state.commandBuffer[2]<<24)|
					                 (state.commandBuffer[3]<<16)|
					                 (state.commandBuffer[4]<<8)|
					                  state.commandBuffer[5];
					unsigned int LEN=(state.commandBuffer[7]<<8)|
					                  state.commandBuffer[8];

					LBA*=HARDDISK_SECTOR_LENGTH;
					LEN*=HARDDISK_SECTOR_LENGTH;

					auto toWrite=townsPtr->dmac.MemoryToDevice(DMACh,LEN-state.bytesTransferred);
					townsPtr->dmac.SetDMATransferEnd(TOWNSDMA_SCSI);
					if(true==cpputil::WriteBinaryFile(
					    state.dev[state.selId].imageFName,
					    LBA+state.bytesTransferred,
					    (unsigned int)toWrite.size(),
					    toWrite.data()))
					{
						state.bytesTransferred+=toWrite.size();
						if(LEN<=state.bytesTransferred)
						{
							state.status=STATUSCODE_GOOD;
							state.message=0;
							EnterStatusPhase();
						}
						else
						{
							townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DATA_INTERVAL);
							// Continue Data-Out Phase.
						}
					}
					else
					{
						state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
						state.status=STATUSCODE_CHECK_CONDITION;
						state.message=0; // What am I supposed to return?
						EnterStatusPhase();
					}
				}
				else
				{
					state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
					state.status=STATUSCODE_CHECK_CONDITION;
					state.message=0; // What am I supposed to return?
					EnterStatusPhase();
				}
				break;
			default:
				townsPtr->debugger.ExternalBreak("DATA OUT Phase: Command Not Supported.");
				break;
			}
		}
		else // DMA Not Ready check again after waiting for some period.
		{
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DATA_INTERVAL);
		}
	}
}

std::vector <unsigned char> TownsSCSI::MakeInquiryData(int scsiId) const
{
	std::vector <unsigned char> dat;

	// 8.2.5.1 Standard INQUIRY data
	dat.resize(36);
	switch(state.dev[scsiId].devType)
	{
	default:
		dat[0]=0b01111111; // PeripheralQualifier(7,6,5)|PeriphefalDeviceType(4,3,2,1,0)
		dat[1]=0; // RMB(7)|DeviceTypeModifier(6...0)
		dat[2]=1; // SCSI-1
		dat[3]=0; // SCSI-1 message format
		dat[4]=0; // Additional length of the parameters? What parameter?
		dat[5]=0; // Reserved
		dat[6]=0; // Reserved
		dat[7]=0; // RelAdr|WBus32|WBus16|Sync|Linked|Rsvd|CmdQue|SftRe
		for(int i=8; i<36; ++i)
		{
			dat[i]=0;
		}
		break;
	case SCSIDEVICE_HARDDISK:
		dat[0]=0b00000000; // PeripheralQualifier(7,6,5)|PeriphefalDeviceType(4,3,2,1,0) Table 47
		dat[1]=0; // RMB(7)|DeviceTypeModifier(6...0)
		dat[2]=1; // SCSI-1
		dat[3]=0; // SCSI-1 message format
		dat[4]=0; // Additional length of the parameters? What parameter?
		dat[5]=0; // Reserved
		dat[6]=0; // Reserved
		dat[7]=0; // RelAdr|WBus32|WBus16|Sync|Linked|Rsvd|CmdQue|SftRe

		dat[ 8]='T';
		dat[ 9]='S';
		dat[10]='U';
		dat[11]='G';
		dat[12]='A';
		dat[13]='R';
		dat[14]='U';
		dat[15]=0;

		dat[16]='H';
		dat[17]='A';
		dat[18]='R';
		dat[19]='D';
		dat[20]='D';
		dat[21]='I';
		dat[22]='S';
		dat[23]='K';
		dat[24]=0;
		dat[25]=0;
		dat[26]=0;
		dat[27]=0;
		dat[28]=0;
		dat[29]=0;
		dat[30]=0;
		dat[31]=0;

		dat[32]=1;
		dat[33]=0;
		dat[34]=0;
		dat[35]=0;
		break;
	case SCSIDEVICE_CDROM:
		dat[0]=0b00000101; // Table 47
		dat[1]=0b10000000; // RMB(7)|DeviceTypeModifier(6...0)
		dat[2]=1; // SCSI-1
		dat[3]=0; // SCSI-1 message format
		dat[4]=0; // Additional length of the parameters? What parameter?
		dat[5]=0; // Reserved
		dat[6]=0; // Reserved
		dat[7]=0; // RelAdr|WBus32|WBus16|Sync|Linked|Rsvd|CmdQue|SftRe

		dat[ 8]='T';
		dat[ 9]='S';
		dat[10]='U';
		dat[11]='G';
		dat[12]='A';
		dat[13]='R';
		dat[14]='U';
		dat[15]=0;

		dat[16]='C';
		dat[17]='D';
		dat[18]='R';
		dat[19]='O';
		dat[20]='M';
		dat[21]=0;
		dat[22]=0;
		dat[23]=0;
		dat[24]=0;
		dat[25]=0;
		dat[26]=0;
		dat[27]=0;
		dat[28]=0;
		dat[29]=0;
		dat[30]=0;
		dat[31]=0;

		dat[32]=1;
		dat[33]=0;
		dat[34]=0;
		dat[35]=0;

		break;
	}

	return dat;
}

std::vector <unsigned char> TownsSCSI::MakeReadCapacityData(int scsiId) const
{
	std::vector <unsigned char> dat;
	dat.resize(8);
	unsigned long long int numLBA=state.dev[scsiId].imageSize/HARDDISK_SECTOR_LENGTH;
	dat[0]=((numLBA>>24)&255);
	dat[1]=((numLBA>>16)&255);
	dat[2]=((numLBA>> 8)&255);
	dat[3]=( numLBA     &255);
	dat[4]=0;
	dat[5]=0;
	dat[6]=((HARDDISK_SECTOR_LENGTH>>8)&255);
	dat[7]= (HARDDISK_SECTOR_LENGTH    &255);
	return dat;
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
