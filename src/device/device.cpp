#include <iostream>

#include "device.h"


Device::Device()
{
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
