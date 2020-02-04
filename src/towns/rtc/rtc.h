#ifndef RTC_IS_INCLUDED
#define RTC_IS_INCLUDED
/* { */

class TownsRTC : public Device
{
private:
	class FMTowns *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "RTC";}

	enum
	{
		REG_1SEC=0,
		REG_10SEC=1,
		REG_MIN=2,
		REG_10MIN=3,
		REG_HOUR=4,
		REG_10HOUR=5, // Bit 2 is AM/PM
		REG_WKDAY=6,  // 0 to 6
		REG_1DAY=7,
		REG_10DAY=8,
		REG_MONTH=9,
		REG_10MONTH=0x0A,
		REG_YEAR=0x0B,
		REG_10YEAR=0x0C,
	};

	enum 
	{
		STATE_NONE,
		STATE_COMMAND,
	};

	class State
	{
	public:
		unsigned int state;
		bool hour24; // If true, return 00:00 to 23:59 scale
		unsigned int registerLatch;
		unsigned int lastDataWrite;
		void PowerOn(void);
		void Reset(void);
	};

	State state;

	TownsRTC(class FMTowns *townsPtr)
	{
		this->townsPtr=townsPtr;
	}

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);
};

/* } */
#endif
