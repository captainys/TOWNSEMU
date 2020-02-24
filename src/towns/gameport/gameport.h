#ifndef GAMEPORT_IS_INCLUDED
#define GAMEPORT_IS_INCLUDED
/* { */

#include "cheapmath.h"

class TownsGamePort : public Device
{
private:
	class FMTowns *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "GAMEPORT";}

	enum
	{
		MOUSE,
		GAMEPAD,
		// CYBERSTICK,  Hope for the future versions.
	};

	class Port
	{
	public:
		int device;  // MOUSE, GAMEPAD, or CYBERSTICK
		int state;
		bool COM;
		bool button[2];
		Vec2i mouseMotion;
		long long int lastReadTime;

		void Write(bool COM,bool T1,bool T2);
		unsigned char Read(long long int townsTime); // Reading last coordinate should reset motion.  Not a const.
	};

	class State
	{
	public:
		Port ports[2];
		void PowerOn(void);
		void Reset(void);
	};

	State state;

	TownsGamePort(class FMTowns *townsPtr)
	{
		this->townsPtr=townsPtr;
		state.ports[0].device=GAMEPAD;
		state.ports[1].device=MOUSE;
		state.PowerOn();
	}

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);
};

/* } */
#endif
