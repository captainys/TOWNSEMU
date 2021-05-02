#include "runvm.h"
#include "fssimplewindow_connection.h"


/* virtual */ void TownsCommandQueue::Main(TownsThread &,FMTowns &,const TownsARGV &,Outside_World &)
{
}
/* virtual */ void TownsCommandQueue::ExecCommandQueue(TownsThread &vmThread,FMTowns &towns,Outside_World *outside_world)
{
	if(""!=this->cmdline)
	{
		auto cmd=cmdInterpreter.Interpret(this->cmdline);
		cmdInterpreter.Execute(vmThread,towns,outside_world,cmd);
		if(TownsCommandInterpreter::CMD_QUIT==cmd.primaryCmd)
		{
		}
		this->cmdline="";
	}
}

TownsVM::TownsVM()
{
}
TownsVM::~TownsVM()
{
	Free();
}

void TownsVM::Alloc(void)
{
	townsPtr=new FMTowns;
	townsThreadPtr=new TownsThread;
	cmdQueuePtr=new TownsCommandQueue;
	outsideWorldPtr=new FsSimpleWindowConnection;
	townsThreadPtr->SetRunMode(TownsThread::RUNMODE_POWER_OFF);
	townsThreadPtr->SetReturnOnPause(true);
}
void TownsVM::Free(void)
{
	delete townsPtr;
	delete townsThreadPtr;
	delete cmdQueuePtr;
	delete outsideWorldPtr;
	townsPtr=nullptr;
	townsThreadPtr=nullptr;
	cmdQueuePtr=nullptr;
	outsideWorldPtr=nullptr;
}

void TownsVM::Run(void)
{
	if(nullptr==townsPtr ||
	   TownsThread::RUNMODE_POWER_OFF==townsThreadPtr->GetRunMode() ||
	   TownsThread::RUNMODE_EXIT==townsThreadPtr->GetRunMode())
	{
		Free();
		Alloc();

		FMTowns::Setup(*townsPtr,outsideWorldPtr,profile);
		townsThreadPtr->SetRunMode(TownsThread::RUNMODE_RUN);
		townsThreadPtr->VMStart(townsPtr,outsideWorldPtr,cmdQueuePtr);
	}
	else
	{
		townsThreadPtr->SetRunMode(TownsThread::RUNMODE_RUN);
	}

	townsThreadPtr->VMMainLoop(townsPtr,outsideWorldPtr,cmdQueuePtr);

	if(TownsThread::RUNMODE_EXIT==townsThreadPtr->GetRunMode())
	{
		townsThreadPtr->VMEnd(townsPtr,outsideWorldPtr,cmdQueuePtr);
		Free();
	}
}
bool TownsVM::IsRunning(void) const
{
	return
		(nullptr!=townsThreadPtr &&
		 TownsThread::RUNMODE_POWER_OFF!=townsThreadPtr->GetRunMode() &&
		 TownsThread::RUNMODE_EXIT!=townsThreadPtr->GetRunMode());
}
