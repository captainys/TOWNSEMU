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
	FsSimpleWindowConnection::Sound *outsideWorldSoundPtr=nullptr;

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
	outsideWorldPtr->lowerRightIcon=Outside_World::LOWER_RIGHT_MENU;
	townsThreadPtr->SetRunMode(TownsThread::RUNMODE_POWER_OFF);
	townsThreadPtr->SetReturnOnPause(true);
	outsideWorldSoundPtr=outsideWorldPtr->CreateSound();
}
template <class CPUCLASS>
void TownsVM<CPUCLASS>::Free(void)
{
	if(nullptr!=outsideWorldPtr)
	{
		outsideWorldPtr->DeleteSound(outsideWorldSoundPtr);
	}
	outsideWorldSoundPtr=nullptr;
	delete townsThreadPtr;
	delete townsPtr;
	delete cmdQueuePtr;
	delete outsideWorldPtr;
	townsPtr=nullptr;
	townsThreadPtr=nullptr;
	cmdQueuePtr=nullptr;
	outsideWorldPtr=nullptr;
}

template <class CPUCLASS>
void TownsVM<CPUCLASS>::Run(void)
{
	if(nullptr==townsPtr ||
	   TownsThread::RUNMODE_POWER_OFF==townsThreadPtr->GetRunMode() ||
	   TownsThread::RUNMODE_EXIT==townsThreadPtr->GetRunMode())
	{
		Free();
		Alloc();

		townsPtr->Setup(*townsPtr,outsideWorldPtr,profile);
		townsThreadPtr->SetRunMode(TownsThread::RUNMODE_RUN);
		townsThreadPtr->VMStart(townsPtr,outsideWorldPtr,outsideWorldSoundPtr,cmdQueuePtr);
	}
	else
	{
		townsThreadPtr->SetRunMode(TownsThread::RUNMODE_RUN);
	}

	townsThreadPtr->VMMainLoop(townsPtr,outsideWorldPtr,outsideWorldSoundPtr,cmdQueuePtr);

	if(TownsThread::RUNMODE_EXIT==townsThreadPtr->GetRunMode())
	{
		townsThreadPtr->VMEnd(townsPtr,outsideWorldPtr,cmdQueuePtr);
		Free();
	}
	else
	{
		townsPtr->ForceRender(lastImage,*outsideWorldPtr);
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
