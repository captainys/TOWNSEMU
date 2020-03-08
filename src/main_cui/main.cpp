#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#include "towns.h"
#include "townsthread.h"
#include "townscommand.h"
#include "townsargv.h"
#include "i486symtable.h"
#include "cpputil.h"
#include "discimg.h"

#include "fssimplewindow_connection.h"



class TownsCUIThread : public TownsUIThread
{
	virtual void Main(TownsThread &vmThread,FMTowns &towns,const TownsARGV &argv,Outside_World &outside_world);
};

/* virtual */ void TownsCUIThread::Main(TownsThread &townsThread,FMTowns &towns,const TownsARGV &argv,Outside_World &outside_world)
{
	TownsCommandInterpreter cmdInterpreter;
	for(;;)
	{
		if(true==argv.interactive)
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
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			townsThread.vmLock.lock();
			auto vmTerminate=towns.var.powerOff;
			townsThread.vmLock.unlock();
			if(true==vmTerminate)
			{
				break;
			}
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
}



int Run(FMTowns &towns,const TownsARGV &argv,Outside_World &outside_world)
{
	TownsThread townsThread;
	if(true==argv.autoStart)
	{
		if(true==argv.debugger)
		{
			townsThread.SetRunMode(TownsThread::RUNMODE_DEBUGGER);
		}
		else
		{
			townsThread.SetRunMode(TownsThread::RUNMODE_FREE);
		}
	}

	TownsCUIThread cuiThread;

	std::thread UIThread(&TownsCUIThread::Run,&cuiThread,&townsThread,&towns,&argv,&outside_world);
	townsThread.Start(&towns,&outside_world);

	UIThread.join();

	return towns.var.returnCode;
}

bool Setup(FMTowns &towns,const TownsARGV &argv)
{
	if(""==argv.ROMPath)
	{
		std::cout << "Usage:\n";
		std::cout << "main_cui rom_directory_name\n";
		return false;
	}

	if(true!=towns.LoadROMImages(argv.ROMPath.c_str()))
	{
		std::cout << towns.physMem.abortReason << std::endl;
		return false;
	}
	for(int drv=0; drv<2; ++drv)
	{
		if(""!=argv.fdImgFName[drv])
		{
			towns.fdc.LoadRawBinary(drv,argv.fdImgFName[drv].c_str());
		}
	}
	if(""!=argv.cdImgFName)
	{
		auto errCode=towns.cdrom.state.GetDisc().Open(argv.cdImgFName);
		if(DiscImage::ERROR_NOERROR!=errCode)
		{
			std::cout << DiscImage::ErrorCodeToText(errCode);
		}
	}

	if(0<argv.symbolFName.size())
	{
		towns.debugger.GetSymTable().fName=argv.symbolFName;
		if(true==towns.debugger.GetSymTable().Load(argv.symbolFName.c_str()))
		{
			std::cout << "Loaded Symbol Table: " << argv.symbolFName << std::endl;
		}
		else
		{
			std::cout << "Failed to Load Symbol Table: " << argv.symbolFName << std::endl;
			std::cout << "Will create a new file." << argv.symbolFName << std::endl;
		}
	}

	towns.var.freeRunTimerShift=9;
	std::cout << "**** For Testing Purpose Free-Running Timer is scaled up by 512.\n";

	std::cout << "Loaded ROM Images.\n";

	towns.Reset();
	towns.physMem.takeJISCodeLog=true;
	towns.io.EnableLog();
	towns.cpu.enableCallStack=true;

	std::cout << "Virtual Machine Reset.\n";

	return true;
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


	FMTowns towns;
	if(true!=Setup(towns,argv))
	{
		return 1;
	}

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
	return Run(towns,argv,*outside_world);
}
