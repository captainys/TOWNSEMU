/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef YM2612_IS_INCLUDED
#define YM2612_IS_INCLUDED
/* { */

#include <vector>
#include <string>


/*! G** D*** I*!  I didn't realize data sheet of YM2612 is not available today!
*/
class YM2612
{
public:
	// [2] pp. 200  Calculation of timer.  Intenral clock is 600KHz 1tick=(1/600K)sec=1667ns.
	// [2] pp. 201:
	// Timer A takes (12*(1024-NATick))/(600)ms to count up.  NATick is 10-bit counter.
	// Timer B takes (192*(256-NBTick))/(600)ms to count up.  NBTick is 8-bit counter.
	// NATick counts up every 12 internal-clock ticks.
	// NBTick counts up every 192 internal-clock ticks.

	#define SINETABLE \
		static const char sineTable[PHASE_STEPS]= { \
			+0 ,+3 ,+6 ,+9 ,+12,+15,+17,+20,+22,+24,+26,+28,+29,+30,+31,+31, \
			+32,+31,+31,+30,+29,+28,+26,+24,+22,+20,+17,+15,+12,+9 ,+6 ,+3 , \
			+0 ,-3 ,-6 ,-9 ,-12,-15,-17,-20,-22,-24,-26,-28,-29,-30,-31,-31, \
			-32,-31,-31,-30,-29,-28,-26,-24,-22,-20,-17,-15,-12,-9 ,-6 ,-3 , };

	enum
	{
		NUM_SLOTS=4,
		NUM_CHANNELS=6,

		PHASE_STEPS=64,
		PHASE_MASK=63,
		UNSCALED_MAX=32,

		WAVE_SAMPLING_RATE=22050,
	};

	enum
	{
		REG_TIMER_A_COUNT_HIGH=0x24,
		REG_TIMER_A_COUNT_LOW=0x25,
		REG_TIMER_B_COUNT=0x26,
		REG_TIMER_CONTROL=0x27,
		REG_KEY_ON_OFF=0x28,
	};

	enum
	{
		TICK_DURATION_IN_NS=1667,
		TIMER_A_PER_TICK=12,
		TIMER_B_PER_TICK=192,
		NTICK_TIMER_A=1024*TIMER_A_PER_TICK,
		NTICK_TIMER_B= 256*TIMER_B_PER_TICK,
	};

	class Slot
	{
	public:
		unsigned int DT,MULTI;
		unsigned int TL;
		unsigned int KS,AR;
		unsigned int AM;
		unsigned int DR;
		unsigned int SR;
		unsigned int SL,RR;
		unsigned int SSG_EG;

		void Clear(void);

		inline char UnscaledOutput(int phase) const
		{
			SINETABLE;
			return sineTable[phase&PHASE_MASK];
		}
		inline char UnscaledOutput(int phase,unsigned int FB) const
		{
			SINETABLE;
			if(0==FB)
			{
				return sineTable[phase&PHASE_MASK];
			}
			else
			{
				static const unsigned int rShift[8]={0,4,3,2,1,0,0,0};
				static const unsigned int lShift[8]={0,0,0,0,0,0,1,2};

				short o=sineTable[phase&PHASE_MASK];
				short sign=(o&0xFF00);

				// FB must be 0 to 7
				o=(((o>>rShift[FB])|sign)<<lShift[FB]);
				return sineTable[(phase+o)&PHASE_MASK];
			}
		}
	};
	class Channel
	{
	public:
		unsigned int F_NUM,BLOCK;
		unsigned int FB,CONNECT;
		unsigned int L,R,AMS,PMS;
		unsigned int usingSlot;
		Slot slots[NUM_SLOTS];

		void Clear();
	};

	class State
	{
	public:
		unsigned long long int deviceTimeInNS;
		unsigned long long int lastTickTimeInNS;
		Channel channels[NUM_CHANNELS];
		unsigned int F_NUM_3CH[3],BLOCK_3CH[3];
		unsigned char reg[256];  // I guess only 0x21 to 0xB6 are used.
		unsigned long long int timerCounter[2];
		bool timerUp[2];

		void PowerOn(void);
		void Reset(void);
	};

	State state;

	unsigned int TLtoDB100[128];   // 100 times dB
	unsigned int SLtoDB100[16];    // 100 times dB
	unsigned char DBto127Scale[97]; // dB to 0 to 127 scale

	YM2612();
	~YM2612();
private:
	void MakeTLtoDB100(void);
	void MakeSLtoDB100(void);
	void MakeDBto127Scale(void);
public:
	void PowerOn(void);
	void Reset(void);

	void WriteRegister(unsigned int channelBase,unsigned int reg,unsigned int value);
	unsigned int ReadRegister(unsigned int channelBase,unsigned int reg) const;

	void Run(unsigned long long int systemTimeInNS);

	bool TimerAUp(void) const;
	bool TimerBUp(void) const;

	/*! Returns timer-up state of 
	*/
	bool TimerUp(unsigned int timerId) const;

	/*!
	*/
	inline static unsigned int KSToRate(unsigned int KS,unsigned int BLOCK,unsigned int NOTE);

	/*!
	*/
	std::vector <unsigned char> MakeWave(unsigned int ch) const;

	std::vector <std::string> GetStatusText(void) const;
};


/* } */
#endif
