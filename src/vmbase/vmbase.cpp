#include "vmbase.h"
#include "device.h"



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

void VMBase::RunScheduledTasks(long long int vmTime)
{
	for(auto devIndex=allDevices[0]->vmNextTaskScheduledDeviceIndex;
	    0<=devIndex;
	    devIndex=allDevices[devIndex]->vmNextTaskScheduledDeviceIndex)
	{
		auto devPtr=allDevices[devIndex];
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
	vmAbort=true;
	vmAbortDeviceName=devName;
	vmAbortReason=abortReason;
}
