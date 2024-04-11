#include <windows.h>
#include <mmeapi.h>
#include "../midi_interface.h"

class MIDI_Actual : public MIDI_Interface
{
private:
	HMIDIOUT hMidi=NULL;

public:
	MIDI_Actual();
	~MIDI_Actual();
	void SendCommand(unsigned char cmdBuf[]) override;
};

MIDI_Actual::MIDI_Actual()
{
	if(MMSYSERR_NOERROR==midiOutOpen(
	    &hMidi,
	    MIDI_MAPPER,
	    NULL, // No callback
	    NULL, // Not needed because no callback
	    CALLBACK_NULL))
	{
	}
	else
	{
		hMidi=NULL;
	}
}

MIDI_Actual::~MIDI_Actual()
{
	if(NULL!=hMidi)
	{
		midiOutClose(hMidi);
	}
}

void MIDI_Actual::SendCommand(unsigned char cmdBuf[])
{
	if(NULL!=hMidi)
	{
		DWORD msg=DWORD(cmdBuf[0])|(DWORD(cmdBuf[1])<<8)|(DWORD(cmdBuf[2])<<16);
		midiOutShortMsg(hMidi,msg);
	}
}

MIDI_Interface *MIDI_Interface::Create(void)
{
	return new MIDI_Actual;
}
void MIDI_Interface::Delete(MIDI_Interface *itfc)
{
	auto *ptr=dynamic_cast<MIDI_Actual *>(itfc);
	if(nullptr!=ptr)
	{
		delete ptr;
	}
	// If not, let it leak.  What else can I do?
}
