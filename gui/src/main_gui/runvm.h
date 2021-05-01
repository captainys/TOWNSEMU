#ifndef RUNVM_IS_INCLUDED
#define RUNVM_IS_INCLUDED
/* { */



#include "towns.h"
#include "townsthread.h"
#include "outside_world.h"
#include "townscommand.h"


class TownsCommandQueue : public TownsUIThread
{
public:
	using TownsUIThread::uiLock;
	std::string cmdline;
	TownsCommandInterpreter cmdInterpreter;

	virtual void Main(TownsThread &vmThread,FMTowns &towns,const TownsARGV &argv,Outside_World &outside_world);
	virtual void ExecCommandQueue(TownsThread &vmThread,FMTowns &towns,Outside_World *outside_world);
};

class TownsVM
{
public:
	TownsThread *townsThreadPtr;
	TownsCommandQueue *cmdQueuePtr;
	Outside_World *outsideWorldPtr;

	TownsVM();
	~TownsVM();
	void Run(void);
};



/* } */
#endif
