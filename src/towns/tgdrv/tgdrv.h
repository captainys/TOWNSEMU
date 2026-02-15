/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */

#ifndef TSUGARUDRIVE_IS_INCLUDED
#define TSUGARUDRIVE_IS_INCLUDED
/* { */

#include "device.h"
#include "townsdef.h"
#include "cpputil.h"
#include "filesys.h"
#include "i486.h"



/*
add_library(townsnewdevice newdevice.h newdevice.cpp)
target_link_libraries(townsnewdevice device cpputil towns townsdef)
target_include_directories(townsnewdevice PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
*/

/*
	allDevices.push_back(newDevice);
*/


class TownsTgDrv : public Device
{
private:
	class FMTownsCommon *townsPtr;
	bool monitor=false;
public:
	virtual const char *DeviceName(void) const{return "TGDRIVE";}

	const uint32_t TGDRV_ID=0x52444754; // "TGDR"
	const uint16_t TGDRV_ID_SHORT=0x4754; // "TG"

	const uint16_t DRIVELETTER_BUFFER=0x0109; // CS:0109H 8-byte drive-letter buffer
	const uint16_t DUMMYDPB_BUFFER=0x211;

	bool useSlashSlash=false; // Keep it false for compatible ROM.
	FileSys sharedDir[TOWNS_TGDRV_MAX_NUM_DRIVES];

	void SetMonitor(bool monitor);
	bool GetMonitor(void) const;

	class State
	{
	public:
		State();
		void PowerOn(void);
		void Reset(void);
	};

	class DOSDPB
	{
	public:
		unsigned int  DRIVE_CODE;		//	DB		? 		; 0=A drive
		unsigned int  UNIT_CODE;			//	DB		?		; +01h
		unsigned int  BYTES_PER_SECTOR;	//	DW		?		; +02h
		unsigned int  CLUSTER_MASK;		//	DB		?		; +04h
		unsigned int  CLUSTER_SHIFT;		//	DB		?		; +05h  SHL CLUSTER to get to SECTOR from the top of Data Sector
		unsigned int  FIRST_FAT_SECTOR;	//	DW		?		; +06h
		unsigned int  NUM_FATS;			//	DB		?		; +08h
		unsigned int  NUM_DIRENTS;		//	DW		?		; +09h
		unsigned int  FIRST_DATA_SECTOR;	//	DW		?		; +0bh
		unsigned int  MAX_CLUSTER_NUM;	//	DW		?		; +0dh
		unsigned int  SECTORS_PER_FAT;	//	DB		?		; +0fh  1 byte in DOS V3.x  2 bytes V4.x and later
		unsigned int  FIRST_DIR_SECTOR;	//	DW		?		; +10h/+11h  How many sectors to skip to get to the root dir.
		unsigned int  DEV_DRIVER_OFFSET; //	DW		?		; +12h/+13h
		unsigned int  DEV_DRIVER_SEG; //	DW		?		; +14h/+15h
		unsigned int  MEDIA_DESC_TYPE;	//	DB		?		; +16h/+17h  See BPB_MEDIA_DESC_TYPE
		unsigned int  ACCESS_FLAG;		//	DB		?		; +17h/+18h
		unsigned int  NEXT_DPB_OFFSET;	//	DW		?		; +18h/+19h
		unsigned int  NEXT_DPB_SEG;		//	DW		?		; +1Ah/+1Bh  Set in Finalize_DPB_Loop in MSINIT.ASM(MS-DOS V2.0 soruce)
		unsigned int  LAST_CLUSTER_ALLOC;//	DW		?		; +1Ch/+1Dh  Is this LAST_CLUSTER_ALLOC?  Or is this current working directory.
		unsigned int  NUM_FREE_CLUSTERS;	//	DW		?		; +1Eh/+1Fh
									//	; 20h bytes total Ver 3.x, 21h bytes total Ver 4.x and later
	};

	State state;

	TownsTgDrv(class FMTownsCommon *townsPtr);

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);

	bool Int2F_1101_Rmdir(void);
	bool Int2F_1103_Mkdir(void);
	bool Int2F_1105_Chdir(void);
	bool Int2F_1108_ReadFromRemoteFile(void);
	bool Int2F_1109_WriteToRemoteFile(void);
	bool Int2F_1106_CloseRemoteFile(void);
	bool Int2F_1107_Flush(void);
	bool Int2F_110C_GetDiskInformation(void);
	bool Int2F_110E_SetFileAttrib(void);
	bool Int2F_110F_GetFileAttrib(void);
	bool Int2F_1111_Rename(void);
	bool Int2F_1113_Delete(void);
	bool Int2F_1116_OpenExistingFile(void);
	bool Int2F_1117_CreateOrTruncate(void);
	bool Int2F_111B_FindFirst(void);
	bool Int2F_111C_FindNext(void);
	bool Int2F_111D_CloseAll(void);
	bool Int2F_1123_QualifyRemoteFileName(void);
	bool Int2F_1125_RedirectedPrinterMode(void);
	bool Int2F_112E_ExtendedOpenOrCreate(void);

	/*! Check file name.  If it includes ".." it returns DOS error code.
	    Also it blocks kanji and kana characters.
	    Input file name must be a fully-qualified file name from DOS.
	*/
	unsigned int CheckFileName(const std::string &fName) const;

	uint16_t FetchPSP(void) const;
	uint16_t FetchStackParam0(void) const;
	uint16_t FetchStackParam1(void) const;
	uint16_t FetchStackParam2(void) const;
	uint16_t FetchStackParam3(void) const;
	void ReturnAX(uint16_t ax);
	void ReturnBX(uint16_t ax);
	void ReturnCX(uint16_t ax);
	void ReturnDX(uint16_t ax);

	int FullyQualifiedFileNameToSharedDirIndex(const std::string &fn) const;
	char FullyQualifiedFileNameToDriveLetter(const std::string &fn) const;
	int DriveLetterToSharedDirIndex(char letter) const;
	void MakeDOSDirEnt(uint32_t DTABuffer,const FileSys::DirectoryEntry &dirent);
	void MakeVMSFT(const class i486DXCommon::SegmentRegister &seg,uint32_t offset,char driveLetter,int hostSFTIdx,FileSys::SystemFileTable &hostSFT);
	unsigned int FetchDriveCodeFromSFT(const class i486DXCommon::SegmentRegister &seg,uint32_t offset) const;
	uint16_t FetchSFTReferenceCount(const class i486DXCommon::SegmentRegister &seg,uint32_t offset) const;
	uint32_t FetchFilePositionFromSFT(const class i486DXCommon::SegmentRegister &seg,uint32_t offset) const;
	unsigned int FetchDeviceInfoFromSFT(const class i486DXCommon::SegmentRegister &seg,uint32_t offset) const;
	std::string FetchCString(uint32_t physAddr) const;
	std::string FetchCString(const class i486DXCommon::SegmentRegister &seg,uint32_t offset) const;
	uint32_t FetchDOSDateTimeFromSFT(const class i486DXCommon::SegmentRegister &seg,uint32_t offset) const;

	uint8_t FetchByte(uint32_t linearAddr) const;
	uint16_t FetchWord(uint32_t linearAddr) const;
	uint32_t FetchDword(uint32_t linearAddr) const;
	void StoreByte(uint32_t linearAddr,uint8_t data);
	void StoreWord(uint32_t linearAddr,uint16_t data);
	void StoreDword(uint32_t linearAddr,uint32_t data);

	void AddDPB(unsigned int lastDPBSEG,unsigned int lastDPBOFFSET,unsigned int newDPBSEG,unsigned int newDPBOFFSET);
	DOSDPB FetchDPB(unsigned int SEG,unsigned int OFFSET) const;
	void StoreDPB(unsigned int SEG,unsigned int OFFSET,DOSDPB dpb);


	bool Install(void);

	unsigned int DriveLetterToDriveIndex(char drvLetter) const;
	char DriveIndexToDriveLetter(unsigned int driveIndex) const;
	unsigned int GetCDSCount(void) const;
	unsigned int GetCDSLength(void) const;
	std::string GetFilenameBuffer1(void) const;
	std::string GetFilenameBuffer2(void) const;
	std::string GetLastOfFilename(std::string in) const;
	std::string FilenameTo11Bytes(std::string in) const;
	std::string FullPathToSubDir(std::string fn) const;
	std::string DropDriveLetter(std::string ful) const;
	uint32_t GetCDSAddress(unsigned int driveIndex) const ; // 0 means A drive
	uint16_t GetCDSType(unsigned int driveIndex) const ;  // 0 means A drive
	uint32_t GetDTAAddress(void) const;
	uint32_t GetFilenameBufferAddress(void) const;
	uint32_t GetFilenameBuffer2Address(void) const;
	uint32_t GetSDBAddress(void) const;
	uint16_t GetSAttr(void) const;
	uint32_t GetSAttrAddress(void) const;
	uint32_t GetDPBSize(void) const;

	std::vector <std::string> GetStatusText(void) const;

	/*! Version used for serialization.
	*/
	virtual uint32_t SerializeVersion(void) const;
	/*! Device-specific Serialization.
	*/
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	/*! Device-specific De-serialization.
	*/
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};

/* } */
#endif
