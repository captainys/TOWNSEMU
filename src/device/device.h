#ifndef DEVICE_IS_INCLUDED
#define DEVICE_IS_INCLUDED
/* { */

#include <string>

class Device
{
public:
	mutable bool abort;
	mutable std::string abortReason;

	long long int deviceTime;

	Device();

	void Abort(const std::string &abortReason) const;

	virtual const char *DeviceName(void) const=0;

	virtual void Reset(void);

	virtual bool IOWriteByte(unsigned int ioport,unsigned int data);
	virtual bool IOWriteWord(unsigned int ioport,unsigned int data);
	virtual bool IOWriteDword(unsigned int ioport,unsigned int data);

	class IORead
	{
	public:
		bool valueReturned;
		unsigned int value;
	};

	virtual IORead IOReadByte(unsigned int ioport);
	virtual IORead IOReadWord(unsigned int ioport);
	virtual IORead IOReadDword(unsigned int ioport);
};


/* } */
#endif
