/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
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
