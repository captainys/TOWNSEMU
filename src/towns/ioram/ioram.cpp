#include "ioram.h"

IORam::IORam()
{
	state.RAM.resize(NUM_PORTS);
	Reset();
}

/* virtual */ void IORam::Reset(void)
{
	for(auto &value : state.RAM)
	{
		value=0;
	}
}

/* virtual */ void IORam::IOWriteByte(unsigned int ioport,unsigned int data)
{
	ioport-=PORT_TOP;
	if(ioport<NUM_PORTS)
	{
		state.RAM[ioport]=data;
	}
}

/* virtual */ void IORam::IOWriteWord(unsigned int ioport,unsigned int data)
{
	ioport-=PORT_TOP;
	if(ioport<NUM_PORTS)
	{
		state.RAM[ioport]=data;
	}
}

/* virtual */ void IORam::IOWriteDword(unsigned int ioport,unsigned int data)
{
	ioport-=PORT_TOP;
	if(ioport<NUM_PORTS)
	{
		state.RAM[ioport]=data;
	}
}

/* virtual */ unsigned int IORam::IOReadByte(unsigned int ioport)
{
	ioport-=PORT_TOP;
	if(ioport<NUM_PORTS)
	{
		return state.RAM[ioport];
	}
	return 0xff;
}

/* virtual */ unsigned int IORam::IOReadWord(unsigned int ioport)
{
	ioport-=PORT_TOP;
	if(ioport<NUM_PORTS)
	{
		return state.RAM[ioport];
	}
	return 0xffff;
}

/* virtual */ unsigned int IORam::IOReadDword(unsigned int ioport)
{
	ioport-=PORT_TOP;
	if(ioport<NUM_PORTS)
	{
		return state.RAM[ioport];
	}
	return 0xffffffff;
}
