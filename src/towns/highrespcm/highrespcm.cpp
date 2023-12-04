#include <stdint.h>
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
	DMAE=false;
	DMAEIRQEnabled=false;

	bufferIRQEnabled=false;
	bufferOverrun=false;
	bufferUnderrun=false;

	audioIRQEnabled=false;

	audioInIRQ=false;
	bufferIRQ=false;
	DMAIRQ=false;

	SNDFormat=false; // WAV by default.
	stereo=false;
	bit16=false;

	recording=false;
}

////////////////////////////////////////////////////////////

TownsHighResPCM::TownsHighResPCM(FMTownsCommon *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
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

	switch(ioport)
	{
	case TOWNSIO_HIGHRESPCM_BANK://         0x510, // [2] pp.832
		state.DMACBase=(0!=(data&1));
		break;
	case TOWNSIO_HIGHRESPCM_DMASTATUS://    0x511, // [2] pp.832
		if(0!=(data&1))
		{
			state.DMAE=false;
			state.DMAIRQ=false; // Prob
		}
		state.DMAEIRQEnabled=(0!=(data&2));
		state.DMAC.modeCtrl=(data&0x10);
		break;
	case TOWNSIO_HIGHRESPCM_DMACOUNT_LOW:// 0x512, // [2] pp.833
		state.DMAC.currentCount&=0xff00;
		state.DMAC.currentCount|=(data&0xff);
		state.DMAC.terminalCount=false; // Probably
		if(true!=state.DMACBase)
		{
			state.DMAC.baseCount=state.DMAC.currentCount;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMACOUNT_HIGH://0x513, // [2] pp.833
		state.DMAC.currentCount&=0xff;
		state.DMAC.currentCount|=((data&0xff)<<8);
		state.DMAC.terminalCount=false; // Probably
		if(true!=state.DMACBase)
		{
			state.DMAC.baseCount=state.DMAC.currentCount;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_LOW://  0x514, // [2] pp.834
		state.DMAC.currentAddr&=0xffffff00;
		state.DMAC.currentAddr|=(data&0xff);
		if(true!=state.DMACBase)
		{
			state.DMAC.baseAddr=state.DMAC.currentAddr;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_MIDLOW://0x515, // [2] pp.834
		state.DMAC.currentAddr&=0xffff00ff;
		state.DMAC.currentAddr|=((data&0xff)<<8);
		if(true!=state.DMACBase)
		{
			state.DMAC.baseAddr=state.DMAC.currentAddr;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_MIDHIGH://0x516, // [2] pp.834
		state.DMAC.currentAddr&=0xff00ffff;
		state.DMAC.currentAddr|=((data&0xff)<<16);
		if(true!=state.DMACBase)
		{
			state.DMAC.baseAddr=state.DMAC.currentAddr;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_HIGH:// 0x517, // [2] pp.834
		state.DMAC.currentAddr&=0x00ffffff;
		state.DMAC.currentAddr|=((data&0xff)<<24);
		if(true!=state.DMACBase)
		{
			state.DMAC.baseAddr=state.DMAC.currentAddr;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_CLOCK://0x518, // [2] pp.834
		switch(data&15)
		{
		case 0:
			state.freq=48000;
			break;
		case 1:
			state.freq=44100;
			break;
		case 2:
			state.freq=32000;
			break;
		case 3:
			state.freq=22050;
			break;
		case 4:
			state.freq=19200;
			break;
		case 5:
			state.freq=16000;
			break;
		case 6:
			state.freq=11025;
			break;
		case 7:
			state.freq=9600;
			break;
		case 8:
			state.freq=8000;
			break;
		case 9:
			state.freq=5513;
			break;
		case 10:
			state.freq=24000;
			break;
		case 11:
			state.freq=12000;
			break;
		}
		break;
	case TOWNSIO_HIGHRESPCM_MODE://         0x519, // [2] pp.835
		if(0!=(data&0x20))
		{
			state.SNDFormat=true;
			state.stereo=false;
			state.bit16=false;
		}
		else
		{
			state.SNDFormat=false;
			state.stereo=(0!=(data&0x10));
			state.bit16=(0!=(data&0x08));
		}
		state.conversionLevel=(data&7);
		break;
	case TOWNSIO_HIGHRESPCM_SYSCONTROL://   0x51A, // [2] pp.836
		if(0!=(data&0x80))
		{
			state.resetTime=townsPtr->state.townsTime+RESET_TIME;
			Reset();
		}
		else
		{
			state.recording=(0!=(data&0x10));  // Not supported yet though.
		}
		switch(data&7)
		{
		default:
			state.interrupt=NO_INTERRUPT;
			break;
		case 0:
			state.interrupt=4;
			break;
		case 1:
			state.interrupt=5;
			break;
		case 2:
			state.interrupt=10;
			break;
		case 3:
			state.interrupt=14;
			break;
		case 4:
			state.interrupt=15;
			break;
		}
		break;
	case TOWNSIO_HIGHRESPCM_BUFFCONTROL://  0x51B, // [2] pp.837
		state.bufferIRQEnabled=(0!=(data&0x80));
		if(0!=(data&0x40))
		{
			state.bufferOverrun=false;
		}
		if(0!=(data&0x20))
		{
			state.bufferUnderrun=false;
		}
		if(0x0F==(data&0x0F))
		{
			// Initialize buffer?
		}
		break;
	case TOWNSIO_HIGHRESPCM_REC_PLAYBACK:// 0x51C, // [2] pp.838
		state.audioIRQEnabled=(0!=(data&0x20));
		if(0!=(data&0x10))
		{
			state.audioLevelDetected=false;
			state.audioInIRQ=false; // Prob.
		}
		state.DREQMask=(0!=(data&2));
		state.bufferMask=(0!=(data&1));
		break;
	case TOWNSIO_HIGHRESPCM_REC_PEAK_MON:// 0x51D, // [2] pp.839
		if(0==(data&0x80))
		{
			state.audioLevelDetected=false; // Prob.
			state.audioInIRQ=false; // Prob.
		}
		else
		{
			state.peakLevel=data;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DATA_LOW://     0x51E, // [2] pp.840
		break;
	case TOWNSIO_HIGHRESPCM_DATA_HIGH://    0x51F, // [2] pp.840
		break;
	}
}
unsigned int TownsHighResPCM::IOReadByte(unsigned int ioport)
{
	if(true!=state.enabled)
	{
		return 0xFF;
	}

	uint8_t data=0;
	switch(ioport)
	{
	case TOWNSIO_HIGHRESPCM_BANK://         0x510, // [2] pp.832
		data=(state.DMACBase ? 1 : 0);
		data|=(state.DMAIRQ ? 0x10 : 0);
		data|=(state.bufferIRQ ? 0x20 : 0);
		data|=(state.audioInIRQ ? 0x40 : 0);
		break;
	case TOWNSIO_HIGHRESPCM_DMASTATUS://    0x511, // [2] pp.832
		break;
	case TOWNSIO_HIGHRESPCM_DMACOUNT_LOW:// 0x512, // [2] pp.833
		break;
	case TOWNSIO_HIGHRESPCM_DMACOUNT_HIGH://0x513, // [2] pp.833
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_LOW://  0x514, // [2] pp.834
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_MIDLOW://0x515, // [2] pp.834
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_MIDHIGH://0x516, // [2] pp.834
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_HIGH:// 0x517, // [2] pp.834
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_CLOCK://0x518, // [2] pp.834
		break;
	case TOWNSIO_HIGHRESPCM_MODE://         0x519, // [2] pp.835
		break;
	case TOWNSIO_HIGHRESPCM_SYSCONTROL://   0x51A, // [2] pp.836
		break;
	case TOWNSIO_HIGHRESPCM_BUFFCONTROL://  0x51B, // [2] pp.837
		break;
	case TOWNSIO_HIGHRESPCM_REC_PLAYBACK:// 0x51C, // [2] pp.838
		break;
	case TOWNSIO_HIGHRESPCM_REC_PEAK_MON:// 0x51D, // [2] pp.839
		break;
	case TOWNSIO_HIGHRESPCM_DATA_LOW://     0x51E, // [2] pp.840
		break;
	case TOWNSIO_HIGHRESPCM_DATA_HIGH://    0x51F, // [2] pp.840
		break;
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
