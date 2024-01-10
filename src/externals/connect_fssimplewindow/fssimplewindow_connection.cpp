/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <stdio.h>

#include "fssimplewindow.h"
#include "fssimplewindow_connection.h"

// G** D*** Windows headers! >>
#ifdef REG_NONE
#undef REG_NONE
#endif
#ifdef OUT
#undef OUT
#endif
// *od *amn Windows headers! <<

#include "cpputil.h"
#include "towns.h"
#include "icons.h"
#include "ysgamepad.h"
#include "townsparam.h"

#ifndef TSUGARU_I486_HIGH_FIDELITY
#define WINDOW_TITLE "FM Towns Emulator - TSUGARU"
#else
#define WINDOW_TITLE "FM Towns Emulator - TSUGARU (High-Fidelity Mode)"
#endif


FsSimpleWindowConnection::FsSimpleWindowConnection()
{
	FSKEYtoTownsKEY=new unsigned int [FSKEY_NUM_KEYCODE];
	FSKEYState=new unsigned int [FSKEY_NUM_KEYCODE];

	SetKeyboardMode(TOWNS_KEYBOARD_MODE_DIRECT);
	SetKeyboardLayout(KEYBOARD_LAYOUT_US);

	for(int i=0; i<FSKEY_NUM_KEYCODE; ++i)
	{
		FSKEYState[i]=0;
	}
}
FsSimpleWindowConnection::~FsSimpleWindowConnection()
{
	delete [] FSKEYtoTownsKEY;
	delete [] FSKEYState;
}

/* virtual */ std::vector <std::string> FsSimpleWindowConnection::MakeDefaultKeyMappingText(void) const
{
	unsigned int FSKEYtoTownsKEY[FSKEY_NUM_KEYCODE];
	MakeKeyMapFromLayout(FSKEYtoTownsKEY,KEYBOARD_LAYOUT_US);
	std::vector <std::string> text;
	text.push_back("#HostKeyCode            TownsKeyCode");
	for(int i=0; i<FSKEY_NUM_KEYCODE; ++i)
	{
		text.push_back("");
		text.back()+=FsKeyCodeToString(i);
		text.back()+=" ";
		while(text.back().size()<24)
		{
			text.back()+=" ";
		}
		text.back()+=TownsKeyCodeToStr(FSKEYtoTownsKEY[i]);
	}
	text.push_back("# Available Host Key Code");
	for(int i=0; i<FSKEY_NUM_KEYCODE; ++i)
	{
		text.push_back(std::string("#")+FsKeyCodeToString(i));
	}
	text.push_back("# Available Towns Key Code");
	for(int i=0; i<256; ++i)
	{
		auto str=TownsKeyCodeToStr(i);
		if(""!=str)
		{
			text.push_back("#"+str);
		}
	}
	return text;
}

/* virtual */ std::vector <std::string> FsSimpleWindowConnection::MakeKeyMappingText(void) const
{
	std::vector <std::string> text;
	for(int i=0; i<FSKEY_NUM_KEYCODE; ++i)
	{
		text.push_back("");
		text.back()+=FsKeyCodeToString(i);
		text.back()+=" ";
		while(text.back().size()<24)
		{
			text.back()+=" ";
		}
		text.back()+=TownsKeyCodeToStr(FSKEYtoTownsKEY[i]);
	}
	return text;
}
/* virtual */ void FsSimpleWindowConnection::LoadKeyMappingFromText(const std::vector <std::string> &text)
{
	for(int i=0; i<FSKEY_NUM_KEYCODE; ++i)
	{
		FSKEYtoTownsKEY[i]=TOWNS_JISKEY_NULL;
	}
	for(auto str : text)
	{
		if('#'==str[0])
		{
			continue;
		}
		auto argv=cpputil::Parser(str.c_str());
		if(2==argv.size())
		{
			auto fsKey=FsStringToKeyCode(argv[0].c_str());
			auto townsKey=TownsStrToKeyCode(argv[1]);
			FSKEYtoTownsKEY[fsKey]=townsKey;
		}
	}
}

/* virtual */ void FsSimpleWindowConnection::Start(void)
{
}
/* virtual */ void FsSimpleWindowConnection::Stop(void)
{
}

/* virtual */ void FsSimpleWindowConnection::DevicePolling(class FMTownsCommon &towns)
{
	// WindosInterface class is now in charge of updating device status.
	// Before DevicePolling is called, TownsThread::VMMainLoop calls WindowInterface::Communicate
	// to transfer cached events and device status to this->windowEvent.

	bool ctrlKey=(0!=windowEvent.keyState[FSKEY_CTRL]);
	bool shiftKey=(0!=windowEvent.keyState[FSKEY_SHIFT]);

	if(true!=Outside_World::gameDevsNeedUpdateCached)
	{
		std::cout << "Squawk!  Game Devices that need updates not cached!" << std::endl;
	}

	for(auto &mos : windowEvent.mouseEvents)
	{
		if(LOWER_RIGHT_NONE!=lowerRightIcon && FSMOUSEEVENT_LBUTTONDOWN==mos.evt)
		{
			int wid=windowEvent.winWid;
			int hei=windowEvent.winHei;

			int iconWid=0;
			int iconHei=0;
			switch(lowerRightIcon)
			{
			case LOWER_RIGHT_NONE:
				break;
			case LOWER_RIGHT_PAUSE:
				iconWid=PAUSE_wid;
				iconHei=PAUSE_hei;
				break;
			case LOWER_RIGHT_MENU:
				iconWid=MENU_wid;
				iconHei=MENU_hei;
				break;
			}
			if(wid-iconWid<mos.mx && hei-iconHei<mos.my)
			{
				this->pauseKey=true;
			}
		}
	}

	int lb=windowEvent.lastKnownMouse.lb;
	int mb=windowEvent.lastKnownMouse.mb;
	int rb=windowEvent.lastKnownMouse.rb;
	int mx=windowEvent.lastKnownMouse.mx;
	int my=windowEvent.lastKnownMouse.my;

	auto &gamePads=windowEvent.gamePads;

	if(prevGamePads.size()!=gamePads.size())
	{
		// Probably the first time.  There is no previous game-pad states.
		// Make it a copy.
		prevGamePads=gamePads;
	}

	bool gamePadEmulationByKey=false; // Emulate a gamepad with keyboard
	bool mouseEmulationByNumPad=false; // Emulate mouse with keyboard numpad
	for(unsigned int portId=0; portId<TOWNS_NUM_GAMEPORTS; ++portId)
	{
		if(TOWNS_GAMEPORTEMU_KEYBOARD==gamePort[portId] ||
		   TOWNS_GAMEPORTEMU_MOUSE_BY_KEY==gamePort[portId])
		{
			gamePadEmulationByKey=true;
		}
		if(TOWNS_GAMEPORTEMU_MOUSE_BY_NUMPAD==gamePort[portId])
		{
			mouseEmulationByNumPad=true;
		}
	}

	for(auto vk : virtualKeys)
	{
		if(0<=vk.physicalId && vk.physicalId<gamePads.size())
		{
			if(prevGamePads[vk.physicalId].buttons[vk.button]!=gamePads[vk.physicalId].buttons[vk.button])
			{
				if(0!=gamePads[vk.physicalId].buttons[vk.button])
				{
					towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_PRESS  ,vk.townsKey);
				}
				else
				{
					towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_RELEASE,vk.townsKey);
				}
			}
		}
	}


	// Strike Commander throttle control.
	if(TOWNS_APPSPECIFIC_STRIKECOMMANDER==towns.state.appSpecificSetting && 0<=throttlePhysicalId && throttlePhysicalId<gamePads.size())
	{
		if(prevGamePads[throttlePhysicalId].axes[throttleAxis]!=
		   gamePads[throttlePhysicalId].axes[throttleAxis])
		{
			int prev=(1.0F-prevGamePads[throttlePhysicalId].axes[throttleAxis])*5.0F;
			int now=(1.0F-gamePads[throttlePhysicalId].axes[throttleAxis])*5.0F;

			if(prev<0)
			{
				prev=0;
			}
			else if(9<prev)
			{
				prev=9;
			}
			if(now<0)
			{
				now=0;
			}
			else if(9<now)
			{
				now=9;
			}
			// When C++17 is available :-P
			// prev=std::clamp<int>(prev,0,9);
			// now=std::clamp<int>(prev,0,9);

			if(prev!=now)
			{
				unsigned int key;
				if(now<9)
				{
					key=TOWNS_JISKEY_1+now;
				}
				else
				{
					key=TOWNS_JISKEY_0;
				}
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_PRESS  ,key);
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_RELEASE,key);
			}
		}
	}
	// Wing Commander Throttle Control
	if(0<=throttlePhysicalId && throttlePhysicalId<gamePads.size())
	{
		/* The following method does not make too many key strokes,
		   however, if another key is pressed while the set speed is still changing,
		   the speed stops changing.
		if(TOWNS_APPSPECIFIC_WINGCOMMANDER1==towns.state.appSpecificSetting)
		{
			unsigned int setSpeed,maxSpeed;
			towns.GetWingCommanderSetSpeedMaxSpeed(setSpeed,maxSpeed);

			unsigned int thr=(unsigned int)((1.0f-gamePads[throttlePhysicalId].axes[throttleAxis])*128.0f); // 0-255 scale
			thr=thr*maxSpeed/255;
			if(1!=wingCommander1ThrottleState && setSpeed<thr)
			{
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_NUM_MINUS);
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_NUM_PLUS);
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_PRESS  ,TOWNS_JISKEY_NUM_PLUS);
				wingCommander1ThrottleState=1;
			}
			else if(-1!=wingCommander1ThrottleState && thr<setSpeed)
			{
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_NUM_PLUS);
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_NUM_MINUS);
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_PRESS  ,TOWNS_JISKEY_NUM_MINUS);
				wingCommander1ThrottleState=-1;
			}
			else if((0<wingCommander1ThrottleState && thr<=setSpeed) ||
			        (wingCommander1ThrottleState<0 && setSpeed<=thr))
			{
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_NUM_PLUS);
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_NUM_MINUS);
				wingCommander1ThrottleState=0;
			}
		} */
		if(TOWNS_APPSPECIFIC_WINGCOMMANDER2==towns.state.appSpecificSetting ||
		   TOWNS_APPSPECIFIC_WINGCOMMANDER1==towns.state.appSpecificSetting)
		{
			unsigned int setSpeed,maxSpeed;
			towns.GetWingCommanderSetSpeedMaxSpeed(setSpeed,maxSpeed);

			unsigned int prevThr=(unsigned int)((1.0f-prevGamePads[throttlePhysicalId].axes[throttleAxis])*128.0f); // 0-255 scale
			unsigned int thr=(unsigned int)((1.0f-gamePads[throttlePhysicalId].axes[throttleAxis])*128.0f); // 0-255 scale
			prevThr=prevThr*maxSpeed/255;
			thr=thr*maxSpeed/255;

			if(prevThr!=thr)
			{
				std::cout << "pre " << prevThr << " new " << thr << " set " << setSpeed << " max " << maxSpeed << std::endl;
				lastThrottleMoveTime=towns.state.townsTime;
				nextThrottleUpdateTime=towns.state.townsTime;
			}

			if(towns.state.townsTime<lastThrottleMoveTime+4*PER_SECOND && nextThrottleUpdateTime<=towns.state.townsTime)
			{
				// Wing Commander I speed up/slow down while plus or minus key is held down.
				// However, if another key is pressed during the acceleration or desceleration
				// the set-speed stops changing.
				// Therefore, it needs to send key-release code periodically to make the program
				// think the plus or minus key is re-pressed to allow throttle to work while
				// other keys are functional.
				nextThrottleUpdateTime=towns.state.townsTime+PER_SECOND/16;
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_NUM_PLUS);
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_NUM_MINUS);
				if(setSpeed<thr)
				{
					towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_PRESS  ,TOWNS_JISKEY_NUM_PLUS);
				}
				else if(thr<setSpeed)
				{
					towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_PRESS  ,TOWNS_JISKEY_NUM_MINUS);
				}
			}
		}
		else if(TOWNS_APPSPECIFIC_AIRWARRIOR_V2==towns.state.appSpecificSetting)
		{
			unsigned int prevInputThr=(unsigned int)((1.0f-prevGamePads[throttlePhysicalId].axes[throttleAxis])*8.1f); // 0 to 16 scale
			unsigned int inputThr=(unsigned int)((1.0f-gamePads[throttlePhysicalId].axes[throttleAxis])*8.1f); // 0 to 16 scale

			if(prevInputThr!=inputThr)
			{
				lastThrottleMoveTime=towns.state.townsTime;
				nextThrottleUpdateTime=towns.state.townsTime;
			}

			if(towns.state.townsTime<lastThrottleMoveTime+4*PER_SECOND && nextThrottleUpdateTime<=towns.state.townsTime)
			{
				nextThrottleUpdateTime=towns.state.townsTime+PER_SECOND/16;
				unsigned int currentThr=(towns.mem.FetchByte(towns.state.appSpecific_ThrottlePtr))/12; // 0 to 16
				unsigned int keyToPress=TOWNS_JISKEY_NULL;
				if(currentThr<inputThr)
				{
					keyToPress=TOWNS_JISKEY_C;
				}
				else if(inputThr<currentThr)
				{
					keyToPress=TOWNS_JISKEY_V;
				}
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_PRESS  ,keyToPress);
				towns.keyboard.PushFifo(TOWNS_KEYFLAG_JIS_RELEASE,keyToPress);
			}
		}
		else if(TOWNS_APPSPECIFIC_AFTERBURNER2==towns.state.appSpecificSetting)
		{
			// Afterburner II Application-Specific Customization
			// Contribution from BCC.
			unsigned int inputThr = (unsigned int)((1.0f - gamePads[throttlePhysicalId].axes[throttleAxis]) * 1.5f); // 0 to 2 scale
			towns.AB2_Throttle(inputThr);
		}
	}

	// For the time translation mode only.
	// if(true==keyTranslationMode)
	if(TOWNS_KEYBOARD_MODE_DIRECT!=keyboardMode) // Means one of the translation modes.
	{
		for(auto c : windowEvent.charCode)
		{
			if(0==windowEvent.keyState[FSKEY_CTRL])
			{
				if(' '<=c)
				{
					unsigned char byteData[2]={0,0};
					if(0<TownsKeyboard::TranslateChar(byteData,c))
					{
						towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_PRESS  ,byteData[1]);
						towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_RELEASE,byteData[1]);
					}
				}
			}
		}
		for(auto c : windowEvent.keyCode)
		{
			if(PAUSE_KEY_CODE==c)
			{
				PauseKeyPressed();
			}
			if(hostShortCut[c].inUse && hostShortCut[c].ctrl==ctrlKey && hostShortCut[c].shift==shiftKey)
			{
				this->commandQueue.push(hostShortCut[c].cmdStr);
				continue;
			}

			this->ProcessInkey(towns,FSKEYtoTownsKEY[c]);
			unsigned char keyFlags=0;
			switch(c)
			{
			default:
				// CTRL+C, CTRL+S, CTRL+Q...
				if(ctrlKey && FSKEY_A<=c && c<=FSKEY_Z)
				{
					// Can take Ctrl+? and Ctrl+Shift+?, but Shift+? is taken by FsInkeyChar() already.
					// Therefore this block should only process only if Ctrl key is held down.
					keyFlags=TOWNS_KEYFLAG_CTRL;
					if(shiftKey)
					{
						keyFlags=TOWNS_KEYFLAG_SHIFT;
					}
					towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_PRESS,  FSKEYtoTownsKEY[c]);
					towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_RELEASE,FSKEYtoTownsKEY[c]);
				}
				break;
			case FSKEY_ESC:
				// User Request: Want to use ESC as ESC.
				// Problem: F-BASIC386 uses Break.
				// Trying: Physical ESC key makes both BREAK and ESC strokes.
				keyFlags|=(ctrlKey ? TOWNS_KEYFLAG_CTRL : 0);
				keyFlags|=(shiftKey ? TOWNS_KEYFLAG_SHIFT : 0);
				if(TOWNS_KEYBOARD_MODE_TRANSLATION1==keyboardMode)
				{
					towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_BREAK);
					towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_BREAK);
					towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_ESC);
					towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_ESC);
				}
				else if(TOWNS_KEYBOARD_MODE_TRANSLATION2==keyboardMode)
				{
					towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_ESC);
					towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_ESC);
				}
				else if(TOWNS_KEYBOARD_MODE_TRANSLATION3==keyboardMode)
				{
					towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_PRESS,  TOWNS_JISKEY_BREAK);
					towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_RELEASE,TOWNS_JISKEY_BREAK);
				}
				break;
			case FSKEY_ENTER:
			case FSKEY_BS:
			case FSKEY_TAB:
			case FSKEY_HOME:
			case FSKEY_END:
			case FSKEY_PAGEUP:
			case FSKEY_PAGEDOWN:
			case FSKEY_NUMLOCK:
			case FSKEY_ALT:
			case FSKEY_INS:
			case FSKEY_DEL:
			case FSKEY_F1:
			case FSKEY_F2:
			case FSKEY_F3:
			case FSKEY_F4:
			case FSKEY_F5:
			case FSKEY_F6:
			case FSKEY_F7:
			case FSKEY_F8:
			case FSKEY_F9:
			case FSKEY_F10:
			case FSKEY_F11:
			case FSKEY_F12:
			case FSKEY_CAPSLOCK:
			case FSKEY_CONVERT:
			case FSKEY_NONCONVERT:
			case FSKEY_KANA:       // Japanese JIS Keyboard Only => Win32 VK_KANA
			case FSKEY_RO:         // Japanese JIS Keyboard Only => Win32 VK_OEM_102
			case FSKEY_ZENKAKU:    // Japanese JIS Keyboard Only => Full Pitch/Half Pitch
			case FSKEY_WHEELUP:
			case FSKEY_WHEELDOWN:
			case FSKEY_CONTEXT:
			case FSKEY_UP:
			case FSKEY_DOWN:
			case FSKEY_LEFT:
			case FSKEY_RIGHT:
				keyFlags|=(ctrlKey ? TOWNS_KEYFLAG_CTRL : 0);
				keyFlags|=(shiftKey ? TOWNS_KEYFLAG_SHIFT : 0);
				towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_PRESS,  FSKEYtoTownsKEY[c]);
				towns.keyboard.PushFifo(keyFlags|TOWNS_KEYFLAG_JIS_RELEASE,FSKEYtoTownsKEY[c]);
				break;
			}
		}
	}
	else // if(TOWNS_KEYBOARD_MODE_DIRECT==keyboardMode)
	{
		for(auto c : windowEvent.keyCode)
		{
			unsigned char byteData=0;
			this->ProcessInkey(towns,FSKEYtoTownsKEY[c]);
			if(PAUSE_KEY_CODE==c)
			{
				PauseKeyPressed();
			}
			if(hostShortCut[c].inUse && hostShortCut[c].ctrl==ctrlKey && hostShortCut[c].shift==shiftKey)
			{
				this->commandQueue.push(hostShortCut[c].cmdStr);
				continue;
			}

			if(TOWNS_JISKEY_NULL!=FSKEYtoTownsKEY[c])
			{
				if(true==gamePadEmulationByKey &&
				   (FSKEY_Z==c ||
				    FSKEY_X==c ||
				    FSKEY_A==c ||
				    FSKEY_S==c ||
				    FSKEY_LEFT==c ||
				    FSKEY_RIGHT==c ||
				    FSKEY_UP==c ||
				    FSKEY_DOWN==c))
				{
					break;
				}
				if(true==mouseEmulationByNumPad &&
				   (FSKEY_TEN0==c ||
				    FSKEY_TEN1==c ||
				    FSKEY_TEN2==c ||
				    FSKEY_TEN3==c ||
				    FSKEY_TEN4==c ||
				    FSKEY_TEN5==c ||
				    FSKEY_TEN6==c ||
				    FSKEY_TEN7==c ||
				    FSKEY_TEN8==c ||
				    FSKEY_TEN9==c ||
				    FSKEY_TENSTAR==c ||
				    FSKEY_TENSLASH==c))
				{
					break;
				}

				byteData|=(ctrlKey ? TOWNS_KEYFLAG_CTRL : 0);
				byteData|=(shiftKey ? TOWNS_KEYFLAG_SHIFT : 0);
				if(0!=FSKEYState[c])
				{
					byteData|=0xF0; // Typamatic==Repeat?
				}
				else
				{
					byteData|=TOWNS_KEYFLAG_JIS_PRESS;
				}
				towns.keyboard.PushFifo(byteData,FSKEYtoTownsKEY[c]);

				// There is a possibility that FsGetKeyState turns 1 before FsInkey catches a keycode.
				// If so, the first inkey may make a typamatic (repeat) code, which may be disregarded
				// by some programs.
				// Therefore, turn it 1 upon inkey, and turn it off if FsGetKeyState detects key release.
				// Don't turn it on by FsGetKeyState.
				FSKEYState[c]=1;
			}
		}
		for(int key=FSKEY_NULL; key<FSKEY_NUM_KEYCODE; ++key)
		{
			if(true==gamePadEmulationByKey &&
			   (FSKEY_Z==key ||
			    FSKEY_X==key ||
			    FSKEY_A==key ||
			    FSKEY_S==key ||
			    FSKEY_LEFT==key ||
			    FSKEY_RIGHT==key ||
			    FSKEY_UP==key ||
			    FSKEY_DOWN==key))
			{
				continue;
			}
			if(true==mouseEmulationByNumPad &&
			   (FSKEY_TEN0==key ||
			    FSKEY_TEN1==key ||
			    FSKEY_TEN2==key ||
			    FSKEY_TEN3==key ||
			    FSKEY_TEN4==key ||
			    FSKEY_TEN5==key ||
			    FSKEY_TEN6==key ||
			    FSKEY_TEN7==key ||
			    FSKEY_TEN8==key ||
			    FSKEY_TEN9==key ||
			    FSKEY_TENSTAR==key ||
			    FSKEY_TENSLASH==key))
			{
				continue;
			}

			unsigned char byteData=0;
			auto sta=windowEvent.keyState[key];
			if(0!=FSKEYtoTownsKEY[key] && 0!=FSKEYState[key] && 0==sta)
			{
				byteData|=(ctrlKey ? TOWNS_KEYFLAG_CTRL : 0);
				byteData|=(shiftKey ? TOWNS_KEYFLAG_SHIFT : 0);
				byteData|=TOWNS_KEYFLAG_JIS_RELEASE;
				towns.keyboard.PushFifo(byteData,FSKEYtoTownsKEY[key]);
			}
			// See comment above regarding the timing of FsGetKeyState and FsInkey.
			if(0==sta)
			{
				FSKEYState[key]=0;
			}
		}
	}

	if(towns.eventLog.mode!=TownsEventLog::MODE_PLAYBACK)
	{
		bool mouseEmulationByAnalogAxis=false;
		for(unsigned int portId=0; portId<TOWNS_NUM_GAMEPORTS; ++portId)
		{
			switch(gamePort[portId])
			{
			default:
				// Not implemented yet.
				break;
			case TOWNS_GAMEPORTEMU_KEYBOARD:
				{
					bool Abutton=(0!=windowEvent.keyState[FSKEY_Z]);
					bool Bbutton=(0!=windowEvent.keyState[FSKEY_X]);
					bool run=(0!=windowEvent.keyState[FSKEY_A]);
					bool pause=(0!=windowEvent.keyState[FSKEY_S]);
					bool left=(0!=windowEvent.keyState[FSKEY_LEFT]);
					bool right=(0!=windowEvent.keyState[FSKEY_RIGHT]);
					if(true==left && true==right)
					{
						right=false;
					}
					bool up=(0!=windowEvent.keyState[FSKEY_UP]);
					bool down=(0!=windowEvent.keyState[FSKEY_DOWN]);
					if(true==up && true==down)
					{
						down=false;
					}
					towns.SetGamePadState(portId,Abutton,Bbutton,left,right,up,down,run,pause);
				}
				break;
			case TOWNS_GAMEPORTEMU_PHYSICAL0:
			case TOWNS_GAMEPORTEMU_PHYSICAL1:
			case TOWNS_GAMEPORTEMU_PHYSICAL2:
			case TOWNS_GAMEPORTEMU_PHYSICAL3:
			case TOWNS_GAMEPORTEMU_PHYSICAL4:
			case TOWNS_GAMEPORTEMU_PHYSICAL5:
			case TOWNS_GAMEPORTEMU_PHYSICAL6:
			case TOWNS_GAMEPORTEMU_PHYSICAL7:
				{
					int padId=gamePort[portId]-TOWNS_GAMEPORTEMU_PHYSICAL0;
					if(0<=padId && padId<gamePads.size())
					{
						auto &reading=gamePads[padId];
						towns.SetGamePadState(
						    portId,
						    reading.buttons[0],
						    reading.buttons[1],
						    reading.dirs[0].upDownLeftRight[2],
						    reading.dirs[0].upDownLeftRight[3],
						    reading.dirs[0].upDownLeftRight[0],
						    reading.dirs[0].upDownLeftRight[1],
						    reading.buttons[2],
						    reading.buttons[3]);
					}
				}
				break;
			case TOWNS_GAMEPORTEMU_ANALOG0:
			case TOWNS_GAMEPORTEMU_ANALOG1:
			case TOWNS_GAMEPORTEMU_ANALOG2:
			case TOWNS_GAMEPORTEMU_ANALOG3:
			case TOWNS_GAMEPORTEMU_ANALOG4:
			case TOWNS_GAMEPORTEMU_ANALOG5:
			case TOWNS_GAMEPORTEMU_ANALOG6:
			case TOWNS_GAMEPORTEMU_ANALOG7:
				{
					int padId=gamePort[portId]-TOWNS_GAMEPORTEMU_ANALOG0;
					if(0<=padId && padId<gamePads.size())
					{
						auto &reading=gamePads[padId];
						YsGamdPadTranslateAnalogToDigital(&reading.dirs[0],reading.axes[0],reading.axes[1]);
						towns.SetGamePadState(
						    portId,
						    reading.buttons[0],
						    reading.buttons[1],
						    reading.dirs[0].upDownLeftRight[2],
						    reading.dirs[0].upDownLeftRight[3],
						    reading.dirs[0].upDownLeftRight[0],
						    reading.dirs[0].upDownLeftRight[1],
						    reading.buttons[2],
						    reading.buttons[3]);
					}
				}
				break;

			case TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL0:
			case TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL1:
			case TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL2:
			case TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL3:
			case TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL4:
			case TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL5:
			case TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL6:
			case TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL7:
				{
					int padId=gamePort[portId]-TOWNS_GAMEPORTEMU_CAPCOM_BY_PHYSICAL0;
					if(0<=padId && padId<gamePads.size())
					{
						auto &reading=gamePads[padId];
						bool up=reading.dirs[0].upDownLeftRight[2];
						bool dn=reading.dirs[0].upDownLeftRight[3];
						bool lf=reading.dirs[0].upDownLeftRight[0];
						bool ri=reading.dirs[0].upDownLeftRight[1];

						// Muscle Bomber cannot start without pressing START button.
						// So, I use physical buttons 8 and 9 for START/SELECT.
						if(reading.buttons[8])
						{
							up=true;
							dn=true;
						}
						if(reading.buttons[9])
						{
							lf=true;
							ri=true;
						}

						towns.SetCAPCOMCPSFState(
						    portId,
						    up,
						    dn,
						    lf,
						    ri,
						    reading.buttons[0],
						    reading.buttons[1],
						    reading.buttons[2],
						    reading.buttons[3],
						    reading.buttons[4],
						    reading.buttons[5],
						    reading.buttons[6],
						    reading.buttons[7]);
					}
				}
				break;

			case TOWNS_GAMEPORTEMU_MOUSE_BY_KEY:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_NUMPAD:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL0:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL1:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL2:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL3:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL4:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL5:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL6:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL7:
				{
					{
						const int accel=1;
						const int maxSpeed=80;
						const int div=20;

						bool upDownLeftRight[4]={false,false,false,false};
						bool button[2]={false,false};

						mouseEmulationByAnalogAxis=true;
						if(TOWNS_GAMEPORTEMU_MOUSE_BY_KEY==gamePort[portId])
						{
							upDownLeftRight[0]=(0!=windowEvent.keyState[FSKEY_UP]);
							upDownLeftRight[1]=(0!=windowEvent.keyState[FSKEY_DOWN]);
							upDownLeftRight[2]=(0!=windowEvent.keyState[FSKEY_LEFT]);
							upDownLeftRight[3]=(0!=windowEvent.keyState[FSKEY_RIGHT]);
							button[0]=(0!=windowEvent.keyState[FSKEY_Z]);
							button[1]=(0!=windowEvent.keyState[FSKEY_X]);
						}
						else if(TOWNS_GAMEPORTEMU_MOUSE_BY_NUMPAD==gamePort[portId])
						{
							upDownLeftRight[0]=(0!=windowEvent.keyState[FSKEY_TEN7] || 0!=windowEvent.keyState[FSKEY_TEN8] || 0!=windowEvent.keyState[FSKEY_TEN9]);
							upDownLeftRight[1]=(0!=windowEvent.keyState[FSKEY_TEN1] || 0!=windowEvent.keyState[FSKEY_TEN2] || 0!=windowEvent.keyState[FSKEY_TEN3]);
							upDownLeftRight[2]=(0!=windowEvent.keyState[FSKEY_TEN1] || 0!=windowEvent.keyState[FSKEY_TEN4] || 0!=windowEvent.keyState[FSKEY_TEN7]);
							upDownLeftRight[3]=(0!=windowEvent.keyState[FSKEY_TEN3] || 0!=windowEvent.keyState[FSKEY_TEN6] || 0!=windowEvent.keyState[FSKEY_TEN9]);
							button[0]=(0!=windowEvent.keyState[FSKEY_TENSLASH]);
							button[1]=(0!=windowEvent.keyState[FSKEY_TENSTAR]);
						}
						else
						{
							int padId=gamePort[portId]-TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL0;
							if(0<=padId && padId<gamePads.size())
							{
								const auto &reading=gamePads[padId];
								upDownLeftRight[0]=(0!=reading.dirs[0].upDownLeftRight[0]);
								upDownLeftRight[1]=(0!=reading.dirs[0].upDownLeftRight[1]);
								upDownLeftRight[2]=(0!=reading.dirs[0].upDownLeftRight[2]);
								upDownLeftRight[3]=(0!=reading.dirs[0].upDownLeftRight[3]);
								button[0]=(0!=reading.buttons[0]);
								button[1]=(0!=reading.buttons[1]);
							}
						}
						if(true==upDownLeftRight[0])
						{
							mouseDY+=accel;
						}
						else if(true==upDownLeftRight[1])
						{
							mouseDY-=accel;
						}
						else
						{
							mouseDY=0;
						}
						if(mouseDY<-maxSpeed)
						{
							mouseDY=-maxSpeed;
						}
						if(mouseDY>maxSpeed)
						{
							mouseDY=maxSpeed;
						}
						if(0!=true==upDownLeftRight[2])
						{
							mouseDX+=accel;
						}
						else if(0!=true==upDownLeftRight[3])
						{
							mouseDX-=accel;
						}
						else
						{
							mouseDX=0;
						}
						if(mouseDX<-maxSpeed)
						{
							mouseDX=-maxSpeed;
						}
						if(mouseDX>maxSpeed)
						{
							mouseDX=maxSpeed;
						}
						towns.SetMouseMotion(portId,mouseDX/div,mouseDY/div);
						towns.SetMouseButtonState(button[0],button[1]);
					}
				}
				break;
			case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG0:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG1:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG2:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG3:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG4:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG5:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG6:
			case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG7:
				{
					{
						const double maxSpeed=20.0;

						mouseEmulationByAnalogAxis=true;
						int padId=gamePort[portId]-TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG0;
						if(0<=padId && padId<gamePads.size())
						{
							const auto &reading=gamePads[padId];
							float dx=reading.axes[0]*maxSpeed;
							float dy=reading.axes[1]*maxSpeed;
							towns.SetMouseMotion(portId,-dx,-dy);
							towns.SetMouseButtonState(0!=reading.buttons[0],0!=reading.buttons[1]);
						}
					}
				}
				break;

			case TOWNS_GAMEPORTEMU_PHYSICAL0_AS_CYBERSTICK:
			case TOWNS_GAMEPORTEMU_PHYSICAL1_AS_CYBERSTICK:
			case TOWNS_GAMEPORTEMU_PHYSICAL2_AS_CYBERSTICK:
			case TOWNS_GAMEPORTEMU_PHYSICAL3_AS_CYBERSTICK:
			case TOWNS_GAMEPORTEMU_PHYSICAL4_AS_CYBERSTICK:
			case TOWNS_GAMEPORTEMU_PHYSICAL5_AS_CYBERSTICK:
			case TOWNS_GAMEPORTEMU_PHYSICAL6_AS_CYBERSTICK:
			case TOWNS_GAMEPORTEMU_PHYSICAL7_AS_CYBERSTICK:
				{
					auto physId=gamePort[portId]-TOWNS_GAMEPORTEMU_PHYSICAL0_AS_CYBERSTICK;
					if(0<=physId && physId<gamePads.size())
					{
						auto axisReading=gamePads[physId];

						float x=axisReading.axes[0];
						float y=axisReading.axes[1];
						float z=axisReading.axes[2];
						float w=axisReading.axes[3];
						int ix=x*127.0;
						int iy=y*127.0;
						int iz=z*127.0;
						int iw=w*127.0;
						ix=cpputil::Clamp(ix,-128,127);
						iy=cpputil::Clamp(iy,-128,127);
						iz=cpputil::Clamp(iz,-128,127);
						iw=cpputil::Clamp(iw,-128,127);

						unsigned int trig=0;
						trig|=(axisReading.buttons[0] ? 0x01 : 0);
						trig|=(axisReading.buttons[1] ? 0x02 : 0);
						trig|=(axisReading.buttons[2] ? 0x04 : 0);
						trig|=(axisReading.buttons[3] ? 0x08 : 0);
						trig|=(axisReading.buttons[4] ? 0x10 : 0);
						trig|=(axisReading.buttons[5] ? 0x20 : 0);
						trig|=(axisReading.buttons[6] ? 0x40 : 0);
						trig|=(axisReading.buttons[7] ? 0x80 : 0);
						trig|=(axisReading.buttons[8] ? 0x100 : 0);
						trig|=(axisReading.buttons[9] ? 0x200 : 0);
						trig|=(axisReading.buttons[10] ? 0x400 : 0);
						trig|=(axisReading.buttons[11] ? 0x800 : 0);
						towns.SetCyberStickState(portId,ix,iy,iz,iw,trig);
					}
				}
				break;

			case TOWNS_GAMEPORTEMU_CYBERSTICK:
				if(true==cyberStickAssignment && 0<=mouseByFlightstickPhysicalId && mouseByFlightstickPhysicalId<gamePads.size())
				{
					auto axisReading=gamePads[mouseByFlightstickPhysicalId];
					decltype(axisReading) throttleReading;
					float z=0;
					if(0<=throttlePhysicalId && throttlePhysicalId<gamePads.size())
					{
						throttleReading=gamePads[throttlePhysicalId];
						z=gamePads[throttlePhysicalId].axes[throttleAxis];
					}
					else
					{
						throttleReading=axisReading;
						z=gamePads[throttlePhysicalId].axes[2];
					}

					float x=axisReading.axes[0];
					float y=axisReading.axes[1];
					int ix=x*127.0;
					int iy=y*127.0;
					int iz=z*127.0;
					int iw=0;
					ix=cpputil::Clamp(ix,-128,127);
					iy=cpputil::Clamp(iy,-128,127);
					iz=cpputil::Clamp(iz,-128,127);

					unsigned int trig=0;
					trig|=((axisReading.buttons[0] || throttleReading.buttons[0]) ? 0x01 : 0);
					trig|=((axisReading.buttons[1] || throttleReading.buttons[1]) ? 0x02 : 0);
					trig|=((axisReading.buttons[2] || throttleReading.buttons[2]) ? 0x04 : 0);
					trig|=((axisReading.buttons[3] || throttleReading.buttons[3]) ? 0x08 : 0);
					trig|=((axisReading.buttons[4] || throttleReading.buttons[4]) ? 0x10 : 0);
					trig|=((axisReading.buttons[5] || throttleReading.buttons[5]) ? 0x20 : 0);
					trig|=((axisReading.buttons[6] || throttleReading.buttons[6]) ? 0x40 : 0);
					trig|=((axisReading.buttons[7] || throttleReading.buttons[7]) ? 0x80 : 0);
					trig|=((axisReading.buttons[8] || throttleReading.buttons[8]) ? 0x100 : 0);
					trig|=((axisReading.buttons[9] || throttleReading.buttons[9]) ? 0x200 : 0);
					trig|=((axisReading.buttons[10] || throttleReading.buttons[10]) ? 0x400 : 0);
					trig|=((axisReading.buttons[11] || throttleReading.buttons[11]) ? 0x800 : 0);
					towns.SetCyberStickState(portId,ix,iy,iz,iw,trig);
				}
				break;
			}
		}

		if(TOWNS_APPSPECIFIC_DAIKOUKAIJIDAI2==towns.state.appSpecificSetting &&
		   true==towns.Daikoukai2_ControlMouseByArrowKeys(
			    lb,mb,rb,mx,my,
			    windowEvent.keyState[FSKEY_LEFT],
			    windowEvent.keyState[FSKEY_UP],
			    windowEvent.keyState[FSKEY_RIGHT],
			    windowEvent.keyState[FSKEY_DOWN]))
		{
			this->ProcessMouse(towns,lb,mb,rb,mx,my);
		}
		else if(mouseEmulationByAnalogAxis!=true)
		{
			struct YsGamePadReading reading;
			mx-=this->dx;
			my-=this->dy;
			if(true==mouseByFlightstickAvailable && 0<=mouseByFlightstickPhysicalId && mouseByFlightstickPhysicalId<gamePads.size())
			{
				reading=gamePads[mouseByFlightstickPhysicalId];
				if(true!=mouseByFlightstickEnabled)
				{
					float dx=reading.axes[0]-lastJoystickPos[0];
					float dy=reading.axes[1]-lastJoystickPos[1];
					if(dx<=-0.1F || 0.1F<=dx || dy<=-0.1F || 0.1F<=dy)
					{
						mouseByFlightstickEnabled=true;
						lastMousePosForSwitchBackToNormalMode[0]=mx;
						lastMousePosForSwitchBackToNormalMode[1]=my;
					}
				}
				else
				{
					int dx=mx-lastMousePosForSwitchBackToNormalMode[0];
					int dy=my-lastMousePosForSwitchBackToNormalMode[1];
					if(dx<-10 || 10<dx || dy<-10 || 10<dy)
					{
						mouseByFlightstickEnabled=false;
						lastJoystickPos[0]=reading.axes[0];
						lastJoystickPos[1]=reading.axes[1];
					}
				}
			}

			if(true==mouseByFlightstickEnabled && TOWNS_APPSPECIFIC_WINGCOMMANDER1==towns.state.appSpecificSetting)
			{
				// Wing Commander 1 turned out to be using separate joystick position from the mouse coordinate.
				// Need to translate to the mouse coordinate.
				int curStickX,curStickY;
				curStickX=towns.mem.FetchDword(towns.state.appSpecific_StickPosXPtr);
				curStickY=towns.mem.FetchDword(towns.state.appSpecific_StickPosYPtr);
				curStickX=(curStickX&0x7FFF)-(curStickX&0x8000);
				curStickY=(curStickY&0x7FFF)-(curStickY&0x8000);

				float fx=reading.axes[0];
				float fy=reading.axes[1];
				fx=ApplyZeroZone(fx,mouseByFlightstickZeroZoneX);
				fy=ApplyZeroZone(fy,mouseByFlightstickZeroZoneY);
				int inputX=(int)(fx*80.0f);  // Thrustmaster does not really let stick coord move to the corner.
				int inputY=(int)(fy*80.0f);  // Should take some buffer to let it maneuver at full rotational speed.  (64.0->80.0)

				if(inputX<-63)
				{
					inputX=-63;
				}
				if(63<inputX)
				{
					inputX=63;
				}
				if(inputY<-63)
				{
					inputY=-63;
				}
				if(63<inputY)
				{
					inputY=63;
				}

				// Joystick Input Left=negative Right=positive    NoseUp=positive NoseDown=negative
				// Wing Commander Internal  Left=negative RIght=positive    NoseUp=positive NoseDown=negative
				int diffX=inputX-curStickX;
				int diffY=inputY-curStickY;

				if(diffX<-15)
				{
					diffX=-15;
				}
				if(15<diffX)
				{
					diffX=15;
				}
				if(diffY<-15)
				{
					diffY=-15;
				}
				if(15<diffY)
				{
					diffY=15;
				}

				towns.SetMouseButtonState((0!=lb),(0!=rb));
				for(auto &p : towns.gameport.state.ports)
				{
					if(p.device==TownsGamePort::MOUSE)
					{
						p.mouseMotion.Set(-diffX,-diffY);
					}
				}
			}
			else if(true==mouseByFlightstickEnabled && TOWNS_APPSPECIFIC_AIRWARRIOR_V2==towns.state.appSpecificSetting)
			{
				// Wing Commander 1 turned out to be using separate joystick position from the mouse coordinate.
				// Need to translate to the mouse coordinate.
				int curStickX,curStickY;
				curStickX=towns.mem.FetchDword(towns.state.appSpecific_StickPosXPtr);
				curStickY=towns.mem.FetchDword(towns.state.appSpecific_StickPosYPtr);
				curStickX=(curStickX&0x7FFF)-(curStickX&0x8000);
				curStickY=(curStickY&0x7FFF)-(curStickY&0x8000);

				float fx=reading.axes[0];
				float fy=reading.axes[1];
				fx=ApplyZeroZone(fx,mouseByFlightstickZeroZoneX);
				fy=ApplyZeroZone(fy,mouseByFlightstickZeroZoneY);
				int inputX=(int)(fx*250.0f);
				int inputY=(int)(fy*250.0f);

				// Joystick Input Left=negative Right=positive    NoseUp=positive NoseDown=negative
				// Wing Commander Internal  Left=negative RIght=positive    NoseUp=positive NoseDown=negative
				int diffX=inputX-curStickX;
				int diffY=inputY-curStickY;

				if(diffX<-80)
				{
					diffX=-80;
				}
				if(80<diffX)
				{
					diffX=80;
				}
				if(diffY<-80)
				{
					diffY=-80;
				}
				if(80<diffY)
				{
					diffY=80;
				}

				const int minimum_threshold=16;
				if(-minimum_threshold<diffX && diffX<0)
				{
					diffX=-minimum_threshold;
				}
				if(0<diffX && diffX<minimum_threshold)
				{
					diffX=minimum_threshold;
				}
				if(-minimum_threshold<diffY && diffY<0)
				{
					diffY=-minimum_threshold;
				}
				if(0<diffY && diffY<minimum_threshold)
				{
					diffY=minimum_threshold;
				}

				towns.SetMouseButtonState((0!=lb),(0!=rb));
				for(auto &p : towns.gameport.state.ports)
				{
					if(p.device==TownsGamePort::MOUSE)
					{
						p.mouseMotion.Set(-diffX,-diffY);
					}
				}
			}
			else if(true==mouseByFlightstickEnabled)
			{
				float fx=reading.axes[0];
				float fy=reading.axes[1];
				fx=ApplyZeroZone(fx,mouseByFlightstickZeroZoneX);
				fy=ApplyZeroZone(fy,mouseByFlightstickZeroZoneY);
				fx*=mouseByFlightstickScaleX;
                fy*=mouseByFlightstickScaleY;
				mx=mouseByFlightstickCenterX+(int)fx;
				my=mouseByFlightstickCenterY+(int)fy;
				lb=reading.buttons[0];
				rb=reading.buttons[1];
				if(TOWNS_APPSPECIFIC_WINGCOMMANDER2==towns.state.appSpecificSetting)
				{
					// Wing Commander 2 allows negative mouse coordinate, or the control will be really slow nose down.
					// But sending below -120 (2x scale) apparently changes the neutral position.
					if(mx<-20)
					{
						mx=-20;
					}
					if(my<-240)
					{
						my=-240;
					}
				}
				else
				{
					if(mx<0)
					{
						mx=0;
					}
					if(my<0)
					{
						my=0;
					}
				}
				this->ProcessMouse(towns,lb,mb,rb,mx,my);
			}
			else
			{
				int wid,hei;
				FsGetWindowSize(wid,hei);
				if(mx<0)
				{
					mx=0;
				}
				else if(wid<=mx)
				{
					mx=wid-1;
				}
				if(my<0)
				{
					my=0;
				}
				else if(hei<=my)
				{
					my=hei-1;
				}
				if(0!=scaling) // Just in case
				{
					mx=mx*100/scaling;
					my=my*100/scaling;
				}
				this->ProcessMouse(towns,lb,mb,rb,mx,my);
			}
		}
	}
}

/* virtual */ bool FsSimpleWindowConnection::ImageNeedsFlip(void)
{
	return false;
}
/* virtual */ void FsSimpleWindowConnection::SetKeyboardLayout(unsigned int layout)
{
	MakeKeyMapFromLayout(FSKEYtoTownsKEY,layout);
}

/* static */ void FsSimpleWindowConnection::MakeKeyMapFromLayout(unsigned int FSKEYtoTownsKEY[FSKEY_NUM_KEYCODE],unsigned int layout)
{
	for(int i=0; i<FSKEY_NUM_KEYCODE; ++i)
	{
		FSKEYtoTownsKEY[i]=TOWNS_JISKEY_NULL;
	}
	FSKEYtoTownsKEY[FSKEY_NULL]=        TOWNS_JISKEY_NULL;
	FSKEYtoTownsKEY[FSKEY_SPACE]=       TOWNS_JISKEY_SPACE;
	FSKEYtoTownsKEY[FSKEY_0]=           TOWNS_JISKEY_0;
	FSKEYtoTownsKEY[FSKEY_1]=           TOWNS_JISKEY_1;
	FSKEYtoTownsKEY[FSKEY_2]=           TOWNS_JISKEY_2;
	FSKEYtoTownsKEY[FSKEY_3]=           TOWNS_JISKEY_3;
	FSKEYtoTownsKEY[FSKEY_4]=           TOWNS_JISKEY_4;
	FSKEYtoTownsKEY[FSKEY_5]=           TOWNS_JISKEY_5;
	FSKEYtoTownsKEY[FSKEY_6]=           TOWNS_JISKEY_6;
	FSKEYtoTownsKEY[FSKEY_7]=           TOWNS_JISKEY_7;
	FSKEYtoTownsKEY[FSKEY_8]=           TOWNS_JISKEY_8;
	FSKEYtoTownsKEY[FSKEY_9]=           TOWNS_JISKEY_9;
	FSKEYtoTownsKEY[FSKEY_A]=           TOWNS_JISKEY_A;
	FSKEYtoTownsKEY[FSKEY_B]=           TOWNS_JISKEY_B;
	FSKEYtoTownsKEY[FSKEY_C]=           TOWNS_JISKEY_C;
	FSKEYtoTownsKEY[FSKEY_D]=           TOWNS_JISKEY_D;
	FSKEYtoTownsKEY[FSKEY_E]=           TOWNS_JISKEY_E;
	FSKEYtoTownsKEY[FSKEY_F]=           TOWNS_JISKEY_F;
	FSKEYtoTownsKEY[FSKEY_G]=           TOWNS_JISKEY_G;
	FSKEYtoTownsKEY[FSKEY_H]=           TOWNS_JISKEY_H;
	FSKEYtoTownsKEY[FSKEY_I]=           TOWNS_JISKEY_I;
	FSKEYtoTownsKEY[FSKEY_J]=           TOWNS_JISKEY_J;
	FSKEYtoTownsKEY[FSKEY_K]=           TOWNS_JISKEY_K;
	FSKEYtoTownsKEY[FSKEY_L]=           TOWNS_JISKEY_L;
	FSKEYtoTownsKEY[FSKEY_M]=           TOWNS_JISKEY_M;
	FSKEYtoTownsKEY[FSKEY_N]=           TOWNS_JISKEY_N;
	FSKEYtoTownsKEY[FSKEY_O]=           TOWNS_JISKEY_O;
	FSKEYtoTownsKEY[FSKEY_P]=           TOWNS_JISKEY_P;
	FSKEYtoTownsKEY[FSKEY_Q]=           TOWNS_JISKEY_Q;
	FSKEYtoTownsKEY[FSKEY_R]=           TOWNS_JISKEY_R;
	FSKEYtoTownsKEY[FSKEY_S]=           TOWNS_JISKEY_S;
	FSKEYtoTownsKEY[FSKEY_T]=           TOWNS_JISKEY_T;
	FSKEYtoTownsKEY[FSKEY_U]=           TOWNS_JISKEY_U;
	FSKEYtoTownsKEY[FSKEY_V]=           TOWNS_JISKEY_V;
	FSKEYtoTownsKEY[FSKEY_W]=           TOWNS_JISKEY_W;
	FSKEYtoTownsKEY[FSKEY_X]=           TOWNS_JISKEY_X;
	FSKEYtoTownsKEY[FSKEY_Y]=           TOWNS_JISKEY_Y;
	FSKEYtoTownsKEY[FSKEY_Z]=           TOWNS_JISKEY_Z;
	FSKEYtoTownsKEY[FSKEY_ESC]=         TOWNS_JISKEY_BREAK;
	FSKEYtoTownsKEY[FSKEY_F1]=          TOWNS_JISKEY_PF01;
	FSKEYtoTownsKEY[FSKEY_F2]=          TOWNS_JISKEY_PF02;
	FSKEYtoTownsKEY[FSKEY_F3]=          TOWNS_JISKEY_PF03;
	FSKEYtoTownsKEY[FSKEY_F4]=          TOWNS_JISKEY_PF04;
	FSKEYtoTownsKEY[FSKEY_F5]=          TOWNS_JISKEY_PF05;
	FSKEYtoTownsKEY[FSKEY_F6]=          TOWNS_JISKEY_PF06;
	FSKEYtoTownsKEY[FSKEY_F7]=          TOWNS_JISKEY_PF07;
	FSKEYtoTownsKEY[FSKEY_F8]=          TOWNS_JISKEY_PF08;
	FSKEYtoTownsKEY[FSKEY_F9]=          TOWNS_JISKEY_PF09;
	FSKEYtoTownsKEY[FSKEY_F10]=         TOWNS_JISKEY_PF10;
	FSKEYtoTownsKEY[FSKEY_F11]=         TOWNS_JISKEY_PF11;
	FSKEYtoTownsKEY[FSKEY_F12]=         TOWNS_JISKEY_PF12;
	FSKEYtoTownsKEY[FSKEY_PRINTSCRN]=   TOWNS_JISKEY_NULL;
	FSKEYtoTownsKEY[FSKEY_CAPSLOCK]=    TOWNS_JISKEY_CAPS;
	FSKEYtoTownsKEY[FSKEY_SCROLLLOCK]=  TOWNS_JISKEY_NULL; // Can assign something later.
	FSKEYtoTownsKEY[FSKEY_PAUSEBREAK]=  TOWNS_JISKEY_BREAK;
	FSKEYtoTownsKEY[FSKEY_BS]=          TOWNS_JISKEY_BACKSPACE;
	FSKEYtoTownsKEY[FSKEY_TAB]=         TOWNS_JISKEY_TAB;
	FSKEYtoTownsKEY[FSKEY_ENTER]=       TOWNS_JISKEY_RETURN;
	FSKEYtoTownsKEY[FSKEY_SHIFT]=       TOWNS_JISKEY_SHIFT;
	FSKEYtoTownsKEY[FSKEY_CTRL]=        TOWNS_JISKEY_CTRL;
	FSKEYtoTownsKEY[FSKEY_ALT]=         TOWNS_JISKEY_NULL; // Can assign something later.
	FSKEYtoTownsKEY[FSKEY_INS]=         TOWNS_JISKEY_INSERT;
	FSKEYtoTownsKEY[FSKEY_DEL]=         TOWNS_JISKEY_DELETE;
	FSKEYtoTownsKEY[FSKEY_HOME]=        TOWNS_JISKEY_HOME;
	FSKEYtoTownsKEY[FSKEY_END]=         TOWNS_JISKEY_NULL; // Should be translated as SHIFT+DEL
	FSKEYtoTownsKEY[FSKEY_PAGEUP]=      TOWNS_JISKEY_PREV;
	FSKEYtoTownsKEY[FSKEY_PAGEDOWN]=    TOWNS_JISKEY_NEXT;
	FSKEYtoTownsKEY[FSKEY_UP]=          TOWNS_JISKEY_UP;
	FSKEYtoTownsKEY[FSKEY_DOWN]=        TOWNS_JISKEY_DOWN;
	FSKEYtoTownsKEY[FSKEY_LEFT]=        TOWNS_JISKEY_LEFT;
	FSKEYtoTownsKEY[FSKEY_RIGHT]=       TOWNS_JISKEY_RIGHT;
	FSKEYtoTownsKEY[FSKEY_NUMLOCK]=     TOWNS_JISKEY_NULL; // Can assign something later.
	FSKEYtoTownsKEY[FSKEY_TILDA]=       TOWNS_JISKEY_ESC;
	FSKEYtoTownsKEY[FSKEY_MINUS]=       TOWNS_JISKEY_MINUS;
	FSKEYtoTownsKEY[FSKEY_PLUS]=        TOWNS_JISKEY_HAT;
	FSKEYtoTownsKEY[FSKEY_LBRACKET]=    TOWNS_JISKEY_LEFT_SQ_BRACKET;
	FSKEYtoTownsKEY[FSKEY_RBRACKET]=    TOWNS_JISKEY_RIGHT_SQ_BRACKET;
	FSKEYtoTownsKEY[FSKEY_BACKSLASH]=   TOWNS_JISKEY_BACKSLASH;
	FSKEYtoTownsKEY[FSKEY_SEMICOLON]=   TOWNS_JISKEY_SEMICOLON;
	FSKEYtoTownsKEY[FSKEY_SINGLEQUOTE]= TOWNS_JISKEY_COLON;
	FSKEYtoTownsKEY[FSKEY_COMMA]=       TOWNS_JISKEY_COMMA;
	FSKEYtoTownsKEY[FSKEY_DOT]=         TOWNS_JISKEY_DOT;
	FSKEYtoTownsKEY[FSKEY_SLASH]=       TOWNS_JISKEY_SLASH;
	FSKEYtoTownsKEY[FSKEY_TEN0]=        TOWNS_JISKEY_NUM_0;
	FSKEYtoTownsKEY[FSKEY_TEN1]=        TOWNS_JISKEY_NUM_1;
	FSKEYtoTownsKEY[FSKEY_TEN2]=        TOWNS_JISKEY_NUM_2;
	FSKEYtoTownsKEY[FSKEY_TEN3]=        TOWNS_JISKEY_NUM_3;
	FSKEYtoTownsKEY[FSKEY_TEN4]=        TOWNS_JISKEY_NUM_4;
	FSKEYtoTownsKEY[FSKEY_TEN5]=        TOWNS_JISKEY_NUM_5;
	FSKEYtoTownsKEY[FSKEY_TEN6]=        TOWNS_JISKEY_NUM_6;
	FSKEYtoTownsKEY[FSKEY_TEN7]=        TOWNS_JISKEY_NUM_7;
	FSKEYtoTownsKEY[FSKEY_TEN8]=        TOWNS_JISKEY_NUM_8;
	FSKEYtoTownsKEY[FSKEY_TEN9]=        TOWNS_JISKEY_NUM_9;
	FSKEYtoTownsKEY[FSKEY_TENDOT]=      TOWNS_JISKEY_NUM_DOT;
	FSKEYtoTownsKEY[FSKEY_TENSLASH]=    TOWNS_JISKEY_NUM_SLASH;
	FSKEYtoTownsKEY[FSKEY_TENSTAR]=     TOWNS_JISKEY_NUM_STAR;
	FSKEYtoTownsKEY[FSKEY_TENMINUS]=    TOWNS_JISKEY_NUM_MINUS;
	FSKEYtoTownsKEY[FSKEY_TENPLUS]=     TOWNS_JISKEY_NUM_PLUS;
	FSKEYtoTownsKEY[FSKEY_TENENTER]=    TOWNS_JISKEY_NUM_RETURN;
	FSKEYtoTownsKEY[FSKEY_WHEELUP]=     TOWNS_JISKEY_UP;
	FSKEYtoTownsKEY[FSKEY_WHEELDOWN]=   TOWNS_JISKEY_DOWN;
	FSKEYtoTownsKEY[FSKEY_CONTEXT]=     TOWNS_JISKEY_ALT; // Can assign something later.

	// Japanese keyboard
	FSKEYtoTownsKEY[FSKEY_CONVERT]=     TOWNS_JISKEY_CONVERT;
	FSKEYtoTownsKEY[FSKEY_NONCONVERT]=  TOWNS_JISKEY_NO_CONVERT;
	FSKEYtoTownsKEY[FSKEY_KANA]=        TOWNS_JISKEY_KATAKANA;
	// FSKEYtoTownsKEY[FSKEY_COLON]=       TOWNS_JISKEY_COLON; // Need to switch with single quote
	// FSKEYtoTownsKEY[FSKEY_AT]=          TOWNS_JISKEY_AT;  // FSKEY_AT collides with FSKEY_TILDA. This disables ESC.
	FSKEYtoTownsKEY[FSKEY_RO]=          TOWNS_JISKEY_DOUBLEQUOTE;

	// The following key codes won't be returned by FsInkey()
	// These may return non zero for FsGetKeyState
	FSKEYtoTownsKEY[FSKEY_LEFT_CTRL]=   TOWNS_JISKEY_CTRL;
	FSKEYtoTownsKEY[FSKEY_RIGHT_CTRL]=  TOWNS_JISKEY_CTRL;
	FSKEYtoTownsKEY[FSKEY_LEFT_SHIFT]=  TOWNS_JISKEY_SHIFT;
	FSKEYtoTownsKEY[FSKEY_RIGHT_SHIFT]= TOWNS_JISKEY_SHIFT;
	FSKEYtoTownsKEY[FSKEY_LEFT_ALT]=    TOWNS_JISKEY_NULL;
	FSKEYtoTownsKEY[FSKEY_RIGHT_ALT]=   TOWNS_JISKEY_NULL;
}

/* virtual */ void FsSimpleWindowConnection::RegisterHostShortCut(std::string hostKeyLabel,bool ctrl,bool shift,std::string cmdStr)
{
	auto fsKey=FsStringToKeyCode(hostKeyLabel.c_str());
	if(FSKEY_NULL!=fsKey)
	{
		hostShortCut[fsKey].inUse=true;
		hostShortCut[fsKey].ctrl=ctrl;
		hostShortCut[fsKey].shift=shift;
		hostShortCut[fsKey].cmdStr=cmdStr;
	}
}

/* virtual */ void FsSimpleWindowConnection::RegisterPauseResume(std::string hostKeyLabel)
{
	auto fsKey=FsStringToKeyCode(hostKeyLabel.c_str());
	if(FSKEY_NULL!=fsKey)
	{
		PAUSE_KEY_CODE=fsKey;
	}
	else
	{
		PAUSE_KEY_CODE=DEFAULT_PAUSE_KEY_CODE;
	}
}

void FsSimpleWindowConnection::PauseKeyPressed(void)
{
	if(0==windowEvent.keyState[FSKEY_SHIFT])
	{
		this->pauseKey=true;
	}
	else
	{
		ToggleMouseCursor();
	}
}

/* virtual */ void FsSimpleWindowConnection::ToggleMouseCursor(void)
{
	showMouseCursor=(showMouseCursor!=true);
}

////////////////////////////////////////////////////////////////

void FsSimpleWindowConnection::WindowConnection::Start(void)
{
	int wid=640*shared.scaling/100;
	int hei=480*shared.scaling/100;

	int winY0=0;
	if(true==windowShift)
	{
		winY0=48;
	}


	// In tight GUI integration, FsResizeWindow will re-enter OnDraw call-back, and will crash inside
	// unless bitmaps are ready.  Do it before FsResizeWindow.

	// Make PAUSE and MENU icons.  Used only in the tightly-integrated GUI.
	PAUSEicon.resize(4*PAUSE_wid*PAUSE_hei);
	MENUicon.resize(4*MENU_wid*MENU_hei);
	for(int y=0; y<PAUSE_hei; ++y)
	{
		int Y=PAUSE_hei-1-y;
		for(int x=0; x<PAUSE_wid; ++x)
		{
			PAUSEicon[(y*PAUSE_wid+x)*4  ]=PAUSE[(Y*PAUSE_wid+x)*4  ];
			PAUSEicon[(y*PAUSE_wid+x)*4+1]=PAUSE[(Y*PAUSE_wid+x)*4+1];
			PAUSEicon[(y*PAUSE_wid+x)*4+2]=PAUSE[(Y*PAUSE_wid+x)*4+2];
			PAUSEicon[(y*PAUSE_wid+x)*4+3]=PAUSE[(Y*PAUSE_wid+x)*4+3];
		}
	}
	for(int y=0; y<MENU_hei; ++y)
	{
		int Y=MENU_hei-1-y;
		for(int x=0; x<MENU_wid; ++x)
		{
			MENUicon[(y*MENU_wid+x)*4  ]=MENU[(Y*MENU_wid+x)*4  ];
			MENUicon[(y*MENU_wid+x)*4+1]=MENU[(Y*MENU_wid+x)*4+1];
			MENUicon[(y*MENU_wid+x)*4+2]=MENU[(Y*MENU_wid+x)*4+2];
			MENUicon[(y*MENU_wid+x)*4+3]=MENU[(Y*MENU_wid+x)*4+3];
		}
	}



	if(0==FsCheckWindowOpen())
	{
		FsOpenWindow(0,winY0,wid,hei+STATUS_HEI,1,WINDOW_TITLE);
	}
	else
	{
		FsResizeWindow(wid,hei+STATUS_HEI);
	}

	switch(windowModeOnStartUp)
	{
	case TownsStartParameters::WINDOW_MAXIMIZE:
		FsPollDevice();
		FsMaximizeWindow();
		for(int i=0; i<10; ++i)
		{
			FsPollDevice();
		}
		break;
	case TownsStartParameters::WINDOW_FULLSCREEN:
		FsPollDevice();
		FsMakeFullScreen();
		for(int i=0; i<10; ++i)
		{
			FsPollDevice();
		}
		autoScaling=true;
		break;
	}

	winThr.winWid=640;
	winThr.winHei=480;

	glClearColor(0,0,0,0);
	mainTexId=GenTexture();
	statusTexId=GenTexture();

	pauseIconTexId=GenTexture();
	UpdateTexture(pauseIconTexId,PAUSE_wid,PAUSE_hei,PAUSEicon.data());
	menuIconTexId=GenTexture();
	UpdateTexture(menuIconTexId,MENU_wid,MENU_hei,MENUicon.data());

	// Make initial status bitmap
	Put16x16Invert(0,15,CD_IDLE);
	for(int fd=0; fd<2; ++fd)
	{
		Put16x16Invert(16+16*fd,15,FD_IDLE);
	}
	for(int hdd=0; hdd<6; ++hdd)
	{
		Put16x16Invert(48+16*hdd,15,HDD_IDLE);
	}


	if(true!=winThrEx.gamePadInitialized)
	{
		YsGamePadInitialize();
		winThrEx.gamePadInitialized=true;
	}

	auto nGameDevs=YsGamePadGetNumDevices();
	if(0<nGameDevs)
	{
		winThrEx.primary.gamePads.resize(nGameDevs);
		for(unsigned int i=0; i<nGameDevs; ++i)
		{
			YsGamePadRead(&winThrEx.primary.gamePads[i],i);
		}
	}
}
void FsSimpleWindowConnection::WindowConnection::Stop(void)
{
	if(TownsStartParameters::WINDOW_NORMAL!=windowModeOnStartUp)
	{
		FsUnmaximizeWindow();
	}
	glDeleteTextures(1,&mainTexId);
	glDeleteTextures(1,&statusTexId);
	glDeleteTextures(1,&pauseIconTexId);
	glDeleteTextures(1,&menuIconTexId);
}
/*! Called from the Window Thread.
*/
void FsSimpleWindowConnection::WindowConnection::Interval(void)
{
	BaseInterval();

	FsPollDevice();

	if(shared.showMouseCursor!=(FsIsMouseCursorVisible()!=0))
	{
		if(true==shared.showMouseCursor)
		{
			FsShowMouseCursor(1);
		}
		else
		{
			FsShowMouseCursor(0);
		}
	}

	FsGetWindowSize(winThrEx.primary.winWid,winThrEx.primary.winHei);

	int code;
	while(FSKEY_NULL!=(code=FsInkey()))
	{
		winThrEx.primary.keyCode.push_back(code);
	}
	while(0!=(code=FsInkeyChar()))
	{
		winThrEx.primary.charCode.push_back(code);
	}
	for(int key=0; key<FSKEY_NUM_KEYCODE; ++key)
	{
		winThrEx.primary.keyState[key]=FsGetKeyState(key);
	}
	for(;;)
	{
		winThrEx.primary.lastKnownMouse.Read();
		if(FSMOUSEEVENT_NONE==winThrEx.primary.lastKnownMouse.evt)
		{
			break;
		}
		winThrEx.primary.mouseEvents.push_back(winThrEx.primary.lastKnownMouse);
	}

	PollGamePads();

	{
		std::lock_guard <std::mutex> lock(deviceStateLock);
		winThr.VMClosed=shared.VMClosedFromVMThread;
		winThr.gamePadsNeedUpdate=shared.gamePadsNeedUpdate;
		if(true==sharedEx.readyToSend.EventEmpty())
		{
			sharedEx.readyToSend=winThrEx.primary;
			winThrEx.primary.CleanUpEvents();
		}
	}
}
/*! Called from the Window thread.
      VM thread may access scaling, dx, dy, and lowerRightIcon, which therefore must be locked.
*/
void FsSimpleWindowConnection::WindowConnection::Render(bool swapBuffers)
{
	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);

	if(0==winThr.mostRecentImage.wid || 0==winThr.mostRecentImage.hei)
	{
		return;
	}

	auto imgWid=winThr.mostRecentImage.wid;
	auto imgHei=winThr.mostRecentImage.hei;

	// {
	renderingLock.lock();

	if(true==autoScaling)
	{
		if(0<imgWid && 0<imgHei)
		{
			unsigned int scaleX=100*winWid/imgWid;
			unsigned int scaleY=100*(winHei-STATUS_HEI)/imgHei;
			shared.scaling=std::min(scaleX,scaleY);
		}
	}

	unsigned int renderWid=imgWid*shared.scaling/100;
	unsigned int renderHei=imgHei*shared.scaling/100;
	shared.dx=(renderWid<winWid ? (winWid-renderWid)/2 : 0);
	shared.dy=(renderHei<(winHei-STATUS_HEI) ? (winHei-STATUS_HEI-renderHei)/2 : 0);

	UpdateStatusBitmap();

	UpdateTexture(statusTexId,STATUS_WID,STATUS_HEI,winThr.statusBitmap);
	UpdateTexture(mainTexId,winThr.mostRecentImage.wid,winThr.mostRecentImage.hei,winThr.mostRecentImage.rgba.data());

	auto lowerRightIcon=shared.lowerRightIcon;

	auto dx=shared.dx;
	auto dy=shared.dy;
	auto scaling=shared.scaling;

	auto strikeCommanderSpecial=sharedEx.statusBarInfo.strikeCommanderSpecial;

	renderingLock.unlock();
	// }


	if(true!=autoScaling)
	{
		if(winThr.winWid!=imgWid || winThr.winHei!=imgHei)
		{
			winThr.winWid=imgWid;
			winThr.winHei=imgHei;
			winThrEx.sinceLastResize=10;
		}
		else if(0<winThrEx.sinceLastResize)
		{
			--winThrEx.sinceLastResize;
			if(0==winThrEx.sinceLastResize)
			{
				FsResizeWindow(winThr.winWid*scaling/100,winThr.winHei*scaling/100+STATUS_HEI);
			}
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0,0,winWid,winHei);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f,float(winWid),float(winHei),0.0f,-1,1);


	glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glColor3f(1,1,1);

	glBindTexture(GL_TEXTURE_2D,statusTexId);
	DrawTextureRect(0,winHei-1-STATUS_HEI,STATUS_WID,winHei-1);

	switch(lowerRightIcon)
	{
	case LOWER_RIGHT_NONE:
		break;
	case LOWER_RIGHT_PAUSE:
		glBindTexture(GL_TEXTURE_2D,pauseIconTexId);
		DrawTextureRect(winWid-PAUSE_wid,winHei-1-PAUSE_hei,winWid,winHei-1);
		break;
	case LOWER_RIGHT_MENU:
		glBindTexture(GL_TEXTURE_2D,menuIconTexId);
		DrawTextureRect(winWid-MENU_wid,winHei-1-MENU_hei,winWid,winHei-1);
		break;
	}

	glBindTexture(GL_TEXTURE_2D,mainTexId);
	DrawTextureRect(dx,dy+imgHei*scaling/100,dx+imgWid*scaling/100,dy);

	glDisable(GL_TEXTURE_2D);

	if(true==strikeCommanderSpecial)
	{
		int x;
		glColor3ub(128,128,255);
		glBegin(GL_LINES);

		x=dx+160*2*scaling/100;
		glVertex2i(x,winHei-1);
		glVertex2i(x,winHei-STATUS_HEI+1);

		x=dx+224*2*scaling/100;
		glVertex2i(x,winHei-1);
		glVertex2i(x,winHei-STATUS_HEI+1);

		x=dx+278*2*scaling/100;
		glVertex2i(x,winHei-1);
		glVertex2i(x,winHei-STATUS_HEI+1);

		glEnd();
	}

	if(true==swapBuffers)
	{
		FsSwapBuffers();
	}
}

void FsSimpleWindowConnection::WindowConnection::UpdateImage(TownsRender::ImageCopy &img)
{
	renderingLock.lock();
	std::swap(winThr.mostRecentImage,img);
	renderingLock.unlock();
}

/*! Called in the VM thread.
    WindowInterface  ->(Device State)-> Outside_World
    WindowInterface  <-(Game Pads In Use)<- Outside_World
    WindowInterface  <-(Show Mouse Cursor) <- Outside_World
*/
void FsSimpleWindowConnection::WindowConnection::Communicate(Outside_World *ow)
{
	auto outside_world=dynamic_cast<FsSimpleWindowConnection*>(ow);
	std::swap(outside_world->prevGamePads,outside_world->windowEvent.gamePads);

	{
		std::lock_guard<std::mutex> lock(deviceStateLock);

		// Kind of want to use swap, but Communicate can be called more than once before the
		// next Interval is called, in which case state can go back and force between two
		// samples.  Therefore, copy here.
		outside_world->windowEvent=sharedEx.readyToSend;
		sharedEx.readyToSend.CleanUpEvents();

		shared.gamePadsNeedUpdate=outside_world->gamePadsNeedUpdate;
		shared.showMouseCursor=outside_world->showMouseCursor;
	}
	{
		std::lock_guard<std::mutex> lock(renderingLock);

		sharedEx.statusBarInfo=outside_world->statusBarInfo;
		shared.lowerRightIcon=outside_world->lowerRightIcon;

		outside_world->scaling=shared.scaling;
		outside_world->dx=shared.dx;
		outside_world->dy=shared.dy;
	}
}

Outside_World::WindowInterface *FsSimpleWindowConnection::CreateWindowInterface(void) const
{
	return new WindowConnection;
}
void FsSimpleWindowConnection::DeleteWindowInterface(Outside_World::WindowInterface *PTR) const
{
	auto ptr=dynamic_cast<WindowConnection*>(PTR);
	if(nullptr!=ptr)
	{
		delete ptr;
	}
}

void FsSimpleWindowConnection::WindowConnection::PollGamePads(void)
{
	for(auto padId : winThr.gamePadsNeedUpdate)
	{
		if(padId<winThrEx.primary.gamePads.size())
		{
			YsGamePadRead(&winThrEx.primary.gamePads[padId],padId);
		}
	}
}

void FsSimpleWindowConnection::WindowConnection::UpdateStatusBitmap(void)
{
	// Update Status Bitmap
	if(winThrEx.prevStatusBarInfo.cdAccessLamp!=sharedEx.statusBarInfo.cdAccessLamp)
	{
		Put16x16SelectInvert(0,15,CD_IDLE,CD_BUSY,sharedEx.statusBarInfo.cdAccessLamp);
	}
	for(int fd=0; fd<2; ++fd)
	{
		if(winThrEx.prevStatusBarInfo.fdAccessLamp[fd]!=sharedEx.statusBarInfo.fdAccessLamp[fd])
		{
			Put16x16SelectInvert(16+16*fd,15,FD_IDLE,FD_BUSY,sharedEx.statusBarInfo.fdAccessLamp[fd]);
		}
	}
	for(int hdd=0; hdd<6; ++hdd)
	{
		if(winThrEx.prevStatusBarInfo.scsiAccessLamp[hdd]!=sharedEx.statusBarInfo.scsiAccessLamp[hdd])
		{
			Put16x16SelectInvert(48+16*hdd,15,HDD_IDLE,HDD_BUSY,sharedEx.statusBarInfo.scsiAccessLamp[hdd]);
		}
	}
	winThrEx.prevStatusBarInfo=sharedEx.statusBarInfo;
}

GLuint FsSimpleWindowConnection::WindowConnection::GenTexture(void)
{
	GLuint texId;

	glGenTextures(1,&texId);  // Reserve one texture identifier
	glBindTexture(GL_TEXTURE_2D,texId);  // Making the texture identifier current (or bring it to the deck)

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	return texId;
}

void FsSimpleWindowConnection::WindowConnection::UpdateTexture(GLuint texId,int wid,int hei,const unsigned char *rgba) const
{
	glBindTexture(GL_TEXTURE_2D,texId);
	glTexImage2D
	    (GL_TEXTURE_2D,
	     0,
	     GL_RGBA,
	     wid,
	     hei,
	     0,
	     GL_RGBA,
	     GL_UNSIGNED_BYTE,
	     rgba);
}
void FsSimpleWindowConnection::WindowConnection::DrawTextureRect(int x0,int y0,int x1,int y1) const
{
	glBegin(GL_QUADS);

	glTexCoord2f(0,0);
	glVertex2i(x0,y1);

	glTexCoord2f(1,0);
	glVertex2i(x1,y1);

	glTexCoord2f(1,1);
	glVertex2i(x1,y0);

	glTexCoord2f(0,1);
	glVertex2i(x0,y0);

	glEnd();
}

////////////////////////////////////////////////////////////////

Outside_World::Sound *FsSimpleWindowConnection::CreateSound(void) const
{
	return new SoundConnection;
}

void FsSimpleWindowConnection::DeleteSound(Sound *PTR) const
{
	auto ptr=dynamic_cast<SoundConnection *>(PTR);
	if(nullptr!=ptr)
	{
		delete ptr;
	}
}

void FsSimpleWindowConnection::SoundConnection::Start(void)
{
	soundPlayer.Start();
	cddaStartHSG=0;
#ifdef AUDIO_USE_STREAMING
	YsSoundPlayer::StreamingOption FMPCMStreamOpt;
	FMPCMStreamOpt.ringBufferLengthMillisec=TownsSound::FM_PCM_MILLISEC_PER_WAVE*2+TownsSound::WAVE_STREAMING_SAFETY_BUFFER;
	soundPlayer.StartStreaming(FMPCMStream,FMPCMStreamOpt);
#endif
}
void FsSimpleWindowConnection::SoundConnection::Stop(void)
{
	soundPlayer.End();
}

void FsSimpleWindowConnection::SoundConnection::Polling(void)
{
	soundPlayer.KeepPlaying();
}

void FsSimpleWindowConnection::SoundConnection::CDDAPlay(const DiscImage &discImg,DiscImage::MinSecFrm from,DiscImage::MinSecFrm to,bool repeat,unsigned int,unsigned int)
{
	auto wave=discImg.GetWave(from,to);
	cddaChannel.CreateFromSigned16bitStereo(44100,wave);
	if(true==repeat)
	{
		soundPlayer.PlayBackground(cddaChannel);
	}
	else
	{
		soundPlayer.PlayOneShot(cddaChannel);
	}
	cddaStartHSG=from.ToHSG();
}
void FsSimpleWindowConnection::SoundConnection::CDDASetVolume(float leftVol,float rightVol)
{
	soundPlayer.SetVolumeLR(cddaChannel,leftVol,rightVol);
}
void FsSimpleWindowConnection::SoundConnection::CDDAStop(void)
{
	soundPlayer.Stop(cddaChannel);
}
void FsSimpleWindowConnection::SoundConnection::CDDAPause(void)
{
	soundPlayer.Pause(cddaChannel);
}
void FsSimpleWindowConnection::SoundConnection::CDDAResume(void)
{
	soundPlayer.Resume(cddaChannel);
}
bool FsSimpleWindowConnection::SoundConnection::CDDAIsPlaying(void)
{
	return (YSTRUE==soundPlayer.IsPlaying(cddaChannel));
}
DiscImage::MinSecFrm FsSimpleWindowConnection::SoundConnection::CDDACurrentPosition(void)
{
	double sec=soundPlayer.GetCurrentPosition(cddaChannel);
	unsigned long long secHSG=(unsigned long long)(sec*75.0);
	unsigned long long posInDisc=secHSG+cddaStartHSG;

	DiscImage::MinSecFrm msf;
	msf.FromHSG(posInDisc);
	return msf;
}

void FsSimpleWindowConnection::SoundConnection::FMPCMPlay(std::vector <unsigned char> &wave)
{
#ifdef AUDIO_USE_STREAMING
	YsSoundPlayer::SoundData nextWave;
	nextWave.CreateFromSigned16bitStereo(YM2612::WAVE_SAMPLING_RATE,wave);
	soundPlayer.AddNextStreamingSegment(FMPCMStream,nextWave);
#else
	FMPCMChannel.CreateFromSigned16bitStereo(YM2612::WAVE_SAMPLING_RATE,wave);
	soundPlayer.PlayOneShot(FMPCMChannel);
#endif
}
void FsSimpleWindowConnection::SoundConnection::FMPCMPlayStop(void)
{
}
bool FsSimpleWindowConnection::SoundConnection::FMPCMChannelPlaying(void)
{
#ifdef AUDIO_USE_STREAMING
	unsigned int numSamples=(TownsSound::FM_PCM_MILLISEC_PER_WAVE*YM2612::WAVE_SAMPLING_RATE+999)/1000;
	return YSTRUE!=soundPlayer.StreamPlayerReadyToAcceptNextNumSample(FMPCMStream,numSamples);
#else
	return YSTRUE==soundPlayer.IsPlaying(FMPCMChannel);
#endif
}

void FsSimpleWindowConnection::SoundConnection::BeepPlay(int samplingRate, std::vector<unsigned char> &wave) {
	BeepChannel.CreateFromSigned16bitStereo(samplingRate, wave);
	soundPlayer.PlayOneShot(BeepChannel);
}

void FsSimpleWindowConnection::SoundConnection::BeepPlayStop() {
	soundPlayer.Stop(BeepChannel);
}

bool FsSimpleWindowConnection::SoundConnection::BeepChannelPlaying() const {
	return YSTRUE == soundPlayer.IsPlaying(BeepChannel);
}
