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


class TownsCommandQueue : public TownsUIThread
{
public:
	using TownsUIThread::uiLock;
	std::queue <std::string> cmdqueue;
	TownsCommandInterpreter cmdInterpreter;

	virtual void Main(TownsThread &vmThread,FMTowns &towns,const TownsARGV &argv,Outside_World &outside_world);
	virtual void ExecCommandQueue(TownsThread &vmThread,FMTowns &towns,Outside_World *outside_world);

	void SendCommand(std::string cmd);
};

class TownsVM
{
public:
	TownsProfile profile;
	FMTowns *townsPtr=nullptr;
	TownsThread *townsThreadPtr=nullptr;
	TownsCommandQueue *cmdQueuePtr=nullptr;
	Outside_World *outsideWorldPtr=nullptr;

	TownsVM();
	~TownsVM();

	void Alloc(void);
	void Free(void);

	void Run(void);
	bool IsRunning(void) const;
	void SendCommand(std::string cmd);
};



/* } */
#endif
