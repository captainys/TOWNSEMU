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
	class FMTowns *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "TGDRIVE";}

	const uint32_t TGDRV_ID=0x52444754; // "TGDR"
	const uint16_t TGDRV_ID_SHORT=0x4754; // "TG"

	FileSys sharedDir[TOWNS_TGDRV_MAX_NUM_DRIVES];

	class State
	{
	public:
		State();
		void PowerOn(void);
		void Reset(void);

		char driveLetters[TOWNS_TGDRV_MAX_NUM_DRIVES];
	};

	State state;

	TownsTgDrv(class FMTowns *townsPtr);

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
	bool Int2F_110C_GetDiskInformation(void);
	bool Int2F_110F_GetFileAttrib(void);
	bool Int2F_1111_Rename(void);
	bool Int2F_1113_Delete(void);
	bool Int2F_1116_OpenExistingFile(void);
	bool Int2F_1117_OpenOrTruncate(void);
	bool Int2F_111B_FindFirst(void);
	bool Int2F_111C_FindNext(void);
	bool Int2F_1123_QualifyRemoteFileName(void);
	bool Int2F_1125_RedirectedPrinterMode(void);

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
	void MakeVMSFT(const class i486DX::SegmentRegister &seg,uint32_t offset,char driveLetter,int hostSFTIdx,FileSys::SystemFileTable &hostSFT);
	unsigned int FetchDriveCodeFromSFT(const class i486DX::SegmentRegister &seg,uint32_t offset) const;
	uint32_t FetchFilePositionFromSFT(const class i486DX::SegmentRegister &seg,uint32_t offset) const;
	unsigned int FetchDeviceInfoFromSFT(const class i486DX::SegmentRegister &seg,uint32_t offset) const;
	std::string FetchCString(uint32_t physAddr) const;
	std::string FetchCString(const class i486DX::SegmentRegister &seg,uint32_t offset) const;


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
