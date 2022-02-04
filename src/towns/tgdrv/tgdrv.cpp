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
			Install();
			townsPtr->cpu.SetCF(false);
			break;
		case TOWNS_VM_TGDRV_INT2FH://      0x02,
			std::cout << "INT 2FH Intercept." << std::endl;
			// Set PF if not my drive.
			// Clear PF if my drive.
			townsPtr->cpu.SetPF(true);
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

void TownsTgDrv::Install(void)
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
	}
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
