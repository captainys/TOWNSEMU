#ifndef TOWNS_IS_INCLUDED
#define TOWNS_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "device.h"
#include "i486.h"
#include "inout.h"
#include "ramrom.h"

class FMTowns
{
public:
	// I'm talking about 66MHz to 120MHz ball park.
	// 1 clock is 1/66 to 1/120us.
	// Make it nano-seconds?
	// 1 micro second is 1/1M second.
	// 1 nano second is 1/1G second.

	bool abort;
	std::string abortReason;

	class State
	{
		long long int townsTime;

		/*! After running one instruction, townsTime may not be exactly the same
		    as the real time (or whatever requested time.)
		    This variable remembers how many clocks townsTime is ahead of the real time.
		*/
		long long int clockBalance;
	};

	i486DX cpu;
	InOut io;
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

	/*! After constructing FMTowns class, call this function to specify where to look
	    for the ROM images.
	    It just redirect the call to mem.LoadROMImages() function.
	    Returns false if it could not read ROM images.
	*/
	bool LoadROMImages(const char dirName[]);

	/*! Once the ROMs are loaded, call Reset function to start the virtual machine.
	*/
	void Reset(void);



	// Following functions are for testing purposes.
};


/* } */
#endif
