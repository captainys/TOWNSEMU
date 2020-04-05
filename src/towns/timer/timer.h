/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef TIMER_IS_INCLUDED
#define TIMER_IS_INCLUDED
/* { */


#include <vector>
#include <string>

#include "device.h"
#include "townsdef.h"



class TownsTimer : public Device
{
private:
	class FMTowns *townsPtr;
	class TownsPIC *picPtr;
public:
	virtual const char *DeviceName(void) const{return "INTERVALTIMER";}

	enum
	{
		NUM_CHANNELS=8,        // Actually 3 channels, but I want to use (channel&7) rather than (channel%6).
		NUM_CHANNELS_ACTUAL=6,
		TICK_INTERVAL=3257,   // 3257nano-seconds
	};

	class State
	{
	public:
		class Channel
		{
		public:
			unsigned char mode,lastCmd;
			unsigned short counter,counterInitialValue;
			unsigned short latchedCounter;
			unsigned short increment;
			bool OUT;
			mutable bool latched,bcd;

			unsigned int RL;
			bool accessLow; // true->access low byte  false->access high byte
		};
		unsigned long long int lastTickTimeInNS;
		Channel channels[NUM_CHANNELS];


		bool TMMSK[2];  // Only Channels 0 and 1.
		bool TMOUT[2];
		bool SOUND;

		void PowerOn(void);
		void Reset(void);

		void Latch(unsigned int ch);
		unsigned short ReadLatchedCounter(unsigned int ch) const;
		void SetChannelCounter(unsigned int ch,unsigned int value);
		unsigned int ReadChannelCounter(unsigned int ch); // accessLow may flip.  Not to be const.
		void ProcessControlCommand(unsigned int ch,unsigned int cmd);

		void TickIn(unsigned int nTick);
	};

	State state;

	TownsTimer(class FMTowns *townsPtr,class TownsPIC *picPtr);

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void RunScheduledTask(unsigned long long int townsTime);

	void UpdatePICRequest(void) const;

	std::vector <std::string> GetStatusText(void) const;
};

/* } */
#endif
