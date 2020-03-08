#include <iostream>

#include "cpputil.h"
#include "towns.h"
#include "townsdef.h"
#include "townsmap.h"
#include "render.h"
#include "outside_world.h"



void FMTowns::ProcessVMToHostCommand(unsigned int vmCmd,unsigned int paramLen,const unsigned char param[])
{
	switch(vmCmd)
	{
	case TOWNS_VMIF_CMD_CAPTURE_CRTC:
		std::cout << "0x" << cpputil::Ubtox(param[0]) << ",0x" << cpputil::Ubtox(param[1]) << "," << std::endl;
		{
			const int nReg=sizeof(crtc.state.crtcReg)/sizeof(crtc.state.crtcReg[0]);
			for(int i=0; i<nReg; ++i)
			{
				std::cout << "0x" << cpputil::Ustox(crtc.state.crtcReg[i]) << ",";
				if(i%16==15 || nReg-1==i)
				{
					std::cout << std::endl;
				}
			}
		}
		break;
	case TOWNS_VMIF_CMD_PAUSE:
		debugger.ExternalBreak("Break Request from VM");
		break;
	case TOWNS_VMIF_CMD_EXIT_VM:
		var.powerOff=true;
		var.returnCode=param[0];
		break;
	}
}
