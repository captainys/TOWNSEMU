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


FsSimpleWindowConnection::FsSimpleWindowConnection()
{
	FSKEYtoTownsKEY=new unsigned int [FSKEY_NUM_KEYCODE];
	FSKEYState=new unsigned int [FSKEY_NUM_KEYCODE];

	SetKeyboardMode(KEYBOARD_MODE_DIRECT);
	SetKeyboardLayout(KEYBOARD_LAYOUT_US);

	for(int i=0; i<FSKEY_NUM_KEYCODE; ++i)
	{
		FSKEYState[i]=0;
	}

	if(true!=gamePadInitialized)
	{
		YsGamePadInitialize();
		gamePadInitialized=true;
	}

	auto nGameDevs=YsGamePadGetNumDevices();
	if(0<nGameDevs)
	{
		gamePads.resize(nGameDevs);
		prevGamePads.resize(nGameDevs);
		for(unsigned int i=0; i<nGameDevs; ++i)
		{
			YsGamePadRead(&gamePads[i],i);
			prevGamePads[i]=gamePads[i];
		}
	}
}
FsSimpleWindowConnection::~FsSimpleWindowConnection()
{
	delete [] FSKEYtoTownsKEY;
	delete [] FSKEYState;
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
	int wid=640*scaling/100;
	int hei=480*scaling/100;

	int winY0=0;
	if(true==windowShift)
	{
		winY0=48;
	}

	FsOpenWindow(0,winY0,wid,hei+STATUS_HEI,1);
	this->winWid=640;
	this->winHei=480;
	FsSetWindowTitle("FM Towns Emulator - TSUGARU");
	soundPlayer.Start();
	cddaStartHSG=0;

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
}
/* virtual */ void FsSimpleWindowConnection::Stop(void)
{
	soundPlayer.End();
}
/* virtual */ void FsSimpleWindowConnection::DevicePolling(class FMTowns &towns)
{
	FsPollDevice();
	PollGamePads();

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

	// For the time translation mode only.
	// if(true==keyTranslationMode)
	if(KEYBOARD_MODE_TRANSLATION==keyboardMode)
	{
		unsigned int c;
		while(0!=(c=FsInkeyChar()))
		{
			if(0==FsGetKeyState(FSKEY_CTRL))
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
		while(0!=(c=FsInkey()))
		{
			if(PAUSE_KEY_CODE==c)
			{
				this->pauseKey=true;
			}

			this->ProcessInkey(towns,FSKEYtoTownsKEY[c]);
			unsigned char byteData[2]={0,0};
			switch(c)
			{
			default:
				byteData[1]=0;
				break;
			// CTRL+C, CTRL+S, CTRL+Q
			case FSKEY_C:
			case FSKEY_S:
			case FSKEY_Q:
				if(0!=FsGetKeyState(FSKEY_CTRL))
				{
					byteData[0]=TOWNS_KEYFLAG_CTRL;
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  FSKEYtoTownsKEY[c]);
					towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,FSKEYtoTownsKEY[c]);
				}
				break;
			case FSKEY_ENTER:
			case FSKEY_BS:
			case FSKEY_TAB:
			case FSKEY_ESC:
			case FSKEY_UP:
			case FSKEY_DOWN:
			case FSKEY_LEFT:
			case FSKEY_RIGHT:
			case FSKEY_HOME:
			case FSKEY_PAGEUP:
			case FSKEY_PAGEDOWN:
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
				byteData[0]|=(0!=FsGetKeyState(FSKEY_CTRL) ? TOWNS_KEYFLAG_CTRL : 0);
				byteData[0]|=(0!=FsGetKeyState(FSKEY_SHIFT) ? TOWNS_KEYFLAG_SHIFT : 0);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_PRESS,  FSKEYtoTownsKEY[c]);
				towns.keyboard.PushFifo(byteData[0]|TOWNS_KEYFLAG_JIS_RELEASE,FSKEYtoTownsKEY[c]);
				break;
			}
			if(0!=byteData[1])
			{
				byteData[0]=TOWNS_KEYFLAG_JIS_PRESS;
				towns.keyboard.PushFifo(byteData[0],byteData[1]);
				byteData[0]=TOWNS_KEYFLAG_JIS_RELEASE;
				towns.keyboard.PushFifo(byteData[0],byteData[1]);
			}
		}
	}
	else
	{
		unsigned int c;
		while(0!=(c=FsInkey()))
		{
			unsigned char byteData=0;
			this->ProcessInkey(towns,FSKEYtoTownsKEY[c]);
			if(PAUSE_KEY_CODE==c)
			{
				this->pauseKey=true;
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

				byteData|=(0!=FsGetKeyState(FSKEY_CTRL) ? TOWNS_KEYFLAG_CTRL : 0);
				byteData|=(0!=FsGetKeyState(FSKEY_SHIFT) ? TOWNS_KEYFLAG_SHIFT : 0);
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
			auto sta=FsGetKeyState(key);
			if(0!=FSKEYtoTownsKEY[key] && 0!=FSKEYState[key] && 0==sta)
			{
				byteData|=(0!=FsGetKeyState(FSKEY_CTRL) ? TOWNS_KEYFLAG_CTRL : 0);
				byteData|=(0!=FsGetKeyState(FSKEY_SHIFT) ? TOWNS_KEYFLAG_SHIFT : 0);
				byteData|=TOWNS_KEYFLAG_JIS_RELEASE;
				towns.keyboard.PushFifo(byteData,FSKEYtoTownsKEY[key]);
			}
			// See comment above regarding the timing of FsGetKeyState and FsInkey.
			if(0==sta)
			{
				FSKEYState[key]=0;
			}
		}
		while(0!=FsInkeyChar())
		{
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
					bool Abutton=(0!=FsGetKeyState(FSKEY_Z));
					bool Bbutton=(0!=FsGetKeyState(FSKEY_X));
					bool run=(0!=FsGetKeyState(FSKEY_A));
					bool pause=(0!=FsGetKeyState(FSKEY_S));
					bool left=(0!=FsGetKeyState(FSKEY_LEFT));
					bool right=(0!=FsGetKeyState(FSKEY_RIGHT));
					if(true==left && true==right)
					{
						right=false;
					}
					bool up=(0!=FsGetKeyState(FSKEY_UP));
					bool down=(0!=FsGetKeyState(FSKEY_DOWN));
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
				if(true!=gamePadInitialized)
				{
					YsGamePadInitialize();
					gamePadInitialized=true;
				}
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
				if(true!=gamePadInitialized)
				{
					YsGamePadInitialize();
					gamePadInitialized=true;
				}
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
					if(TOWNS_GAMEPORTEMU_MOUSE_BY_KEY!=gamePort[portId] &&
					   TOWNS_GAMEPORTEMU_MOUSE_BY_NUMPAD!=gamePort[portId] &&
					   true!=gamePadInitialized)
					{
						YsGamePadInitialize();
						gamePadInitialized=true;
					}
					{
						const int accel=1;
						const int maxSpeed=80;
						const int div=20;

						bool upDownLeftRight[4]={false,false,false,false};
						bool button[2]={false,false};

						mouseEmulationByAnalogAxis=true;
						if(TOWNS_GAMEPORTEMU_MOUSE_BY_KEY==gamePort[portId])
						{
							upDownLeftRight[0]=(0!=FsGetKeyState(FSKEY_UP));
							upDownLeftRight[1]=(0!=FsGetKeyState(FSKEY_DOWN));
							upDownLeftRight[2]=(0!=FsGetKeyState(FSKEY_LEFT));
							upDownLeftRight[3]=(0!=FsGetKeyState(FSKEY_RIGHT));
							button[0]=(0!=FsGetKeyState(FSKEY_Z));
							button[1]=(0!=FsGetKeyState(FSKEY_X));
						}
						else if(TOWNS_GAMEPORTEMU_MOUSE_BY_NUMPAD==gamePort[portId])
						{
							upDownLeftRight[0]=(0!=FsGetKeyState(FSKEY_TEN7) || 0!=FsGetKeyState(FSKEY_TEN8) || 0!=FsGetKeyState(FSKEY_TEN9));
							upDownLeftRight[1]=(0!=FsGetKeyState(FSKEY_TEN1) || 0!=FsGetKeyState(FSKEY_TEN2) || 0!=FsGetKeyState(FSKEY_TEN3));
							upDownLeftRight[2]=(0!=FsGetKeyState(FSKEY_TEN1) || 0!=FsGetKeyState(FSKEY_TEN4) || 0!=FsGetKeyState(FSKEY_TEN7));
							upDownLeftRight[3]=(0!=FsGetKeyState(FSKEY_TEN3) || 0!=FsGetKeyState(FSKEY_TEN6) || 0!=FsGetKeyState(FSKEY_TEN9));
							button[0]=(0!=FsGetKeyState(FSKEY_TENSLASH));
							button[1]=(0!=FsGetKeyState(FSKEY_TENSTAR));
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
					if(true!=gamePadInitialized)
					{
						YsGamePadInitialize();
						gamePadInitialized=true;
					}
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
			}
		}

		if(mouseEmulationByAnalogAxis!=true)
		{
			struct YsGamePadReading reading;
			int lb,mb,rb,mx,my;
			FsGetMouseEvent(lb,mb,rb,mx,my);
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

			if(true==mouseByFlightstickEnabled)
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
				if(mx<0)
				{
					mx=0;
				}
				if(my<0)
				{
					my=0;
				}
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
				mx=mx*100/scaling;
				my=my*100/scaling;
			}

			this->ProcessMouse(towns,lb,mb,rb,mx,my);
		}
	}
}
void FsSimpleWindowConnection::PollGamePads(void)
{
	if(true!=Outside_World::gameDevsNeedUpdateCached)
	{
		std::cout << "Squawk!  Game Devices that need updates not cached!" << std::endl;
	}
	for(auto padId : Outside_World::gamePadsNeedUpdate)
	{
		if(padId<gamePads.size())
		{
			prevGamePads[padId]=gamePads[padId];
			YsGamePadRead(&gamePads[padId],padId);
		}
	}
}
/* virtual */ void FsSimpleWindowConnection::UpdateStatusBitmap(class FMTowns &towns)
{
	// Update Status Bitmap
	{
		bool busy=(true!=towns.cdrom.state.DRY);
		if(cdAccessLamp!=busy)
		{
			Put16x16SelectInvert(0,15,CD_IDLE,CD_BUSY,busy);
			cdAccessLamp=busy;
		}
	}
	for(int fd=0; fd<2; ++fd)
	{
		bool busy=(fd==towns.fdc.DriveSelect() && true==towns.fdc.state.busy);
		if(fdAccessLamp[fd]!=busy)
		{
			Put16x16SelectInvert(16+16*fd,15,FD_IDLE,FD_BUSY,busy);
			fdAccessLamp[fd]=busy;
		}
	}
	for(int hdd=0; hdd<6; ++hdd)
	{
		bool busy=(hdd==towns.scsi.state.selId && true==towns.scsi.state.BUSY);
		if(scsiAccessLamp[hdd]!=busy)
		{
			Put16x16SelectInvert(48+16*hdd,15,HDD_IDLE,HDD_BUSY,busy);
			scsiAccessLamp[hdd]=busy;
		}
	}

}
/* virtual */ void FsSimpleWindowConnection::Render(const TownsRender::Image &img)
{
	if(this->winWid!=img.wid)  // Height is not correct yet   :-P
	{
		this->winWid=img.wid;
		sinceLastResize=10;
	}
	else if(0<sinceLastResize)
	{
		--sinceLastResize;
		if(0==sinceLastResize)
		{
			FsResizeWindow(this->winWid*scaling/100,this->winHei*scaling/100+STATUS_HEI);
		}
	}

	std::vector <unsigned char> flip;
	flip.resize(img.wid*img.hei*4);

	auto upsideDown=img.rgba;
	auto rightSideUp=flip.data()+(img.hei-1)*img.wid*4;
	for(unsigned int y=0; y<img.hei; ++y)
	{
		memcpy(rightSideUp,upsideDown,img.wid*4);
		upsideDown+=img.wid*4;
		rightSideUp-=img.wid*4;
	}

	glClear(GL_COLOR_BUFFER_BIT);
	int wid,hei;
	FsGetWindowSize(wid,hei);
	glViewport(0,0,wid,hei);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f,float(wid),float(hei),0.0f,-1,1);

	glPixelZoom((float)scaling/100.0f,(float)scaling/100.0f);
	glRasterPos2i(0,(img.hei*scaling/100)-1);
	glDrawPixels(img.wid,img.hei,GL_RGBA,GL_UNSIGNED_BYTE,flip.data());

	glRasterPos2i(0,hei-1);
	glPixelZoom(1,1);
	glDrawPixels(STATUS_WID,STATUS_HEI,GL_RGBA,GL_UNSIGNED_BYTE,statusBitmap);

	FsSwapBuffers();
}

/* virtual */ void FsSimpleWindowConnection::SetKeyboardLayout(unsigned int layout)
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

/* virtual */ void FsSimpleWindowConnection::CDDAPlay(const DiscImage &discImg,DiscImage::MinSecFrm from,DiscImage::MinSecFrm to,bool repeat,unsigned int leftLevel,unsigned int rightLevel)
{
	if(256<leftLevel)
	{
		leftLevel=256;
	}
	if(256<rightLevel)
	{
		rightLevel=256;
	}
	auto wave=discImg.GetWave(from,to);
	if(leftLevel<256 || rightLevel<256)
	{
		for(long long int i=0; i+3<wave.size(); i+=4)
		{
			int left= cpputil::GetWord(wave.data()+i);
			int right=cpputil::GetWord(wave.data()+i+2);;
			left= (left &0x7FFF)-(left &0x8000);
			right=(right&0x7FFF)-(right&0x8000);
			left= left *leftLevel /256;
			right=right*rightLevel/256;
			cpputil::PutWord(wave.data()+i  ,left);
			cpputil::PutWord(wave.data()+i+2,right);
		}
	}
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
/* virtual */ void FsSimpleWindowConnection::CDDAStop(void)
{
	soundPlayer.Stop(cddaChannel);
}
/* virtual */ void FsSimpleWindowConnection::CDDAPause(void)
{
	soundPlayer.Pause(cddaChannel);
}
/* virtual */ void FsSimpleWindowConnection::CDDAResume(void)
{
	soundPlayer.Resume(cddaChannel);
}
/* virtual */ bool FsSimpleWindowConnection::CDDAIsPlaying(void)
{
	return (YSTRUE==soundPlayer.IsPlaying(cddaChannel));
}
/* virtual */ DiscImage::MinSecFrm FsSimpleWindowConnection::CDDACurrentPosition(void)
{
	double sec=soundPlayer.GetCurrentPosition(cddaChannel);
	unsigned long long secHSG=(unsigned long long)(sec*75.0);
	unsigned long long posInDisc=secHSG+cddaStartHSG;

	DiscImage::MinSecFrm msf;
	msf.FromHSG(posInDisc);
	return msf;
}

/* virtual */ void FsSimpleWindowConnection::PCMPlay(std::vector <unsigned char > &wave)
{
	PCMChannel.CreateFromSigned16bitStereo(RF5C68::FREQ,wave);
	soundPlayer.PlayOneShot(PCMChannel);
}
/* virtual */ void FsSimpleWindowConnection::PCMPlayStop(void)
{
	soundPlayer.Stop(PCMChannel);
}
/* virtual */ bool FsSimpleWindowConnection::PCMChannelPlaying(void)
{
	return YSTRUE==soundPlayer.IsPlaying(PCMChannel);
}

/* virtual */ void FsSimpleWindowConnection::FMPlay(std::vector <unsigned char> &wave)
{
	FMChannel.CreateFromSigned16bitStereo(YM2612::WAVE_SAMPLING_RATE,wave);

	// std::string fName;
	// fName="tone";
	// fName+=cpputil::Itoa(ch);
	// fName+=".wav";
	// auto waveFile=FMChannel[ch].MakeWavByteData();
	// cpputil::WriteBinaryFile(fName,waveFile.size(),waveFile.data());

	soundPlayer.PlayOneShot(FMChannel);
}
/* virtual */ void FsSimpleWindowConnection::FMPlayStop(void)
{
}
/* virtual */ bool FsSimpleWindowConnection::FMChannelPlaying(void)
{
	return YSTRUE==soundPlayer.IsPlaying(FMChannel);
}

