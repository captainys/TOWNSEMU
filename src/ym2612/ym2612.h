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



/*******************************************************************************

    YM2612 Emulator for Tsugaru:  Tsugaru-Ben

*******************************************************************************/



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

	enum
	{
		NUM_SLOTS=4,
		NUM_CHANNELS=6,

		// Sine table
		PHASE_STEPS=4096,      // 4096=360degrees
		PHASE_MASK=4095,
		UNSCALED_MAX=2048,
		SLOTOUT_TO_NPI=8,         // 1.0 output from an upstream slot is 8PI.

		SLOTFLAGS_ALL=0x0F,

		TONE_CHOPOFF_MILLISEC=4000,

		WAVE_SAMPLING_RATE=44100,
		WAVE_OUTPUT_AMPLITUDE_MAX_DEFAULT=8192,

		TL_MAX=127,
	};

	enum
	{
		REG_LFO=0x22,
		REG_TIMER_A_COUNT_HIGH=0x24,
		REG_TIMER_A_COUNT_LOW=0x25,
		REG_TIMER_B_COUNT=0x26,
		REG_TIMER_CONTROL=0x27,
		REG_KEY_ON_OFF=0x28,

		REG_DT_MULTI=0x30,
		REG_TL=0x40,
		REG_KS_AR=0x50,
		REG_AM_DR=0x60,
		REG_SR=0x70,
		REG_SL_RR=0x80,
		REG_SSG_EG=0x90,

		REG_FNUM1=0xA0,
		REG_FNUM2=0xA4,

		REG_FB_CNCT=0xB0,
		REG_LR_AMS_PMS=0xB4,
	};

	enum
	{
		// FM Towns Technical Databook tells internal clock frequency is 600KHz.
		// Which is 1667ns per clock.
		// However, actual measurement suggests it is 690KHz, which makes 1449ns per clock.
		TICK_DURATION_IN_NS=1449,
		TIMER_A_PER_TICK=12,
		TIMER_B_PER_TICK=192,
		NTICK_TIMER_A=1024*TIMER_A_PER_TICK,
		NTICK_TIMER_B= 256*TIMER_B_PER_TICK,
	};

	enum {
		CH_IDLE=0,
		CH_PLAYING=1,
		CH_RELEASE=2,
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

		// Cache for wave-generation >>
		mutable unsigned long long int microsecS12;  // Microsec from start of a tone by (microsec12>>12)
		uint64_t toneDurationMicrosecS12;   // In (microsec<<12).
		mutable unsigned int phaseS12;      // 5-bit phase=((phaseS12>>12)&0x1F)
		unsigned int phaseS12Step;  // Increment of phase12 per time step.
		unsigned int env[6];       // Envelope is in Db100 scale.  0 to 9600.  Time is (microsec>>10) (=microsecS12>>22)
		unsigned int envDurationCache; // in (microsec>>10)
		bool InReleasePhase;
		unsigned int ReleaseStartTime,ReleaseEndTime;
		unsigned int ReleaseStartDbX100;
		mutable unsigned int lastDbX100Cache;  // 0 to 9600 scale.
		// Cache for wave-generation <<

		void Clear(void);

		// phase is from the phaseGenerator*MULTI,DETUNE,PMS
		// phaseShift is input from the upstream slot.
		inline int UnscaledOutput(int phase,int phaseShift) const;
		inline int UnscaledOutput(int phase,int phaseShift,unsigned int FB,int lastSlot0Out) const;

		// Interpolate Envelope as Db.  Output is amplitude 4096 scale.
		// Time input is close to ms, but it is actually (microsec>>10).
		inline int EnvelopedOutputDbToAmpl(int phase,int phaseShift,unsigned int timeInMS,unsigned int FB,int lastSlot0Out) const;
		inline int EnvelopedOutputDbToAmpl(int phase,int phaseShift,unsigned int timeInMS) const;
		// DB scale: 0 to 9600
		inline int InterpolateEnvelope(unsigned int timeInMS) const;

		int DetuneContributionToPhaseStepS12(unsigned int BLOCK,unsigned int NOTE) const;
	};

	class Channel
	{
	public:
		unsigned int F_NUM,BLOCK;
		unsigned int FB,CONNECT;
		unsigned int L,R,AMS,PMS;
		unsigned int usingSlot;
		Slot slots[NUM_SLOTS];

		// Cache for wave-generation >>
		unsigned int playState;
		mutable int lastSlot0Out[2];
		// Cache for wave-generation <<

		void Clear();
		unsigned int Note(void) const;
		unsigned int KC(void) const;
	};

	class State
	{
	public:
		bool LFO;
		unsigned int FREQCTRL;
		unsigned long long int deviceTimeInNS;
		unsigned long long int lastTickTimeInNS;
		Channel channels[NUM_CHANNELS];
		unsigned int F_NUM_3CH[3],BLOCK_3CH[3];
		unsigned int F_NUM_6CH[3],BLOCK_6CH[3];
		unsigned char reg[256];  // I guess only 0x21 to 0xB6 are used.
		unsigned long long int timerCounter[2];
		bool timerUp[2];
		unsigned int playingCh; // Bit 0 to 5.

		int volume=WAVE_OUTPUT_AMPLITUDE_MAX_DEFAULT;

		void PowerOn(void);
		void Reset(void);
	};

	State state;
	bool channelMute[NUM_CHANNELS]={false,false,false,false,false,false};

	static unsigned int attackExp[4096];
	static unsigned int attackExpInverse[4096];

	static int sineTable[PHASE_STEPS];
	static unsigned int TLtoDB100[128];   // 100 times dB
	static unsigned int SLtoDB100[16];    // 100 times dB
	static unsigned int DB100to4095Scale[9601]; // dB to 0 to 4095 scale
	static unsigned int DB100from4095Scale[4096]; // 4095 scale to dB
	static unsigned int linear4096to9600[4097]; // Linear 4096 scale to 9600 scale
	static unsigned int linear9600to4096[9601]; // Linear 9600 scale to 4096 scale
	static const unsigned int connToOutChannel[8][4];
	static int MULTITable[16];

	struct ConnectionToOutputSlot
	{
		unsigned int nOutputSlots;
		int slots[4];
	};
	static const struct ConnectionToOutputSlot connectionToOutputSlots[8];


	class RegWriteLog
	{
	public:
		unsigned char chBase,reg,data;
		unsigned int count=1;
	};
	bool takeRegLog=false;
	std::vector <RegWriteLog> regWriteLog;


	YM2612();
	~YM2612();
private:
	void MakeSineTable(void);
	void MakeTLtoDB100(void);
	void MakeSLtoDB100(void);
	void MakeDB100to4095Scale(void);
	void MakeLinearScaleTable(void);
	void MakeAttackProfileTable(void);
public:
	void PowerOn(void);
	void Reset(void);

	/*! Writes to a register, and if a channel starts playing a tone, it calls KeyOn and returns between 0 to 5.
	    65535 otherwise.
	*/
	unsigned int WriteRegister(unsigned int channelBase,unsigned int reg,unsigned int value);
	unsigned int ReadRegister(unsigned int channelBase,unsigned int reg) const;

	void Run(unsigned long long int systemTimeInNS);

	bool TimerAUp(void) const;
	bool TimerBUp(void) const;

	/*! Returns timer-up state of 
	*/
	bool TimerUp(unsigned int timerId) const;

	/*! Cache parameters for calculating wave.
	*/
	void KeyOn(unsigned int ch,unsigned int slotFlags=SLOTFLAGS_ALL);

	/*! Update phase update (times 2^12) per step for slot.
	*/
	void UpdatePhase12StepSlot(Slot &slot,const unsigned int hertzX16,int detuneContribution);

	/*! Update phase update (times 2^12) per step for channel.
	*/
	void UpdatePhase12StepSlot(Channel &ch);

	/*! Sampling rate is defined by WAVE_SAMPLING_RATE.
	*/
	std::vector <unsigned char> MakeWaveAllChannels(unsigned long long int millisec) const;

	/*! For debugging purpose.  Make wave for a specific channel.
	*/
	std::vector <unsigned char> MakeWave(unsigned int ch,unsigned long long int millisec) const;

public:
	/*! Adds a wave to the buffer, and returns the number of samples (number_of_bytes_filled/4).
	    Sampling rate is defined by WAVE_SAMPLING_RATE.
	*/
	long long int MakeWaveForNSamples(unsigned char wavBuf[],unsigned long long int numSamplesRequested) const;

	/*! Adds a wave to the buffer, and returns the number of samples (number_of_bytes_filled/4).
	    Sampling rate is defined by WAVE_SAMPLING_RATE.
	*/
	long long int MakeWaveForNSamples(unsigned char wavBuf[],unsigned int nPlayingCh,unsigned int playingCh[],unsigned long long int numSamplesRequested) const;
private:
	class WithLFO;
	class WithoutLFO;
	template <class LFO>
	long long int MakeWaveForNSamplesTemplate(unsigned char wavBuf[],unsigned int nPlayingCh,unsigned int playingCh[],unsigned long long int numSamplesRequested) const;

	/*! lastSlot0Out is input/output.  Needed for calculating feedback.
	*/
	template <class LFOClass>
	int CalculateAmplitude(int chNum,const uint64_t timeInMicrosecS12[NUM_SLOTS],const unsigned int slotPhase[4],const int AMS4096[4],int &lastSlot0Out) const;


public:
	/*! Change channel state to RELEASE.
	*/
	void KeyOff(unsigned int ch,unsigned int slotFlags=SLOTFLAGS_ALL);


	/*! Check if the tone is done, and update playingCh and playing state.
	*/
	void CheckToneDone(unsigned int chNum);


	/*! Check if the tone is done, and update playingCh and playing state.
	*/
	void CheckToneDoneAllChannels(void);


	/*! Updates slot envelope.
	*/
	void UpdateSlotEnvelope(const Channel &ch,Slot &slot);


	/*!
	*/
	void UpdateRelease(const Channel &ch,Slot &slot);


	/*! BLOCK_NOTE is as calculated by [2] pp.204.  Isn't it just high-5 bits of BLOCK|F_NUM2?
	    Return value:
	       true    Envelope calculated
	       false   Envelope not calculated. (AR==0)
	    Envelope:
	       env[0]  Duration for attack (in microseconds)
	       env[1]  TL amplitude (0-127)
	       env[2]  Duration for decay (in microseconds)
	       env[3]  SL amplitude (0-127)
	       env[4]  Duration after reaching SL.
	       env[5]  Zero
	*/
	bool CalculateEnvelope(unsigned int env[6],unsigned int BLOCK_NOTE,const Slot &slot) const;
private:
	inline bool NoTone(unsigned int env[6]) const
	{
		env[0]=0;
		env[1]=0;
		env[2]=0;
		env[3]=0;
		env[4]=0;
		env[5]=0;
		return false;
	}


public:
	/*! Based on [2] Table I-5-37
		BLOCK=4		    	Freq Ratio	Freq/Fnum
		C5	523.3	1371	        	0.381692195
		B4	493.9	1294	1.05952622	0.381684699
		A4#	466.2	1222	1.059416559	0.381505728
		A4	440  	1153	1.059545455	0.381613183
		G4#	415.3	1088	1.059475078	0.381709559
		G4	392  	1027	1.059438776	0.381694255
		F4#	370 	969 	1.059459459	0.381836945
		F4	349.2	915 	1.059564719	0.381639344
		E4	329.6	864 	1.059466019	0.381481481
		D4#	311.1	815 	1.05946641	0.381717791
		D4	293.7	769 	1.059244127	0.381924577
		C4#	277.2	726 	1.05952381	0.381818182

		Average Freq/Fnum
		0.381693162

		BLOCK=7		Fnum*0.381693162* 8=Freq
		BLOCK=6		Fnum*0.381693162* 4=Freq
		BLOCK=5		Fnum*0.381693162* 2=Freq
		BLOCK=4		Fnum*0.381693162   =Freq
		BLOCK=3		Fnum*0.381693162/ 2=Freq
		BLOCK=2		Fnum*0.381693162/ 4=Freq
		BLOCK=1		Fnum*0.381693162/ 8=Freq
		BLOCK=0		Fnum*0.381693162/16=Freq

		F-Number Sampled from F-BASIC 386.
		PLAY "O4A"  -> 1038 must correspond to 440Hz -> Ratio should be 0.423892100192678.
	*/
	inline unsigned int BLOCK_FNUM_to_FreqX16(unsigned int BLOCK,unsigned int FNUM)
	{
		/* Value based on [2]
		static const unsigned int scale[8]=
		{
			(3817*16/10)/16,
			(3817*16/10)/8,
			(3817*16/10)/4,
			(3817*16/10)/2,
			(3817*16/10),
			(3817*16/10)*2,
			(3817*16/10)*4,
			(3817*16/10)*8,
		}; */
		// Value based on the observation.
		static const unsigned int scale[8]=
		{
			(423892    /1000),   // (4238*16/10)/16,
			(423892  *2/1000),   // (4239*16/10)/8,
			(423892  *4/1000),   // (4239*16/10)/4,
			(423892  *8/1000),   // (4239*16/10)/2,
			(423892 *16/1000),   // (4239*16/10),
			(423892 *32/1000),   // (4239*16/10)*2,
			(423892 *64/1000),   // (4239*16/10)*4,
			(423892*128/1000),   // (4239*16/10)*8,
		};
		FNUM*=scale[BLOCK&7];
		FNUM/=1000;
		return FNUM;
	}


	static void GetCarrierSlotFromConnection(int &numCarrierSlots,int carrierSlots[4],unsigned int connection);


	std::vector <std::string> GetStatusText(void) const;
};


/* } */
#endif
