#ifndef SOUND_IS_INCLUDED
#define SOUND_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "device.h"
#include "townsdef.h"
#include "cpputil.h"
#include "ym2612.h"

class TownsSound : public Device
{
private:
	class FMTowns *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "SOUND";}

	class State
	{
	public:
		YM2612 ym2612;
		unsigned int muteFlag;
		unsigned int addrLatch[2];

		void PowerOn(void);
		void Reset(void);
		void ResetVariables(void);
	};

	State state;

	TownsSound(class FMTowns *townsPtr)
	{
		this->townsPtr=townsPtr;
	}

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void RunScheduledTask(unsigned long long int townsTime);

	std::vector <std::string> GetStatusText(void) const;
};

/* } */
#endif
