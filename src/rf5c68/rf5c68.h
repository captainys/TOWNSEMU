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
	void RenewIRQTimer(unsigned int ch);


	/*!
	*/
	void SetUpRepeat(unsigned int chNum);
};


/* } */
#endif
