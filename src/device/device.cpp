/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>

#include "device.h"


Device::Device(VMBase *vmPtr)
{
	this->vmPtr=vmPtr;
	commonState.deviceTime=0;
	commonState.scheduleTime=TIME_NO_SCHEDULE;
	abort=false;
}

void Device::Abort(const std::string &abortReason) const
{
	abort=true;
	this->abortReason=abortReason;
}

/* virtual */ void Device::PowerOn(void)
{
	Reset();
}

/* virtual */ void Device::Reset(void)
{
}

/* virtual */ void Device::RunScheduledTask(unsigned long long int townsTime)
{
	commonState.scheduleTime=TIME_NO_SCHEDULE;
}

/* virtual */ void Device::IOWriteByte(unsigned int ioport,unsigned int data)
{
	ioport;
	data;
}
/* virtual */ void Device::IOWriteWord(unsigned int ioport,unsigned int data)
{
	IOWriteByte(ioport,data&255);
	IOWriteByte(ioport+1,(data>>8)&255);
}
/* virtual */ void Device::IOWriteDword(unsigned int ioport,unsigned int data)
{
	IOWriteByte(ioport,data&255);
	IOWriteByte(ioport+1,(data>>8)&255);
	IOWriteByte(ioport+2,(data>>16)&255);
	IOWriteByte(ioport+3,(data>>24)&255);
}

/* virtual */ unsigned int Device::IOReadByte(unsigned int ioport)
{
	return 0xff;
}
/* virtual */ unsigned int Device::IOReadWord(unsigned int ioport)
{
	return IOReadByte(ioport)|(IOReadByte(ioport+1)<<8);
}
/* virtual */ unsigned int Device::IOReadDword(unsigned int ioport)
{
	return IOReadByte(ioport)|(IOReadByte(ioport+1)<<8)|(IOReadByte(ioport+2)<<16)|(IOReadByte(ioport+3)<<24);
}
