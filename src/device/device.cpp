#include <iostream>

#include "device.h"


Device::Device()
{
	abort=false;
}

void Device::Abort(const char abortReason[]) const
{
	abort=true;
	this->abortReason=abortReason;
}

/* virtual */ void Device::Reset(void)
{
}

/* virtual */ bool Device::IOWriteByte(unsigned int ioport,unsigned int data)
{
	ioport;
	data;
	return false;
}
/* virtual */ bool Device::IOWriteWord(unsigned int ioport,unsigned int data)
{
	ioport;
	data;
	return false;
}
/* virtual */ bool Device::IOWriteDword(unsigned int ioport,unsigned int data)
{
	ioport;
	data;
	return false;
}

/* virtual */ Device::IORead Device::IOReadByte(unsigned int ioport)
{
	IORead ioread;
	ioread.valueReturned=false;
	ioread.value=0xffffffff;
	return ioread;
}
/* virtual */ Device::IORead Device::IOReadWord(unsigned int ioport)
{
	IORead ioread;
	ioread.valueReturned=false;
	ioread.value=0xffff;
	return ioread;
}
/* virtual */ Device::IORead Device::IOReadDword(unsigned int ioport)
{
	IORead ioread;
	ioread.valueReturned=false;
	ioread.value=0xff;
	return ioread;
}
