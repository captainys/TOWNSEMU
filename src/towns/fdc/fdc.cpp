#include "cpputil.h"
#include "fdc.h"
#include "townsdef.h"
#include "towns.h"



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
	driveSelect=0;
	lastCmd=0;
	lastStatus=0;
	busy=false;
}
void TownsFDC::State::SendCommand(unsigned int data)
{
	lastCmd=data;
	lastStatus=MakeUpStatus(data);
}
unsigned int TownsFDC::State::CommandToCommandType(unsigned int cmd) const
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
unsigned char TownsFDC::State::MakeUpStatus(unsigned int cmd) const
{
	unsigned char data=0;
	if(0xFE==cmd)
	{
		cmd=0xD0; // System ROM is using this 0xFE.  Same as 0xD0?  Or Reset?
	}
	data|=(true!=DriveReady() ? 0x80 : 0);
	data|=(true==busy ? 0x01 : 0);
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
		data|=(drive[driveSelect].trackPos==0 ? 0x04 : 0);
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
		data|=(drive[driveSelect].trackPos==0 ? 0x04 : 0);
		data|=(IndexHole() ?      0x02 : 0);
		data&=0xFE;
		break;
	}
	return data;
}
bool TownsFDC::State::DriveReady(void) const
{
	return true; // Tentative.
}
bool TownsFDC::State::WriteProtected(void) const
{
	return false; // Tentative.
}
bool TownsFDC::State::SeekError(void) const
{
	return false;
}
bool TownsFDC::State::CRCError(void) const
{
	return false;
}
bool TownsFDC::State::IndexHole(void) const
{
	return false;
}
bool TownsFDC::State::RecordType(void) const
{
	return false;
}
bool TownsFDC::State::RecordNotFound(void) const
{
	return false;
}
bool TownsFDC::State::LostData(void) const
{
	return false;
}
bool TownsFDC::State::DataRequest(void) const
{
	return false;
}
bool TownsFDC::State::WriteFault(void) const
{
	return false;
}

////////////////////////////////////////////////////////////


TownsFDC::TownsFDC(class FMTowns *townsPtr)
{
	this->townsPtr=townsPtr;

	Reset();
	debugBreakOnCommandWrite=false;
}

/* virtual */ void TownsFDC::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_FDC_STATUS_COMMAND://       0x200, // [2] pp.253
		// During the start-up, System ROM writes 0FEH to this register,
		// which is not listed in the data sheet of MB
		state.SendCommand(data);
		if(true==debugBreakOnCommandWrite)
		{
			townsPtr->debugger.ExternalBreak("FDC Command Write"+cpputil::Ubtox(data));
		}
		break;
	case TOWNSIO_FDC_TRACK://                0x202, // [2] pp.253
		state.drive[state.driveSelect].trackReg=data;
		break;
	case TOWNSIO_FDC_SECTOR://               0x204, // [2] pp.253
		state.drive[state.driveSelect].sectorReg=data;
		break;
	case TOWNSIO_FDC_DATA://                 0x205, // [2] pp.253
		state.drive[state.driveSelect].dataReg=data;
		break;
	case TOWNSIO_FDC_DRIVE_STATUS_CONTROL:// 0x208, // [2] pp.253
		break;
	case TOWNSIO_FDC_DRIVE_SELECT://         0x20C, // [2] pp.253
		if(data&1)
		{
			state.driveSelect=0;
		}
		else if(data&2)
		{
			state.driveSelect=1;
		}
		else if(data&4)
		{
			state.driveSelect=2;
		}
		else if(data&8)
		{
			state.driveSelect=3;
		}
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
		data|=(state.DriveReady() ? 2 : 0);
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
