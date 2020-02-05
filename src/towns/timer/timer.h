#ifndef TIMER_IS_INCLUDED
#define TIMER_IS_INCLUDED
/* { */

#include "device.h"
#include "townsdef.h"
#include "cpputil.h"

class TownsTimer : public Device
{
private:
	class FMTowns *townsPtr;
	class TownsPIC *picPtr;
public:
	virtual const char *DeviceName(void) const{return "INTERVALTIMER";}

	class State
	{
	public:
		void PowerOn(void);
		void Reset(void);
	};

	State state;

	TownsTimer(class FMTowns *townsPtr,class TownsPIC *picPtr)
	{
		this->townsPtr=townsPtr;
	}

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void RunScheduledTask(unsigned long long int townsTime);
};

/* } */
#endif
