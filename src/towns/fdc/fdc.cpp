#include <iostream>

#include "cpputil.h"
#include "fdc.h"
#include "townsdef.h"
#include "towns.h"


void TownsFDC::ImageFile::SaveIfModified(void)
{
	bool modified=false;
	for(int i=0; i<d77.GetNumDisk(); ++i)
	{
		auto diskPtr=d77.GetDisk(i);
		if(true==diskPtr->IsModified())
		{
			modified=true;
			break;
		}
	}

	if(true==modified)
	{
		std::cout << __FUNCTION__ << std::endl;
		std::cout << "I'm supposed to save modified disk image." << std::endl;
		std::cout << "But I haven't implemented yet." << std::endl;
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
		d.diskInserted=true; // Will be replaced with D77 disk image class
		d.trackPos=0;      // Actual head location.
		d.trackReg=0;      // Value in track register 0202H
		d.sectorReg=1;     // Value in sector register 0x04H
		d.dataReg=0;       // Value in data register 0x06H
	}
	driveSwitch=false;
	driveSelectBit=0;
	busy=false;
	MODEB=false;
	HISPD=false;
	INUSE=false;
	lastCmd=0;
	lastStatus=0;
}


////////////////////////////////////////////////////////////


TownsFDC::TownsFDC(class FMTowns *townsPtr)
{
	this->townsPtr=townsPtr;

	Reset();
	debugBreakOnCommandWrite=false;
	for(auto &d : state.drive)
	{
		d.imgFileNum=-1;
		d.diskIndex=-1;
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

////////////////////////////////////////////////////////////


void TownsFDC::SendCommand(unsigned int data)
{
	if(0xFE==data)
	{
		// Prob reset.
		Reset();
	}
	else if((data&0xF0)!=0xD0 && 0==state.driveSelectBit)
	{
		// Drive not selected.
		return;
	}
	else
	{
		auto &drv=state.drive[DriveSelect()];
		switch(data&0xF0)
		{
		case 0x00: // Restore
			std::cout << __FUNCTION__ << std::endl;
			std::cout << "Command " << cpputil::Ubtox(data) << " not supported yet." << std::endl;
			break;
		case 0x10: // Seek
			std::cout << __FUNCTION__ << std::endl;
			std::cout << "Command " << cpputil::Ubtox(data) << " not supported yet." << std::endl;
			break;
		case 0x20: // Step?
		case 0x30: // Step?
			std::cout << __FUNCTION__ << std::endl;
			std::cout << "Command " << cpputil::Ubtox(data) << " not supported yet." << std::endl;
			break;
		case 0x40: // Step In
		case 0x50: // Step In
			std::cout << __FUNCTION__ << std::endl;
			std::cout << "Command " << cpputil::Ubtox(data) << " not supported yet." << std::endl;
			break;
		case 0x60: // Step Out
		case 0x70: // Step Out
			std::cout << __FUNCTION__ << std::endl;
			std::cout << "Command " << cpputil::Ubtox(data) << " not supported yet." << std::endl;
			break;

		case 0x80: // Read Data (Read Sector)
		case 0x90: // Read Data (Read Sector)
			break;
		case 0xA0: // Write Data (Write Sector)
		case 0xB0: // Write Data (Write Sector)
			std::cout << __FUNCTION__ << std::endl;
			std::cout << "Command " << cpputil::Ubtox(data) << " not supported yet." << std::endl;
			break;

		case 0xC0: // Read Address
			std::cout << __FUNCTION__ << std::endl;
			std::cout << "Command " << cpputil::Ubtox(data) << " not supported yet." << std::endl;
			break;
		case 0xE0: // Read Track
			std::cout << __FUNCTION__ << std::endl;
			std::cout << "Command " << cpputil::Ubtox(data) << " not supported yet." << std::endl;
			break;
		case 0xF0: // Write Track
			std::cout << __FUNCTION__ << std::endl;
			std::cout << "Command " << cpputil::Ubtox(data) << " not supported yet." << std::endl;
			break;

		case 0xD0: // Force Interrupt
			if(true==state.busy)
			{
				state.lastCmd=data;
				state.busy=false;
				return; // Don't update status.
			}
			state.busy=false;
			break;
		}
	}
	state.lastCmd=data;
	state.lastStatus=MakeUpStatus(data);
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
	return false; // Tentative.
}
bool TownsFDC::SeekError(void) const
{
	return false;
}
bool TownsFDC::CRCError(void) const
{
	return false;
}
bool TownsFDC::IndexHole(void) const
{
	return false;
}
bool TownsFDC::RecordType(void) const
{
	return false;
}
bool TownsFDC::RecordNotFound(void) const
{
	return false;
}
bool TownsFDC::LostData(void) const
{
	return false;
}
bool TownsFDC::DataRequest(void) const
{
	return false;
}
bool TownsFDC::WriteFault(void) const
{
	return false;
}


////////////////////////////////////////////////////////////


/* virtual */ void TownsFDC::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_FDC_STATUS_COMMAND://       0x200, // [2] pp.253
		// During the start-up, System ROM writes 0FEH to this register,
		// which is not listed in the data sheet of MB
		SendCommand(data);
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
		break;
	case TOWNSIO_FDC_DRIVE_SELECT://         0x20C, // [2] pp.253
		if(0==state.driveSelectBit && 0!=(state.driveSelectBit&0x0F))
		{
			// Latch MODE-B and HISPD
			state.MODEB=(0!=(data&0x80));
			state.HISPD=(0!=(data&0x40));
		}
		state.INUSE=(0!=(data&0x10));
		state.driveSelectBit=(data&0x0F);
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
		return state.lastStatus;
		break;
	case TOWNSIO_FDC_TRACK://                0x202, // [2] pp.253
		break;
	case TOWNSIO_FDC_SECTOR://               0x204, // [2] pp.253
		break;
	case TOWNSIO_FDC_DATA://                 0x205, // [2] pp.253
		break;
	case TOWNSIO_FDC_DRIVE_STATUS_CONTROL:// 0x208, // [2] pp.253
		data=5; // Bit0:Always 1   Bit2:3.5inch drive
		data|=(DriveReady() ? 2 : 0);
		break;
	case TOWNSIO_FDC_DRIVE_SELECT://         0x20C, // [2] pp.253
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
