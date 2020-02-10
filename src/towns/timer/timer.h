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

	TownsTimer(class FMTowns *townsPtr,class TownsPIC *picPtr)
	{
		this->townsPtr=townsPtr;
		this->picPtr=picPtr;
	}

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
