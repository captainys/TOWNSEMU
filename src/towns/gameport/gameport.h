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
	class FMTowns *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "GAMEPORT";}

	enum
	{
		NONE,
		MOUSE,
		GAMEPAD,
		CYBERSTICK,  // Hope for the future versions.
		MOUSEREAD_RESET_TIMEOUT=1000000,
	};

	enum
	{
		MOUSESTATE_XHIGH,
		MOUSESTATE_XLOW,
		MOUSESTATE_YHIGH,
		MOUSESTATE_YLOW,

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
		Vec2i mouseMotion;

		// Once CPU starts reading the mouse, mouseMotion should stay the same.
		// If the value changes after 4-bits of the motion x or y is read before
		// the rest 4-bits, it would mess up the pointer.
		// To prevent this situation, mouseMotion is copied to mouseMotionCopy
		// when the first 4-bit of DX is sent to the CPU.
		Vec2i mouseMotionCopy;

		long long int lastAccessTime;

		void Write(long long int townsTime,bool COM,unsigned char TRIG);
		unsigned char Read(long long int townsTime); // Reading last coordinate should reset motion.  Not a const.
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

	TownsGamePort(class FMTowns *townsPtr);

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
