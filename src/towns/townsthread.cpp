#include <iostream>
#include <chrono>

#include "townsthread.h"
#include "render.h"


TownsThread::TownsThread(void)
{
	runMode=RUNMODE_PAUSE;
	unitTestDone=false;
	returnCode=0;
}

void TownsThread::Start(FMTowns *townsPtr,Outside_World *outside_world)
{
	bool terminate=false;
	this->townsPtr=townsPtr;

	outside_world->OpenWindow();

	TownsRender render;
	for(;true!=terminate;)
	{
		int runModeCopy=0;

		vmLock.lock();
		runModeCopy=runMode;
		switch(runMode)
		{
		case RUNMODE_PAUSE:
			townsPtr->ForceRender(render,*outside_world);
			outside_world->DevicePolling(*townsPtr);
			break;
		case RUNMODE_FREE:
			townsPtr->cpu.DetachDebugger();
			for(unsigned int clocksPassed=0; 
			    clocksPassed<10000 && true!=townsPtr->CheckAbort();
			    )
			{
				clocksPassed+=townsPtr->RunOneInstruction();
				townsPtr->pic.ProcessIRQ(townsPtr->cpu,townsPtr->mem);
				townsPtr->RunFastDevicePolling();
				townsPtr->RunScheduledTasks();
				townsPtr->CheckRenderingTimer(render,*outside_world);
			}
			outside_world->DevicePolling(*townsPtr);
			if(true==townsPtr->CheckAbort())
			{
				PrintStatus(*townsPtr);
				std::cout << ">";
				runMode=RUNMODE_PAUSE;
			}
			break;
		case RUNMODE_DEBUGGER:
			townsPtr->cpu.AttachDebugger(&townsPtr->debugger);
			for(unsigned int clocksPassed=0; 
			    clocksPassed<1000 && true!=townsPtr->CheckAbort();
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
			outside_world->DevicePolling(*townsPtr);
			if(true==townsPtr->CheckAbort())
			{
				PrintStatus(*townsPtr);
				std::cout << ">";
				runMode=RUNMODE_PAUSE;
			}
			break;
		case RUNMODE_ONE_INSTRUCTION:
			townsPtr->cpu.AttachDebugger(&townsPtr->debugger);
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
		if(true==UnitTestDone(*townsPtr))
		{
			runMode=RUNMODE_EXIT;
		}
		vmLock.unlock();

		if(RUNMODE_PAUSE==runModeCopy)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		signalLock.lock();
		signalLock.unlock();
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

bool TownsThread::UnitTestDone(const FMTowns &towns)
{
	if(towns.cpu.GetEDX()==0x47555354 && towns.cpu.GetEAX()==0x21555241)
	{
		unitTestDone=true;
		returnCode=0;
		return true;
	}
	else if(towns.cpu.GetEDX()==0x4C494146 && towns.cpu.GetEAX()==0x4C494146)
	{
		unitTestDone=true;
		returnCode=1;
		return true;
	}
}
