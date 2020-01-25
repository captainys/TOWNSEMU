#ifndef INOUT_IS_INCLUDED
#define INOUT_IS_INCLUDED
/* { */

#include <vector>

#include "device.h"

class InOut
{
protected:
	enum
	{
		NUM_IO_ADDR=0x10000
	};
	std::vector <Device *> ioMap;

public:
	class IOLog
	{
	public:
		bool output;
		unsigned int port;
		unsigned int value;
	};

	bool takeLog;
	std::vector <IOLog> log;

	InOut();
	void EnableLog(void);
	void DisableLog(void);

	void ClearLog(void);

	void AddDevice(Device *devPtr,unsigned int minIOPort,unsigned int maxIOPort);

	unsigned int In8(unsigned int port);
	unsigned int In16(unsigned int port);
	unsigned int In32(unsigned int port);

	void Out8(unsigned int port,unsigned int value);
	void Out16(unsigned int port,unsigned int value);
	void Out32(unsigned int port,unsigned int value);
};

/* } */
#endif
