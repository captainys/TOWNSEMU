/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
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
		RESTORE_TIME=            1000000,  //  1ms in Nano Seconds.  Just took arbitrary.  Need to make it real.
		SEEK_TIME=               1000000,  //  1ms in Nano Seconds.  Just took arbitrary.  Need to make it real.
		STEP_TIME=               1000000,  //  1ms in Nano Seconds.  Just took arbitrary.  Need to make it real.
		SECTOR_READ_WRITE_TIME=  1000000,  //  1ms in Nano Seconds.  Just took arbitrary.  Need to make it real.
		WRITE_TRACK_TIME=        1000000,  //  1ms in Nano Seconds.  Just took arbitrary.  Need to make it real.
		ADDRMARK_READ_TIME=      1000000,  //  5ms in Nano Seconds.  Just took arbitrary.  Need to make it real.
	};
	enum
	{
		MEDIA_UNKNOWN=0,
		MEDIA_2D=1,
		MEDIA_2DD_640KB=2,
		MEDIA_2DD_720KB=3,
		MEDIA_2HD_1232KB=4,
		MEDIA_2HD_1440KB=5,
		MEDIA_SINGLE_DENSITY=0xFF,  // Not supported by TSUGARU.
	};
	class ImageFile
	{
	public:
		int fileType;
		std::string fName;
		D77File d77;
		bool LoadD77orRAW(std::string fName);
		bool LoadD77(std::string fName);
		bool LoadRAW(std::string fName);
		void SaveIfModified(void);
	};
	ImageFile imgFile[NUM_DRIVES];

	std::vector <std::string> searchPaths;

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
			int mediaType;

			bool motor;
			bool diskChange;
			mutable int pretendDriveNotReadyCount=0;

			void DiskChanged(void);
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

		bool recordType,recordNotFound,CRCError,lostData,writeFault;
		unsigned int addrMarkReadCount;

		long long int scheduleTime;

		void Reset(void);
	};

	class FMTowns *townsPtr;
	class TownsPIC *PICPtr;
	class TownsDMAC *DMACPtr;

	State state;

	bool debugBreakOnCommandWrite;

	virtual const char *DeviceName(void) const{return "FDC";}

	static bool IsD77Extension(std::string ext);

	TownsFDC(class FMTowns *townsPtr,class TownsPIC *PICPtr,class TownsDMAC *dmacPtr);

	bool LoadD77orRAW(unsigned int driveNum,const char fName[],bool verbose=true);

	bool LoadD77(unsigned int driveNum,const char fName[],bool verbose=true);
	bool LoadRawBinary(unsigned int driveNum,const char fName[],bool verbose=true);

	/*! D77 image can have multiple disks in one file.
	    diskIdx is for pointing a disk in the multiple-disk D77 image.
	*/
	void LinkDiskImageToDrive(int imgIndex,int diskIdx,int driveNum);
private:
	void SaveIfModifiedAndUnlinkDiskImage(unsigned int imgIndex);

public:
	void Eject(unsigned int driveNum);
	D77File::D77Disk *GetDriveDisk(int driveNum);
	const D77File::D77Disk *GetDriveDisk(int driveNum) const;
	ImageFile *GetDriveImageFile(int driveNum);
	const ImageFile *GetDriveImageFile(int driveNum) const;

	void SaveModifiedDiskImages(void);

	void SetWriteProtect(int driveNum,bool writeProtect);

	/*! Identifies the disk type based on the whole capacity.
	*/
	unsigned int IdentifyDiskMediaType(const D77File::D77Disk *diskPtr) const;

	/*! Identifies the disk type based on the track capacity (sector_size*#sectors).
	*/
	unsigned int IdentifyDiskMediaTypeFromTrackCapacity(unsigned int trackCapacity) const;

	/*! Returns true if disk media type and drive mode is compatible.
	    Drive mode does not distinguish 720KB and 640KB modes.
	    Both two parameters needs to be MEDIA_???.
	*/
	inline bool CheckMediaTypeAndDriveModeCompatible(unsigned int mediaType,unsigned int driveMode) const
	{
		if(MEDIA_2DD_720KB==mediaType)
		{
			mediaType=MEDIA_2DD_640KB;
		}
		if(MEDIA_2DD_720KB==driveMode)
		{
			driveMode=MEDIA_2DD_640KB;
		}
		return mediaType==driveMode;
	}

	/*! Returns true if disk media type and drive mode is compatible for formatting.
	    1.23MB floppy disk can be formatted to 1.44MB.
	    Both two parameters needs to be MEDIA_???.
	*/
	inline bool CheckMediaTypeAndDriveModeCompatibleForFormat(unsigned int mediaType,unsigned int driveMode) const
	{
		if(MEDIA_2DD_720KB==mediaType)
		{
			mediaType=MEDIA_2DD_640KB;
		}
		if(MEDIA_2HD_1440KB==mediaType)
		{
			mediaType=MEDIA_2HD_1232KB;
		}
		if(MEDIA_2DD_720KB==driveMode)
		{
			driveMode=MEDIA_2DD_640KB;
		}
		if(MEDIA_2HD_1440KB==driveMode)
		{
			driveMode=MEDIA_2HD_1232KB;
		}
		return mediaType==driveMode;
	}


	void SendCommand(unsigned int data);

	unsigned int CommandToCommandType(unsigned int cmd) const;
	unsigned char MakeUpStatus(unsigned int cmd) const;

	/*! Returns the selected drive.  Or zero(FD0) if no drive is selected.
	*/
	unsigned int DriveSelect(void) const;

	/*! Returns media type the drive is configured for.
	    Apparently the same FDC settings for 640KB and 720KB.  So, if it is set for 2DD, it returns MEDIA_2DD_640KB.
	*/
	unsigned int GetDriveMode(void) const;

	/*! Turns off BUSY flag.  Also if IRQ is not masked it raises IRR flag of PIC.
	*/
	void MakeReady(void);

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
	static std::string FDCCommandToExplanation(unsigned char cmd);
	static std::string MediaTypeToString(unsigned int mediaType);



	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};


/* } */
#endif
