/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <string>
#include <chrono>

#include "towns.h"
#include "townsthread.h"
#include "townscommand.h"
#include "townsargv.h"
#include "i486symtable.h"
#include "cpputil.h"
#include "discimg.h"

#include "fssimplewindow_connection.h"


#ifdef _WIN32
#include <timeapi.h>
#else
static void timeBeginPeriod(int)
{
}
static void timeEndPeriod(int)
{
}
#endif


class TownsCUIThread : public TownsUIThread
{
public:
	using TownsUIThread::uiLock;
	std::string cmdline;
	TownsCommandInterpreter cmdInterpreter;
	bool uiTerminate=false;

	virtual void Main(TownsThread &vmThread,FMTownsCommon &towns,const TownsARGV &argv,Outside_World &outside_world) override;
	virtual void ExecCommandQueue(TownsThread &vmThread,FMTownsCommon &towns,Outside_World *outside_world,Outside_World::Sound *sound) override;
};

/* virtual */ void TownsCUIThread::Main(TownsThread &townsThread,FMTownsCommon &towns,const TownsARGV &argv,Outside_World &outside_world)
{
	for(auto &ftfr : argv.toSend)
	{
		towns.var.ftfr.AddHostToVM(ftfr.hostFName,ftfr.vmFName);
	}

	if(true==argv.interactive)
	{
		while(true!=uiTerminate)
		{
			std::string cmdline;
			std::cout << ">";
			std::getline(std::cin,cmdline);

			uiLock.lock();
			this->cmdline=cmdline;
			if(true==this->vmTerminated)
			{
				uiTerminate=true;
			}
			uiLock.unlock();

			bool commandDone=false;
			while(true!=commandDone && true!=uiTerminate)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				uiLock.lock();
				if(""==this->cmdline)
				{
					commandDone=true;
				}
				if(true==this->vmTerminated)
				{
					uiTerminate=true;
				}
				uiLock.unlock();
			}
		}
	}
}

/* virtual */ void TownsCUIThread::ExecCommandQueue(TownsThread &townsThread,FMTownsCommon &towns,Outside_World *outside_world,Outside_World::Sound *sound)
{
	if(true==cmdInterpreter.waitVM)
	{
		unsigned int vmState;
		vmState=townsThread.GetRunMode();
		if(TownsThread::RUNMODE_PAUSE==vmState)
		{
			cmdInterpreter.waitVM=false;
		}
		else if(TownsThread::RUNMODE_EXIT==vmState)
		{
			uiTerminate=true;
		}
	}
	else
	{
		if(""!=this->cmdline)
		{
			auto cmd=cmdInterpreter.Interpret(this->cmdline);
			cmdInterpreter.Execute(townsThread,towns,outside_world,sound,cmd);
			if(TownsCommandInterpreter::CMD_QUIT==cmd.primaryCmd)
			{
				uiTerminate=true;
			}
			this->cmdline="";
		}
		while(true!=outside_world->commandQueue.empty())
		{
			auto cmd=cmdInterpreter.Interpret(outside_world->commandQueue.front());
			cmdInterpreter.Execute(townsThread,towns,outside_world,sound,cmd);
			if(TownsCommandInterpreter::CMD_QUIT==cmd.primaryCmd)
			{
				uiTerminate=true;
			}
			outside_world->commandQueue.pop();
		}
	}
}



template <class CPUCLASS>
int Run(FMTownsTemplate <CPUCLASS> &towns,const TownsARGV &argv,Outside_World &outside_world,Outside_World::Sound &sound,Outside_World::WindowInterface &window)
{
	TownsThread townsThread;

	if(true==argv.debugger)
	{
		towns.EnableDebugger();
	}
	else
	{
		towns.DisableDebugger();
	}

	if(true==argv.autoStart)
	{
		townsThread.SetRunMode(TownsThread::RUNMODE_RUN);
	}

	TownsCUIThread cuiThread;

	std::thread UIThread(&TownsCUIThread::Run,&cuiThread,&townsThread,&towns,&argv,&outside_world);

	std::thread VMThread([&]{
		townsThread.VMStart(&towns,&outside_world,&cuiThread);
		townsThread.VMMainLoop(&towns,&outside_world,&sound,&window,&cuiThread);
		townsThread.VMEnd(&towns,&outside_world,&cuiThread);
	});

	auto t0=std::chrono::high_resolution_clock::now();
	window.ClearVMClosedFlag();
	while(true!=window.CheckVMClosed())
	{
		window.Interval();
		auto t=std::chrono::high_resolution_clock::now();
		auto dt=t-t0;
		if(16<=std::chrono::duration_cast<std::chrono::milliseconds>(dt).count())
		{
			window.Render(true);
			t0=t;
		}
		else
		{
			timeBeginPeriod(1);
			std::this_thread::sleep_for(std::chrono::milliseconds(8));
			timeEndPeriod(1);
		}
	}

	VMThread.join();
	UIThread.join();

	return towns.var.returnCode;
}

int main(int ac,char *av[])
{
	if(sizeof(void *)<8)
	{
		std::cout << "This requires minimum 64-bit CPU.\n";
		return 1;
	}
	if(sizeof(long long int)<8)
	{
		std::cout << "long long it needs to be minimum 64-bit.\n";
		return 1;
	}


	TownsARGV argv;
	if(true!=argv.AnalyzeCommandParameter(ac,av))
	{
		return 1;
	}


	auto *outside_world=new FsSimpleWindowConnection;
	auto *sound=outside_world->CreateSound();
	auto *window=outside_world->CreateWindowInterface();
	if(i486DXCommon::HIGH_FIDELITY==argv.CPUFidelityLevel)
	{
		static FMTownsTemplate <i486DXHighFidelity> towns;
		if(true!=FMTownsCommon::Setup(towns,outside_world,window,argv))
		{
			return 1;
		}
		window->Start();
		return Run(towns,argv,*outside_world,*sound,*window);
	}
	else
	{
		static FMTownsTemplate <i486DXDefaultFidelity> towns;
		if(true!=FMTownsCommon::Setup(towns,outside_world,window,argv))
		{
			return 1;
		}
		window->Start();
		return Run(towns,argv,*outside_world,*sound,*window);
	}
}
