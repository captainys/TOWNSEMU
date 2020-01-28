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

	enum
	{
		SEND_NONE_AFTER_RESET,
		SEND_CD_AFTER_RESET,
		SEND_F0_AFTER_RESET,
		SEND_F1_AFTER_RESET,
	};
	unsigned int afterReset;

	enum
	{
		FIFO_BUF_LEN=32
	};
	unsigned int nFifoFilled;
	unsigned char fifoBuf[FIFO_BUF_LEN];
	void PushFifo(unsigned char code1,unsigned char code2); // Always push a pair.


	virtual const char *DeviceName(void) const{return "KEYBOARD";}
	TownsKeyboard();
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void Reset(void);
};

/* } */
#endif
