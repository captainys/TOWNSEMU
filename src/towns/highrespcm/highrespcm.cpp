#include "highrespcm.h"
#include "towns.h"



void TownsHighResPCM::State::PowerOn(void)
{
	Reset();
}
void TownsHighResPCM::State::Reset(void)
{
	DMAC.baseCount=0;
	DMAC.currentCount=0;
	DMAC.baseAddr=0;
	DMAC.currentAddr=0;
	DMAC.modeCtrl=0;
	DMAC.terminalCount=false;
	DMACBase=false;

	AudioInIRQ=false;
	bufferIRQ=false;
	DMAIRA=false;

	SNDFormat=false; // WAV by default.
	stereo=false;
	bit16=false;
}

////////////////////////////////////////////////////////////

TownsHighResPCM::TownsHighResPCM(FMTownsCommon *townsPtr) : Device(townsPtr)
{
	PowerOn();
	std::cout << "Warning: TownsHighResPCM is experimental and not included in state-save at this time." << std::endl;
}
void TownsHighResPCM::PowerOn(void)
{
	Reset();
}
void TownsHighResPCM::Reset(void)
{
	state.Reset();
}

void TownsHighResPCM::IOWriteByte(unsigned int ioport,unsigned int data)
{
	if(true!=state.enabled)
	{
		return;
	}
}
unsigned int TownsHighResPCM::IOReadByte(unsigned int ioport)
{
	if(true!=state.enabled)
	{
		return 0xFF;
	}

	return 0xFF;
}

////////////////////////////////////////////////////////////

uint32_t TownsHighResPCM::SerializeVersion(void) const
{
	return 0;
}

void TownsHighResPCM::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
}

bool TownsHighResPCM::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	return true;
}
