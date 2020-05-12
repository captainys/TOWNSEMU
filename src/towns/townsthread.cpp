/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <chrono>

#include "townsthread.h"
#include "render.h"


TownsThread::TownsThread(void)
{
	runMode=RUNMODE_PAUSE;
}

void TownsThread::Start(FMTowns *townsPtr,Outside_World *outside_world)
{
	bool terminate=false;
	this->townsPtr=townsPtr;
	townsPtr->cdrom.SetOutsideWorld(outside_world);
	townsPtr->sound.SetOutsideWorld(outside_world);

	outside_world->Start();

	auto lastWallClockTime=std::chrono::high_resolution_clock::now();

	TownsRender render;
	for(;true!=terminate;)
	{
		auto wallClockTime=std::chrono::high_resolution_clock::now();
		auto passed=std::chrono::duration_cast<std::chrono::nanoseconds>(wallClockTime-lastWallClockTime).count();
		lastWallClockTime=wallClockTime;

		int runModeCopy=0;

		vmLock.lock();
		runModeCopy=runMode;

		if(RUNMODE_PAUSE!=runMode)
		{
			townsPtr->state.wallClockTime+=passed;
		}

		switch(runMode)
		{
		case RUNMODE_PAUSE:
			townsPtr->ForceRender(render,*outside_world);
			outside_world->DevicePolling(*townsPtr);
			break;
		case RUNMODE_FREE:
			townsPtr->cpu.DetachDebugger();
			townsPtr->cpu.enableCallStack=false;
			for(unsigned int clocksPassed=0; 
			    clocksPassed<NUM_CLOCKS_PER_TIME_SYNC && true!=townsPtr->CheckAbort();
			    )
			{
				clocksPassed+=townsPtr->RunOneInstruction();
				townsPtr->pic.ProcessIRQ(townsPtr->cpu,townsPtr->mem);
				townsPtr->RunFastDevicePolling();
				townsPtr->RunScheduledTasks();
				townsPtr->CheckRenderingTimer(render,*outside_world);
			}
			AdjustRealTime(townsPtr,lastWallClockTime);

			outside_world->DevicePolling(*townsPtr);
			townsPtr->eventLog.Interval(*townsPtr);
			if(true==townsPtr->CheckAbort())
			{
				PrintStatus(*townsPtr);
				std::cout << ">";
				runMode=RUNMODE_PAUSE;
			}
			break;
		case RUNMODE_DEBUGGER:
			townsPtr->cpu.AttachDebugger(&townsPtr->debugger);
			townsPtr->cpu.enableCallStack=true;
			for(unsigned int clocksPassed=0; 
			    clocksPassed<NUM_CLOCKS_PER_TIME_SYNC && true!=townsPtr->CheckAbort();
			    )
			{
				clocksPassed+=townsPtr->RunOneInstruction();
				townsPtr->pic.ProcessIRQ(townsPtr->cpu,townsPtr->mem);
				townsPtr->RunFastDevicePolling();
				townsPtr->RunScheduledTasks();
				townsPtr->CheckRenderingTimer(render,*outside_world);
				if(true==townsPtr->debugger.stop)
				{
					PrintStatus(*townsPtr);
					std::cout << ">";
					runMode=RUNMODE_PAUSE;
					break;
				}
			}
			AdjustRealTime(townsPtr,lastWallClockTime);
			outside_world->DevicePolling(*townsPtr);
			townsPtr->eventLog.Interval(*townsPtr);
			if(true==townsPtr->CheckAbort())
			{
				PrintStatus(*townsPtr);
				std::cout << ">";
				runMode=RUNMODE_PAUSE;
			}
			break;
		case RUNMODE_ONE_INSTRUCTION:
			townsPtr->cpu.AttachDebugger(&townsPtr->debugger);
			townsPtr->cpu.enableCallStack=true;
			if(true!=townsPtr->CheckAbort())
			{
				townsPtr->RunOneInstruction();
				townsPtr->pic.ProcessIRQ(townsPtr->cpu,townsPtr->mem);
				townsPtr->RunFastDevicePolling();
				townsPtr->RunScheduledTasks();
			}
			PrintStatus(*townsPtr);
			std::cout << ">";
			runMode=RUNMODE_PAUSE;
			break;
		case RUNMODE_EXIT:
			terminate=true;
			break;
		default:
			terminate=true;
			std::cout << "Undefined VM RunMode!" << std::endl;
			break;
		}
		if(true==townsPtr->var.powerOff)
		{
			runMode=RUNMODE_EXIT;
		}
		if(townsPtr->state.nextSecondInTownsTime<=townsPtr->state.townsTime)
		{
			townsPtr->state.nextSecondInTownsTime+=PER_SECOND;
			townsPtr->fdc.SaveModifiedDiskImages();
		}
		vmLock.unlock();

		if(RUNMODE_PAUSE==runModeCopy)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		signalLock.lock();
		signalLock.unlock();
	}

	std::cout << "Ending Towns Thread." << std::endl;
	townsPtr->fdc.SaveModifiedDiskImages();

	outside_world->Stop();
}

void TownsThread::AdjustRealTime(FMTowns *townsPtr,std::chrono::time_point<std::chrono::high_resolution_clock> lastWallClockTime)
{
	townsPtr->var.timeAdjustLog[townsPtr->var.timeAdjustLogPtr]=townsPtr->state.townsTime-townsPtr->state.wallClockTime;
	townsPtr->var.timeAdjustLogPtr=(townsPtr->var.timeAdjustLogPtr+1)&(FMTowns::Variable::TIME_ADJUSTMENT_LOG_LEN-1);

	if(townsPtr->state.townsTime<townsPtr->state.wallClockTime) // VM lagging
	{
		// One option is to fast-forward townsTime to match wallClockTime by doing:
		// townsPtr->state.townsTime=townsPtr->state.wallClockTime;
		//
		// Then in the next iteration, scheduled tasks will fire all at once, which breaks some logic.
		// Rather, want to let VM catch up by virtually spending longer time.
		// The question is how?


		townsPtr->state.wallClockTime=townsPtr->state.townsTime;
	}
	else
	{
		if(true==townsPtr->state.noWait)
		{
			townsPtr->state.wallClockTime=townsPtr->state.townsTime;
		}
		else
		{
			auto toWait=townsPtr->state.townsTime-townsPtr->state.wallClockTime;
			for(;;)
			{
				auto diff=std::chrono::high_resolution_clock::now()-lastWallClockTime;
				if(toWait<=std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count())
				{
					break;
				}
				townsPtr->RunFastDevicePolling();
			}
		}
	}
}

int TownsThread::GetRunMode(void) const
{
	return runMode;
}
void TownsThread::SetRunMode(int nextRunMode)
{
	runMode=nextRunMode;
}

void TownsThread::PrintStatus(const FMTowns &towns) const
{
	towns.PrintStatus();
}


////////////////////////////////////////////////////////////


void TownsUIThread::Run(TownsThread *vmThread,FMTowns *towns,const TownsARGV *argv,Outside_World *outside_world)
{
	Main(*vmThread,*towns,*argv,*outside_world);
}
