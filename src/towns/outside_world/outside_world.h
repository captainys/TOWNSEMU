/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef OUTSIDE_WORLD_IS_INCLUDED
#define OUTSIDE_WORLD_IS_INCLUDED
/* { */

#include "render.h"
#include "discimg.h"
#include "rf5c68.h"
#include "ym2612.h"
#include "townsdef.h"

class Outside_World
{
public:
	bool windowShift=false;

	// Mouse will be automatically identified by towns.gameport.
	// Only game-pad emulation takes effect.
	unsigned int gamePort[2];

	// Pause mouse integration until mouse cursor is moved.
	// Strike Commander controls the view direction with mouse, and press F1 to reset.
	// The program moves the mouse coordinate to the center of the window when F1 is pressed.
	// However, with mouse integration turned on, the mouse coordinate moves back to wherever
	// host mouse cursor is located, and the view changes again.  To prevent it,
	// Mouse Integration should be paused until mouse is moved.
	// To more generalize, mouse integration is turned on when mouse moves, and pauses
	// when the mouse cursor in the VM is stationary at the host's mouse coordinate for
	// several steps, it pauses again.
	enum
	{
		MOUSE_STATIONARY_COUNT=4
	};
	bool mouseIntegrationActive=false;
	int lastMx,lastMy,mouseStationaryCount=MOUSE_STATIONARY_COUNT;

	enum
	{
		KEYBOARD_MODE_DIRECT,
		KEYBOARD_MODE_TRANSLATION
	};
	unsigned int keyboardMode=KEYBOARD_MODE_DIRECT;

	enum
	{
		KEYBOARD_LAYOUT_US,
		KEYBOARD_LAYOUT_JP,
	};

	enum
	{
		STATUS_WID=640,
		STATUS_HEI=16
	};
	unsigned char *statusBitmap;
	bool cdAccessLamp=false;
	bool fdAccessLamp[4]={false,false,false,false};
	bool scsiAccessLamp[6]={false,false,false,false,false,false};
	unsigned int scaling=100; // In Percent

	Outside_World();
	~Outside_World();

	virtual void Start(void)=0;
	virtual void Stop(void)=0;
	virtual void DevicePolling(class FMTowns &towns)=0;
	virtual void UpdateStatusBitmap(class FMTowns &towns)=0;
	virtual void Render(const TownsRender::Image &img)=0;

	void SetKeyboardMode(unsigned int mode);
	virtual void SetKeyboardLayout(unsigned int layout)=0;

	void Put16x16(int x0,int y0,const unsigned char icon16x16[]);
	void Put16x16Invert(int x0,int y0,const unsigned char icon16x16[]);

	void Put16x16Select(int x0,int y0,const unsigned char idleIcon16x16[],const unsigned char busyIcon16x16[],bool busy);
	void Put16x16SelectInvert(int x0,int y0,const unsigned char idleIcon16x16[],const unsigned char busyIcon16x16[],bool busy);

	/*! Implementation should call this function for each inkey for application-specific augmentation to work correctly.
	*/
	void ProcessInkey(class FMTowns &towns,int townsKey);

	/*! Implementation should call this function for each mouse reading for application-specific augmentation to work correctly.
	*/
	void ProcessMouse(class FMTowns &towns,int lb,int mb,int rb,int mx,int my);

public:
	virtual void CDDAPlay(const DiscImage &discImg,DiscImage::MinSecFrm from,DiscImage::MinSecFrm to,bool repeat)=0;
	virtual void CDDAStop(void)=0;
	virtual void CDDAPause(void)=0;
	virtual void CDDAResume(void)=0;
	virtual bool CDDAIsPlaying(void)=0;
	virtual DiscImage::MinSecFrm CDDACurrentPosition(void)=0;



public:
	virtual void PCMPlay(RF5C68 &pcm,unsigned int ch)=0; // Making a wave will change the flag: repeatAfterThisSegment of the channel.
	virtual void PCMPlay(unsigned int ch,std::vector <unsigned char > &wave)=0;
	virtual void PCMPlayStop(int ch)=0;
	virtual bool PCMChannelPlaying(int ch)=0;
	virtual double PCMCurrentPosition(int ch)=0;



public:
	virtual void FMPlay(unsigned int ch,std::vector <unsigned char> &wave)=0;
	virtual void FMPlayStop(int ch)=0;
	virtual bool FMChannelPlaying(int ch)=0;
};


/* } */
#endif
