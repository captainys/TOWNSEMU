/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include "discimg.h"
#include "cdrom.h"
#include "townsdef.h"
#include "towns.h"
#include "cpputil.h"
#include "outside_world.h"



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
	// Do not clear imgFileName on reset.
	ResetMPU();
	enableSIRQ=false;
	enableDEI=false;
	discChanged=false;
	CDDAState=CDDA_IDLE;
	nextCDDAPollingTime=0;
	CDDAEndTime.Set(0,2,0);
}

void TownsCDROM::SetOutsideWorld(class Outside_World *outside_world)
{
	this->OutsideWorld=outside_world;
}

void TownsCDROM::UpdateCDDAStateInternal(long long int townsTime,Outside_World &outside_world)
{
	state.nextCDDAPollingTime=townsTime+State::CDDA_POLLING_INTERVAL;
	if(State::CDDA_PLAYING==state.CDDAState)
	{
		if(true!=OutsideWorld->CDDAIsPlaying())
		{
			state.CDDAState=State::CDDA_STOPPING;
		}
	}
	else if(State::CDDA_STOPPING==state.CDDAState)
	{
		state.CDDAState=State::CDDA_ENDED;
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
	ClearStatusQueue();
	DMATransfer=false;
	CPUTransfer=false;
}

////////////////////////////////////////////////////////////

TownsCDROM::TownsCDROM(class FMTowns *townsPtr,class TownsPIC *PICPtr,class TownsDMAC *DMACPtr) : Device(townsPtr)
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
		var.lastCmdIssuedAt.SEG=townsPtr->cpu.state.CS().value;
		var.lastCmdIssuedAt.OFFSET=townsPtr->cpu.state.EIP;
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
		var.lastParamWrittenAt.SEG=townsPtr->cpu.state.CS().value;
		var.lastParamWrittenAt.OFFSET=townsPtr->cpu.state.EIP;
		break;
	case TOWNSIO_CDROM_TRANSFER_CTRL://     0x4C6, // [2] pp.227
		state.DMATransfer=(0!=(data&0x10));
		state.CPUTransfer=(0!=(data&0x08));
		if(true==state.DMATransfer)
		{
			auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_CDROM);
			bool DMAAvailable=(nullptr!=DMACh && (0<DMACh->currentCount && 0xFFFFFFFF!=(DMACh->currentCount&0xFFFFFFFF)));
			if(true==DMAAvailable && true!=state.DTSF)
			{
				state.DTSF=true;
				townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+READ_SECTOR_TIME);
			}
		}
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
	case State::CDDA_IDLE:
		text.back()+="IDLE";
		break;
	case State::CDDA_PLAYING:
		text.back()+="PLAYING";
		break;
	case State::CDDA_STOPPING:
		text.back()+="STOPPING";
		break;
	case State::CDDA_ENDED:
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
	std::string ext=cpputil::GetExtension(fName.c_str());
	cpputil::Capitalize(ext);
	if(".BIN"==ext || ".IMG"==ext)
	{
		std::string cueFName=cpputil::RemoveExtension(fName.c_str());
		cueFName+=".CUE";
		if(DiscImage::ERROR_NOERROR==state.GetDisc().Open(cueFName))
		{
			state.discChanged=true;
			return true;
		}
		cueFName=cpputil::RemoveExtension(fName.c_str());
		cueFName+=".cue";
		if(DiscImage::ERROR_NOERROR==state.GetDisc().Open(cueFName))
		{
			state.discChanged=true;
			return true;
		}
	}

	state.discChanged=true;
	return state.GetDisc().Open(fName);
}
void TownsCDROM::ExecuteCDROMCommand(void)
{
	if(true==var.debugBreakOnCommandWrite)
	{
		bool commandTypeCheck=true;
		if(0xFFFF!=var.debugBreakOnSpecificCommand && (state.cmd&0x9F)!=(var.debugBreakOnSpecificCommand&0x9F))
		{
			commandTypeCheck=false;
		}
		if(true==commandTypeCheck)
		{
			townsPtr->debugger.ExternalBreak("CDROM Command Exec");
			std::cout << "CDROM Command " << cpputil::Ubtox(state.cmd) << " |";
			for(int i=0; i<8; ++i)
			{
				std::cout << cpputil::Ubtox(state.paramQueue[i]) << " ";
			}
			std::cout << std::endl;
		}
	}

	// std::cout << "CDROM Command " << cpputil::Ubtox(state.cmd) << " |";
	// for(int i=0; i<8; ++i)
	// {
	// 	std::cout << cpputil::Ubtox(state.paramQueue[i]) << " ";
	// }
	// std::cout << std::endl;

	for(int i=0; i<8; ++i)
	{
		var.lastParam[i]=state.paramQueue[i];
	}

	state.delayedSIRQ=false;

	switch(state.cmd&0x9F)
	{
	case CDCMD_SEEK://       0x00,
		state.DRY=false;
		state.delayedSIRQ=true;
		townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DELAYED_STATUS_IRQ_TIME);
		break;
	case CDCMD_MODE2READ://  0x01,
		std::cout << "CDROM Command " << cpputil::Ubtox(state.cmd) << " not implemented yet." << std::endl;
		break;
	case CDCMD_MODE1READ://  0x02,
		{
			DiscImage::MinSecFrm msfBegin,msfEnd;

			msfBegin.min=DiscImage::BCDToBin(state.paramQueue[0]);
			msfBegin.sec=DiscImage::BCDToBin(state.paramQueue[1]);
			msfBegin.frm=DiscImage::BCDToBin(state.paramQueue[2]);
			msfEnd.min=DiscImage::BCDToBin(state.paramQueue[3]);
			msfEnd.sec=DiscImage::BCDToBin(state.paramQueue[4]);
			msfEnd.frm=DiscImage::BCDToBin(state.paramQueue[5]);

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
				if(0!=(CMDFLAG_STATUS_REQUEST&state.cmd))
				{
					// Should I immediately return No-Error status before starting transfer?
					// BIOS is not checking it immediately.
					// 2F Boot ROM _IS_ checking it immediately.
					SetStatusNoError();
					if(true==state.enableSIRQ)
					{
						state.delayedSIRQ=true;
						townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DELAYED_STATUS_IRQ_TIME);
					}
					else
					{
						townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+READ_SECTOR_TIME);
					}
				}
				else
				{
					townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+READ_SECTOR_TIME);
				}

				state.DRY=false;
				state.DTSF=false;
			}
		}
		break;
	case CDCMD_RAWREAD://    0x03,
		std::cout << "CDROM Command " << cpputil::Ubtox(state.cmd) << " not implemented yet." << std::endl;
		break;
	case CDCMD_CDDAPLAY://   0x04,
		state.DRY=false;
		state.delayedSIRQ=true;
		townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DELAYED_STATUS_IRQ_TIME);
		break;
	case CDCMD_TOCREAD://    0x05,
		if(CMDFLAG_STATUS_REQUEST&state.cmd)
		{
			SetStatusQueueForTOC();
		}
		break;
	case CDCMD_SUBQREAD://   0x06,
		if(CMDFLAG_STATUS_REQUEST&state.cmd)
		{
			SetStatusSubQRead();
		}
		break;
	case CDCMD_UNKNOWN1://   0x1F, // NOP and requst status? I guess?
		std::cout << "CDROM Command " << cpputil::Ubtox(state.cmd) << " not implemented yet." << std::endl;
		break;

	case CDCMD_SETSTATE://   0x80,
		if(CMDFLAG_STATUS_REQUEST&state.cmd)
		{
			townsPtr->UnscheduleDeviceCallBack(*this);
			SetStatusDriveNotReadyOrDiscChangedOrNoError();
			if(State::CDDA_ENDED==state.CDDAState)
			{
				PushStatusCDDAPlayEnded();
				state.CDDAState=State::CDDA_IDLE;
			}
			if(CMDFLAG_IRQ&state.cmd)
			{
				SetSIRQ_IRR();
			}
		}
		break;
	case CDCMD_CDDASET://    0x81,
		if(CMDFLAG_STATUS_REQUEST&state.cmd)
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
		if(nullptr!=OutsideWorld)
		{
			OutsideWorld->CDDAPause();
		}
		// Fix for ChaseHQ.
		// CDDAState must be reset to IDLE regardless of the Status Request.
		// ChaseHQ was issuing CDDAPAUSE command without Status Request flag.
		state.CDDAState=State::CDDA_IDLE;
		if(CMDFLAG_STATUS_REQUEST&state.cmd)
		{
			if(true!=SetStatusDriveNotReadyOrDiscChanged())
			{
				state.PushStatusQueue(0,0,0,0);
				state.PushStatusQueue(0x12,0,0,0);
			}
		}
		break;
	case CDCMD_UNKNOWN2://   0x86,
		std::cout << "CDROM Command " << cpputil::Ubtox(state.cmd) << " not implemented yet." << std::endl;
		break;
	case CDCMD_CDDARESUME:// 0x87,
		if(nullptr!=OutsideWorld)
		{
			OutsideWorld->CDDAResume();
		}
		if(CMDFLAG_STATUS_REQUEST&state.cmd)
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

	// Tentatively Drive Not Ready.
}
/* virtual */ void TownsCDROM::RunScheduledTask(unsigned long long int townsTime)
{
	switch(state.cmd&0x9F)
	{
	case CDCMD_SEEK://       0x00,
		// delayedSIRQ is used in CDCMD_SEEK only when 0!=(state.cmd&CMDFLAG_STATUS_REQUEST)
		if(true==state.delayedSIRQ)
		{
			state.DRY=true;  
			state.delayedSIRQ=false;
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+SEEK_TIME);
			state.ClearStatusQueue();
			if(0!=(state.cmd&CMDFLAG_STATUS_REQUEST))
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
		else
		{
			// I think I should not push status queue in here unless 0!=(CMDFLAG_STATUS_REQUEST&state.cmd),
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
			if(0!=(state.cmd&CMDFLAG_STATUS_REQUEST))
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
		if(true==state.delayedSIRQ)
		{
			state.delayedSIRQ=false;
			state.SIRQ=true;
			PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+READ_SECTOR_TIME);
		}
		else
		{
			if(state.readingSectorHSG<=state.endSectorHSG) // Have more data.
			{
				auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_CDROM);
				bool DMAAvailable=(nullptr!=DMACh && (0<DMACh->currentCount && 0xFFFFFFFF!=(DMACh->currentCount&0xFFFFFFFF)));

				// Initial State.  CPU doesn't know data ready, therefore does not make DMAC available.
				if(true!=DMAAvailable || true!=state.DMATransfer)
				{
					state.ClearStatusQueue();
					if(0!=(state.cmd&CMDFLAG_STATUS_REQUEST))
					{
						SetStatusDataReady();
						if(0!=(state.cmd&CMDFLAG_IRQ) && true==state.enableSIRQ)
						{
							PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
						}
					}
					state.SIRQ=true;
					state.DEI=false;
					state.DTSF=false;

					// I don't know if there is a possibility that DMA transfer flag (DTS) is set to true before this point.
					// SYSROM of 2F and 2MX implies that DTS is like a trigger and should be set to true after everthing is set up.
					// But, just in case if DMATransfer is already enabled.
					if(true==state.DTSF)
					{
						state.DTSF=true;
						townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+READ_SECTOR_TIME);
					}
					// Otherwise write to DTS should trigger the DMA transfer.
				}
				// Second State.  DMA available, but the transfer hasn't started.  Wait for the CPU to write DTS=1.
				// Third State.  Transfer done for a sector.
				else if(true==DMAAvailable && true==state.DMATransfer && true==state.DTSF)
				{
					townsPtr->NotifyDiskRead();

					auto data=state.GetDisc().ReadSectorMODE1(state.readingSectorHSG,1);
					if(DMACh->currentCount+1<data.size())
					{
						data.resize(DMACh->currentCount+1);
					}
					DMACPtr->DeviceToMemory(DMACh,data);
					++state.readingSectorHSG;
					townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+NOTIFICATION_TIME);
					state.DMATransfer=false;
					state.DTSF=false;  // Should I turn it off also? -> Looks like I should.  Based on 2MX SYSROM FC00:00001CF7.  It waits for DTSF to clear.
					state.DEI=true;

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
				// state.DEI=true;

				if(0!=(state.cmd&CMDFLAG_STATUS_REQUEST))
				{
					state.SIRQ=true;
					SetStatusReadDone();
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
	case CDCMD_CDDAPLAY:
		if(true==state.delayedSIRQ)
		{
			state.delayedSIRQ=false;
			state.DRY=true;

			DiscImage::MinSecFrm msfBegin,msfEnd,twoSec;
			twoSec.Set(0,2,0);

			msfBegin.min=DiscImage::BCDToBin(state.paramQueue[0]);
			msfBegin.sec=DiscImage::BCDToBin(state.paramQueue[1]);
			msfBegin.frm=DiscImage::BCDToBin(state.paramQueue[2]);
			msfBegin-=twoSec;

			msfEnd.min=DiscImage::BCDToBin(state.paramQueue[3]);
			msfEnd.sec=DiscImage::BCDToBin(state.paramQueue[4]);
			msfEnd.frm=DiscImage::BCDToBin(state.paramQueue[5]);
			msfEnd-=twoSec;

			if(nullptr!=OutsideWorld)
			{
				bool repeat=(1==state.paramQueue[6]); // Should I say 0!= ?
				OutsideWorld->CDDAPlay(state.GetDisc(),msfBegin,msfEnd,repeat);
				state.CDDAState=State::CDDA_PLAYING;
				state.CDDAEndTime=msfEnd;
			}
			if(CMDFLAG_STATUS_REQUEST&state.cmd)
			{
				SetStatusDriveNotReadyOrDiscChangedOrNoError();
				state.SIRQ=true;
				if(0!=(state.cmd&CMDFLAG_IRQ) && true==state.enableSIRQ)
				{
					PICPtr->SetInterruptRequestBit(TOWNSIRQ_CDROM,true);
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
	if(true==CDDAIsPlaying())
	{
		next2ndByteOfStatusCode=0x03; // Prob: Response to A0H (80H+REQSTA), 00 03 xx xx means CDDA is playing.
	}
	state.PushStatusQueue(0,next2ndByteOfStatusCode,0,0);
}
void TownsCDROM::SetStatusDriveNotReady(void)
{
	state.PushStatusQueue(0x21,7,0,0);
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
	if(true==SetStatusDriveNotReadyOrDiscChanged())
	{
		return;
	}
	auto &disc=state.GetDisc();

	state.statusQueue.clear();

	SetStatusNoError();

	state.PushStatusQueue(0x16,0,0,0);
	state.PushStatusQueue(0x17,1,0,0);

	state.PushStatusQueue(0x16,0,0,0);
	state.PushStatusQueue(0x17,DiscImage::BinToBCD(disc.GetNumTracks()),0,0);

	auto length=DiscImage::HSGtoMSF(disc.GetNumSectors()+DiscImage::HSG_BASE);
	state.PushStatusQueue(0x16,0,0,0);
	state.PushStatusQueue(0x17,
	                      DiscImage::BinToBCD(length.min),
	                      DiscImage::BinToBCD(length.sec),
	                      DiscImage::BinToBCD(length.frm));

	for(auto &trk : disc.GetTracks())
	{
		unsigned char secondByte=(trk.trackType==DiscImage::TRACK_AUDIO ? 0 : 0x40);
		state.PushStatusQueue(0x16,secondByte,0,0);

		// F29 Retaliator is expecting trk.start+trk.preGap.
		auto HSG=DiscImage::MSFtoHSG(trk.start+trk.preGap);
		HSG+=DiscImage::HSG_BASE;
		auto MSF=DiscImage::HSGtoMSF(HSG);

		state.PushStatusQueue(0x17,
	                      DiscImage::BinToBCD(MSF.min),
	                      DiscImage::BinToBCD(MSF.sec),
	                      DiscImage::BinToBCD(MSF.frm));
	}
}

void TownsCDROM::SetStatusSubQRead(void)
{
	if(true==SetStatusDriveNotReadyOrDiscChanged())
	{
		return;
	}

	if(nullptr==OutsideWorld)
	{
		SetStatusDriveNotReady();
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

	if(State::CDDA_PLAYING==state.CDDAState)
	{
		discTime=OutsideWorld->CDDACurrentPosition();
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
	if(nullptr!=OutsideWorld)
	{
		OutsideWorld->CDDAStop();
	}
	state.ClearStatusQueue();
	if(true!=SetStatusDriveNotReadyOrDiscChanged())
	{
		state.CDDAState=State::CDDA_ENDED;
		SetStatusNoError();
		PushStatusCDDAStopDone();
		state.PushStatusQueue(0,0x0D,0,0);
	}
	else
	{
		// See fix for ChaseHQ in CDDAPAUSE.
		state.CDDAState=State::CDDA_IDLE;
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
