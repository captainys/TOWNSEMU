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

void TownsThread::Start(FMTowns *townsPtr,Outside_World *outside_world,class TownsUIThread *uiThread)
{
	bool terminate=false;
	this->townsPtr=townsPtr;
	townsPtr->cdrom.SetOutsideWorld(outside_world);
	townsPtr->sound.SetOutsideWorld(outside_world);

	outside_world->Start();

	TownsRender render;
	for(;true!=terminate;)
	{
		auto realTime0=std::chrono::high_resolution_clock::now();
		auto townsTime0=townsPtr->state.townsTime;

		int runModeCopy=0;

		runModeCopy=runMode;

		bool clockTicking=false;  // Will be made true if VM is running.

		switch(runMode)
		{
		case RUNMODE_PAUSE:
			townsPtr->ForceRender(render,*outside_world);
			outside_world->DevicePolling(*townsPtr);
			break;
		case RUNMODE_RUN:
			clockTicking=true;
			{
				auto nextTimeSync=townsPtr->state.townsTime+NANOSECONDS_PER_TIME_SYNC;
				while(townsPtr->state.townsTime<nextTimeSync && true!=townsPtr->CheckAbort())
				{
					townsPtr->RunOneInstruction();
					townsPtr->pic.ProcessIRQ(townsPtr->cpu,townsPtr->mem);
					townsPtr->RunFastDevicePolling();
					townsPtr->RunScheduledTasks();
					if(true==townsPtr->debugger.stop)
					{
						PrintStatus(*townsPtr);
						std::cout << ">";
						runMode=RUNMODE_PAUSE;
						break;
					}
				}
			}
			townsPtr->ProcessSound(outside_world);
			townsPtr->CheckRenderingTimer(render,*outside_world);
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

		if(RUNMODE_PAUSE==runModeCopy)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		uiThread->uiLock.lock();
		uiThread->ExecCommandQueue(*this,*townsPtr,outside_world);
		uiThread->uiLock.unlock();

		if(true==clockTicking)
		{
			AdjustRealTime(townsPtr,townsPtr->state.townsTime-townsTime0,realTime0,outside_world);
		}
	}

	std::cout << "Ending Towns Thread." << std::endl;
	townsPtr->fdc.SaveModifiedDiskImages();

	if(0<townsPtr->var.CMOSFName.size())
	{
		if(true!=cpputil::WriteBinaryFile(townsPtr->var.CMOSFName,TOWNS_CMOS_SIZE,townsPtr->physMem.state.CMOSRAM))
		{
			std::cout << "Failed to save CMOS." << std::endl;
		}
	}

	outside_world->Stop();
}

void TownsThread::AdjustRealTime(FMTowns *townsPtr,long long int townsTimePassed,std::chrono::time_point<std::chrono::high_resolution_clock> time0,Outside_World *outside_world)
{
	long long int realTimePassed=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-time0).count();

	townsPtr->var.timeAdjustLog[townsPtr->var.timeAdjustLogPtr]=townsTimePassed-realTimePassed;
	townsPtr->var.timeAdjustLogPtr=(townsPtr->var.timeAdjustLogPtr+1)&(FMTowns::Variable::TIME_ADJUSTMENT_LOG_LEN-1);

	if(townsTimePassed<realTimePassed) // VM lagging
	{
		// One option is to fast-forward townsTime to match wallClockTime by doing:
		// townsPtr->state.townsTime=townsPtr->state.wallClockTime;
		//
		// Then in the next iteration, scheduled tasks will fire all at once, which breaks some logic.
		// Rather, want to let VM catch up by virtually spending longer time.
		// The question is how?
	}
	else
	{
		if(true!=townsPtr->state.noWait)
		{
			while(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-time0).count()<townsTimePassed)
			{
				townsPtr->ProcessSound(outside_world);
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
