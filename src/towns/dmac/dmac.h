/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef DMAC_IS_INCLUDED
#define DMAC_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include "device.h"

class TownsDMAC : public Device
{
private:
	class FMTownsCommon *townsPtr;
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

			bool terminalCount;        // 00AB

			bool AUTI(void) const; // Auto re-initialize

			/*! Transfer data from device to memory.
			    Returns number of bytes written to memory.
			*/
			unsigned int DeviceToMemory(FMTownsCommon *townsPtr,unsigned long long len,const unsigned char dat[]);
			/*! Transfer data from memory to device.
			    Returns number of bytes read from memory.
			*/
			std::vector <unsigned char> MemoryToDevice(FMTownsCommon *townsPtr,unsigned int length);
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

	TownsDMAC(class FMTownsCommon *townsPtr);

	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);

	/*! Returns a pointer to the DMA channel, or nullptr if the channel is unavailable.
	*/
	State::Channel *GetDMAChannel(unsigned int ch);
	const State::Channel *GetDMAChannel(unsigned int ch) const;

	/*! Emulate ~END signal input to 71071.
	*/
	void SetDMATransferEnd(unsigned int chNum);

	/*! Transfer data from device to memory.
	    Returns number of bytes written to memory.
	*/
	unsigned int DeviceToMemory(State::Channel *DMACh,const std::vector <unsigned char> &dat);
	unsigned int DeviceToMemory(State::Channel *DMACh,unsigned long long len,const unsigned char dat[]);
	/*! Transfer data from memory to device.
	    Returns number of bytes read from memory.
	*/
	std::vector <unsigned char> MemoryToDevice(State::Channel *DMACh,unsigned int length);

	std::vector <std::string> GetStateText(void) const;


	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);
};


/* } */
#endif
