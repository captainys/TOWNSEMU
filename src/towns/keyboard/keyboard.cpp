#include "cpputil.h"
#include "keyboard.h"
#include "townsdef.h"



void TownsKeyboard::State::Reset(void)
{
}
TownsKeyboard::TownsKeyboard()
{
}
/* virtual */ void TownsKeyboard::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_KEYBOARD_DATA://       0x600, // [2] pp.234
		break;
	case TOWNSIO_KEYBOARD_STATUS_CMD:// 0x602, // [2] pp.231
		break;
	case TOWNSIO_KEYBOARD_IRQ://        0x604, // [2] pp.236
		break;
	}
}
/* virtual */ unsigned int TownsKeyboard::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case TOWNSIO_KEYBOARD_DATA://       0x600, // [2] pp.234
		break;
	case TOWNSIO_KEYBOARD_STATUS_CMD:// 0x602, // [2] pp.231
		return 0;  // IBF=0 Data Empty. Always Ready.
	case TOWNSIO_KEYBOARD_IRQ://        0x604, // [2] pp.236
		return 0; // NO IRQ Implemented yet.
	}
	return 0xff;
}
/* virtual */ void TownsKeyboard::Reset(void)
{
	state.Reset();
}
