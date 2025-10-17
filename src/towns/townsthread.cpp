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
#include <memory>

#include "townsthread.h"


TownsThread::TownsThread(void)
{
	runMode=RUNMODE_PAUSE;
}

void TownsThread::VMStart(FMTownsCommon *townsPtr,Outside_World *outside_world,class TownsUIThread *uiThread)
{
	this->townsPtr=townsPtr;

	outside_world->Start();

	if(""!=townsPtr->var.startUpStateFName)
	{
		townsPtr->LoadState(townsPtr->var.startUpStateFName);
	}

	switch(townsPtr->state.appSpecificSetting)
	{
	case TOWNS_APPSPECIFIC_BRANDISH:
		// Brandish doesn't know about 80486.  It confuses as 80386SX and messes up.
		townsPtr->state.pretend386DX=true;
		break;
	case TOWNS_APPSPECIFIC_LEMMINGS2:
		renderTiming=RENDER_TIMING_AVOID_FIRST1MS_OF_VERTICAL;
		break;
	}
}

void TownsThread::VMMainLoop(
    FMTownsTemplate <i486DXDefaultFidelity>  *townsPtr,
    Outside_World *outside_world,
    Outside_World::Sound *sound,
    Outside_World::WindowInterface *window,
    class TownsUIThread *uiThread)
{
	VMMainLoopTemplate(townsPtr,outside_world,sound,window,uiThread);
}
void TownsThread::VMMainLoop(
    FMTownsTemplate <i486DXHighFidelity>  *townsPtr,
    Outside_World *outside_world,
    Outside_World::Sound *sound,
    Outside_World::WindowInterface *window,
    class TownsUIThread *uiThread)
{
	VMMainLoopTemplate(townsPtr,outside_world,sound,window,uiThread);
}

template <class FMTownsClass>
void TownsThread::VMMainLoopTemplate(
    FMTownsClass *townsPtr,
    Outside_World *outside_world,
    Outside_World::Sound *sound,
    Outside_World::WindowInterface *window,
    class TownsUIThread *uiThread)
{
	townsPtr->sound.SetOutsideWorld(sound);
	townsPtr->sound.SetCDROMPointer(&townsPtr->cdrom);
	townsPtr->scsi.SetOutsideWorld(sound);
	sound->Start();

	TownsRender render;
	bool terminate=false;
	for(;true!=terminate;)
	{
		auto realTime0=std::chrono::high_resolution_clock::now();
		auto townsTime0=townsPtr->state.townsTime;

		int runModeCopy=0;

		runModeCopy=runMode;

		bool clockTicking=false;  // Will be made true if VM is running.

		townsPtr->var.justLoadedState=false;

		switch(runMode)
		{
		case RUNMODE_PAUSE:
			outside_world->UpdateStatusBarInfo(*townsPtr);
			// Do not call window->Communicate(outwide_world); here because townsPtr->ForceRender will call it inside.
			townsPtr->ForceRender(render,*outside_world,*window);
			outside_world->DevicePolling(*townsPtr);
			if(true==outside_world->PauseKeyPressed())
			{
				runMode=RUNMODE_RUN;
				townsPtr->debugger.stop=false;
			}
			townsPtr->sound.ProcessSilence();
			break;
		case RUNMODE_RUN:
			clockTicking=true;
			{
				townsPtr->var.nextTimeSync=townsPtr->state.townsTime+NANOSECONDS_PER_TIME_SYNC;
				townsPtr->debugger.ClearStopFlag();
				if(true==townsPtr->CheckAbort())
				{
					townsPtr->var.nextTimeSync=0;
				}
				while(townsPtr->state.townsTime<townsPtr->var.nextTimeSync)
				{
					// With the inner-loop, it saves one 64-bit comparison + conditional jump per instruction for RunFastDevicePolling.
					uint32_t payBack=0;
					while(townsPtr->state.townsTime<=townsPtr->state.nextFastDevicePollingTime &&
					      true!=townsPtr->debugger.stop) // Same check, except one timer check
					{
						townsPtr->RunOneInstruction();
						townsPtr->pic.ProcessIRQ(townsPtr->CPU(),townsPtr->mem);
						payBack+=TIME_DEFICIT_PAYBACK_PER_INSTRUCTION;
					}

					payBack=std::min<uint32_t>(payBack,timeDeficit);
					townsPtr->state.townsTime+=payBack;
					timeDeficit-=payBack;

					townsPtr->RunScheduledTasks();
					townsPtr->RunFastDevicePolling();

					if(true==townsPtr->debugger.stop)
					{
						if(true==townsPtr->debugger.lastBreakPointInfo.ShouldBreak() &&
						   townsPtr->CPU().state.CS().value==townsPtr->var.powerOffAt.SEG &&
						   townsPtr->CPU().state.EIP==townsPtr->var.powerOffAt.OFFSET)
						{
							std::cout << "Break at the power-off point." << std::endl;
							std::cout << "Normal termination of a unit testing." << std::endl;
							townsPtr->var.powerOff=true;
							break;
						}

						if(""!=townsPtr->debugger.lastBreakPointInfo.saveState)
						{
							if(true!=townsPtr->debugger.lastBreakPointInfo.saveStateMem)
							{
								if(true!=townsPtr->SaveState(townsPtr->debugger.lastBreakPointInfo.saveState))
								{
									std::cout << "Error Saving " << townsPtr->debugger.lastBreakPointInfo.saveState << std::endl;
								}
								else
								{
									std::cout << "Saved " << townsPtr->debugger.lastBreakPointInfo.saveState << std::endl;
								}
							}
							else
							{
								unsigned int slot=0;
								slot=cpputil::Atoi(townsPtr->debugger.lastBreakPointInfo.saveState.data());
								FMTownsCommon::Variable::MemoryStateSave state;
								state.data=townsPtr->SaveStateMem();
								state.CSEIP.SEG=townsPtr->CPU().state.CS().value;
								state.CSEIP.OFFSET=townsPtr->CPU().state.EIP;
								state.townsTime=townsPtr->state.townsTime;
								townsPtr->var.memoryStateSave[slot]=state;
								std::cout << "Saved state to memory slot:" << slot << std::endl;
							}
						}

						if(true!=townsPtr->debugger.lastBreakPointInfo.ShouldBreak())
						{
							if(0!=(townsPtr->debugger.lastBreakPointInfo.flags&i486Debugger::BRKPNT_FLAG_MONITOR_ONLY) ||
							   0==(townsPtr->debugger.lastBreakPointInfo.flags&i486Debugger::BRKPNT_FLAG_SILENT_UNTIL_BREAK))
							{
								if(0==(i486Debugger::BRKPNT_FLAG_SHORT_FORMAT&townsPtr->debugger.lastBreakPointInfo.flags))
								{
									std::cout << "Passed " << townsPtr->debugger.lastBreakPointInfo.passedCount << " times." << std::endl;
									PrintStatus(*townsPtr);
								}
								else
								{
									townsPtr->PrintDisassembly();
								}
							}
							townsPtr->debugger.ClearStopFlag();
							this->SetRunMode(RUNMODE_RUN);
						}
						else
						{
							std::cout << "Passed " << townsPtr->debugger.lastBreakPointInfo.passedCount << " times." << std::endl;
							PrintStatus(*townsPtr);
							std::cout << ">";
							runMode=RUNMODE_PAUSE;
						}
						break;
					}

					CheckRenderingTimer(*townsPtr,*window,outside_world->ImageNeedsFlip());
				}
			}
			townsPtr->ProcessSound(outside_world);
			townsPtr->cdrom.UpdateCDDAState(townsPtr->state.townsTime);

			outside_world->ProcessAppSpecific(*townsPtr);
			if(townsPtr->state.nextDevicePollingTime<townsPtr->state.townsTime)
			{
				outside_world->UpdateStatusBarInfo(*townsPtr);
				window->Communicate(outside_world);
				outside_world->DevicePolling(*townsPtr);
				sound->Polling();
				townsPtr->state.nextDevicePollingTime=townsPtr->state.townsTime+FMTownsCommon::DEVICE_POLLING_INTERVAL;
				if(true==townsPtr->debugger.stop)
				{
					// ExternalBreak may be sent from a device.  Stop flag is cleared at the beginning of RUNMODE_RUN.
					// Must change to RUNMODE_PAUSE here.
					PrintStatus(*townsPtr);
					std::cout << ">";
					runMode=RUNMODE_PAUSE;
				}
			}
			townsPtr->eventLog.Interval(*townsPtr);
			if(true==townsPtr->CheckAbort() || outside_world->PauseKeyPressed())
			{
				PrintStatus(*townsPtr);
				std::cout << ">";
				runMode=RUNMODE_PAUSE;
			}
			break;
		case RUNMODE_ONE_INSTRUCTION:
			if(true!=townsPtr->CheckAbort())
			{
				townsPtr->debugger.ClearStopFlag();
				townsPtr->RunOneInstruction();
				townsPtr->pic.ProcessIRQ(townsPtr->CPU(),townsPtr->mem);
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
		if(true==outside_world->closeWindow && RUNMODE_EXIT!=runMode)
		{
			std::cout << "Window closed.\n";
			runMode=RUNMODE_EXIT;
		}
		if(true==townsPtr->var.powerOff)
		{
			if(true!=townsPtr->var.pauseOnPowerOff)
			{
				runMode=RUNMODE_EXIT;
			}
			else if(RUNMODE_PAUSE!=runMode)
			{
				PrintStatus(*townsPtr);
				std::cout << ">";
				runMode=RUNMODE_PAUSE;
			}
		}
		if(townsPtr->state.nextSecondInTownsTime<=townsPtr->state.townsTime)
		{
			townsPtr->state.nextSecondInTownsTime+=PER_SECOND;
			townsPtr->fdc.SaveModifiedDiskImages();
			townsPtr->physMem.state.memCard.SaveRawImageIfModified();
		}
		if(true==townsPtr->autoQSS)
		{
			int x,y;
			if(true==townsPtr->CheckAutoQSS(x,y))
			{
				outside_world->commandQueue.push("QSS"); // I should have made FMTowns class own the external command queue :-P
			}
		}

		if(RUNMODE_PAUSE==runModeCopy)
		{
			townsPtr->fdc.SaveModifiedDiskImages();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if(true==returnOnPause)
			{
				break;
			}
		}

		uiThread->uiLock.lock();
		uiThread->ExecCommandQueue(*this,*townsPtr,outside_world,sound);
		uiThread->uiLock.unlock();
		if(true==townsPtr->var.justLoadedState)
		{
		}
		else if(true==clockTicking)
		{
			AdjustRealTime(townsPtr,townsPtr->state.townsTime-townsTime0,realTime0,outside_world);
		}
	}

	sound->Stop();
	window->NotifyVMClosed();
}
void TownsThread::VMEnd(FMTownsCommon *townsPtr,Outside_World *outside_world,class TownsUIThread *uiThread)
{
	townsPtr->cdrom.WaitUntilAsyncWaveReaderFinished();

	uiThread->uiLock.lock();
	uiThread->vmTerminated=true;
	uiThread->uiLock.unlock();

	std::cout << "Ending Towns Thread." << std::endl;
	townsPtr->fdc.SaveModifiedDiskImages();

	if(0<townsPtr->var.CMOSFName.size())
	{
		auto expanded=townsPtr->var.ExpandFileName(townsPtr->var.CMOSFName);
		if(true!=cpputil::WriteBinaryFile(expanded,TOWNS_CMOS_SIZE,townsPtr->physMem.state.CMOSRAM))
		{
			std::cout << "Failed to save CMOS." << std::endl;
		}
	}

	outside_world->Stop();

	if(true==townsPtr->var.forceQuitOnPowerOff)
	{
		exit(0);
	}
}

void TownsThread::CheckRenderingTimer(FMTownsCommon &towns,class Outside_World::WindowInterface &window,bool imageNeedsFlip)
{
	if(towns.state.nextRenderingTime<=towns.state.townsTime)
	{
		bool isTiming=false;
		switch(renderTiming)
		{
		case RENDER_TIMING_OUTSIDE_VSYNC:
			isTiming=(true!=towns.crtc.InVSYNC(towns.state.townsTime));
			break;
		case RENDER_TIMING_AVOID_FIRST1MS_OF_VERTICAL:
			isTiming=towns.crtc.AvoidFirst1msOfVerticalPeriod(towns.state.townsTime);
			break;
		default:
			std_unreachable;
		}
		if(true==isTiming)
		{
			if(true==window.SendNewImage(towns,imageNeedsFlip))
			{
				towns.state.nextRenderingTime=towns.state.townsTime+TOWNS_RENDERING_FREQUENCY;
			}
		}
	}
}


// Case 1
// Time deficit occurs when the VM lags behind the real-time (realTimePassed>cpuTimePassed):
// 
// RealTime  |------------>|1500us
// VM        |----->|1000us
//                  |      |deficit 500us
// Hopefully it is due to screen rendering, disk access, or something temporary.

// Case 2
// When the VM catches up (deficit+realTimePassed<cpuTimePassed):
// 
// RealTime  |    |deficit-->|RalTimePassed-->|Wait------>|
// VM        |    |----------------------------------->|
//                                                     |->| New deficit

// Case 3 : General case of Case 1
// When the VM doesn't catch up, but pay back some deficit (cpuTimePassed>realTimePassed):
// RealTime  |    |deficit-->|RealTimePassed-->|
// VM        |    |------------------->|       |
//                                     |------>|  New deficit


void TownsThread::AdjustRealTime(FMTownsCommon *townsPtr,long long int cpuTimePassed,std::chrono::time_point<std::chrono::high_resolution_clock> time0,Outside_World *outside_world)
{
	long long int realTimePassed=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-time0).count();

	townsPtr->var.timeAdjustLog[townsPtr->var.timeAdjustLogPtr]=cpuTimePassed-realTimePassed;
	townsPtr->var.timeDeficitLog[townsPtr->var.timeAdjustLogPtr]=townsPtr->state.timeDeficit;
	townsPtr->var.timeAdjustLogPtr=(townsPtr->var.timeAdjustLogPtr+1)&(FMTownsCommon::Variable::TIME_ADJUSTMENT_LOG_LEN-1);

	int64_t balance=cpuTimePassed-(townsPtr->state.timeDeficit+realTimePassed);
	if(balance<0)  // Case 3
	{
		if(true==townsPtr->var.catchUpRealTime)
		{
			townsPtr->state.timeDeficit=(-balance);
		}
		else
		{
			townsPtr->state.timeDeficit=0;
		}
	}
	else // Case 2
	{
		if(true!=townsPtr->state.noWait)
		{
			while(townsPtr->state.timeDeficit+realTimePassed<cpuTimePassed)
			{
				townsPtr->ProcessSound(outside_world);
				realTimePassed=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-time0).count();
			}
			int64_t newBalance=cpuTimePassed-(townsPtr->state.timeDeficit+realTimePassed);
			townsPtr->state.timeDeficit=-newBalance;
		}
		else
		{
			townsPtr->state.timeDeficit=0;
		}
	}
	this->timeDeficit=townsPtr->state.timeDeficit;

	if(FMTownsCommon::State::CATCHUP_DEFICIT_CUTOFF<townsPtr->state.timeDeficit)
	{
		townsPtr->state.timeDeficit=FMTownsCommon::State::CATCHUP_DEFICIT_CUTOFF;
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

void TownsThread::SetReturnOnPause(bool flag)
{
	returnOnPause=flag;
}

void TownsThread::PrintStatus(const FMTownsCommon &towns) const
{
	towns.PrintStatus();
}


////////////////////////////////////////////////////////////


void TownsUIThread::Run(TownsThread *vmThread,FMTownsCommon *towns,const TownsARGV *argv,Outside_World *outside_world)
{
	Main(*vmThread,*towns,*argv,*outside_world);
}
