/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef IORAM_IS_INCLUDED
#define IORAM_IS_INCLUDED
/* { */

#include <vector>

#include "device.h"

/*! Unpublished specification?
    FM Towns IO Port 3000H to 3FFFH(?) just seem to work as a RAM.
*/
class IORam : public Device
{
public:
	class State
	{
	public:
		std::vector <unsigned int> RAM;
	};

	enum
	{
		NUM_PORTS=0x1000,
		PORT_TOP=0x3000
	};

	State state;

	virtual const char *DeviceName(void) const{return "IO_RAM";}

	IORam();

	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data);
	virtual void IOWriteDword(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);
	virtual unsigned int IOReadWord(unsigned int ioport);
	virtual unsigned int IOReadDword(unsigned int ioport);
};

/* } */
#endif
