#ifndef HIGHRESPCM_IS_INCLUDED
#define HIGHRESPCM_IS_INCLUDED
/* { */

#include <vector>
#include "dmac.h"

class TownsHighResPCM : public Device
{
private:
	class FMTownsCommon *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "HighResPCM";}

	class State
	{
	public:
		bool enabled=false;

		TownsDMAC::State::Channel DMAC;
		bool DMACBase=false;             // 0510H bit 0

		bool AudioInIRQ=false,bufferIRQ=false,DMAIRA=false;

		bool SNDFormat=false; // WAV by default.
		bool stereo=false;
		bool bit16=false;

		void PowerOn(void);
		void Reset(void);
	};
	State state;

	TownsHighResPCM(FMTownsCommon *townsPtr);
	void PowerOn(void);
	void Reset(void);

	void IOWriteByte(unsigned int ioport,unsigned int data) override;

	unsigned int IOReadByte(unsigned int ioport) override;

	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};


/* } */
#endif
