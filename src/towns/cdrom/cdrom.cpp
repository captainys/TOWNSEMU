/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <math.h>
#include "discimg.h"
#include "cdrom.h"
#include "townsdef.h"
#include "towns.h"
#include "cpputil.h"

TownsCDROM::AsyncWaveReader::AsyncWaveReader()
{
}
TownsCDROM::AsyncWaveReader::~AsyncWaveReader()
{
	if(STATE_IDLE!=GetState())
	{
		thr.join();
		state=STATE_IDLE;
	}
}
unsigned int TownsCDROM::AsyncWaveReader::GetState(void)
{
	stateLock.lock();
	auto stateCopy=state;
	stateLock.unlock();
	return stateCopy;
}
void TownsCDROM::AsyncWaveReader::Start(DiscImage *discImg,DiscImage::MinSecFrm from,DiscImage::MinSecFrm to)
{
	if(STATE_IDLE!=GetState())
	{
		thr.join(); // Just in case.  Flush the last task.
	}
	this->discImg=discImg;
	this->from=from;
	this->to=to;
	std::thread t(&TownsCDROM::AsyncWaveReader::ThreadFunc,this);
	std::swap(t,thr);
}
std::vector <unsigned char> &TownsCDROM::AsyncWaveReader::GetWave(void)
{
	thr.join();
	state=STATE_IDLE;
	return wave;
}
void TownsCDROM::AsyncWaveReader::ThreadFunc(void)
{
	wave=discImg->GetWave(from,to);
	stateLock.lock();
	state=STATE_DATAREADY;
	stateLock.unlock();
}

////////////////////////////////////////////////////////////

TownsCDROM::State::State()
{
	imgPtr=new DiscImage;
}
TownsCDROM::State::~State()
{
	delete imgPtr;
}
const DiscImage &TownsCDROM::State::GetDisc(void) const
{
	return *imgPtr;
}
DiscImage &TownsCDROM::State::GetDisc(void)
{
	return *imgPtr;
}
void TownsCDROM::State::ClearStatusQueue(void)
{
	statusQueue.clear();
}
void TownsCDROM::State::PushStatusQueue(unsigned char d0,unsigned char d1,unsigned char d2,unsigned char d3)
{
	statusQueue.push_back(d0);
	statusQueue.push_back(d1);
	statusQueue.push_back(d2);
	statusQueue.push_back(d3);
}
void TownsCDROM::State::Reset(void)
{
	ResetMPU();
	enableSIRQ=false;
	enableDEI=false;
	discChanged=false;
	CDDAState=CDDA_IDLE;
	nextCDDAPollingTime=0;
	CDDAEndTime.Set(0,2,0);

	CDDAWave.clear();
	CDDAPlayPointer=0;
}

void TownsCDROM::UpdateCDDAStateInternal(long long int townsTime)
{
	state.nextCDDAPollingTime=townsTime+CDDA_POLLING_INTERVAL;
	if(CDDA_PLAYING==state.CDDAState)
	{
		if(state.CDDAWave.size()<=state.CDDAPlayPointer)
		{
			state.CDDAState=CDDA_STOPPING;
		}
	}
	else if(CDDA_STOPPING==state.CDDAState)
	{
		state.CDDAState=CDDA_ENDED;
	}
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
	for(int i=0; i<8; ++i)
	{
		paramQueue[i]=0;
	}
	readingSectorHSG=0;
	endSectorHSG=0;
	headPositionHSG=0;
	ClearStatusQueue();
	DMATransfer=false;
	CPUTransfer=false;
}

////////////////////////////////////////////////////////////

TownsCDROM::TownsCDROM(class FMTownsCommon *townsPtr,class TownsPIC *PICPtr,class TownsDMAC *DMACPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
	this->PICPtr=PICPtr;
	this->DMACPtr=DMACPtr;
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
	bool cmdOrParam=false;
	switch(ioport)
	{
	case TOWNSIO_CDROM_MASTER_CTRL_STATUS://0x4C0, // [2] pp.224
		if(0!=(data&0x80)) // SMIC: Clear SIRQ
		{
			state.SIRQ=false;
			if(true!=state.SIRQ && true!=state.DEI)
			{
				PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,false);
			}
		}
		if(0!=(data&0x40)) // DEIC: Clera DMA-End IRQ
		{
			state.DEI=false;
			if(true!=state.SIRQ && true!=state.DEI)
			{
				PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,false);
			}
		}
		if(0!=(data&0x04))
		{
			state.ResetMPU();
		}
		state.enableSIRQ=(0!=(data&0x02));
		state.enableDEI=(0!=(data&0x01));
		break;
	case TOWNSIO_CDROM_COMMAND_STATUS://    0x4C2, // [2] pp.224
		// std::cout << "CDROM Command " << cpputil::Ubtox(data) << std::endl;
		if(true==state.SIRQ) // If a new command is sent before clearing the previous command.
		{
			// For Fractal Engine Demo.
			// Only explanation I can imagine why command 00H returns the state and shoots an IRQ.
			data|=(state.cmd&0x60);
		}
		state.cmdReceived=true;
		state.cmd=data;
		var.lastCmdIssuedAt.SEG=townsPtr->CPU().state.CS().value;
		var.lastCmdIssuedAt.OFFSET=townsPtr->CPU().state.EIP;
		cmdOrParam=true;
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
		var.lastParamWrittenAt.SEG=townsPtr->CPU().state.CS().value;
		var.lastParamWrittenAt.OFFSET=townsPtr->CPU().state.EIP;
		cmdOrParam=true;
		break;
	case TOWNSIO_CDROM_TRANSFER_CTRL://     0x4C6, // [2] pp.227
		state.DMATransfer=(0!=(data&0x10));
		state.CPUTransfer=(0!=(data&0x08));
		if(true==state.DMATransfer)
		{
			state.WaitForDTSSTS=false;
			auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_CDROM);
			bool DMAAvailable=(nullptr!=DMACh && (0<DMACh->currentCount && 0xFFFFFFFF!=(DMACh->currentCount&0xFFFFFFFF)));
			if(true==DMAAvailable && true!=state.DTSF)
			{
				if((DMACh->modeCtrl&0x0C)!=0x04)
				{
					// Confirmed in real FM TOWNS 2MX.
					// If DMA is configured to I/O to Memory, CPU freezes when DTS is set.
					Abort(
						"Whole system freezes when DTS is set, but the DMA direction is not set as I/O to MEM.\n"
						"Confirmed in real FM TOWNS 2MX.  Presumably DMA locks the memory bus indefinitely.");
				}
				state.DTSF=true;
				townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+state.readSectorTime);
			}
		}
		else if(true==state.CPUTransfer)
		{
			state.WaitForDTSSTS=false;
			state.STSF=true;
		}
		break;
	case TOWNSIO_CDROM_SUBCODE_STATUS://    0x4CC,
		// No write
		break;
	case TOWNSIO_CDROM_SUBCODE_DATA://      0x4CD,
		// No write
		break;
	}

	// Regarding cmdOrParam flag:
	// AWESOME bangs on 04C0h, and constantly clear DEI and SIRQ, regardless of the events.
	// The following check must only be done when command is written or parameter queue changes,
	// or the command is reset every time 04C0h is banged.
	if(true==cmdOrParam && true==state.cmdReceived && PARAM_QUEUE_LEN<=state.nParamQueue)
	{
		ExecuteCDROMCommand();
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
		data|=(0<state.statusQueue.size() ? 2 : 0);
		data|=(state.DRY  ?    1 : 0);
		return data;
	case TOWNSIO_CDROM_COMMAND_STATUS://    0x4C2, // [2] pp.224
		if(0<state.statusQueue.size())
		{
			data=state.statusQueue.front();
			for(int i=0; i<state.statusQueue.size()-1; ++i)
			{
				state.statusQueue[i]=state.statusQueue[i+1];
			}
			state.statusQueue.pop_back();
			return data;
		}
		break;
	case TOWNSIO_CDROM_CACHE_2XSPEED:
		break;
	case TOWNSIO_CDROM_PARAMETER_DATA://    0x4C4, // [2] pp.224
		if(true==state.STSF)
		{
			if(0==var.sectorCacheForCPUTransfer.size())
			{
				if(CDCMD_MODE1READ==(state.cmd&0x9F))
				{
					var.sectorCacheForCPUTransfer=state.GetDisc().ReadSectorMODE1(state.readingSectorHSG,1);
				}
				else if(CDCMD_MODE2READ==(state.cmd&0x9F))
				{
					var.sectorCacheForCPUTransfer=state.GetDisc().ReadSectorMODE2(state.readingSectorHSG,1);
				}
				else
				{
					var.sectorCacheForCPUTransfer=state.GetDisc().ReadSectorRAW(state.readingSectorHSG,1);
				}
			}

			if(state.CPUTransferPointer<var.sectorCacheForCPUTransfer.size())
			{
				unsigned char data=var.sectorCacheForCPUTransfer[state.CPUTransferPointer++];
				if(var.sectorCacheForCPUTransfer.size()<=state.CPUTransferPointer)
				{
					++state.readingSectorHSG;
					townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+NOTIFICATION_TIME);
					state.CPUTransfer=false;
					state.STSF=false;
					state.DEI=true;  // It's CPU transfer.  Should it set DMA END Interrupt?
					state.CPUTransferPointer=0;
					var.sectorCacheForCPUTransfer.clear();
					if(true==var.debugBreakOnDEI)
					{
						townsPtr->debugger.ExternalBreak("CD-ROM Software Read End");
					}
				}
				return data;
			}
			else
			{
				Abort("CDROM I/O Read 04C4H Overflow");
			}
		}
		else
		{
			// Just in case, it may be VM error.
			Abort("CDROM I/O Read 04C4H while STSF=false");
		}
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

std::vector <std::string> TownsCDROM::GetStatusText(void) const
{
	std::vector <std::string> text;

	text.push_back("");
	text.back()+="Last Command:"+cpputil::Ubtox(state.cmd);

	switch(state.cmd&0x9F)
	{
	case CDCMD_SEEK://       0x00,
		text.back()+="(SEEK)";
		break;
	case CDCMD_MODE2READ://  0x01,
		text.back()+="(MODE2READ)";
		break;
	case CDCMD_MODE1READ://  0x02,
		text.back()+="(MODE1READ)";
		break;
	case CDCMD_RAWREAD://    0x03,
		text.back()+="(RAWREAD)";
		break;
	case CDCMD_CDDAPLAY://   0x04,
		text.back()+="(CDDAPLAY)";
		break;
	case CDCMD_TOCREAD://    0x05,
		text.back()+="(TOCREAD)";
		break;
	case CDCMD_SUBQREAD://   0x06,
		text.back()+="(SUBQREAD)";
		break;
	case CDCMD_UNKNOWN1://   0x1F, // NOP and requst status? I guess?
		text.back()+="(UNKNOWN1)";
		break;

	case CDCMD_SETSTATE://   0x80,
		text.back()+="(SETSTATE)";
		break;
	case CDCMD_CDDASET://    0x81,
		text.back()+="(CDDASET)";
		break;
	case CDCMD_CDDASTOP://   0x84,
		text.back()+="(CDDASTOP)";
		break;
	case CDCMD_CDDAPAUSE://  0x85,
		text.back()+="(CDDAPAUSE)";
		break;
	case CDCMD_UNKNOWN2://   0x86,
		text.back()+="(UNKNOWN2)";
		break;
	case CDCMD_CDDARESUME:// 0x87,
		text.back()+="(CDDARESUME)";
		break;
	}
	if(0!=(state.cmd&0x40))
	{
		text.back()+="+IRQ";
	}
	if(0!=(state.cmd&0x20))
	{
		text.back()+="+REQSTA";
	}


	text.push_back("Last Command Param:");
	for(int i=0; i<8; ++i)
	{
		text.back()+=cpputil::Ubtox(var.lastParam[i])+" ";
	}

	text.push_back("Param Queue (Towns->CD):");
	for(int i=0; i<state.nParamQueue; ++i)
	{
		text.back()+=cpputil::Ubtox(state.paramQueue[i])+" ";
	}

	text.push_back("Last Command Issued At:");
	text.back()+=var.lastCmdIssuedAt.Format();
	text.back()+="   Last Parameter Written At:";
	text.back()+=var.lastParamWrittenAt.Format();

	text.push_back("Reading Sector(HSG):");
	text.back()+=cpputil::Itoa(state.readingSectorHSG);
	text.back()+="  End Sector(HSG):";
	text.back()+=cpputil::Itoa(state.endSectorHSG);

	text.push_back("Status Queue (CD->Towns):");
	for(auto d : state.statusQueue)
	{
		text.back()+=cpputil::Ubtox(d)+" ";
	}

	text.push_back("DMA Transfer:");
	text.back()+=(true==state.DMATransfer ? "1" : "0");
	text.back()+=("  CPU Transfer:");
	text.back()+=(true==state.CPUTransfer ? "1" : "0");

	text.push_back("");
	text.back()+="SIRQ:";
	text.back()+=(true==state.SIRQ ? "1" : "0");
	text.back()+="  DEI:";
	text.back()+=(true==state.DEI ? "1" : "0");
	text.back()+="  STSF:";
	text.back()+=(true==state.STSF ? "1" : "0");
	text.back()+="  DTSF:";
	text.back()+=(true==state.DTSF ? "1" : "0");
	text.back()+="  DRY:";
	text.back()+=(true==state.DRY ? "1" : "0");
	text.back()+="  enableSIRQ:";
	text.back()+=(true==state.enableSIRQ ? "1" : "0");
	text.back()+="  enableDEI:";
	text.back()+=(true==state.enableDEI ? "1" : "0");

	text.push_back("CDDA:");
	switch(state.CDDAState)
	{
	case CDDA_IDLE:
		text.back()+="IDLE";
		break;
	case CDDA_PLAYING:
		text.back()+="PLAYING";
		break;
	case CDDA_PAUSED:
		text.back()+="PAUSED";
		break;
	case CDDA_STOPPING:
		text.back()+="STOPPING";
		break;
	case CDDA_ENDED:
		text.back()+="ENDED";
		break;
	default:
		text.back()+="!!Undefined!!";
		break;
	}

	return text;
}

unsigned int TownsCDROM::LoadDiscImage(const std::string &fName)
{
	while(AsyncWaveReader::STATE_BUSY==waveReader.GetState());

	std::string ext=cpputil::GetExtension(fName.c_str());
	cpputil::Capitalize(ext);
	if(".BIN"==ext || ".IMG"==ext)
	{
		std::string cueFName=cpputil::RemoveExtension(fName.c_str());
		cueFName+=".CUE";
		if(DiscImage::ERROR_NOERROR==state.GetDisc().Open(cpputil::FindFileWithSearchPaths(cueFName,searchPaths)))
		{
			state.discChanged=true;
			return true;
		}
		cueFName=cpputil::RemoveExtension(fName.c_str());
		cueFName+=".cue";
		if(DiscImage::ERROR_NOERROR==state.GetDisc().Open(cpputil::FindFileWithSearchPaths(cueFName,searchPaths)))
		{
			state.discChanged=true;
			return true;
		}
	}

	state.discChanged=true;
	return state.GetDisc().Open(cpputil::FindFileWithSearchPaths(fName,searchPaths));
}

void TownsCDROM::Eject(void)
{
	state.GetDisc().CleanUp();
}

void TownsCDROM::BreakOnCommandCheck(const char phase[])
{
	if(true==var.debugBreakOnCommandWrite || true==var.debugMonitorCommandWrite)
	{
		bool commandTypeCheck=true;
		if(0xFFFF!=var.debugBreakOnSpecificCommand && (state.cmd&0x9F)!=(var.debugBreakOnSpecificCommand&0x9F))
		{
			commandTypeCheck=false;
		}
		if(true==commandTypeCheck)
		{
			std::string msg="CDROM Command ";
			msg+=phase;
			if(true==var.debugBreakOnCommandWrite)
			{
				townsPtr->debugger.ExternalBreak(msg);
			}
			std::cout << "CDROM Command " << cpputil::Ubtox(state.cmd) << " |";
			for(int i=0; i<8; ++i)
			{
				std::cout << cpputil::Ubtox(state.paramQueue[i]) << " ";
			}
			std::cout << std::endl;
		}
	}
}

void TownsCDROM::ExecuteCDROMCommand(void)
{
	BreakOnCommandCheck("Write");
	switch(state.cmd&0x9F)
	{
	case CDCMD_CDDAPLAY://   0x04,
		PrepareCDDAPlay();
		break;
	}
	state.DRY=false;
	state.delayedSIRQ=true;
	townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DELAYED_STATUS_IRQ_TIME);
}

void TownsCDROM::PrepareCDDAPlay(void)
{
	DiscImage::MinSecFrm msfBegin,msfEnd;
	auto offset=DiscImage::MakeMSF(0,2,0);

	msfBegin.min=DiscImage::BCDToBin(state.paramQueue[0]);
	msfBegin.sec=DiscImage::BCDToBin(state.paramQueue[1]);
	msfBegin.frm=DiscImage::BCDToBin(state.paramQueue[2]);
	msfBegin-=offset;

	msfEnd.min=DiscImage::BCDToBin(state.paramQueue[3]);
	msfEnd.sec=DiscImage::BCDToBin(state.paramQueue[4]);
	msfEnd.frm=DiscImage::BCDToBin(state.paramQueue[5]);
	msfEnd-=offset;

	waveReader.Start(&state.GetDisc(),msfBegin,msfEnd);
}

void TownsCDROM::DelayedCommandExecution(unsigned long long int townsTime)
{
	BreakOnCommandCheck("Exec");

	state.DRY=true; // Tentative
	state.delayedSIRQ=false;

	for(int i=0; i<8; ++i)
	{
		var.lastParam[i]=state.paramQueue[i];
	}

	switch(state.cmd&0x9F)
	{
	case CDCMD_SEEK://       0x00,
		{
			state.DRY=true;  // Apparently CDC can take command while the head is moving.
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+SEEK_TIME);
			state.ClearStatusQueue();
			if(true==StatusRequestBit(state.cmd))
			{
				if(true!=SetStatusDriveNotReadyOrDiscChanged())
				{
					SetStatusNoError();
					if(0!=(CMDFLAG_IRQ&state.cmd))
					{
						PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
						state.SIRQ=true;
					}
				}
			}
		}
		break;
	case CDCMD_MODE1READ://  0x02,
	case CDCMD_MODE2READ://  0x01,
	case CDCMD_RAWREAD://    0x03,
		{
			// CDDA needs to stop when MODE1READ is sent while playing.
			state.CDDAState=CDDA_IDLE;

			// TownsOS V2.1 L20 issues MODE1READ command without checking the status by GETSTATE.
			// This causes an issue when redirecting Internal CD-ROM to external CD-ROM.
			// Drive Not Ready must be checked in here.
			// Also, the 1st byte of the error code looks to be non-zero.
			// Probably the logic is:
			//    GETSTATE when CD media is not in  -> GETSTATE command itself succeeds (1st byte==0), but the drive is not ready (2nd byte==9)
			//    MODE1READ when CD media is not in -> MODE1READ command fails (1st byte=0x21), and the drive is not ready (2nd byte=9)
			if(0==state.GetDisc().GetNumTracks())
			{
				state.PushStatusQueue(0x21,9,0,0);
				break;
			}

			DiscImage::MinSecFrm msfBegin,msfEnd;

			msfBegin.min=DiscImage::BCDToBin(state.paramQueue[0]);
			msfBegin.sec=DiscImage::BCDToBin(state.paramQueue[1]);
			msfBegin.frm=DiscImage::BCDToBin(state.paramQueue[2]);
			msfEnd.min=DiscImage::BCDToBin(state.paramQueue[3]);
			msfEnd.sec=DiscImage::BCDToBin(state.paramQueue[4]);
			msfEnd.frm=DiscImage::BCDToBin(state.paramQueue[5]);

			BeginReadSector(msfBegin,msfEnd);
		}
		break;
	case CDCMD_CDDAPLAY://   0x04,
		if(AsyncWaveReader::STATE_DATAREADY!=waveReader.GetState())
		{
			state.delayedSIRQ=true;
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DELAYED_STATUS_IRQ_TIME);
		}
		else
		{
			// I realized ChaseHQ go into infinite loop unless Status Queue is cleared.
			// I'm wondering if I should do the same for all other commands.
			state.ClearStatusQueue();

			state.DRY=true;

			DiscImage::MinSecFrm msfBegin,msfEnd;
			auto offset=DiscImage::MakeMSF(0,2,0);

			msfBegin.min=DiscImage::BCDToBin(state.paramQueue[0]);
			msfBegin.sec=DiscImage::BCDToBin(state.paramQueue[1]);
			msfBegin.frm=DiscImage::BCDToBin(state.paramQueue[2]);
			msfBegin-=offset;

			msfEnd.min=DiscImage::BCDToBin(state.paramQueue[3]);
			msfEnd.sec=DiscImage::BCDToBin(state.paramQueue[4]);
			msfEnd.frm=DiscImage::BCDToBin(state.paramQueue[5]);
			msfEnd-=offset;

			bool repeat=(1==state.paramQueue[6]); // Should I say 0!= ?
			state.CDDAWave=waveReader.GetWave();;
			state.CDDAPlayPointer=0;

			state.CDDAState=CDDA_PLAYING;
			state.CDDAStartTime=msfBegin;
			state.CDDAEndTime=msfEnd;
			state.CDDARepeat=repeat;

			if(true==StatusRequestBit(state.cmd))
			{
				SetStatusDriveNotReadyOrDiscChangedOrNoError();
				state.SIRQ=true;
				if(0!=(state.cmd&CMDFLAG_IRQ) && true==state.enableSIRQ)
				{
					PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
				}
			}

			townsPtr->OnCDDAStart();
		}
		break;
	case CDCMD_TOCREAD://    0x05,
		// Shadow of the Beast 2 issues command 05H (STATUS_REQUEST=0) and expects to get 0x16 status byte immediately
		// without No-Error status.  (It probably is what STATUS_REQUEST bit means).
		state.statusQueue.clear();
		if(true==StatusRequestBit(state.cmd))
		{
			if(true==SetStatusDriveNotReadyOrDiscChanged())
			{
				return;
			}
			SetStatusNoError();
		}
		SetStatusQueueForTOC();
		break;
	case CDCMD_SUBQREAD://   0x06,
		if(true==StatusRequestBit(state.cmd))
		{
			SetStatusSubQRead();
		}
		break;
	case CDCMD_UNKNOWN1://   0x1F, // NOP and requst status? I guess?
		std::cout << "CDROM Command " << cpputil::Ubtox(state.cmd) << " function unknown." << std::endl;
		std::cout << "Currently just return no-error status." << std::endl;
		if(true==StatusRequestBit(state.cmd))
		{
			if(true==SetStatusDriveNotReadyOrDiscChanged())
			{
				return;
			}
			SetStatusNoError();
			if(CMDFLAG_IRQ&state.cmd)
			{
				SetSIRQ_IRR();
			}
		}
		break;
	case CDCMD_UNKNOWN3://   0x9F, ? Used by TownsMagazine Vol.2
		std::cout << "CDROM Command " << cpputil::Ubtox(state.cmd) << " function unknown." << std::endl;
		std::cout << "Currently just returns error status." << std::endl;
		state.PushStatusQueue(0x21,0,0,0);
		break;
	case CDCMD_SETSTATE://   0x80,
		if(true==StatusRequestBit(state.cmd))
		{
			townsPtr->UnscheduleDeviceCallBack(*this);
			SetStatusDriveNotReadyOrDiscChangedOrNoError();
			if(CDDA_ENDED==state.CDDAState)
			{
				// 2020/07/30
				// Vain Dream crashes when CD BIOS Call AX=53C0H returns an error because the BIOS is expecting
				// status 00 00 00 00, but this PushStatusCDDAPlayEnded() pushes 07 00 00 00.
				// The retry code jumps to 4600:0084
				//     4600:0084 1E                        PUSH    DS
				//     4600:0085 8CC8                      MOV     AX,CS
				// However, it should really jump to 4600:0085.  By jumping to 4600:0084, PUSH DS moves SP
				// by two bytes, and the subsequent RETF fails to return to the correct address.
				//
				// Vain Dream runs by commenting out PushStatusCDDAPlayEnded() below.  But,
				// in the past I saw something (presumably one version of CD-ROM BIOS) was expecting
				// 07 00 00 00 status after CDDA is done, but I cannot find it
				//
				// Maybe subsequent to CDDA Stop command?

				// PushStatusCDDAPlayEnded();
				state.CDDAState=CDDA_IDLE;
			}
			if(CMDFLAG_IRQ&state.cmd)
			{
				SetSIRQ_IRR();
			}
		}
		break;
	case CDCMD_CDDASET://    0x81,
		// ChaseHQ issues command 0xC1 and expects no status is returned.
		// Therefore this command should not return status if STATUS-REQUEST bit is off.
		// It still compares the 1st status byte ang 0x08, but the meaning is unknown.
		if(true==StatusRequestBit(state.cmd))
		{
			// I don't know what to do with this command.
			// CDROM BIOS AH=52H fires command 0xA1 with parameters {07 FF 00 00 00 00 00 00}
			SetStatusDriveNotReadyOrDiscChangedOrNoError();
		}
		break;
	case CDCMD_CDDASTOP://   0x84,
		if(true==CDDAIsPlaying())
		{
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+CDDASTOP_TIME);
		}
		else
		{
			StopCDDA();  // Already stopped, but it sets up status queue.
		}
		break;
	case CDCMD_CDDAPAUSE://  0x85,
		// Fix for ChaseHQ.
		// CDDAState must be reset to IDLE regardless of the Status Request.
		// ChaseHQ was issuing CDDAPAUSE command without Status Request flag.
		state.CDDAState=CDDA_PAUSED;
		if(true==StatusRequestBit(state.cmd))
		{
			if(true!=SetStatusDriveNotReadyOrDiscChanged())
			{
				state.PushStatusQueue(0,0x01,0,0);  // 2nd byte=01 means PAUSED.
				state.PushStatusQueue(0x12,0,0,0);
			}
		}
		break;
	case CDCMD_UNKNOWN2://   0x86,
		std::cout << "CDROM Command " << cpputil::Ubtox(state.cmd) << " not implemented yet." << std::endl;
		break;
	case CDCMD_CDDARESUME:// 0x87,
		if(CDDA_PAUSED==state.CDDAState)
		{
			state.CDDAState=CDDA_PLAYING;
		}
		if(true==StatusRequestBit(state.cmd))
		{
			if(true!=SetStatusDriveNotReadyOrDiscChanged())
			{
				state.PushStatusQueue(0,0,0,0);
				state.PushStatusQueue(0x13,0,0,0);
			}
		}
		break;
	}

	state.cmdReceived=false;
	state.nParamQueue=0;
}

void TownsCDROM::BeginReadSector(DiscImage::MinSecFrm msfBegin,DiscImage::MinSecFrm msfEnd)
{
	state.readingSectorHSG=msfBegin.ToHSG();
	state.endSectorHSG=msfEnd.ToHSG();
	if(state.readingSectorHSG>state.endSectorHSG || state.readingSectorHSG<150) // 150frames=two seconds
	{
		SetStatusParameterError();
		state.readingSectorHSG=0;
		state.endSectorHSG=0;
	}
	else
	{
		state.readingSectorHSG-=150;
		state.endSectorHSG-=150;

		uint64_t distance;
		if(state.readingSectorHSG<state.headPositionHSG)
		{
			distance=state.headPositionHSG-state.readingSectorHSG;
		}
		else
		{
			distance=state.readingSectorHSG-state.headPositionHSG;
		}
		uint64_t seekTime=distance;
		seekTime*=state.maxSeekTime;
		seekTime/=MAX_NUM_SECTORS;

		// Shadow of the Beast issues command 02H and expects status to be returned.
		// Probably MODE2READ command needs to disregard STATUS REQUEST bit.

		// Should I immediately return No-Error status before starting transfer?
		// BIOS is not checking it immediately.
		// 2F Boot ROM _IS_ checking it immediately.
		SetStatusNoError();
		if(true==state.enableSIRQ)
		{
			state.SIRQ=true;
			PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
		}
		townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+state.readSectorTime+seekTime+var.sectorReadTimeDelay);

		state.DRY=false;
		state.DTSF=false;
		state.WaitForDTSSTS=false;
	}
}

/* virtual */ void TownsCDROM::RunScheduledTask(unsigned long long int townsTime)
{
	if(true==state.delayedSIRQ)
	{
		DelayedCommandExecution(townsTime);
		return;
	}

	switch(state.cmd&0x9F)
	{
	case CDCMD_SEEK://       0x00,
		{
			// I think I should not push status queue in here unless true==StatusRequestBit(state.cmd),
			// and same for IRR flag of PIC unless 0!=(CMDFLAG_IRQ&state.cmd).
			// However, Fractal Engine Demo expect Status Queue and IRR from command 00H.
			//
			// Command 00H looks to be from a coding error.  Pretty much a CDC command in 
			// DS:[27E4H] was cleared from the previous command execution.  Then the same byte
			// is used for the next command before a value is written.
			//
			// My guess is that the following two instructinons:
			//
			// 000C:00001986 A0E4270000                MOV     AL,[000027E4H]
			// 000C:0000198B EE                        OUT     DX,AL
			//
			// should be something like:
			//		MOV		AL,060H
			//		MOV		[000027E4H],AL
			//		OUT		DX,AL
			//
			// One very strange coding error that Fractal Engine Demo has is it writes the
			// command before clearing SIRQ and DEI from the previous command.
			// Probably, very probably, because the SIRQ from previous command (62H) is not cleared
			// when 00H is shot, the command may have inherited STATUS request and IRQ flag from
			// the previous command.

			// Probably status code 4 means Seek Done.
			// FM Towns 2F BIOS waits for No Error (00H) and then waits for 04H after issuing command 20H Seek.
			if(true==StatusRequestBit(state.cmd))
			{
				state.PushStatusQueue(4,0,0,0);
				if(0!=(CMDFLAG_IRQ&state.cmd))
				{
					PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
					state.SIRQ=true;
				}
			}
		}
		break;

	case CDCMD_MODE1READ://  0x02,
	case CDCMD_MODE2READ://  0x01,
	case CDCMD_RAWREAD://    0x03,
		{
			if(state.readingSectorHSG<=state.endSectorHSG) // Have more data.
			{
				auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_CDROM);
				bool DMAAvailable=(nullptr!=DMACh && (0<DMACh->currentCount && 0xFFFFFFFF!=(DMACh->currentCount&0xFFFFFFFF)));

				if(true==state.WaitForDTSSTS)
				{
					// I was avoiding to implement LOST DATA because I thought correctly written program won't cuase LOST DATA.
					// However, looks like Shadow of the Beast 2 is intentionally causing lost data in the following procedure:
					//
					//     Loop to clear CD status bytes:
					//     000C:0006E918 D0E8                      SHR     AL,1
					//     000C:0006E91A 7312                      JAE     0006E92E  Jump if not SRQ
					//     000C:0006E91C B2C2                      MOV     DL,C2H
					//     000C:0006E91E EC                        IN      AL,DX     Clear status bytes
					//     000C:0006E91F C1C808                    ROR     EAX,08H
					//     000C:0006E922 EC                        IN      AL,DX     Clear status bytes
					//     000C:0006E923 C1C808                    ROR     EAX,08H
					//     000C:0006E926 EC                        IN      AL,DX     Clear status bytes
					//     000C:0006E927 C1C808                    ROR     EAX,08H
					//     000C:0006E92A EC                        IN      AL,DX     Clear status bytes
					//     000C:0006E92B C1C808                    ROR     EAX,08H
					//     000C:0006E92E 66BAC004                  MOV     DX,04C0H
					//     000C:0006E932 EC                        IN      AL,DX
					//     000C:0006E933 B0C0                      MOV     AL,C0H    Clear SIRQ and DEI
					//     000C:0006E935 EE                        OUT     DX,AL
					//     000C:0006E936 B2C0                      MOV     DL,C0H
					//     Procedure Entry:
					//     000C:0006E938 EC                        IN      AL,DX
					//     000C:0006E939 D0E8                      SHR     AL,1
					//     000C:0006E93B 75DB                      JNE     0006E918 { Loop to clear CD status bytes:}
					//     000C:0006E93D 73F7                      JAE     0006E936
					//     000C:0006E93F C3                        RET
					//
					// This procedure is called when the user presses a game-pad button in the event scene after the menu.
					// The event scene looks to be rendered using a similar technique used in the Fractal Engine.
					// The program needs to abort the rendering by aborting the CDC command, and it did it by just
					// clearing DEI and SIRQ and status bytes.  It is a violent way of terminating a command, but
					// if it is what it does, I need to emulate lost data.

					// Means DMA was not set up in time.
					state.ClearStatusQueue();
					state.PushStatusQueue(0x21,0x0F,0,0); // Abnormal termination.  I don't know which error to return.
					state.SIRQ=false;
					if(true==StatusRequestBit(state.cmd) && 0!=(state.cmd&CMDFLAG_IRQ) && true==state.enableSIRQ)
					{
						PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
						state.SIRQ=true;
					}
					state.DRY=true;
					state.DEI=false;
					state.DTSF=false;
					state.STSF=false;
					if(CDCMD_MODE1READ==(state.cmd&0x9F))
					{
						std::cout << "MODE1READ time out." << std::endl;
					}
					else if(CDCMD_MODE2READ==(state.cmd&0x9F))
					{
						std::cout << "MODE2READ time out." << std::endl;
					}
					else
					{
						std::cout << "RAWREAD time out." << std::endl;
					}
					return;
				}

				// Initial State.  CPU doesn't know data ready, therefore does not make DMAC available.
				if(true!=DMAAvailable || true!=state.DMATransfer)
				{
					if(true==PICPtr->GetInterruptRequestBit(TOWNSIRQ_CDROM) &&
					   true==state.DEI)
					{
						// If the previous IRR hasn't been consumed, check back again after STATUS_CHECKBACK_TIME.
						// This interpretation is most likely wrong.  But, Shadow of the Beast 2's CD-ROM IRQ handler
						// is expecting alternating SIRQ(Data Ready) and DEI, and missing one DEI due to IF=0 will
						// put it into the infinite DRY-wait loop.
						// Two changes to deal with this issue.  One is LOSTDATA_TIMEOUT.  Make sure if the CPU
						// doesn't read an available data in time, just let it time out, and DRY=1.
						// Also, let Data Ready on hold if the previous IRR hasn't been consumed.
						// Actually IRR should be an output pin from CDC and input pin of PIC.
						// Therefore, CDC owns IRR.  So, it is entirely possible that CDC waits next Data Ready
						// until the previous IRR from DEI is consumed.

						// To try this, press a gamepad button during the fat brick dragon in the game-start
						// demo is visible.

						// This check contradicted with Yumimi Mix.  Therefore, I made it so that it also checks
						// true==state.DEI.  It clears Yumimi Mix.
						townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+STATUS_CHECKBACK_TIME);
						return;
					}

					state.ClearStatusQueue();

					if(true==var.debugBreakOnDataReady)
					{
						townsPtr->debugger.ExternalBreak("CD-ROM Data Ready");
					}

					// See above comment about Shadow of the Beast for why not checking STATUS REQUEST bit for setting status.
					SetStatusDataReady();

					if(true==StatusRequestBit(state.cmd) && 0!=(state.cmd&CMDFLAG_IRQ) && true==state.enableSIRQ)
					{
						PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
					}

					state.SIRQ=true;
					state.DEI=false;
					state.DTSF=false;

					// Write to DTS (state.DMATransfer) should trigger the DMA transfer.
					// If the CPU does not do so before the LOSTDATA_TIMEOUT, abort the command.
					state.WaitForDTSSTS=true;
					townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+LOSTDATA_TIMEOUT);
				}
				// Second State.  DMA available, but the transfer hasn't started.  Wait for the CPU to write DTS=1.
				// Third State.  Transfer done for a sector.
				else if(true==DMAAvailable && true==state.DMATransfer && true==state.DTSF)
				{
					townsPtr->NotifyDiskRead();

					state.headPositionHSG=state.readingSectorHSG;

					std::vector <unsigned char> data;
					if(CDCMD_MODE1READ==(state.cmd&0x9F))
					{
						data=state.GetDisc().ReadSectorMODE1(state.readingSectorHSG,1);
					}
					else if(CDCMD_MODE2READ==(state.cmd&0x9F))
					{
						data=state.GetDisc().ReadSectorMODE2(state.readingSectorHSG,1);
					}
					else
					{
						data=state.GetDisc().ReadSectorRAW(state.readingSectorHSG,1);
					}
					if(DMACh->currentCount+1<data.size())
					{
						data.resize(DMACh->currentCount+1);
					}
					DMACPtr->DeviceToMemory(DMACh,data);
					DMACPtr->SetDMATransferEnd(TOWNSDMA_CDROM);
					++state.readingSectorHSG;
					townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+NOTIFICATION_TIME);
					state.DMATransfer=false;
					state.DTSF=false;  // Should I turn it off also? -> Looks like I should.  Based on 2MX SYSROM FC00:00001CF7.  It waits for DTSF to clear.
					state.DEI=true;

					if(true==var.debugBreakOnDEI)
					{
						townsPtr->debugger.ExternalBreak("CD-ROM DMA End Interrupt");
					}

					if(true==state.enableDEI)
					{
						PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
						// No more interrupt.  End of transfer.
					}
				}
			}
			else
			{
				state.DRY=true;
				state.ClearStatusQueue();
				state.DTSF=false;

				// See above comment about Shadow of the Beast for why not checking STATUS REQUEST bit for setting status.
				SetStatusReadDone();

				if(true==StatusRequestBit(state.cmd))
				{
					state.SIRQ=true;
					if(0!=(state.cmd&CMDFLAG_IRQ) && true==state.enableSIRQ)
					{
						PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
					}
				}
				else
				{
					state.SIRQ=false;
				}
			}
		}
		break;
	case CDCMD_CDDASTOP:
		StopCDDA();  // Status queue set in StopCDDA().
		break;
	}
}
void TownsCDROM::SetStatusDriveNotReadyOrDiscChangedOrNoError(void)
{
	if(true!=SetStatusDriveNotReadyOrDiscChanged())
	{
		SetStatusNoError();
	}
}
bool TownsCDROM::SetStatusDriveNotReadyOrDiscChanged(void)
{
	if(0==state.GetDisc().GetNumTracks())
	{
		SetStatusDriveNotReady();
		return true;
	}
	else if(true==state.discChanged)
	{
		SetStatusDiscChanged();
		state.discChanged=false;
		return true;
	}
	return false;
}
void TownsCDROM::SetStatusNoError(void)
{
	unsigned char next2ndByteOfStatusCode=0;
	if(CDDA_PAUSED==state.CDDAState)
	{
		// Probably: Response to A0H, 00 01 xx xx means CDDA is paused.
		// 0256:00001F01 A03100                    MOV     AL,[0031H]   // Second byte of the status code, if the first byte is 00H
		// 0256:00001F04 3C01                      CMP     AL,01H
		// 0256:00001F06 7408                      JE      00001F10
		// 0256:00001F08 E83006                    CALL    0000253B
		// 0256:00001F0B E8EF05                    CALL    000024FD { ()}
		// 0256:00001F0E EB02                      JMP     00001F12
		// 0256:00001F10 B422                      MOV     AH,22H   // Already-Paused Error.
		// 0256:00001F12 C3                        RET
		next2ndByteOfStatusCode=0x01;
	}
	else if(true==CDDAIsPlaying())
	{
		next2ndByteOfStatusCode=0x03;
		// Probably: Response to A0H (80H+REQSTA), 00 03 xx xx means CDDA is playing.
		// Confirmed: Shadow of the Beast 2 checks the 2nd byte to be 03 for verifying that the CDDA started playing.
		//    000C:0006F30B 80FC03                    CMP     AH,03H
		//    000C:0006F30E 75ED                      JNE     0006F2FD
	}
	state.PushStatusQueue(0,next2ndByteOfStatusCode,0,0);
}
void TownsCDROM::SetStatusDriveNotReady(void)
{
	// BIOS Disassembly suggests Drive-Not-Ready is 00 09 xx xx.  V2.1 L20 0421:277C
	state.PushStatusQueue(0,9,0,0);
}
void TownsCDROM::SetStatusDiscChanged(void)
{
	state.PushStatusQueue(0x21,8,0,0);
}
void TownsCDROM::SetStatusReadDone(void)
{
	state.PushStatusQueue(0x06,0,0,0);
}
void TownsCDROM::SetStatusHardError(void)
{
	state.PushStatusQueue(0x21,04,0,0);
}
void TownsCDROM::SetStatusParameterError(void)
{
	state.PushStatusQueue(0x21,01,0,0);
}
void TownsCDROM::SetStatusDataReady(void)
{
	state.PushStatusQueue(0x22,0,0,0);
}
void TownsCDROM::SetStatusQueueForTOC(void)
{
	auto &disc=state.GetDisc();

	// Based on Shadow of the Beast reverse engineering,
	// first status pair should be 0x16 xx 0xA0 xx, 0x17 first-track xx xx
	state.PushStatusQueue(0x16,0,0xA0,0);
	state.PushStatusQueue(0x17,1,0,0);

	// Based on Shadow of the Beast reverse engineering,
	// second status pair should be 0x16 xx 0xA1 xx, 0x17 NumberOfTracksBCD xx xx
	state.PushStatusQueue(0x16,0,0xA1,0);
	state.PushStatusQueue(0x17,DiscImage::BinToBCD(disc.GetNumTracks()),0,0);

	// Based on Shadow of the Beast reverse engineering,
	// third status pair should be 0x16 xx 0xA2 xx, 0x17 (disc length)
	auto length=DiscImage::HSGtoMSF(disc.GetNumSectors()+DiscImage::HSG_BASE);
	state.PushStatusQueue(0x16,0,0xA2,0);
	state.PushStatusQueue(0x17,
	                      DiscImage::BinToBCD(length.min),
	                      DiscImage::BinToBCD(length.sec),
	                      DiscImage::BinToBCD(length.frm));

	// Shadow of the Beast looks to be expecting:
	//   0x16 TrackType (TrackNumberBCD) xx
	int trkNum=1;
	for(auto &trk : disc.GetTracks())
	{
		unsigned char secondByte=(trk.trackType==DiscImage::TRACK_AUDIO ? 0 : 0x40);
		state.PushStatusQueue(0x16,secondByte,DiscImage::BinToBCD(trkNum),0);

		// F29 Retaliator is expecting trk.start+trk.preGap (2 seconds).
		// Actually, probably what I should do is add 2 seconds to all the tracks,
		// and then subtract 2 seconds when play back.

		// Advantage Tennis doesn't like it.
		// It seems that I need to return trk.start+trk.preGap, and then
		// subtract 2 seconds when play back.

		// Only problem: Wing Commander intro skips first two seconds.

		auto HSG=DiscImage::MSFtoHSG(trk.start+trk.preGap);
		HSG+=DiscImage::HSG_BASE;
		auto MSF=DiscImage::HSGtoMSF(HSG);

		state.PushStatusQueue(0x17,
	                      DiscImage::BinToBCD(MSF.min),
	                      DiscImage::BinToBCD(MSF.sec),
	                      DiscImage::BinToBCD(MSF.frm));

		++trkNum;
	}
}

void TownsCDROM::SetStatusSubQRead(void)
{
	if(true==SetStatusDriveNotReadyOrDiscChanged())
	{
		return;
	}

	// From reverse-engineering of CD-ROM BIOS, it is most likely:
	// 00 00 00 00  <- First return no error.
	// 18H xx trkBCD xx
	// 19H trkTimeBCD trkTimeBCD trkTimeBCD
	// 19H xx discTimeBCD discTimeBCD
	// 20H  discTimeBCD xx xx
	state.PushStatusQueue(0,0,0,0);

	DiscImage::MinSecFrm twoSec,discTime;
	twoSec.Set(0,2,0);

	if(CDDA_PLAYING==state.CDDAState)
	{
		uint64_t playPtr=state.CDDAPlayPointer;
		playPtr/=4;
		playPtr*=SECTOR_PER_SEC_1X;
		playPtr/=CDDA_SAMPLING_RATE;
		playPtr+=state.CDDAStartTime.ToHSG();

		discTime.FromHSG(playPtr);

		if(state.CDDAEndTime<discTime)
		{
			discTime=state.CDDAEndTime;
		}
	}
	else
	{
		discTime=state.CDDAEndTime;
	}

	auto trackTime=state.GetDisc().DiscTimeToTrackTime(discTime);
	discTime+=twoSec;

	state.PushStatusQueue(
		0x18,
		0,
		DiscImage::BinToBCD(trackTime.track),
		0);
	state.PushStatusQueue(
		0x19,
		DiscImage::BinToBCD(trackTime.MSF.min),
		DiscImage::BinToBCD(trackTime.MSF.sec),
		DiscImage::BinToBCD(trackTime.MSF.frm));
	state.PushStatusQueue(
		0x19,
		0,
		DiscImage::BinToBCD(discTime.min),
		DiscImage::BinToBCD(discTime.sec));
	state.PushStatusQueue(
		0x20,
		DiscImage::BinToBCD(discTime.frm),
		0,
		0);
}

void TownsCDROM::PushStatusCDDAStopDone(void)
{
	state.PushStatusQueue(0x11,0,0,0);
}

void TownsCDROM::PushStatusCDDAPlayEnded(void)
{
	state.PushStatusQueue(0x07,0,0,0);
}

void TownsCDROM::StopCDDA(void)
{
	state.ClearStatusQueue();
	if(true!=SetStatusDriveNotReadyOrDiscChanged())
	{
		state.CDDAState=CDDA_ENDED;
		SetStatusNoError();
		PushStatusCDDAStopDone();
		state.PushStatusQueue(0,0x0D,0,0);
	}
	else
	{
		// See fix for ChaseHQ in CDDAPAUSE.
		state.CDDAState=CDDA_IDLE;
	}
}

void TownsCDROM::SetSIRQ_IRR(void)
{
	if(0<state.statusQueue.size())
	{
		state.SIRQ=true;
		PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
	}
}

/* virtual */ uint32_t TownsCDROM::SerializeVersion(void) const
{
	// Version 4
	//   Added CPUTransferPointer
	// Version 5
	//   Added headPositionHSG
	return 5;
}
/* virtual */ void TownsCDROM::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	std::string stateDir,stateName;
	cpputil::SeparatePathFile(stateDir,stateName,stateFName);

	PushString(data,state.GetDisc().fName);
	PushString(data,cpputil::MakeRelativePath(state.GetDisc().fName,stateDir));

	PushBool(data,state.SIRQ); // 4C0H bit 7
	PushBool(data,state.DEI);  // 4C0H bit 6 (DMA End Flag)
	PushBool(data,state.STSF); // 4C0H bit 5 (Software Transfer End)
	PushBool(data,state.DTSF); // 4C0H bit 4 (DMA Transfer in progress)
	PushBool(data,state.DRY);  // 4C0H bit 0 (Ready to receive command)

	PushBool(data,state.enableSIRQ);
	PushBool(data,state.enableDEI);

	PushBool(data,state.cmdReceived);
	PushUint16(data,state.cmd);
	PushInt32(data,state.nParamQueue);
	for(auto p : state.paramQueue)
	{
		PushUint16(data,p);
	}
	for(auto q : state.statusQueue)
	{
		PushUint16(data,q);
	}

	PushUint32(data,state.readingSectorHSG);
	PushUint32(data,state.endSectorHSG);
	PushUint32(data,state.headPositionHSG);

	PushBool(data,state.DMATransfer);
	PushBool(data,state.CPUTransfer); // Both are not supposed to be 1, but I/O can set it that way.
	PushBool(data,state.WaitForDTSSTS);

	PushBool(data,state.discChanged);

	PushBool(data,state.delayedSIRQ);

	uint64_t CDDAPlayingSector=state.CDDAPlayPointer;
	CDDAPlayingSector/=4;
	CDDAPlayingSector*=SECTOR_PER_SEC_1X; // 75 sector per second
	CDDAPlayingSector/=CDDA_SAMPLING_RATE; // Make it sector offset.
	auto CDDAPlayingTime=state.CDDAStartTime.ToHSG()+CDDAPlayingSector;

	PushUint32(data,state.CDDAState);
	PushInt64(data,state.nextCDDAPollingTime);
	PushUint32(data,state.CDDAStartTime.ToHSG());
	PushUint32(data,CDDAPlayingTime);
	PushUint32(data,state.CDDAEndTime.ToHSG());
	PushBool(data,state.CDDARepeat);

	PushUint32(data,state.readSectorTime); // Version 3 or newer.

	PushUint32(data,state.CPUTransferPointer); // Version 4 or newer
}
/* virtual */ bool TownsCDROM::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	std::string stateDir,stateName;
	cpputil::SeparatePathFile(stateDir,stateName,stateFName);



	std::string fName=ReadString(data);
	std::string relPath;
	if(1<=version)
	{
		relPath=ReadString(data);
	}


	// See disk-image search rule in townsstate.cpp
	bool loaded=false;

	// (1) Try using the filename stored in the state file as is.
	if(true!=loaded)
	{
		if(cpputil::FileExists(fName) &&
		   DiscImage::ERROR_NOERROR!=state.GetDisc().Open(fName))
		{
			loaded=true;
		}
	}

	// (2) Try state path+relative path
	auto stateRel=cpputil::MakeFullPathName(stateDir,relPath);
	if(true!=loaded)
	{
		if(cpputil::FileExists(stateRel) &&
		   DiscImage::ERROR_NOERROR!=state.GetDisc().Open(stateRel))
		{
			loaded=true;
		}
	}

	// (3) Try image search path+file name
	if(true!=loaded)
	{
		std::string imgDir,imgName;
		cpputil::SeparatePathFile(imgDir,imgName,fName);
		for(auto dir : searchPaths)
		{
			auto ful=cpputil::MakeFullPathName(dir,imgName);
			if(cpputil::FileExists(ful) &&
			   DiscImage::ERROR_NOERROR!=state.GetDisc().Open(ful))
			{
				loaded=true;
			}
		}
	}

	// (4) Try state path+file name
	if(true!=loaded)
	{
		std::string imgDir,imgName;
		cpputil::SeparatePathFile(imgDir,imgName,fName);
		auto ful=cpputil::MakeFullPathName(stateDir,imgName);
		if(cpputil::FileExists(ful) &&
		   DiscImage::ERROR_NOERROR!=state.GetDisc().Open(ful))
		{
			loaded=true;
		}
	}



	state.SIRQ=ReadBool(data); // 4C0H bit 7
	state.DEI=ReadBool(data);  // 4C0H bit 6 (DMA End Flag)
	state.STSF=ReadBool(data); // 4C0H bit 5 (Software Transfer End)
	state.DTSF=ReadBool(data); // 4C0H bit 4 (DMA Transfer in progress)
	state.DRY=ReadBool(data);  // 4C0H bit 0 (Ready to receive command)

	state.enableSIRQ=ReadBool(data);
	state.enableDEI=ReadBool(data);

	state.cmdReceived=ReadBool(data);
	state.cmd=ReadUint16(data);
	state.nParamQueue=ReadInt32(data);
	for(auto &p : state.paramQueue)
	{
		p=ReadUint16(data);
	}
	for(auto &q : state.statusQueue)
	{
		q=ReadUint16(data);
	}

	state.readingSectorHSG=ReadUint32(data);
	state.endSectorHSG=ReadUint32(data);
	if(5<=version)
	{
		state.headPositionHSG=ReadUint32(data);
	}
	else
	{
		state.headPositionHSG=0;
	}

	state.DMATransfer=ReadBool(data);
	state.CPUTransfer=ReadBool(data); // Both are not supposed to be 1, but I/O can set it that way.
	state.WaitForDTSSTS=ReadBool(data);

	state.discChanged=ReadBool(data);
    
	state.delayedSIRQ=ReadBool(data);
    
	state.CDDAState=ReadUint32(data);
	state.nextCDDAPollingTime=ReadInt64(data);
	if(2<=version)
	{
		state.CDDAStartTime.FromHSG(ReadUint32(data));

		uint64_t CDDAPlayingTime=ReadUint32(data);
		CDDAPlayingTime-=state.CDDAStartTime.ToHSG();
		CDDAPlayingTime*=CDDA_SAMPLING_RATE;
		CDDAPlayingTime/=SECTOR_PER_SEC_1X;
		CDDAPlayingTime*=4;
		state.CDDAPlayPointer=CDDAPlayingTime;

		state.CDDAEndTime.FromHSG(ReadUint32(data));
		state.CDDARepeat=ReadBool(data);
	}
	else
	{
		state.CDDAState=CDDA_IDLE;
		state.CDDAEndTime.FromHSG(ReadUint32(data));
	}

	if(3<=version)
	{
		state.readSectorTime=ReadUint32(data);
	}

	state.CPUTransferPointer=0;
	if(4<=version)
	{
		state.CPUTransferPointer=ReadUint32(data);
	}
	var.sectorCacheForCPUTransfer.clear();
	return true;
}

void TownsCDROM::ResumeCDDAAfterRestore(void)
{
	unsigned int leftLinear=255;
	unsigned int rightLinear=255;
	if(CDDA_PLAYING==state.CDDAState || CDDA_PAUSED==state.CDDAState)
	{
		state.CDDAWave=state.GetDisc().GetWave(state.CDDAStartTime,state.CDDAEndTime);
		// state.CDDAPlayPointer should have already been set.
	}
}

void TownsCDROM::AddWaveForNumSamples(unsigned char waveBuf[],unsigned int numSamples,int outSamplingRate)
{
	if(CDDA_SAMPLING_RATE!=outSamplingRate)
	{
		std::cout << "TownsCDROM::int AddWaveForNumSamples does not support other than " << CDDA_SAMPLING_RATE << "Hz" << std::endl;
		state.CDDAState=CDDA_STOPPING;
		return;
	}

	auto CDDAWaveSize=(state.CDDAWave.size()+3)&~3;  // Just in case, force it to be 4*N.
	if(0==CDDAWaveSize)
	{
		state.CDDAState=CDDA_STOPPING;
		return;
	}

	if(true==var.CDDAmute)
	{
		state.CDDAPlayPointer+=numSamples*4;
		if(true==state.CDDARepeat)
		{
			state.CDDAPlayPointer%=CDDAWaveSize;
		}
	}
	else
	{
		int Lvol=townsPtr->GetEleVolCDLeft();
		int Rvol=townsPtr->GetEleVolCDRight();
		if(true!=townsPtr->GetEleVolCDLeftEN())
		{
			Lvol=0;
		}
		if(true!=townsPtr->GetEleVolCDRightEN())
		{
			Rvol=0;
		}

		if(63==Lvol && 63==Rvol)
		{
			uint64_t writePtr=0;
			for(uint64_t i=0; i<numSamples && state.CDDAPlayPointer<CDDAWaveSize; ++i)
			{
				int L=cpputil::GetSignedWord(waveBuf+writePtr);
				int R=cpputil::GetSignedWord(waveBuf+writePtr+2);

				L+=cpputil::GetSignedWord(state.CDDAWave.data()+state.CDDAPlayPointer);
				R+=cpputil::GetSignedWord(state.CDDAWave.data()+state.CDDAPlayPointer+2);

				L=std::max(std::min(L,32767),-32767);
				R=std::max(std::min(R,32767),-32767);

				cpputil::PutWord(waveBuf+writePtr,L);
				cpputil::PutWord(waveBuf+writePtr+2,R);

				writePtr+=4;
				state.CDDAPlayPointer+=4;
				if(true==state.CDDARepeat && CDDAWaveSize<=state.CDDAPlayPointer)
				{
					state.CDDAPlayPointer=0;
				}
			}
		}
		else
		{
			uint64_t writePtr=0;
			for(uint64_t i=0; i<numSamples && state.CDDAPlayPointer<CDDAWaveSize; ++i)
			{
				int L=cpputil::GetSignedWord(waveBuf+writePtr);
				int R=cpputil::GetSignedWord(waveBuf+writePtr+2);

				int Lcd=cpputil::GetSignedWord(state.CDDAWave.data()+state.CDDAPlayPointer);
				int Rcd=cpputil::GetSignedWord(state.CDDAWave.data()+state.CDDAPlayPointer+2);

				Lcd=Lcd*Lvol/63;
				Rcd=Rcd*Rvol/63;

				L+=Lcd;
				R+=Rcd;

				L=std::max(std::min(L,32767),-32767);
				R=std::max(std::min(R,32767),-32767);

				cpputil::PutWord(waveBuf+writePtr,L);
				cpputil::PutWord(waveBuf+writePtr+2,R);

				writePtr+=4;
				state.CDDAPlayPointer+=4;
				if(true==state.CDDARepeat && CDDAWaveSize<=state.CDDAPlayPointer)
				{
					state.CDDAPlayPointer=0;
				}
			}
		}
		if(true!=state.CDDARepeat && CDDAWaveSize<=state.CDDAPlayPointer)
		{
			state.CDDAState=CDDA_STOPPING;
		}
	}
}
