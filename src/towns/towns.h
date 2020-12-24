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

#include "eventlog.h"



// Adding a device:
// (1) Make a class
// (2) Implement State, IO functions, Reset function.
// (3) Add as data member in FMTowns class.
// (4) In FMTowns::FMTowns() add to allDevices.
// (5) In FMTowns::FMTowns() add to io.
class FMTowns : public VMBase, public Device, public i486DX::FMTownsMouseBiosInterceptor, public i486DX::INT21HInterceptor
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
		FAST_DEVICE_POLLING_INTERVAL=10000,  // Nano-seconds
		DEVICE_POLLING_INTERVAL=   8000000,  // 8ms

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

		/*! CPU time is calculated from the clock.
		*/
		long long int cpuTime;

		/*! Nanoseconds VM is lagging behind the real time.
		*/
		long long int timeDeficit=0;

		enum
		{
			CATCHUP_PER_INSTRUCTION=1024, // Nanoseconds catch-up per instruction.  Must be 2^n.
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
		long long int freq;


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
		unsigned int tbiosVersion;
		bool mouseBIOSActive=false;
		int mouseDisplayPage=0;
		unsigned int TBIOS_physicalAddr=0;
		unsigned int TBIOS_mouseInfoOffset=0;
		unsigned int MOS_work_linearAddr=0;
		unsigned int MOS_work_physicalAddr=0;
		unsigned int MOS_pulsePerPixelH=8,MOS_pulsePerPixelV=8;
		int mouseIntegrationSpeed=256;

		/*! Application-Specific Customization parameters.
		*/
		unsigned int appSpecificSetting=0;
		unsigned int appSpecific_MousePtrX=0;
		unsigned int appSpecific_MousePtrY=0;
		unsigned int appSpecific_WC2_EventQueueBaseAddr=0;  // DS:03CCH
		uint32_t appSpecific_WC_setSpeedPtr=0;
		uint32_t appSpecific_WC_maxSpeedPtr=0;
		uint32_t appSpecific_WC1_StickPosXPtr=0;
		uint32_t appSpecific_WC1_StickPosYPtr=0;
		bool appSpecific_HoldMouseIntegration=false;

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
		unsigned long long int nextRenderingTime;

		bool powerOff=false;
		int returnCode=0;

		bool debugBreakOnLButtonUp=false;
		bool debugBreakOnLButtonDown=false;

		/*! If this flag is true, fake damper-wire line is rendered.
		*/
		bool damperWireLine=false;

		bool scanLineEffectIn15KHz=false;

		/*! Enable/disable mouse integration.
		    To disable mouse integration, set this flag false, and call DontControlMouse().
		    To enable, set this flag true, then VM will respect the next ControlMouse and SetMouseButtonState functions and on.
		*/
		bool mouseIntegration=true;

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


		enum
		{
			TIME_ADJUSTMENT_LOG_LEN=64
		};
		unsigned int timeAdjustLogPtr=0;
		long long int timeAdjustLog[TIME_ADJUSTMENT_LOG_LEN];

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
		i486DX::FarPointer powerOffAt;


		int lastKnownMouseX=0,lastKnownMouseY=0;


		i486DX::FarPointer disassemblePointer;
		Variable();
		void Reset(void);
	};

	// Machine State >>
	State state;
	i486DX cpu;
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


	FMTowns();

	unsigned int MachineID(void) const;


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


	/*!
	*/
	void SetMouseMotion(int port,int dx,int dy);


	/*! Returns the mouse coordinate that TBIOS is thinking.
	    Returns false if it could not get the coordinate.
	*/
	bool GetMouseCoordinate(int &mx,int &my,unsigned int tbiosid) const;


	/*! After constructing FMTowns class, call this function to specify where to look
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


	/*! Run one instruction and returns the number of clocks passed. */
	unsigned int RunOneInstruction(void);

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


	/*! I/O access for internal devices. */
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data);
	virtual void IOWriteDword(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);
	virtual unsigned int IOReadWord(unsigned int ioport);
	virtual unsigned int IOReadDword(unsigned int ioport);
	virtual void RunScheduledTask(unsigned long long int townsTime);


	/*! Electric Volume.  Returns 31 when max level.  0 when -32dB. */
	unsigned int GetEleVolCDLeft(void) const;
	unsigned int GetEleVolCDRight(void) const;



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
};


/* } */
#endif
