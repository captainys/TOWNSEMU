/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef KEYBOARD_IS_INCLUDED
#define KEYBOARD_IS_INCLUDED
/* { */

#include <string>
#include "device.h"
#include "townsdef.h"

class TownsKeyboard : public Device
{
public:
	enum
	{
		KEY_REPEAT_INTERVAL=833333, // 1000000000/1200 nano secounds
	};

	class State
	{
	public:
		bool IRQEnabled;
		bool KBINT;       // Bit 0 of I/O 604H

		unsigned int bootKeyComb=BOOT_KEYCOMB_NONE;
		unsigned int bootKeyCombSequenceCounter=0;

		unsigned char lastCmd=0;

		void Reset(void);
	};

	struct TownsKeyCombination
	{
		bool shift,ctrl;
		unsigned int keyCode;
	};

	State state;

	enum
	{
		FIFO_BUF_LEN=32
	};
	unsigned int nFifoFilled;
	unsigned char fifoBuf[FIFO_BUF_LEN];
	std::string autoType;

	bool debugBreakOnReturnKey=false;

	class FMTownsCommon *townsPtr;
	class TownsPIC *picPtr;

	void SetAutoType(std::string str);
	void PushFifo(unsigned char code1,unsigned char code2); // Always push a pair.

	void BootSequenceStarted(void);
	void SetBootKeyCombination(unsigned int keyComb);



	/*! Translates an ASCII code to a sequence of FM Towns Keyboard codes.
	    Returns the number of bytes of the generated key codes either 2 or 0.
	    For making it a key-release code, do keyCodeBuf[0]|=TOWNS_KEYFLAG_THUMBSHIFT_RELEASE.
	*/
	static unsigned int TranslateChar(unsigned char keyCodeBuf[2],unsigned char c);
	void TypeToFifo(const unsigned char keyCodeBuf[2]);
private:
	static TownsKeyCombination translation_data[256];

public:
	virtual const char *DeviceName(void) const{return "KEYBOARD";}
	TownsKeyboard(class FMTownsCommon *townsPtr,class TownsPIC *picPtr);
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void RunScheduledTask(unsigned long long int townsTime);

	virtual void Reset(void);

	bool InFifoBuffer(unsigned int code) const;

	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};

/* } */
#endif
