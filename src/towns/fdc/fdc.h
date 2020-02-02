#ifndef FDC_IS_INCLUDED
#define FDC_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include "device.h"
#include "d77.h"



class TownsFDC : public Device
{
public:
	enum
	{
		IMGFILE_RAW,
		IMGFILE_D77
	};
	enum
	{
		NUM_DRIVES=4,
		RESTORE_TIME=20000,     // In Nano Seconds.  Just arbitrary.  Need to make it real.
		SEEK_TIME=20000,        // In Nano Seconds.  Just arbitrary.  Need to make it real.
		STEP_TIME=10000,
		SECTOR_READ_WRITE_TIME=5000,  // In Nano Seconds.  Just arbitrary.  Need to make it real.
	};
	class ImageFile
	{
	public:
		int fileType;
		std::string fName;
		D77File d77;
		void SaveIfModified(void);
	};
	ImageFile imgFile[NUM_DRIVES];

	class State
	{
	public:
		class Drive
		{
		public:
			int trackPos;      // Actual head location.
			int trackReg;      // Value in track register 0202H
			int sectorReg;     // Value in sector register 0x04H
			int dataReg;       // Value in data register 0x06H

			int lastSeekDir;   // For STEP command.
			int imgFileNum;    // Pointer to imgFile.
			int diskIndex;     // Disk Index in imgFile[imgFileNum]

			bool motor;
		};

		Drive drive[NUM_DRIVES];
		bool driveSwitch;  // [2] pp.258
		bool busy;
		bool MODEB,HISPD;  // [2] pp.258, pp.809
		bool INUSE;
		unsigned int side; // Is side common for all drives?  Or Per drive?
		bool CLKSEL,DDEN,IRQMSK;

		unsigned int driveSelectBit;
		unsigned int lastCmd;
		unsigned int lastStatus;

		bool recordType,recordNotFound,CRCError,lostData;

		long long int scheduleTime;

		void Reset(void);
	};

	class FMTowns *townsPtr;
	class TownsDMAC *DMACPtr;

	State state;

	bool debugBreakOnCommandWrite;

	virtual const char *DeviceName(void) const{return "FDC";}

	TownsFDC(class FMTowns *townsPtr,class TownsDMAC *dmacPtr);

	bool LoadRawBinary(unsigned int driveNum,const char fName[],bool verbose=true);
	D77File::D77Disk *GetDriveDisk(int driveNum);
	const D77File::D77Disk *GetDriveDisk(int driveNum) const;
	ImageFile *GetDriveImageFile(int driveNum);
	const ImageFile *GetDriveImageFile(int driveNum) const;

	void SendCommand(unsigned int data);

	unsigned int CommandToCommandType(unsigned int cmd) const;
	unsigned char MakeUpStatus(unsigned int cmd) const;
	unsigned int DriveSelect(void) const;

	bool DriveReady(void) const;
	bool WriteProtected(void) const;
	bool SeekError(void) const;
	bool CRCError(void) const;
	bool IndexHole(void) const;
	bool RecordType(void) const;
	bool RecordNotFound(void) const;
	bool LostData(void) const;
	bool DataRequest(void) const;
	bool WriteFault(void) const;

	virtual void RunScheduledTask(unsigned long long int townsTime);
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void Reset(void);

	std::vector <std::string> GetStatusText(void) const;
};


/* } */
#endif
