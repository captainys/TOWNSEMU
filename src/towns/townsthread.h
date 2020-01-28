#ifndef TOWNSTHREAD_IS_INCLUDED
#define TOWNSTHREAD_IS_INCLUDED
/* { */


#include <thread>
#include <mutex>

#include "towns.h"

class TownsThread
{
private:
	FMTowns *townsPtr;
	int runMode;

public:
	mutable std::mutex vmLock;

	enum
	{
		RUNMODE_PAUSE,
		RUNMODE_FREE,
		RUNMODE_DEBUGGER,
		RUNMODE_ONE_INSTRUCTION, // Always with debugger.
		RUNMODE_EXIT,
	};

	TownsThread();

	void Start(FMTowns *townsPtr);

	/*! Returns the current run mode.
	    The thread that calls this function must take vmLock before calling.
	*/
	int GetRunMode(void) const;

	/*! Sets the current run mode.
	    The thread that calls this function must take vmLock before calling. 
	*/
	void SetRunMode(int runModevoid);
};


/* } */
#endif
