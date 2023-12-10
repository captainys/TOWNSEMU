#ifndef HIGHRESPCM_IS_INCLUDED
#define HIGHRESPCM_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include <stdint.h>
#include "dmac.h"

class TownsHighResPCM : public Device
{
private:
	class FMTownsCommon *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "HighResPCM";}

	enum
	{
		RESET_TIME=1000,
		NO_INTERRUPT=0xff,
		DMA_CHECK_INTERVAL=500000, // Every 500us
		INTERNAL_BUFFER_SIZE=8192, // There is no info about the size of High-Res PCM internal buffer.
		                           // Confirmed 1024 is too small.
		                           // Maybe 8KB is reasonable for a 1993 sound chip.
	};

	class State
	{
	public:
		bool enabled=false;

		uint64_t resetTime=0;
		unsigned int freq=19200;

		TownsDMAC::State::Channel DMAC;
		bool DMACBase=false;             // 0510H bit 0
		bool DMAE=false;
		bool DMAEIRQEnabled=false;

		bool bufferIRQEnabled=false;
		bool bufferOverrun=false,bufferUnderrun=false;

		bool audioIRQEnabled=false;
		bool audioLevelDetected=false;

		bool SNDFormat=false; // WAV by default.
		bool stereo=false;
		bool bit16=false;

		bool recording=false;
		bool dataPort=false; // false:Level Monitor  true:Software-transfer port

		bool DREQMask=false;
		bool bufferMask=false;

		unsigned int conversionLevel=0;

		unsigned int interrupt=NO_INTERRUPT;

		unsigned int peakLevel=0;  // bit7 is Level Over.
		unsigned int peakLevelThreshold=0;

		int balance=0; // Balance for re-sampling.
		std::vector <uint8_t> dataBuffer;

		void PowerOn(void);
		void Reset(void);
	};
	State state;

	class Variables
	{
	public:
		bool mute=false;
	};
	Variables var;

	TownsHighResPCM(FMTownsCommon *townsPtr);
	void PowerOn(void);
	void Reset(void);

	void RunScheduledTask(unsigned long long int townsTime) override;

	void IOWriteByte(unsigned int ioport,unsigned int data) override;

	unsigned int IOReadByte(unsigned int ioport) override;

	void ProcessDMA(void);
	void UpdatePIC(void);

	class WAV8Mono;
	class WAV8Stereo;
	class WAV16Mono;
	class WAV16Stereo;
	class SND;
	template <class Format>
	class Populator;

	void DropWaveForNumSamples(unsigned int nSamples,unsigned int WAVE_OUT_SAMPLING_RATE);
	void AddWaveForNumSamples(uint8_t output[],unsigned int nSamples,unsigned int WAVE_OUT_SAMPLING_RATE);

	std::vector <std::string> GetStatusText(void) const;

	uint32_t SerializeVersion(void) const override;
	void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const override;
	bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version) override;
};


/* } */
#endif
