#include "runvm.h"
#include "fssimplewindow_connection.h"


/* virtual */ void TownsCommandQueue::Main(TownsThread &,FMTownsCommon &,const TownsARGV &,Outside_World &)
{
}
/* virtual */ void TownsCommandQueue::ExecCommandQueue(TownsThread &vmThread,FMTownsCommon &towns,Outside_World *outside_world,Outside_World::Sound *sound)
{
	for(int i=0; i<2; ++i)
	{
		auto &cmdqueue=(i==0 ? this->cmdqueue : outside_world->commandQueue);
		while(true!=cmdqueue.empty())
		{
			auto cmdline=cmdqueue.front();
			cmdqueue.pop();

			std::cout << cmdline << std::endl;

			auto cmd=cmdInterpreter.Interpret(cmdline);
			cmdInterpreter.Execute(vmThread,towns,outside_world,sound,cmd);
			if(TownsCommandInterpreter::CMD_QUIT==cmd.primaryCmd)
			{
			}
		}
	}
}

void TownsCommandQueue::SendCommand(std::string cmd)
{
	uiLock.lock();
	this->cmdqueue.push(cmd);
	uiLock.unlock();
}
