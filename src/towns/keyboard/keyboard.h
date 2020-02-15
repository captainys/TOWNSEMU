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
	void PushFifo(unsigned char code1,unsigned char code2); // Always push a pair.

	/*! Translates an ASCII code to a sequence of FM Towns Keyboard codes.
	    Returns the number of bytes of the generated key codes.
	    FM Towns key codes are two-bytes long.
	    Therefore always the returned number of bytes is even.
	    The maximum number of bytes generated should be 4, but just in case,
	    pass 16-byte long array always.
	*/
	static unsigned int TranslateChar(unsigned char keyCodeBuf[16],bool keyPress,bool keyRelease);
private:
	static TownsKeyCombination translation_data[256];

public:
	virtual const char *DeviceName(void) const{return "KEYBOARD";}
	TownsKeyboard();
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);

	virtual void Reset(void);
};

/* } */
#endif
