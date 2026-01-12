#ifndef RUNVM_IS_INCLUDED
#define RUNVM_IS_INCLUDED
/* { */



#ifdef OUT
#undef OUT
#endif

#include <queue>

#include "towns.h"
#include "townsthread.h"
#include "outside_world.h"
#include "townscommand.h"
#include "townsprofile.h"
#include "fssimplewindow_connection.h"

class TownsCommandQueue : public TownsUIThread
{
public:
	using TownsUIThread::uiLock;
	std::queue <std::string> cmdqueue;
	TownsCommandInterpreter cmdInterpreter;

	virtual void Main(TownsThread &vmThread,FMTownsCommon &towns,const TownsARGV &argv,Outside_World &outside_world) override;
	virtual void ExecCommandQueue(TownsThread &vmThread,FMTownsCommon &towns,Outside_World *outside_world,Outside_World::Sound *sound) override;

	void SendCommand(std::string cmd);
};

template <class CPUCLASS>
class TownsVM
{
public:
	TownsProfile profile;
	TownsRender lastImage;
	FMTownsTemplate <CPUCLASS> *townsPtr=nullptr;
	TownsThread *townsThreadPtr=nullptr;
	TownsCommandQueue *cmdQueuePtr=nullptr;
	FsSimpleWindowConnection *outsideWorldPtr=nullptr;
	FsSimpleWindowConnection::WindowInterface *outsideWorldWindowPtr=nullptr;

	TownsVM();
	~TownsVM();

	void Alloc(void);
	void Free(void);

	void Run(void);
	bool IsRunning(void) const;
	void SendCommand(std::string cmd);
};

template <class CPUCLASS>
TownsVM<CPUCLASS>::TownsVM()
{
}
template <class CPUCLASS>
TownsVM<CPUCLASS>::~TownsVM()
{
	Free();
}

template <class CPUCLASS>
void TownsVM<CPUCLASS>::Alloc(void)
{
	townsPtr=new FMTownsTemplate <CPUCLASS>;
	townsThreadPtr=new TownsThread;
	cmdQueuePtr=new TownsCommandQueue;
	outsideWorldPtr=new FsSimpleWindowConnection;
	townsThreadPtr->SetRunMode(TownsThread::RUNMODE_POWER_OFF);
	townsThreadPtr->SetReturnOnPause(true);
	outsideWorldWindowPtr=outsideWorldPtr->CreateWindowInterface();
}
template <class CPUCLASS>
void TownsVM<CPUCLASS>::Free(void)
{
	if(nullptr!=outsideWorldPtr)
	{
		outsideWorldPtr->DeleteWindowInterface(outsideWorldWindowPtr);
	}
	outsideWorldWindowPtr=nullptr;
	delete townsThreadPtr;
	delete townsPtr;
	delete cmdQueuePtr;
	delete outsideWorldPtr;
	townsPtr=nullptr;
	townsThreadPtr=nullptr;
	cmdQueuePtr=nullptr;
	outsideWorldPtr=nullptr;
}

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

template <class CPUCLASS>
void TownsVM<CPUCLASS>::Run(void)
{
	bool freshStart=false,VMPowerOff=false;

	if(nullptr==townsPtr ||
	   TownsThread::RUNMODE_POWER_OFF==townsThreadPtr->GetRunMode() ||
	   TownsThread::RUNMODE_EXIT==townsThreadPtr->GetRunMode())
	{
		Free();
		Alloc();

		townsPtr->Setup(*townsPtr,outsideWorldPtr,outsideWorldWindowPtr,profile);
		outsideWorldWindowPtr->Start();
		freshStart=true;
	}

	outsideWorldPtr->lowerRightIcon=Outside_World::LOWER_RIGHT_MENU;

	std::thread VMThread([&]
	{
		townsThreadPtr->SetRunMode(TownsThread::RUNMODE_RUN);
		auto outsideWorldSoundPtr=outsideWorldPtr->CreateSound();
		if(true==freshStart)
		{
			townsThreadPtr->VMStart(townsPtr,outsideWorldPtr,cmdQueuePtr);
		}
		townsThreadPtr->VMMainLoop(townsPtr,outsideWorldPtr,outsideWorldSoundPtr,outsideWorldWindowPtr,cmdQueuePtr);
		if(TownsThread::RUNMODE_EXIT==townsThreadPtr->GetRunMode())
		{
			townsThreadPtr->VMEnd(townsPtr,outsideWorldPtr,cmdQueuePtr);
			VMPowerOff=true;
		}
		outsideWorldPtr->DeleteSound(outsideWorldSoundPtr);
	});

	auto t0=std::chrono::high_resolution_clock::now();
	outsideWorldWindowPtr->ClearVMClosedFlag();
	while(true!=outsideWorldWindowPtr->CheckVMClosed())
	{
		outsideWorldWindowPtr->Interval();
		auto t=std::chrono::high_resolution_clock::now();
		auto dt=t-t0;
		if(50<=std::chrono::duration_cast<std::chrono::milliseconds>(dt).count() || true==outsideWorldWindowPtr->winThr.newImageRendered)
		{
			outsideWorldWindowPtr->Render(true);
			t0=t;
			outsideWorldWindowPtr->winThr.newImageRendered=false;
		}
		else
		{
			timeBeginPeriod(1);
			std::this_thread::sleep_for(std::chrono::milliseconds(8));
			timeEndPeriod(1);
		}
	}

	VMThread.join();

	outsideWorldPtr->lowerRightIcon=Outside_World::LOWER_RIGHT_NONE;

	if(true==VMPowerOff)
	{
		outsideWorldWindowPtr->Stop();
		Free();
	}
	else
	{
		townsPtr->ForceRender(lastImage,*outsideWorldPtr,*outsideWorldWindowPtr);
	}
}
template <class CPUCLASS>
bool TownsVM<CPUCLASS>::IsRunning(void) const
{
	return
		(nullptr!=townsThreadPtr &&
		 TownsThread::RUNMODE_POWER_OFF!=townsThreadPtr->GetRunMode() &&
		 TownsThread::RUNMODE_EXIT!=townsThreadPtr->GetRunMode());
}
template <class CPUCLASS>
void TownsVM<CPUCLASS>::SendCommand(std::string cmd)
{
	if(nullptr!=cmdQueuePtr)
	{
		cmdQueuePtr->SendCommand(cmd);
	}
}

/* } */
#endif
