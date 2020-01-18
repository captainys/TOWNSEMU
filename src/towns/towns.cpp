#include "towns.h"



FMTowns::FMTowns()
{
	abort=false;
	allDevices.push_back(&io);
	allDevices.push_back(&mem);
	Reset();
}

void FMTowns::LoadROMImages(const char dirName[])
{
	if(true!=mem.LoadROMImages(dirName))
	{
		abort=true;
		abortReason="Unable to load ROM images.";
	}
}

void FMTowns::Reset(void)
{
	cpu.Reset();
	for(auto devPtr : allDevices)
	{
		devPtr->Reset();
	}
}
