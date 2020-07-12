/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "outside_world.h"

#include "towns.h"

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
void Outside_World::ProcessInkey(class FMTowns &towns,int townsKey)
{
	if(TOWNS_APPSPECIFIC_STRIKECOMMANDER==towns.state.appSpecificSetting && TOWNS_JISKEY_PF01==townsKey)
	{
		pauseMouseIntegration=true;
	}
}
void Outside_World::ProcessMouse(class FMTowns &towns,int lb,int mb,int rb,int mx,int my)
{
	if(TOWNS_APPSPECIFIC_STRIKECOMMANDER==towns.state.appSpecificSetting && true==pauseMouseIntegration)
	{
		int dx=lastMx-mx;
		int dy=lastMy-my;
		if(dx<-4 || 4<dx || dy<-4 || 4<dy)
		{
			pauseMouseIntegration=false;
		}
	}
	lastMx=mx;
	lastMy=my;
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
