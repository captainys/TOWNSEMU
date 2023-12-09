#ifndef HIGHRESPCM_IS_INCLUDED
#define HIGHRESPCM_IS_INCLUDED
/* { */

#include <vector>
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

		bool audioInIRQ=false,bufferIRQ=false;

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

		std::vector <uint8_t> dataBuffer;

		void PowerOn(void);
		void Reset(void);
	};
	State state;

	TownsHighResPCM(FMTownsCommon *townsPtr);
	void PowerOn(void);
	void Reset(void);

	void RunScheduledTask(unsigned long long int townsTime) override;

	void IOWriteByte(unsigned int ioport,unsigned int data) override;

	unsigned int IOReadByte(unsigned int ioport) override;

	void ProcessDMA(void);
	void UpdatePIC(void);

	uint32_t SerializeVersion(void) const override;
	void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const override;
	bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version) override;
};


/* } */
#endif
