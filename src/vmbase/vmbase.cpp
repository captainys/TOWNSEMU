/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "vmbase.h"



class VMBase::Device0 : public Device
{
public:
	Device0(VMBase *vmPtr);
	virtual const char *DeviceName(void) const{return "PlaceHolderForTaskScheduledDevicesChain";}
};

VMBase::Device0::Device0(VMBase *vmPtr) : Device(vmPtr)
{
}

////////////////////////////////////////////////////////////

VMBase::VMBase()
{
	dev0=new Device0(this);
	allDevices.push_back(dev0);
}
VMBase::~VMBase()
{
	delete dev0;
	dev0=nullptr;
}

void VMBase::ScheduleDeviceCallBack(Device &dev,long long int timer)
{
	dev.commonState.scheduleTime=timer;
	if(dev.vmPrevTaskScheduledDeviceIndex<0)
	{
		dev.vmPrevTaskScheduledDeviceIndex=0;
		dev.vmNextTaskScheduledDeviceIndex=allDevices[0]->vmNextTaskScheduledDeviceIndex;

		allDevices[0]->vmNextTaskScheduledDeviceIndex=dev.vmDeviceIndex;
		if(0<=dev.vmNextTaskScheduledDeviceIndex)
		{
			allDevices[dev.vmNextTaskScheduledDeviceIndex]->vmPrevTaskScheduledDeviceIndex=dev.vmDeviceIndex;
		}
	}
}
void VMBase::UnscheduleDeviceCallBack(Device &dev)
{
	dev.commonState.scheduleTime=dev.TIME_NO_SCHEDULE;
	if(0<=dev.vmPrevTaskScheduledDeviceIndex)
	{
		allDevices[dev.vmPrevTaskScheduledDeviceIndex]->vmNextTaskScheduledDeviceIndex=dev.vmNextTaskScheduledDeviceIndex;
		if(0<=dev.vmNextTaskScheduledDeviceIndex)
		{
			allDevices[dev.vmNextTaskScheduledDeviceIndex]->vmPrevTaskScheduledDeviceIndex=dev.vmPrevTaskScheduledDeviceIndex;
		}

		dev.vmPrevTaskScheduledDeviceIndex=-1;
		dev.vmNextTaskScheduledDeviceIndex=-1;
	}
}


void VMBase::CacheDeviceIndex(void)
{
	for(int i=0; i<allDevices.size(); ++i)
	{
		allDevices[i]->vmDeviceIndex=i;
	}
}

void VMBase::Abort(std::string devName,std::string abortReason)
{
	stopFlags|=STOP_ABORT;
	vmAbortDeviceName=devName;
	vmAbortReason=abortReason;
}

void VMBase::ClearAbortFlag(void)
{
	stopFlags&=~STOP_ABORT;
}
void VMBase::DebugBreak(void)
{
	stopFlags|=STOP_DEBUG_BREAK;
}
void VMBase::ClearDebugBreakFlag(void)
{
	stopFlags&=~STOP_DEBUG_BREAK;
}

std::vector <std::string> VMBase::GetScheduledTasksText(void) const
{
	std::vector <std::string> text;
	for(auto devIdx=dev0->vmNextTaskScheduledDeviceIndex; 0<=devIdx; devIdx=allDevices[devIdx]->vmNextTaskScheduledDeviceIndex)
	{
		auto ptr=allDevices[devIdx];

		text.push_back("");
		text.back()+=ptr->DeviceName();
		text.back().push_back(' ');
		while(text.back().size()<16)
		{
			text.back().push_back(' ');
		}
		text.back()+=cpputil::Uitoa(ptr->commonState.scheduleTime/1000000);
		text.back()+="(ms)";
	}
	return text;
}
