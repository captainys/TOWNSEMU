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

#include "outside_world.h"



////////////////////////////////////////////////////////////



TownsSCSI::SCSIIOThread::SCSIIOThread()
{
	std::thread t(&TownsSCSI::SCSIIOThread::ThreadFunc,this);
	std::swap(t,thr);
}
TownsSCSI::SCSIIOThread::~SCSIIOThread()
{
	{
		std::unique_lock <std::mutex> lock(mutex);
		cmd=CMD_QUIT;
	}
	cond.notify_all();
	thr.join();
}
void TownsSCSI::SCSIIOThread::ThreadFunc(void)
{
	std::unique_lock<std::mutex> lock(mutex);
	for(;;)
	{
		// mutex is locked.  Can access command and parameters.

		// Atomically check condition, release lock, and then wait until notified
		// If the 'check condition' comes before 'wait' as advertised,
		// this should exit immidiately if there are some tasks to do.

		// To answer the question: what if it is not waiting when the main thread notify_all?
		// this 'wait' will wait for notification, or will not wait if the condition is true.
		// If tasks are populated, or quit flat is set before this 'wait', it will not block.
		cond.wait(lock,[=]{return cmd!=CMD_NONE;});

		// When wait exits, this thread owns the lock.

		if(CMD_QUIT==cmd)
		{
			cmd=CMD_NONE;
			break;
		}
		else if(CMD_FILEREAD==cmd)
		{
		    data=cpputil::ReadBinaryFile(fName,filePtr,length);
			cmd=CMD_NONE;
			dataReady=true;
		}
		else if(CMD_CDREAD==cmd)
		{
			data=discImgPtr->ReadSectorMODE1(filePtr,length);
			cmd=CMD_NONE;
			dataReady=true;
		}
	}
}
bool TownsSCSI::SCSIIOThread::IsBusy(void) const
{
	if(mutex.try_lock())
	{
		bool r=(CMD_NONE!=cmd);
		mutex.unlock();
		return r;
	}
	return true; // Couldn't take a lock -> Thread is busy.
}
void TownsSCSI::SCSIIOThread::WaitReady(void)
{
	std::unique_lock<std::mutex> lock(mutex);
}
void TownsSCSI::SCSIIOThread::SetUpFileRead(std::string fName,uint64_t filePtr,uint64_t length)
{
	{
		std::unique_lock <std::mutex> lock(mutex);
		cmd=CMD_FILEREAD;
		dataReady=false;
		this->fName=fName;
		this->filePtr=filePtr;
		this->length=length;
	}
	cond.notify_all();
}
void TownsSCSI::SCSIIOThread::SetUpCDRead(const DiscImage *discImgPtr,uint64_t LBA,uint64_t LEN)
{
	{
		std::unique_lock <std::mutex> lock(mutex);
		cmd=CMD_CDREAD;
		dataReady=false;
		this->discImgPtr=discImgPtr;
		this->filePtr=LBA;
		this->length=LEN;
	}
	cond.notify_all();
}
const std::vector <unsigned char> *TownsSCSI::SCSIIOThread::GetData(void)
{
	const std::vector <unsigned char> *dataPtr=nullptr;
	if(mutex.try_lock())
	{
		if(true==dataReady)
		{
			dataPtr=&data;
		}
		mutex.unlock();
	}
	return dataPtr;
}



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
	commandLength[SCSICMD_SENSE]          =6;
	commandLength[SCSICMD_READ_6]         =6;
	commandLength[SCSICMD_WRITE_6]        =6;
	commandLength[SCSICMD_SEEK_6]         =6;
	commandLength[SCSICMD_INQUIRY]        =6;
	commandLength[SCSICMD_PREVENT_REMOVAL]=6;
	commandLength[SCSICMD_READ_CAPACITY]  =10;
	commandLength[SCSICMD_READ_10]        =10;
	commandLength[SCSICMD_WRITE_10]       =10;
	commandLength[SCSICMD_SEEK_10]        =10;
	commandLength[SCSICMD_VERIFY_10]      =10;
	commandLength[SCSICMD_READ_SUBCHANNEL]=10;
	commandLength[SCSICMD_READTOC]        =10;
	commandLength[SCSICMD_PLAY_AUDIO_MSF] =10;
	commandLength[SCSICMD_PAUSE_RESUME]   =10;
}

void TownsSCSI::SetOutsideWorld(class Outside_World *ptr)
{
	this->outsideworld=ptr;
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
	if(scsiId<MAX_NUM_SCSIDEVICES)
	{
		if(DiscImage::ERROR_NOERROR==state.dev[scsiId].discImg.Open(fName))
		{
			state.deviceConnected=true;
			state.dev[scsiId].imageFName=fName;
			state.dev[scsiId].devType=SCSIDEVICE_CDROM;
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

	if(SCSICMD_READ_6==state.commandBuffer[0] ||
	   SCSICMD_READ_10==state.commandBuffer[0])
	{
		if(SCSIDEVICE_HARDDISK==state.dev[state.selId].devType ||
		   SCSIDEVICE_CDROM==state.dev[state.selId].devType)
		{
			unsigned int LBA,LEN;

			if(SCSICMD_READ_10==state.commandBuffer[0])
			{
				LBA=(state.commandBuffer[2]<<24)|
				    (state.commandBuffer[3]<<16)|
				    (state.commandBuffer[4]<<8)|
				     state.commandBuffer[5];
				LEN=(state.commandBuffer[7]<<8)|
				     state.commandBuffer[8];
			}
			else
			{
				LBA=((state.commandBuffer[1]&0x1F)<<16)|
				    (state.commandBuffer[2]<<8)|
				     state.commandBuffer[3];
				LEN= state.commandBuffer[4];
				if(0==LEN)
				{
					LEN=256;
				}
			}

			if(SCSIDEVICE_HARDDISK==state.dev[state.selId].devType)
			{
				LBA*=HARDDISK_SECTOR_LENGTH;
				LEN*=HARDDISK_SECTOR_LENGTH;
				ioThread.SetUpFileRead(
				        state.dev[state.selId].imageFName,
				        LBA+state.bytesTransferred,
				        LEN-state.bytesTransferred);
			}
			else if(SCSIDEVICE_CDROM==state.dev[state.selId].devType)
			{
				ioThread.SetUpCDRead(
				        &state.dev[state.selId].discImg,
				        LBA,
				        LEN);
			}
		}
	}
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
		state.ATN=(0!=(data&0x10));
		if(0!=(data&0x20))
		{
		}
		state.IMSK=(0!=(data&0x40));
		state.WEN=(0!=(data&0x80));

		if(true!=IRQEnabled())
		{
			// Probably resetting IMSK should also clear IRR.
			townsPtr->pic.SetInterruptRequestBit(TOWNSIRQ_SCSI,false);
		}
		break;
	}
}
/* virtual */ unsigned int TownsSCSI::IOReadByte(unsigned int ioport)
{
	/* 2021/03/27
	SCSI Controller can accept command even when no device is connected.

	This change was necessary to address the situation:
	(1) Hard-disk drive-letter is registered in CMOS, but
	(2) No hard-disk image is mounted.

	This is similar to the situation when a hard-disk drive-letter is registered in CMOS, but
	the external hard drive is powered off.

	The real FM TOWNS can boot in this setting.  However, the BIOS (IO.SYS) was waiting for the
	SCSI ready signal forever unless I comment out the following four lines.

	if(true!=state.deviceConnected)
	{
		return 0xff;
	}
	*/

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

			// Confirmed on FM TOWNS 2MX:
			// If the high 3-bits of the second byte (Logical Unit ID) is non zero,
			// Towns SCSI controller returns status code 02H (CHECK CONDITION).
			// I still don't understand what this Logical Unit ID is.
			if(0!=((state.commandBuffer[1]>>5)&7))
			{
				state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
				state.status=STATUSCODE_CHECK_CONDITION;
				state.message=0;
				EnterStatusPhase();
			}
			else
			{
				ExecSCSICommand();
			}
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
	case SCSICMD_SENSE:
		EnterDataInPhase();
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
	case SCSICMD_READ_6:
	case SCSICMD_READ_10:
		if(SCSIDEVICE_HARDDISK==state.dev[state.selId].devType ||
		   SCSIDEVICE_CDROM==state.dev[state.selId].devType)
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
	case SCSICMD_WRITE_6:
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
	case SCSICMD_VERIFY_10:
		if (SCSIDEVICE_HARDDISK==state.dev[state.selId].devType)
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
	case SCSICMD_READTOC:
	case SCSICMD_READ_SUBCHANNEL:
		if (SCSIDEVICE_CDROM==state.dev[state.selId].devType)
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
	case SCSICMD_PLAY_AUDIO_MSF:
		if(SCSIDEVICE_CDROM==state.dev[state.selId].devType)
		{
			auto start=DiscImage::MakeMSF(state.commandBuffer[3],state.commandBuffer[4],state.commandBuffer[5]);
			auto end=DiscImage::MakeMSF(state.commandBuffer[6],state.commandBuffer[7],state.commandBuffer[8]);
			start-=DiscImage::MakeMSF(0,2,0);
			end-=DiscImage::MakeMSF(0,2,0);
			if(nullptr!=outsideworld)
			{
				outsideworld->CDDAPlay(state.dev[state.selId].discImg,start,end,false,255,255); // There is no repeat.  Electric Volume not connected to SCSI CD.
			}
			state.dev[state.selId].CDDAEndTime=end;
			state.status=STATUSCODE_GOOD;
			state.message=0; // What am I supposed to return?
			state.senseKey=SENSEKEY_NO_SENSE;
			EnterStatusPhase();
		}
		else
		{
			state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
			state.status=STATUSCODE_CHECK_CONDITION;
			state.message=0; // What am I supposed to return?
			EnterStatusPhase();
		}
		break;
	case SCSICMD_PAUSE_RESUME:
		if(SCSIDEVICE_CDROM==state.dev[state.selId].devType)
		{
			if(state.commandBuffer[8]&1) // Resume
			{
				outsideworld->CDDAResume();
			}
			else
			{
				outsideworld->CDDAPause();
			}
			state.status=STATUSCODE_GOOD;
			state.message=0; // What am I supposed to return?
			state.senseKey=SENSEKEY_NO_SENSE;
			EnterStatusPhase();
		}
		else
		{
			state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
			state.status=STATUSCODE_CHECK_CONDITION;
			state.message=0; // What am I supposed to return?
			EnterStatusPhase();
		}
		break;
	case SCSICMD_SEEK_6:
	case SCSICMD_SEEK_10:
		state.status=STATUSCODE_GOOD;
		state.message=0; // What am I supposed to return?
		state.senseKey=SENSEKEY_NO_SENSE;
		EnterStatusPhase();
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
			case SCSICMD_SENSE:
				{
					unsigned char senseData[8]={0,0,0,0,0,0,0,0};
					senseData[0]=0xF0;
					senseData[2]=state.senseKey;

					townsPtr->dmac.DeviceToMemory(DMACh,sizeof(senseData),senseData);
					townsPtr->dmac.SetDMATransferEnd(TOWNSDMA_SCSI);
					state.status=STATUSCODE_GOOD;
					state.message=0;
					EnterStatusPhase();
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
			case SCSICMD_READ_6:
			case SCSICMD_READ_10:
				if(SCSIDEVICE_HARDDISK==state.dev[state.selId].devType ||
				   SCSIDEVICE_CDROM==state.dev[state.selId].devType)
				{
					auto dataPtr=ioThread.GetData();
					if(nullptr==dataPtr)
					{
						townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+IOTHREAD_WAIT_INTERVAL);
						break;
					}

					auto &data=*dataPtr;

					townsPtr->NotifyDiskRead();

					unsigned int LBA,LEN;

					if(SCSICMD_READ_10==state.commandBuffer[0])
					{
						LBA=(state.commandBuffer[2]<<24)|
						    (state.commandBuffer[3]<<16)|
						    (state.commandBuffer[4]<<8)|
						     state.commandBuffer[5];
						LEN=(state.commandBuffer[7]<<8)|
						     state.commandBuffer[8];
					}
					else
					{
						LBA=((state.commandBuffer[1]&0x1F)<<16)|
						    (state.commandBuffer[2]<<8)|
						     state.commandBuffer[3];
						LEN= state.commandBuffer[4];
						if(0==LEN)
						{
							LEN=256;
						}
					}

					unsigned int bytesTransferred=0;
					bytesTransferred=townsPtr->dmac.DeviceToMemory(
					    DMACh,
					    data.size()-state.bytesTransferred,
					    data.data()+state.bytesTransferred);

					if(SCSIDEVICE_HARDDISK==state.dev[state.selId].devType)
					{
						LEN*=HARDDISK_SECTOR_LENGTH;
					}
					else if(SCSIDEVICE_CDROM==state.dev[state.selId].devType)
					{
						LEN*=CDROM_SECTOR_LENGTH;
					}

					if(0<bytesTransferred)
					{
						state.bytesTransferred+=bytesTransferred;
						townsPtr->dmac.SetDMATransferEnd(TOWNSDMA_SCSI);
					}
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
			case SCSICMD_READ_SUBCHANNEL:
				if(SCSIDEVICE_CDROM==state.dev[state.selId].devType)
				{
					// Tentatively return CDDA not in progress.
					unsigned char subQData[48];
					for(auto &b : subQData)
					{
						b=0;
					}
					// subQData[1]=11h Playing 12H Paused 13H Completed 14H Error 15H No CDDA State
					// subQData[6]  Track
					// subQData[7]  Index
					// subQData[8]  Absolute CD Addr MSB
					// subQData[9]  Absolute CD Addr
					// subQData[10]  Absolute CD Addr
					// subQData[11]  Absolute CD Addr LSB
					// subQData[12]  Track relative CD Addr MSB
					// subQData[13]  Track relative CD Addr
					// subQData[14]  Track relative CD Addr
					// subQData[15]  Track relative CD Addr LSB
					if(true==outsideworld->CDDAIsPlaying())
					{
						subQData[1]=0x11;

						auto discTime=outsideworld->CDDACurrentPosition();
						auto trackTime=state.dev[state.selId].discImg.DiscTimeToTrackTime(discTime);
						discTime+=DiscImage::MakeMSF(0,2,0);

						subQData[6]=trackTime.track;

						subQData[9]=(unsigned char)discTime.min;
						subQData[10]=(unsigned char)discTime.sec;
						subQData[11]=(unsigned char)discTime.frm;
						subQData[13]=(unsigned char)trackTime.MSF.min;
						subQData[14]=(unsigned char)trackTime.MSF.sec;
						subQData[15]=(unsigned char)trackTime.MSF.frm;

						state.dev[state.selId].CDDAWasPlaying=true;
					}
					else if(true==state.dev[state.selId].CDDAWasPlaying)
					{
						// WAV playback and CDDA playback time not exactly match.
						// Make sure CDDA end time is reported at least once.

						subQData[1]=0x11;

						auto trackTime=state.dev[state.selId].discImg.DiscTimeToTrackTime(state.dev[state.selId].CDDAEndTime);

						subQData[6]=trackTime.track;

						subQData[9]=(unsigned char)state.dev[state.selId].CDDAEndTime.min;
						subQData[10]=(unsigned char)state.dev[state.selId].CDDAEndTime.sec;
						subQData[11]=(unsigned char)state.dev[state.selId].CDDAEndTime.frm;
						subQData[13]=(unsigned char)trackTime.MSF.min;
						subQData[14]=(unsigned char)trackTime.MSF.sec;
						subQData[15]=(unsigned char)trackTime.MSF.frm;

						state.dev[state.selId].CDDAWasPlaying=false;
					}
					else
					{
						subQData[1]=0x13;
					}
					subQData[2]=0x00;  subQData[3]=44;




					townsPtr->dmac.DeviceToMemory(DMACh,sizeof(subQData),subQData);
					townsPtr->dmac.SetDMATransferEnd(TOWNSDMA_SCSI);
					state.status=STATUSCODE_GOOD;
					state.message=0;
					EnterStatusPhase();
				}
				else
				{
					state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
					state.status=STATUSCODE_CHECK_CONDITION;
					state.message=0; // What am I supposed to return?
					EnterStatusPhase();
				}
				break;
			case SCSICMD_READTOC:
				if(SCSIDEVICE_CDROM==state.dev[state.selId].devType &&
				   (state.commandBuffer[6]<=state.dev[state.selId].discImg.GetNumTracks() ||
				    0xAA==state.commandBuffer[6]))  // 0xAA returns the total length.
				{
					auto tocData=MakeTOCData(
					    state.selId,
					    state.commandBuffer[6],
					    (state.commandBuffer[7]<<8)|state.commandBuffer[8],
					    (0!=(state.commandBuffer[1]&2)));

					townsPtr->dmac.DeviceToMemory(DMACh,tocData);
					townsPtr->dmac.SetDMATransferEnd(TOWNSDMA_SCSI);
					state.status=STATUSCODE_GOOD;
					state.message=0;
					EnterStatusPhase();
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
			case SCSICMD_WRITE_6:
			case SCSICMD_WRITE_10:
				if(SCSIDEVICE_HARDDISK==state.dev[state.selId].devType)
				{
					unsigned int LBA,LEN;
					if(state.commandBuffer[0]==SCSICMD_WRITE_10)
					{
						LBA=(state.commandBuffer[2]<<24)|
						    (state.commandBuffer[3]<<16)|
						    (state.commandBuffer[4]<<8)|
						     state.commandBuffer[5];
						LEN=(state.commandBuffer[7]<<8)|
						     state.commandBuffer[8];
					}
					else
					{
						LBA=((state.commandBuffer[1]&0x1F)<<16)|
						    (state.commandBuffer[2]<<8)|
						     state.commandBuffer[3];
						LEN= state.commandBuffer[4];
						if(0==LEN)
						{
							LEN=256;
						}
					}

					LBA*=HARDDISK_SECTOR_LENGTH;
					LEN*=HARDDISK_SECTOR_LENGTH;

					auto toWrite=townsPtr->dmac.MemoryToDevice(DMACh,LEN-state.bytesTransferred);
					if(0==toWrite.size())
					{
						if(LEN<=state.bytesTransferred) // DOS6 installer sends LEN==0.
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
						townsPtr->dmac.SetDMATransferEnd(TOWNSDMA_SCSI);
						ioThread.WaitReady();
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
				}
				else
				{
					state.senseKey=SENSEKEY_ILLEGAL_REQUEST;
					state.status=STATUSCODE_CHECK_CONDITION;
					state.message=0; // What am I supposed to return?
					EnterStatusPhase();
				}
				break;
			case SCSICMD_VERIFY_10:
				if (SCSIDEVICE_HARDDISK==state.dev[state.selId].devType)
				{
					auto bytChk=(state.commandBuffer[1]&0x02)!=0;

					if (bytChk==false) {
						state.status=STATUSCODE_GOOD;
						state.message=0;
						EnterStatusPhase();
					} else {
						townsPtr->debugger.ExternalBreak("DATA OUT Phase: VERIFY(10) with bytChk=1 Not Supported.");
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

std::vector <unsigned char> TownsSCSI::MakeTOCData(int scsiId,unsigned int startTrack,unsigned int allocSize,bool MSF) const
{
	std::vector <unsigned char> dat;

	if(SCSIDEVICE_CDROM==state.dev[scsiId].devType)
	{
		// Placeholder for TOC length
		dat.push_back(0);
		dat.push_back(0);

		dat.push_back(1);
		dat.push_back(state.dev[scsiId].discImg.GetNumTracks());

		if(0xAA==startTrack)
		{
			dat.push_back(0);
			dat.push_back(0);
			dat.push_back(0);
			dat.push_back(0);

			auto length=state.dev[scsiId].discImg.GetNumSectors()+DiscImage::HSG_BASE;
			if(true==MSF)
			{
				auto MSF=DiscImage::HSGtoMSF(length);
				dat.push_back(0);
				dat.push_back((unsigned char)MSF.min);
				dat.push_back((unsigned char)MSF.sec);
				dat.push_back((unsigned char)MSF.frm);
			}
			else
			{
				dat.push_back((length>>24)&0xFF);
				dat.push_back((length>>16)&0xFF);
				dat.push_back((length>>8)&0xFF);
				dat.push_back( length    &0xFF);
			}
		}
		else
		{
			auto &allTracks=state.dev[scsiId].discImg.GetTracks();
			for(auto trk=startTrack; trk<=state.dev[scsiId].discImg.GetNumTracks(); ++trk)
			{
				auto &t=allTracks[trk-1];
				unsigned char ADR_CTRL=0;
				if(DiscImage::TRACK_AUDIO==t.trackType)
				{
					ADR_CTRL=0x00;
				}
				else
				{
					ADR_CTRL=0x14;
				}
				dat.push_back(0);
				dat.push_back(ADR_CTRL);
				dat.push_back(trk);
				dat.push_back(0);

				auto start=t.start+t.preGap;
				if(true==MSF)
				{
					start+=DiscImage::MakeMSF(0,2,0);  // 2 seconds magic
					dat.push_back(0);
					dat.push_back((unsigned char)start.min);
					dat.push_back((unsigned char)start.sec);
					dat.push_back((unsigned char)start.frm);
				}
				else
				{
					unsigned int LBA=DiscImage::MSFtoHSG(start);
					dat.push_back((LBA>>24)&0xFF);
					dat.push_back((LBA>>16)&0xFF);
					dat.push_back((LBA>>8)&0xFF);
					dat.push_back( LBA    &0xFF);
				}

				if(allocSize<=dat.size())
				{
					break;
				}
			}
		}

		if(allocSize<dat.size())
		{
			dat.resize(allocSize);
		}

		dat[0]=(dat.size()>>8)&0xFF;
		dat[1]= dat.size()    &0xFF;
	}

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

/* virtual */ uint32_t TownsSCSI::SerializeVersion(void) const
{
	return 1;
}
/* virtual */ void TownsSCSI::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	std::string stateDir,stateName;
	cpputil::SeparatePathFile(stateDir,stateName,stateFName);

	for(auto &dev : state.dev)
	{
		PushUint32(data,dev.devType);
		PushString(data,dev.imageFName);
		PushString(data,cpputil::MakeRelativePath(dev.imageFName,stateDir));
		PushInt64(data,dev.imageSize);
	}

	PushBool(data,state.deviceConnected);

	PushUint32(data,state.nCommandFilled);
	for(auto c : state.commandBuffer)
	{
		PushUint16(data,c);
	}
	PushUint32(data,state.bytesTransferred);

	PushBool(data,state.REQ);
	PushBool(data,state.I_O);
	PushBool(data,state.MSG);
	PushBool(data,state.C_D);
	PushBool(data,state.BUSY);
	PushBool(data,state.INT);
	PushBool(data,state.PERR);
	PushBool(data,state.DMAE);
	PushBool(data,state.SEL);
	PushBool(data,state.ATN);
	PushBool(data,state.IMSK);
	PushBool(data,state.WEN);

	PushUint32(data,state.selId);
	PushUint32(data,state.phase);
	PushUint32(data,state.lastDataByte);

	PushUint16(data,state.status);
	PushUint16(data,state.message);
	PushUint32(data,state.senseKey);
}
/* virtual */ bool TownsSCSI::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	std::string stateDir,stateName;
	cpputil::SeparatePathFile(stateDir,stateName,stateFName);

	for(auto &dev : state.dev)
	{
		dev.devType=ReadUint32(data);
		std::string fName=ReadString(data);
		std::string relPath;
		if(1<=version)
		{
			relPath=ReadString(data);
		}
		ReadInt64(data); // Dummy read
		// Do not auto-load hard-disk image.
		if(SCSIDEVICE_CDROM==dev.devType)
		{
			// See disk-image search rule in townsstate.cpp
			bool loaded=false;

			// (1) Try using the filename stored in the state file as is.
			if(true!=loaded)
			{
				if(cpputil::FileExists(fName) &&
				   DiscImage::ERROR_NOERROR!=dev.discImg.Open(fName))
				{
					dev.imageFName=fName;
					loaded=true;
				}
			}

			// (2) Try state path+relative path
			auto stateRel=cpputil::MakeFullPathName(stateDir,relPath);
			if(true!=loaded)
			{
				if(cpputil::FileExists(stateRel) &&
				   DiscImage::ERROR_NOERROR!=dev.discImg.Open(stateRel))
				{
					dev.imageFName=stateRel;
					loaded=true;
				}
			}

			// (3) Try image search path+file name
			// No search paths for SCSI CD-ROM.

			// (4) Try state path+file name
			if(true!=loaded)
			{
				std::string imgDir,imgName;
				cpputil::SeparatePathFile(imgDir,imgName,fName);
				auto ful=cpputil::MakeFullPathName(stateDir,imgName);
				if(cpputil::FileExists(ful) &&
				   DiscImage::ERROR_NOERROR!=dev.discImg.Open(ful))
				{
					dev.imageFName=ful;
					loaded=true;
				}
			}
		}
	}

	state.deviceConnected=ReadBool(data);

	state.nCommandFilled=ReadUint32(data);
	for(auto &c : state.commandBuffer)
	{
		c=ReadUint16(data);
	}
	state.bytesTransferred=ReadUint32(data);

	state.REQ=ReadBool(data);
	state.I_O=ReadBool(data);
	state.MSG=ReadBool(data);
	state.C_D=ReadBool(data);
	state.BUSY=ReadBool(data);
	state.INT=ReadBool(data);
	state.PERR=ReadBool(data);
	state.DMAE=ReadBool(data);
	state.SEL=ReadBool(data);
	state.ATN=ReadBool(data);
	state.IMSK=ReadBool(data);
	state.WEN=ReadBool(data);

	state.selId=ReadUint32(data);
	state.phase=ReadUint32(data);
	state.lastDataByte=ReadUint32(data);

	state.status=ReadUint16(data);
	state.message=ReadUint16(data);
	state.senseKey=ReadUint32(data);

	return true;
}
