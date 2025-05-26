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

/* From the real REX3586.  ROM (IO 7010h) returns the following bits.  Low-bit first.

First 20 bits:  0xFFFF0
Next 40 bits: 0x?????????? (Mac address)
Next 16 bits: 0xA200
Next 152 bits:  All 1
Next 24 bits: 0x4C414E ("LAN")
Last 4 bits: All 0
*/

class RatocREX3586 : public Device
{
public:
	enum
	{
		NUM_REG_BANKS=4,
		NUM_REGS=0x20,
	};

	class State
	{
	public:
		unsigned char regs[NUM_REG_BANKS][NUM_REGS];
	};
	State state;

	virtual const char *DeviceName(void) const{return "REX3586";}

	RatocREX3586(class FMTownsCommon *ptr);

	void PowerOn(void) override;
	void Reset(void) override;

	void IOWriteByte(unsigned int ioport,unsigned int data) override;
	unsigned int IOReadByte(unsigned int ioport) override;

	uint32_t SerializeVersion(void) const override;
	void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const override;
	bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version) override;
};

/* } */
#endif
