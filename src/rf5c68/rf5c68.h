/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef RF5C68_IS_INCLUDED
#define RF5C68_IS_INCLUDED
/* { */


#include <vector>
#include <cstdint>
#include <string>

class RF5C68
{
public:
	enum
	{
		SAMPLING_RATE=20833,  // Supposed to be 19200.  Where is this 20833 come from? The master clock of the RF5C68 in FM TOWNS is 8 MHz, and dividing it by 384 gives 20.833 Hz.
		WAVERAM_SIZE=65536,
		NUM_CHANNELS=8,
		FREQ=20833,
		FD_BIT_SHIFT=11,
		BANK_SHIFT=12, // 2^12 bytes per bank.
		LOOP_STOP_CODE=0xFF,
		WAVE_OUTPUT_AMPLITUDE_MAX_DEFAULT=4096,
		// Was 8192.
		// Had to change amplitude max of YM2612 to 4096 to prevent Super Daisenryaku tones from breaking.
		// Adjusted to keep balance.
	};

	enum
	{
		REG_ENV=0,
		REG_PAN=1,
		REG_FDL=2,
		REG_FDH=3,
		REG_LSL=4,
		REG_LSH=5,
		REG_ST=6,
		REG_CONTROL=7,
		REG_CH_ON_OFF=8,
	};

	class Channel
	{
	public:
		unsigned char ENV,PAN,ST;
		unsigned short FD,LS;

		// playPtr is set when:
		//    Written to ST, or
		//    Play reached the end and came back to LS.
		uint16_t playPtr,playPtrLeftOver;
		bool repeatAfterThisSegment;

		bool IRQAfterThisPlayBack=false;
		unsigned short IRQBank=0; // 00H to 0FH.  64KB/4K=16 banks.
	};
	class StartAndStopChannelBits
	{
	public:
		unsigned chStartPlay=0,chStopPlay=0;
	};
	class State
	{
	public:
		std::vector <unsigned char> waveRAM;
		Channel ch[NUM_CHANNELS];

		int timeBalance;       // For WAV generation.
		int volume=WAVE_OUTPUT_AMPLITUDE_MAX_DEFAULT;

		// For Linear interpolation
		int Lout_prev;
		int Rout_prev;

		bool playing;          // Bit 7 of I/O 04F7H
		unsigned short Bank;   // Bank x000H
		unsigned char CB;      // Channel
		unsigned char chOnOff; // I/O 04F8H

		inline bool IRQ() const
		{
			return 0!=IRQBank;
		}
		unsigned char IRQBank,IRQBankMask;
	};
	State state;
	bool chMute[NUM_CHANNELS]={false,false,false,false,false,false,false,false};


	RF5C68();
	void Clear(void);

	/*! Writes to the control register.
	    Returns channels that starts playing.
	*/
	StartAndStopChannelBits WriteControl(unsigned char value);

	/*! Writes to the channel on/off register.
	    Returns channels that starts playing. 
	*/
	StartAndStopChannelBits WriteChannelOnOff(unsigned char value);

	/*! Writes to the IRQ Bank mask register. */
	void WriteIRQBankMask(unsigned char value);

protected:
	/*! Writes to the ENV register. */
	void WriteENV(unsigned char value);

	/*! Writes to the ENV register. */
	void WritePAN(unsigned char value);

	/*! Writes to FDL register. */
	void WriteFDL(unsigned char value);

	/*! Writes to FDL register. */
	void WriteFDH(unsigned char value);

	/*! Writes to LSL register. */
	void WriteLSL(unsigned char value);

	/*! Writes to LSL register. */
	void WriteLSH(unsigned char value);

	/*! Writes to ST register. */
	void WriteST(unsigned char value);

public:
	/*! Writes to Wave RAM. */
	inline void WriteWaveRAM(unsigned int offset,unsigned char value)
	{
		state.waveRAM[state.Bank+offset]=value;
	}
	/*! Read from Wave RAM. */
	inline unsigned char ReadWaveRAM(unsigned int offset) const
	{
		return state.waveRAM[state.Bank+offset];
	}

	std::vector <std::string> GetStatusText(void) const;

	/*! Make 19.2KHz signed 16-bit Stereo wave for requested samples.
	    Returns actual number of samples filled in the buffer.  Number of bytes will be return-value*4.
	    Buffer must be long enough for numSamples*4.
	*/
	unsigned int MakeWaveForNumSamples(unsigned char waveBuf[],unsigned int numSamples,int outSamplingRate,uint64_t lastWAVGenTime);

	unsigned int AddWaveForNumSamples(unsigned char waveBuf[],unsigned int numSamples,int outSamplingRate,uint64_t lastWAVGenTime);

	/*! Returns true if playing.
	*/
	bool IsPlaying(void) const;

	/*! Notified from the controller that the play has stopped.
	*/
	void PlayStopped(unsigned int ch);

	/*! Raise IRQ flag of the bank.  Bank must be 0 to 0FH.
	*/
	void SetIRQBank(unsigned int bank);

	/*!
	*/
	void SetUpNextSegment(unsigned int chNum);



	/*! If useScheduling is false, VMTime can be 0.
	    To use register scheduling, set useScheduling=true, and give VMTime to WriteRegister and MakeWaveForNumSamples / AddWaveForNumSamples.
	*/
	void WriteRegister(unsigned char reg,unsigned char data,uint64_t VMTimeInNS);

private:
	StartAndStopChannelBits ReallyWriteRegister(unsigned int reg,unsigned int value,uint64_t systemTimeInNS);
	void WriteRegisterSchedule(unsigned int reg,unsigned int value,uint64_t systemTimeInNS);

public:
	class RegWriteLog
	{
	public:
		unsigned char reg,data;
		uint64_t systemTimeInNS;
	};
	bool useScheduling=false;
	std::vector <RegWriteLog> regWriteSched;
	void FlushRegisterSchedule(void);
};


/* } */
#endif
