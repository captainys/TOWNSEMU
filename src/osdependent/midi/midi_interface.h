#ifndef MIDI_INTERFACE_IS_INCLUDED
#define MIDI_INTERFACE_IS_INCLUDED
/* { */

class MIDI_Interface
{
public:
	static MIDI_Interface *Create(void);
	static void Delete(MIDI_Interface *);
	virtual void SendCommand(unsigned char cmdBuf[])=0;
};

/* } */
#endif
