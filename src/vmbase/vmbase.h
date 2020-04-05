#ifndef VMBASE_IS_INCLUDED
#define VMBASE_IS_INCLUDED
/* { */



#include <vector>
#include <string>

class VMBase
{
protected:
	/*! Device0 is a dummy device.  Always at allDevices[0].
	    Used as a head pointer to the task-scheduled device chain.
	*/
	class Device0;
	Device0 *dev0;

	std::vector <class Device *> allDevices;

public:
	mutable bool vmAbort=false;
	mutable std::string vmAbortDeviceName,vmAbortReason;

	VMBase();
	~VMBase();

	/*! Run scheduled tasks.
	*/
	void RunScheduledTasks(long long int vmTime);
	/*!
	*/
	void ScheduleDeviceCallBack(class Device &dev,long long int timer);
	/*!
	*/
	void UnscheduleDeviceCallBack(class Device &dev);

	void CacheDeviceIndex(void);

	void Abort(std::string devName,std::string abortReason);

	inline bool CheckAbort(void) const
	{
		return vmAbort;
	}
};



/* } */
#endif
