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
void InOut::Out8(unsigned int port,unsigned int value)
{
	std::cout << "Write IO:[" << cpputil::Ustox(port) << "] " << cpputil::Ubtox(value) << std::endl;
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
	std::cout << "Write IO:[" << cpputil::Ustox(port) << "] " << cpputil::Ustox(value) << std::endl;
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
	std::cout << "Write IO:[" << cpputil::Ustox(port) << "] " << cpputil::Uitox(value) << std::endl;
	if(true==takeLog)
	{
		IOLog l;
		l.output=true;
		l.port=port;
		l.value=value;
		log.push_back(l);
	}
}
