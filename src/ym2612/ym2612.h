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

	enum
	{
		REG_TIMER_CONTROL=0x27,
		REG_TIMER_A_COUNT_HIGH=0x24,
		REG_TIMER_A_COUNT_LOW=0x25,
		REG_TIMER_B_COUNT=0x26,
	};

	enum
	{
		TICK_DURATION_IN_NS=1667,
		TIMER_A_PER_TICK=12,
		TIMER_B_PER_TICK=192,
		NTICK_TIMER_A=TICK_DURATION_IN_NS*TIMER_A_PER_TICK,
		NTICK_TIMER_B=TICK_DURATION_IN_NS*TIMER_B_PER_TICK,
	};

	class State
	{
	public:
		unsigned long long int deviceTimeInNS;
		unsigned long long int lastTickTimeInNS;
		unsigned char reg[256];  // I guess only 0x21 to 0xB6 are used.
		unsigned long long int timerCounter[2];
		bool timerUp[2];

		void PowerOn(void);
		void Reset(void);
	};

	State state;

	YM2612();
	~YM2612();
	void PowerOn(void);
	void Reset(void);

	void WriteRegister(unsigned int reg,unsigned int value);
	unsigned int ReadRegister(unsigned int reg) const;

	void Run(unsigned long long int systemTimeInNS);

	bool TimerAUp(void) const;
	bool TimerBUp(void) const;

	/*! Returns timer-up state of 
	*/
	bool TimerUp(unsigned int timerId) const;

	std::vector <std::string> GetStatusText(void) const;
};


/* } */
#endif
