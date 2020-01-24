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

	/*! Default behavior of PowerOn is just call Reset.
	*/
	virtual void PowerOn(void);
	virtual void Reset(void);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data);
	virtual void IOWriteDword(unsigned int ioport,unsigned int data);

	virtual unsigned int IOReadByte(unsigned int ioport);
	virtual unsigned int IOReadWord(unsigned int ioport);
	virtual unsigned int IOReadDword(unsigned int ioport);
};


/* } */
#endif
