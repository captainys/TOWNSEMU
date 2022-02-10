/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include "tgdrv.h"
#include "towns.h"

TownsTgDrv::State::State()
{
}
void TownsTgDrv::State::PowerOn(void)
{
}
void TownsTgDrv::State::Reset(void)
{
}

TownsTgDrv::TownsTgDrv(class FMTowns *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
}
/* virtual */ void TownsTgDrv::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsTgDrv::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsTgDrv::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_VM_TGDRV:
		switch(data)
		{
		case TOWNS_VM_TGDRV_INSTALL://     0x01,
			// Input:
			//   CS:0109H(DRIVELETTER_BUFFER) is 8-byte drive-letter buffer to be filled. (Initial all FFh)
			//   CS:0111H(ERRMSG_BUFFER) is 256-byte buffer for sending error messages to the client.

			std::cout << "Installing Tsugaru Drive." << std::endl;
			if(true==Install())
			{
				townsPtr->cpu.SetCF(false);
			}
			else
			{
				townsPtr->cpu.SetCF(true);
			}
			break;
		case TOWNS_VM_TGDRV_INT2FH://      0x02,
			// To use AL for OUT DX,AL  AX is copied to BX.
			// AX is also at SS:[SP]
			std::cout << "INT 2FH Intercept. Req=" << cpputil::Ustox(townsPtr->cpu.GetBX()) << std::endl;
			// Set PF if not my drive.
			// Clear PF if my drive.
			bool myDrive=false;
			switch(townsPtr->cpu.GetBX())
			{
			case 0x6809:
				// Installation Check.
				//   If installed, return:
				//     AX=4754 "TG"
				//     BX=Installed Segment
				//     CF=clear
				myDrive=true;
				ReturnAX(TGDRV_ID_SHORT);
				ReturnBX(townsPtr->cpu.state.CS().value);
				townsPtr->cpu.SetCF(false);
				break;
			case 0x1101:
				myDrive=Int2F_1101_Rmdir();
				break;
			case 0x1103:
				myDrive=Int2F_1103_Mkdir();
				break;
			case 0x1105:
				myDrive=Int2F_1105_Chdir();
				break;
			case 0x1106:
				myDrive=Int2F_1106_CloseRemoteFile();
				break;
			case 0x1107:
				myDrive=Int2F_1107_Flush();
				break;
			case 0x1108:
				myDrive=Int2F_1108_ReadFromRemoteFile();
				break;
			case 0x1109:
				myDrive=Int2F_1109_WriteToRemoteFile();
				break;
			case 0x110C:
				myDrive=Int2F_110C_GetDiskInformation();
				break;
			case 0x110E:
				myDrive=Int2F_110E_SetFileAttrib();
				break;
			case 0x110F:
				myDrive=Int2F_110F_GetFileAttrib();
				break;
			case 0x1111:
				myDrive=Int2F_1111_Rename();
				break;
			case 0x1113:
				myDrive=Int2F_1113_Delete();
				break;
			case 0x1116:
				myDrive=Int2F_1116_OpenExistingFile();
				break;
			case 0x1117:
				myDrive=Int2F_1117_OpenOrTruncate();
				break;
			case 0x111B:
				myDrive=Int2F_111B_FindFirst();
				break;
			case 0x111C:
				myDrive=Int2F_111C_FindNext();
				break;
			case 0x111D:
				myDrive=Int2F_111D_CloseAll();
				break;
			case 0x1123:
				myDrive=Int2F_1123_QualifyRemoteFileName();
				break;
			case 0x1125:
				myDrive=Int2F_1125_RedirectedPrinterMode();
				break;
			}
			townsPtr->cpu.SetPF(true!=myDrive);
			break;
		}
		break;
	}
}
/* virtual */ unsigned int TownsTgDrv::IOReadByte(unsigned int ioport)
{
	return 0xff;
}

bool TownsTgDrv::Int2F_1101_Rmdir(void)
{
	auto fName=GetFilenameBuffer1();
	auto driveLetter=FullyQualifiedFileNameToDriveLetter(fName);
	auto sharedDirIdx=FullyQualifiedFileNameToSharedDirIndex(fName);
	if(0<=sharedDirIdx)
	{
		auto subPath=DropDriveLetter(fName);
		std::cout << fName << std::endl;
		std::cout << subPath << std::endl;

		auto invalidErr=CheckFileName(subPath);
		if(TOWNS_DOSERR_NO_ERROR!=invalidErr)
		{
			ReturnAX(invalidErr);
			townsPtr->cpu.SetCF(true);
			return true; // Yes it's my drive.
		}

		if(true==sharedDir[sharedDirIdx].RmdirSubPath(subPath))
		{
			townsPtr->cpu.SetCF(false);
		}
		else
		{
			ReturnAX(TOWNS_DOSERR_INVALID_ACCESS);
			townsPtr->cpu.SetCF(true);
		}

		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_1103_Mkdir(void)
{
	auto fName=GetFilenameBuffer1();
	auto driveLetter=FullyQualifiedFileNameToDriveLetter(fName);
	auto sharedDirIdx=FullyQualifiedFileNameToSharedDirIndex(fName);
	if(0<=sharedDirIdx)
	{
		auto subPath=DropDriveLetter(fName);
		std::cout << fName << std::endl;
		std::cout << subPath << std::endl;

		auto invalidErr=CheckFileName(subPath);
		if(TOWNS_DOSERR_NO_ERROR!=invalidErr)
		{
			ReturnAX(invalidErr);
			townsPtr->cpu.SetCF(true);
			return true; // Yes it's my drive.
		}

		if(true==sharedDir[sharedDirIdx].MkdirSubPath(subPath))
		{
			townsPtr->cpu.SetCF(false);
		}
		else
		{
			ReturnAX(TOWNS_DOSERR_INVALID_ACCESS);
			townsPtr->cpu.SetCF(true);
		}

		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_1105_Chdir(void)
{
	auto fName=GetFilenameBuffer1();
	auto driveLetter=FullyQualifiedFileNameToDriveLetter(fName);
	auto sharedDirIdx=FullyQualifiedFileNameToSharedDirIndex(fName);
	if(0<=sharedDirIdx)
	{
		auto CDSAddr=GetCDSAddress(DriveLetterToDriveIndex(driveLetter));

		auto subPath=DropDriveLetter(fName);
		std::cout << fName << std::endl;
		std::cout << subPath << std::endl;

		auto invalidErr=CheckFileName(subPath);
		if(TOWNS_DOSERR_NO_ERROR!=invalidErr)
		{
			ReturnAX(invalidErr);
			townsPtr->cpu.SetCF(true);
			return true; // Yes it's my drive.
		}

		if(0==subPath.size() || "\\"==subPath || "/"==subPath)
		{
			for(int i=6; i<0x43; ++i)
			{
				townsPtr->mem.StoreByte(CDSAddr+i,0);
			}
			townsPtr->cpu.SetCF(false);
		}
		else if(true==sharedDir[sharedDirIdx].SubPathIsDirectory(subPath) && subPath.size()<0x43-7)
		{
			for(int i=0; i<subPath.size() && i+6<0x42; ++i)
			{
				townsPtr->mem.StoreByte(CDSAddr+6+i,subPath[i]);
			}
			for(int i=subPath.size(); i+6<0x43; ++i)
			{
				townsPtr->mem.StoreByte(CDSAddr+6+i,0);
			}
			townsPtr->cpu.SetCF(false);
		}
		else
		{
			ReturnAX(TOWNS_DOSERR_FILE_NOT_FOUND);
			townsPtr->cpu.SetCF(true);
		}
		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_1106_CloseRemoteFile(void)
{
	char drvLetter='A'+FetchDriveCodeFromSFT(townsPtr->cpu.state.ES(),townsPtr->cpu.state.DI());
	auto sharedDirIdx=DriveLetterToSharedDirIndex(drvLetter);
	if(0<=sharedDirIdx)
	{
		auto hostSFTIdx=townsPtr->cpu.FetchWord(
		    townsPtr->cpu.state.CS().addressSize,
		    townsPtr->cpu.state.ES(),
		    townsPtr->cpu.state.DI()+0x0B,
		    townsPtr->mem);
		if(0<=hostSFTIdx &&
		   hostSFTIdx<FileSys::MAX_NUM_OPEN_FILE &&
		   true==sharedDir[sharedDirIdx].sft[hostSFTIdx].IsOpen())
		{
			sharedDir[sharedDirIdx].CloseFile(hostSFTIdx);
			townsPtr->cpu.StoreWord(
				townsPtr->mem,
				townsPtr->cpu.state.CS().addressSize,
				townsPtr->cpu.state.ES(),
				townsPtr->cpu.state.DI(),
				0);  // Clear ref count.
			townsPtr->cpu.SetCF(false);
		}
		else
		{
			// File not open.
			ReturnAX(TOWNS_DOSERR_INVALID_HANDLE);
			townsPtr->cpu.SetCF(true);
		}
		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_1107_Flush(void)
{
	char drvLetter='A'+FetchDriveCodeFromSFT(townsPtr->cpu.state.ES(),townsPtr->cpu.state.DI());
	auto sharedDirIdx=DriveLetterToSharedDirIndex(drvLetter);
	if(0<=sharedDirIdx)
	{
		// There's nothing to flush.
		townsPtr->cpu.SetCF(false);
		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_1108_ReadFromRemoteFile(void)
{
	char drvLetter='A'+FetchDriveCodeFromSFT(townsPtr->cpu.state.ES(),townsPtr->cpu.state.DI());
	auto sharedDirIdx=DriveLetterToSharedDirIndex(drvLetter);
	if(0<=sharedDirIdx)
	{
		auto hostSFTIdx=townsPtr->cpu.FetchWord(
		    townsPtr->cpu.state.CS().addressSize,
		    townsPtr->cpu.state.ES(),
		    townsPtr->cpu.state.DI()+0x0B,
		    townsPtr->mem);
		if(0<=hostSFTIdx &&
		   hostSFTIdx<FileSys::MAX_NUM_OPEN_FILE &&
		   true==sharedDir[sharedDirIdx].sft[hostSFTIdx].IsOpen())
		{
			auto position=FetchFilePositionFromSFT(townsPtr->cpu.state.ES(),townsPtr->cpu.state.DI());
			sharedDir[sharedDirIdx].Seek(hostSFTIdx,position);

			auto data=sharedDir[sharedDirIdx].sft[hostSFTIdx].Read(townsPtr->cpu.GetCX());
			auto DTAAddr=GetDTAAddress();
			for(auto d : data)
			{
				townsPtr->mem.StoreByte(DTAAddr++,d);
			}
			ReturnCX(data.size());

			townsPtr->cpu.StoreDword(
				townsPtr->mem,
				townsPtr->cpu.state.CS().addressSize,
				townsPtr->cpu.state.ES(),
				townsPtr->cpu.state.DI()+0x15,
				sharedDir[sharedDirIdx].sft[hostSFTIdx].GetFilePointer());

			townsPtr->cpu.SetCF(false);
		}
		else
		{
			// File not open.
			ReturnAX(TOWNS_DOSERR_INVALID_HANDLE);
			townsPtr->cpu.SetCF(true);
		}
		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_1109_WriteToRemoteFile(void)
{
	char drvLetter='A'+FetchDriveCodeFromSFT(townsPtr->cpu.state.ES(),townsPtr->cpu.state.DI());
	auto sharedDirIdx=DriveLetterToSharedDirIndex(drvLetter);
	if(0<=sharedDirIdx)
	{
		auto hostSFTIdx=townsPtr->cpu.FetchWord(
		    townsPtr->cpu.state.CS().addressSize,
		    townsPtr->cpu.state.ES(),
		    townsPtr->cpu.state.DI()+0x0B,
		    townsPtr->mem);
		if(0<=hostSFTIdx &&
		   hostSFTIdx<FileSys::MAX_NUM_OPEN_FILE &&
		   true==sharedDir[sharedDirIdx].sft[hostSFTIdx].IsOpen())
		{
			auto DMABuffer=GetDTAAddress();
			std::vector <unsigned char> data;
			data.resize(townsPtr->cpu.GetCX());
			for(int i=0; i<townsPtr->cpu.GetCX(); ++i)
			{
				data[i]=townsPtr->mem.FetchByte(DMABuffer+i);
			}

			auto position=FetchFilePositionFromSFT(townsPtr->cpu.state.ES(),townsPtr->cpu.state.DI());
			sharedDir[sharedDirIdx].Seek(hostSFTIdx,position);

			auto bytesWritten=sharedDir[sharedDirIdx].sft[hostSFTIdx].Write(data);
			auto filePointer=sharedDir[sharedDirIdx].sft[hostSFTIdx].GetFilePointer();
			townsPtr->cpu.StoreDword(
				townsPtr->mem,
				townsPtr->cpu.state.CS().addressSize,
				townsPtr->cpu.state.ES(),
				townsPtr->cpu.state.DI()+0x15,
				filePointer);

			uint32_t newSize=townsPtr->cpu.FetchDword(
				townsPtr->cpu.state.CS().addressSize,
				townsPtr->cpu.state.ES(),
				townsPtr->cpu.state.DI()+0x11,
				townsPtr->mem);

			if(newSize<filePointer)
			{
				townsPtr->cpu.StoreDword(
					townsPtr->mem,
					townsPtr->cpu.state.CS().addressSize,
					townsPtr->cpu.state.ES(),
					townsPtr->cpu.state.DI()+0x11,
					filePointer);
			}
			ReturnCX(bytesWritten);
			townsPtr->cpu.SetCF(false);
		}
		else
		{
			ReturnAX(TOWNS_DOSERR_INVALID_ACCESS);
			townsPtr->cpu.SetCF(true);
		}
		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_110C_GetDiskInformation(void)
{
	auto CDS=FetchCString(townsPtr->cpu.state.ES(),townsPtr->cpu.state.DI());
	if(CDS.size()<3)
	{
		return false;
	}

	std::cout << CDS << std::endl;
	unsigned char drvLetter=cpputil::Capitalize(CDS[2]);
	auto sharedDirIndex=DriveLetterToSharedDirIndex(drvLetter);

	if(0<=sharedDirIndex) // My drive.
	{
		ReturnAX(0x0001);
		ReturnBX(0xFFFF);
		ReturnCX(0x0200);
		if(true==sharedDir[sharedDirIndex].linked)
		{
			ReturnDX(0xFFFF);
		}
		else
		{
			// No file, no space.
			ReturnDX(0);
		}
		townsPtr->cpu.SetCF(false);
		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_110E_SetFileAttrib(void)
{
	auto fName=GetFilenameBuffer1();
	auto driveLetter=FullyQualifiedFileNameToDriveLetter(fName);
	auto sharedDirIdx=FullyQualifiedFileNameToSharedDirIndex(fName);
	if(0<=sharedDirIdx)
	{
		// Not supported, but pretend to have succeeded.

		auto attr=FetchStackParam0();
		auto subPath=DropDriveLetter(fName);
		std::cout << subPath << std::endl;
		std::cout << cpputil::Ustox(attr) << std::endl;

		townsPtr->cpu.SetCF(false);
		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_110F_GetFileAttrib(void)
{
	auto fName=GetFilenameBuffer1();
	auto driveLetter=FullyQualifiedFileNameToDriveLetter(fName);
	auto sharedDirIdx=FullyQualifiedFileNameToSharedDirIndex(fName);
	if(0<=sharedDirIdx)
	{
		auto subPath=DropDriveLetter(fName);

		auto invalidErr=CheckFileName(fName);
		if(TOWNS_DOSERR_NO_ERROR!=invalidErr)
		{
			ReturnAX(invalidErr);
			townsPtr->cpu.SetCF(true);
			return true; // Yes it's my drive.
		}

		auto dirent=sharedDir[sharedDirIdx].GetFileAttrib(subPath);
		if(true!=dirent.endOfDir)
		{
			ReturnAX(dirent.attr);
			ReturnBX(dirent.length>>16);
			townsPtr->cpu.SetDI(dirent.length&0xFFFF);
			ReturnCX(dirent.FormatDOSTime());
			ReturnDX(dirent.FormatDOSDate());
			townsPtr->cpu.SetCF(false);
		}
		else
		{
			ReturnAX(TOWNS_DOSERR_FILE_NOT_FOUND);
			townsPtr->cpu.SetCF(true);
		}

		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_1111_Rename(void)
{
	auto fn1=GetFilenameBuffer1();
	auto fn2=GetFilenameBuffer2();
	auto driveLetter=FullyQualifiedFileNameToDriveLetter(fn1);
	auto sharedDirIdx=FullyQualifiedFileNameToSharedDirIndex(fn1);
	if(0<=sharedDirIdx)
	{
		townsPtr->cpu.SetCF(true); // Tentative

		std::cout << fn1 << std::endl;
		std::cout << fn2 << std::endl;

		auto fn2DriveLetter=FullyQualifiedFileNameToDriveLetter(fn2);
		if(fn2DriveLetter!=driveLetter)
		{
			ReturnAX(TOWNS_DOSERR_ACCESS_DENIED);
			return true; // Yes, it's my drive.
		}

		fn1=DropDriveLetter(fn1);
		fn2=DropDriveLetter(fn2);

		if(true==sharedDir[sharedDirIdx].RenameSubPath(fn1,fn2))
		{
			townsPtr->cpu.SetCF(false);
		}
		else
		{
			ReturnAX(TOWNS_DOSERR_ACCESS_DENIED);
		}

		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_1113_Delete(void)
{
	auto fName=GetFilenameBuffer1();
	auto driveLetter=FullyQualifiedFileNameToDriveLetter(fName);
	auto sharedDirIdx=FullyQualifiedFileNameToSharedDirIndex(fName);
	if(0<=sharedDirIdx)
	{
		auto invalidErr=CheckFileName(fName);
		if(TOWNS_DOSERR_NO_ERROR!=invalidErr)
		{
			ReturnAX(invalidErr);
			townsPtr->cpu.SetCF(true);
			return true; // Yes it's my drive.
		}

		townsPtr->cpu.SetCF(true);
		ReturnAX(TOWNS_DOSERR_INVALID_ACCESS);

		auto subPath=DropDriveLetter(fName);

		if(true==sharedDir[sharedDirIdx].DeleteSubPathFile(subPath))
		{
			townsPtr->cpu.SetCF(false);
		}

		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_1116_OpenExistingFile(void)
{
	auto fName=GetFilenameBuffer1();
	auto driveLetter=FullyQualifiedFileNameToDriveLetter(fName);
	auto sharedDirIdx=FullyQualifiedFileNameToSharedDirIndex(fName);
	if(0<=sharedDirIdx)
	{
		auto subPath=DropDriveLetter(fName);

		auto invalidErr=CheckFileName(fName);
		if(TOWNS_DOSERR_NO_ERROR!=invalidErr)
		{
			ReturnAX(invalidErr);
			townsPtr->cpu.SetCF(true);
			return true; // Yes it's my drive.
		}

		auto hostSFTIdx=sharedDir[sharedDirIdx].OpenExistingFile(FetchPSP(),subPath,FileSys::OPENMODE_RW);
		if(0<=hostSFTIdx)
		{
			MakeVMSFT(townsPtr->cpu.state.ES(),townsPtr->cpu.state.DI(),driveLetter,hostSFTIdx,sharedDir[sharedDirIdx].sft[hostSFTIdx]);
			townsPtr->cpu.SetCF(false);
		}
		else
		{
			ReturnAX(TOWNS_DOSERR_FILE_NOT_FOUND);
			townsPtr->cpu.SetCF(true);
		}
		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_1117_OpenOrTruncate(void)
{
	auto fName=GetFilenameBuffer1();
	auto driveLetter=FullyQualifiedFileNameToDriveLetter(fName);
	auto sharedDirIdx=FullyQualifiedFileNameToSharedDirIndex(fName);
	if(0<=sharedDirIdx)
	{
		townsPtr->cpu.SetCF(true);

		auto subPath=DropDriveLetter(fName);
		auto mode=FetchStackParam0();

		auto invalidErr=CheckFileName(subPath);
		if(TOWNS_DOSERR_NO_ERROR!=invalidErr)
		{
			ReturnAX(invalidErr);
			townsPtr->cpu.SetCF(true);
			return true; // Yes it's my drive.
		}

		std::cout << cpputil::Ustox(mode) << std::endl;

		if(0==(mode&0xFF00))
		{
			// Normal create
			auto hostSFTIdx=sharedDir[sharedDirIdx].OpenFileNotTruncate(FetchPSP(),subPath,FileSys::OPENMODE_RW);
			if(0<=hostSFTIdx)
			{
				MakeVMSFT(townsPtr->cpu.state.ES(),townsPtr->cpu.state.DI(),driveLetter,hostSFTIdx,sharedDir[sharedDirIdx].sft[hostSFTIdx]);
				townsPtr->cpu.SetCF(false);
			}
		}
		else if(0x0100==(mode&0xFF00))
		{
			// Truncate
			std::cout << "Truncate mode not supported yet." << std::endl;
		}

		return true; // Yes, it's my drive.
	}
	return false; // No, it's not my drive.
}
bool TownsTgDrv::Int2F_111B_FindFirst(void)
{
	auto sAttr=GetSAttr();
	std::string fn=GetFilenameBuffer1();
	auto DTABuffer=GetDTAAddress();
	if(fn.size()<3)
	{
		return false;
	}

	std::cout << fn << std::endl;
	std::cout << cpputil::Ustox(sAttr) << std::endl;
	std::cout << cpputil::Uitox(GetDTAAddress()) << std::endl;

	unsigned char drvLetter=cpputil::Capitalize(fn[2]); // Like \\N.A.
	auto sharedDirIndex=DriveLetterToSharedDirIndex(drvLetter);

	std::cout << sharedDirIndex << std::endl;

	if(0<=sharedDirIndex)
	{
		townsPtr->mem.StoreByte(DTABuffer,0x80|(drvLetter-'A'+1));  // (drv&0x7F) is FCB drive index.
		auto last=GetLastOfFilename(fn);
		std::cout << last << std::endl;
		auto eleven=FilenameTo11Bytes(last);
		std::cout << eleven << std::endl;

		for(int i=0; i<11; ++i)
		{
			townsPtr->mem.StoreByte(DTABuffer+1+i,eleven[i]);
		}
		townsPtr->mem.StoreByte(DTABuffer+0x0C,(unsigned char)sAttr);
		townsPtr->mem.StoreWord(DTABuffer+0x0D,1);  // Entry Count? Always 1?
		// townsPtr->mem.StoreWord(DTABuffer+0x0F,1);  // Cluster Number? Always 1?
		townsPtr->mem.StoreDword(DTABuffer+0x11,0);  // Entry Count? Always 1?

		if(0!=(sAttr&TOWNS_DOS_DIRENT_ATTR_VOLLABEL))
		{
			for(int i=0; i<11; ++i)
			{
				townsPtr->mem.StoreByte(DTABuffer+0x15+i,"TSUGARUDRIV"[i]);
			}
			townsPtr->mem.StoreByte(DTABuffer+0x15+0x0B,TOWNS_DOS_DIRENT_ATTR_VOLLABEL);
			for(int i=0x16; i<0x1C; ++i)
			{
				townsPtr->mem.StoreByte(DTABuffer+0x15+i,0);
			}
		}
		else
		{
			auto subDir=FullPathToSubDir(fn);

			auto invalidErr=CheckFileName(subDir);
			if(TOWNS_DOSERR_NO_ERROR!=invalidErr)
			{
				ReturnAX(invalidErr);
				townsPtr->cpu.SetCF(true);
				return true; // Yes it's my drive.
			}

			bool found=false;
			int fsIdx=-1;
			for(;;)
			{
				FileSys::DirectoryEntry dirent;
				if(fsIdx<0)
				{
					fsIdx=sharedDir[sharedDirIndex].FindFirst(dirent,FetchPSP(),subDir);
					if(fsIdx<0) // No available find struct or file not found.
					{
						break;
					}
				}
				else
				{
					dirent=sharedDir[sharedDirIndex].FindNext(fsIdx);
				}
				if(true==dirent.endOfDir)
				{
					break;
				}

				auto fName11=FilenameTo11Bytes(dirent.fName);
				if(true==FileSys::DOSTemplateMatch(eleven,fName11) &&
				   true==FileSys::DOSAttrMatch(sAttr,dirent.attr))
				{
					MakeDOSDirEnt(DTABuffer+0x15,dirent);
					townsPtr->mem.StoreWord(DTABuffer+0x0F,fsIdx);  // Use Cluster Number to connect fsIdx
					townsPtr->cpu.SetCF(false);
					found=true;
					break;
				}
			}

			if(true!=found)
			{
				// if not found
				ReturnAX(TOWNS_DOSERR_FILE_NOT_FOUND);
				townsPtr->cpu.SetCF(true);
			}
		}
		return true; // Yes, it's mine.
	}
	return false;
}
bool TownsTgDrv::Int2F_111C_FindNext(void)
{
	// ES:DI is CDS for the drive.
	uint32_t DTABuffer=GetDTAAddress();
	unsigned char drv=townsPtr->mem.FetchByte(DTABuffer);
	if(0==(drv&0x80))
	{
		return false;
	}

	char templ11[11];
	for(int i=0; i<11; ++i)
	{
		templ11[i]=townsPtr->mem.FetchByte(DTABuffer+1+i);
	}
	uint16_t sAttr=townsPtr->mem.FetchByte(DTABuffer+0x0C);

	drv=(drv&0x7F)-1+'A'; // (drv&0x7F) is FCB drive index.
	auto sharedDirIndex=DriveLetterToSharedDirIndex(drv);
	if(0<=sharedDirIndex)
	{
		bool found=false;
		int sfIdx=townsPtr->mem.FetchWord(DTABuffer+0x0F);
		if(true==sharedDir[sharedDirIndex].FindStructValid(sfIdx))
		{
			for(;;)
			{
				auto dirent=sharedDir[sharedDirIndex].FindNext(sfIdx);

				auto fName11=FilenameTo11Bytes(dirent.fName);
				if(true==dirent.endOfDir)
				{
					break;
				}
				if(true==FileSys::DOSTemplateMatch(templ11,fName11) &&
				   true==FileSys::DOSAttrMatch(sAttr,dirent.attr))
				{
					MakeDOSDirEnt(DTABuffer+0x15,dirent);
					townsPtr->cpu.SetCF(false);
					found=true;
					break;
				}
			}
			if(true!=found)
			{
				// if not found
				ReturnAX(TOWNS_DOSERR_NO_MORE_FILES);
				townsPtr->cpu.SetCF(true);
			}
		}
		else
		{
			ReturnAX(TOWNS_DOSERR_FILE_NOT_FOUND);
			townsPtr->cpu.SetCF(true);
		}
		return true;
	}
	return false;
}
bool TownsTgDrv::Int2F_111D_CloseAll(void)
{
	// http://www.ctyme.com/intr/rb-4327.htm  This tells PSP should be taken from
	// Is DOS6 CurrentPDB same address?  I just take it from FetchPSP()
	auto PSP=FetchPSP();
	for(auto &fs : sharedDir)
	{
		fs.CloseAllForPSP(PSP);
	}
	return false; // Must continue to the next INT 2F device.  Don't take the ownership.
}
bool TownsTgDrv::Int2F_1123_QualifyRemoteFileName(void)
{
	auto fn=FetchCString(townsPtr->cpu.state.DS(),townsPtr->cpu.state.SI());
	std::cout << fn << std::endl;
	auto sharedDirIndex=FullyQualifiedFileNameToSharedDirIndex(fn);
	if(0<=sharedDirIndex)
	{
		// MSCDEX looks to be ignoring it anyway.
		townsPtr->cpu.SetCF(true);
		return true;
	}
	return false;
}
bool TownsTgDrv::Int2F_1125_RedirectedPrinterMode(void)
{
	return false; // Not my drive.  Not my printer actually.
}

unsigned int TownsTgDrv::CheckFileName(const std::string &fName) const
{
	for(int i=0; i<fName.size(); ++i)
	{
		if('.'==fName[i] && '.'==fName[i+1])
		{
			return TOWNS_DOSERR_ACCESS_DENIED;
		}
		if(fName[i]<' ' || 0x80<=fName[i])
		{
			return TOWNS_DOSERR_ACCESS_DENIED;
		}
	}
	return TOWNS_DOSERR_NO_ERROR;
}

uint16_t TownsTgDrv::FetchPSP(void) const
{
	return townsPtr->cpu.FetchWord(
	            townsPtr->cpu.state.SS().addressSize,
	            townsPtr->cpu.state.SS(),
	            townsPtr->cpu.state.SP()+8,
	            townsPtr->mem);
}
uint16_t TownsTgDrv::FetchStackParam0(void) const
{
	return townsPtr->cpu.FetchWord(
	            townsPtr->cpu.state.SS().addressSize,
	            townsPtr->cpu.state.SS(),
	            townsPtr->cpu.state.SP()+10,
	            townsPtr->mem);
}
uint16_t TownsTgDrv::FetchStackParam1(void) const
{
	return townsPtr->cpu.FetchWord(
	            townsPtr->cpu.state.SS().addressSize,
	            townsPtr->cpu.state.SS(),
	            townsPtr->cpu.state.SP()+12,
	            townsPtr->mem);
}
uint16_t TownsTgDrv::FetchStackParam2(void) const
{
	return townsPtr->cpu.FetchWord(
	            townsPtr->cpu.state.SS().addressSize,
	            townsPtr->cpu.state.SS(),
	            townsPtr->cpu.state.SP()+14,
	            townsPtr->mem);
}
uint16_t TownsTgDrv::FetchStackParam3(void) const
{
	return townsPtr->cpu.FetchWord(
	            townsPtr->cpu.state.SS().addressSize,
	            townsPtr->cpu.state.SS(),
	            townsPtr->cpu.state.SP()+16,
	            townsPtr->mem);
}
void TownsTgDrv::ReturnAX(uint16_t ax)
{
	townsPtr->cpu.StoreWord(
	    townsPtr->mem,
	    townsPtr->cpu.state.SS().addressSize,
	    townsPtr->cpu.state.SS(),
	    townsPtr->cpu.state.SP(),
	    ax);
}
void TownsTgDrv::ReturnBX(uint16_t bx)
{
	townsPtr->cpu.StoreWord(
	    townsPtr->mem,
	    townsPtr->cpu.state.SS().addressSize,
	    townsPtr->cpu.state.SS(),
	    townsPtr->cpu.state.SP()+2,
	    bx);
}
void TownsTgDrv::ReturnCX(uint16_t cx)
{
	townsPtr->cpu.StoreWord(
	    townsPtr->mem,
	    townsPtr->cpu.state.SS().addressSize,
	    townsPtr->cpu.state.SS(),
	    townsPtr->cpu.state.SP()+4,
	    cx);
}
void TownsTgDrv::ReturnDX(uint16_t dx)
{
	townsPtr->cpu.StoreWord(
	    townsPtr->mem,
	    townsPtr->cpu.state.SS().addressSize,
	    townsPtr->cpu.state.SS(),
	    townsPtr->cpu.state.SP()+6,
	    dx);
}

int TownsTgDrv::FullyQualifiedFileNameToSharedDirIndex(const std::string &fn) const
{
	if(2<=fn.size() && ':'==fn[1])
	{
		return DriveLetterToSharedDirIndex(cpputil::Capitalize(fn[0]));
	}
	else if(('/'==fn[0] && '/'==fn[1]) || ('\\'==fn[0] && '\\'==fn[1]))
	{
		return DriveLetterToSharedDirIndex(cpputil::Capitalize(fn[2]));
	}
	return -1;
}

char TownsTgDrv::FullyQualifiedFileNameToDriveLetter(const std::string &fn) const
{
	if(2<=fn.size() && ':'==fn[1])
	{
		return cpputil::Capitalize(fn[0]);
	}
	else if(('/'==fn[0] && '/'==fn[1]) || ('\\'==fn[0] && '\\'==fn[1]))
	{
		return cpputil::Capitalize(fn[2]);
	}
	return -1;
}

int TownsTgDrv::DriveLetterToSharedDirIndex(char letter) const
{
	for(int i=0; i<TOWNS_TGDRV_MAX_NUM_DRIVES; ++i)
	{
		if(letter==townsPtr->cpu.FetchByte(
			townsPtr->cpu.state.CS().addressSize,
			townsPtr->cpu.state.CS(),
			DRIVELETTER_BUFFER+i,
			townsPtr->mem))
		{
			return i;
		}
	}
	return -1;
}
void TownsTgDrv::MakeVMSFT(const class i486DX::SegmentRegister &seg,uint32_t offset,char driveLetter,int hostSFTIdx,FileSys::SystemFileTable &hostSFT)
{
	auto &cpu=townsPtr->cpu;
	auto &mem=townsPtr->mem;
	auto &CS=cpu.state.CS();
	cpu.StoreWord(mem,CS.addressSize,seg,offset+0x00,1); // Ref Count
	cpu.StoreWord(mem,CS.addressSize,seg,offset+0x02,hostSFT.mode);
	cpu.StoreByte(mem,CS.addressSize,seg,offset+0x04,hostSFT.attr);

	uint16_t devInfo;
	devInfo=cpputil::Capitalize(driveLetter)-'A';
	devInfo|=0x8000; // Redirected.
	cpu.StoreWord(mem,CS.addressSize,seg,offset+0x05,devInfo);

	cpu.StoreDword(mem,CS.addressSize,seg,offset+0x07,TGDRV_ID); // Redirected. No DPB.  Write Tsugaru Drive ID.
	cpu.StoreWord(mem,CS.addressSize,seg,offset+0x0B,hostSFTIdx); // Use this word to connect with host.

	cpu.StoreWord(mem,CS.addressSize,seg,offset+0x0D,hostSFT.FormatDOSTime());
	cpu.StoreWord(mem,CS.addressSize,seg,offset+0x0F,hostSFT.FormatDOSDate());
	cpu.StoreDword(mem,CS.addressSize,seg,offset+0x11,hostSFT.GetFileSize());
	cpu.StoreDword(mem,CS.addressSize,seg,offset+0x15,hostSFT.GetFilePointer());

	cpu.StoreWord(mem,CS.addressSize,seg,offset+0x19,0); // Rel cluster.  N/A.
	cpu.StoreWord(mem,CS.addressSize,seg,offset+0x1B,0); // Absolute cluster.  N/A.
	cpu.StoreWord(mem,CS.addressSize,seg,offset+0x1D,0); // Dir ent sector N/A
	cpu.StoreWord(mem,CS.addressSize,seg,offset+0x1F,0); // Dir ent position in sector N/A

	auto eleven=FilenameTo11Bytes(cpputil::GetBaseName(hostSFT.fName));
	for(int i=0; i<11; ++i)
	{
		cpu.StoreByte(mem,CS.addressSize,seg,offset+0x20+i,eleven[i]);
	}

	cpu.StoreByte(mem,CS.addressSize,seg,offset+0x31,FetchPSP());
}
void TownsTgDrv::MakeDOSDirEnt(uint32_t DTABuffer,const FileSys::DirectoryEntry &dirent)
{
	std::string fName11;
	if("."==dirent.fName || ".."==dirent.fName)
	{
		fName11=dirent.fName;
		while(fName11.size()<11)
		{
			fName11.push_back(' ');
		}
	}
	else
	{
		fName11=FilenameTo11Bytes(dirent.fName);
	}
	for(int i=0; i<11; ++i)
	{
		townsPtr->mem.StoreByte(DTABuffer+i,fName11[i]);
	}
	townsPtr->mem.StoreByte(DTABuffer+0x0B,dirent.attr);
	townsPtr->mem.StoreByte(DTABuffer+0x0C,0);

	townsPtr->mem.StoreWord(DTABuffer+0x16,dirent.FormatDOSTime());
	townsPtr->mem.StoreWord(DTABuffer+0x18,dirent.FormatDOSDate());
	townsPtr->mem.StoreWord(DTABuffer+0x1A,0); // First cluster N/A for Network file
	townsPtr->mem.StoreDword(DTABuffer+0x1C,(uint32_t)dirent.length);
}
unsigned int TownsTgDrv::FetchDriveCodeFromSFT(const class i486DX::SegmentRegister &seg,uint32_t offset) const
{
	unsigned int flags=FetchDeviceInfoFromSFT(seg,offset);
	return flags&0x1F;
}
uint32_t TownsTgDrv::FetchFilePositionFromSFT(const class i486DX::SegmentRegister &seg,uint32_t offset) const
{
	return townsPtr->cpu.FetchDword(
		townsPtr->cpu.state.CS().addressSize,
		seg,
		offset+0x15,
		townsPtr->mem);
}
unsigned int TownsTgDrv::FetchDeviceInfoFromSFT(const class i486DX::SegmentRegister &seg,uint32_t offset) const
{
	return townsPtr->cpu.FetchWord(
		townsPtr->cpu.state.CS().addressSize,
		seg,
		offset+0x05,
		townsPtr->mem);
}
std::string TownsTgDrv::FetchCString(uint32_t physAddr) const
{
	std::string str;
	for(;;)
	{
		auto c=townsPtr->mem.FetchByte(physAddr++);
		if(0==c)
		{
			break;
		}
		str.push_back(c);
	}
	return str;
}
std::string TownsTgDrv::FetchCString(const i486DX::SegmentRegister &seg,uint32_t offset) const
{
	std::string str;
	for(;;)
	{
		auto c=townsPtr->cpu.FetchByte(
		    townsPtr->cpu.state.CS().addressSize,
		    seg,
		    offset++,
			townsPtr->mem);
		if(0==c)
		{
			break;
		}
		str.push_back(c);
	}
	return str;
}

bool TownsTgDrv::Install(void)
{
	auto &cpu=townsPtr->cpu;
	auto &mem=townsPtr->mem;

	std::cout << "AX=" << cpputil::Ustox(cpu.GetAX()) << std::endl;
	std::cout << "BX=" << cpputil::Ustox(cpu.GetBX()) << std::endl;
	std::cout << "CX=" << cpputil::Ustox(cpu.GetCX()) << std::endl;
	std::cout << "DX=" << cpputil::Ustox(cpu.GetDX()) << std::endl;
	std::cout << "SI=" << cpputil::Ustox(cpu.state.SI()) << std::endl;
	std::cout << "DI=" << cpputil::Ustox(cpu.state.DI()) << std::endl;

	// CS:0080  Length of command parameter
	// CS:0081- Command parameter
	std::string param;
	int len=cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.CS(),0x0080,mem);
	for(int i=0; i<len; ++i)
	{
		param.push_back(cpu.FetchByte(cpu.state.CS().addressSize,cpu.state.CS(),0x0081+i,mem));
	}
	std::cout << "{" << param << "}" << std::endl;


	auto CDSCount=GetCDSCount();
	std::cout << "CDS Count=" << CDSCount << std::endl;

	for(int i=0; i<CDSCount; ++i)
	{
		std::cout << 'A'+i << " ";
		std::cout << cpputil::Ustox(GetCDSType(i)) << "h " << std::endl;
	}

	std::vector <char> drives;
	for(int i=0; i+3<param.size(); ++i)
	{
		if('/'==param[i] && ('D'==param[i+1] || 'd'==param[i+1]) && ':'==param[i+2])
		{
			unsigned int d=DriveLetterToDriveIndex(param[i+3]);
			if(d<=CDSCount && 0==GetCDSType(d))  // Check drive is unused.
			{
				drives.push_back(cpputil::Capitalize(param[i+3]));
			}
		}
		else if('/'==param[i] && ('F'==param[i+1] || 'f'==param[i+1]) && ':'==param[i+2])
		{
			unsigned int d=DriveLetterToDriveIndex(param[i+3]);
			if(d<=CDSCount)  // Don't check drive is unused.
			{
				drives.push_back(cpputil::Capitalize(param[i+3]));
			}
		}
	}
	if(0==drives.size())
	{
		unsigned int driveIndex=0;
		for(auto &fs : sharedDir)
		{
			if(true==fs.linked)
			{
				while(driveIndex<CDSCount)
				{
					if(0==GetCDSType(driveIndex))
					{
						drives.push_back(DriveIndexToDriveLetter(driveIndex));
						++driveIndex;
						break;
					}
					++driveIndex;
				}
			}
			if(CDSCount<=driveIndex)
			{
				break;
			}
		}
	}
	if(0<drives.size())
	{
		int I=0;
		for(auto letter : drives)
		{
			if(TOWNS_TGDRV_MAX_NUM_DRIVES<=I)
			{
				break;
			}

			cpu.StoreByte(
			    mem,
			    cpu.state.CS().addressSize,
			    cpu.state.DS(),
			    DRIVELETTER_BUFFER+I,
			    letter);

			char str[2]={letter,0};
			std::cout << "Assign Drive " << str << std::endl;

			auto CDSAddr=GetCDSAddress(DriveLetterToDriveIndex(letter));
			for(int i=0; i<GetCDSLength(); ++i)
			{
				mem.StoreByte(CDSAddr+i,0);
			}
			mem.StoreByte(CDSAddr  ,'\\');
			mem.StoreByte(CDSAddr+1,'\\');
			mem.StoreByte(CDSAddr+2,letter);
			mem.StoreByte(CDSAddr+3,'.');
			mem.StoreByte(CDSAddr+4,'A');
			mem.StoreByte(CDSAddr+5,'.');
			mem.StoreWord(CDSAddr+0x43,0xC000);
			mem.StoreDword(CDSAddr+0x45,TGDRV_ID); // Put "TGDR" instead of DPB pointer.
			mem.StoreWord(CDSAddr+0x4F,6); // Length for "\\P.A."

			++I;
		}
		cpu.StoreByte(
		    mem,
		    cpu.state.CS().addressSize,
		    cpu.state.DS(),
		    0x108,
		    I);
		if(0<I)
		{
			return true;
		}
	}
	return false;
}

unsigned int TownsTgDrv::DriveLetterToDriveIndex(char drvLetter) const
{
	if('a'<=drvLetter && drvLetter<='z')
	{
		return drvLetter-'a';
	}
	else
	{
		return drvLetter-'A';
	}
}
char TownsTgDrv::DriveIndexToDriveLetter(unsigned int driveIndex) const
{
	return 'A'+driveIndex;
}
unsigned int TownsTgDrv::GetCDSCount(void) const
{
	auto &mem=townsPtr->mem;
	auto addr=townsPtr->state.DOSLOLSEG*0x10+TOWNS_DOS_CDS_COUNT;
	return mem.FetchByte(addr);
}

unsigned int TownsTgDrv::GetCDSLength(void) const
{
	auto dosverMajor=townsPtr->state.DOSVER&0xFF;
	if(4<=dosverMajor)
	{
		return 0x58;
	}
	else
	{
		return 0x51;
	}
}

std::string TownsTgDrv::GetFilenameBuffer1(void) const
{
	auto addr=GetFilenameBufferAddress();
	std::string fn;
	for(;;)
	{
		auto c=townsPtr->mem.FetchByte(addr++);
		if(0==c)
		{
			break;
		}
		fn.push_back(c);
	}
	return fn;
}
std::string TownsTgDrv::GetFilenameBuffer2(void) const
{
	auto addr=GetFilenameBuffer2Address();
	std::string fn;
	for(;;)
	{
		auto c=townsPtr->mem.FetchByte(addr++);
		if(0==c)
		{
			break;
		}
		fn.push_back(c);
	}
	return fn;
}
std::string TownsTgDrv::GetLastOfFilename(std::string in) const
{
	int lastSlash=0;
	for(int i=0; i<in.size(); ++i)
	{
		if('/'==in[i] || '\\'==in[i])
		{
			lastSlash=i+1;
		}
	}
	std::string last;
	for(int i=lastSlash; i<in.size(); ++i)
	{
		last.push_back(in[i]);
	}
	return last;
}
std::string TownsTgDrv::FilenameTo11Bytes(std::string in) const
{
	int ptr=0;
	std::string eleven;
	while(ptr<in.size() && eleven.size()<8 && '.'!=in[ptr])
	{
		eleven.push_back(in[ptr++]);
	}
	while(eleven.size()<8)
	{
		eleven.push_back(' ');
	}

	while('.'!=in[ptr] && ptr<in.size())
	{
		++ptr;
	}

	if('.'==in[ptr])
	{
		++ptr;
		while(ptr<in.size() && eleven.size()<11)
		{
			eleven.push_back(in[ptr++]);
		}
	}
	while(eleven.size()<11)
	{
		eleven.push_back(' ');
	}
	return eleven;
}
std::string TownsTgDrv::FullPathToSubDir(std::string fn) const
{
	std::string subdir;
	int i0=0;
	if(('/'==fn[0] && '/'==fn[1]) || ('\\'==fn[0] && '\\'==fn[1]))
	{
		// \\Q.A. format
		i0=6;
	}
	else if(0!=fn[0] && ':'==fn[1])
	{
		// Q: format
		i0=2;
	}
	// Assume after last slash is find template.
	int lastSlash=0;
	for(int i=i0; i<fn.size(); ++i)
	{
		if('/'==fn[i] || '\\'==fn[i])
		{
			lastSlash=subdir.size();
		}
		subdir.push_back(fn[i]);
	}
	subdir.resize(lastSlash);
	return subdir;
}
std::string TownsTgDrv::DropDriveLetter(std::string ful) const
{
	if(('/'==ful[0] && '/'==ful[1]) || ('\\'==ful[0] && '\\'==ful[1]))
	{
		// \\Q.A. format
		if('/'==ful[6] || '\\'==ful[6])
		{
			return ful.c_str()+7;
		}
		return ful.c_str()+6;
	}
	else if(0!=ful[0] && ':'==ful[1])
	{
		// Q: format
		if('/'==ful[2] || '\\'==ful[2])
		{
			return ful.c_str()+3;
		}
		return ful.c_str()+2;
	}
	return ful;
}
uint32_t TownsTgDrv::GetCDSAddress(unsigned int driveIndex) const
{
	auto &mem=townsPtr->mem;
	auto DOSADDR=townsPtr->state.DOSLOLSEG*0x10;
	auto ofs=mem.FetchWord(DOSADDR+TOWNS_DOS_CDS_LIST_PTR);
	auto seg=mem.FetchWord(DOSADDR+TOWNS_DOS_CDS_LIST_PTR+2);
	return seg*0x10+ofs+GetCDSLength()*driveIndex;
}

uint16_t TownsTgDrv::GetCDSType(unsigned int driveIndex) const
{
	auto addr=GetCDSAddress(driveIndex);
	return townsPtr->mem.FetchWord(addr+0x43);
}
uint32_t TownsTgDrv::GetDTAAddress(void) const
{
	auto dosverMajor=townsPtr->state.DOSVER&0xFF;
	uint64_t DTAPointer;
	if(dosverMajor<4)
	{
		DTAPointer=townsPtr->state.DOSLOLSEG*0x10+0x2DA;
	}
	else
	{
		DTAPointer=townsPtr->state.DOSLOLSEG*0x10+0x32C;
	}
	uint32_t off=townsPtr->mem.FetchWord(DTAPointer);
	uint32_t seg=townsPtr->mem.FetchWord(DTAPointer+2);
	return seg*0x10+off;
}
uint32_t TownsTgDrv::GetFilenameBufferAddress(void) const
{
	auto dosverMajor=townsPtr->state.DOSVER&0xFF;
	if(dosverMajor<4)
	{
		return townsPtr->state.DOSLOLSEG*0x10+0x360;
	}
	else
	{
		return townsPtr->state.DOSLOLSEG*0x10+0x3BE;
	}
}
uint32_t TownsTgDrv::GetFilenameBuffer2Address(void) const
{
	return GetFilenameBufferAddress()+0x80; // Need to check if it is same in DOS6.
}
uint32_t TownsTgDrv::GetSDBAddress(void) const
{
	auto dosverMajor=townsPtr->state.DOSVER&0xFF;
	if(dosverMajor<4)
	{
		return townsPtr->state.DOSLOLSEG*0x10+0x460;
	}
	else
	{
		return townsPtr->state.DOSLOLSEG*0x10+0x4BE;
	}
}
uint16_t TownsTgDrv::GetSAttr(void) const
{
	auto addr=GetSAttrAddress();
	return townsPtr->mem.FetchWord(addr);
}
uint32_t TownsTgDrv::GetSAttrAddress(void) const
{
	auto dosverMajor=townsPtr->state.DOSVER&0xFF;
	if(dosverMajor<4)
	{
		return townsPtr->state.DOSLOLSEG*0x10+0x508;
	}
	else
	{
		return townsPtr->state.DOSLOLSEG*0x10+0x56D;
	}
}



/* virtual */ uint32_t TownsTgDrv::SerializeVersion(void) const
{
	return 0;
}
/* virtual */ void TownsTgDrv::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
}
/* virtual */ bool TownsTgDrv::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	return true;
}
