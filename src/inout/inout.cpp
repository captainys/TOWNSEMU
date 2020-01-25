#include <iostream>

#include "cpputil.h"
#include "inout.h"



InOut::InOut()
{
	takeLog=false;
	ioMap.resize(NUM_IO_ADDR);
	for(auto &devPtr : ioMap)
	{
		devPtr=nullptr;
	}
}
void InOut::EnableLog(void)
{
	takeLog=true;
}
void InOut::DisableLog(void)
{
	takeLog=false;
}
void InOut::ClearLog(void)
{
	log.clear();
}

void InOut::AddDevice(Device *devPtr,unsigned int minIOPort,unsigned int maxIOPort)
{
	for(auto port=minIOPort; port<NUM_IO_ADDR && port<=maxIOPort; ++port)
	{
		ioMap[port]=devPtr;
	}
}

void InOut::AddDevice(Device *devPtr,unsigned int ioPort)
{
	if(ioPort<NUM_IO_ADDR)
	{
		ioMap[ioPort]=devPtr;
	}
}

unsigned int InOut::In8(unsigned int port)
{
	unsigned int value=0xff;

	if(port<NUM_IO_ADDR && nullptr!=ioMap[port])
	{
		value=ioMap[port]->IOReadByte(port);
	}

	// Read from appropriate device..
	std::cout << "Read IO8:[" << cpputil::Ustox(port) << "] " << cpputil::Ubtox(value) << std::endl;
	if(true==takeLog)
	{
		IOLog l;
		l.output=false;
		l.port=port;
		l.value=value;
		log.push_back(l);
	}
	return value;
}

unsigned int InOut::In16(unsigned int port)
{
	unsigned int value=0xffff;

	if(port<NUM_IO_ADDR && nullptr!=ioMap[port])
	{
		value=ioMap[port]->IOReadWord(port);
	}

	std::cout << "Read IO16:[" << cpputil::Ustox(port) << "] " << cpputil::Ubtox(value) << std::endl;
	if(true==takeLog)
	{
		IOLog l;
		l.output=false;
		l.port=port;
		l.value=value;
		log.push_back(l);
	}
	return value;
}

unsigned int InOut::In32(unsigned int port)
{
	unsigned int value=0xffffffff;

	if(port<NUM_IO_ADDR && nullptr!=ioMap[port])
	{
		value=ioMap[port]->IOReadDword(port);
	}

	std::cout << "Read IO32:[" << cpputil::Ustox(port) << "] " << cpputil::Ubtox(value) << std::endl;
	if(true==takeLog)
	{
		IOLog l;
		l.output=false;
		l.port=port;
		l.value=value;
		log.push_back(l);
	}
	return value;
}

void InOut::Out8(unsigned int port,unsigned int value)
{
	std::cout << "Write IO8:[" << cpputil::Ustox(port) << "] " << cpputil::Ubtox(value) << std::endl;

	if(port<NUM_IO_ADDR && nullptr!=ioMap[port])
	{
		ioMap[port]->IOWriteByte(port,value);
	}

	if(true==takeLog)
	{
		IOLog l;
		l.output=true;
		l.port=port;
		l.value=value;
		log.push_back(l);
	}
}
void InOut::Out16(unsigned int port,unsigned int value)
{
	std::cout << "Write IO16:[" << cpputil::Ustox(port) << "] " << cpputil::Ustox(value) << std::endl;

	if(port<NUM_IO_ADDR && nullptr!=ioMap[port])
	{
		ioMap[port]->IOWriteWord(port,value);
	}

	if(true==takeLog)
	{
		IOLog l;
		l.output=true;
		l.port=port;
		l.value=value;
		log.push_back(l);
	}
}
void InOut::Out32(unsigned int port,unsigned int value)
{
	std::cout << "Write IO32:[" << cpputil::Ustox(port) << "] " << cpputil::Uitox(value) << std::endl;

	if(port<NUM_IO_ADDR && nullptr!=ioMap[port])
	{
		ioMap[port]->IOWriteDword(port,value);
	}

	if(true==takeLog)
	{
		IOLog l;
		l.output=true;
		l.port=port;
		l.value=value;
		log.push_back(l);
	}
}
