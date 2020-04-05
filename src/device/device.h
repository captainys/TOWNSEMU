/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef DEVICE_IS_INCLUDED
#define DEVICE_IS_INCLUDED
/* { */

#include <string>

#include "vmbase.h"

class Device
{
protected:
	VMBase *vmPtr;

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

	Device(VMBase *);

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
