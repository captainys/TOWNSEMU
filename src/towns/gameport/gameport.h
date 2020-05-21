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
		MOUSE,
		GAMEPAD,
		CYBERSTICK,  // Hope for the future versions.
	};

	class Port
	{
	public:
		int device;  // MOUSE, GAMEPAD, or CYBERSTICK
		int state;
		bool COM;
		bool button[2],left,right,up,down,run,pause;
		Vec2i mouseMotion;
		long long int lastReadTime;

		void Write(bool COM,bool T1,bool T2);
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
};

/* } */
#endif
