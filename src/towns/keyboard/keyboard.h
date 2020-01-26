#ifndef KEYBOARD_IS_INCLUDED
#define KEYBOARD_IS_INCLUDED
/* { */

#include "device.h"

class TownsKeyboard : public Device
{
public:
	class State
	{
	public:
		void Reset(void);
	};

	State state;

	virtual const char *DeviceName(void) const{return "KEYBOARD";}
	TownsKeyboard();
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void Reset(void);
};

/* } */
#endif
