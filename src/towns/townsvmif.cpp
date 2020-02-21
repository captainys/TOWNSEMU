#include <iostream>

#include "cpputil.h"
#include "towns.h"
#include "townsdef.h"
#include "townsmap.h"
#include "render.h"
#include "outside_world.h"



void FMTowns::ProcessVMToHostCommand(unsigned int vmCmd,unsigned int paramLen,const unsigned char param[])
{
	std::cout << "Received VM-Host Command:" << cpputil::Ubtox(vmCmd) << std::endl;
}
