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


void TownsFDC::ImageFile::SaveIfModified(void)
{
	if(true==d77.IsModified())
	{
		std::vector <unsigned char> bin;
		switch(fileType)
		{
		case IMGFILE_RAW:
			bin=d77.MakeRawImage();
			break;
		case IMGFILE_D77:
			bin=d77.MakeD77Image();
			break;
		}
		if(0<bin.size())
		{
			if(true!=cpputil::WriteBinaryFile(fName,bin.size(),bin.data()))
			{
				std::cout << "Warning!  Floppy Disk Image could not be saved." << std::endl;
			}
			else
			{
				std::cout << "Auto-saved disk image:" << fName << std::endl;
			}
		}
	}

	for(int i=0; i<d77.GetNumDisk(); ++i)
	{
		d77.GetDisk(i)->ClearModified();
	}
}

////////////////////////////////////////////////////////////

void TownsFDC::State::Reset(void)
{
	for(auto &d : drive)
	{
		d.trackPos=0;      // Actual head location.
		d.trackReg=0;      // Value in track register 0202H
		d.sectorReg=1;     // Value in sector register 0x04H
		d.dataReg=0;       // Value in data register 0x06H
		d.lastSeekDir=1;
		d.motor=false;
		d.diskChange=false;
	}
	driveSwitch=false;
	driveSelectBit=1;      // Looks like A drive is selected by default.
	side=0;
	busy=false;
	MODEB=false;
	HISPD=false;
	INUSE=false;
	lastCmd=0;
	lastStatus=0;

	CLKSEL=false;
	DDEN=false;
	IRQMSK=true;

	recordType=false;
	recordNotFound=false;
	CRCError=false;
	lostData=false;
	writeFault=false;

	addrMarkReadCount=0;
}


////////////////////////////////////////////////////////////


TownsFDC::TownsFDC(class FMTowns *townsPtr,class TownsPIC *PICPtr,class TownsDMAC *DMACPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
	this->PICPtr=PICPtr;
	this->DMACPtr=DMACPtr;

	Reset();
	debugBreakOnCommandWrite=false;
	for(auto &d : state.drive)
	{
		d.imgFileNum=-1;
		d.diskIndex=-1;
		d.mediaType=MEDIA_UNKNOWN;
	}
	for(auto &i : imgFile)
	{
		i.fileType=IMGFILE_RAW;
	}
}


bool TownsFDC::LoadRawBinary(unsigned int driveNum,const char fName[],bool verbose)
{
	driveNum&=3;

	auto bin=cpputil::ReadBinaryFile(fName);
	if(0==bin.size())
	{
		return false;
	}

	// First unlink any drive pointing to the disk image
	for(auto &d : state.drive)
	{
		if(d.imgFileNum==driveNum)
		{
			d.imgFileNum=-1;
			d.diskIndex=-1;
		}
	}

	imgFile[driveNum].SaveIfModified();

	if(true==imgFile[driveNum].d77.SetRawBinary(bin,verbose))
	{
		imgFile[driveNum].fileType=IMGFILE_RAW;
		imgFile[driveNum].fName=fName;
		state.drive[driveNum].imgFileNum=driveNum;
		state.drive[driveNum].diskIndex=0;
		state.drive[driveNum].mediaType=IdentifyDiskMediaType(imgFile[driveNum].d77.GetDisk(0));
		state.drive[driveNum].diskChange=true;
		return true;
	}
	else
	{
		return false;
	}
}

D77File::D77Disk *TownsFDC::GetDriveDisk(int driveNum)
{
	if(0<=driveNum && driveNum<NUM_DRIVES)
	{
		auto &drv=state.drive[driveNum];
		if(0<=drv.imgFileNum && drv.imgFileNum<NUM_DRIVES)
		{
			return imgFile[drv.imgFileNum].d77.GetDisk(drv.diskIndex);
		}
	}
	return nullptr;
}
const D77File::D77Disk *TownsFDC::GetDriveDisk(int driveNum) const
{
	if(0<=driveNum && driveNum<NUM_DRIVES)
	{
		auto &drv=state.drive[driveNum];
		if(0<=drv.imgFileNum && drv.imgFileNum<NUM_DRIVES)
		{
			return imgFile[drv.imgFileNum].d77.GetDisk(drv.diskIndex);
		}
	}
	return nullptr;
}
TownsFDC::ImageFile *TownsFDC::GetDriveImageFile(int driveNum)
{
	if(0<=driveNum && driveNum<NUM_DRIVES)
	{
		auto &drv=state.drive[driveNum];
		if(0<=drv.imgFileNum && drv.imgFileNum<NUM_DRIVES)
		{
			return &imgFile[drv.imgFileNum];
		}
	}
	return nullptr;
}
const TownsFDC::ImageFile *TownsFDC::GetDriveImageFile(int driveNum) const
{
	if(0<=driveNum && driveNum<NUM_DRIVES)
	{
		auto &drv=state.drive[driveNum];
		if(0<=drv.imgFileNum && drv.imgFileNum<NUM_DRIVES)
		{
			return &imgFile[drv.imgFileNum];
		}
	}
	return nullptr;
}

void TownsFDC::SaveModifiedDiskImages(void)
{
	for(auto &img : imgFile)
	{
		img.SaveIfModified();
	}
}

void TownsFDC::SetWriteProtect(int driveNum,bool writeProtect)
{
	auto diskPtr=GetDriveDisk(driveNum);
	if(nullptr!=diskPtr)
	{
		if(true==writeProtect)
		{
			diskPtr->SetWriteProtected();
		}
		else
		{
			diskPtr->ClearWriteProtected();
		}
	}
}

unsigned int TownsFDC::IdentifyDiskMediaType(const D77File::D77Disk *diskPtr) const
{
	unsigned int totalSize=0;
	for(auto loc : diskPtr->AllTrack())
	{
		auto trk=diskPtr->GetTrack(loc.track,loc.side);
		for(auto &sec : trk->sector)
		{
			totalSize+=(128<<sec.sizeShift);
		}
	}
	totalSize/=1024;
	unsigned int mediaType=MEDIA_UNKNOWN;
	if(600<=totalSize && totalSize<680)
	{
		return MEDIA_2DD_640KB;
	}
	else if(680<=totalSize && totalSize<760)
	{
		return MEDIA_2DD_720KB;
	}
	else if(1200<=totalSize && totalSize<1300)
	{
		return MEDIA_2HD_1232KB;
	}
	else if(1400<=totalSize && totalSize<1500)
	{
		return MEDIA_2HD_1440KB;
	}
	return MEDIA_UNKNOWN;
}

////////////////////////////////////////////////////////////


void TownsFDC::SendCommand(unsigned int cmd)
{
	if(0xFE==cmd)
	{
		// Meaning of 0xFE is unknown.
		// MB8877 Data sheet does not list this command.
		// Should I take it as 0xDE?
		state.lastStatus=MakeUpStatus(0xD0);
	}
	else if((cmd&0xF0)!=0xD0 && 0==state.driveSelectBit)
	{
		// Drive not selected.
		return;
	}
	else
	{
		auto &drv=state.drive[DriveSelect()];
		switch(cmd&0xF0)
		{
		case 0x00: // Restore
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+RESTORE_TIME);
			state.recordType=false;
			state.recordNotFound=false;
			state.CRCError=false;
			state.lostData=false;
			state.writeFault=false;
			state.busy=true;
			break;
		case 0x10: // Seek
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+SEEK_TIME);
			state.recordType=false;
			state.recordNotFound=false;
			state.CRCError=false;
			state.lostData=false;
			state.writeFault=false;
			state.busy=true;
			break;
		case 0x20: // Step?
		case 0x30: // Step?
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+STEP_TIME);
			state.recordType=false;
			state.recordNotFound=false;
			state.CRCError=false;
			state.lostData=false;
			state.writeFault=false;
			state.busy=true;
			break;
		case 0x40: // Step In
		case 0x50: // Step In
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+STEP_TIME);
			state.recordType=false;
			state.recordNotFound=false;
			state.CRCError=false;
			state.lostData=false;
			state.writeFault=false;
			state.busy=true;
			break;
		case 0x60: // Step Out
		case 0x70: // Step Out
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+STEP_TIME);
			state.recordType=false;
			state.recordNotFound=false;
			state.CRCError=false;
			state.lostData=false;
			state.writeFault=false;
			state.busy=true;
			break;

		case 0x80: // Read Data (Read Sector)
		case 0x90: // Read Data (Read Sector)
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+SECTOR_READ_WRITE_TIME);
			state.recordType=false;
			state.recordNotFound=false;
			state.CRCError=false;
			state.lostData=false;
			state.writeFault=false;
			state.busy=true;
			break;
		case 0xA0: // Write Data (Write Sector)
		case 0xB0: // Write Data (Write Sector)
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+SECTOR_READ_WRITE_TIME);
			state.recordType=false;
			state.recordNotFound=false;
			state.CRCError=false;
			state.lostData=false;
			state.writeFault=false;
			state.busy=true;
			break;

		case 0xC0: // Read Address
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+ADDRMARK_READ_TIME);
			state.recordType=false;
			state.recordNotFound=false;
			state.CRCError=false;
			state.lostData=false;
			state.writeFault=false;
			state.busy=true;
			break;
		case 0xE0: // Read Track
			state.recordType=false;
			state.recordNotFound=false;
			state.CRCError=false;
			state.lostData=false;
			state.writeFault=false;
			std::cout << __FUNCTION__ << std::endl;
			std::cout << "Command " << cpputil::Ubtox(cmd) << " not supported yet." << std::endl;
			break;
		case 0xF0: // Write Track
			state.recordType=false;
			state.recordNotFound=false;
			state.CRCError=false;
			state.lostData=false;
			state.writeFault=false;
			std::cout << __FUNCTION__ << std::endl;
			std::cout << "Command " << cpputil::Ubtox(cmd) << " not supported yet." << std::endl;
			break;

		case 0xD0: // Force Interrupt
			state.recordType=false;
			state.recordNotFound=false;
			state.CRCError=false;
			state.lostData=false;
			state.writeFault=false;
			if(true==state.busy)
			{
				state.lastCmd=cmd;
				state.busy=false;
				state.lastStatus&=0xFE; // Bit0 must be cleared.
				return;                 // Other bits stays the same.
			}
			state.busy=false;
			break;
		}
	}
	state.lastCmd=cmd;
	state.lastStatus=MakeUpStatus(cmd);
}
unsigned int TownsFDC::CommandToCommandType(unsigned int cmd) const
{
	if(0xFE==cmd)
	{
		return 4;
	}
	switch(cmd&0xF0)
	{
	case 0x00: // Restore
	case 0x10: // Seek
	case 0x20: // Step?
	case 0x30: // Step?
	case 0x40: // Step In
	case 0x50: // Step In
	case 0x60: // Step Out
	case 0x70: // Step Out
		return 1;

	case 0x80: // Read Data (Read Sector)
	case 0x90: // Read Data (Read Sector)
	case 0xA0: // Write Data (Write Sector)
	case 0xB0: // Write Data (Write Sector)
		return 2;

	case 0xC0: // Read Address
	case 0xE0: // Read Track
	case 0xF0: // Write Track
		return 3;

	case 0xD0: // Force Interrupt
		return 4;
	}
	return 0; // What?
}
unsigned char TownsFDC::MakeUpStatus(unsigned int cmd) const
{
	unsigned char data=0;
	if(0xFE==cmd)
	{
		cmd=0xD0; // System ROM is using this 0xFE.  Same as 0xD0?  Or Reset?
	}
	data|=(true!=DriveReady() ? 0x80 : 0);
	data|=(true==state.busy ? 0x01 : 0);
	switch(cmd&0xF0)
	{
	case 0x00: // Restore
	case 0x10: // Seek
	case 0x20: // Step?
	case 0x30: // Step?
	case 0x40: // Step In
	case 0x50: // Step In
	case 0x60: // Step Out
	case 0x70: // Step Out
		data|=(WriteProtected() ? 0x40 : 0);
		data|=(DriveReady() ?     0x20 : 0); // Head-Engaged: Make it same as drive ready.
		data|=(SeekError() ?      0x10 : 0);
		data|=(CRCError() ?       0x08 : 0);
		data|=(state.drive[DriveSelect()].trackPos==0 ? 0x04 : 0);
		data|=(IndexHole() ?      0x02 : 0);
		break;

	case 0x80: // Read Data (Read Sector)
	case 0x90: // Read Data (Read Sector)
		data|=(RecordType() ?     0x20 : 0);
		data|=(RecordNotFound() ? 0x10 : 0);
		data|=(CRCError() ?       0x08 : 0);
		data|=(LostData() ?       0x04 : 0);
		data|=(DataRequest() ?    0x02 : 0);
		break;
	case 0xA0: // Write Data (Write Sector)
	case 0xB0: // Write Data (Write Sector)
		data|=(WriteProtected() ? 0x40 : 0);
		data|=(WriteFault() ?     0x20 : 0);
		data|=(RecordNotFound() ? 0x10 : 0);
		data|=(CRCError() ?       0x08 : 0);
		data|=(LostData() ?       0x04 : 0);
		data|=(DataRequest() ?    0x02 : 0);
		break;

	case 0xC0: // Read Address
		data|=(RecordNotFound() ? 0x10 : 0);
		data|=(CRCError() ?       0x08 : 0);
		data|=(LostData() ?       0x04 : 0);
		data|=(DataRequest() ?    0x02 : 0);
		break;
	case 0xE0: // Read Track
		data|=(LostData() ?       0x04 : 0);
		data|=(DataRequest() ?    0x02 : 0);
		break;
	case 0xF0: // Write Track
		data|=(WriteProtected() ? 0x40 : 0);
		data|=(WriteFault() ?     0x20 : 0);
		data|=(LostData() ?       0x04 : 0);
		data|=(DataRequest() ?    0x02 : 0);
		break;

	case 0xD0: // Force Interrupt
		data|=(WriteProtected() ? 0x40 : 0);
		data|=(DriveReady() ?     0x20 : 0); // Head-Engaged: Make it same as drive ready.
		data|=(state.drive[DriveSelect()].trackPos==0 ? 0x04 : 0);
		data|=(IndexHole() ?      0x02 : 0);
		data&=0xFE;
		break;
	}
	return data;
}
unsigned int TownsFDC::DriveSelect(void) const
{
	if(0!=(state.driveSelectBit&1))
	{
		return 0;
	}
	if(0!=(state.driveSelectBit&2))
	{
		return 1;
	}
	if(0!=(state.driveSelectBit&4))
	{
		return 2;
	}
	if(0!=(state.driveSelectBit&8))
	{
		return 3;
	}
	return 0;
}

unsigned int TownsFDC::GetDriveMeode(void) const
{
	if(true!=state.DDEN)
	{
		return MEDIA_SINGLE_DENSITY;
	}
	else if(true==state.HISPD && true==state.MODEB)
	{
		return MEDIA_2HD_1440KB;
	}
	else if(true==state.HISPD && true!=state.MODEB)
	{
		return MEDIA_2HD_1232KB;
	}
	else if(true!=state.HISPD)
	{
		return MEDIA_2DD_640KB;
	}
	return MEDIA_UNKNOWN;
}

void TownsFDC::MakeReady(void)
{
	state.busy=false;
	PICPtr->SetInterruptRequestBit(TOWNSIRQ_FDC,state.IRQMSK);
}

bool TownsFDC::DriveReady(void) const
{
	if(0!=state.driveSelectBit && nullptr!=GetDriveDisk(DriveSelect()))
	{
		return true;
	}
	return false;
}
bool TownsFDC::WriteProtected(void) const
{
	auto diskPtr=GetDriveDisk(DriveSelect());
	if(nullptr!=diskPtr)
	{
		return diskPtr->IsWriteProtected();
	}
	return false; // Tentative.
}
bool TownsFDC::SeekError(void) const
{
	return false;
}
bool TownsFDC::CRCError(void) const
{
	return state.CRCError;
}
bool TownsFDC::IndexHole(void) const
{
	return false;
}
bool TownsFDC::RecordType(void) const
{
	return state.recordType;
}
bool TownsFDC::RecordNotFound(void) const
{
	return state.recordNotFound;
}
bool TownsFDC::LostData(void) const
{
	return state.lostData;
}
bool TownsFDC::DataRequest(void) const
{
	return false;
}
bool TownsFDC::WriteFault(void) const
{
	return state.writeFault;
}


////////////////////////////////////////////////////////////

/* virtual */ void TownsFDC::RunScheduledTask(unsigned long long int townsTime)
{
	auto &drv=state.drive[DriveSelect()];
	auto diskPtr=GetDriveDisk(DriveSelect());

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
		if(nullptr!=diskPtr)
		{
			if(true==CheckMediaTypeAndDriveModeCompatible(drv.mediaType,GetDriveMeode()))
			{
				auto secPtr=diskPtr->GetSector(drv.trackPos,state.side,drv.sectorReg);
				if(nullptr!=secPtr)
				{
					auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_FPD);
					if(nullptr!=DMACh)
					{
						townsPtr->NotifyDiskRead();

						DMACPtr->DeviceToMemory(DMACh,secPtr->sectorData);
						// What am I supposed to if error during DMA?
						if(state.lastCmd&0x10 && diskPtr->GetSector(drv.trackPos,state.side,drv.sectorReg+1)) // Multi Record
						{
							++drv.sectorReg;
							townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+SECTOR_READ_WRITE_TIME);
						}
						else
						{
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
					state.recordNotFound=true;
					MakeReady();
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
		if(nullptr!=diskPtr)
		{
			if(true==diskPtr->IsWriteProtected())
			{
				// Write protected.
			}
			else if(true==CheckMediaTypeAndDriveModeCompatible(drv.mediaType,GetDriveMeode()))
			{
				auto secPtr=diskPtr->GetSector(drv.trackPos,state.side,drv.sectorReg);
				if(nullptr!=secPtr)
				{
					auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_FPD);
					if(nullptr!=DMACh)
					{
						auto toWrite=DMACPtr->MemoryToDevice(DMACh,(unsigned int)secPtr->sectorData.size());
						if(toWrite.size()==secPtr->sectorData.size())
						{
							diskPtr->WriteSector(drv.trackPos,state.side,drv.sectorReg,toWrite.size(),toWrite.data());
							diskPtr->SetModified();
							if(state.lastCmd&0x10 && diskPtr->GetSector(drv.trackPos,state.side,drv.sectorReg+1)) // Multi Record
							{
								++drv.sectorReg;
								townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+SECTOR_READ_WRITE_TIME);
							}
							else
							{
								MakeReady();
							}
						}
						else
						{
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
					state.recordNotFound=true;
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
		if(nullptr!=diskPtr)
		{
			if(true==CheckMediaTypeAndDriveModeCompatible(drv.mediaType,GetDriveMeode()))
			{
				// Copy CHRN and CRC CRC to DMA.
				auto trkPtr=diskPtr->GetTrack(drv.trackPos,state.side);
				if(nullptr!=trkPtr)
				{
					if(trkPtr->sector.size()<=state.addrMarkReadCount)
					{
						state.addrMarkReadCount=0;
					}
					auto &sector=trkPtr->sector[state.addrMarkReadCount];

					std::vector <unsigned char> CRHN_CRC=
					{
						sector.cylinder,
						sector.head,
						sector.sector,
						sector.sizeShift,
						0x7f, // How can I calculate CRC?
						0x7f
					};
					auto DMACh=DMACPtr->GetDMAChannel(TOWNSDMA_FPD);
					if(nullptr!=DMACh)
					{
						DMACPtr->DeviceToMemory(DMACh,CRHN_CRC);
						MakeReady();
					}
					else
					{
						state.lostData=true;
						MakeReady();
					}

					++state.addrMarkReadCount;
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
		std::cout << __FUNCTION__ << std::endl;
		std::cout << "Command " << cpputil::Ubtox(state.lastCmd) << " not supported yet." << std::endl;
		MakeReady();
		break;
	case 0xF0: // Write Track
		townsPtr->UnscheduleDeviceCallBack(*this);
		std::cout << __FUNCTION__ << std::endl;
		std::cout << "Command " << cpputil::Ubtox(state.lastCmd) << " not supported yet." << std::endl;
		MakeReady();
		break;

	default:
	case 0xD0: // Force Interrupt
		townsPtr->UnscheduleDeviceCallBack(*this);
		state.busy=false;
		break;
	}
	state.lastStatus=MakeUpStatus(state.lastCmd);
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
		PICPtr->SetInterruptRequestBit(TOWNSIRQ_FDC,false);
		if(true==debugBreakOnCommandWrite)
		{
			townsPtr->debugger.ExternalBreak("FDC Command Write "+cpputil::Ubtox(data));
		}
		break;
	case TOWNSIO_FDC_TRACK://                0x202, // [2] pp.253
		state.drive[DriveSelect()].trackReg=data;
		break;
	case TOWNSIO_FDC_SECTOR://               0x204, // [2] pp.253
		state.drive[DriveSelect()].sectorReg=data;
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
		return state.lastStatus;
	case TOWNSIO_FDC_TRACK://                0x202, // [2] pp.253
		break;
	case TOWNSIO_FDC_SECTOR://               0x204, // [2] pp.253
		break;
	case TOWNSIO_FDC_DATA://                 0x205, // [2] pp.253
		data=state.drive[DriveSelect()].dataReg;
		break;
	case TOWNSIO_FDC_DRIVE_STATUS_CONTROL:// 0x208, // [2] pp.253
		data=(true==state.drive[DriveSelect()].diskChange ? 1 : 0); // DSKCHG [2] pp.773
		state.drive[DriveSelect()].diskChange=false;
		data|=(DriveReady() ? 2 : 0);
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
	state.Reset();
}

std::vector <std::string> TownsFDC::GetStatusText(void) const
{
	std::string line;
	std::vector <std::string> text;

	text.push_back(line);
	text.back()="FDC";

	for(auto &drv : state.drive)
	{
		const int driveNum=int(&drv-state.drive);
		auto imgFilePtr=GetDriveImageFile(driveNum);

		text.push_back(line);
		text.back()="Drive"+cpputil::Ubtox(driveNum)+" ";
		if(nullptr!=imgFilePtr)
		{
			text.back()+="(";
			text.back()+=imgFilePtr->fName;
			text.back()+=")";
		}

		text.push_back(line);
		text.back()+="TRKPOS:"+cpputil::Uitoa(drv.trackPos)+" TRKREG:"+cpputil::Uitoa(drv.trackReg);
		text.back()+=" SEC:"+cpputil::Uitoa(drv.sectorReg)+" LAST SEEK DIR:"+cpputil::Itoa(drv.lastSeekDir);
		text.back()+=" DATAREG:"+cpputil::Ubtox(drv.dataReg)+" MOTOR:"+(drv.motor ? "ON" : "OFF");
	}

	text.push_back(line);
	text.back()="DRVSELBIT:"+cpputil::Ubtox(state.driveSelectBit)+" SIDE:"+cpputil::Ubtox(state.side);

	text.push_back(line);
	text.back()="BUSY:";
	text.back()+=(state.busy ? "1" : "0");
	text.back()+=" MODEB:";
	text.back()+=(state.MODEB ? "1" : "0");
	text.back()+=" HISPD:";
	text.back()+=(state.HISPD ? "1" : "0");
	text.back()+=" INUSE:";
	text.back()+=(state.INUSE ? "1" : "0");

	text.push_back(line);
	text.back()="CLKSEL:";
	text.back()+=(state.CLKSEL ? "1" : "0");
	text.back()+=" DDEN:";
	text.back()+=(state.DDEN ? "1" : "0");
	text.back()+=" IRQMSK:";
	text.back()+=(state.IRQMSK ? "1" : "0");
	

	text.push_back(line);
	text.back()="RecordType:";
	text.back()+=(state.recordType ? "1" : "0");
	text.back()+=" RecNotFound:";
	text.back()+=(state.recordNotFound ? "1" : "0");
	text.back()+=" CRCError:";
	text.back()+=(state.CRCError ? "1" : "0");
	text.back()+=" LostData:";
	text.back()+=(state.lostData ? "1" : "0");

	text.push_back(line);
	text.back()="Last CMD:";
	text.back()+=cpputil::Ubtox(state.lastCmd);
	text.back()+=" STATUS:";
	text.back()+=cpputil::Ubtox(state.lastStatus);

	return text;
}
