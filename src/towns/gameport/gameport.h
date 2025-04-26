/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef GAMEPORT_IS_INCLUDED
#define GAMEPORT_IS_INCLUDED
/* { */

#include "cheapmath.h"
#include "townsdef.h"

class TownsGamePort : public Device
{
private:
	class FMTownsCommon *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "GAMEPORT";}

	enum
	{
		NONE,
		MOUSE,
		GAMEPAD,
		CYBERSTICK,
		CAPCOMCPSF,
		GAMEPAD_6BTN,
		LIBBLERABBLE,
		MOUSEREAD_RESET_TIMEOUT=1000000, // 1000000ns=1ms
		CYBERSTICK_TIMEOUT=2000000,      // 2000000ns=2ms
		CYBERSTICK_READ_INTERVAL=3500,   //    3500ns=3.5us
		CYBERSTICK_BOOT_IDLE_TIME=1000000000, // Don't activate for 1 second to prevent boot menu.
	};

	enum
	{
		MAX_NUM_BUTTONS=32
	};

	enum
	{
		MOUSESTATE_XHIGH,
		MOUSESTATE_XLOW,
		MOUSESTATE_YHIGH,
		MOUSESTATE_YLOW,

		CYBERSTICK_00A,
		CYBERSTICK_00B, // 
		CYBERSTICK_01A, // I don't remember interval, but I guess 0.01ms separation is good.
		CYBERSTICK_01B,
		CYBERSTICK_02A,
		CYBERSTICK_02B,
		CYBERSTICK_03A,
		CYBERSTICK_03B,
		CYBERSTICK_04A,
		CYBERSTICK_04B,
		CYBERSTICK_05A,
		CYBERSTICK_05B,
		CYBERSTICK_06A,
		CYBERSTICK_06B,
		CYBERSTICK_07A,
		CYBERSTICK_07B,
		CYBERSTICK_08A,
		CYBERSTICK_08B,
		CYBERSTICK_09A,
		CYBERSTICK_09B,
		CYBERSTICK_10A,
		CYBERSTICK_10B,
		CYBERSTICK_11A,
		CYBERSTICK_11B,

	NUM_MOUSESTATE,
	};

	class Port
	{
	public:
		int device;  // MOUSE, GAMEPAD, or CYBERSTICK
		int state;
		bool COM=false;       // COM from CPU.
		unsigned char TRIG=3; // TRIG from CPU.
		bool button[2],left,right,up,down,run,pause;

		Vec2i mouseMotion;  // For CyberStick, used as XY position.
		int zAxis=0; // For CyberStick
		int wAxis=0; // For CyberStick
		int trig=0;  // For CyberStick and CAPCOM CPSF

		// Once CPU starts reading the mouse, mouseMotion should stay the same.
		// If the value changes after 4-bits of the motion x or y is read before
		// the rest 4-bits, it would mess up the pointer.
		// To prevent this situation, mouseMotion is copied to mouseMotionCopy
		// when the first 4-bit of DX is sent to the CPU.
		Vec2i mouseMotionCopy;
		int zAxisCopy; // For CyberStick
		int wAxisCopy; // For CyberStick
		bool monitorMouse=false;

		long long int lastAccessTime;
		long long int lastStateChangeTime;


		// Used in Read() >>
		bool lastButtonRead[2]={false,false};
		long long int lastButtonDownTime[2]={0,0};
		long long int maxButtonHoldTime[2]={0,0};
		// Used in Read() <<


		void Write(long long int townsTime,bool COM,unsigned char TRIG);
		unsigned char Read(long long int townsTime); // Reading last coordinate should reset motion.  Not a const.

		void SetGamePadState(bool Abutton,bool Bbutton,bool left,bool right,bool up,bool down,bool run,bool pause,long long int townsTime);
		void SetCyberStickState(int x,int y,int z,int w,unsigned int trig,long long int townsTime);
		void SetCAPCOMCPSFState(bool left,bool right,bool up,bool down,bool A,bool B,bool X,bool Y,bool L,bool R, bool start,bool select,long long int townsTime);

		void SetAutoShotTimer(unsigned int prevTrig,unsigned int trig,long long int townsTime);

		// Don't have to save state >>
		// BaseTime is set in Set***State when the button is pressed.
		long long int autoShotInterval[MAX_NUM_BUTTONS];  // 0->No auto shot
		long long int autoShotBaseTime[MAX_NUM_BUTTONS];
		// Don't have to save state <<
	};

	class State
	{
	public:
		unsigned int bootKeyComb=BOOT_KEYCOMB_NONE;

		Port ports[2];
		void PowerOn(void);
		void Reset(void);
	};

	State state;

	static unsigned int EmulationTypeToDeviceType(unsigned int emulationType);

	TownsGamePort(class FMTownsCommon *townsPtr);

	void BootSequenceStarted(void);
	void SetBootKeyCombination(unsigned int keyComb);

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};

/* } */
#endif
