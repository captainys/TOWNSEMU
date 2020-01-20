#ifndef INOUT_IS_INCLUDED
#define INOUT_IS_INCLUDED
/* { */

#include <vector>

#include "device.h"

class InOut : public Device
{
public:
	virtual const char *DeviceName(void) const{return "IO";}

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

	void ClearLog(void);

	unsigned int In8(unsigned int port);
	unsigned int In16(unsigned int port);
	unsigned int In32(unsigned int port);

	void Out8(unsigned int port,unsigned int value);
	void Out16(unsigned int port,unsigned int value);
	void Out32(unsigned int port,unsigned int value);
};

/* } */
#endif
