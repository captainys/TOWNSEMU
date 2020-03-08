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
#include "fdc.h"
#include "rtc.h"
#include "keyboard.h"
#include "physmem.h"
#include "memaccess.h"
#include "sound.h"
#include "gameport.h"
#include "timer.h"



// Adding a device:
// (1) Make a class
// (2) Implement State, IO functions, Reset function.
// (3) Add as data member in FMTowns class.
// (4) In FMTowns::FMTowns() add to allDevices.
// (5) In FMTowns::FMTowns() add to io.
class FMTowns : public Device, public i486DX::FMTownsMouseBiosInterceptor
{
public:
	// I'm talking about 66MHz to 120MHz ball park.
	// 1 clock is 1/66 to 1/120us.
	// Make it nano-seconds?
	// 1 micro second is 1/1M second.
	// 1 nano second is 1/1G second.

	enum
	{
		FREQUENCY_DEFAULT=66,              // MHz
		FAST_DEVICE_POLLING_INTERVAL=100,  // Nano-seconds
	};

	virtual const char *DeviceName(void) const{return "FMTOWNS";}

	using Device::abort;
	using Device::abortReason;

	class State
	{
	public:
		/*! Time passed since power on in nano seconds.
		    Initially I thought to make it micro seconds, but may be too coarse.
		    And, a 32-bit integer is good for 4,000,000,000 nano seconds = 4 seconds.
		    A 64-bit integer should be able to hold decades.
		    I think 64-bit is long enough.  So, I make it signed int.
		*/
		unsigned long long int townsTime;

		/*! 
		*/
		unsigned long long int nextFastDevicePollingTime;

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


		/*! TBIOS version, TBIOS physical address (it should be contiguous in physical memory),
		    Mouse work area pointer.  Captured upon MOS_start and MOS_end.
		*/
		unsigned int tbiosVersion;
		bool mouseBIOSActive=false;
		unsigned int TBIOS_physicalAddr,MOS_work_linearAddr,MOS_work_physicalAddr;


		void PowerOn(void);
		void Reset(void);
	};

	class Variable
	{
	public:
		unsigned int freeRunTimerShift;
		unsigned long long int nextRenderingTime;

		bool powerOff=false;
		int returnCode=0;

		enum
		{
			VM2HOST_PARAM_QUEUE_LENGTH=256,
		};
		unsigned int nVM2HostParam;
		unsigned char VM2HostParam[VM2HOST_PARAM_QUEUE_LENGTH];

		i486DX::FarPointer disassemblePointer;
		Variable();
		void Reset(void);
	};

	// Machine State >>
	State state;
	i486DX cpu;
	i486Debugger debugger;
	TownsPIC pic;
	TownsRTC rtc;
	TownsDMAC dmac;
	TownsCDROM cdrom;
	TownsPhysicalMemory physMem;
	TownsCRTC crtc;
	TownsFDC fdc;
	TownsKeyboard keyboard;
	TownsSound sound;
	TownsGamePort gameport;
	TownsTimer timer;
	// Machine State <<

	unsigned int townsType;
	Variable var;
	InOut io;
	Memory mem;

	/*! Pointers of all devices (except *this) must be stored in allDevices.
	*/
	std::vector <Device *> allDevices;

	/*! Pointers of the devices that requires very fast polling must be stored in this fastDevices.
	    Such as YM2612 and PCM (TownsSound class).
	*/
	std::vector <Device *> fastDevices;


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

	/*! Returns a string that corresponds to the TBIOS version for debugging.  (In tbiosid.cpp)
	*/
	const char *TBIOSIDENTtoString(unsigned int tbios) const;


	/*! Adjust mouse return based on the mouse coordinate that Towns is thinking and mouse coordinate of the host.
	    Returns true if it thinks it can control the mouse coordinate.  false otherwise.
	    (In tbiosid.cpp)
	*/
	bool ControlMouse(int hostMouseX,int houstMouseY,unsigned int tbiosid);


	/*! Returns the mouse coordinate that TBIOS is thinking.
	    Returns false if it could not get the coordinate.
	*/
	bool GetMouseCoordinate(int &mx,int &my,unsigned int tbiosid) const;


	bool CheckAbort(void) const;

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

	/*! Run one instruction and returns the number of clocks passed. */
	unsigned int RunOneInstruction(void);

	/*! This function will be called from the CPU when CALL FS:[0040H] where FS=0110H.
	    It is an opportunity for the virtual machine to identify the operating-system version.
	*/
	virtual void InterceptMouseBIOS(void);

	/*! Run scheduled tasks.
	*/
	void RunScheduledTasks(void);

	/*!
	*/
	void ScheduleDeviceCallBack(Device &dev,long long int timer);

	/*!
	*/
	void UnscheduleDeviceCallBack(Device &dev);

	/*! Check nextFastDevicePollingTime and call RunScheduledTask function of the devices in fastDevices.
	*/
	void RunFastDevicePolling(void);

	/*! Check Rendering Timer and render if townsTime catches up with the timer.
	    It will increment rendering timer.
	    Returns true if rendered.
	*/
	bool CheckRenderingTimer(class TownsRender &render,class Outside_World &world);

	/*! Force render regardless of the rendering timer.
	    It does not update the rendering timer.
	*/
	void ForceRender(class TownsRender &render,class Outside_World &world);


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



	// VM<->Host Interface
	void ProcessVMToHostCommand(unsigned int vmCmd,unsigned int paramLen,const unsigned char param[]);



	// Following functions are for testing purposes.
	unsigned int FetchByteCS_EIP(int offset) const;
	i486DX::Instruction FetchInstruction(void) const;
	std::vector <std::string> GetStackText(unsigned int numBytes) const;
	void PrintStack(unsigned int numBytes) const;
	void PrintDisassembly(void) const;
	std::vector <std::string> GetRealModeIntVectorsText(void) const;
	void DumpRealModeIntVectors(void) const;
	std::vector <std::string> GetCallStackText(void) const;
	void PrintCallStack(void) const;
	void PrintPIC(void) const;
	void PrintDMAC(void) const;
	void PrintFDC(void) const;
	void PrintTimer(void) const;
	void PrintSound(void) const;

	void PrintStatus(void) const;

	static void MakeINTInfo(class i486SymbolTable &symTable);
};


/* } */
#endif
