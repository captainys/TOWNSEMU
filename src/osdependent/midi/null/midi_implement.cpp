#include "../midi_interface.h"

class MIDI_Actual : public MIDI_Interface
{
public:
	void SendCommand(const unsigned char cmdBuf[]) override
	{
	}
	void SendExclusiveCommand(const unsigned char cmdBuf[],int len) override
	{
	}
};

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
