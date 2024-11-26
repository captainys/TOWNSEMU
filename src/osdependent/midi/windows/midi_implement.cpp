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
	void SendCommand(const unsigned char cmdBuf[]) override;
	void SendExclusiveCommand(const unsigned char cmdBuf[],int len) override;
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
		hMidi=NULL;
	}
}

void MIDI_Actual::SendCommand(const unsigned char cmdBuf[])
{
	if(NULL!=hMidi)
	{
		DWORD msg=DWORD(cmdBuf[0])|(DWORD(cmdBuf[1])<<8)|(DWORD(cmdBuf[2])<<16);
		midiOutShortMsg(hMidi,msg);
	}
}

void MIDI_Actual::SendExclusiveCommand(const unsigned char cmdBuf[],int len)
{
	if (NULL != hMidi)
	{
		MIDIHDR mhMidi;
		unsigned char SendBuf[14]{};

		SendBuf[0]=0xf0;
		memcpy(&SendBuf[1],cmdBuf,len);
		SendBuf[len+1]=0xf7;

		ZeroMemory(&mhMidi, sizeof(mhMidi));

		mhMidi.lpData=(LPSTR)SendBuf;
		mhMidi.dwBufferLength=len+2;
		mhMidi.dwBytesRecorded=len+2;

		midiOutPrepareHeader(hMidi,&mhMidi,sizeof(mhMidi));
		midiOutLongMsg(hMidi,&mhMidi,sizeof(mhMidi));
		while ((mhMidi.dwFlags&MHDR_DONE) == 0);
		midiOutUnprepareHeader(hMidi,&mhMidi,sizeof(mhMidi));
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
