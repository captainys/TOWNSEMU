#include <iostream>

#include "cpputil.h"
#include "inout.h"



InOut::InOut()
{
	takeLog=false;
}
void InOut::EnableLog(void)
{
	takeLog=true;
}
void InOut::ClearLog(void)
{
	log.clear();
}

unsigned int InOut::In8(unsigned int port)
{
	unsigned int value=0xff;
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
	unsigned int value=0xff;
	// Read from appropriate device..
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
	unsigned int value=0xff;
	// Read from appropriate device..
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
	if(true==takeLog)
	{
		IOLog l;
		l.output=true;
		l.port=port;
		l.value=value;
		log.push_back(l);
	}
}
