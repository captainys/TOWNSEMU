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
#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "cpputil.h"
#include "towns.h"
#include "townsdef.h"
#include "townsmap.h"
#include "render.h"
#include "outside_world.h"
#include "i486symtable.h"



void FMTownsCommon::State::PowerOn(void)
{
	Reset();
	townsTime=0;
	nextSecondInTownsTime=PER_SECOND;
	nextFastDevicePollingTime=FAST_DEVICE_POLLING_INTERVAL;
	currentFreq=FREQUENCY_DEFAULT;
	fastModeFreq=FREQUENCY_DEFAULT;
	resetReason=0;
	nextRenderingTime=0;
}

/* static */ bool FMTownsCommon::Setup(FMTownsCommon &towns,Outside_World *outside_world,Outside_World::WindowInterface *windowInterface,const TownsStartParameters &argv)
{
	if(""==argv.ROMPath)
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "  Tsugaru_CUI rom_directory_name" << std::endl;
		std::cout << "or," << std::endl;
		std::cout << "  Tsugaru_CUI -HELP" << std::endl;
		return false;
	}

	towns.var.fileNameAlias=argv.fileNameAlias;

	if(true!=towns.LoadROMImages(argv.ROMPath.c_str()))
	{
		std::cout << towns.vmAbortReason << std::endl;
		return false;
	}

	if(TOWNSTYPE_MARTY==argv.townsType &&
	   TOWNSADDR_MARTY_ROM3_END-TOWNSADDR_MARTY_ROM0_BASE!=towns.physMem.martyRom.size())
	{
		std::cout << "Marty requires EX-ROM images." << std::endl;
		return false;
	}

	towns.fdc.fastDrive=argv.fastFD;
	towns.fdc.searchPaths=argv.fdSearchPaths;
	for(int drv=0; drv<4; ++drv)
	{
		if(""!=argv.fdImgFName[drv])
		{
			auto imgFileName=towns.var.ApplyAlias(argv.fdImgFName[drv]);
			towns.fdc.LoadD77orRDDorRAW(drv,imgFileName.c_str(),towns.state.townsTime);
			if(true==argv.fdImgWriteProtect[drv])
			{
				// D77 image may have write-protect switch.
				// Enable write protect only when specified by the parameter.
				towns.fdc.SetWriteProtect(drv,true);
			}
		}
	}

	if(TOWNSTYPE_UNKNOWN!=argv.townsType)
	{
		towns.townsType=argv.townsType;
		towns.physMem.SetUpMemoryAccess(argv.townsType,towns.TownsTypeToCPUType(argv.townsType));
	}

	towns.cdrom.searchPaths=argv.cdSearchPaths;
	if(""!=argv.cdImgFName)
	{
		auto imgFileName=towns.var.ApplyAlias(argv.cdImgFName);
		auto errCode=towns.cdrom.state.GetDisc().Open(imgFileName);
		if(DiscImage::ERROR_NOERROR!=errCode)
		{
			std::cout << DiscImage::ErrorCodeToText(errCode);
		}
	}
	if(0!=argv.cdSpeed)
	{
		towns.cdrom.state.readSectorTime=TOWNS_CD_READ_SECTOR_TIME_1X/argv.cdSpeed;
		towns.cdrom.state.maxSeekTime=TOWNS_CD_SEEK_TIME_1X/argv.cdSpeed;
	}

	towns.scsi.fastMode=argv.fastSCSI;
	for(int scsiID=0; scsiID<TownsStartParameters::MAX_NUM_SCSI_DEVICES; ++scsiID)
	{
		auto scsi=argv.scsiImg[scsiID];
		auto imgFileName=towns.var.ApplyAlias(scsi.imgFName);
		if(scsi.imageType==TownsStartParameters::SCSIIMAGE_HARDDISK)
		{
			if(true!=towns.scsi.LoadHardDiskImage(scsiID,imgFileName))
			{
				std::cout << "Failed to load hard disk image." << std::endl;
			}
		}
		else if(scsi.imageType==TownsStartParameters::SCSIIMAGE_CDROM)
		{
			if(true!=towns.scsi.LoadCDImage(scsiID,imgFileName))
			{
				std::cout << "Failed to load SCSI CD image." << std::endl;
			}
		}
	}

	if(0!=argv.freq)
	{
		towns.state.currentFreq=argv.freq;
		towns.state.fastModeFreq=argv.freq;
	}
	if(0!=argv.slowModeFreq)
	{
		towns.var.slowModeFreq=argv.slowModeFreq;
	}
	towns.CPU().state.fpuState.enabled=argv.useFPU;

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
	if(true==argv.zeroCMOS)
	{
		std::vector <unsigned char> cmos;
		for(int i=0; i<TOWNS_CMOS_SIZE; ++i)
		{
			cmos.push_back(0);
		}
		towns.physMem.SetCMOS(cmos);
	}
	if(true==argv.alwaysBootToFASTMode)
	{
		towns.physMem.state.CMOSRAM[TOWNS_CMOSRAM_FASTMODE_FLAG]=1;
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
	towns.var.considerVRAMOffsetInMouseIntegration=argv.considerVRAMOffsetInMouseIntegration;
	towns.var.mouseMinX=argv.mouseMinX;
	towns.var.mouseMaxX=argv.mouseMaxX;
	towns.var.mouseMinY=argv.mouseMinY;
	towns.var.mouseMaxY=argv.mouseMaxY;
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
		towns.sound.state.rf5c68.state.volume=argv.pcmVol;
	}
	towns.sound.var.maximumDoubleBuffering=argv.maximumSoundDoubleBuffering;

	if(true==argv.powerOffAtBreakPoint)
	{
		i486Debugger::BreakPointInfo info;
		info.passCountUntilBreak=argv.powerOffAtPassCount;
		towns.var.powerOffAt.MakeFromString(argv.powerOffAt);
		towns.debugger.AddBreakPoint(towns.var.powerOffAt,info);
	}

	std::cout << "Loaded ROM Images.\n";

	towns.Reset();
	towns.physMem.takeJISCodeLog=false;

	std::cout << "Virtual Machine Reset.\n";

	for(int i=0; i<TOWNS_NUM_GAMEPORTS; ++i)
	{
		outside_world->gamePort[i]=argv.gamePort[i];
		towns.gameport.state.ports[i].device=TownsGamePort::EmulationTypeToDeviceType(argv.gamePort[i]);
		towns.gameport.state.ports[i].maxButtonHoldTime[0]=argv.maxButtonHoldTime[i][0];
		towns.gameport.state.ports[i].maxButtonHoldTime[1]=argv.maxButtonHoldTime[i][1];
	}

	towns.AppSpecificSetup(outside_world,argv);

	for(auto i=0; i<argv.sharedDir.size() && i<TownsVnDrv::MAX_NUM_SHARED_DIRECTORIES; ++i)
	{
		towns.vndrv.sharedDir[i].hostPath=argv.sharedDir[i];
		towns.vndrv.sharedDir[i].linked=true;

		towns.tgdrv.sharedDir[i].hostPath=argv.sharedDir[i];
		towns.tgdrv.sharedDir[i].linked=true;
	}

	towns.crtc.state.highResAvailable=argv.highResAvailable;

	towns.highResPCM.state.enabled=argv.highResPCM;

	if(TOWNS_KEYBOARD_MODE_DEFAULT!=argv.keyboardMode)
	{
		outside_world->keyboardMode=argv.keyboardMode;
	}
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
			towns.physMem.state.memCard.memCardType=argv.memCardType;
			towns.physMem.state.memCard.fName=argv.memCardImgFName;
			towns.physMem.state.memCard.changed=false;  // Because it was already in upon power-on.
			towns.physMem.state.memCard.writeProtected=argv.memCardWriteProtected;
		}
	}

	towns.var.damperWireLine=argv.damperWireLine;
	towns.var.scanLineEffectIn15KHz=argv.scanLineEffectIn15KHz;
	towns.var.forceQuitOnPowerOff=argv.forceQuitOnPowerOff;

	outside_world->throttlePhysicalId=argv.throttlePhysicalId;
	outside_world->throttleAxis=argv.throttleAxis;

	outside_world->scaling=argv.scaling;
	windowInterface->shared.scaling=argv.scaling;
	windowInterface->autoScaling=argv.autoScaling;
	windowInterface->windowShift=argv.windowShift;
	windowInterface->windowModeOnStartUp=argv.windowModeOnStartUp;

	outside_world->mouseByFlightstickAvailable=argv.mouseByFlightstickAvailable;
	outside_world->cyberStickAssignment=argv.cyberStickAssignment;
	outside_world->mouseByFlightstickPhysicalId=argv.mouseByFlightstickPhysicalId;
	outside_world->mouseByFlightstickZeroZoneX=argv.mouseByFlightstickZeroZoneX;
	outside_world->mouseByFlightstickZeroZoneY=argv.mouseByFlightstickZeroZoneY;
	outside_world->mouseByFlightstickCenterX=argv.mouseByFlightstickCenterX;
	outside_world->mouseByFlightstickCenterY=argv.mouseByFlightstickCenterY;
	outside_world->mouseByFlightstickScaleX=argv.mouseByFlightstickScaleX;
	outside_world->mouseByFlightstickScaleY=argv.mouseByFlightstickScaleY;

	outside_world->CacheGamePadIndicesThatNeedUpdates();

	// Start-Up VM State cannot be loaded until outside_world is initialized.
	// If the VM state is loaded here, CDDA will not be started.
	// It should be delayed.
	towns.var.startUpStateFName=argv.startUpStateFName;

	towns.var.quickScrnShotDir=argv.quickScrnShotDir;
	towns.var.scrnShotX0=argv.scrnShotX0;
	towns.var.scrnShotY0=argv.scrnShotY0;
	towns.var.scrnShotWid=argv.scrnShotWid;
	towns.var.scrnShotHei=argv.scrnShotHei;

	for(int i=0; i<2; ++i)
	{
		if(""!=argv.mapXYExpression[i])
		{
			towns.mapXY[i].Decode(argv.mapXYExpression[i]);
			if(true==towns.mapXY[i].error)
			{
				std::cout << "Error in map-XY expression" << std::endl;
				std::cout << towns.mapXY[i].errorMessage << std::endl;
			}
		}
	}


	for(auto hsc : argv.hostShortCutKeys)
	{
		outside_world->RegisterHostShortCut(hsc.hostKey,hsc.ctrl,hsc.shift,hsc.cmdStr);
	}
	outside_world->RegisterPauseResume(argv.pauseResumeKeyLabel);

	towns.var.quickStateSaveFName=argv.quickStateSaveFName;

	if(""!=argv.RS232CtoTCPAddr)
	{
		if(towns.serialport.ConnectSocketClient(argv.RS232CtoTCPAddr))
		{
			std::cout << "COM0 connected to " << argv.RS232CtoTCPAddr << std::endl;
		}
		else
		{
			std::cout << "Warning!  RS232C to TCP forwarding failed to connect!" << std::endl;
			std::cout << "Host address " << argv.RS232CtoTCPAddr << std::endl;
		}
	}

	return true;
}

void FMTownsCommon::AppSpecificSetup(Outside_World *outside_world,const TownsStartParameters &argv)
{
	switch(state.appSpecificSetting)
	{
	case TOWNS_APPSPECIFIC_RASHINBAN:
		if(gameport.state.ports[1].device==TOWNS_GAMEPORTEMU_MOUSE)
		{
			gameport.state.ports[0].device=TOWNS_GAMEPORTEMU_MOUSE;
			gameport.state.ports[1].device=TOWNS_GAMEPORTEMU_NONE;
		}
		break;
	case TOWNS_APPSPECIFIC_LEMMINGS2:
		// Lemmings2's FBIOS seems to be extermely sensitive to timing!
		// What version is that?
		// Lower frequency causes CD-ROM BIOS to fail.
		// Faster frequency will cause flickering.
		// So far 18MHz to 20MHz are the only stable frequencies I found that is stable and prevent flickering.
		state.currentFreq=20;
		var.slowModeFreq=state.currentFreq;
		state.fastModeFreq=state.currentFreq;
		cdrom.state.readSectorTime=TOWNS_CD_READ_SECTOR_TIME_1X;
		break;
	case TOWNS_APPSPECIFIC_ORGEL:
		cdrom.state.readSectorTime=TOWNS_CD_READ_SECTOR_TIME_1X;
		cdrom.var.sectorReadTimeDelay=TownsCDROM::SECTORREAD_DELAY_ORGEL;
		break;
	}
}

void FMTownsCommon::State::Reset(void)
{
	clockBalance=0;

	tbiosVersion=TBIOS_UNKNOWN;
	TBIOS_physicalAddr=0;
	MOS_work_linearAddr=0;
	MOS_work_physicalAddr=0;
	mouseBIOSActive=false;
	mouseDisplayPage=0;

	serialROMBitCount=0;
	lastSerialROMCommand=0;

	// resetReason should survive Reset.
}


////////////////////////////////////////////////////////////


FMTownsCommon::Variable::Variable()
{
	freeRunTimerShift=0;
	for(auto &t : timeAdjustLog)
	{
		t=0;
	}
	powerOffAt.SEG=0xFFFF0000;
	powerOffAt.OFFSET=0;
	Reset();
}

void FMTownsCommon::Variable::Reset(void)
{
	// freeRunTimerShift should survive Reset.
	disassemblePointer.SEG=0;
	disassemblePointer.OFFSET=0;

	nVM2HostParam=0;
}

std::string FMTownsCommon::Variable::ApplyAlias(std::string incoming) const
{
	auto INCOMING=incoming;
	cpputil::Capitalize(INCOMING);
	auto found=fileNameAlias.find(INCOMING);
	if(fileNameAlias.end()!=found)
	{
		return found->second;
	}
	return incoming;
}

////////////////////////////////////////////////////////////


FMTownsCommon::FMTownsCommon() : 
	Device(this),
	physMem(this,&mem,&sound.state.rf5c68),
	keyboard(this,&pic),
	crtc(this,&sprite),
	sprite(this,&physMem),
	pic(this),
	dmac(this),
	cdrom(this,&pic,&dmac),
	fdc(this,&pic,&dmac),
	scsi(this),
	rtc(this),
	sound(this),
	gameport(this),
	timer(this,&pic),
	serialport(this),
	vndrv(this),
	tgdrv(this),
	mapXY{this,this},
	highResPCM(this)
{
	/* Memo to myself:
	To instantiate high-fidelity VM and default-fidelity VM in the same executable
	without making RunOneInstruction virtual, CPU class is in the sub-class.
	Therefore, CPU is not ready until sub-class is ready, which is not yet at this line.
	Any initialization, including caching CPU pointer using CPU() function, needs to
	wait until the sub-class constructor.
	*/

	townsType=TOWNSTYPE_2_MX;

	debugger.ioLabel=FMTownsIOMap();
	debugger.GetSymTable().MakeDOSIntFuncLabel();
	MakeINTInfo(debugger.GetSymTable());

	allDevices.push_back(this);
	allDevices.push_back(&pic);
	allDevices.push_back(&dmac);
	allDevices.push_back(&physMem);
	allDevices.push_back(&crtc);
	allDevices.push_back(&sprite);
	allDevices.push_back(&fdc);
	allDevices.push_back(&scsi);
	allDevices.push_back(&cdrom);
	allDevices.push_back(&rtc);
	allDevices.push_back(&sound);
	allDevices.push_back(&gameport);
	allDevices.push_back(&timer);
	allDevices.push_back(&keyboard);
	allDevices.push_back(&serialport);
	allDevices.push_back(&vndrv);
	allDevices.push_back(&tgdrv);
	allDevices.push_back(&highResPCM);
	VMBase::CacheDeviceIndex();

	physMem.SetMainRAMSize(4*1024*1024);

	physMem.SetVRAMSize(TOWNS_VRAM_SIZE);
	physMem.SetCVRAMSize(TOWNS_CVRAM_SIZE);
	physMem.SetSpriteRAMSize(TOWNS_SPRITERAM_SIZE);
	physMem.SetDummySize(TOWNS_WAVERAM_SIZE);
	for(int i=0; i<TOWNS_CMOS_SIZE; ++i)
	{
		physMem.state.CMOSRAM[i]=defCMOS[i];
	}

	physMem.FMRVRAMAccess.townsPtr=this;
	physMem.FMRVRAMAccess.crtcPtr=&this->crtc;

	// Free-run counter since FM TOWNS 2UG [2] pp.801
	// Didn't it exist since the first model FM TOWNS 2?
	// I vaguely rember I used something similar when I wrote my first flight simulator 
	// submitted to Japan National High School Students' Programming Contest.
	// FM TOWNS 2UG didn't exist then.
	// I'm positive that I was using the second-generation FM TOWNS then.
	// I'll check if I can find the source code from my old backups.

	// Do range I/O mapping first, then do single I/O mapping.
	// Range I/O mapping may wipe single I/O mapping.

	// Range I/O mappings >>>
	io.AddDevice(this,TOWNSIO_CPU_MISC3/*0x24*/);
	io.AddDevice(this,TOWNSIO_FREERUN_TIMER_LOW/*0x26*/,TOWNSIO_MACHINE_ID_HIGH/*0x31*/);
	io.AddDevice(&crtc,TOWNSIO_MX_HIRES/*0x470*/,TOWNSIO_MX_IMGOUT_D3/*0x477*/);
	io.AddDevice(&keyboard,TOWNSIO_KEYBOARD_DATA/*0x600*/,TOWNSIO_KEYBOARD_IRQ/*0x604*/);
	io.AddDevice(&fdc,TOWNSIO_FDC_STATUS_COMMAND/*0x200*/,TOWNSIO_FDC_DRIVE_SWITCH/*0x20E*/);
	io.AddDevice(&physMem,TOWNSIO_CMOS_BASE,TOWNSIO_CMOS_END-1);


	// Individual I/O mappings >>>
	io.AddDevice(&scsi,TOWNSIO_SCSI_DATA);           // 0xC30 [2] pp.263
	io.AddDevice(&scsi,TOWNSIO_SCSI_STATUS_CONTROL); // 0xC32 [2] pp.262
	io.AddDevice(&scsi,TOWNSIO_SCSI_WORD_TFR_AVAIL); // 0xC34 [2] pp.801


	io.AddDevice(&crtc,TOWNSIO_CRTC_ADDRESS);//             0x440,
	io.AddDevice(&crtc,TOWNSIO_CRTC_DATA_LOW);//            0x442,
	io.AddDevice(&crtc,TOWNSIO_CRTC_DATA_HIGH);//           0x443,
	io.AddDevice(&crtc,TOWNSIO_HSYNC_VSYNC);//              0xFDA0,
	io.AddDevice(&crtc,TOWNSIO_FMR_HSYNC_VSYNC);//          0xFF86
	io.AddDevice(&crtc,TOWNSIO_VIDEO_OUT_CTRL_ADDRESS);//   0x448,
	io.AddDevice(&crtc,TOWNSIO_VIDEO_OUT_CTRL_DATA);//      0x44A,
	io.AddDevice(&crtc,TOWNSIO_DPMD_SPRITEBUSY_SPRITEPAGE); // 0x44C
	io.AddDevice(&crtc,TOWNSIO_ANALOGPALETTE_CODE);//=  0xFD90,
	io.AddDevice(&crtc,TOWNSIO_ANALOGPALETTE_BLUE);//=  0xFD92,
	io.AddDevice(&crtc,TOWNSIO_ANALOGPALETTE_RED);//=   0xFD94,
	io.AddDevice(&crtc,TOWNSIO_ANALOGPALETTE_GREEN);//= 0xFD96,
	io.AddDevice(&crtc,TOWNSIO_FMR_DIGITALPALETTE0);// 0xFD98,
	io.AddDevice(&crtc,TOWNSIO_FMR_DIGITALPALETTE1);// 0xFD99,
	io.AddDevice(&crtc,TOWNSIO_FMR_DIGITALPALETTE2);// 0xFD9A,
	io.AddDevice(&crtc,TOWNSIO_FMR_DIGITALPALETTE3);// 0xFD9B,
	io.AddDevice(&crtc,TOWNSIO_FMR_DIGITALPALETTE4);// 0xFD9C,
	io.AddDevice(&crtc,TOWNSIO_FMR_DIGITALPALETTE5);// 0xFD9D,
	io.AddDevice(&crtc,TOWNSIO_FMR_DIGITALPALETTE6);// 0xFD9E,
	io.AddDevice(&crtc,TOWNSIO_FMR_DIGITALPALETTE7);// 0xFD9F,
	io.AddDevice(&crtc,TOWNSIO_WRITE_TO_CLEAR_VSYNCIRQ); // 0x5CA


	io.AddDevice(this,TOWNSIO_POWER_CONTROL);   //        0x22
	io.AddDevice(this,TOWNSIO_SERIAL_ROM_CTRL); //        0x32,
	io.AddDevice(this,TOWNSIO_FMR_RESOLUTION); // 0x400
	io.AddDevice(this,TOWNSIO_VM_HOST_IF_CMD_STATUS);
	io.AddDevice(this,TOWNSIO_VM_HOST_IF_DATA);
	io.AddDevice(this,TOWNSIO_ELEVOL_1_DATA); //           0x4E0, // [2] pp.18, pp.174
	io.AddDevice(this,TOWNSIO_ELEVOL_1_COM); //            0x4E1, // [2] pp.18, pp.174
	io.AddDevice(this,TOWNSIO_ELEVOL_2_DATA); //           0x4E2, // [2] pp.18, pp.174
	io.AddDevice(this,TOWNSIO_ELEVOL_2_COM); //            0x4E3, // [2] pp.18, pp.174
	io.AddDevice(this,TOWNSIO_MAINRAM_WAIT_1STGEN); //     0x5E0,
	io.AddDevice(this,TOWNSIO_MAINRAM_WAIT); //            0x5E2,
	io.AddDevice(this,TOWNSIO_VRAMWAIT); //                0x5E6,
	io.AddDevice(this,TOWNSIO_FASTMODE); //                0x5EC, // [2] pp.794




	io.AddDevice(&pic,TOWNSIO_PIC_PRIMARY_ICW1);//          0x00
	io.AddDevice(&pic,TOWNSIO_PIC_PRIMARY_ICW2_3_4_OCW);//  0x02
	io.AddDevice(&pic,TOWNSIO_PIC_SECONDARY_ICW1);//        0x10
	io.AddDevice(&pic,TOWNSIO_PIC_SECONDARY_ICW2_3_4_OCW);//0x12


	io.AddDevice(&dmac,TOWNSIO_DMAC_INITIALIZE);//          0xA0,
	io.AddDevice(&dmac,TOWNSIO_DMAC_CHANNEL);//             0xA1,
	io.AddDevice(&dmac,TOWNSIO_DMAC_COUNT_LOW);//           0xA2,
	io.AddDevice(&dmac,TOWNSIO_DMAC_COUNT_HIGH);//          0xA3,
	io.AddDevice(&dmac,TOWNSIO_DMAC_ADDRESS_LOWEST);//      0xA4,
	io.AddDevice(&dmac,TOWNSIO_DMAC_ADDRESS_MIDLOW);//      0xA5,
	io.AddDevice(&dmac,TOWNSIO_DMAC_ADDRESS_MIDHIGH);//     0xA6,
	io.AddDevice(&dmac,TOWNSIO_DMAC_ADDRESS_HIGHEST);//     0xA7,
	io.AddDevice(&dmac,TOWNSIO_DMAC_DEVICE_CONTROL_LOW);//  0xA8,
	io.AddDevice(&dmac,TOWNSIO_DMAC_DEVICE_CONTROL_HIGH);// 0xA9,
	io.AddDevice(&dmac,TOWNSIO_DMAC_MODE_CONTROL);//        0xAA,
	io.AddDevice(&dmac,TOWNSIO_DMAC_STATUS);//              0xAB,
	io.AddDevice(&dmac,TOWNSIO_DMAC_TEMPORARY_REG_LOW);//   0xAC,
	io.AddDevice(&dmac,TOWNSIO_DMAC_TEMPORARY_REG_HIGH);//  0xAD,
	io.AddDevice(&dmac,TOWNSIO_DMAC_REQUEST);//             0xAE,
	io.AddDevice(&dmac,TOWNSIO_DMAC_MASK);//                0xAF,


	io.AddDevice(this,TOWNSIO_RESET_REASON);
	io.AddDevice(&physMem,TOWNSIO_FMR_VRAM_OR_MAINRAM);
	io.AddDevice(&physMem,TOWNSIO_SYSROM_DICROM);
	io.AddDevice(&physMem,TOWNSIO_DICROM_BANK);
	io.AddDevice(&physMem,TOWNSIO_MEMSIZE);
	io.AddDevice(&physMem,TOWNSIO_FMR_VRAMMASK);
	io.AddDevice(&physMem,TOWNSIO_FMR_VRAMDISPLAYMODE);
	io.AddDevice(&physMem,TOWNSIO_FMR_VRAMPAGESEL);
	io.AddDevice(&physMem,TOWNSIO_TVRAM_WRITE);
	io.AddDevice(&physMem,TOWNSIO_VRAMACCESSCTRL_ADDR); //      0x458, // [2] pp.17,pp.112
	io.AddDevice(&physMem,TOWNSIO_VRAMACCESSCTRL_DATA_LOW); //  0x45A, // [2] pp.17,pp.112
	io.AddDevice(&physMem,TOWNSIO_VRAMACCESSCTRL_DATA_HIGH); // 0x45B, // [2] pp.17,pp.112
	io.AddDevice(&physMem,TOWNSIO_MEMCARD_STATUS); //           0x48A, // [2] pp.93
	io.AddDevice(&physMem,TOWNSIO_MEMCARD_BANK); //             0x490, // [2] pp.794
	io.AddDevice(&physMem,TOWNSIO_MEMCARD_ATTRIB); //           0x491, // [2] pp.795
	io.AddDevice(&physMem,TOWNSIO_KANJI_JISCODE_HIGH);//  0xFF94,
	io.AddDevice(&physMem,TOWNSIO_KANJI_JISCODE_LOW);//   0xFF95,
	io.AddDevice(&physMem,TOWNSIO_KANJI_PTN_HIGH);//      0xFF96,
	io.AddDevice(&physMem,TOWNSIO_KANJI_PTN_LOW);//       0xFF97,
	io.AddDevice(&physMem,TOWNSIO_KVRAM_OR_ANKFONT);//    0xFF99,


	io.AddDevice(&cdrom,TOWNSIO_CDROM_MASTER_CTRL_STATUS);
	io.AddDevice(&cdrom,TOWNSIO_CDROM_COMMAND_STATUS);
	io.AddDevice(&cdrom,TOWNSIO_CDROM_CACHE_2XSPEED);
	io.AddDevice(&cdrom,TOWNSIO_CDROM_PARAMETER_DATA);
	io.AddDevice(&cdrom,TOWNSIO_CDROM_TRANSFER_CTRL);
	io.AddDevice(&cdrom,TOWNSIO_CDROM_SUBCODE_STATUS);
	io.AddDevice(&cdrom,TOWNSIO_CDROM_SUBCODE_DATA);


	io.AddDevice(&rtc,TOWNSIO_RTC_DATA);//                 0x70,
	io.AddDevice(&rtc,TOWNSIO_RTC_COMMAND);//              0x80,


	io.AddDevice(&sprite,TOWNSIO_SPRITE_ADDRESS);//           0x450, // [2] pp.128
	io.AddDevice(&sprite,TOWNSIO_SPRITE_DATA);//              0x452, // [2] pp.128


	io.AddDevice(&gameport,TOWNSIO_GAMEPORT_A_INPUT);  //0x4D0,
	io.AddDevice(&gameport,TOWNSIO_GAMEPORT_B_INPUT);  //0x4D2,
	io.AddDevice(&gameport,TOWNSIO_GAMEPORT_OUTPUT);   //0x4D6,


	io.AddDevice(&sound,TOWNSIO_SOUND_MUTE);//              0x4D5, // [2] pp.18,
	io.AddDevice(&sound,TOWNSIO_SOUND_STATUS_ADDRESS0);//   0x4D8, // [2] pp.18,
	io.AddDevice(&sound,TOWNSIO_SOUND_DATA0);//             0x4DA, // [2] pp.18,
	io.AddDevice(&sound,TOWNSIO_SOUND_ADDRESS1);//          0x4DC, // [2] pp.18,
	io.AddDevice(&sound,TOWNSIO_SOUND_DATA1);//             0x4DE, // [2] pp.18,
	io.AddDevice(&sound,TOWNSIO_SOUND_INT_REASON);//        0x4E9, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_INT_MASK);//      0x4EA, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_INT);//           0x4EB, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_ENV);//           0x4F0, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_PAN);//           0x4F1, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_FDL);//           0x4F2, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_FDH);//           0x4F3, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_LSL);//           0x4F4, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_LSH);//           0x4F5, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_ST);//            0x4F6, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_CTRL);//          0x4F7, // [2] pp.19,
	io.AddDevice(&sound,TOWNSIO_SOUND_PCM_CH_ON_OFF);//     0x4F8, // [2] pp.19,

	io.AddDevice(&sound,TOWNSIO_SOUND_SAMPLING_DATA); //    0x4E7, // [2] pp.179,
	io.AddDevice(&sound,TOWNSIO_SOUND_SAMPLING_FLAGS);//    0x4E8, // [2] pp.179,


	io.AddDevice(&timer,TOWNSIO_TIMER0_COUNT);//             0x40,
	io.AddDevice(&timer,TOWNSIO_TIMER1_COUNT);//             0x42,
	io.AddDevice(&timer,TOWNSIO_TIMER2_COUNT);//             0x44,
	io.AddDevice(&timer,TOWNSIO_TIMER_0_1_2_CTRL);//         0x46,
	io.AddDevice(&timer,TOWNSIO_TIMER3_COUNT);//             0x50,
	io.AddDevice(&timer,TOWNSIO_TIMER4_COUNT);//             0x52,
	io.AddDevice(&timer,TOWNSIO_TIMER5_COUNT);//             0x54,
	io.AddDevice(&timer,TOWNSIO_TIMER_3_4_5_CTRL);//         0x56,
	io.AddDevice(&timer,TOWNSIO_TIMER_INT_CTRL_INT_REASON);

	io.AddDevice(&serialport,TOWNSIO_RS232C_STATUS_COMMAND); // 0xA02, // [2] pp.269
	io.AddDevice(&serialport,TOWNSIO_RS232C_DATA); //           0xA00, // [2] pp.274
	io.AddDevice(&serialport,TOWNSIO_RS232C_INT_REASON); //     0xA06, // [2] pp.275
	io.AddDevice(&serialport,TOWNSIO_RS232C_INT_CONTROL); //    0xA08, // [2] pp.276

	io.AddDevice(&vndrv,TOWNSIO_VNDRV_APICHECK);//       0x2F10,
	io.AddDevice(&vndrv,TOWNSIO_VNDRV_ENABLE);//         0x2F12,
	io.AddDevice(&vndrv,TOWNSIO_VNDRV_COMMAND);//        0x2F14,
	io.AddDevice(&vndrv,TOWNSIO_VNDRV_AUXCOMMAND);//     0x2F18,

	io.AddDevice(&tgdrv,TOWNSIO_VM_TGDRV);

	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_BANK);//         0x510, // [2] pp.832
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_DMASTATUS);//    0x511, // [2] pp.832
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_DMACOUNT_LOW);// 0x512, // [2] pp.833
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_DMACOUNT_HIGH);//0x513, // [2] pp.833
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_DMAADDR_LOW);//  0x514, // [2] pp.834
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_DMAADDR_MIDLOW);//0x515, // [2] pp.834
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_DMAADDR_MIDHIGH);//0x516, // [2] pp.834
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_DMAADDR_HIGH);// 0x517, // [2] pp.834
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_CLOCK);//0x518, // [2] pp.834
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_MODE);//         0x519, // [2] pp.835
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_SYSCONTROL);//   0x51A, // [2] pp.836
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_BUFFCONTROL);//  0x51B, // [2] pp.837
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_REC_PLAYBACK);// 0x51C, // [2] pp.838
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_REC_PEAK_MON);// 0x51D, // [2] pp.839
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_DATA_LOW);//     0x51E, // [2] pp.840
	io.AddDevice(&highResPCM,TOWNSIO_HIGHRESPCM_DATA_HIGH);//    0x51F, // [2] pp.840

	baseClassReady=true;
}

/* static */ int FMTownsCommon::TownsTypeToCPUType(unsigned int townsType)
{
	switch(townsType)
	{
	case FMR_50_60:
	case FMR_50S:
	case FMR_70:
		return TOWNSCPU_UNKNOWN;
	case TOWNSTYPE_MODEL1_2:
	case TOWNSTYPE_1F_2F:
	case TOWNSTYPE_10F_20F:
	case TOWNSTYPE_2_CX:
	case TOWNSTYPE_2_HG:
		return TOWNSCPU_80386DX;
	case TOWNSTYPE_2_UX:
	case TOWNSTYPE_2_UG:
	case TOWNSTYPE_MARTY:
		return TOWNSCPU_80386SX;
	case TOWNSTYPE_2_HR:
	case TOWNSTYPE_2_UR:
	case TOWNSTYPE_2_MA:
	case TOWNSTYPE_2_ME:
	case TOWNSTYPE_2_MF_FRESH:
	// EA
		return TOWNSCPU_80486SX;
	case TOWNSTYPE_2_MX:
	// HA,
		return TOWNSCPU_80486DX;
	case TOWNSTYPE_2_HC:
	// HB
		return TOWNSCPU_PENTIUM;
	// FS,FT
	//   AM486DX
	}
	return TOWNSCPU_UNKNOWN;
}

int FMTownsCommon::GetCPUType(void) const
{
	return TownsTypeToCPUType(townsType);
}

unsigned int FMTownsCommon::MachineID(void) const
{
	const int i80286=0;
	const int i80386=1;
	const int i80486SX=2;
	const int i80486DX=2;
	const int i80386SX=3;
	const int Pentium=2;  // Thanks, WINDY!

	unsigned int lowByte=0,highByte=0;

	switch(townsType)
	{
	case FMR_50_60:
		lowByte=0xF8|i80386;
		break;
	case FMR_50S:
		lowByte=0xE8|i80386;
		break;
	case FMR_70:
		lowByte=0xF0|i80386;
		break;
	default:
	case TOWNSTYPE_MODEL1_2:  // 1st Gen: model1: model2
	case TOWNSTYPE_1F_2F:     // 2nd Gen: 1F:2F
	case TOWNSTYPE_10F_20F:   // 3rd Gen: 10F:20F
		lowByte=i80386; // [2] pp.775
		break;
	case TOWNSTYPE_2_UX:
		lowByte=i80386SX; // [2] pp.781
		break;
	case TOWNSTYPE_2_CX:
		lowByte=i80386; // [2] pp.775  Was it 386?
		break;
	case TOWNSTYPE_2_UG:
	case TOWNSTYPE_2_HG:
	case TOWNSTYPE_2_HR:
		lowByte=i80486SX; // [2] pp.781
		break;
	case TOWNSTYPE_2_UR:
	case TOWNSTYPE_2_MA:
	case TOWNSTYPE_2_MX:
	case TOWNSTYPE_2_ME:
	case TOWNSTYPE_2_MF_FRESH:
		lowByte=i80486DX;
		break;
	case TOWNSTYPE_2_HC:
		lowByte=Pentium;
		break;
	case TOWNSTYPE_MARTY:
		lowByte=i80386SX;
		break;
	}

	if(true==state.pretend386DX)
	{
		lowByte&=0xF8;
		lowByte|=i80386;
	}

	switch(townsType)
	{
	case FMR_50_60:
	case FMR_50S:
	case FMR_70:
		highByte=0xFF;
		break;
	default:
	case TOWNSTYPE_MODEL1_2:  // 1st Gen: model1: model2
		highByte=0x1; // [2] pp.775
		break;
	case TOWNSTYPE_1F_2F: // 1F,2F
		highByte=0x2; // [2] pp.775
		break;
	case TOWNSTYPE_10F_20F:   // 3rd Gen: 10F:20F
		highByte=0x4; // [2] pp.826
		break;
	case TOWNSTYPE_2_UX:
		highByte=0x3; // [2] pp.826
		break;
	case TOWNSTYPE_2_CX:
		highByte=0x5; // [2] pp.826
		break;
	case TOWNSTYPE_2_UG:
		highByte=0x6; // [2] pp.826
		break;
	case TOWNSTYPE_2_HG:
		highByte=0x8; // [2] pp.826
		break;
	case TOWNSTYPE_2_HR:
		highByte=0x7; // [2] pp.826
		break;
	case TOWNSTYPE_2_UR:
		highByte=0x9; // [2] pp.826
		break;
	case TOWNSTYPE_2_MA:
		highByte=0xB; // [2] pp.826
		break;
	case TOWNSTYPE_2_MX: // MX
		highByte=0xC; // [2] pp.826
		break;
	case TOWNSTYPE_2_ME:
		highByte=0xD; // [2] pp.826
		break;
	case TOWNSTYPE_2_MF_FRESH:
		highByte=0xF; // [2] pp.826
		break;
	case TOWNSTYPE_2_HC:
		highByte=0x11; // Thanks, WINDY!
		break;
	case TOWNSTYPE_MARTY:
		// Disassembly of TBIOS.SYS indicated that TBIOS.SYS detects marty by testing highByte&0x40.
		highByte=0x4A;
		break;
	}

	return (highByte<<8)|lowByte;
}

bool FMTownsCommon::LoadROMImages(const char dirName[])
{
	if(true!=physMem.LoadROMImages(dirName))
	{
		Device::Abort("Unable to load ROM images.");
		return false;
	}
	return true;
}

void FMTownsCommon::PowerOn(void)
{
	state.PowerOn();
	CPU().PowerOn();
	for(auto devPtr : allDevices)
	{
		if(devPtr!=this)
		{
			devPtr->PowerOn();
		}
	}
}
void FMTownsCommon::Reset(void)
{
	auto &cpu=CPU();
	var.Reset();
	state.Reset();
	cpu.Reset();
	for(auto devPtr : allDevices)
	{
		if(devPtr!=this)
		{
			devPtr->Reset();
		}
	}

	var.disassemblePointer.SEG=cpu.state.CS().value;
	var.disassemblePointer.OFFSET=cpu.state.EIP;
}

void FMTownsCommon::NotifyDiskRead(void)
{
	keyboard.BootSequenceStarted();
	gameport.BootSequenceStarted();
	state.noWait=var.noWaitStandby;
}

void FMTownsCommon::ProcessSound(Outside_World *outside_world)
{
	sound.ProcessSound();
}

/* virtual */ void FMTownsCommon::InterceptMouseBIOS(void)
{
	auto &cpu=CPU();
	if(0==cpu.GetAH())
	{
		if(TownsEventLog::MODE_RECORDING==eventLog.mode || TownsEventLog::MODE_PLAYBACK==eventLog.mode)
		{
			eventLog.LogMouseStart(state.townsTime);
		}

		unsigned int excType,excCode;
		state.MOS_work_linearAddr=cpu.state.GS().baseLinearAddr+cpu.GetEDI();
		state.MOS_work_physicalAddr=cpu.DebugLinearAddressToPhysicalAddress(excType,excCode,state.MOS_work_linearAddr,mem);

		i486DXCommon::SegmentRegister CS;
		cpu.DebugLoadSegmentRegister(CS,0x110,mem,false);
		state.TBIOS_physicalAddr=cpu.DebugLinearAddressToPhysicalAddress(excType,excCode,CS.baseLinearAddr,mem);
		state.tbiosVersion=IdentifyTBIOS(state.TBIOS_physicalAddr);
		state.TBIOS_mouseInfoOffset=FindTBIOSMouseInfoOffset(state.tbiosVersion,state.TBIOS_physicalAddr);

		state.mouseBIOSActive=true;
		state.mouseDisplayPage=0;

		std::cout << "Identified TBIOS as: " << TBIOSIDENTtoString(state.tbiosVersion) << std::endl;

		// It's a good time for some patches.
		switch(state.appSpecificSetting)
		{
		case TOWNS_APPSPECIFIC_DAIKOUKAIJIDAI:
			Daikoukai_ApplyPatchesCacheAddr();
			break;
		}
	}
	else if(0x04==cpu.GetAH()) // Set Position
	{
		// std::cout << "Set Mouse Position:" << cpu.GetDX() << "," << cpu.GetBX() << std::endl;
	}
	else if(0x0C==cpu.GetAH()) // Set Pulse per Pixel
	{
		// std::cout << "Set Pulse per Pixel:" << cpu.GetDH() << "," << cpu.GetDL() << std::endl;
		state.MOS_pulsePerPixelH=cpu.GetDH();
		state.MOS_pulsePerPixelV=cpu.GetDL();
	}
	else if(0x0E==cpu.GetAH())
	{
		state.mouseDisplayPage=cpu.GetAL();
		// std::cout << "Mouse Display Page: " << state.mouseDisplayPage << std::endl;

		// Looks like it is one of the good times of capturing joystick pointer in Air Warrior V2.
		if(TOWNS_APPSPECIFIC_AIRWARRIOR_V2==state.appSpecificSetting)
		{
			i486DXCommon::SegmentRegister DS;
			unsigned int exceptionType,exceptionCode;
			cpu.DebugLoadSegmentRegister(DS,0x0014,mem,false);

			uint32_t ptrToStruct=cpu.DebugFetchDword(32,DS,0x13418,mem);

			state.appSpecific_StickPosXPtr=cpu.DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+ptrToStruct+0x18,mem);
			state.appSpecific_StickPosYPtr=cpu.DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+ptrToStruct+0x1A,mem);
			state.appSpecific_ThrottlePtr=cpu.DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x0002A576,mem);
			state.appSpecific_RudderPtr=cpu.DebugLinearAddressToPhysicalAddress(exceptionType,exceptionCode,DS.baseLinearAddr+0x0002A57A,mem);
			std::cout << "  StickX Physical Addr       =" << cpputil::Uitox(state.appSpecific_StickPosXPtr) << std::endl;
			std::cout << "  StickY Physical Addr       =" << cpputil::Uitox(state.appSpecific_StickPosYPtr) << std::endl;
			std::cout << "  Throttle Physical Addr     =" << cpputil::Uitox(state.appSpecific_ThrottlePtr) << std::endl;
			std::cout << "  Rudder Physical Addr       =" << cpputil::Uitox(state.appSpecific_RudderPtr) << std::endl;
		}
		if(TOWNS_APPSPECIFIC_DAIKOUKAIJIDAI2==state.appSpecificSetting)
		{
			Daikoukai2_CaptureFlags();
		}
	}
	else if(1==cpu.GetAH())
	{
		if(TownsEventLog::MODE_RECORDING==eventLog.mode || TownsEventLog::MODE_PLAYBACK==eventLog.mode)
		{
			eventLog.LogMouseEnd(state.townsTime);
		}
		std::cout << "Mouse BIOS stopped." << std::endl;
		state.mouseBIOSActive=false;
	}
}

/* virtual */ void FMTownsCommon::InterceptINT21H(unsigned int AX,const std::string fName)
{
	if(TownsEventLog::MODE_RECORDING==eventLog.mode)
	{
		if(0x3D00==(AX&0xFF00))
		{
			eventLog.LogFileOpen(state.townsTime,fName);
		}
		else if(0x4B00==(AX&0xFF00))
		{
			eventLog.LogFileExec(state.townsTime,fName);
		}
	}
}

void FMTownsCommon::RunFastDevicePollingInternal(void)
{
	timer.TimerPolling(state.townsTime);
	sound.SoundPolling(state.townsTime);
	crtc.ProcessVSYNCIRQ(state.townsTime);
	state.nextFastDevicePollingTime=state.townsTime+FAST_DEVICE_POLLING_INTERVAL;
}

void FMTownsCommon::SetUpVRAMAccess(bool breakOnRead,bool breakOnWrite)
{
	physMem.SetUpVRAMAccess(TownsTypeToCPUType(townsType),breakOnRead,breakOnWrite);
}

bool FMTownsCommon::FASTModeLamp(void) const
{
	return (0==state.mainRAMWait && state.VRAMWait<3);
}

void FMTownsCommon::SetMainRAMSize(long long int size)
{
	uint64_t RAMEnd=0x7FFFFFFF;
	if(TOWNSTYPE_MARTY==townsType)
	{
		size=std::min<uint64_t>(size,TOWNSADDR_MARTY_ROM0_BASE);
		RAMEnd=TOWNSADDR_MARTY_ROM0_BASE-1;
	}
	else if(TOWNSCPU_80386SX==TownsTypeToCPUType(townsType))
	{
		size=std::min<uint64_t>(size,TOWNSADDR_386SX_VRAM0_BASE);
		RAMEnd=TOWNSADDR_386SX_VRAM0_BASE-1;
	}

	physMem.SetMainRAMSize(size);
	mem.RemoveAccess(physMem.state.RAM.size()-1,RAMEnd);
	mem.AddAccess(&physMem.mainRAMAccess,0x00100000,physMem.state.RAM.size()-1);
}

void FMTownsCommon::ForceRender(class TownsRender &render,class Outside_World &world,Outside_World::WindowInterface &windowInterface)
{
	render.Prepare(crtc);
	render.damperWireLine=var.damperWireLine;
	render.BuildImage(physMem.state.VRAM,crtc.GetPalette(),crtc.chaseHQPalette);
	if(true==world.ImageNeedsFlip())
	{
		render.FlipUpsideDown();
	}
	world.UpdateStatusBarInfo(*this);
	windowInterface.Communicate(&world);
	auto img=render.MoveImage();
	windowInterface.UpdateImage(img);
}

void FMTownsCommon::RenderQuiet(class TownsRender &render,bool layer0,bool layer1)
{
	render.Prepare(crtc);
	render.OerrideShowPage(layer0,layer1);

	auto palette=crtc.GetPalette();
	ApplicationSpecificScreenshotOverride(render,palette);

	render.BuildImage(physMem.state.VRAM,palette,crtc.chaseHQPalette);
}

void FMTownsCommon::RenderEntireVRAMLayerQuiet(class TownsRender &render,unsigned int layer)
{
	render.PrepareEntireVRAMLayer(crtc,layer);
	render.BuildImage(physMem.state.VRAM,crtc.GetPalette(),crtc.chaseHQPalette);
}

bool FMTownsCommon::GetEleVolCDLeftEN(void) const
{
	return state.eleVol[TOWNS_ELEVOL_FOR_CD][TOWNS_ELEVOL_CD_LEFT].EN;
}
bool FMTownsCommon::GetEleVolCDRightEN(void) const
{
	return state.eleVol[TOWNS_ELEVOL_FOR_CD][TOWNS_ELEVOL_CD_RIGHT].EN;
}
unsigned int FMTownsCommon::GetEleVolCDLeft(void) const
{
	return state.eleVol[TOWNS_ELEVOL_FOR_CD][TOWNS_ELEVOL_CD_LEFT].vol;
}
unsigned int FMTownsCommon::GetEleVolCDRight(void) const
{
	return state.eleVol[TOWNS_ELEVOL_FOR_CD][TOWNS_ELEVOL_CD_RIGHT].vol;
}
////////////////////////////////////////////////////////////

void FMTownsCommon::EnableDebugger(void)
{
	auto &cpu=CPU();
	cpu.AttachDebugger(&debugger);
	debugger.stop=false;
	cpu.enableCallStack=true;
}
void FMTownsCommon::DisableDebugger(void)
{
	auto &cpu=CPU();
	cpu.DetachDebugger();
	debugger.stop=false;
	cpu.enableCallStack=false;
}

std::vector <std::string> FMTownsCommon::GetStackText(unsigned int numBytes) const
{
	auto &cpu=CPU();
	std::vector <std::string> text;
	for(unsigned int offsetHigh=0; offsetHigh<numBytes; offsetHigh+=16)
	{
		auto addressSize=cpu.GetStackAddressingSize();
		std::string line;
		line="SS+"+cpputil::Uitox(offsetHigh)+":";
		for(unsigned int offsetLow=0; offsetLow<16 && offsetHigh+offsetLow<numBytes; ++offsetLow)
		{
			line+=cpputil::Ubtox(cpu.DebugFetchByte(addressSize,cpu.state.SS(),cpu.state.ESP()+offsetHigh+offsetLow,mem));
			line.push_back(' ');
		}
		text.push_back(line);
	}
	return text;
}
void FMTownsCommon::PrintStack(unsigned int numBytes) const
{
	auto &cpu=CPU();
	for(auto s : GetStackText(numBytes))
	{
		std::cout << s << std::endl;
		if(nullptr!=cpu.debuggerPtr)
		{
			cpu.debuggerPtr->WriteLogFile(s);
		}
	}
}
void FMTownsCommon::PrintDisassembly(void) const
{
	auto &cpu=CPU();
	i486DXCommon::InstructionAndOperand instOp;
	MemoryAccess::ConstMemoryWindow emptyMemWin;
	cpu.DebugFetchInstruction(emptyMemWin,instOp,mem);
	auto disasm=cpu.Disassemble(instOp.inst,instOp.op1,instOp.op2,cpu.state.CS(),cpu.state.EIP,mem,debugger.GetSymTable(),debugger.GetIOTable());
	std::cout << disasm << std::endl;
	if(nullptr!=cpu.debuggerPtr)
	{
		cpu.debuggerPtr->WriteLogFile(disasm);
	}
}

std::vector <std::string> FMTownsCommon::GetRealModeIntVectorsText(void) const
{
	std::vector <std::string> text;
	for(int i=0; i<256; i+=4)
	{
		std::string str;
		for(int j=0; j<4; ++j)
		{
			if(0<j)
			{
				str+=" | ";
			}
			auto ij=i+j;
			str+=cpputil::Ubtox(ij)+" ";
			unsigned int offset=physMem.state.RAM[ij*4]|((unsigned int)physMem.state.RAM[ij*4+1]<<8);
			unsigned int seg=physMem.state.RAM[ij*4+2]|((unsigned int)physMem.state.RAM[ij*4+3]<<8);
			str+=cpputil::Ustox(seg)+":"+cpputil::Ustox(offset);
		}
		text.push_back(str);
	}
	return text;
}
void FMTownsCommon::DumpRealModeIntVectors(void) const
{
	auto &cpu=CPU();
	for(auto s : GetRealModeIntVectorsText())
	{
		std::cout << s << std::endl;
		if(nullptr!=cpu.debuggerPtr)
		{
			cpu.debuggerPtr->WriteLogFile(s);
		}
	}
}
std::vector <std::string> FMTownsCommon::GetCallStackText(void) const
{
	auto &cpu=CPU();
	return debugger.GetCallStackText(cpu);
}
void FMTownsCommon::PrintCallStack(void) const
{
	auto &cpu=CPU();
	for(auto str : GetCallStackText())
	{
		std::cout << str << std::endl;
		if(nullptr!=cpu.debuggerPtr)
		{
			cpu.debuggerPtr->WriteLogFile(str);
		}
	}
}
void FMTownsCommon::PrintPIC(void) const
{
	auto &cpu=CPU();
	for(auto str : pic.GetStateText())
	{
		std::cout << str << std::endl;
		if(nullptr!=cpu.debuggerPtr)
		{
			cpu.debuggerPtr->WriteLogFile(str);
		}
	}
}

void FMTownsCommon::PrintDMAC(void) const
{
	auto &cpu=CPU();
	for(auto str : dmac.GetStateText())
	{
		std::cout << str << std::endl;
		if(nullptr!=cpu.debuggerPtr)
		{
			cpu.debuggerPtr->WriteLogFile(str);
		}
	}
}

void FMTownsCommon::PrintFDC(void) const
{
	auto &cpu=CPU();
	for(auto str : fdc.GetStatusText())
	{
		std::cout << str << std::endl;
		if(nullptr!=cpu.debuggerPtr)
		{
			cpu.debuggerPtr->WriteLogFile(str);
		}
	}
}

void FMTownsCommon::PrintTimer(void) const
{
	auto &cpu=CPU();
	for(auto str : timer.GetStatusText())
	{
		std::cout << str << std::endl;
		if(nullptr!=cpu.debuggerPtr)
		{
			cpu.debuggerPtr->WriteLogFile(str);
		}
	}
}

void FMTownsCommon::PrintSound(void) const
{
	auto &cpu=CPU();
	for(auto str : sound.GetStatusText())
	{
		std::cout << str << std::endl;
		if(nullptr!=cpu.debuggerPtr)
		{
			cpu.debuggerPtr->WriteLogFile(str);
		}
	}
}

void FMTownsCommon::PrintStatus(void) const
{
	if(true==VMBase::CheckAbort())
	{
		std::cout << "VM Aborted!" << std::endl;
		std::cout << "Device:" << vmAbortDeviceName << std::endl;
		std::cout << "Reason:" << vmAbortReason << std::endl;
	}

	auto &cpu=CPU();
	if(nullptr!=cpu.debuggerPtr)
	{
		std::cout << "Debugger Enabled." << std::endl;
	}
	std::cout << "Towns TIME (Nano-Seconds): " << state.townsTime << std::endl;
	cpu.PrintState();
	PrintStack(32);
	if(""!=debugger.externalBreakReason)
	{
		std::cout << debugger.externalBreakReason << std::endl;
		if(nullptr!=cpu.debuggerPtr)
		{
			cpu.debuggerPtr->WriteLogFile(debugger.externalBreakReason);
		}
	}
	if(""!=debugger.additionalDisasm)
	{
		std::cout << debugger.additionalDisasm << std::endl;
		if(nullptr!=cpu.debuggerPtr)
		{
			cpu.debuggerPtr->WriteLogFile(debugger.additionalDisasm);
		}
	}
	PrintDisassembly();
}

std::vector <std::string> FMTownsCommon::GetMouseStatusText(void) const
{
	std::vector <std::string> text;

	text.push_back("");
	text.back()+="Last Known Raw Coord (1X scale): (";
	text.back()+=cpputil::Itoa(var.lastKnownMouseX);
	text.back()+=",";
	text.back()+=cpputil::Itoa(var.lastKnownMouseY);
	text.back()+=")";

	int integX,integY;
	if(true==GetMouseCoordinate(integX,integY,state.tbiosVersion))
	{
		text.push_back("Mouse Integration Coord:(");
		text.back()+=cpputil::Itoa(integX);
		text.back()+=",";
		text.back()+=cpputil::Itoa(integY);
		text.back()+=")";
	}
	else
	{
		text.push_back("Mouse Integration Unavailable");
	}

	unsigned int VRAMSize=(crtc.InSinglePageMode() ? 0x80000 : 0x40000);

	for(unsigned int page=0; page<(crtc.InSinglePageMode() ? 1 : 2); ++page)
	{
		text.push_back("");
		text.back()+="On Page "+cpputil::Ubtox(page);

		auto zoom2x=crtc.GetPageZoom2X(page);
		auto topLeft1X=crtc.GetPageOriginOnMonitor(page);

		auto mx=var.lastKnownMouseX-topLeft1X.x();
		auto my=var.lastKnownMouseY-topLeft1X.y();
		if(0<zoom2x.x())
		{
			mx=mx*2/zoom2x.x();
		}
		if(0<zoom2x.y())
		{
			my=my*2/zoom2x.y();
		}

		auto VRAMoffset=crtc.GetPageVRAMAddressOffset(page);
		auto bytesPerLine=crtc.GetPageBytesPerLine(page);
		if(0!=bytesPerLine)
		{
			unsigned int VRAMHeight=VRAMSize/bytesPerLine;
			my+=VRAMoffset/bytesPerLine;
			my%=VRAMHeight;
		}

		text.push_back("");
		text.back()+="(";
		text.back()+=cpputil::Itoa(mx);
		text.back()+=",";
		text.back()+=cpputil::Itoa(my);
		text.back()+=")  ";

		unsigned int VRAMAddr=0x40000*page;
		VRAMAddr+=bytesPerLine*my;
		VRAMAddr+=crtc.GetPageBitsPerPixel(page)*mx/8;
		text.back()+=cpputil::Uitox(VRAMAddr);
		text.back()+="H";
	}

	for(unsigned int p=0; p<2; ++p)
	{
		if(TownsGamePort::MOUSE==gameport.state.ports[p].device)
		{
			text.push_back("PORT:");
			text.back()+=cpputil::Itoa(p);
			text.back()+=("  Motion:");
			text.back()+=cpputil::Itoa(gameport.state.ports[p].mouseMotion.x());
			text.back()+=(",");
			text.back()+=cpputil::Itoa(gameport.state.ports[p].mouseMotion.y());
			text.back()+=("  Read State:");
			text.back()+=cpputil::Itoa(gameport.state.ports[p].state);
		}
	}

	return text;
}

/* static */ void FMTownsCommon::MakeINTInfo(class i486SymbolTable &symTable)
{
	symTable.AddINTLabel(0x40,"Timer");
	symTable.AddINTLabel(0x41,"Keyboard");
	symTable.AddINTLabel(0x42,"RS232C");
	symTable.AddINTLabel(0x43,"ExpRS232C");
	symTable.AddINTLabel(0x44,"IO Board");
	symTable.AddINTLabel(0x45,"IO Board");
	symTable.AddINTLabel(0x46,"FloppyDisk");
	symTable.AddINTLabel(0x47,"(PIC Bridge)");
	symTable.AddINTLabel(0x48,"SCSI");
	symTable.AddINTLabel(0x49,"CD-ROM");
	symTable.AddINTLabel(0x4A,"IO Board");
	symTable.AddINTLabel(0x4B,"VSYNC");
	symTable.AddINTLabel(0x4C,"Printer");
	symTable.AddINTLabel(0x4D,"FM/PCM");
	symTable.AddINTLabel(0x4E,"IO Board");
	symTable.AddINTLabel(0x4F,"Unused");


	symTable.AddINTLabel(0x93,"Disk");
	symTable.AddINTFuncLabel(0x93,0x00,"Set Mode");
	symTable.AddINTFuncLabel(0x93,0x01,"Get Mode");
	symTable.AddINTFuncLabel(0x93,0x02,"Get Status");
	symTable.AddINTFuncLabel(0x93,0x03,"Restore");
	symTable.AddINTFuncLabel(0x93,0x04,"Seek HSG");
	symTable.AddINTFuncLabel(0x93,0x05,"Read HSG");
	symTable.AddINTFuncLabel(0x93,0x14,"Seek MSF");
	symTable.AddINTFuncLabel(0x93,0x15,"Read MSF");
	symTable.AddINTFuncLabel(0x93,0x50,"CDDA Play");
	symTable.AddINTFuncLabel(0x93,0x51,"Get Play Info");
	symTable.AddINTFuncLabel(0x93,0x52,"CDDA Stop");
	symTable.AddINTFuncLabel(0x93,0x53,"Get Play State");
	symTable.AddINTFuncLabel(0x93,0x54,"Read TOC");
	symTable.AddINTFuncLabel(0x93,0x55,"CDDA Pause");
	symTable.AddINTFuncLabel(0x93,0x56,"CDDA Resume");

	symTable.AddINTLabel(0x90,"Keyboard");
	symTable.AddINTFuncLabel(0x90,0x00,"Init");
	symTable.AddINTFuncLabel(0x90,0x01,"Config Buffer");
	symTable.AddINTFuncLabel(0x90,0x02,"Set Code System");
	symTable.AddINTFuncLabel(0x90,0x03,"Get Code System");
	symTable.AddINTFuncLabel(0x90,0x04,"Set Lock");
	symTable.AddINTFuncLabel(0x90,0x05,"Set Click Sound");
	symTable.AddINTFuncLabel(0x90,0x06,"Clear Buffer");
	symTable.AddINTFuncLabel(0x90,0x07,"Check Input");
	symTable.AddINTFuncLabel(0x90,0x08,"Get Shift-Key State");
	symTable.AddINTFuncLabel(0x90,0x09,"Inkey");
	symTable.AddINTFuncLabel(0x90,0x0A,"Input Matrix");
	symTable.AddINTFuncLabel(0x90,0x0B,"Push Key Code");
	symTable.AddINTFuncLabel(0x90,0x0C,"Set PF Key INT");
	symTable.AddINTFuncLabel(0x90,0x0D,"Get PF Key INT");
	symTable.AddINTFuncLabel(0x90,0x0E,"Assign Key");
	symTable.AddINTFuncLabel(0x90,0x0F,"Get Key Assignment");

	symTable.AddINTLabel(0x94,"Printer");
	symTable.AddINTLabel(0x96,"Calendar");
	symTable.AddINTLabel(0x97,"Timer");
	symTable.AddINTLabel(0x98,"Clock");
	symTable.AddINTLabel(0x9B,"RS232C");
	symTable.AddINTLabel(0x9E,"Beep");

	symTable.AddINTLabel(0xAE,"INT-Manager");
	symTable.AddINTFuncLabel(0xAE,0x00,"Set INT Data Block Addr");
	symTable.AddINTFuncLabel(0xAE,0x01,"Get INT Data Block Addr");
	symTable.AddINTFuncLabel(0xAE,0x02,"Set INT Mask");
	symTable.AddINTFuncLabel(0xAE,0x03,"Get INT Mask");
	symTable.AddINTFuncLabel(0xAE,0x04,"Get INT Data Block Table");

	symTable.AddINTLabel(0xAF,"SysService");
	symTable.AddINTFuncLabel(0xAF,0x00,"JIS to SJIS");
	symTable.AddINTFuncLabel(0xAF,0x01,"SJIS to JIS");
	symTable.AddINTFuncLabel(0xAF,0x02,"Get CPU Type");
	symTable.AddINTFuncLabel(0xAF,0x03,"JIS to SJIS 2");
	symTable.AddINTFuncLabel(0xAF,0x04,"SJIS to JIS 2");
	symTable.AddINTFuncLabel(0xAF,0x05,"Get Machine Info");

	symTable.AddINTLabel(0xB0,"?Used in BIOS");

	symTable.AddINTLabel(0x8E,"Exp-SysService");
	symTable.AddINTFuncLabel(0x8E,0x00,"Get System Info");
	symTable.AddINTFuncLabel(0x8E,0x01,"Printer Feeder Control");
	symTable.AddINTFuncLabel(0x8E,0x02,"? Used in OAK1.SYS Init");
	symTable.AddINTFuncLabel(0x8E,0x20,"(Prob)Get Available Protected-Mode Mem in KB");
	symTable.AddINTFuncLabel(0x8E,0x21,"(Prob)Alloc Protected-Mode Mem in KB");
	// DOS Extender for Towns uses 0x8E,0x20 and 0x21 for allocating space for the Page Table
	// 4A2A:00001CFF 9C                        PUSHF
	// 4A2A:00001D00 FA                        CLI
	// 4A2A:00001D01 B420                      MOV     AH,20H
	// 4A2A:00001D03 CD8E                      INT     8EH
	// 4A2A:00001D05 83F907                    CMP     CX,0007H    CX<=0040:0061 (Prob)Available 1K blocks
	// 4A2A:00001D08 0F861800                  JBE     00001D24
	// 4A2A:00001D0C B421                      MOV     AH,21H      CX=(Prob)Number of 1K blocks
	// 4A2A:00001D0E CD8E                      INT     8EH
	// 4A2A:00001D10 890EB31B                  MOV     [1BB3H],CX
	// 4A2A:00001D14 893EB51B                  MOV     [1BB5H],DI
	// 4A2A:00001D18 8916B71B                  MOV     [1BB7H],DX
	// 4A2A:00001D1C 0AE4                      OR      AH,AH
	// 4A2A:00001D1E 0F850200                  JNE     00001D24
	// 4A2A:00001D22 9D                        POPF
	// 4A2A:00001D23 C3                        RET

	// Information based on https://github.com/nabe-abk/free386/blob/master/doc-ja/dosext/coco_nsd.txt >>
	symTable.AddINTFuncLabel(0x8E,0xC000,"COCO Install Check");
	symTable.AddINTFuncLabel(0x8E,0xC002,"COCO?");
	symTable.AddINTFuncLabel(0x8E,0xC003,"COCO Get Installed Drivers");
	symTable.AddINTFuncLabel(0x8E,0xC005,"COCO Get ? in DS:DI");
	symTable.AddINTFuncLabel(0x8E,0xC103,"COCO Get Installed Driver Info");
	symTable.AddINTFuncLabel(0x8E,0xC104,"COCO Get NSD Driver Info");
	symTable.AddINTFuncLabel(0x8E,0xC10C,"COCO?");
	// Information based on https://github.com/nabe-abk/free386/blob/master/doc-ja/dosext/coco_nsd.txt <<


	symTable.AddINTLabel(0xFD,"Wait CX*10us");
}

////////////////////////////////////////////////////////////

FMTownsCommon::MemoryEvaluation::MemoryEvaluation(FMTownsCommon *townsPtr)
{
	this->townsPtr=townsPtr;
}

bool FMTownsCommon::MemoryEvaluation::Decode(std::string str)
{
	ready=false;
	errorMessage="";
	cpputil::Capitalize(str);
	if(true==Analyze(str))
	{
		Evaluate(); // Dummy-evaluate.
		if(true!=error)
		{
			ready=true;
		}
	}
	if(""==errorMessage)
	{
		errorMessage="There was error(s) in the expression.";
	}
	return ready;
}

std::string FMTownsCommon::MemoryEvaluation::MatchCustomKeyword(std::string str) const
{
	unsigned int skip=0;
	if("BYTE:"==str.substr(0,5))
	{
		return "BYTE:";
	}
	else if("WORD:"==str.substr(0,5))
	{
		return "WORD:";
	}
	else if("DWORD:"==str.substr(0,6))
	{
		return "DWORD:";
	}
	return "";
}
bool FMTownsCommon::MemoryEvaluation::IsCustomUnaryOperator(std::string str) const
{
	return ("BYTE:"==str.substr(0,5) ||
	        "WORD:"==str.substr(0,5) ||
	        "DWORD:"==str.substr(0,6));
}
long long int FMTownsCommon::MemoryEvaluation::EvaluateCustomUnaryOperator(const Term *t,long long int operand) const
{
	// All capitalized in Decode.
	if("BYTE:"==t->label.substr(0,5))
	{
		return EvaluateMemoryReference(operand,1);
	}
	else if("WORD:"==t->label.substr(0,5))
	{
		return EvaluateMemoryReference(operand,2);
	}
	else if("DWORD:"==t->label.substr(0,6))
	{
		return EvaluateMemoryReference(operand,4);
	}
	return 0;
}

long long int FMTownsCommon::MemoryEvaluation::EvaluateRawNumber(const std::string &str) const
{
	return cpputil::Atoi(str.c_str());
}

unsigned int FMTownsCommon::MemoryEvaluation::EvaluateMemoryReference(unsigned int addr,unsigned int nBytes) const
{
	int data=0;
	for(int i=0; i<nBytes; ++i)
	{
		data|=(townsPtr->mem.FetchByte(addr+i)<<(i*8));
	}
	return data;
}

void FMTownsCommon::EnableAutoQSS(unsigned int autoQSSThresholdX,unsigned int autoQSSThresholdY)
{
	autoQSS=true;
	GetMapXY(lastAutoQSSXY[0],lastAutoQSSXY[1]);
	autoQSSThresholdXY[0]=autoQSSThresholdX;
	autoQSSThresholdXY[1]=autoQSSThresholdY;
}
void FMTownsCommon::DisableAutoQSS(void)
{
	autoQSS=false;
}
bool FMTownsCommon::CheckAutoQSS(int &x,int &y)
{
	if(true==autoQSS && lastAutoQSSCheckTime+AUTOQSS_CHECK_INTERVAL<state.townsTime)
	{
		lastAutoQSSCheckTime=state.townsTime;
		if(TOWNS_APPSPECIFIC_DAIKOUKAIJIDAI2==state.appSpecificSetting) // Prevent mouse-cursor contamination.
		{
			int mx,my;
			if(true!=GetMouseCoordinate(mx,my,state.tbiosVersion) || mx<500)
			{
				return false;
			}
		}

		if(true==GetMapXY(x,y))
		{
			if(std::abs(x-lastAutoQSSXY[0])>=autoQSSThresholdXY[0] ||
			   std::abs(y-lastAutoQSSXY[1])>=autoQSSThresholdXY[1])
			{
				lastAutoQSSXY[0]=x;
				lastAutoQSSXY[1]=y;
				return true;
			}
		}
	}
	return false;
}

bool FMTownsCommon::GetMapXY(int &x,int &y) const
{
	switch(state.appSpecificSetting)
	{
	case TOWNS_APPSPECIFIC_DAIKOUKAIJIDAI2:
		if(true==Daikoukaijidai2_MapXY(x,y))
		{
			return true;
		}
		break;
	}
	if(true==mapXY[0].ready || true==mapXY[1].ready)
	{
		x=(true==mapXY[0].ready ? mapXY[0].Evaluate() : 0);
		y=(true==mapXY[1].ready ? mapXY[1].Evaluate() : 0);
		return true;
	}
	return false;
}

bool FMTownsCommon::GetApplicationSpecificMapXY(int &x,int &y) const
{
	switch(state.appSpecificSetting)
	{
	case TOWNS_APPSPECIFIC_DAIKOUKAIJIDAI2:
		return Daikoukaijidai2_MapXY(x,y);
	}
	return false;
}

void FMTownsCommon::ApplicationSpecificScreenshotOverride(class TownsRender &render,TownsCRTC::AnalogPalette &palette) const
{
	switch(state.appSpecificSetting)
	{
	case TOWNS_APPSPECIFIC_DAIKOUKAIJIDAI2:
		return Daikoukaijidai2_ScreenshotOverride(render,palette);
	}
}
