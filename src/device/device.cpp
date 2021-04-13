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
#include "cpputil.h"

Device::Device(VMBase *vmPtr)
{
	this->vmPtr=vmPtr;
	commonState.deviceTime=0;
	commonState.scheduleTime=TIME_NO_SCHEDULE;
}

void Device::Abort(const std::string &abortReason) const
{
	vmPtr->Abort(DeviceName(),abortReason);
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

/* virtual */ std::vector <unsigned char> Device::Serialize(std::string) const
{
	std::vector <unsigned char> empty;
	BeginSerialization(empty,~0);
	return empty;
}
/* virtual */ bool Device::Deserialize(const std::vector <unsigned char> &,std::string)
{
	return false;
}
void Device::BeginSerialization(std::vector <unsigned char> &buf,unsigned int version) const
{
	buf.resize(36);
	for(auto &b : buf)
	{
		b=0;
	}

	cpputil::PutDword(buf.data()+32,version);

	std::string devName=DeviceName();
	if(31<devName.size())
	{
		std::cout << "Device Name exceeds 31 letters." << std::endl;
		return;
	}

	for(int i=0; i<devName.size(); ++i)
	{
		buf[i]=(unsigned char)devName[i];
	}
}

/* static */ void Device::PushInt64(std::vector <unsigned char> &buf,int64_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+8);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
	buf[len+2]=(data>>16)&0xff;
	buf[len+3]=(data>>24)&0xff;
	buf[len+4]=(data>>32)&0xff;
	buf[len+5]=(data>>40)&0xff;
	buf[len+6]=(data>>48)&0xff;
	buf[len+7]=(data>>56)&0xff;
}
/* static */ void Device::PushUint64(std::vector <unsigned char> &buf,uint64_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+8);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
	buf[len+2]=(data>>16)&0xff;
	buf[len+3]=(data>>24)&0xff;
	buf[len+4]=(data>>32)&0xff;
	buf[len+5]=(data>>40)&0xff;
	buf[len+6]=(data>>48)&0xff;
	buf[len+7]=(data>>56)&0xff;
}
/* static */ void Device::PushInt32(std::vector <unsigned char> &buf,int32_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+4);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
	buf[len+2]=(data>>16)&0xff;
	buf[len+3]=(data>>24)&0xff;
}
/* static */ void Device::PushUint32(std::vector <unsigned char> &buf,uint32_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+4);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
	buf[len+2]=(data>>16)&0xff;
	buf[len+3]=(data>>24)&0xff;
}
/* static */ void Device::PushInt16(std::vector <unsigned char> &buf,int16_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+2);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
}
/* static */ void Device::PushUint16(std::vector <unsigned char> &buf,uint16_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+2);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
}
/* static */ void Device::PushBool(std::vector <unsigned char> &buf,bool flag)
{
	auto len=buf.size();
	buf.resize(buf.size()+1);
	buf[len  ]= flag;
}

/* static */ uint64_t Device::ReadUint64(const unsigned char *&data)
{
	uint64_t v;
	v= data[0]|
	  (data[1]<<8)|
	  (data[2]<<16)|
	  (data[3]<<24)|
	  (data[4]<<32)|
	  (data[5]<<40)|
	  (data[6]<<48)|
	  (data[7]<<56);
	data+=8;
	return v;
}
/* static */ int64_t Device::ReadInt64(const unsigned char *&data)
{
	int64_t v;
	v= data[0]|
	  (data[1]<<8)|
	  (data[2]<<16)|
	  (data[3]<<24)|
	  (data[4]<<32)|
	  (data[5]<<40)|
	  (data[6]<<48)|
	  (data[7]<<56);
	data+=8;
	return v;
}
/* static */ uint32_t Device::ReadUint32(const unsigned char *&data)
{
	uint32_t v;
	v= data[0]|
	  (data[1]<<8)|
	  (data[2]<<16)|
	  (data[3]<<24);
	data+=4;
	return v;
}
/* static */ int32_t Device::ReadInt32(const unsigned char *&data)
{
	int32_t v;
	v= data[0]|
	  (data[1]<<8)|
	  (data[2]<<16)|
	  (data[3]<<24);
	data+=4;
	return v;
}
/* static */ uint16_t Device::ReadUint16(const unsigned char *&data)
{
	uint16_t v;
	v= data[0]|
	  (data[1]<<8);
	data+=2;
	return v;
}
/* static */ int16_t Device::ReadInt16(const unsigned char *&data)
{
	int16_t v;
	v= data[0]|
	  (data[1]<<8);
	data+=2;
	return v;
}
/* static */ bool Device::ReadBool(const unsigned char *&data)
{
	bool v=(0!=*data);
	++data;
	return v;
}
