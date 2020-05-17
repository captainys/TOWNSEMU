/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef TOWNSTHREAD_IS_INCLUDED
#define TOWNSTHREAD_IS_INCLUDED
/* { */


#include <chrono>
#include <thread>
#include <mutex>

#include "towns.h"
#include "outside_world.h"

class TownsThread
{
private:
	FMTowns *townsPtr;
	int runMode;

public:
	enum
	{
		NANOSECONDS_PER_TIME_SYNC=1000000,
	};

	enum
	{
		RUNMODE_PAUSE,
		RUNMODE_RUN,
		RUNMODE_ONE_INSTRUCTION, // Always with debugger.
		RUNMODE_EXIT,
	};

	TownsThread();

	void Start(FMTowns *townsPtr,Outside_World *outside_world,class TownsUIThread *uiThread);
private:
	void AdjustRealTime(FMTowns *townsPtr);

public:

	/*! Returns the current run mode.
	    The thread that calls this function must take vmLock before calling.
	*/
	int GetRunMode(void) const;

	/*! Sets the current run mode.
	    The thread that calls this function must take vmLock before calling. 
	*/
	void SetRunMode(int runModevoid);

	void PrintStatus(const FMTowns &towns) const;
};

class TownsUIThread
{
public:
	mutable std::mutex uiLock;

	void Run(TownsThread *vmThread,FMTowns *towns,const class TownsARGV *argv,Outside_World *outside_world);
private:
	/*! Main function should only populate command queue, and not execute.
	    Main thread will lock uiLock and then call ExecCommandQueue, where the UI commands should be executed.
	*/
	virtual void Main(TownsThread &vmThread,FMTowns &towns,const class TownsARGV &argv,Outside_World &outside_world)=0;

public:
	/*! ExecCommandQueue is called from the main thread.
	    uiLock is owned by the main thread when ExecCommandQueue is called.
	*/
	virtual void ExecCommandQueue(TownsThread &vmThread,FMTowns &towns,Outside_World *outside_world)=0;
};

/* } */
#endif
