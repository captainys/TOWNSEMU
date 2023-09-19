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
#include <memory>

#include "towns.h"
#include "outside_world.h"
#include "render.h"

class TownsThread
{
private:
	FMTownsCommon *townsPtr;
	int runMode=RUNMODE_PAUSE;
	bool returnOnPause=false;

	// This will be used for virtually slwoing down CPU when VM is lagging.
	long long int timeDeficit=0;

	enum
	{
		RENDER_TIMING_OUTSIDE_VSYNC,
		RENDER_TIMING_FIRST1MS_OF_VERTICAL,
	};
	unsigned int renderTiming=RENDER_TIMING_OUTSIDE_VSYNC;

public:
	enum
	{
		NANOSECONDS_PER_TIME_SYNC=    1000000, // 1ms
		TIME_DEFICIT_PAYBACK_PER_INSTRUCTION= 1000, // 1us
	};

	enum
	{
		RUNMODE_POWER_OFF,
		RUNMODE_PAUSE,
		RUNMODE_RUN,
		RUNMODE_ONE_INSTRUCTION, // Always with debugger.
		RUNMODE_EXIT,
	};

	TownsThread();

	void VMStart(FMTownsCommon *townsPtr,Outside_World *outside_world,class TownsUIThread *uiThread);
	void VMMainLoop(FMTownsTemplate <i486DXDefaultFidelity> *townsPtr,Outside_World *outside_world,Outside_World::Sound *sound,Outside_World::WindowInterface *window,class TownsUIThread *uiThread);
	void VMMainLoop(FMTownsTemplate <i486DXHighFidelity> *townsPtr,Outside_World *outside_world,Outside_World::Sound *sound,Outside_World::WindowInterface *window,class TownsUIThread *uiThread);
	void VMEnd(FMTownsCommon *townsPtr,Outside_World *outside_world,class TownsUIThread *uiThread);
private:
	void CheckRenderingTimer(FMTownsCommon &towns,class Outside_World::WindowInterface &window,bool imageNeedsFlip);

	template <class FMTownsClass>
	void VMMainLoopTemplate(FMTownsClass *townsPtr,Outside_World *outside_world,Outside_World::Sound *sound,Outside_World::WindowInterface *window,class TownsUIThread *uiThread);
	void AdjustRealTime(FMTownsCommon *townsPtr,long long int cpuTimePassed,std::chrono::time_point<std::chrono::high_resolution_clock> time0,Outside_World *outside_world);

public:

	/*! Returns the current run mode.
	    The thread that calls this function must take vmLock before calling.
	*/
	int GetRunMode(void) const;

	/*! Sets the current run mode.
	    The thread that calls this function must take vmLock before calling. 
	*/
	void SetRunMode(int runModevoid);

	/*! If true, VMMainLoop will return when VM is paused.
	*/
	void SetReturnOnPause(bool flag);

	void PrintStatus(const FMTownsCommon &towns) const;
};

class TownsUIThread
{
public:
	mutable std::mutex uiLock;
	bool vmTerminated=false;

	void Run(TownsThread *vmThread,FMTownsCommon *towns,const class TownsARGV *argv,Outside_World *outside_world);
private:
	/*! Main function should only populate command queue, and not execute.
	    Main thread will lock uiLock and then call ExecCommandQueue, where the UI commands should be executed.
	*/
	virtual void Main(TownsThread &vmThread,FMTownsCommon &towns,const class TownsARGV &argv,Outside_World &outside_world)=0;

public:
	/*! ExecCommandQueue is called from the main thread.
	    uiLock is owned by the main thread when ExecCommandQueue is called.
	    It must also executes commands in the outside_world->commandQueue.
	*/
	virtual void ExecCommandQueue(TownsThread &vmThread,FMTownsCommon &towns,Outside_World *outside_world,Outside_World::Sound *sound)=0;
};

/* } */
#endif
