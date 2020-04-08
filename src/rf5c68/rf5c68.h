#ifndef RF5C68_IS_INCLUDED
#define RF5C68_IS_INCLUDED
/* { */


#include <vector>

class RF5C68
{
public:
	enum
	{
		WAVERAM_SIZE=65536,
		NUM_CHANNELS=8
	};

	class Channel
	{
	public:
		unsigned char ENV,PAN,ST;
		unsigned short FD,LS;
	};
	class State
	{
	public:
		std::vector <unsigned char> waveRAM;
		Channel ch[NUM_CHANNELS];

		bool playing;          // Bit 7 of I/O 04F7H
		unsigned char Bank;    // Bank x000H
		unsigned char CB;      // Channel
		unsigned char chOnOff; // I/O 04F8H
	};
	State state;


	RF5C68();
	void Clear(void);

	/*! Writes to the control register.
	    Returns channels that starts playing.
	*/
	unsigned char WriteControl(unsigned char value);

	/*! Writes to the channel on/off register.
	    Returns channels that starts playing. 
	*/
	unsigned char WriteChannelOnOff(unsigned char value);

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
};


/* } */
#endif
