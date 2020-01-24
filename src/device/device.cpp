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
	ioport;
	data;
}
/* virtual */ void Device::IOWriteDword(unsigned int ioport,unsigned int data)
{
	ioport;
	data;
}

/* virtual */ unsigned int Device::IOReadByte(unsigned int ioport)
{
	return 0xff;
}
/* virtual */ unsigned int Device::IOReadWord(unsigned int ioport)
{
	return 0xffff;
}
/* virtual */ unsigned int Device::IOReadDword(unsigned int ioport)
{
	return 0xffffffff;
}
