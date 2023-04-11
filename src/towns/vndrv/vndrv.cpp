/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>

#include "device.h"
#include "townsdef.h"
#include "cpputil.h"
#include "miscutil.h"



#include "towns.h"


void TownsVnDrv::State::PowerOn(void)
{
}
void TownsVnDrv::State::Reset(void)
{
	lastAPICheckWrite=0xffff;
	enabled=false;
}

////////////////////////////////////////////////////////////

TownsVnDrv::TownsVnDrv(class FMTownsCommon *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
}

FileSys *TownsVnDrv::GetSharedDir(unsigned int drvNum)
{
	for(unsigned int i=0; i<MAX_NUM_SHARED_DIRECTORIES; ++i)
	{
		auto &dir=sharedDir[i];
		if(true==dir.linked)
		{
			if(0==drvNum)
			{
				return &dir;
			}
			else
			{
				--drvNum;
			}
		}
	}
	return nullptr;
}
const FileSys *TownsVnDrv::GetSharedDir(unsigned int drvNum) const
{
	for(unsigned int i=0; i<MAX_NUM_SHARED_DIRECTORIES; ++i)
	{
		auto &dir=sharedDir[i];
		if(true==dir.linked)
		{
			if(0==drvNum)
			{
				return &dir;
			}
			else
			{
				--drvNum;
			}
		}
	}
	return nullptr;
}

const unsigned int TownsVnDrv::NumDrives(void) const
{
	unsigned int n=0;
	for(auto &dir : sharedDir)
	{
		if(true==dir.linked)
		{
			++n;
		}
	}
	return n;
}

/* virtual */ void TownsVnDrv::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsVnDrv::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsVnDrv::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_VNDRV_APICHECK://       0x2F10,
		state.lastAPICheckWrite&=0xFF00;
		state.lastAPICheckWrite|=(data&0xFF);
		break;
	case TOWNSIO_VNDRV_APICHECK+1://       0x2F10,
		state.lastAPICheckWrite&=0x00FF;
		state.lastAPICheckWrite|=((data&0xFF)<<8);
		break;
	case TOWNSIO_VNDRV_ENABLE://         0x2F12,
		if(ENABLE_CODE==data)
		{
			state.enabled=true;
		}
		else if(DISABLE_CODE==data)
		{
			state.enabled=false;
		}
		break;
	case TOWNSIO_VNDRV_COMMAND://        0x2F14,
		// Word-access only
		break;
	case TOWNSIO_VNDRV_AUXCOMMAND://     0x2F18,
		ExecAuxCommand(data);
		break;
	}
}
/* virtual */ void TownsVnDrv::IOWriteWord(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_VNDRV_APICHECK://       0x2F10,
		state.lastAPICheckWrite=data;
		break;
	case TOWNSIO_VNDRV_ENABLE://         0x2F12,
		if(ENABLE_CODE==data)
		{
			state.enabled=true;
		}
		else if(DISABLE_CODE==data)
		{
			state.enabled=false;
		}
		break;
	case TOWNSIO_VNDRV_COMMAND://        0x2F14,
		ExecPrimaryCommand(data);
		break;
	case TOWNSIO_VNDRV_AUXCOMMAND://     0x2F18,
		ExecAuxCommand(data);
		break;
	}
}
/* virtual */ unsigned int TownsVnDrv::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_VNDRV_APICHECK://       0x2F10,
		return (~state.lastAPICheckWrite)&0xFF;
	case TOWNSIO_VNDRV_APICHECK+1://       0x2F10,
		return (~state.lastAPICheckWrite>>8)&0xFF;
	case TOWNSIO_VNDRV_ENABLE://         0x2F12,
		return (state.enabled ? ENABLE_CODE : DISABLE_CODE);
	case TOWNSIO_VNDRV_COMMAND://        0x2F14,
		break;
	case TOWNSIO_VNDRV_AUXCOMMAND://     0x2F18,
		break;
	}
	return 0xff;
}
/* virtual */ unsigned int TownsVnDrv::IOReadWord(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_VNDRV_APICHECK://       0x2F10,
		return ~state.lastAPICheckWrite;
	case TOWNSIO_VNDRV_ENABLE://         0x2F12,
		return (state.enabled ? ENABLE_CODE : DISABLE_CODE);
	case TOWNSIO_VNDRV_COMMAND://        0x2F14,
		break;
	case TOWNSIO_VNDRV_AUXCOMMAND://     0x2F18,
		break;
	}
	return 0xffff;
}
void TownsVnDrv::ExecPrimaryCommand(unsigned int cmd)
{
	if(true==state.enabled)
	{
		switch((cmd>>8)&0xFF)
		{
		case TOWNS_VNDRV_CMD_GET_DRIVES://   0x00,
			townsPtr->CPU().SetAL(NumDrives());
			break;
		case TOWNS_VNDRV_CMD_FIND_FIRST://   0x1B,
			{
				auto drvNum=(cmd&0xFF);
				std::cout << "VNDRV Request FIND_FIRST " << drvNum << std::endl;
				auto drvPtr=GetSharedDir(drvNum);
				if(nullptr==drvPtr)
				{
					townsPtr->CPU().SetAX(TOWNS_VNDRV_ERR_PATH_NOT_FOUND);
					std::cout << "Path doesn't exist." << std::endl;
					break;
				}

				std::string subDir;
				auto ESI=townsPtr->CPU().state.ESI();
				for(;;)
				{
					auto c=townsPtr->CPU().DebugFetchByte(32,townsPtr->CPU().state.FS(),ESI++,townsPtr->mem);
					if(0==c)
					{
						break;
					}
					subDir.push_back(c);
				}
				{
					// For better security, I should simplify subDir and make sure it doesn't go outside of the hostPath.
					cpputil::SimplifyPath(subDir);
					bool violation=false;
					for(int i=0; i+1<subDir.size(); ++i)
					{
						if(subDir[i]=='.' && subDir[i+1]=='.') // Don't allow ".." to be in the path.
						{
							townsPtr->CPU().SetAX(TOWNS_VNDRV_ERR_PATH_NOT_FOUND);
							violation=true;
							break;
						}
					}
					if(true==violation)
					{
						std::cout << "Path violation." << subDir << std::endl;
						break;
					}
				}

				townsPtr->CPU().SetAX(TOWNS_VNDRV_ERR_FILE_NOT_FOUND);
				// auto dirEnt=drvPtr->FindFirst(subDir);
				// if(true==dirEnt.endOfDir)
				// {
				// 	townsPtr->CPU().SetAX(TOWNS_VNDRV_ERR_FILE_NOT_FOUND);
				// 	std::cout << "File not found" << std::endl;
				// 	break;
				// }
				// townsPtr->CPU().SetAX(TOWNS_VNDRV_ERR_NO_ERROR);
				// StoreDirEnt(dirEnt);
			}
			break;
		case TOWNS_VNDRV_CMD_FIND_NEXT://    0x1C,
			{
				auto drvNum=(cmd&0xFF);
				std::cout << "VNDRV Request FIND_NEXT " << std::endl;
				auto drvPtr=GetSharedDir(drvNum);
				if(nullptr==drvPtr)
				{
					townsPtr->CPU().SetAX(TOWNS_VNDRV_ERR_PATH_NOT_FOUND);
					std::cout << "Path doesn't exist." << std::endl;
					break;
				}

				townsPtr->CPU().SetAX(TOWNS_VNDRV_ERR_NO_MORE_FILES);
				// auto dirEnt=drvPtr->FindNext();
				// if(true==dirEnt.endOfDir)
				// {
				// 	townsPtr->CPU().SetAX(TOWNS_VNDRV_ERR_NO_MORE_FILES);
				// 	std::cout << "File not found" << std::endl;
				// 	break;
				// }
				// townsPtr->CPU().SetAX(TOWNS_VNDRV_ERR_NO_ERROR);
				// StoreDirEnt(dirEnt);
			}
			break;
		}
	}
}
void TownsVnDrv::ExecAuxCommand(unsigned int cmd)
{
	if(true==state.enabled)
	{
		switch(cmd)
		{
		case TOWNS_VNDRV_AUXCMD_DEBUGBREAK://0x00,
			townsPtr->debugger.ExternalBreak("Break from VNDRV");
			break;
		case TOWNS_VNDRV_AUXCMD_PRINTCSTR:// 0x09,
			{
				unsigned int i=0;
				std::string str;
				for(;;)
				{
					auto data=townsPtr->CPU().DebugFetchByte(32,townsPtr->CPU().state.DS(),townsPtr->CPU().state.EBX()+i,townsPtr->mem);
					if(0==data)
					{
						break;
					}
					str.push_back(data);
					++i;
				}
				std::cout << str << std::endl; // Should I line-break?
			}
			break;
		case TOWNS_VNDRV_AUXCMD_MEMDUMP://   0x0A,
			{
				i486DXCommon::FarPointer ptr;
				ptr.SEG=townsPtr->CPU().state.DS().value;
				ptr.OFFSET=townsPtr->CPU().state.EBX();
				for(auto line : miscutil::MakeMemDump(townsPtr->CPU(),townsPtr->mem,ptr,townsPtr->CPU().state.ECX(),/*shiftJIS=*/true))
				{
					std::cout << line << std::endl;
				}
			}
			break;
		case TOWNS_VNDRV_AUXCMD_MEMDUMP_LINEAR://     0x0B,
			{
				i486DXCommon::FarPointer ptr;
				ptr.SEG=i486DXCommon::FarPointer::LINEAR_ADDR;
				ptr.OFFSET=townsPtr->CPU().state.EBX();
				for(auto line : miscutil::MakeMemDump(townsPtr->CPU(),townsPtr->mem,ptr,townsPtr->CPU().state.ECX(),/*shiftJIS=*/true))
				{
					std::cout << line << std::endl;
				}
			}
			break;
		case TOWNS_VNDRV_AUXCMD_MEMDUMP_PHYSICAL://   0x0C,
			{
				i486DXCommon::FarPointer ptr;
				ptr.SEG=i486DXCommon::FarPointer::PHYS_ADDR;
				ptr.OFFSET=townsPtr->CPU().state.EBX();
				for(auto line : miscutil::MakeMemDump(townsPtr->CPU(),townsPtr->mem,ptr,townsPtr->CPU().state.ECX(),/*shiftJIS=*/true))
				{
					std::cout << line << std::endl;
				}
			}
			break;
		}
	}
}

void TownsVnDrv::StoreDirEnt(const FileSys::DirectoryEntry &dirEnt)
{
	auto &GS=townsPtr->CPU().state.GS();
	auto EDI=townsPtr->CPU().state.EDI();
	townsPtr->CPU().DebugStoreByte(townsPtr->mem,32,GS,EDI,dirEnt.attr);

	unsigned int timeStamp=0;
	timeStamp|=((dirEnt.year-1980)<<25);
	timeStamp|=(dirEnt.month<<21);
	timeStamp|=(dirEnt.day<<16);
	timeStamp|=(dirEnt.hours<<11);
	timeStamp|=(dirEnt.minutes<<5);
	timeStamp|=(dirEnt.seconds>>1);

	townsPtr->CPU().DebugStoreDword(townsPtr->mem,32,GS,EDI+1,timeStamp);
	townsPtr->CPU().DebugStoreDword(townsPtr->mem,32,GS,EDI+5,(unsigned int)dirEnt.length);

	char file8_3[12];
	for(auto &c : file8_3)
	{
		c=0;
	}
	if("."==dirEnt.fName || ".."==dirEnt.fName)
	{
		for(int i=0; i<dirEnt.fName.size(); ++i)
		{
			file8_3[i]=dirEnt.fName[i];
		}
	}
	else
	{
		int dst=0,state=0;  // State 0:Before Extension  1:Extension
		for(auto c : dirEnt.fName)
		{
			if(0==state)
			{
				if('.'==c)
				{
					file8_3[dst++]='.';
					state=1;
				}
				else if(dst<8)
				{
					file8_3[dst++]=c;
				}
			}
			else
			{
				if('.'==c)
				{
					file8_3[ 8]='.';
					dst=8;
				}
				else if(dst<12)
				{
					file8_3[dst++]=c;
				}
			}
		}
		for(dst=dst; dst<sizeof(file8_3); ++dst)
		{
			file8_3[dst]=0;
		}
	}

	for(int i=0; i<12; ++i)
	{
		townsPtr->CPU().DebugStoreByte(townsPtr->mem,32,GS,EDI+9+i,file8_3[i]);
	}
	townsPtr->CPU().DebugStoreByte(townsPtr->mem,32,GS,EDI+9+12,0);
}
