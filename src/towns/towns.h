/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef TOWNS_IS_INCLUDED
#define TOWNS_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "lineparser.h"

#include "vmbase.h"
#include "device.h"
#include "i486.h"
#include "i486debug.h"
#include "inout.h"
#include "ramrom.h"

// FM Towns specific
#include "pic.h"
#include "dmac.h"
#include "cdrom.h"
#include "crtc.h"
#include "sprite.h"
#include "fdc.h"
#include "scsi.h"
#include "rtc.h"
#include "keyboard.h"
#include "physmem.h"
#include "memaccess.h"
#include "sound.h"
#include "gameport.h"
#include "timer.h"
#include "serialport.h"
#include "vndrv.h"
#include "tgdrv.h"
#include "townsparam.h"

#include "eventlog.h"



// Adding a device:
// (1) Make a class
// (2) Implement State, IO functions, Reset function.
// (3) Add as data member in FMTownsCommon class.
// (4) In FMTownsCommon::FMTownsCommon() add to allDevices.
// (5) In FMTownsCommon::FMTownsCommon() add to io.
class FMTownsCommon : public VMBase, public Device, public i486DXCommon::FMTownsMouseBiosInterceptor, public i486DXCommon::INT21HInterceptor
{
public:
	// I'm talking about 66MHz to 120MHz ball park.
	// 1 clock is 1/66 to 1/120us.
	// Make it nano-seconds?
	// 1 micro second is 1/1M second.
	// 1 nano second is 1/1G second.

	enum
	{
		FREQUENCY_DEFAULT=25,                // MHz
		FREQUENCY_SLOWMODE_DEFAULT=5,        // MHz
		FAST_DEVICE_POLLING_INTERVAL=10000,  // Nano-seconds
		DEVICE_POLLING_INTERVAL=   8000000,  // 8ms

		AUTOQSS_CHECK_INTERVAL=   25000000,  // 25ms

		RESET_REASON_SOFTWARE=1,
		RESET_REASON_CPU=2,
	};

	virtual const char *DeviceName(void) const{return "FMTOWNS";}

	static const unsigned char defCMOS[];

	class State
	{
	public:
		/*! Time passed since power on in nano seconds.
		    Initially I thought to make it micro seconds, but may be too coarse.
		    And, a 32-bit integer is good for 4,000,000,000 nano seconds = 4 seconds.
		    A 64-bit integer should be able to hold decades.
		    I think 64-bit is long enough.  So, I make it signed int.
		*/
		long long int townsTime;

		int64_t nextDevicePollingTime=0;

		/*! Nanoseconds VM is lagging behind the real time.
		*/
		long long int timeDeficit=0;

		enum
		{
			CATCHUP_DEFICIT_CUTOFF=2000000, // If the deficit is above this threshold, just don't increase.
		};



		/*! If noWait is false and if townsTime goes ahead of real time, VM will wait until real time catches up with the townsTime.
		    By default noWait is true until the first disk read, when var.noWaitStandBy is copied to noWait.
		    Default value of noWaitStandBy is false, in which case noWait will turn to false as soon as the first sector is read.
		*/
		bool noWait=true;


		/*! If this flag is true, VM will report the CPU as 80386DX from I/O port 0030H.
		    DOS Extender that comes with Towns OS V2.1L10 (Not V2.1L10B) blocks the execution
		    unless the CPU is 80386DX or 80386SX.

		    This flag needs to be set true to boot a software title based on V2.1L10.

		    There is no such issue with V2.1L10B, and that's probably why we see few
		    software titles that is running on V2.1L10.
		*/
		bool pretend386DX=false;

		/*! 
		*/
		unsigned long long int nextRenderingTime=0;

		/*! 
		*/
		long long int nextFastDevicePollingTime;

		/*!
		*/
		long long int nextSecondInTownsTime=0;

		/*! Number of clocks times 1000 since last update of townsTime.
		    After running one instruction, townsTime may not be exactly the same
		    as the real time (or whatever requested time.)
		    This variable remembers how many clocks townsTime is ahead of the real time.
		*/
		long long int clockBalance;

		/*! Clock frequency in MHz.  Default is FREQUENCY_DEFAULT.
		*/
		long long int currentFreq;
		long long int fastModeFreq;


		/*!
		*/
		int mainRAMWait=0,VRAMWait=0;


		/*! Reset reason.
		*/
		unsigned int resetReason;


		/*! Serial-ROM access
		*/
		unsigned int serialROMBitCount;
		unsigned int lastSerialROMCommand;


		/*! Electric Volume
		*/
		class ElectricVolume
		{
		public:
			bool EN=true,C32=false,C0=false;
			unsigned int vol=31;
		};
		ElectricVolume eleVol[2][4];
		unsigned int eleVolChLatch[2]={0,0};


		/*! TBIOS version, TBIOS physical address (it should be contiguous in physical memory),
		    Mouse work area pointer.  Captured upon MOS_start and MOS_end.
		*/
		unsigned int tbiosVersion=TBIOS_UNKNOWN;
		bool mouseBIOSActive=false;
		int mouseDisplayPage=0;
		unsigned int TBIOS_physicalAddr=0;
		unsigned int TBIOS_mouseInfoOffset=0;
		unsigned int MOS_work_linearAddr=0;
		unsigned int MOS_work_physicalAddr=0;
		unsigned int MOS_pulsePerPixelH=8,MOS_pulsePerPixelV=8;
		int mouseIntegrationSpeed=256;

		unsigned short DOSSEG=TOWNS_DOS_SEG; // for V2.1 L20 IO.SYS
		unsigned short DOSVER=0;
		unsigned short DOSLOLOFF=0;
		unsigned short DOSLOLSEG=0;

		/*! Application-Specific Customization parameters.
		*/
		unsigned int appSpecificSetting=0;
		unsigned int appSpecific_MousePtrX=0;
		unsigned int appSpecific_MousePtrY=0;
		uint32_t appSpecific_StickPosXPtr=0;
		uint32_t appSpecific_StickPosYPtr=0;
		uint32_t appSpecific_ThrottlePtr=0;
		uint32_t appSpecific_RudderPtr=0;
		unsigned int appSpecific_WC2_EventQueueBaseAddr=0;  // DS:03CCH        // Needs to be state-saved
		uint32_t appSpecific_WC_setSpeedPtr=0;
		uint32_t appSpecific_WC_maxSpeedPtr=0;
		bool appSpecific_HoldMouseIntegration=false;
		uint32_t appSpecific_Dunmas_SpellPower=0; // Won't be saved in state.

		uint32_t appSpecific_Daikoukai_YNDialogXAddr=0;          // Needs to be state-saved
		uint32_t appSpecific_Daikoukai_YNDialogYAddr=0;          // Needs to be state-saved
		uint32_t appSpecific_Daikoukai_DentakuDialogXAddr=0;          // Needs to be state-saved
		uint32_t appSpecific_Daikoukai_DentakuDialogYAddr=0;          // Needs to be state-saved

		uint32_t appSpecific_Daikoukai2_MapX=0; // Won't be saved in state.
		uint32_t appSpecific_Daikoukai2_MapY=0; // Won't be saved in state.
		uint32_t appSpecific_Daikoukai2_p_flag=0; // Won't be saved in state.
		uint32_t appSpecific_Daikoukai2_GetaYAddr=0; // Won't be saved in state.
		uint32_t appSpecific_Daikoukai2_DentakuXAddr=0; // Won't be saved in state.
		uint32_t appSpecific_Daikoukai2_DentakuYAddr=0; // Won't be saved in state.
		uint32_t appSpecific_Daikoukai2_YesNoXAddr=0; // Won't be saved in state.
		uint32_t appSpecific_Daikoukai2_YesNoYAddr=0; // Won't be saved in state.

		void PowerOn(void);
		void Reset(void);
	};

	class VMHostFileTransfer
	{
	public:
		class File
		{
		public:
			bool started=false;
			unsigned int offset=0;
			std::string hostFName,vmFName;
			std::vector <unsigned char> bin; // Loaded when transfer starts.
		};
		std::vector <File> toSend;
		std::vector <File> toRecv;

		void AddHostToVM(std::string hostFName,std::string vmFName);
		void AddVMToHost(std::string vmFName,std::string hostFName);
	};

	class Variable
	{
	public:
		unsigned int freeRunTimerShift;

		uint64_t nextTimeSync; // Used from TownsThread.

		bool powerOff=false;
		int returnCode=0;

		bool debugBreakOnLButtonUp=false;
		bool debugBreakOnLButtonDown=false;

		/*! If this flag is true, fake damper-wire line is rendered.
		*/
		bool damperWireLine=false;

		bool scanLineEffectIn15KHz=false;

		// Report from TGMOUSE.EXE running in Windows 3.1
		mutable bool mousePositionReported=false;
		int mouseXReported=0,mouseYReported=0;

		/*! Enable/disable mouse integration.
		    To disable mouse integration, set this flag false, and call DontControlMouse().
		    To enable, set this flag true, then VM will respect the next ControlMouse and SetMouseButtonState functions and on.
		*/
		bool mouseIntegration=true;

		int mouseMinX=TownsStartParameters::DEFAULT_MOUSE_MINX;
		int mouseMaxX=TownsStartParameters::DEFAULT_MOUSE_MAXX;
		int mouseMinY=TownsStartParameters::DEFAULT_MOUSE_MINY;
		int mouseMaxY=TownsStartParameters::DEFAULT_MOUSE_MAXY;

		/*! Flag to consider VRAM offset in mouse integration.  Default value is true.
		*/
		bool considerVRAMOffsetInMouseIntegration=true;

		/*! noWaitStandby will be copied to state.noWait on NotifyDiskRead().
		*/
		bool noWaitStandby=false;

		/*! Back up of frequency.  Super Daisenryaku for FM Towns is, I believe, is the best among ports.
		    However, especially at higher-than-16MHz clock frequency, the map scrolls too fast.
		    To make it more playable, an application-specific option is added to reduce frequency to 2MHz
		    while mouse button is held down.  To recover the frequency when the mouse button is up,
		    the original frequency is saved in frequencyBackup.
		*/
		unsigned int frequencyBackup=0;

		unsigned int slowModeFreq=FREQUENCY_SLOWMODE_DEFAULT;

		/*! VM State loaded at start-up.
		    Loaded in TownsThread::VMStart
		*/
		std::string startUpStateFName;

		/*!
		*/
		std::string quickScrnShotDir;

		// scrnShotWid==0 or scrnShotHei==0 mean no cropping.
		unsigned int scrnShotX0=0,scrnShotY0=0,scrnShotWid=0,scrnShotHei=0;

		/*!
		*/
		std::string quickStateSaveFName;


		enum
		{
			TIME_ADJUSTMENT_LOG_LEN=4096
		};
		unsigned int timeAdjustLogPtr=0;
		int64_t timeAdjustLog[TIME_ADJUSTMENT_LOG_LEN];
		int64_t timeDeficitLog[TIME_ADJUSTMENT_LOG_LEN];

		enum
		{
			VM2HOST_PARAM_QUEUE_LENGTH=256,
		};
		unsigned int nVM2HostParam;
		unsigned char VM2HostParam[VM2HOST_PARAM_QUEUE_LENGTH];

		VMHostFileTransfer ftfr;

		std::string CMOSFName;

		/*! If this flag is true, VM thread pauses, not closes on power off.
		*/
		bool pauseOnPowerOff=false;


		/*! Use exit(0) function on power off.
		*/
		bool forceQuitOnPowerOff=false;


		/*! If this flag is true, and if VM lags behind the real time, townsTime will be fast-forwarded to 
		    catch up with the real time.  It will force interrupts fired up roughly real time.  However,
		    timing-sensitive application may break because all of a sudden multiple devices becomes active
		    and raise IRR simultaneously.  Like ChaseHQ will get flicker if this flag is true.

		    Setting this flag false can prevent timing break down.  However, interrupts will lag, and 
		    the execution may become slower.
		*/
		bool catchUpRealTime=true;


		/*! When CS:EIP is powerOffAt, the VM immediately quits with exit(0);
		    The instruction pointer must be a break point, and the debugger must be enabled for this feature.
		    For unit testing.
		*/
		i486DXCommon::FarPointer powerOffAt;


		int lastKnownMouseX=0,lastKnownMouseY=0;


		/*! I think this flag is a dirty way of dealing with the state load, but TownsThread needs to know
		    the machine state has just been loaded.
		*/
		bool justLoadedState=false;



		/*!
		*/
		std::unordered_map <std::string,std::string> fileNameAlias;


		/*!
		*/
		std::vector <uint8_t> vmToHost;



		i486DXCommon::FarPointer disassemblePointer;
		Variable();
		void Reset(void);
	};

	// Machine State >>
	State state;
	virtual i486DXCommon &CPU(void)=0;
	virtual const i486DXCommon &CPU(void) const=0;
	i486Debugger debugger;
	TownsEventLog eventLog;
	TownsPIC pic;
	TownsRTC rtc;
	TownsDMAC dmac;
	TownsCDROM cdrom;
	TownsPhysicalMemory physMem;
	TownsCRTC crtc;
	TownsSprite sprite;
	TownsFDC fdc;
	TownsSCSI scsi;
	TownsKeyboard keyboard;
	TownsSound sound;
	TownsGamePort gameport;
	TownsTimer timer;
	TownsSerialPort serialport;
	TownsVnDrv vndrv;
	TownsTgDrv tgdrv;
	// Machine State <<

	unsigned int townsType;
	Variable var;
	InOut io;
	Memory mem;

	/*! Pointers of all devices (except *this) must be stored in allDevices.
	*/
	using VMBase::allDevices;



	// Conceptual execution model
	//	void RunOneInstruction()
	//	{
	//		clocksPassed+=cpuPtr->RunOneInstruction();
	//		townsTime+=(clocksPassed/cpuPtr->ClocksPerNanoSec());
	//		clocksPassed=(clocksPassed%cpuPtr->ClocksPerNanoSec());
	//		for(auto devPtr : allDevPtr)
	//		{
	//			devPtr->RunToWallClock(townsTime);
	//		}
	//	}
	//	long long int RunToClock(long long int nanoSecPassed)
	//	{
	//		auto stopTime=townsTime+nanoSecPassed)
	//		while(townsTime<stopTime)
	//		{
	//			RunOneInstruction();
	//		}
	//		auto balance=townsTime-stopTime;
	//		return balance;
	//	}


	bool baseClassReady=false;
	// Will be set to true at the end of the constructor.
	// Will be removed when the class is stable.
	FMTownsCommon();


	virtual void Abort(std::string devName,std::string abortReason) override
	{
		VMBase::Abort(devName,abortReason);
		var.nextTimeSync=0;
	}


	static bool Setup(FMTownsCommon &towns,Outside_World *outside_world,const TownsStartParameters &argv);


	unsigned int MachineID(void) const;

	int GetCPUType(void) const;

	static int TownsTypeToCPUType(unsigned int townsType);


	/*! Returns four words in the TBIOS header located at:
	        Physical Address:00100000
	        Physical Address:00100008
	        Physical Address:00100010
	        Physical Address:00100018
	    (In tbiosid.cpp)
	*/
	void GetTBIOSIdentifierStrings(std::string s[4],unsigned int biosPhysicalBaseAddr) const;

	/*! Identifies TBIOS version.  (In tbiosid.cpp)
	*/
	unsigned int IdentifyTBIOS(unsigned int biosPhysicalBaseAddr) const;


	unsigned int FindTBIOSMouseInfoOffset(unsigned int tbiosVersion,unsigned int biosPhysicalBaseAddr) const;


	/*! Returns a string that corresponds to the TBIOS version for debugging.  (In tbiosid.cpp)
	*/
	const char *TBIOSIDENTtoString(unsigned int tbios) const;

	/*! Called back from CRTC when HST register is written.
	    Probably it happens only once per application, and hopefully this function call
	    will not stress the VM.
	    Can be used for identifying an application.
	*/
	void OnCRTC_HST_Write(void);


	/*! Called back from CDROM when CDDA starts playing.
	    Can be used as a cue for identifying an application.
	    Will be called regardless of success or failure to start playing.
	*/
	void OnCDDAStart(void);


	/*! Adjust mouse return based on the mouse coordinate that Towns is thinking and mouse coordinate of the host.
	    Returns true if it thinks it can control the mouse coordinate.  false otherwise.
	    This function is ignored if var.mouseIntegration=false;
	    (In tbiosid.cpp)
	    If parameters diffX and diffY are given, it returns the difference between 
	    current mouse coordinate in the VM and the host mouse coordinate.
	*/
	bool ControlMouse(int hostMouseX,int houstMouseY,unsigned int tbiosid);
	bool ControlMouse(int &diffX,int &diffY,int hostMouseX,int houstMouseY,unsigned int tbiosid);

	/*! Control mouse return.  The difference from ControlMouse is the input x and y are in the Towns's
	    mouse coordinate.
	*/
	bool ControlMouseInVMCoord(int goalMouseX,int goalMouseY,unsigned int tbiosid);

	bool ControlMouseByDiff(int diffX,int diffY,unsigned int tbiosid,int slowDownRange=0);


	/*! Get Wing Commander set-speed and max-speed.
	    Only available when App-Specific augmentation is turned on.
	*/
	void GetWingCommanderSetSpeedMaxSpeed(unsigned int &setSpeed,unsigned int &maxSpeed);


	/*! Make mouse motion reported to the VM (0,0).
	    Once called, effective until next ControlMouse and var.mouseIntegration==true.
	*/
	void DontControlMouse(void);


	/*! Set Mouse-Button State.  Called from Outside_World.
	    This function is ignored if var.mouseIntegration=false;
	*/
	void SetMouseButtonState(bool lButton,bool rButton);


	/*! Set Game-Pad State.  Called from Outside_World.
	*/
	void SetGamePadState(int port,bool Abutton,bool Bbutton,bool left,bool right,bool up,bool down,bool run,bool pause);


	/*! Set Cyber-Stick State.  Called from Outside_World.
	*/
	void SetCyberStickState(int port,int x,int y,int z,int w,unsigned int trig);

	/*! Set CAPCOM CPSF State.  Called from Outside_World.
	*/
	void SetCAPCOMCPSFState(int port,bool left,bool right,bool up,bool down,bool A,bool B,bool X,bool Y,bool L,bool R, bool start,bool select);



	/*!
	*/
	void SetMouseMotion(int port,int dx,int dy);


	/*! Returns the mouse coordinate that TBIOS is thinking.
	    Returns false if it could not get the coordinate.
	*/
	bool GetMouseCoordinate(int &mx,int &my,unsigned int tbiosid) const;


	/*! After constructing FMTownsCommon class, call this function to specify where to look
	    for the ROM images.
	    It just redirect the call to mem.LoadROMImages() function.
	    Returns false if it could not read ROM images.
	*/
	bool LoadROMImages(const char dirName[]);

	/*! Once the ROMs are loaded, call PowerOn function to start the virtual machine.
	*/
	void PowerOn(void);

	/*! Resets the virtual machine.
	*/
	void Reset(void);

	/*! This function is called when:
	      Floppy-disk sector is read,
	      CD-ROM sector is read, and
	      Hard-disk sector is read.
	    Can take it as an indication that the boot-sequence has started.
	    If, temporary-boot device is selected by key-combination, this function can switch
	    keyboard back to the normal state.
	*/
	void NotifyDiskRead(void);


	/*! Process sound.
	*/
	void ProcessSound(Outside_World *outside_world);

	/*! This function will be called from the CPU when CALL FS:[0040H] where FS=0110H.
	    It is an opportunity for the virtual machine to identify the operating-system version.
	*/
	virtual void InterceptMouseBIOS(void);

	/*! This function will be called from the CPU in PushCallStack in response to INT 21H.
	*/
	virtual void InterceptINT21H(unsigned int AX,const std::string fName);

	/*! Run scheduled tasks.
	*/
	inline void RunScheduledTasks(void)
	{
		VMBase::RunScheduledTasks(state.townsTime);
	}

	/*! Set main RAM size.  Can change it only before starting the VM.
	    VM probably will crash if you do this after starting.
	*/
	void SetMainRAMSize(long long int size);

	/*!
	*/
	using VMBase::ScheduleDeviceCallBack;

	/*!
	*/
	using VMBase::UnscheduleDeviceCallBack;

private:
	void RunFastDevicePollingInternal(void);
public:
	/*! Check nextFastDevicePollingTime and call RunScheduledTask function of the devices in fastDevices.
	*/
	inline void RunFastDevicePolling(void)
	{
		if(state.nextFastDevicePollingTime<state.townsTime)
		{
			RunFastDevicePollingInternal();
		}
	}

	/*! Check Rendering Timer and render if townsTime catches up with the timer.
	    It will increment rendering timer.
	    Returns true if rendered.
	*/
	bool CheckRenderingTimer(class TownsRender &render,class Outside_World &world);

	/*! Force render regardless of the rendering timer.
	    It does not update the rendering timer.
	*/
	void ForceRender(class TownsRender &render,class Outside_World &world);


	/*! Render, but not transfer to the window.
	*/
	void RenderQuiet(class TownsRender &render,bool layer0,bool layer1);


	/*! Render one entire VRAM layer, but not transfer to the window.
	*/
	void RenderEntireVRAMLayerQuiet(class TownsRender &render,unsigned int layer);


	/*! Set up VRAM access.  Can have breakOnRead/breakOnWrite flag for debugging.
	    Zero performance penalty if both flags are false.
	*/
	void SetUpVRAMAccess(bool breakOnRead,bool breakOnWrite);


	bool FASTModeLamp(void) const;


	/*! I/O access for internal devices. */
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data);
	virtual void IOWriteDword(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);
	virtual unsigned int IOReadWord(unsigned int ioport);
	virtual unsigned int IOReadDword(unsigned int ioport);
	virtual void RunScheduledTask(unsigned long long int townsTime);
	void UpdateEleVol(int eleVol);
	void UpdateCDEleVol(Outside_World *outside_world);

	/*! Electric Volume.  Returns 31 when max level.  0 when -32dB. */
	bool GetEleVolCDLeftEN(void) const;
	bool GetEleVolCDRightEN(void) const;
	unsigned int GetEleVolCDLeft(void) const;
	unsigned int GetEleVolCDRight(void) const;

	/*! Adjust CPU frequency to simulate memory wait.
	*/
	void AdjustMachineSpeedForMemoryWait(void);

	// VM<->Host Interface
	void ProcessVMToHostCommand(unsigned int vmCmd,unsigned int paramLen,const unsigned char param[]);
	void VMHostFileTransfer(void);


	// Following functions are for testing purposes.
	void EnableDebugger(void);
	void DisableDebugger(void);

	std::vector <std::string> GetStackText(unsigned int numBytes) const;
	void PrintStack(unsigned int numBytes) const;
	void PrintDisassembly(void) const;
	std::vector <std::string> GetRealModeIntVectorsText(void) const;
	void DumpRealModeIntVectors(void) const;
	std::vector <std::string> GetCallStackText(void) const;
	std::vector <std::string> GetMouseStatusText(void) const;
	void PrintCallStack(void) const;
	void PrintPIC(void) const;
	void PrintDMAC(void) const;
	void PrintFDC(void) const;
	void PrintTimer(void) const;
	void PrintSound(void) const;

	void PrintStatus(void) const;

	static void MakeINTInfo(class i486SymbolTable &symTable);

	using VMBase::GetScheduledTasksText;


	bool SaveState(std::string fName) const;
	bool LoadState(std::string fName,class Outside_World &outsideWorld);

	virtual uint32_t SerializeVersion(void) const;
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const;
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version);



	// Toward semi-automated map generation.
	// Feedback from Mutsu development.
	class MemoryEvaluation : public LineParser
	{
	private:
		FMTownsCommon *townsPtr=nullptr;
	public:
		MemoryEvaluation(FMTownsCommon *townsPtr);

		bool ready=false;

		bool Decode(std::string str);

		std::string MatchCustomKeyword(std::string str) const override;
		bool IsCustomUnaryOperator(std::string str) const override;
		long long int EvaluateCustomUnaryOperator(const Term *t,long long int operand) const override;

		// Eg.  Write formula like the following.
		//   BYTE:$3600A
		//   (WORD:0x3600A-0x4E00)/0x50
		//   (WORD:0x600A-0x4E00)%0x50
		// Addresses are physical addresses.
		long long int EvaluateRawNumber(const std::string &str) const override;

		unsigned int EvaluateMemoryReference(unsigned int addr,unsigned int nBytes) const;
	};
	MemoryEvaluation mapXY[2];
	bool autoQSS=false;
	int lastAutoQSSXY[2]={0,0};
	int autoQSSThresholdXY[2]={0,0};
	uint64_t lastAutoQSSCheckTime=0;

	void EnableAutoQSS(unsigned int autoQSSThresholdX,unsigned int autoQSSThresholdY);
	void DisableAutoQSS(void);
	// Returns true if calculated map-coordinate moved more than the specified threshold in at least one axis.
	// If it returns true, x and y are calculated map-coordinate.
	bool CheckAutoQSS(int &x,int &y);

	bool GetMapXY(int &x,int &y) const;

	// For the time being, it is easier for me to encode map XY of Daikoukaijidai2 in this function.
	// I need to think about two-mode games.
	bool GetApplicationSpecificMapXY(int &x,int &y) const;

	void ApplicationSpecificScreenshotOverride(class TownsRender &render,class TownsCRTC::AnalogPalette &palette) const;

	// Dungeon Master Keyboard Shortcut
	// Yoffset is needed because click coordinate is shifted about 8 pixels down in Japanese mode.
	void Dunmas_Spell_Char(int charNum,int Yoffset);
	void Dunmas_Spell_Level(int level);
	void Dunmas_Spell_Light(int Yoffset);
	void Dunmas_Spell_Torch(int Yoffset);
	void Dunmas_Spell_SeeThrough(int Yoffset);
	void Dunmas_Spell_MakeLifePotion(int Yoffset);
	void Dunmas_Spell_MakeDetoxPotion(int Yoffset);
	void Dunmas_Spell_MakeStaminaPotion(int Yoffset);
	void Dunmas_Spell_Fireball(int Yoffset);
	void Dunmas_Spell_FourFireballs(int Yoffset);
	void Dunmas_Spell_LightningBolt(int Yoffset);
	void Dunmas_Spell_Defense(int Yoffset);
	void Dunmas_Spell_FireDefense(int Yoffset);
	void Dunmas_FrontRow_Attack(int level,bool continuous,int Yoffset);
	void Dunmas_All_Attack(int level,bool continuous,int Yoffset);

	void Daikoukai_ApplyPatchesCacheAddr(void);
	void Daikoukai_RightClick(void);
	void Daikoukai_YKey(void);
	void Daikoukai_NKey(void);
	void Daikoukai_Left(void);
	void Daikoukai_Right(void);
	void Daikoukai_CourseSet(void);
	void Daikoukai_F1(void);
	void Daikoukai_F2(void);
	void Daikoukai_F3(void);
	void Daikoukai_F4(void);
	void Daikoukai_F5(void);
	void Daikoukai_F6(void);
	void Daikoukai_F7(void);
	void Daikoukai_F8(void);
	void Daikoukai_DentakuButton(int dx,int dy);
	void Daikoukai_Dentaku_Plus1000(void);
	void Daikoukai_Dentaku_Plus100(void);
	void Daikoukai_Dentaku_Plus10(void);
	void Daikoukai_Dentaku_Plus1(void);
	void Daikoukai_Dentaku_Minus1000(void);
	void Daikoukai_Dentaku_Minus100(void);
	void Daikoukai_Dentaku_Minus10(void);
	void Daikoukai_Dentaku_Minus1(void);
	void Daikoukai_Dentaku_0(void);
	void Daikoukai_Dentaku_1(void);
	void Daikoukai_Dentaku_2(void);
	void Daikoukai_Dentaku_3(void);
	void Daikoukai_Dentaku_4(void);
	void Daikoukai_Dentaku_5(void);
	void Daikoukai_Dentaku_6(void);
	void Daikoukai_Dentaku_7(void);
	void Daikoukai_Dentaku_8(void);
	void Daikoukai_Dentaku_9(void);
	void Daikoukai_Dentaku_Max(void);
	void Daikoukai_Dentaku_Min(void);
	void Daikoukai_Dentaku_AC(void);
	void Daikoukai_Dentaku_RET(void);

	void AB2_Identify(void);  // Called on Mouse-BIOS initialization.
	void AB2_Throttle(unsigned int inputThr);

	void Daikoukai2_CaptureFlags(void);
	bool Daikoukai2_ControlMouseByArrowKeys(
		int &lb,int &mb,int &rb,int &mx,int &my,
		unsigned int leftKey,
		unsigned int upKey,
		unsigned int rightKey,
		unsigned int downKey);
	void Daikoukai2_TakeOverKeystroke(unsigned int code1,unsigned int code2);
	bool Daikoukaijidai2_MapXY(int &x,int &y) const;
	void Daikoukaijidai2_ScreenshotOverride(TownsRender &render,TownsCRTC::AnalogPalette &palette) const;
};

template <class CPUCLASS>
class FMTownsTemplate : public FMTownsCommon
{
private:
	CPUCLASS _cpu;

	// Will be removed when the code is stable.
	// In fact, this function probably would never be called anyway before baseClassReady
	// since virtual functions are not ready when vtable is not ready.
	inline void CheckBaseClassReady(void) const
	{
		if(true!=baseClassReady)
		{
			std::cout << "Someone tried to access CPU before it is ready." << std::endl;
			exit(1);
		}
	}

public:
	i486DXCommon &CPU(void) override final
	{
		CheckBaseClassReady();
		return _cpu;
	}
	const i486DXCommon &CPU(void) const override final
	{
		CheckBaseClassReady();
		return _cpu;
	}

	FMTownsTemplate() : _cpu(this)
	{
		auto &cpu=CPU();
		cpu.mouseBIOSInterceptorPtr=this;
		cpu.int21HInterceptorPtr=this;

		// This function caches the CPU pointer, and therefore cannot be
		// called in the constructor of the base class, when CPU is not ready yet.
		physMem.SetUpMemoryAccess(townsType,TownsTypeToCPUType(townsType));

		PowerOn();
	}

	/*! Run one instruction and returns the number of clocks passed. */
	inline unsigned int RunOneInstruction(void)
	{
		auto clocksPassed=_cpu.RunOneInstruction(mem,io);
		state.clockBalance+=clocksPassed*1000;

		// Since last update, clockBalance*1000/freq nano seconds have passed.
		// Eg.  66MHz ->  66 clocks passed means 1 micro second.
		//                clockBalance is 66000.
		//                clockBalance/freq=1000.  1000 nano seconds.
		auto FREQ=state.currentFreq;
		auto passedInNanoSec=(state.clockBalance/FREQ);
		state.townsTime+=passedInNanoSec;
		state.clockBalance%=FREQ;

		var.disassemblePointer.SEG=_cpu.state.CS().value;
		var.disassemblePointer.OFFSET=_cpu.state.EIP;

		return clocksPassed;
	}

};

class FMTownsWithMediumFidelityCPU : public FMTownsTemplate <i486DXDefaultFidelity>
{
};
class FMTownsWithHighFidelityCPU : public FMTownsTemplate <i486DXHighFidelity>
{
};

/* } */
#endif
