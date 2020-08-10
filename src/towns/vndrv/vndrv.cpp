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

TownsVnDrv::TownsVnDrv(class FMTowns *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
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
			break;
		case TOWNS_VNDRV_CMD_FIND_FIRST://   0x1B,
			break;
		case TOWNS_VNDRV_CMD_FIND_NEXT://    0x1C,
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
					auto data=townsPtr->cpu.DebugFetchByte(32,townsPtr->cpu.state.DS(),townsPtr->cpu.state.EBX()+i,townsPtr->mem);
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
				i486DX::FarPointer ptr;
				ptr.SEG=townsPtr->cpu.state.DS().value;
				ptr.OFFSET=townsPtr->cpu.state.EBX();
				for(auto line : miscutil::MakeMemDump(townsPtr->cpu,townsPtr->mem,ptr,townsPtr->cpu.state.ECX(),/*shiftJIS=*/true))
				{
					std::cout << line << std::endl;
				}
			}
			break;
		}
	}
}
