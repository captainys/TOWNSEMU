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
	townsThreadPtr=new TownsThread;
	cmdQueuePtr=new TownsCommandQueue;
	outsideWorldPtr=new FsSimpleWindowConnection;

}
TownsVM::~TownsVM()
{
	delete townsThreadPtr;
	delete cmdQueuePtr;
	delete outsideWorldPtr;
}
void TownsVM::Run(void)
{
}
