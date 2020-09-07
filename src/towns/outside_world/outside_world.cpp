/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>

#include "outside_world.h"

#include "towns.h"
#include "townsdef.h"

Outside_World::Outside_World()
{
	gamePort[0]=TOWNS_GAMEPORTEMU_NONE;
	gamePort[1]=TOWNS_GAMEPORTEMU_NONE;
	statusBitmap=new unsigned char [STATUS_WID*STATUS_HEI*4];
	for(int i=0; i<STATUS_WID*STATUS_HEI*4; ++i)
	{
		statusBitmap[i]=0;
	}
}
Outside_World::~Outside_World()
{
	delete [] statusBitmap;
	statusBitmap=nullptr;
}
void Outside_World::SetKeyboardMode(unsigned int mode)
{
	keyboardMode=mode;
}
void Outside_World::AddVirtualKey(unsigned int townsKey,int physicalId,unsigned int button)
{
	VirtualKey vk;
	vk.townsKey=townsKey;
	vk.physicalId=physicalId;
	vk.button=button;
	virtualKeys.push_back(vk);
}
void Outside_World::Put16x16(int x0,int y0,const unsigned char icon16x16[])
{
	auto dstPtr=statusBitmap+(STATUS_WID*y0+x0)*4;
	auto srcPtr=icon16x16;
	for(auto y=0; y<16; ++y)
	{
		for(auto x=0; x<16*4; ++x)
		{
			dstPtr[x]=*srcPtr;
			++srcPtr;
		}
		dstPtr+=STATUS_WID*4;
	}
}
bool Outside_World::PauseKeyPressed(void)
{
	auto flag=pauseKey;
	pauseKey=false;
	return flag;
}
void Outside_World::ProcessInkey(class FMTowns &towns,int townsKey)
{
}
void Outside_World::ProcessMouse(class FMTowns &towns,int lb,int mb,int rb,int mx,int my)
{
	towns.SetMouseButtonState((0!=lb),(0!=rb));
	if(true==mouseIntegrationActive)
	{
		int diffX,diffY;
		towns.ControlMouse(diffX,diffY,mx,my,towns.state.tbiosVersion);
		if(0==diffX && 0==diffY)
		{
			--mouseStationaryCount;
			if(mouseStationaryCount<=0)
			{
				mouseIntegrationActive=false;
				// std::cout << "Mouse Integration Paused" << std::endl;
			}
		}
		else
		{
			mouseStationaryCount=MOUSE_STATIONARY_COUNT;
		}
	}
	else
	{
		int dx=lastMx-mx;
		int dy=lastMy-my;
		if(0!=dx || 0!=dy)
		{
			// std::cout << "Mouse Integration Active" << std::endl;
			mouseIntegrationActive=true;
			mouseStationaryCount=MOUSE_STATIONARY_COUNT;
		}
		towns.DontControlMouse();
	}
	lastMx=mx;
	lastMy=my;
}
void Outside_World::ProcessAppSpecific(class FMTowns &towns)
{
	if(TOWNS_APPSPECIFIC_WINGCOMMANDER1==towns.state.appSpecificSetting)
	{
		auto debugStop=towns.debugger.stop; // FetchWord may break due to MEMR.
		this->mouseByFlightstickCenterX=2*(int)towns.mem.FetchWord(towns.state.appSpecific_MousePtrX+4);
		this->mouseByFlightstickCenterY=2*(int)towns.mem.FetchWord(towns.state.appSpecific_MousePtrY+4);
		towns.debugger.stop=debugStop;
	}
	else if(TOWNS_APPSPECIFIC_WINGCOMMANDER2==towns.state.appSpecificSetting)
	{
		auto debugStop=towns.debugger.stop; // FetchWord may break due to MEMR.
		this->mouseByFlightstickCenterX=2*(int)towns.mem.FetchWord(towns.state.appSpecific_MousePtrX+8);
		this->mouseByFlightstickCenterY=2*(int)towns.mem.FetchWord(towns.state.appSpecific_MousePtrY+8);
		towns.debugger.stop=debugStop;
	}
}

void Outside_World::Put16x16Invert(int x0,int y0,const unsigned char icon16x16[])
{
	auto dstPtr=statusBitmap+(STATUS_WID*y0+x0)*4;
	auto srcPtr=icon16x16;
	for(auto y=0; y<16; ++y)
	{
		for(auto x=0; x<16*4; ++x)
		{
			dstPtr[x]=*srcPtr;
			++srcPtr;
		}
		dstPtr-=STATUS_WID*4;
	}
}
void Outside_World::Put16x16Select(int x0,int y0,const unsigned char idleIcon16x16[],const unsigned char busyIcon16x16[],bool busy)
{
	if(true==busy)
	{
		Put16x16(x0,y0,busyIcon16x16);
	}
	else
	{
		Put16x16(x0,y0,idleIcon16x16);
	}
}
void Outside_World::Put16x16SelectInvert(int x0,int y0,const unsigned char idleIcon16x16[],const unsigned char busyIcon16x16[],bool busy)
{
	if(true==busy)
	{
		Put16x16Invert(x0,y0,busyIcon16x16);
	}
	else
	{
		Put16x16Invert(x0,y0,idleIcon16x16);
	}
}

/* virtual */ std::vector <std::string> Outside_World::MakeKeyMappingText(void) const
{
	std::vector <std::string> text;
	std::cout << __FUNCTION__ << " is not implemented in this environment." << std::endl;
	return text;
}
/* virtual */ void Outside_World::LoadKeyMappingFromText(const std::vector <std::string> &text)
{
	std::cout << __FUNCTION__ << " is not implemented in this environment." << std::endl;
}

/* virtual */ void Outside_World::CacheGamePadIndicesThatNeedUpdates(void)
{
	gameDevsNeedUpdateCached=true;
	gamePadsNeedUpdate.clear();
	for(auto padType : gamePort)
	{
		switch(padType)
		{
		case TOWNS_GAMEPORTEMU_PHYSICAL0:
		case TOWNS_GAMEPORTEMU_PHYSICAL1:
		case TOWNS_GAMEPORTEMU_PHYSICAL2:
		case TOWNS_GAMEPORTEMU_PHYSICAL3:
		case TOWNS_GAMEPORTEMU_PHYSICAL4:
		case TOWNS_GAMEPORTEMU_PHYSICAL5:
		case TOWNS_GAMEPORTEMU_PHYSICAL6:
		case TOWNS_GAMEPORTEMU_PHYSICAL7:
			UseGamePad(padType-TOWNS_GAMEPORTEMU_PHYSICAL0);
			break;
		case TOWNS_GAMEPORTEMU_ANALOG0:
		case TOWNS_GAMEPORTEMU_ANALOG1:
		case TOWNS_GAMEPORTEMU_ANALOG2:
		case TOWNS_GAMEPORTEMU_ANALOG3:
		case TOWNS_GAMEPORTEMU_ANALOG4:
		case TOWNS_GAMEPORTEMU_ANALOG5:
		case TOWNS_GAMEPORTEMU_ANALOG6:
		case TOWNS_GAMEPORTEMU_ANALOG7:
			UseGamePad(padType-TOWNS_GAMEPORTEMU_ANALOG0);
			break;
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL0:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL1:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL2:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL3:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL4:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL5:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL6:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL7:
			UseGamePad(padType-TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL0);
			break;
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG0:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG1:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG2:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG3:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG4:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG5:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG6:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG7:
			UseGamePad(padType-TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG0);
			break;
		}
	}
	for(auto &vk : virtualKeys)
	{
		UseGamePad(vk.physicalId);
	}
	if(true==mouseByFlightstickAvailable && 0<=mouseByFlightstickPhysicalId)
	{
		UseGamePad(mouseByFlightstickPhysicalId);
	}
	if(0<=strikeCommanderThrottlePhysicalId)
	{
		UseGamePad(strikeCommanderThrottlePhysicalId);
	}
}

void Outside_World::UseGamePad(unsigned int gamePadIndex)
{
	// G*d D**n useless std::find.
	for(auto i : gamePadsNeedUpdate)
	{
		if(i==gamePadIndex)
		{
			return;
		}
	}
	gamePadsNeedUpdate.push_back(gamePadIndex);
}
