#include <stdint.h>
#include "highrespcm.h"
#include "towns.h"
#include "cpputil.h"


inline void WordOp_Add(unsigned char *ptr,int value)
{
	value+=cpputil::GetSignedWord(ptr);

	if(value<-32767)
	{
		value=-32767;
	}
	else if(32767<value)
	{
		value=32767;
	}
	cpputil::PutWord(ptr,(value&0xFFFF));
}

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
	DMAC.bytesPerCount=2;

	DMACBase=false;
	DMAE=false;
	DMAEIRQEnabled=false;

	bufferIRQEnabled=false;
	bufferOverrun=false;
	bufferUnderrun=false;
	bufferMask=true;

	audioIRQEnabled=false;

	SNDFormat=false; // WAV by default.
	stereo=false;
	bit16=false;

	recording=false;

	peakLevelThreshold=0;
	peakLevel=0;

	interrupt=NO_INTERRUPT;
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

void TownsHighResPCM::RunScheduledTask(unsigned long long int townsTime)
{
	ProcessDMA();
	if(true!=state.bufferMask)
	{
		townsPtr->ScheduleDeviceCallBack(*this,townsTime+DMA_CHECK_INTERVAL);
	}
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
		}
		state.DMAEIRQEnabled=(0!=(data&2));
		state.DMAC.modeCtrl=(data&0x10);
		UpdatePIC();
		break;
	case TOWNSIO_HIGHRESPCM_DMACOUNT_LOW:// 0x512, // [2] pp.833
		if(true==state.DMACBase)
		{
			state.DMAC.baseCount&=0xff00;
			state.DMAC.baseCount|=(data&0xff);
		}
		else
		{
			state.DMAC.currentCount&=0xff00;
			state.DMAC.currentCount|=(data&0xff);
			state.DMAC.terminalCount=false; // Probably
			state.DMAC.baseCount=state.DMAC.currentCount;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMACOUNT_HIGH://0x513, // [2] pp.833
		if(true==state.DMACBase)
		{
			state.DMAC.baseCount&=0xff;
			state.DMAC.baseCount|=((data&0xff)<<8);
		}
		else
		{
			state.DMAC.currentCount&=0xff;
			state.DMAC.currentCount|=((data&0xff)<<8);
			state.DMAC.terminalCount=false; // Probably
			state.DMAC.baseCount=state.DMAC.currentCount;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_LOW://  0x514, // [2] pp.834
		if(true==state.DMACBase)
		{
			state.DMAC.baseAddr&=0xffffff00;
			state.DMAC.baseAddr|=(data&0xff);
		}
		else
		{
			state.DMAC.currentAddr&=0xffffff00;
			state.DMAC.currentAddr|=(data&0xff);
			state.DMAC.baseAddr=state.DMAC.currentAddr;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_MIDLOW://0x515, // [2] pp.834
		if(true==state.DMACBase)
		{
			state.DMAC.baseAddr&=0xffff00ff;
			state.DMAC.baseAddr|=((data&0xff)<<8);
		}
		else
		{
			state.DMAC.currentAddr&=0xffff00ff;
			state.DMAC.currentAddr|=((data&0xff)<<8);
			state.DMAC.baseAddr=state.DMAC.currentAddr;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_MIDHIGH://0x516, // [2] pp.834
		if(true==state.DMACBase)
		{
			state.DMAC.baseAddr&=0xff00ffff;
			state.DMAC.baseAddr|=((data&0xff)<<16);
		}
		else
		{
			state.DMAC.currentAddr&=0xff00ffff;
			state.DMAC.currentAddr|=((data&0xff)<<16);
			state.DMAC.baseAddr=state.DMAC.currentAddr;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_HIGH:// 0x517, // [2] pp.834
		if(true==state.DMACBase)
		{
			state.DMAC.baseAddr&=0x00ffffff;
			state.DMAC.baseAddr|=((data&0xff)<<24);
		}
		else
		{
			state.DMAC.currentAddr&=0x00ffffff;
			state.DMAC.currentAddr|=((data&0xff)<<24);
			state.DMAC.baseAddr=state.DMAC.currentAddr;
		}
		break;
	case TOWNSIO_HIGHRESPCM_CLOCK:    //0x518, // [2] pp.834
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
			state.dataPort=(0!=(data&0x20));
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
			state.dataBuffer.clear(); // Prob.
			state.balance=1;
		}
		UpdatePIC();
		break;
	case TOWNSIO_HIGHRESPCM_REC_PLAYBACK:// 0x51C, // [2] pp.838
		state.audioIRQEnabled=(0!=(data&0x20));
		if(0!=(data&0x10))
		{
			state.audioLevelDetected=false;
		}
		state.DREQMask=(0!=(data&2));
		if(true==state.bufferMask && 0==(data&1))
		{
			townsPtr->ScheduleDeviceCallBack(*this,townsPtr->state.townsTime+DMA_CHECK_INTERVAL);
		}
		state.bufferMask=(0!=(data&1));
		UpdatePIC();
		break;
	case TOWNSIO_HIGHRESPCM_REC_PEAK_MON:// 0x51D, // [2] pp.839
		if(0==(data&0x80))
		{
			state.audioLevelDetected=false; // Prob.
			state.peakLevel=0;
		}
		else
		{
			state.peakLevelThreshold=data;
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
		data|=(state.DMAE ? 0x10 : 0);
		data|=((state.bufferOverrun || state.bufferUnderrun) ? 0x20 : 0);
		data|=(state.audioLevelDetected ? 0x40 : 0);
		break;
	case TOWNSIO_HIGHRESPCM_DMASTATUS://    0x511, // [2] pp.832
		if(true==state.DMAC.terminalCount)
		{
			data|=1;
		}
		if(true==state.DMAEIRQEnabled)
		{
			data|=2;
		}
		if(true==state.DMAC.AUTI())
		{
			data|=0x10;
		}
		break;
	case TOWNSIO_HIGHRESPCM_DMACOUNT_LOW:// 0x512, // [2] pp.833
		data=(true==state.DMACBase ? state.DMAC.baseCount : state.DMAC.currentCount)&0xFF;
		break;
	case TOWNSIO_HIGHRESPCM_DMACOUNT_HIGH://0x513, // [2] pp.833
		data=((true==state.DMACBase ? state.DMAC.baseCount : state.DMAC.currentCount)>>8)&0xFF;
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_LOW://  0x514, // [2] pp.834
		data=(true==state.DMACBase ? state.DMAC.baseAddr : state.DMAC.currentAddr)&0xFF;
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_MIDLOW://0x515, // [2] pp.834
		data=((true==state.DMACBase ? state.DMAC.baseAddr : state.DMAC.currentAddr)>>8)&0xFF;
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_MIDHIGH://0x516, // [2] pp.834
		data=((true==state.DMACBase ? state.DMAC.baseAddr : state.DMAC.currentAddr)>>16)&0xFF;
		break;
	case TOWNSIO_HIGHRESPCM_DMAADDR_HIGH:// 0x517, // [2] pp.834
		data=((true==state.DMACBase ? state.DMAC.baseAddr : state.DMAC.currentAddr)>>24)&0xFF;
		break;
	case TOWNSIO_HIGHRESPCM_CLOCK:      //0x518, // [2] pp.834
		switch(state.freq)
		{
		case 48000:
			data=0;
			break;
		case 44100:
			data=1;
			break;
		case 32000:
			data=2;
			break;
		case 22050:
			data=3;
			break;
		case 19200:
			data=4;
			break;
		case 16000:
			data=5;
			break;
		case 11025:
			data=6;
			break;
		case 9600:
			data=7;
			break;
		case 8000:
			data=8;
			break;
		case 5513:
			data=9;
			break;
		case 24000:
			data=10;
			break;
		case 12000:
			data=11;
			break;
		}
		break;
	case TOWNSIO_HIGHRESPCM_MODE://         0x519, // [2] pp.835
		if(true==state.SNDFormat)
		{
			data|=0x20;
		}
		if(true==state.stereo)
		{
			data|=0x10;
		}
		if(true==state.bit16)
		{
			data|=0x08;
		}
		data|=state.conversionLevel;
		break;
	case TOWNSIO_HIGHRESPCM_SYSCONTROL://   0x51A, // [2] pp.836
		if(townsPtr->state.townsTime<state.resetTime)
		{
			data=0x80;
		}
		switch(state.interrupt)
		{
		case 4:
			break;
		case 5:
			data|=1;
			break;
		case 10:
			data|=2;
			break;
		case 14:
			data|=3;
			break;
		case 15:
			data|=4;
			break;
		default:
			data|=7;
			break;
		}
		if(true==state.dataPort)
		{
			data|=0x20;
		}
		if(true==state.recording)
		{
			data|=0x10;
		}
		break;
	case TOWNSIO_HIGHRESPCM_BUFFCONTROL://  0x51B, // [2] pp.837
		if(true==state.bufferIRQEnabled)
		{
			data|=0x80;
		}
		if(true==state.bufferOverrun)
		{
			data|=0x20;
		}
		if(true==state.bufferUnderrun)
		{
			data|=0x10;
		}
		break;
	case TOWNSIO_HIGHRESPCM_REC_PLAYBACK:// 0x51C, // [2] pp.838
		if(townsPtr->state.townsTime<state.resetTime)
		{
			data|=0xC0; // ~AD_RDY=1,  ~DA_RDY=1
		}
		if(true==state.audioIRQEnabled)
		{
			data|=0x20;
		}
		if(true==state.audioLevelDetected)
		{
			data|=0x10;
		}
		if(true==state.DREQMask)
		{
			data|=2;
		}
		if(true==state.bufferMask)
		{
			data|=1;
		}
		break;
	case TOWNSIO_HIGHRESPCM_REC_PEAK_MON:// 0x51D, // [2] pp.839
		return state.peakLevel;
		break;
	case TOWNSIO_HIGHRESPCM_DATA_LOW://     0x51E, // [2] pp.840
		break;
	case TOWNSIO_HIGHRESPCM_DATA_HIGH://    0x51F, // [2] pp.840
		break;
	}
	return data;
}

void TownsHighResPCM::ProcessDMA(void)
{
	if(true!=state.DREQMask && true!=state.bufferMask)
	{
		if(true!=state.recording) // isPlaying
		{
			auto bytesAvailable=state.DMAC.CountsAvailable()*2; // 16-bit transfer....
			if(0<bytesAvailable && state.dataBuffer.size()+bytesAvailable<INTERNAL_BUFFER_SIZE)
			{
				auto data=state.DMAC.MemoryToDevice(townsPtr,bytesAvailable);
				state.dataBuffer.insert(state.dataBuffer.end(),data.begin(),data.end());
				if(0<data.size())
				{
					state.DMAE=true;
					UpdatePIC();

					if(true!=state.DMAC.AUTI())
					{
						state.DREQMask=true;
					}
				}
			}
		}
		else
		{
			// Recording not supported yet.
		}
	}
}

void TownsHighResPCM::UpdatePIC(void)
{
	if(NO_INTERRUPT!=state.interrupt)
	{
		bool IRQ=(state.DMAEIRQEnabled && state.DMAE) ||
		         (state.bufferIRQEnabled && (state.bufferOverrun || state.bufferUnderrun)) ||
		         (state.audioIRQEnabled && state.audioLevelDetected);
		townsPtr->pic.SetInterruptRequestBit(state.interrupt,IRQ);
	}
}

class TownsHighResPCM::WAV8Mono
{
public:
	inline void Get(uint8_t sample[4],const uint8_t *incoming)
	{
		uint8_t data=*incoming;
		data-=0x80;
		sample[0]=data;
		sample[1]=data;
		sample[2]=data;
		sample[3]=data;
	}
	constexpr unsigned int BytesPerTimeStep(void) const
	{
		return 1;
	}
};
class TownsHighResPCM::WAV8Stereo
{
public:
	inline void Get(uint8_t sample[4],const uint8_t *incoming)
	{
		uint8_t data=incoming[0];
		data-=0x80;
		sample[0]=data;
		sample[1]=data;
		data=incoming[1];
		data-=0x80;
		sample[2]=data;
		sample[3]=data;
	}
	constexpr unsigned int BytesPerTimeStep(void) const
	{
		return 2;
	}
};
class TownsHighResPCM::WAV16Mono
{
public:
	inline void Get(uint8_t sample[4],const uint8_t *incoming)
	{
		sample[0]=incoming[0];
		sample[1]=incoming[1];
		sample[2]=incoming[0];
		sample[3]=incoming[1];
	}
	constexpr unsigned int BytesPerTimeStep(void) const
	{
		return 2;
	}
};
class TownsHighResPCM::WAV16Stereo
{
public:
	inline void Get(uint8_t sample[4],const uint8_t *incoming)
	{
		sample[0]=incoming[0];
		sample[1]=incoming[1];
		sample[2]=incoming[2];
		sample[3]=incoming[3];
	}
	constexpr unsigned int BytesPerTimeStep(void) const
	{
		return 4;
	}
};
class TownsHighResPCM::SND
{
public:
	inline void Get(uint8_t sample[4],const uint8_t *incoming)
	{
		auto data=*incoming;
		if(0!=(0x80&data))
		{
			uint8_t a=data&0x7F;
			data=0;
			data-=a;
		}
		sample[0]=data;
		sample[1]=data;
		sample[2]=data;
		sample[3]=data;
	}
	constexpr unsigned int BytesPerTimeStep(void) const
	{
		return 1;
	}
};
template <class Format>
class TownsHighResPCM::Populator
{
public:
	// Returns length used from the incoming data buffer.
	unsigned int Populate(int &balance,uint8_t output[],const std::vector <uint8_t> &incoming,unsigned int numSamples,unsigned int incomingFreq,unsigned int outputFreq)
	{
		const unsigned int WAVE_OUT_SAMPLING_RATE=outputFreq;

		Format fmt;
		unsigned int i=0,outPtr=0;
		while(i+fmt.BytesPerTimeStep()<=incoming.size() && 0<numSamples)
		{
			if(0<balance)
			{
				balance-=incomingFreq;
				uint8_t sample[4];
				fmt.Get(sample,incoming.data()+i);
				WordOp_Add(output+outPtr  ,cpputil::GetSignedWord(sample));
				WordOp_Add(output+outPtr+2,cpputil::GetSignedWord(sample+2));
				outPtr+=4;
				--numSamples;
			}
			else
			{
				balance+=WAVE_OUT_SAMPLING_RATE;
				i+=fmt.BytesPerTimeStep();
			}
		}

		return i;
	}
};

void TownsHighResPCM::DropWaveForNumSamples(unsigned int nSamples,unsigned int WAVE_OUT_SAMPLING_RATE)
{
	unsigned int used=0;
	if(true!=state.SNDFormat)
	{
		if(true==state.bit16)  // WAV16
		{
			if(true==state.stereo) // WAV16 Stereo
			{
				WAV16Stereo format;
				used=format.BytesPerTimeStep()*nSamples*state.freq/WAVE_OUT_SAMPLING_RATE;
			}
			else // WAV16 Monaural
			{
				WAV16Mono format;
				used=format.BytesPerTimeStep()*nSamples*state.freq/WAVE_OUT_SAMPLING_RATE;
			}
		}
		else // WAV8
		{
			if(true==state.stereo)
			{
				WAV8Stereo format;
				used=format.BytesPerTimeStep()*nSamples*state.freq/WAVE_OUT_SAMPLING_RATE;
			}
			else
			{
				WAV8Mono format;
				used=format.BytesPerTimeStep()*nSamples*state.freq/WAVE_OUT_SAMPLING_RATE;
			}
		}
	}
	else // SND
	{
		SND format;
		used=format.BytesPerTimeStep()*nSamples*state.freq/WAVE_OUT_SAMPLING_RATE;
	}
	state.dataBuffer.erase(state.dataBuffer.begin(),state.dataBuffer.begin()+used);
}
void TownsHighResPCM::AddWaveForNumSamples(uint8_t output[],unsigned int nSamples,unsigned int WAVE_OUT_SAMPLING_RATE)
{
	unsigned int used=0;
	if(true!=state.SNDFormat)
	{
		if(true==state.bit16)  // WAV16
		{
			if(true==state.stereo) // WAV16 Stereo
			{
				Populator <WAV16Stereo> populator;
				used=populator.Populate(state.balance,output,state.dataBuffer,nSamples,state.freq,WAVE_OUT_SAMPLING_RATE);
			}
			else // WAV16 Monaural
			{
				Populator <WAV16Mono> populator;
				used=populator.Populate(state.balance,output,state.dataBuffer,nSamples,state.freq,WAVE_OUT_SAMPLING_RATE);
			}
		}
		else // WAV8
		{
			if(true==state.stereo)
			{
				Populator <WAV8Stereo> populator;
				used=populator.Populate(state.balance,output,state.dataBuffer,nSamples,state.freq,WAVE_OUT_SAMPLING_RATE);
			}
			else
			{
				Populator <WAV8Mono> populator;
				used=populator.Populate(state.balance,output,state.dataBuffer,nSamples,state.freq,WAVE_OUT_SAMPLING_RATE);
			}
		}
	}
	else // SND
	{
		Populator <SND> populator;
		used=populator.Populate(state.balance,output,state.dataBuffer,nSamples,state.freq,WAVE_OUT_SAMPLING_RATE);
	}
	state.dataBuffer.erase(state.dataBuffer.begin(),state.dataBuffer.begin()+used);
}

////////////////////////////////////////////////////////////

std::vector <std::string> TownsHighResPCM::GetStatusText(void) const
{
	std::vector <std::string> text;

	text.push_back("DMAC");
	text.back()+=" MODE="+cpputil::Ubtox(state.DMAC.modeCtrl);
	text.back()+=" BASEAD="+cpputil::Uitox(state.DMAC.baseAddr);
	text.back()+=" CURRAD="+cpputil::Uitox(state.DMAC.currentAddr);
	text.back()+=" BASECT="+cpputil::Uitox(state.DMAC.baseCount);
	text.back()+=" CURRCT="+cpputil::Uitox(state.DMAC.currentCount);

	text.push_back("BASE=");
	text.back()+=(state.DMACBase ? "1" : "0");
	text.back()+=" DMAEIRQ=";
	text.back()+=(state.DMAEIRQEnabled ? "Enabled" : "Disabled");
	text.back()+=" DMAE=";
	text.back()+=(state.DMAE ? "1" : "0");

	text.push_back("BUFFERIRQ=");
	text.back()+=(state.bufferIRQEnabled ? "Enabled" : "Disabled");
	text.back()+=" UNDERRUN=";
	text.back()+=(state.bufferUnderrun ? "1" : "0");
	text.back()+=" OVERRUN=";
	text.back()+=(state.bufferOverrun ? "1" : "0");

	text.push_back("AUDIOIN_IRQ=");
	text.back()+=(state.audioIRQEnabled ? "Enabled" : "Disabled");
	text.back()+=" LEVEL_DETECTED=";
	text.back()+=(state.audioLevelDetected ? "1" : "0");

	text.push_back("AUDIO_FORMAT=");
	if(true==state.SNDFormat)
	{
		text.back()+="SND";
	}
	else
	{
		text.back()+="WAV";
		if(true==state.bit16)
		{
			text.back()+="16";
		}
		else
		{
			text.back()+="8";
		}
		if(true==state.stereo)
		{
			text.back()+=" STEREO";
		}
		else
		{
			text.back()+=" MONO";
		}
	}

	if(true==state.recording)
	{
		text.push_back("MODE=Recording");
	}
	else
	{
		text.push_back("MODE=Playback");
	}
	text.back()+=" FREQ=";
	text.back()+=cpputil::Uitoa(state.freq);
	text.back()+=" DATAPORT=";
	text.back()+=(true==state.dataPort ? " SOFT-TFR Port" : "LEVEL Monitor");

	text.push_back("DREQMask=");
	text.back()+=(state.DREQMask ? "1" : "0");
	text.back()+=" BufferMask=";
	text.back()+=(state.bufferMask ? "1" : "0");
	text.back()+=" INT=";
	text.back()+=cpputil::Uitoa(state.interrupt);

	text.push_back("Buffer Filled=");
	text.back()+=cpputil::Uitoa(state.dataBuffer.size());

	return text;
}

////////////////////////////////////////////////////////////

uint32_t TownsHighResPCM::SerializeVersion(void) const
{
	return 0;
}

void TownsHighResPCM::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
	PushBool(data,state.enabled);

	PushUint64(data,state.resetTime);
	PushUint32(data,state.freq);

	PushUint32(data,state.DMAC.baseCount);
	PushUint32(data,state.DMAC.currentCount);
	PushUint32(data,state.DMAC.baseAddr);
	PushUint32(data,state.DMAC.currentAddr);
	PushUint16(data,state.DMAC.modeCtrl);
	PushBool(data,state.DMAC.terminalCount);

	PushBool(data,state.DMACBase);
	PushBool(data,state.DMAE);
	PushBool(data,state.DMAEIRQEnabled);

	PushBool(data,state.bufferIRQEnabled);
	PushBool(data,state.bufferOverrun);
	PushBool(data,state.bufferUnderrun);

	PushBool(data,state.audioIRQEnabled);
	PushBool(data,state.audioLevelDetected);

	PushBool(data,state.SNDFormat); // WAV by default.
	PushBool(data,state.stereo);
	PushBool(data,state.bit16);

	PushBool(data,state.recording);
	PushBool(data,state.dataPort); // false:Level Monitor  true:Software-transfer port

	PushBool(data,state.DREQMask);
	PushBool(data,state.bufferMask);

	PushUint32(data,state.conversionLevel);

	PushUint16(data,state.interrupt);

	PushUint32(data,state.peakLevel);  // bit7 is Level Over.
	PushUint32(data,state.peakLevelThreshold);

	PushInt32(data,state.balance); // Balance for re-sampling.
	PushUcharArray(data,state.dataBuffer);
}

bool TownsHighResPCM::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	state.enabled=ReadBool(data);

	state.resetTime=ReadUint64(data);
	state.freq=ReadUint32(data);

	state.DMAC.baseCount=ReadUint32(data);
	state.DMAC.currentCount=ReadUint32(data);
	state.DMAC.baseAddr=ReadUint32(data);
	state.DMAC.currentAddr=ReadUint32(data);
	state.DMAC.modeCtrl=ReadUint16(data);
	state.DMAC.terminalCount=ReadBool(data);

	state.DMACBase=ReadBool(data);
	state.DMAE=ReadBool(data);
	state.DMAEIRQEnabled=ReadBool(data);

	state.bufferIRQEnabled=ReadBool(data);
	state.bufferOverrun=ReadBool(data);
	state.bufferUnderrun=ReadBool(data);

	state.audioIRQEnabled=ReadBool(data);
	state.audioLevelDetected=ReadBool(data);

	state.SNDFormat=ReadBool(data);
	state.stereo=ReadBool(data);
	state.bit16=ReadBool(data);

	state.recording=ReadBool(data);
	state.dataPort=ReadBool(data);

	state.DREQMask=ReadBool(data);
	state.bufferMask=ReadBool(data);

	state.conversionLevel=ReadUint32(data);

	state.interrupt=ReadUint16(data);

	state.peakLevel=ReadUint32(data);
	state.peakLevelThreshold=ReadUint32(data);

	state.balance=ReadInt32(data);
	state.dataBuffer=ReadUcharArray(data);

	return true;
}
