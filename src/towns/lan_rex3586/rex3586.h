/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef REX3586_IS_INCLUDED
#define REX3586_IS_INCLUDED
/* { */


#include <vector>
#include <cstdint>

#include "device.h"
#include "cheapmath.h"
#include "virtual_network.h"


/* From the real REX3586.  ROM (IO 7010h) returns the following bits.  Low-bit first.

First 4 bits: 0
Next 16 bits:  0xFFFF
Next 48 bits: 0x?????????? (Mac address)
Next  8 bits: 0xA2
Next 152 bits:  All 1
Next 24 bits: 0x4C414E ("LAN")
Last 4 bits: All 0
*/

class RatocREX3586 : public Device, public VirtualNetwork::PacketReceiver
{
public:
	enum
	{
		NUM_REG_BANKS=4,
		NUM_REGS=0x20,
	};

	enum
	{
		TXSTATE_TXDONE=0x80,
		TXSTATE_NETBUSY=0x40,
		TXSTATE_TXPKTRCD=0x20,
		TXSTATE_CRLOST=0x10,
		TXSTATE_JABBER=0x08,
		TXSTATE_COL=0x04,
		TXSTATE_COL16=0x02,

		RXSTATE_RXPKT=0x80,
		RXSTATE_BUS_TS_ERR=0x40,
		RXSTATE_DMA_EOP=0x20,
		RXSTATE_RMT_9900H=0x10,
		RXSTATE_SHORT_PKT=0x08,
		RXSTATE_ALIGN_ERR=0x04,
		RXSTATE_CRC_ERR=0x02,
		RXSTATE_OVERFLOW=0x01,
	};

	class State
	{
	public:
		unsigned int ROMReadPtr=0;
		unsigned char regs[NUM_REG_BANKS][NUM_REGS];
		uint64_t MAC=0x00C0D0350582LL; // Is Windows 95 driver assuming MAC starts with 00C0D0 or 00803D?
		// 0x464C59494E47; // "FLYING"
		uint8_t config[2]={0,0};
		bool TXIntEN=false,RXIntEN=false;
		uint8_t INTNum=4; // 4 by REX3586.COM,  10 by Linux driver.  Apparently controlled by 7010H

		uint8_t txState=0,rxState=0;
		
		unsigned int GetCurrentRegisterBank(void) const // Apparently Config 1 Bits 2-3 are the bank.
		{
			return (config[1]>>2)&3;
		}

		std::vector <uint8_t> TXPacket,RXPacket;
	};
	State state;

	class Variable
	{
	public:
		bool enabled=false;
		bool monitorTxPacket=true;
		bool monitorRxPacket=true;
	};
	Variable var;

	VirtualNetwork net;
	class RealNetwork *realNet;

	virtual const char *DeviceName(void) const{return "REX3586";}

	RatocREX3586(class FMTownsCommon *ptr);
	~RatocREX3586();

	void StartRealNetwork(void);

	inline void Polling(void)
	{
		if(true==var.enabled)
		{
			RealPolling();
		}
	}

	void RealPolling(void);

	void UpdatePIC(void);

	void ReceivePacket(size_t len,const uint8_t data[]) override;
	bool RxReady(void) const override;

	void PowerOn(void) override;
	void Reset(void) override;

	std::vector <std::string> GetStatusText(void) const;

	void IOWriteByte(unsigned int ioport,unsigned int data) override;
	unsigned int IOReadByte(unsigned int ioport) override;

	unsigned int IOReadWord(unsigned int ioport) override;

	uint32_t SerializeVersion(void) const override;
	void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const override;
	bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version) override;
};

/* } */
#endif
