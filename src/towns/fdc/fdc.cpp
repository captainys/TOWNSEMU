/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>

#include "cpputil.h"
#include "fdc.h"
#include "townsdef.h"
#include "towns.h"


TownsFDC::TownsFDC(class FMTownsCommon *townsPtr,class TownsPIC *PICPtr,class TownsDMAC *DMACPtr) : DiskDrive(townsPtr)
{
	this->townsPtr=townsPtr;
	this->PICPtr=PICPtr;
	this->DMACPtr=DMACPtr;

	Reset();
	debugBreakOnCommandWrite=false;
}

////////////////////////////////////////////////////////////


void TownsFDC::SendCommand(unsigned int cmd)
{
	DiskDrive::SendCommand(cmd,townsPtr->state.townsTime);
}
void TownsFDC::MakeReady(void)
{
	state.busy=false;
	PICPtr->SetInterruptRequestBit(TOWNSIRQ_FDC,state.IRQMSK);
}

////////////////////////////////////////////////////////////

/* virtual */ void TownsFDC::RunScheduledTask(unsigned long long int townsTime)
{
	auto &drv=state.drive[DriveSelect()];
	auto imgFilePtr=GetDriveImageFile(DriveSelect());
	auto diskIdx=drv.diskIndex;
	DiskDrive::DiskImage *imgPtr=nullptr;
	if(nullptr!=imgFilePtr)
	{
		imgPtr=&imgFilePtr->img;
	}

	state.recordType=false;
	state.recordNotFound=false;
	state.CRCError=false;
	state.lostData=false;
	state.writeFault=false;

	switch(state.lastCmd&0xF0)
	{
	case 0x00: // Restore
		townsPtr->UnscheduleDeviceCallBack(*this);
		MakeReady();
		drv.trackPos=0;
		drv.trackReg=0;
		break;
	case 0x10: // Seek
		townsPtr->UnscheduleDeviceCallBack(*this);
		MakeReady();
		// Seek to dataReg
		if(drv.trackPos<drv.dataReg)
		{
			drv.lastSeekDir=1;
		}
		else if(drv.dataReg<drv.trackPos)
		{
			drv.lastSeekDir=-1;
		}
		drv.trackPos=drv.dataReg;
		if(state.lastCmd&0x10)
		{
			drv.trackReg=drv.trackPos;
		}
		break;
	case 0x20: // Step?
	case 0x30: // Step?
		townsPtr->UnscheduleDeviceCallBack(*this);
		MakeReady();
		drv.trackPos+=drv.lastSeekDir;
		if(drv.trackPos<0)
		{
			drv.trackPos=0;
		}
		else if(80<drv.trackPos)
		{
			drv.trackPos=80;
		}
		if(state.lastCmd&0x10)
		{
			drv.trackReg=drv.trackPos;
		}
		break;
	case 0x40: // Step In
	case 0x50: // Step In
		townsPtr->UnscheduleDeviceCallBack(*this);
		MakeReady();
		++drv.trackPos;
		if(80<drv.trackPos)
		{
			drv.trackPos=80;
		}
		if(state.lastCmd&0x10)
		{
			drv.trackReg=drv.trackPos;
		}
		break;
	case 0x60: // Step Out
	case 0x70: // Step Out
		MakeReady();
		--drv.trackPos;
		if(drv.trackPos<0)
		{
			drv.trackPos=0;
		}
		if(state.lastCmd&0x10)
		{
			drv.trackReg=drv.trackPos;
		}
		break;

	case 0x80: // Read Data (Read Sector)
	case 0x90: // Read Data (Read Sector)
		townsPtr->UnscheduleDeviceCallBack(*this); // Tentativelu
		if(nullptr!=imgPtr)
		{
			if(true==CheckMediaTypeAndDriveModeCompatible(drv.mediaType,GetDriveMode()))
			{
				auto trackPos=drv.trackPos;
				if(MEDIA_2D==drv.mediaType)
				{
					trackPos/=2;
				}

				if(true==fdcMonitor)
				{
					std::cout << "C:" << trackPos << " H:" << state.side << " R:" << GetSectorReg();
				}
				{
					unsigned int keys[2]=
					{
						(trackPos<<16)|(state.side<<8)|GetSectorReg(),
						(GetTrackReg()<<16)|(state.side<<8)|GetSectorReg()
					};
					for(auto key : keys)
					{
						auto found=breakOnReadSector.find(key);
						if(found!=breakOnReadSector.end())
						{
							std::string msg="Read Sector ";
							msg+=cpputil::Ubtox(key>>16);
							msg.push_back(' ');
							msg+=cpputil::Ubtox((key>>8)&255);
							msg.push_back(' ');
							msg+=cpputil::Ubtox(key&255);
							msg+=" at ";
							msg+=cpputil::Ustox(townsPtr->CPU().state.CS().value);
							msg.push_back(':');
							msg+=cpputil::Ustox(townsPtr->CPU().state.EIP);
							if(true==found->second.monitorOnly)
							{
								townsPtr->debugger.WriteLogFile(msg);
								msg.push_back(':');

								std::cout << msg << std::endl;
							}
							else
							{
								townsPtr->debugger.WriteLogFile(msg);
								townsPtr->debugger.ExternalBreak(msg);
							}
						}
					}
				}

				unsigned int nStepsToGetToTheSector=0;
				auto verifySide=(0!=(state.lastCmd&2));
				auto sec=imgPtr->ReadSectorFrom(
				    diskIdx,trackPos,state.side,
				    GetTrackReg(),state.side,GetSectorReg(),verifySide,state.addrMarkReadCount,nStepsToGetToTheSector);
				if(true==sec.exists)
				{
					auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_FPD);
					if(nullptr!=DMACh)
					{
						townsPtr->NotifyDiskRead();

						auto bytesTransferred=DMACPtr->DeviceToMemory(DMACh,sec.data);
						if(sec.data.size()!=bytesTransferred)
						{
							std::cout << "Warning: Not all sector data was transferred by DMA (FD->Mem)." << std::endl;
							std::cout << "Tsugaru does not support resume DMA from middle of a sector." << std::endl;
						}

						// What am I supposed to if error during DMA?
						if(state.lastCmd&0x10 && true==imgPtr->SectorExists(diskIdx,trackPos,state.side,GetSectorReg()+1)) // Multi Record
						{
							SetSectorReg(GetSectorReg()+1);
							townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+SECTOR_READ_WRITE_TIME);
						}
						else
						{
							state.CRCError=(D77File::D77_SECTOR_STATUS_CRC==sec.crcStatus);
							state.recordNotFound=(D77File::D77_SECTOR_STATUS_RECORD_NOT_FOUND==sec.crcStatus);
							state.recordType=(0!=sec.DDM);
							MakeReady();
							DMACPtr->SetDMATransferEnd(TOWNSDMA_FPD);
						}
					}
					else
					{
						state.lostData=true;
						MakeReady();
					}
				}
				else
				{
					if(true==fdcMonitor)
					{
						std::cout << "Record Not Found";
					}
					state.recordNotFound=true;
					MakeReady();
				}
				if(true==fdcMonitor)
				{
					std::cout << std::endl;
				}
			}
			else
			{
				state.recordNotFound=true;
				MakeReady();
			}
		}
		break;
	case 0xA0: // Write Data (Write Sector)
	case 0xB0: // Write Data (Write Sector)
		townsPtr->UnscheduleDeviceCallBack(*this);
		if(nullptr!=imgPtr)
		{
			if(true==imgPtr->WriteProtected(diskIdx))
			{
				// Write protected.
			}
			else if(true==CheckMediaTypeAndDriveModeCompatible(drv.mediaType,GetDriveMode()))
			{
				// Probably, if I send write command to 2D disk, it will write to cylinder=trackPos/2.
				auto trackPos=drv.trackPos;
				if(MEDIA_2D==drv.mediaType)
				{
					trackPos/=2;
				}

				if(true==fdcMonitor)
				{
					std::cout << "C:" << trackPos << " H:" << state.side << " R:" << GetSectorReg();
				}

				auto secLen=imgPtr->GetSectorLength(diskIdx,trackPos,state.side,GetSectorReg());
				if(0<secLen)
				{
					auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_FPD);
					if(nullptr!=DMACh)
					{
						auto toWrite=DMACPtr->MemoryToDevice(DMACh,secLen);
						if(toWrite.size()==secLen)
						{
							imgPtr->WriteSector(diskIdx,trackPos,state.side,GetSectorReg(),toWrite.size(),toWrite.data());
							if(state.lastCmd&0x10 && true==imgPtr->SectorExists(diskIdx,drv.trackPos,state.side,GetSectorReg()+1)) // Multi Record
							{
								SetSectorReg(GetSectorReg()+1);
								townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+SECTOR_READ_WRITE_TIME);
								if(true==fdcMonitor)
								{
									std::cout << " DMA Ct:" << cpputil::Uitox(DMACh->currentCount) << std::endl;
								}
								break; // Not supposed to make ready yet.
							}
							else
							{
								MakeReady();
								DMACPtr->SetDMATransferEnd(TOWNSDMA_FPD);
							}
						}
						else
						{
							{
								Abort("Not all sector data was transferred by DMA (Mem->FD).");
							}
							state.writeFault=true;
							MakeReady();
						}
					}
					else
					{
						state.lostData=true;
						MakeReady();
					}
				}
				else
				{
					if(true==fdcMonitor)
					{
						std::cout << "Record Not Found";
					}
					state.recordNotFound=true;
				}
				if(true==fdcMonitor)
				{
					std::cout << std::endl;
				}
			}
			else
			{
				state.recordNotFound=true;
			}
		}
		MakeReady();
		break;

	case 0xC0: // Read Address
		townsPtr->UnscheduleDeviceCallBack(*this);
		if(nullptr!=imgPtr)
		{
			if(true==CheckMediaTypeAndDriveModeCompatible(drv.mediaType,GetDriveMode()))
			{
				auto trackPos=drv.trackPos;
				if(MEDIA_2D==drv.mediaType)
				{
					trackPos/=2;
				}

				// Copy CHRN and CRC CRC to DMA.
				auto CHRN_CRC=imgPtr->ReadAddress(state.CRCError,diskIdx,trackPos,state.side,state.addrMarkReadCount);
				if(0<CHRN_CRC.size())
				{
					unsigned int keys[2]=
					{
						(trackPos<<16)|(state.side<<8)|uint32_t(CHRN_CRC[2]),
						(uint32_t(CHRN_CRC[0])<<16)|(uint32_t(CHRN_CRC[1])<<8)|uint32_t(CHRN_CRC[2])
					};
					for(auto key : keys)
					{
						auto found=breakOnReadAddress.find(key);
						if(found!=breakOnReadAddress.end())
						{
							std::string msg="Read Address ";
							msg+=cpputil::Ubtox(key>>16);
							msg.push_back(' ');
							msg+=cpputil::Ubtox((key>>8)&255);
							msg.push_back(' ');
							msg+=cpputil::Ubtox(key&255);
							msg+=" at ";
							msg+=cpputil::Ustox(townsPtr->CPU().state.CS().value);
							msg.push_back(':');
							msg+=cpputil::Ustox(townsPtr->CPU().state.EIP);
							if(true==found->second.monitorOnly)
							{
								townsPtr->debugger.WriteLogFile(msg);
								msg.push_back(':');

								std::cout << msg << std::endl;
							}
							else
							{
								townsPtr->debugger.WriteLogFile(msg);
								townsPtr->debugger.ExternalBreak(msg);
							}
						}
					}

					auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_FPD);
					if(nullptr!=DMACh)
					{
						DMACPtr->DeviceToMemory(DMACh,CHRN_CRC);
						MakeReady();
					}
					else
					{
						state.lostData=true;
						MakeReady();
					}
				}
				MakeReady();
			}
			else
			{
				MakeReady();
				state.recordNotFound=true;
			}
		}
		break;
	case 0xE0: // Read Track
		townsPtr->UnscheduleDeviceCallBack(*this);
		if(nullptr!=imgPtr)
		{
			if(true==CheckMediaTypeAndDriveModeCompatible(drv.mediaType,GetDriveMode()))
			{
				auto trackPos=drv.trackPos;
				if(MEDIA_2D==drv.mediaType)
				{
					trackPos/=2;
				}

				// Copy CHRN and CRC CRC to DMA.
				auto trackData=imgPtr->ReadTrack(diskIdx,trackPos,state.side);
				if(0<trackData.size())
				{
					auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_FPD);
					if(nullptr!=DMACh)
					{
						DMACPtr->DeviceToMemory(DMACh,trackData);
						MakeReady();
					}
					else
					{
						state.lostData=true;
						MakeReady();
					}
				}
				MakeReady();
			}
			else
			{
				MakeReady();
				state.recordNotFound=true;
			}
		}
		break;
	case 0xF0: // Write Track
		townsPtr->UnscheduleDeviceCallBack(*this);
		if(nullptr!=imgPtr)
		{
			if(true==imgPtr->WriteProtected(diskIdx))
			{
				// Write protected.
			}
			else if(true==CheckMediaTypeAndDriveModeCompatibleForFormat(drv.mediaType,GetDriveMode()))
			{
				auto trackPos=drv.trackPos;
				if(MEDIA_2D==drv.mediaType)
				{
					trackPos/=2;
				}

				auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_FPD);
				if(nullptr!=DMACh)
				{
					// What's the length?
					// Looks like Towns MENU makes 2DE0H bytes of data.
					// [10]
					//   1232KB format 1024 bytes per sector,  8 sectors per track, 77 tracks
					//   1440KB format  512 bytes per sector, 18 sectors per track, 80 tracks
					//    640KB format  512 bytes per sector,  8 sectors per track, 80 tracks
					//    720KB format  512 bytes per sector,  9 sectors per track, 80 tracks
					// [2] pp. 250
					// From the index hole:
					//    GAP        80 bytes 0x4E
					//    SYNC       12 bytes 0x00
					//    INDEX MARK  4 bytes 0xC2,0xC2,0xC2,0xFC (or 0xF6,0xF6,0xF6,0xFC)
					//    GAP        50 bytes 0x4E
					//    {
					//    SYNC       12 bytes 0x00
					//    ADDR MARK   4 bytes 0xA1,0xA1,0xA1,0xFE (or 0xF5,0xF5,0xF5,0xFE)
					//    CHRN        4 bytes
					//    CRC         2 bytes 0xF7 (Write 1 byte will become 2 bytes of CRC code)
					//    GAP        22 bytes
					//    SYNC       12 bytes
					//    DATA Mark   4 bytes 0xA1,0xA1,0xA1,0xFB (or 0xF5,0xF5,0xF5,0xFB)
					//    DATA        x bytes (x=128*(2^N))
					//    CRC         2 bytes 0xF7 (Write 1 byte will become 2 bytes of CRC code)
					//    GAP        54/84/116/108 bytes.
					//    } times sectors
					//    GAP        598/400/654/? bytes
					// 1232KB format -> 80+12+4+50+(12+4+4+2+22+12+4+1024+2+116)*8+654=10416 (28B0H)
					// 1440KB format -> 80+12+4+50+(12+4+4+2+22+12+4+512+2+108)*18+?=12422+? (3086H+?)
					//  640KB format -> 80+12+4+50+(12+4+4+2+22+12+4+512+2+84)*8+598=6008 (1778H)
					//  720KB format -> 80+12+4+50+(12+4+4+2+22+12+4+512+2+54)*9+400=6198 (1836H)

					// 2HD -> Read 0x3286 bytes
					// 2DD -> Read 0x1836 bytes

					unsigned int len=0;
					switch(GetDriveMode())
					{
					case MEDIA_2DD_640KB:
					case MEDIA_2DD_720KB:
						len=6198;
						break;
					case MEDIA_2HD_1232KB:
						len=10416;
						break;
					case MEDIA_2HD_1440KB:
						len=12934; // Assume ?=512
						break;
					}
					auto formatData=DMACPtr->MemoryToDevice(DMACh,len);
					auto newMediaType=imgPtr->WriteTrack(diskIdx,trackPos,state.side,formatData);
					if(MEDIA_UNKNOWN!=newMediaType)
					{
						drv.mediaType=newMediaType;
					}
					state.writeFault=false;
				}
				else
				{
					state.writeFault=true;
				}
			}
			else
			{
				state.writeFault=true;
			}
		}
		MakeReady();
		break;

	default:
	case 0xD0: // Force Interrupt
		townsPtr->UnscheduleDeviceCallBack(*this);
		state.busy=false;
		if(0!=(state.lastCmd&8))
		{
			PICPtr->SetInterruptRequestBit(TOWNSIRQ_FDC,state.IRQMSK);
		}
		break;
	}
	state.lastStatus=MakeUpStatus(state.lastCmd,townsTime);
}

/* virtual */ void TownsFDC::IOWriteByte(unsigned int ioport,unsigned int data)
{
	auto &drv=state.drive[DriveSelect()];
	switch(ioport)
	{
	case TOWNSIO_FDC_STATUS_COMMAND://       0x200, // [2] pp.253
		// During the start-up, System ROM writes 0FEH to this register,
		// which is not listed in the data sheet of MB
		SendCommand(data);
		state.drive[DriveSelect()].diskChange=false; // Is this the right timing to erase diskChange flag?
		PICPtr->SetInterruptRequestBit(TOWNSIRQ_FDC,false);
		if(true==debugBreakOnCommandWrite)
		{
			townsPtr->debugger.ExternalBreak("FDC Command Write "+cpputil::Ubtox(data)+" "+FDCCommandToExplanation(data));
		}
		if(true==fdcMonitor)
		{
			std::cout << ("FDC Command Write "+cpputil::Ubtox(data)+" "+FDCCommandToExplanation(data)) << std::endl;
		}
		break;
	case TOWNSIO_FDC_TRACK://                0x202, // [2] pp.253
		state.drive[DriveSelect()].trackReg=data;
		break;
	case TOWNSIO_FDC_SECTOR://               0x204, // [2] pp.253
		SetSectorReg(data);
		break;
	case TOWNSIO_FDC_DATA://                 0x205, // [2] pp.253
		state.drive[DriveSelect()].dataReg=data;
		break;
	case TOWNSIO_FDC_DRIVE_STATUS_CONTROL:// 0x208, // [2] pp.253
		state.IRQMSK=(0!=(data&1));
		state.DDEN=(0!=(data&2));
		state.side=(0!=(data&4) ? 1 : 0);  // Is it really bit=ON for side 0?  Counter intuitive.
		drv.motor=(0!=(data&0x10));
		state.CLKSEL=(0!=(data&0x20));
		break;
	case TOWNSIO_FDC_DRIVE_SELECT://         0x20C, // [2] pp.253
		if(0==state.driveSelectBit && 0!=(data&0x0F))
		{
			// Latch MODE-B and HISPD
			state.MODEB=(0!=(data&0x80));
			state.HISPD=(0!=(data&0x40));
		}
		state.INUSE=(0!=(data&0x10));
		state.driveSelectBit=(data&0x0F);
		break;
	case TOWNSIO_FDC_FDDV_EXT:
		break;
	case TOWNSIO_FDC_DRIVE_SWITCH://         0x20E, // [2] pp.253
		// Let's disable it.
		break;
	}
}
/* virtual */ unsigned int TownsFDC::IOReadByte(unsigned int ioport)
{
	unsigned char data=0xFF;
	switch(ioport)
	{
	case TOWNSIO_FDC_STATUS_COMMAND://       0x200, // [2] pp.253
		PICPtr->SetInterruptRequestBit(TOWNSIRQ_FDC,false);
		data=state.lastStatus;
		// This status byte is supposed to be a command status.  My original interpretation was it updates
		// only when FDC command was issued.
		// However, FM-OASYS reads this byte and checks bit 7 for drive-not ready without issuing a command.
		// Presumably, bit 7 of this byte returns current drive-ready state.

		if(0xD0==(state.lastCmd&0xD0) || 0==(state.lastCmd&0x80))
		{
			auto t=townsPtr->state.townsTime;
			t%=166000000;
			bool indexHole=(t<2000000);
			if(true==indexHole)
			{
				data|=2;
			}
			else
			{
				data&=0xFD;
			}
		}

		if(true==DriveReady(townsPtr->state.townsTime))
		{
			data&=0x7F;
		}
		return data;
	case TOWNSIO_FDC_TRACK://                0x202, // [2] pp.253
		data=GetTrackReg();
		break;
	case TOWNSIO_FDC_SECTOR://               0x204, // [2] pp.253
		data=GetSectorReg();
		break;
	case TOWNSIO_FDC_DATA://                 0x205, // [2] pp.253
		data=state.drive[DriveSelect()].dataReg;
		break;
	case TOWNSIO_FDC_DRIVE_STATUS_CONTROL:// 0x208, // [2] pp.253
		data=(true==state.drive[DriveSelect()].diskChange ? 1 : 0); // DSKCHG [2] pp.773
		// Disk BIOS reads FD status like:
		// 0421:00000BE0 BA0802                    MOV     DX,0208H
		// 0421:00000BE3 EC                        IN      AL,DX
		// 0421:00000BE4 EC                        IN      AL,DX
		// 0421:00000BE5 EC                        IN      AL,DX
		// 0421:00000BE6 C3                        RET
		// Therefore, the timing to clear DSKCHG flag is not status-read.
		// Maybe when a command is written?
		data|=(DriveReady(townsPtr->state.townsTime) ? 2 : 0);
		data|=0b01100; // 3-mode drive.      [2] pp.809
		data|=0x80;    // 2 internal drives. [2] pp.773
		break;
	case TOWNSIO_FDC_DRIVE_SELECT://         0x20C, // [2] pp.253
		break;
	case TOWNSIO_FDC_FDDV_EXT:
		data=0x7F; // FDDV EXT=1: FDDV2-0
		break;
	case TOWNSIO_FDC_DRIVE_SWITCH://         0x20E, // [2] pp.253
		data=(true!=state.driveSwitch ? 1 : 0);
		break;
	}
	return data;
}
/* virtual */ void TownsFDC::Reset(void)
{
	DiskDrive::Reset();
}

void TownsFDC::SetBreakOnReadSector(unsigned char CHRN[4],bool monitorOnly)
{
	unsigned int key=(CHRN[0]<<16)|(CHRN[1]<<8)|CHRN[2];
	BreakOnAccess cond;
	cond.monitorOnly=monitorOnly;
	breakOnReadSector[key]=cond;
}

void TownsFDC::ClearBreakOnReadSector(unsigned char CHRN[4])
{
	unsigned int key=(CHRN[0]<<16)|(CHRN[1]<<8)|CHRN[2];
	auto found=breakOnReadSector.find(key);
	if(breakOnReadSector.end()!=found)
	{
		breakOnReadSector.erase(found);
	}
}

void TownsFDC::ClearAllBreakOnReadSector(void)
{
	std::unordered_map <unsigned int,BreakOnAccess> empty;
	std::swap(empty,breakOnReadSector);
}


void TownsFDC::SetBreakOnReadAddress(unsigned char CHRN[4],bool monitorOnly)
{
	unsigned int key=(CHRN[0]<<16)|(CHRN[1]<<8)|CHRN[2];
	BreakOnAccess cond;
	cond.monitorOnly=monitorOnly;
	breakOnReadAddress[key]=cond;
}

void TownsFDC::ClearBreakOnReadAddress(unsigned char CHRN[4])
{
	unsigned int key=(CHRN[0]<<16)|(CHRN[1]<<8)|CHRN[2];
	auto found=breakOnReadAddress.find(key);
	if(breakOnReadAddress.end()!=found)
	{
		breakOnReadAddress.erase(found);
	}
}

void TownsFDC::ClearAllBreakOnReadAddress(void)
{
	std::unordered_map <unsigned int,BreakOnAccess> empty;
	std::swap(empty,breakOnReadAddress);
}
