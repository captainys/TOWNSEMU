/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef VMBASE_IS_INCLUDED
#define VMBASE_IS_INCLUDED
/* { */



#include <vector>
#include <string>
#include "device.h"
#include "cpputil.h"



class VMBase
{
private:
	mutable bool vmAbort=false;

protected:
	/*! Device0 is a dummy device.  Always at allDevices[0].
	    Used as a head pointer to the task-scheduled device chain.
	*/
	class Device0;
	Device0 *dev0;

	std::vector <class Device *> allDevices;

public:
	mutable std::string vmAbortDeviceName,vmAbortReason;

	VMBase();
	~VMBase();

	/*! Run scheduled tasks.
	*/
	inline void RunScheduledTasks(long long int vmTime);
	/*!
	*/
	void ScheduleDeviceCallBack(class Device &dev,long long int timer);
	/*!
	*/
	void UnscheduleDeviceCallBack(class Device &dev);

	void CacheDeviceIndex(void);

	virtual void Abort(std::string devName,std::string abortReason);

	inline bool CheckAbort(void) const
	{
		return vmAbort;
	}

	void ClearAbortFlag(void);

	std::vector <std::string> GetScheduledTasksText(void) const;
};

inline void VMBase::RunScheduledTasks(long long int vmTime)
{
	Device *devPtr=nullptr;
	for(auto devIndex=allDevices[0]->vmNextTaskScheduledDeviceIndex;
	    0<=devIndex;
	    devIndex=devPtr->vmNextTaskScheduledDeviceIndex)
	{
		devPtr=allDevices[devIndex];
		if(devPtr->commonState.scheduleTime<=vmTime)
		{
			// Device may make another schedule in the call back.
			// UnscheduleDeviceCallBack must not wipe a new schedule.
			// Therefore, UnscheduleDeviceCallBack and then RunScheduledTask.
			// Not the other way round.
			UnscheduleDeviceCallBack(*devPtr);
			devPtr->RunScheduledTask(vmTime);
		}
	}
}



/* } */
#endif
