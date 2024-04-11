#ifndef I8253_IS_INCLUDED
#define I8253_IS_INCLUDED
/* { */

#include <stdint.h>

// Goal: Use this class from TownsTimer::State.  i8253/i8254 is used from Towns on-board PIT and MIDI card.

// Memo: Redbook p.617 tells the resolution of PIT on MIDI card is 1/480,000,000 seconds.  480MHz?
// i8253/i8254 PIT Programmable Interval Timer
class i8253
{
public:
	enum
	{
		NUM_CHANNELS=3
	};

	class Channel
	{
	public:
		unsigned char mode,lastCmd;
		unsigned short counter,counterInitialValue;
		unsigned short latchedCounter;
		unsigned short increment;
		bool OUT,counting;
		mutable bool latched,bcd;

		unsigned int RL;
		bool accessLow; // true->access low byte  false->access high byte
	};
	uint64_t lastTickTimeInNS;
	Channel channels[NUM_CHANNELS];

	void PowerOn(void);
	void Reset(void);

	void Latch(unsigned int ch);
	unsigned short ReadLatchedCounter(unsigned int ch) const;
	void SetChannelMode(unsigned int ch,unsigned int mode);
	void SetChannelCounter(unsigned int ch,unsigned int value);
	unsigned int ReadChannelCounter(unsigned int ch); // accessLow may flip.  Not to be const.
	void ProcessControlCommand(unsigned int cmd);

	void TickIn(unsigned int nTick);
};


/* } */
#endif
