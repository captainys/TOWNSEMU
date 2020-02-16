#ifndef KEYBOARD_IS_INCLUDED
#define KEYBOARD_IS_INCLUDED
/* { */

#include "device.h"

class TownsKeyboard : public Device
{
public:
	enum
	{
		KEY_REPEAT_INTERVAL=1666667, // 1000000000/600 nano secounds
	};


	class State
	{
	public:
		bool IRQEnabled;
		bool KBINT;       // Bit 0 of I/O 604H
		void Reset(void);
	};

	struct TownsKeyCombination
	{
		bool shift,ctrl;
		unsigned int keyCode;
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

	class FMTowns *townsPtr;
	class TownsPIC *picPtr;

	void PushFifo(unsigned char code1,unsigned char code2); // Always push a pair.



	/*! Translates an ASCII code to a sequence of FM Towns Keyboard codes.
	    Returns the number of bytes of the generated key codes either 2 or 0.
	    For making it a key-press code, do keyCodeBuf[0]|=TOWNS_KEYFLAG_THUMBSHIFT_PRESS_OR_RELEASE.
	*/
	static unsigned int TranslateChar(unsigned char keyCodeBuf[2],unsigned char c);
private:
	static TownsKeyCombination translation_data[256];

public:
	virtual const char *DeviceName(void) const{return "KEYBOARD";}
	TownsKeyboard(class FMTowns *townsPtr,class TownsPIC *picPtr);
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void RunScheduledTask(unsigned long long int townsTime);

	virtual void Reset(void);
};

/* } */
#endif
