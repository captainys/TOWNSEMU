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
#include "ioram.h"
#include "crtc.h"
#include "fdc.h"
#include "rtc.h"
#include "keyboard.h"
#include "physmem.h"
#include "memaccess.h"
#include "sound.h"
#include "timer.h"



// Adding a device:
// (1) Make a class
// (2) Implement State, IO functions, Reset function.
// (3) Add as data member in FMTowns class.
// (4) In FMTowns::FMTowns() add to allDevices.
// (5) In FMTowns::FMTowns() add to io.
class FMTowns : public Device
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

	bool abort;
	std::string abortReason;

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


		void PowerOn(void);
		void Reset(void);
	};

	class Variable
	{
	public:
		unsigned int freeRunTimerShift;
		unsigned long long int nextRenderingTime;
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
	IORam ioRAM;
	TownsCRTC crtc;
	TownsFDC fdc;
	TownsKeyboard keyboard;
	TownsSound sound;
	TownsTimer timer;
	// Machine State <<

	unsigned int townsType;
	Variable var;
	InOut io;
	Memory mem;
	TownsMainRAMAccess mainRAMAccess;
	TownsMainRAMorSysROMAccess mainRAMorSysROMAccess;
	TownsMainRAMorFMRVRAMAccess mainRAMorFMRVRAMAccess;
	TownsDicROMandDicRAMAccess dicROMandDicRAMAccess;
	TownsFontROMAccess fontROMAccess;
	TownsVRAMAccess VRAMAccess;
	TownsSpriteRAMAccess spriteRAMAccess;
	TownsOsROMAccess osROMAccess;
	TownsWaveRAMAccess waveRAMAccess;
	TownsSysROMAccess sysROMAccess;

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

	/*! Run scheduled tasks.
	*/
	void RunScheduledTasks(void);

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


	/*! I/O access for internal devices. */
	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data);
	virtual void IOWriteDword(unsigned int ioport,unsigned int data);
	virtual unsigned int IOReadByte(unsigned int ioport);
	virtual unsigned int IOReadWord(unsigned int ioport);
	virtual unsigned int IOReadDword(unsigned int ioport);



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
};


/* } */
#endif
