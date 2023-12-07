/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef DISKDIRVE_IS_INCLUDED
#define DISKDIRVE_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include "device.h"
#include "d77.h"



class DiskDrive : public Device
{
public:
	enum
	{
		IMGFILE_RAW,
		IMGFILE_D77,
		IMGFILE_RDD,
	};
	enum
	{
		NUM_DRIVES=4,
		RESTORE_TIME=            1000000,  //  1ms in Nano Seconds.  Just took arbitrary.  Need to make it real.
		SEEK_TIME=               1000000,  //  1ms in Nano Seconds.  Just took arbitrary.  Need to make it real.
		STEP_TIME=               1000000,  //  1ms in Nano Seconds.  Just took arbitrary.  Need to make it real.
		SECTOR_READ_WRITE_TIME=  1000000,  //  1ms in Nano Seconds.  Just took arbitrary.  Need to make it real.
		WRITE_TRACK_TIME=        1000000,  //  1ms in Nano Seconds.  Just took arbitrary.  Need to make it real.
		ADDRMARK_READ_TIME=      1000000,  //  1ms in Nano Seconds.  Just took arbitrary.  Need to make it real.  But, keep it fast for the time being.
		DISK_CHANGE_TIME=       50000000,  //  Minimum 50ms after disk insertion before drive is ready.

		FAST_DRIVE_WAIT_TIME=      10000,
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

	bool fastDrive=false;
	unsigned int RestoreTime(void) const
	{
		return (true==fastDrive ? FAST_DRIVE_WAIT_TIME : RESTORE_TIME);
	}
	unsigned int SeekTime(void) const
	{
		return (true==fastDrive ? FAST_DRIVE_WAIT_TIME : SEEK_TIME);
	}
	unsigned int StepTime(void) const
	{
		return (true==fastDrive ? FAST_DRIVE_WAIT_TIME : STEP_TIME);
	}
	unsigned int SectorReadWriteTime(void) const
	{
		return (true==fastDrive ? FAST_DRIVE_WAIT_TIME : SECTOR_READ_WRITE_TIME);
	}
	unsigned int WriteTrackTime(void) const
	{
		return (true==fastDrive ? FAST_DRIVE_WAIT_TIME : WRITE_TRACK_TIME);
	}
	unsigned int AddrMarkReadTime(void) const
	{
		return (true==fastDrive ? FAST_DRIVE_WAIT_TIME : ADDRMARK_READ_TIME);
	}

	class Sector
	{
	public:
		bool exists=false;
		uint8_t C,H,R,N,crcStatus,DDM;
		std::vector <uint8_t> data;

		void Make(unsigned int C,unsigned int H,unsigned int R,unsigned int N);
	};
	class DiskImage
	{
	public:
		int fileType;
		D77File d77;

		unsigned int GetNumDisk(void) const;

		bool IsModified(void) const;
		void ClearModifiedFlag(void);
		std::vector <unsigned char> MakeImageBinary(void) const;
		std::vector <unsigned char> MakeImageBinaryIfNotTooLong(unsigned int lengthThresholdInBytes) const;
		bool DiskLoaded(int diskIdx) const;

		/*! Identifies the disk type based on the whole capacity.
		*/
		unsigned int IdentifyDiskMediaType(int diskIdx) const;

		/*! Identifies the disk type based on the track capacity (sector_size*#sectors).
		*/
		static unsigned int IdentifyDiskMediaTypeFromTrackCapacity(unsigned int trackCapacity);

		void SetWriteProtect(int diskIdx,bool writeProtect);
		bool WriteProtected(int diskIdx) const;

		bool SetData(int fileType,const std::vector <unsigned char> &bin,bool verboseMode);

		Sector ReadSector(
		    int diskIdx,unsigned int trackPos,unsigned int side,
		    unsigned int C,unsigned int H,unsigned int R,bool verifySide) const;

		/*! Find a sector starting from searchStartFrom-th sector in the track.
		    It updates searchStartFrom, and also returns how many steps were required to get to the sector.
		*/
		Sector ReadSectorFrom(
		    int diskIdx,unsigned int trackPos,unsigned int side,
		    unsigned int C,unsigned int H,unsigned int R,bool verifySide,unsigned int &searchStartFrom,unsigned int &nSteps) const;

		/*! Returns nanoseconds per byte for given RPM.
		    If no information is stored, it returns zero.
		    To emulate sector-read-time protect.
		*/
		unsigned int GetNanoSecPerByte(int diskIdx,unsigned int C,unsigned int H,unsigned int R) const;

		bool WriteSector(int diskIdx,unsigned int C,unsigned int H,unsigned int R,size_t len,const uint8_t data[]);
		unsigned int GetSectorLength(int diskIdx,unsigned int C,unsigned int H,unsigned int R) const;
		bool SectorExists(int diskIdx,unsigned int C,unsigned int H,unsigned int R) const;
		bool WriteSector(int diskIdx,unsigned int C,unsigned int H,unsigned int R,const std::vector <uint8_t> &data);
		std::vector <uint8_t> ReadAddress(bool &crcError,int diskIdx,unsigned int cylinder,unsigned int side,unsigned int &sectorPos) const;

		void SetNumCylinders(int diskIdx,unsigned int n);

		/*! 
		*/
		std::vector <unsigned char> ReadTrack(int diskIdx,unsigned int C,unsigned int H) const;

		/*! Returns the new media type.
		*/
		unsigned int WriteTrack(int diskIdx,unsigned int C,unsigned int H,const std::vector <uint8_t> &data);

		uint16_t CalcCRC(const std::vector<uint8_t> &data) const {
			const uint32_t polynomial = 0b0001000100000010000100000000;   // 0001 0001 0000 0010 0001 [0000 0000]
			uint32_t crc_val = 0xe59a00;
			for(auto it=data.begin(); it!=data.end(); ++it)
			{
				crc_val |= (*it);
				for (size_t i = 0; i < 8; i++) {
					crc_val <<= 1;
					if (crc_val & 0x1000000) crc_val ^= polynomial;
				}
			}
			return crc_val >> 8;
		}
	};
	class ImageFile
	{
	public:
		std::string fName;
		DiskImage img;
		bool LoadD77orRAW(std::string fName);
		bool LoadD77(std::string fName);
		bool LoadRDD(std::string fName);
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
			int _sectorReg;     // Value in sector register 0x04H
			int dataReg;       // Value in data register 0x06H

			int lastSeekDir;   // For STEP command.
			int imgFileNum;    // Pointer to imgFile.
			int diskIndex;     // Disk Index in imgFile[imgFileNum]
			int mediaType;

			bool motor;
			bool diskChange;
			mutable int pretendDriveNotReadyCount=0;
			uint64_t pretendDriveNotReadyUntil=0;  // Not saved in the state.  Zero-ed when state-loaded.

			void DiskChanged(uint64_t vmTime);
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

		unsigned int dataReadPointer=0,expectedWriteLength=0;;
		std::vector <unsigned char> data; // For I/O read and write
		bool DRQ=false,IRQ=false,CRCErrorAfterRead=false;
		uint64_t lastDRQTime=0;

		unsigned int sectorPositionInTrack=0;
		unsigned int nanosecPerByte=0;
		long long int nextIndexHoleTime=0;
		bool DDMErrorAfterRead=false;

		long long int scheduleTime;

		void Reset(void);
	};

	State state;

	virtual const char *DeviceName(void) const{return "FDC";}

	DiskDrive(VMBase *vmBase);

	static bool IsD77Extension(std::string ext);
	static bool IsRDDExtension(std::string ext);

	bool LoadD77orRDDorRAW(unsigned int driveNum,const char fName[],uint64_t vmTime,bool verbose=true);

	bool LoadD77(unsigned int driveNum,const char fName[],uint64_t vmTime,bool verbose=true);
	bool LoadRDD(unsigned int driveNum,const char fName[],uint64_t vmTime,bool verbose=true);
	bool LoadRawBinary(unsigned int driveNum,const char fName[],uint64_t vmTime,bool verbose=true);

	/*! D77 image can have multiple disks in one file.
	    diskIdx is for pointing a disk in the multiple-disk D77 image.
	*/
	void LinkDiskImageToDrive(int imgIndex,int diskIdx,int driveNum,uint64_t vmTime);
private:
	void SaveIfModifiedAndUnlinkDiskImage(unsigned int imgIndex);

public:
	void Eject(unsigned int driveNum);
	ImageFile *GetDriveImageFile(int driveNum);
	const ImageFile *GetDriveImageFile(int driveNum) const;

	void SaveModifiedDiskImages(void);

	void SetWriteProtect(int driveNum,bool writeProtect);

	/*! Returns true if disk media type and drive mode is compatible.
	    Drive mode does not distinguish 720KB and 640KB modes.
	    Both two parameters needs to be MEDIA_???.
	*/
	inline bool CheckMediaTypeAndDriveModeCompatible(unsigned int mediaType,unsigned int driveMode) const
	{
		// TOWNS can read 2D disk, but it is reading as 2DD disk and stepping two tracks at a time.
		// From hardware point of view TOWNS's floppy disk drive apparently is not distinguishing
		// 2D and 2DD.
		// Which means, it officially only support reading 2D, but probably it can format unformatted
		// disk and format even-number tracks to format to 2D disk.
		if(MEDIA_2D==mediaType)
		{
			mediaType=MEDIA_2DD_640KB;
		}
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
		if(MEDIA_2D==mediaType)
		{
			mediaType=MEDIA_2DD_640KB;
		}
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


	void SendCommand(unsigned int data,uint64_t vmTime);  // Give townsTime to vmTime.

	unsigned int CommandToCommandType(unsigned int cmd) const;
	unsigned char MakeUpStatus(unsigned int cmd,uint64_t vmTime) const;

	/*! Returns the selected drive.  Or zero(FD0) if no drive is selected.
	*/
	unsigned int DriveSelect(void) const;

	/*! Returns media type the drive is configured for.
	    Apparently the same FDC settings for 640KB and 720KB.  So, if it is set for 2DD, it returns MEDIA_2DD_640KB.
	*/
	unsigned int GetDriveMode(void) const;

	bool DiskLoaded(int driveNum) const;
	bool DriveReady(uint64_t vmTime) const;
	bool WriteProtected(void) const;
	bool SeekError(void) const;
	bool CRCError(void) const;
	bool IndexHole(void) const;
	bool RecordType(void) const;
	bool RecordNotFound(void) const;
	bool LostData(void) const;
	bool DataRequest(void) const;
	bool WriteFault(void) const;

	void Reset(void);

	std::vector <std::string> GetStatusText(void) const;
	static std::string FDCCommandToExplanation(unsigned char cmd);
	static std::string MediaTypeToString(unsigned int mediaType);

	int GetTrackReg(void) const;
	int GetSectorReg(void) const;
	int GetSectorReg(unsigned int drvSel) const;
	void SetSectorReg(int num);


	virtual uint32_t SerializeVersion(void) const;
	void SerializeVersion0to6(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	bool DeserializeVersion0to6(const unsigned char *&data,std::string stateFName,uint32_t version);
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};


/* } */
#endif
