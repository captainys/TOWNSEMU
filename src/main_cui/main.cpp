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



class TownsCUIThread : public TownsUIThread
{
public:
	using TownsUIThread::uiLock;
	std::string cmdline;
	TownsCommandInterpreter cmdInterpreter;
	bool uiTerminate=false;

	virtual void Main(TownsThread &vmThread,FMTowns &towns,const TownsARGV &argv,Outside_World &outside_world);
	virtual void ExecCommandQueue(TownsThread &vmThread,FMTowns &towns,Outside_World *outside_world);
};

/* virtual */ void TownsCUIThread::Main(TownsThread &townsThread,FMTowns &towns,const TownsARGV &argv,Outside_World &outside_world)
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

/* virtual */ void TownsCUIThread::ExecCommandQueue(TownsThread &townsThread,FMTowns &towns,Outside_World *outside_world)
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
	else if(""!=this->cmdline)
	{
		auto cmd=cmdInterpreter.Interpret(this->cmdline);
		cmdInterpreter.Execute(townsThread,towns,outside_world,cmd);
		if(TownsCommandInterpreter::CMD_QUIT==cmd.primaryCmd)
		{
			uiTerminate=true;
		}
		this->cmdline="";
	}
}



int Run(FMTowns &towns,const TownsARGV &argv,Outside_World &outside_world)
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
	townsThread.Start(&towns,&outside_world,&cuiThread);

	UIThread.join();

	return towns.var.returnCode;
}

bool Setup(FMTowns &towns,Outside_World *outside_world,const TownsARGV &argv)
{
	if(""==argv.ROMPath)
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "  Tsugaru_CUI rom_directory_name" << std::endl;
		std::cout << "or," << std::endl;
		std::cout << "  Tsugaru_CUI -HELP" << std::endl;
		return false;
	}

	if(true!=towns.LoadROMImages(argv.ROMPath.c_str()))
	{
		std::cout << towns.vmAbortReason << std::endl;
		return false;
	}
	for(int drv=0; drv<2; ++drv)
	{
		if(""!=argv.fdImgFName[drv])
		{
			towns.fdc.LoadD77orRAW(drv,argv.fdImgFName[drv].c_str());
			if(true==argv.fdImgWriteProtect[drv])
			{
				// D77 image may have write-protect switch.
				// Enable write protect only when specified by the parameter.
				towns.fdc.SetWriteProtect(drv,true);
			}
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

	for(auto &scsi : argv.scsiImg)
	{
		if(scsi.imageType==TownsARGV::SCSIIMAGE_HARDDISK)
		{
			if(true!=towns.scsi.LoadHardDiskImage(scsi.scsiID,scsi.imgFName))
			{
				std::cout << "Failed to load hard disk image." << std::endl;
			}
		}
		else if(scsi.imageType==TownsARGV::SCSIIMAGE_CDROM)
		{
			std::cout << "SCSI CD-ROM is not supported yet." << std::endl;
		}
	}

	if(0!=argv.freq)
	{
		towns.state.freq=argv.freq;
	}

	if(0!=argv.memSizeInMB)
	{
		int megabyte=argv.memSizeInMB;
		if(64<megabyte)
		{
			megabyte=64;
		}
		towns.SetMainRAMSize(megabyte*1024*1024);
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

	if(0<argv.playbackEventLogFName.size())
	{
		if(true==towns.eventLog.LoadEventLog(argv.playbackEventLogFName))
		{
			towns.eventLog.BeginPlayback();
		}
	}


	towns.var.freeRunTimerShift=0;

	if(true==argv.autoSaveCMOS)
	{
		towns.var.CMOSFName=argv.CMOSFName;
	}
	else
	{
		towns.var.CMOSFName="";
	}
	if(0<argv.CMOSFName.size())
	{
		auto CMOSBinary=cpputil::ReadBinaryFile(argv.CMOSFName);
		if(0<CMOSBinary.size())
		{
			towns.physMem.SetCMOS(CMOSBinary);
		}
	}
	if(""!=argv.keyMapFName)
	{
		std::ifstream ifp(argv.keyMapFName);
		if(ifp.is_open())
		{
			std::vector <std::string> text;
			while(true!=ifp.eof())
			{
				std::string str;
				std::getline(ifp,str);
				text.push_back(str);
			}
			outside_world->LoadKeyMappingFromText(text);
		}
		else
		{
			std::cout << "Cannot load key-mapping file." << std::endl;
			return false;
		}
	}

	towns.keyboard.SetBootKeyCombination(argv.bootKeyComb);
	towns.gameport.SetBootKeyCombination(argv.bootKeyComb);

	towns.state.mouseIntegrationSpeed=argv.mouseIntegrationSpeed;
	towns.state.noWait=argv.noWait;
	towns.state.pretend386DX=argv.pretend386DX;
	towns.var.noWaitStandby=argv.noWaitStandby;
	towns.state.appSpecificSetting=argv.appSpecificSetting;

	towns.var.catchUpRealTime=argv.catchUpRealTime;

	if(0<=argv.fmVol)
	{
		towns.sound.state.ym2612.state.volume=argv.fmVol;
	}
	if(0<=argv.pcmVol)
	{
		towns.sound.state.rf5c68.state.volume=argv.fmVol;
	}

	if(true==argv.powerOffAtBreakPoint)
	{
		towns.var.powerOffAt.MakeFromString(argv.powerOffAt);
		towns.debugger.AddBreakPoint(towns.var.powerOffAt);
	}

	std::cout << "Loaded ROM Images.\n";

	towns.Reset();
	towns.physMem.takeJISCodeLog=true;

	std::cout << "Virtual Machine Reset.\n";

	for(int i=0; i<TOWNS_NUM_GAMEPORTS; ++i)
	{
		outside_world->gamePort[i]=argv.gamePort[i];
		switch(argv.gamePort[i])
		{
		case TOWNS_GAMEPORTEMU_NONE:
			towns.gameport.state.ports[i].device=TownsGamePort::NONE;
			break;
		case TOWNS_GAMEPORTEMU_MOUSE:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_KEY:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_NUMPAD:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL0:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL1:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL2:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL3:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL4:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL5:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL6:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_PHYSICAL7:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG0:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG1:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG2:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG3:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG4:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG5:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG6:
		case TOWNS_GAMEPORTEMU_MOUSE_BY_ANALOG7:
			towns.gameport.state.ports[i].device=TownsGamePort::MOUSE;
			break;
		case TOWNS_GAMEPORTEMU_PHYSICAL0_AS_CYBERSTICK:
		case TOWNS_GAMEPORTEMU_PHYSICAL1_AS_CYBERSTICK:
		case TOWNS_GAMEPORTEMU_PHYSICAL2_AS_CYBERSTICK:
		case TOWNS_GAMEPORTEMU_PHYSICAL3_AS_CYBERSTICK:
		case TOWNS_GAMEPORTEMU_PHYSICAL4_AS_CYBERSTICK:
		case TOWNS_GAMEPORTEMU_PHYSICAL5_AS_CYBERSTICK:
		case TOWNS_GAMEPORTEMU_PHYSICAL6_AS_CYBERSTICK:
		case TOWNS_GAMEPORTEMU_PHYSICAL7_AS_CYBERSTICK:
			towns.gameport.state.ports[i].device=TownsGamePort::CYBERSTICK;
			break;
		default:
			towns.gameport.state.ports[i].device=TownsGamePort::GAMEPAD;
			break;
		}
	}

	for(auto i=0; i<argv.sharedDir.size() && i<TownsVnDrv::MAX_NUM_SHARED_DIRECTORIES; ++i)
	{
		towns.vndrv.sharedDir[i].hostPath=argv.sharedDir[i];
		towns.vndrv.sharedDir[i].linked=true;
	}

	towns.crtc.state.highResAvailable=argv.highResAvailable;

	for(auto vk : argv.virtualKeys)
	{
		auto townsKey=TownsStrToKeyCode(vk.townsKey);
		if(TOWNS_JISKEY_NULL==townsKey)
		{
			townsKey=TownsStrToKeyCode("TOWNS_JISKEY_"+vk.townsKey);
		}
		if(TOWNS_JISKEY_NULL!=townsKey)
		{
			outside_world->AddVirtualKey(townsKey,vk.physicalId,vk.button);
		}
	}

	if(TOWNS_MEMCARD_TYPE_NONE!=argv.memCardType)
	{
		if(true==towns.physMem.state.memCard.LoadRawImage(argv.memCardImgFName))
		{
			towns.physMem.state.memCard.fName=argv.memCardImgFName;
			towns.physMem.state.memCard.changed=false;  // Because it was already in upon power-on.
		}
	}

	outside_world->strikeCommanderThrottlePhysicalId=argv.strikeCommanderThrottlePhysicalId;
	outside_world->strikeCommanderThrottleAxis=argv.strikeCommanderThrottleAxis;

	outside_world->scaling=argv.scaling;
	outside_world->windowShift=argv.windowShift;

	outside_world->mouseByFlightstickAvailable=argv.mouseByFlightstickAvailable;
	outside_world->mouseByFlightstickPhysicalId=argv.mouseByFlightstickPhysicalId;
	outside_world->mouseByFlightstickZeroZoneX=argv.mouseByFlightstickZeroZoneX;
	outside_world->mouseByFlightstickZeroZoneY=argv.mouseByFlightstickZeroZoneY;
	outside_world->mouseByFlightstickCenterX=argv.mouseByFlightstickCenterX;
	outside_world->mouseByFlightstickCenterY=argv.mouseByFlightstickCenterY;
	outside_world->mouseByFlightstickScaleX=argv.mouseByFlightstickScaleX;
	outside_world->mouseByFlightstickScaleY=argv.mouseByFlightstickScaleY;

	outside_world->CacheGamePadIndicesThatNeedUpdates();

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


	static FMTowns towns;
	Outside_World *outside_world=new FsSimpleWindowConnection;
	if(true!=Setup(towns,outside_world,argv))
	{
		return 1;
	}

	return Run(towns,argv,*outside_world);
}
