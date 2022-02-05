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
	for(auto &letter : driveLetters)
	{
		letter=0;
	}
}
void TownsTgDrv::State::PowerOn(void)
{
	for(auto &letter : driveLetters)
	{
		letter=0;
	}
}
void TownsTgDrv::State::Reset(void)
{
	for(auto &letter : driveLetters)
	{
		letter=0;
	}
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
			case 0x111b:
				myDrive=Int2F_111B_FindFirst();
				break;
			case 0x111c:
				myDrive=Int2F_111C_FindNext();
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
	switch(ioport)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		break;
	}
	return 0xff;
}


bool TownsTgDrv::Int2F_111B_FindFirst(void)
{
	auto sAttr=GetSAttr();
	std::string fn=GetFilenameBuffer1();
	auto DTABuffer=GetDTAAddress();
	std::cout << fn << std::endl;
	std::cout << cpputil::Ustox(sAttr) << std::endl;
	std::cout << cpputil::Uitox(GetDTAAddress()) << std::endl;

	unsigned char drvLetter=cpputil::Capitalize(fn[2]); // Like \\N.A.
	auto sharedDirIndex=DriveLetterToSharedDirIndex(drvLetter);

	std::cout << sharedDirIndex << std::endl;

	if(0<=sharedDirIndex)
	{
		townsPtr->mem.StoreByte(DTABuffer,0x80|drvLetter);
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
		townsPtr->mem.StoreWord(DTABuffer+0x0F,1);  // Cluster Number? Always 1?
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
			dirent[sharedDirIndex]=sharedDir[sharedDirIndex].FindFirst(subDir);
			if(true==dirent[sharedDirIndex].endOfDir)
			{
				// if not found
				ReturnAX(TOWNS_DOSERR_FILE_NOT_FOUND);
				townsPtr->cpu.SetCF(true);
			}
			else
			{
				MakeDOSDirEnt(DTABuffer+0x15,dirent[sharedDirIndex]);
				townsPtr->cpu.SetCF(false);
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
	drv&=0x7F;
	auto sharedDirIndex=DriveLetterToSharedDirIndex(drv);
	if(0<=sharedDirIndex)
	{
		if(true==dirent[sharedDirIndex].endOfDir)
		{
			ReturnAX(TOWNS_DOSERR_FILE_NOT_FOUND);
			townsPtr->cpu.SetCF(true);
		}
		else
		{
			dirent[sharedDirIndex]=sharedDir[sharedDirIndex].FindNext();
			if(true==dirent[sharedDirIndex].endOfDir)
			{
				// if not found
				ReturnAX(TOWNS_DOSERR_FILE_NOT_FOUND);
				townsPtr->cpu.SetCF(true);
			}
			else
			{
				MakeDOSDirEnt(DTABuffer+0x15,dirent[sharedDirIndex]);
				townsPtr->cpu.SetCF(false);
			}
		}
		return true;
	}
	return false;
}
bool TownsTgDrv::Int2F_1123_QualifyRemoteFileName(void)
{
	auto physAddr=townsPtr->cpu.state.DS().baseLinearAddr+townsPtr->cpu.state.SI();
	auto fn=FetchCString(physAddr);
	std::cout << fn << std::endl;
	return false;
}
bool TownsTgDrv::Int2F_1125_RedirectedPrinterMode(void)
{
	return false; // Not my drive.  Not my printer actually.
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

int TownsTgDrv::DriveLetterToSharedDirIndex(char letter) const
{
	for(int i=0; i<TOWNS_TGDRV_MAX_NUM_DRIVES; ++i)
	{
		if(letter==state.driveLetters[i])
		{
			return i;
		}
	}
	return -1;
}
void TownsTgDrv::MakeDOSDirEnt(uint32_t DTABuffer,const FileSys::DirectoryEntry &dirent)
{
	auto eleven=FilenameTo11Bytes(dirent.fName);
	for(int i=0; i<11; ++i)
	{
		townsPtr->mem.StoreByte(DTABuffer+i,eleven[i]);
	}
	townsPtr->mem.StoreByte(DTABuffer+0x0B,dirent.attr);
	townsPtr->mem.StoreByte(DTABuffer+0x0C,0);

	uint16_t time=(dirent.hours<<11)|(dirent.minutes<<5)|(dirent.seconds/2);
	uint16_t date=((dirent.year-1980)<<9)|(dirent.month<<5)|(dirent.day);

	townsPtr->mem.StoreWord(DTABuffer+0x16,time);
	townsPtr->mem.StoreWord(DTABuffer+0x18,date);
	townsPtr->mem.StoreWord(DTABuffer+0x1A,0); // First cluster N/A for Network file
	townsPtr->mem.StoreDword(DTABuffer+0x1C,(uint32_t)dirent.length);
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

bool TownsTgDrv::Install(void)
{
	auto &cpu=townsPtr->cpu;
	auto &mem=townsPtr->mem;

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
			if(d<=CDSCount && 0==GetCDSType(d))  // Drive is unused.
			{
				drives.push_back(param[i+3]);
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
			    0x109+I,
			    letter);

			state.driveLetters[I]=letter;

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
	else if(':'==fn[1])
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
