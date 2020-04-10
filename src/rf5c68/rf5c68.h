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
#include <string>

class RF5C68
{
public:
	enum
	{
		WAVERAM_SIZE=65536,
		NUM_CHANNELS=8,
		FREQ=19600,
		FD_BIT_SHIFT=11,
	};

	class Channel
	{
	public:
		unsigned char ENV,PAN,ST;
		unsigned short FD,LS;
		double IRQTimer;
		unsigned char playingBank; // 00H to 0FH.  64KB/4K=16 banks.

		// StartPtr is set when:
		//    Written to ST, or
		//    Play reached the end and came back to LS.
		unsigned short startPtr;
		bool repeatAfterThisSegment;
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

		bool playing;          // Bit 7 of I/O 04F7H
		unsigned short Bank;   // Bank x000H
		unsigned char CB;      // Channel
		unsigned char chOnOff; // I/O 04F8H

		bool IRQ;
		unsigned char IRQBank,IRQBankMask;
	};
	State state;


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

	/*! Make 19.2KHz signed 16-bit wave.
	*/
	std::vector <unsigned char> Make19KHzWave(unsigned int ch);

	/*! Notified from the controller that the play has started.
	    This function sets IRQTimer for the channel.
	*/
	void PlayStarted(unsigned int ch);

	/*! Notified from the controller that the play has stopped.
	*/
	void PlayStopped(unsigned int ch);

	/*!
	*/
	void SetIRQ(unsigned int ch);

	/*!
	*/
	void SetUpNextSegment(unsigned int chNum);
};


/* } */
#endif
