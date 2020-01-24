#ifndef TOWNS_IS_INCLUDED
#define TOWNS_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "device.h"
#include "i486.h"
#include "inout.h"
#include "ramrom.h"

#include "ioram.h"

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
		FREQUENCY_DEFAULT=66
	};

	virtual const char *DeviceName(void) const{return "FMTOWNS";}

	bool abort;
	std::string abortReason;

	class State
	{
	public:
		/*! Time passed since power on in micro seconds.
		    I think 64-bit is long enough.  So, I make it signed int.
		*/
		long long int townsTime;

		/*! After running one instruction, townsTime may not be exactly the same
		    as the real time (or whatever requested time.)
		    This variable remembers how many clocks townsTime is ahead of the real time.
		*/
		long long int clockBalance;

		/*! Clock frequency in MHz.  Default is FREQUENCY_DEFAULT.
		*/
		long long int freq;


		void PowerOn(void);
		void Reset(void);
	};

	State state;

	i486DX cpu;
	InOut io;
	IORam ioRAM;
	Memory mem;
	std::vector <Device *> allDevices;


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
};


/* } */
#endif
