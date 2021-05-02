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

#include <vector>
#include <string>

#include "render.h"
#include "discimg.h"
#include "rf5c68.h"
#include "ym2612.h"
#include "townsdef.h"

class Outside_World
{
public:
	class VirtualKey
	{
	public:
		unsigned int townsKey=0;
		int physicalId=-1;
		unsigned int button=0;
	};

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

	// Wing Commander and Strike Commander series can be configured to use mouse as joystick.
	bool mouseByFlightstickAvailable=false;
	bool cyberStickAssignment=false;
	bool mouseByFlightstickEnabled=false;
	int mouseByFlightstickPhysicalId=-1;  // Physical joystick ID.
	int mouseByFlightstickRecalibrateButton=-1; // Recalibrate button
	int mouseByFlightstickCenterX,mouseByFlightstickCenterY;
	float mouseByFlightstickZeroZoneX=0.0F,mouseByFlightstickZeroZoneY=0.0F;
	float mouseByFlightstickScaleX,mouseByFlightstickScaleY;
	float lastJoystickPos[2]={0.0F,0.0F};
	int lastMousePosForSwitchBackToNormalMode[2]={0,0};

	int wingCommander1ThrottleState=0;

	// For Wing Commander, Strike Commander, Fujitsu Air Warrior V2 Throttle Integration.
	int throttlePhysicalId=-1;
	int throttleAxis=2;  // Typically flight-stick's throttle axis is the 3rd axis (#2 axis).
	uint64_t lastThrottleMoveTime=0;
	uint64_t nextThrottleUpdateTime=0;


	/*! Virtual Keys.
	*/
	std::vector <VirtualKey> virtualKeys;


	/*! Cache of game-pad indices that needs to be updated in polling.
	*/
	std::vector <unsigned int> gamePadsNeedUpdate;
	bool gameDevsNeedUpdateCached=false;


	inline float ApplyZeroZone(float rawInput,float zeroZone)
	{
		if(rawInput<-zeroZone)
		{
			return rawInput+zeroZone;
		}
		else if(zeroZone<=rawInput)
		{
			return rawInput-zeroZone;
		}
		else
		{
			return 0.0;
		}
	}


	unsigned int keyboardMode=TOWNS_KEYBOARD_MODE_DIRECT;

	enum
	{
		KEYBOARD_LAYOUT_US,
		KEYBOARD_LAYOUT_JP,
	};

	enum
	{
		LOWER_RIGHT_NONE,
		LOWER_RIGHT_PAUSE,
		LOWER_RIGHT_MENU,
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
	bool autoScaling=false;
	bool maximizeOnStartUp=false;
	unsigned int dx=0,dy=0;  // Screen (0,0) will be window (dx,dy)
	unsigned int scaling=100; // In Percent
	bool pauseKey=false;

	unsigned int lowerRightIcon=LOWER_RIGHT_NONE;

	Outside_World();
	virtual ~Outside_World();

	virtual void Start(void)=0;
	virtual void Stop(void)=0;
	virtual void DevicePolling(class FMTowns &towns)=0;
	virtual void UpdateStatusBitmap(class FMTowns &towns)=0;
	virtual void Render(const TownsRender::Image &img,const class FMTowns &towns)=0;

	/*! Implementation should return true if the image needs to be flipped before drawn on the window.
	    The flag is transferred to rendering thread class at the beginning of the TownsThread::Start.
	*/
	virtual bool ImageNeedsFlip(void)=0;

	void SetKeyboardMode(unsigned int mode);
	virtual void SetKeyboardLayout(unsigned int layout)=0;

	void AddVirtualKey(unsigned int townsKey,int physicalId,unsigned int button);

	/*! Return pauseKey flag.  The flag is clear after this function.
	*/
	bool PauseKeyPressed(void);

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

	/*! Right now it updates mouse neutral position for Wing Commander 1 if app-specific augumentation is enabled.
	*/
	void ProcessAppSpecific(class FMTowns &towns);

	virtual std::vector <std::string> MakeKeyMappingText(void) const;
	virtual void LoadKeyMappingFromText(const std::vector <std::string> &text);


	/*! Cache gamePadsNeedUpdate member.
	    Reading game pad may not be the fastest function to call, and therefore reading same game pad multiple times
	    in one polling should be avoided.
	    This function caches which game pads needs to be updated.
	    If the sub-class overloads this function, call Outside_World::CacheGamePadIndicesThatNeedUpdates, and then
	    add an ID by calling UseGamePad function..
	*/
	virtual void CacheGamePadIndicesThatNeedUpdates(void);

	/*! Call this function to cache game pad index that needs to be updated every polling.
	*/
	void UseGamePad(unsigned int gamePadIndex);


public:
	/*! Left level and right level can be 0 to 256.  Value above 256 will be rounded to 256.
	*/
	virtual void CDDAPlay(const DiscImage &discImg,DiscImage::MinSecFrm from,DiscImage::MinSecFrm to,bool repeat,unsigned int leftLevel,unsigned int rightLevel)=0;
	virtual void CDDAStop(void)=0;
	virtual void CDDAPause(void)=0;
	virtual void CDDAResume(void)=0;
	virtual bool CDDAIsPlaying(void)=0;
	virtual DiscImage::MinSecFrm CDDACurrentPosition(void)=0;



public:
	virtual void FMPCMPlay(std::vector <unsigned char > &wave)=0;
	virtual void FMPCMPlayStop(void)=0;
	virtual bool FMPCMChannelPlaying(void)=0;



public:
	virtual void BeepPlay(int samplingRate, std::vector<unsigned char>& wave) = 0;
	virtual void BeepPlayStop() = 0;
	virtual bool BeepChannelPlaying() const = 0;
};


/* } */
#endif
