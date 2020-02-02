#ifndef DEVICE_IS_INCLUDED
#define DEVICE_IS_INCLUDED
/* { */

#include <string>

class Device
{
public:
	mutable bool abort;
	mutable std::string abortReason;

	const unsigned long long int TIME_NO_SCHEDULE=~0;

	class CommonState
	{
	public:
		unsigned long long int deviceTime;

		/*! Time next task needs to be performed.
		    If no task scheduled, scheduledTime=TIME_NO_SCHEDULE.
		    RunScheduledTask won't be called unless townsTime passes scheduleTime.
		    In RunScheduleTask function for specific device, time should be reset to TIME_NOSCHEDULE
		    if no more task needs to be done.
		    It is set to TIME_NO_SCHEDULE in the constructor.
		*/
		unsigned long long int scheduleTime;
	};

	CommonState commonState;

	Device();

	void Abort(const std::string &abortReason) const;

	virtual const char *DeviceName(void) const=0;

	/*! Default behavior of PowerOn is just call Reset.
	*/
	virtual void PowerOn(void);
	virtual void Reset(void);

	/*! Default behavior is to set deviceTime to TIME_NO_SCHEDULE.
	*/
	virtual void RunScheduledTask(unsigned long long int townsTime);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte twice
	virtual void IOWriteDword(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte 4 times

	virtual unsigned int IOReadByte(unsigned int ioport);
	virtual unsigned int IOReadWord(unsigned int ioport); // Default behavior calls IOReadByte twice
	virtual unsigned int IOReadDword(unsigned int ioport); // Default behavior calls IOWriteByte 4 times
};

/* } */
#endif
