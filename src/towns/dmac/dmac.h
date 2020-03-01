#ifndef DMAC_IS_INCLUDED
#define DMAC_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include "device.h"

class TownsDMAC : public Device
{
private:
	class FMTowns *townsPtr;
public:
	virtual const char *DeviceName(void) const{return "DMAC";}

	enum
	{
		DEVICE_CONTROL_LOW_FIXED=0x00100000
	};

	class State
	{
	public:
		class Channel
		{
		public:
			unsigned int baseCount;    // 00A2H and 00A3H
			unsigned int currentCount; // 00A2H and 00A3H
			unsigned int baseAddr;     // 00A4 to 00A7
			unsigned int currentAddr;  // 00A4 to 00A7
			unsigned char modeCtrl;    // 00AA

			bool AUTI(void) const; // Auto re-initialize
		};

		unsigned int bitSize;      // 00A0H 8 or 16
		bool BASE;                 // 00A1H
		unsigned int SELCH;        // 00A1H
		unsigned char devCtrl[2];  // 00A8 and 00A9
		unsigned char temporaryReg[2]; // 00AB and 00AC What are they?
		unsigned char req;
		unsigned char mask;
		Channel ch[4];

		void PowerOn(void);
		void Reset(void);
	};

	State state;

	bool debugBreakOnDMACRequest;

	TownsDMAC(class FMTowns *townsPtr)
	{
		this->townsPtr=townsPtr;
		debugBreakOnDMACRequest=false;
	}

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);

	/*! Returns a pointer to the DMA channel, or nullptr if the channel is unavailable.
	*/
	State::Channel *GetDMAChannel(unsigned int ch);
	const State::Channel *GetDMAChannel(unsigned int ch) const;

	/*! Transfer data from device to memory.
	    Returns number of bytes written to memory.
	*/
	unsigned int DeviceToMemory(State::Channel *DMACh,const std::vector <unsigned char> &dat);
	/*! Transfer data from memory to device.
	    Returns number of bytes read from memory.
	*/
	std::vector <unsigned char> MemoryToDevice(State::Channel *DMACh,unsigned int length);

	std::vector <std::string> GetStateText(void) const;
};


/* } */
#endif
