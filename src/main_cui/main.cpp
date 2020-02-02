#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#include "towns.h"
#include "townsthread.h"
#include "townscommand.h"
#include "townsargv.h"
#include "cpputil.h"

#include "fssimplewindow_connection.h"



void Run(FMTowns &towns,Outside_World &outside_world)
{
	TownsThread townsThread;
	std::thread stdTownsThread(&TownsThread::Start,&townsThread,&towns,&outside_world);

	TownsCommandInterpreter cmdInterpreter;
	for(;;)
	{
		std::string cmdline;
		std::cout << ">";
		std::getline(std::cin,cmdline);

		auto cmd=cmdInterpreter.Interpret(cmdline);
		townsThread.signalLock.lock();
		townsThread.vmLock.lock();
		cmdInterpreter.Execute(townsThread,towns,cmd);
		townsThread.vmLock.unlock();
		townsThread.signalLock.unlock();
		if(TownsCommandInterpreter::CMD_QUIT==cmd.primaryCmd)
		{
			break;
		}
		if(true==cmdInterpreter.waitVM)
		{
			for(;;)
			{
				unsigned int vmState;
				townsThread.vmLock.lock();
				vmState=townsThread.GetRunMode();
				townsThread.vmLock.unlock();
				if(TownsThread::RUNMODE_PAUSE==vmState ||
				   TownsThread::RUNMODE_EXIT==vmState)
				{
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	}

	stdTownsThread.join();
}



int main(int ac,char *av[])
{
	if(sizeof(void *)<8)
	{
		printf("This requires minimum 64-bit CPU.\n");
		return 0;
	}


	TownsARGV argv;
	if(true!=argv.AnalyzeCommandParameter(ac,av))
	{
		return 1;
	}
	if(""==argv.ROMPath)
	{
		printf("Usage:\n");
		printf("main_cui rom_directory_name\n");
		return 1;
	}




	FMTowns towns;
	if(true!=towns.LoadROMImages(argv.ROMPath.c_str()))
	{
		std::cout << towns.physMem.abortReason << std::endl;
		return 1;
	}
	for(int drv=0; drv<2; ++drv)
	{
		if(""!=argv.fdImgFName[drv])
		{
			towns.fdc.LoadRawBinary(drv,argv.fdImgFName[drv].c_str());
		}
	}

	towns.var.freeRunTimerShift=9;
	printf("**** For Testing Purpose Free-Running Timer is scaled up by 512.\n");

	printf("Loaded ROM Images.\n");

	towns.Reset();
	towns.physMem.takeJISCodeLog=true;
	towns.io.EnableLog();
	towns.cpu.enableCallStack=true;

	printf("Virtual Machine Reset.\n");

	unsigned int eightBytesCSEIP[8]=
	{
		towns.FetchByteCS_EIP(0),
		towns.FetchByteCS_EIP(1),
		towns.FetchByteCS_EIP(2),
		towns.FetchByteCS_EIP(3),
		towns.FetchByteCS_EIP(4),
		towns.FetchByteCS_EIP(5),
		towns.FetchByteCS_EIP(6),
		towns.FetchByteCS_EIP(7)
	};
	for(auto b : eightBytesCSEIP)
	{
		std::cout << cpputil::Ubtox(b) << std::endl;
	}

	Outside_World *outside_world=new FsSimpleWindowConnection ;
	Run(towns,*outside_world);

	return 0;
}
